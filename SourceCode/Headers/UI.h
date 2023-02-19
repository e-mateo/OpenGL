#pragma once
#include <Scene/Scene.h>
#include <Gameplay/Camera.h>
#include <Multithreading/Threadpool.h>
#include <Resources/ResourceManager.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

class UI
{
public:

	UI() {};
	UI(float _width, float _height);
	void DrawUI(GLFWwindow* window, Resources::Scene& scene, Resources::ResourceManager& resourcesManager, ThreadPool& threadpool, LowRenderer::CameraInputs& inputs);

	inline bool isInGame() {return inGame; }
	inline bool isInMenu() { return inMenu; }
	inline bool isInStart() { return inStart; }
	inline bool isInSetting() { return inSettings; }
	inline bool DrawCollider() { return drawCollider; }

	inline void SetInGame(bool b) { inGame = b;}
	inline void SetInMenu(bool b) { inMenu = b; }
	inline void SetInStart(bool b) { inStart = b; }
	inline void SetInSettings(bool b) { inSettings = b; }


private:

	void CloseStartMenu(GLFWwindow* window);
	void UIInGame();
	void UIStartMenu(GLFWwindow* window, Resources::Scene& scene, Resources::ResourceManager& resourcesManager, ThreadPool& threadpool);
	void UISettings(LowRenderer::CameraInputs& inputs);
	void UIMenu(GLFWwindow* window, Resources::Scene& scene, ThreadPool& threadpool, Resources::ResourceManager& resourcesManager);

	bool alwaysOpen;
	bool inGame;
	bool inMenu;
	bool inStart;
	bool inSettings;
	bool drawCollider;

	float width;
	float height;
};