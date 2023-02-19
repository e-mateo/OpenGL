#include <iostream>

#include <Gameplay/Player.h>


Gameplay::Player::Player()
{
	state = ONGROUND;
	moveSpeed = 15.f;
	jumpForce = 1.43f;
	jumpPower = 5.75f;
	bumpReduce = 0.5f;
	gravity = 0.04321f;
	position = maths::Vec3(0, 0, 0);
	rotationEuler = maths::Vec3(0, 0, 0);
	velocity.y = -1.f;
}

void Gameplay::Player::Update(const double deltaTime, const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera)
{
	velocity.x = 0;
	velocity.z = 0;

	if (position.y <= -50)
		SpawnPlayer(maths::Vec3(50, 10, 350));

	Jump(inputs, camera);
	Move(deltaTime, inputs, camera);
	
	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;

	gameObject->collider.velocity = velocity;
	gameObject->Update(velocity, maths::Vec3(0, 0, 0), maths::Vec3(0, 0, 0));
}

void Gameplay::Player::Jump(const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera)
{
	if (inputs.jump && state == ONGROUND)
	{
		velocity.y = jumpForce;
		state = ONJUMP;
	}
}

void Gameplay::Player::SpawnPlayer(const maths::Vec3 spawnPos)
{
	position = spawnPos;
	rotationEuler = maths::Vec3(0, 0, 0);
	gameObject->position = position;

	gameObject->collider.sphereCollider.pos = spawnPos + maths::Vec3(0, 10, 0);
	gameObject->collider.cubeCollider.pos = position - maths::Vec3(10, 0, 10);
}

void Gameplay::Player::Move(const double deltaTime, const LowRenderer::CameraInputs& inputs, const LowRenderer::Camera camera)
{
	float deltatime = (float)deltaTime;
	if (state == ONGROUND)
	{
		velocity.y = 0;
	}
	else if (state != ONGROUND && state != ONJUMP)
	{
		velocity.y -= gravity * deltatime;
	}
	else if (state == ONJUMP)
	{
		if (velocity.y >= 0)
			velocity.y -= gravity * jumpPower * deltatime / inputs.reduce;
		else
			velocity.y -= (gravity * 1.8f) * jumpPower * deltatime / inputs.reduce;
	}

	velocity = velocity + (camera.dirX * moveSpeed * inputs.moveRight * deltatime / inputs.reduce);
	velocity = velocity + (camera.dirZ * moveSpeed * inputs.moveForward * deltatime / inputs.reduce);
}