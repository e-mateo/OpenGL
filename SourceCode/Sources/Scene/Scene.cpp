#include <chrono>
#include <functional>

#include <Scene/Scene.h>
#include <Collisions/Shape.h>


using namespace maths;
using namespace Resources;
using namespace Core::DataStructure;

////////// CREATE SCENE //////////

Scene::Scene()
{
	chronoEnded = false;

	modelInit = 0;
	textureInit = 0;
	curIdTexture = 0;
}

void Scene::CreateModel(std::string name, std::string filePath, ResourceManager& resourcesManager, ThreadPool& threadpool)
{
	Model* newModel = resourcesManager.Create<Model>(name);
	models.push_back(newModel);

	threadpool.AddQueue(Task(newModel->loadFunc, filePath));
}

void Scene::CreateTexture(std::string name, std::string filePath, ResourceManager& resourcesManager, ThreadPool& threadpool)
{
	Texture* newText = resourcesManager.Create<Texture>(name);
	textures.push_back(newText);

	threadpool.AddQueue(Task(newText->loadFunc, filePath));
}

Mesh* Scene::CreateMesh(Model* model, Texture* texture, ResourceManager& resourcesManager)
{
	Mesh* newMesh = new Mesh(model, texture);
	meshes.push_back(newMesh);
	return newMesh;
}

void Scene::CreateSphereCollider(int radius, Vec3 pos, Model* modelCollider, GameObject* object)
{
	Shape::Sphere collider((float)radius, pos, modelCollider);
	object->collider.sphereCollider = collider;
	object->collider.type = TypeCollider::SPHERE;
}

void Scene::CreateCubeCollider(Vec3 pos, Vec3 size, Model* modelCollider, GameObject* object)
{
	Shape::Cube collider(pos, size, modelCollider);
	object->collider.cubeCollider = collider;
	object->collider.type = TypeCollider::CUBE;
}

GameObject* Scene::CreateGameObject(std::string name, Vec3 _position, Vec3 _rotation, Vec3 _scale, Mesh* mesh, GameObject* parent)
{
	GameObject* newGO = new GameObject();
	newGO->name = name;
	newGO->mesh = mesh;
	newGO->scale = _scale;
	newGO->position = _position;
	newGO->rotation = _rotation;
	newGO->UpdateLocalTransform();

	parent->SetChild(newGO);
	sceneGraph.nodeManager[name] = newGO;

	return newGO;
}

/////////// LOADS ///////////

void Scene::Binding(ResourceManager& resourcesManager)
{
	for (int i = 0; i < models.size(); ++i)
	{
		if (models[i]->isLoaded && !models[i]->isInit)
		{
			models[i]->BindBuffer();
			modelInit++;
		}
	}

	for (int i = 0; i < textures.size(); ++i)
	{
		if (textures[i]->isLoaded && !textures[i]->isInit)
		{
			textures[i]->GenAndBindTexture();
			textureInit++;
		}
	}

	if (!chronoEnded)
		if (modelInit == models.size() && textureInit == textures.size())
			chronoEnded = true;
}
	
void Scene::LoadScene(ResourceManager& resourcesManager, ThreadPool& threadpool)
{
	LoadTextures(resourcesManager, threadpool);
	LoadModels(resourcesManager, threadpool);
	LoadMeshes(resourcesManager);
}

/////////// SKYBOX SCENE ///////////

void Scene::LoadModels(ResourceManager &resourcesManager, ThreadPool& threadpool)
{
	Model* cube = resourcesManager.Create<Model>("Cube");
	cube->CreateCube();
	cube->BindBuffer();

	Model* classicSphere = resourcesManager.Create<Model>("Sphere");
	classicSphere->CreateSphere(20, 20, 1);
	classicSphere->BindBuffer();

	CreateModel("Spirit", "spirit.obj", resourcesManager, threadpool);
	CreateModel("Wizard", "wizard.obj", resourcesManager, threadpool);
	CreateModel("WaterStick", "waterstick.obj", resourcesManager, threadpool);

}

void Scene::LoadTextures(ResourceManager &resourcesManager, ThreadPool& threadpool)
{
	stbi_set_flip_vertically_on_load(true);

	CreateTexture("GroundText", "Resources/Textures/grass.png", resourcesManager, threadpool);
	CreateTexture("BrickText", "Resources/Textures/brick.png", resourcesManager, threadpool);
	CreateTexture("BoxText", "Resources/Textures/box.jpg", resourcesManager, threadpool);
	CreateTexture("BeigeText", "Resources/Textures/peach.jpg", resourcesManager, threadpool);
	CreateTexture("SpiritText", "Resources/Textures/spirit.png", resourcesManager, threadpool);
	CreateTexture("WizardText", "Resources/Textures/wizard.png", resourcesManager, threadpool);
	CreateTexture("WaterStickText", "Resources/Textures/waterstick.png", resourcesManager, threadpool);
}

void Scene::LoadMeshes(ResourceManager& resourcesManager)
{
	LoadLDMeshes(resourcesManager);

	Mesh* playerMesh = CreateMesh(resourcesManager.Get<Model>("Sphere"), resourcesManager.Get<Texture>("GroundText"), resourcesManager);

	Vec3 scale = Vec3(0, 0, 0);
	Vec3 position = Vec3(200, 0, 0);
	Vec3 rotation = Vec3(0, 0, 0);
	GameObject* playerGO = CreateGameObject("Player", position, rotation, scale, playerMesh, sceneGraph.root);
	CreateSphereCollider(25, position + Vec3(0, 10, 0), resourcesManager.Get<Model>("Sphere"), playerGO);
	colliderManager.SetPlayer(sceneGraph.Get("Player"));
	player.gameObject = sceneGraph.Get("Player");
	player.position = sceneGraph.Get("Player")->position;
}

void Scene::LoadLDMeshes(ResourceManager& resourcesManager)
{
	Mesh* cubeMesh = CreateMesh(resourcesManager.Get<Model>("Cube"), resourcesManager.Get<Texture>("GroundText"), resourcesManager);
	Mesh* sphereMesh = CreateMesh(resourcesManager.Get<Model>("Sphere"), resourcesManager.Get<Texture>("BeigeText"), resourcesManager);
	Mesh* brickMesh = CreateMesh(resourcesManager.Get<Model>("Cube"), resourcesManager.Get<Texture>("BrickText"), resourcesManager);
	Mesh* boxMesh = CreateMesh(resourcesManager.Get<Model>("Cube"), resourcesManager.Get<Texture>("BoxText"), resourcesManager);
	Mesh* cliffMesh = CreateMesh(resourcesManager.Get<Model>("Cliff"), resourcesManager.Get<Texture>("CliffText"), resourcesManager);
	Mesh* spiritMesh = CreateMesh(resourcesManager.Get<Model>("Spirit"), resourcesManager.Get<Texture>("SpiritText"), resourcesManager);
	Mesh* wizardMesh = CreateMesh(resourcesManager.Get<Model>("Wizard"), resourcesManager.Get<Texture>("WizardText"), resourcesManager);
	Mesh* waterstickMesh = CreateMesh(resourcesManager.Get<Model>("WaterStick"), resourcesManager.Get<Texture>("WaterStickText"), resourcesManager);


	GameObject* cube = CreateGameObject("Floor", Vec3(-100, -10, -100), Vec3(0, 0, 0), Vec3(700, 10, 700), cubeMesh, sceneGraph.root);
	CreateCubeCollider(cube->position, cube->scale, resourcesManager.Get<Model>("Cube"), cube);

	GameObject* sphere = CreateGameObject("Sphere", Vec3(250, 50, 470), Vec3(0, 0, 0), Vec3(40, 40, 40), sphereMesh, sceneGraph.root);
	CreateSphereCollider(40, sphere->position, resourcesManager.Get<Model>("Sphere"), sphere);

	GameObject* brick = CreateGameObject("Brick", Vec3(30, 0, 300), Vec3(0, 0, 0), Vec3(80, 80, 80), brickMesh, sceneGraph.root);
	CreateCubeCollider(brick->position, brick->scale, resourcesManager.Get<Model>("Cube"), brick);

	GameObject* box = CreateGameObject("Box", Vec3(-50, 0, 150), Vec3(0, 0, 0), Vec3(80, 80, 80), boxMesh, sceneGraph.root);
	CreateCubeCollider(box->position, box->scale, resourcesManager.Get<Model>("Cube"), box);

	GameObject* spirit = CreateGameObject("Spirit", Vec3(420, 40, 330), Vec3(0, 125, 0), Vec3(50, 50, 50), spiritMesh, sceneGraph.root);
	CreateSphereCollider(40, spirit->position - Vec3(0, 0, 0), resourcesManager.Get<Model>("Sphere"), spirit);

	GameObject* wizard = CreateGameObject("Spirit", Vec3(500, 55, 180), Vec3(0, 90, 0), Vec3(40, 40, 40), wizardMesh, sceneGraph.root);
	CreateCubeCollider(wizard->position - Vec3(5, 50, 20), Vec3(wizard->scale.x * 0.75, wizard->scale.y * 2.5, wizard->scale.z * 1), resourcesManager.Get<Model>("Cube"), wizard);

	GameObject* waterstick = CreateGameObject("Spirit", Vec3(250, 55, 180), Vec3(0, 90, 0), Vec3(60, 60, 60), waterstickMesh, sceneGraph.root);
	CreateCubeCollider(waterstick->position - Vec3(6, 55, 6), Vec3(waterstick->scale.x * 0.35, waterstick->scale.y * 2.3, waterstick->scale.z * 0.35), resourcesManager.Get<Model>("Cube"), waterstick);


}


////////// LIGHTS //////////

void Scene::LoadLights(const int shaderProgram)
{
	//Init numbers of lights
	LowRenderer::Light::nbDirLight = 0;
	LowRenderer::Light::nbPointLight = 0;
	LowRenderer::Light::nbSpotLight = 0;


	LowRenderer::Light globalLight(Vec3(0.3f, 0.3f, 0.3f), Vec3(0, 0, 0), Vec3(0, 0, 0));
	globalLight.CreateDirectionalLight(Vec3(0, 0, 0), shaderProgram);

	LowRenderer::Light spotLight(Vec3(0.f, 0.f, 0.f), Vec3(0.9f, 0.8f, 0.8f), Vec3(1, 1, 1));
	spotLight.CreateSpotLight(Vec3(250, 150, 0), Vec3(0.f, -0.75f, 1.0f), 60.f, 120.f, shaderProgram);
}

////////// UPDATE SCENE ////////// 

void Scene::Update(ResourceManager& resourcesManager)
{
	Binding(resourcesManager);


	GameObject* playerGO = sceneGraph.Get("Player");

	playerGO->collider.type = maths::TypeCollider::SPHERE;
	playerGO->collider.sphereCollider.pos = playerGO->position + Vec3(0, 10, 0);

	//Collisions//
	colliderManager.GetAllGameObject(sceneGraph.root);
	bool hasCollided = colliderManager.DoCollisions(player);
	player.position = playerGO->position;

	if (hasCollided)
		player.state = Gameplay::ONGROUND;
	else if (player.state != Gameplay::ONJUMP)
		player.state = Gameplay::ONJUMP;

	sceneGraph.UpdateMatrix();
}

////////// UNLOAD SCENE	 //////////
void Scene::UnloadScene(ResourceManager& resourcesManager, ThreadPool& threadpool)
{
	Unload(resourcesManager);

	for (int i = 0; i < meshes.size(); i++)
		delete meshes[i];

	//Nodes
	chronoEnded = false;

	modelInit = 0;
	textureInit = 0;

	meshes.clear();
	models.clear();
	textures.clear();
	colliderManager.Clear();
	sceneGraph.nodeManager.clear();

	threadpool.DeleteJobs();
}

void Scene::Unload(ResourceManager& resourcesManager)
{
	//Models
	resourcesManager.Delete("Cube");
	resourcesManager.Delete("Sphere");
	resourcesManager.Delete("Spirit");
	resourcesManager.Delete("Wizard");
	resourcesManager.Delete("WaterStick");

	//Textures
	resourcesManager.Delete("GroundText");
	resourcesManager.Delete("BrickText");
	resourcesManager.Delete("BoxText");
	resourcesManager.Delete("BeigeText");
	resourcesManager.Delete("SpiritText");
	resourcesManager.Delete("WizardText");
	resourcesManager.Delete("WaterStickText");
}