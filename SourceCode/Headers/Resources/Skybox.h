#pragma once
#include <UI.h>
#include <Resources/Model.h>
#include <Gameplay/Camera.h>
#include <Resources/Shader.h>
#include <Objects/SceneGraph.h>
#include <Resources/ResourceManager.h>


class Skybox
{
public:
	Skybox();
	~Skybox();

	unsigned int cubemapSkyboxScene;
	unsigned int cubemapCartoonScene;

	Resources::Model* model;

	void LoadSkybox();
	void LoadCubeMap(std::vector<std::string> faces, unsigned int& cubeMap);
	void Draw(LowRenderer::Camera &camera, Resources::ResourceManager& resourcesManager);
};