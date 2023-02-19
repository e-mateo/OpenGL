#include <UI.h>

UI::UI(float _width, float _height)
{
	width = _width;
	height = _height;

	inGame = false;
	inMenu = false;
	inStart = true;
	alwaysOpen = true;
	inSettings = false;
	drawCollider = false;
}

void UI::DrawUI(GLFWwindow* window, Resources::Scene& scene, Resources::ResourceManager& resourcesManager, ThreadPool& threadpool, LowRenderer::CameraInputs& inputs)
{
	if (inStart)
		UIStartMenu(window, scene, resourcesManager, threadpool);

	if (inGame)
		UIInGame();

	if (inMenu)
	{
		UIMenu(window, scene, threadpool, resourcesManager);
		if (inSettings)
			UISettings(inputs);
	}
}

void UI::UIInGame()
{

}

void UI::UIStartMenu(GLFWwindow* window, Resources::Scene& scene, Resources::ResourceManager& resourcesManager, ThreadPool& threadpool)
{
	float winWidth = 320.f;
	float winHeight = 250.f;

	ImGui::SetNextWindowPos(ImVec2((float)(width / 2 - winWidth / 2), (float)(height / 2 - winHeight / 2)));
	ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));

	ImGui::Begin("Welcome to Mystery Machine Surfers", &alwaysOpen);

	ImGui::SetCursorPos(ImVec2(8.f, 27.f));
	if (ImGui::Button("Start", ImVec2(304.f, 100.f)))
	{
		CloseStartMenu(window);
		scene.LoadScene(resourcesManager, threadpool);
	}

	ImGui::SetCursorPos(ImVec2(8.f, 140.f));
	if (ImGui::Button("QUIT", ImVec2(304.f, 100.f)))
	{
		glfwSetWindowShouldClose(window, true);
	}

	ImGui::End();
}

void UI::CloseStartMenu(GLFWwindow* window)
{
	inGame = true;
	inMenu = false;
	inStart = false;
	inSettings = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void UI::UISettings(LowRenderer::CameraInputs& inputs)
{
	float winWidth = 320.f;
	float winHeight = 650.f;
	float winWidthM = 316.f;

	ImGui::SetNextWindowPos(ImVec2((float)(width / 2 - winWidthM / 2 + winWidthM + 8), (float)(height / 2 - winHeight / 2)));
	ImGui::SetNextWindowSize(ImVec2(winWidth, winHeight));

	ImGui::Begin("KEY BINDINGS", &alwaysOpen);

	ImGui::InputTextWithHint("Forward", "EnterText", inputs.moveForwardInput, IM_ARRAYSIZE(inputs.moveForwardInput));
	ImGui::InputTextWithHint("Backward", "EnterText", inputs.moveBackwardInput, IM_ARRAYSIZE(inputs.moveBackwardInput));
	ImGui::InputTextWithHint("Left", "EnterText", inputs.moveLeftInput, IM_ARRAYSIZE(inputs.moveLeftInput));
	ImGui::InputTextWithHint("Right", "EnterText", inputs.moveRightInput, IM_ARRAYSIZE(inputs.moveRightInput));
	ImGui::Checkbox("Draw Colliders ?", &drawCollider);

	//To upper case
	if (inputs.moveForwardInput[0] >= 97 && inputs.moveForwardInput[0] <= 122)
		inputs.moveForwardInput[0] -= 32;
	if (inputs.moveBackwardInput[0] >= 97 && inputs.moveBackwardInput[0] <= 122)
		inputs.moveBackwardInput[0] -= 32;
	if (inputs.moveLeftInput[0] >= 97 && inputs.moveLeftInput[0] <= 122)
		inputs.moveLeftInput[0] -= 32;
	if (inputs.moveRightInput[0] >= 97 && inputs.moveRightInput[0] <= 122)
		inputs.moveRightInput[0] -= 32;

	ImGui::End();
}

void UI::UIMenu(GLFWwindow* window, Resources::Scene& scene, ThreadPool& threadpool, Resources::ResourceManager& resourcesManager)
{
	float winHeight = 459.f;
	float winWidthM = 316.f;

	ImGui::SetNextWindowPos(ImVec2((float)(width / 2 - winWidthM / 2), (float)(height / 2 - winHeight / 2)));
	ImGui::SetNextWindowSize(ImVec2(winWidthM, winHeight));

	ImGui::Begin("PAUSE", &alwaysOpen);

	ImGui::SetCursorPos(ImVec2(8.f, 27.f));
	if (ImGui::Button("RESUME", ImVec2(300.f, 100.f)))
	{
		inGame = true;
		inMenu = false;
		inSettings = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	ImGui::SetCursorPos(ImVec2(8.f, 135.f));
	if (ImGui::Button("MAIN MENU", ImVec2(300.f, 100.f)))
	{
		threadpool.DeleteJobs();
		scene.sceneGraph.Delete(scene.sceneGraph.root);
		scene.sceneGraph.CreateNewRoot();
		scene.UnloadScene(resourcesManager, threadpool);

		inGame = false;
		inMenu = false;
		inStart = true;
		inSettings = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	ImGui::SetCursorPos(ImVec2(8.f, 243.f));
	if (ImGui::Button("SETTINGS", ImVec2(300.f, 100.f)))
	{
		if (!inSettings)
			inSettings = true;
		else
			inSettings = false;
	}

	ImGui::SetCursorPos(ImVec2(8.f, 351.f));
	if (ImGui::Button("QUIT", ImVec2(300.f, 100.f)))
		glfwSetWindowShouldClose(window, true);

	ImGui::End();
}