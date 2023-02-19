#pragma once
#include <Gameplay/Player.h>
#include <Objects/SceneGraph.h>


using namespace Core::DataStructure;

namespace Physics
{
	class ColliderManager
	{
	public:
		void Clear();
		void SetPlayer(GameObject* p);
		void GetAllGameObject(GameObject* root);
		void GetAllChildrens(GameObject* parent);

		bool DoCollisions(Gameplay::Player &player);

	private:
		std::vector<GameObject*> allObject;
		GameObject* playerGO;
	};
}
