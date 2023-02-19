#pragma once
#include <Objects/Mesh.h>
#include <Maths.h>
#include <Debug.h>
#include <Resources/Model.h>
#include <Scene/Light.h>
#include <Gameplay/Player.h>
#include <Resources/Shader.h>
#include <Resources/Texture.h>
#include <Objects/SceneGraph.h>
#include <Multithreading/ThreadPool.h>
#include <Resources/ResourceManager.h>
#include <Collisions/ColliderManager.h>


namespace Resources
{
	class Scene
	{
		int modelInit;
		int textureInit;
		int curIdTexture;

		std::vector<Mesh *> meshes;
		std::vector<Model *> models;
		std::vector<Texture *> textures;

		void Binding(ResourceManager& resourcesManager);

		// Create
		void CreateSphereCollider(int radius, maths::Vec3 pos, Model* modelCollider, GameObject* object);
		void CreateCubeCollider(maths::Vec3 pos, maths::Vec3 size, Model* modelCollider, GameObject* object);
		void CreateModel(std::string name, std::string filePath, ResourceManager& resourcesManager, ThreadPool& threadpool);
		void CreateTexture(std::string name, std::string filePath, ResourceManager& resourcesManager, ThreadPool& threadpool);
		Mesh* CreateMesh(Model* model, Texture* texture, ResourceManager& resourcesManager);
		Core::DataStructure::GameObject* CreateGameObject(std::string name, maths::Vec3 _position, maths::Vec3 _rotation, maths::Vec3 _scale, Mesh* mesh, Core::DataStructure::GameObject* parent);

		// Loads & Unload
		void LoadMeshes(ResourceManager& resourcesManager);
		void LoadLDMeshes(ResourceManager& resourcesManager);
		void LoadModels(ResourceManager& resourcesManager, ThreadPool& threadpool);
		void LoadTextures(ResourceManager& resourcesManager, ThreadPool& threadpool);
		void Unload(ResourceManager& resourcesManager);

	public:
		Scene();

		bool chronoEnded;

		Gameplay::Player player;
		Physics::ColliderManager colliderManager;
		Core::DataStructure::SceneGraph sceneGraph;

		void LoadScene(ResourceManager& resourcesManager, ThreadPool& threadpool);
		void Update(ResourceManager& resourcesManager);
		void UnloadScene(ResourceManager& resourcesManager, ThreadPool& threadpool);

		// Loads
		void LoadLights(const int shaderProgram);

		// Getter
		inline std::vector<Mesh*> GetMeshes() { return meshes; }
		inline std::vector<Model*> GetModels() { return models; }
		inline std::vector<Texture*> GetTextureList() { return textures; }
	};
}