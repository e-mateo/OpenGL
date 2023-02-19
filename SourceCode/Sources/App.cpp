#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

#include <App.h>


int Core::App::Init(const AppInitializer init)
{
	width = init.width;
	height = init.height;

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, init.OpenGl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, init.OpenGl_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(init.width, init.height, init.name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, init.framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	GLint flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(init.glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Camera
	inputs = LowRenderer::CameraInputs();
	camera = LowRenderer::Camera((float)init.width, (float)init.height);
	ui = UI((float)init.width, (float)init.height);
	glfwGetCursorPos(window, &inputs.mouseX, &inputs.mouseY);

	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	threadpool.StartThread();
	skybox.LoadSkybox();
	LoadShaders();

	return 1;
}

void Core::App::LoadShaders()
{
	Resources::Shader* normal = resourcesManager.Create<Resources::Shader>("normalShader");
	std::filesystem::path normalV("Resources/Shaders/normalShader.vert");
	std::filesystem::path normalF("Resources/Shaders/normalShader.frag");
	ASSERT(normal->SetVertexShader(normalV));
	ASSERT(normal->SetFragmentShader(normalF));
	ASSERT(normal->Link());

	Resources::Shader* skybox = resourcesManager.Create<Resources::Shader>("skyboxShader");
	std::filesystem::path skyboxV("Resources/Shaders/skyboxShader.vert");
	std::filesystem::path skyboxF("Resources/Shaders/skyboxShader.frag");
	ASSERT(skybox->SetVertexShader(skyboxV));
	ASSERT(skybox->SetFragmentShader(skyboxF));
	ASSERT(skybox->Link());
}

void Core::App::Update()
{
	// Inputs camera
	glfwPollEvents();
	processInput(window);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(!ui.isInStart())
		skybox.Draw(camera,resourcesManager);


	if (ui.isInGame())
	{
		scene.player.Update(deltaTime, inputs, camera);
		scene.Update(resourcesManager);
	}

	SetupShader();
	Render();

	ui.DrawUI(window, scene, resourcesManager, threadpool, inputs);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void Core::App::SetupShader()
{
	shaderProgram = resourcesManager.Get<Resources::Shader>("normalShader")->shaderProgram;
	glUseProgram(shaderProgram);

	scene.LoadLights(shaderProgram);
	maths::Vec3 camPos = camera.GetPosition();
	glUniform3f(glGetUniformLocation(shaderProgram, "camPos"), camPos.x, camPos.y, camPos.z);
	glUniform1i(glGetUniformLocation(shaderProgram, "nbDirLights"), LowRenderer::Light::nbDirLight);
	glUniform1i(glGetUniformLocation(shaderProgram, "nbPointsLights"), LowRenderer::Light::nbPointLight);
	glUniform1i(glGetUniformLocation(shaderProgram, "nbSpotsLights"), LowRenderer::Light::nbSpotLight);
}

void Core::App::Render()
{
	// Draw Mesh
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUniform1i(glGetUniformLocation(shaderProgram, "drawLine"), false);
	for (int i = 0; i < scene.sceneGraph.root->children.size(); i++)
		DrawMesh(shaderProgram, scene.sceneGraph.root->children[i]);
}

void Core::App::DrawMesh(const int shaderProgram, Core::DataStructure::GameObject* node)
{
	//Update Matrix//
	node->UpdateLocalTransform();
	node->mesh->Update(node->worldTransform, camera);

	//DRAW OBJECTS//
	glBindTexture(GL_TEXTURE_2D, node->mesh->GetIdTexture());
	if (node->mesh->GetCurModel()->isInit)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &node->worldTransform.matrixArray[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, &node->mesh->MVPMatrix.matrixArray[0]);
		glBindVertexArray(node->mesh->GetCurModel()->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, 3 * (GLsizei)node->mesh->GetCurModel()->GetVertexBuffer().size());
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	//DRAW COLLIDERS//
	if (ui.DrawCollider())
	{
		maths::Mat4 MVPCollider = camera.GetVPMatrix() * node->collider.worldMatrix;
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &node->collider.worldMatrix.matrixArray[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, &MVPCollider.matrixArray[0]);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (node->collider.type == maths::TypeCollider::CUBE)
		{
			glBindVertexArray(node->collider.cubeCollider.model->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, 3 * (GLsizei)node->collider.cubeCollider.model->GetVertexBuffer().size());
		}
		else if (node->collider.type == maths::TypeCollider::SPHERE)
		{
			glBindVertexArray(node->collider.sphereCollider.model->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, 3 * (GLsizei)node->collider.sphereCollider.model->GetVertexBuffer().size());
		}
		else if (node->collider.type == maths::TypeCollider::CAPSULE)
		{
			glBindVertexArray(node->collider.capsuleCollider.model->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, 3 * (GLsizei)node->collider.capsuleCollider.model->GetVertexBuffer().size());
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}



	for (int i = 0; i < node->children.size(); i++)
		DrawMesh(shaderProgram, node->children[i]);
}

void Core::App::Unload()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	//Unload scene
	scene.UnloadScene(resourcesManager, threadpool);

	//delete ptr node
	scene.sceneGraph.Delete(scene.sceneGraph.root);

	//Delete Shader
	resourcesManager.Delete("normalShader");
	resourcesManager.Delete("skyboxShader");
}

void Core::App::processInput(GLFWwindow* window)
{
	deltaTime = clock() - oldTime;
	oldTime = clock();

	if (!ui.isInStart() && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && scene.chronoEnded)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		ui.SetInMenu(true);
		ui.SetInGame(false);
	}

	if (ui.isInGame())
	{
		//Keyboard Inputs
		inputs.jump = false;
		inputs.moveRight = 0.f;
		inputs.moveForward = 0.f;

		if (glfwGetKey(window, inputs.moveForwardInput[0]) == GLFW_PRESS) //Forward
			inputs.moveForward = 1.0f;
		else if (glfwGetKey(window, inputs.moveBackwardInput[0]) == GLFW_PRESS) //Backward
			inputs.moveForward = -1.0f;

		if (glfwGetKey(window, inputs.moveLeftInput[0]) == GLFW_PRESS) //Left
			inputs.moveRight = -1.0f;
		else if (glfwGetKey(window, inputs.moveRightInput[0]) == GLFW_PRESS) //Right
			inputs.moveRight = 1.0f;

		if (glfwGetKey(window, 32) == GLFW_PRESS) //Down
			inputs.jump = true;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			camera.moveCamera = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			camera.moveCamera = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		//Mouse Inputs
		double newMouseX, newMouseY;
		glfwGetCursorPos(window, &newMouseX, &newMouseY);
		inputs.mouseDeltaX = (float)(newMouseX - inputs.mouseX);
		inputs.mouseDeltaY = (float)(newMouseY - inputs.mouseY);
		inputs.mouseX = newMouseX;
		inputs.mouseY = newMouseY;

		//Update Camera
		Core::DataStructure::GameObject* player = scene.sceneGraph.Get("Player");
		camera.Update(deltaTime, camera.moveCamera, inputs, player->collider.type, player->position, player->scale);
	}
}