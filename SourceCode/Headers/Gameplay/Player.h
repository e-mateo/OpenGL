#pragma once
#include <Gameplay/Camera.h>
#include <Objects/SceneGraph.h>

namespace Gameplay
{
	enum StatePlayer
	{
		ONGROUND,
		ONJUMP,
	};

	class Player
	{
	public:
		int state;
		maths::Vec3 position;
		maths::Vec3 velocity;
		maths::Vec3 rotationEuler;
		Core::DataStructure::GameObject* gameObject;

	public:
		Player();
		void Update(const double deltaTime, const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera);
		void SpawnPlayer(const maths::Vec3 spawnPos);

	private:
		void Jump(const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera);
		void Move(const double deltaTime, const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera);

	private:
		bool isGrounded;

		float gravity;
		float moveSpeed;
		float jumpForce;
		float jumpPower;
		float bumpReduce;
	};
}