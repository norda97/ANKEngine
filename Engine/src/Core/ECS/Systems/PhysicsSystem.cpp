#include "pch.h"
#include "PhysicsSystem.h"

#include "Core/ECS/EntityComponentSystem.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/RigidBody.h"
#include "Core/ECS/Components/Gravity.h"

void PhysicsSystem::Init()
{
}

void PhysicsSystem::update(ECS& ecs, float dt)
{
	for (auto const& entity : this->entities)
	{
		auto & transform	= ecs.GetComponent<Transform>(entity);
		auto & rigidBody	= ecs.GetComponent<RigidBody>(entity);
		auto const& gravity = ecs.GetComponent<Gravity>(entity);

		transform.Position += rigidBody.Velocity * dt;

		rigidBody.Velocity += gravity.Force * dt;

		const float boundary = 10.0f;
		const float lossFactor = 0.6f;

		bool outX = abs(transform.Position.x) > boundary;
		bool outY = transform.Position.y < 0.f;
		bool outZ = abs(transform.Position.z) > boundary;

		if (outX)
		{
			float direction = (transform.Position.x / abs(transform.Position.x));
			transform.Position.x = boundary * direction;
			rigidBody.Velocity = Vector3::Reflect(rigidBody.Velocity, { direction, 0.f, 0.f }) * lossFactor;
		}
		if (outZ)
		{
			float direction = (transform.Position.z / abs(transform.Position.z));
			transform.Position.z = boundary * direction;
			rigidBody.Velocity = Vector3::Reflect(rigidBody.Velocity, { 0.f, 0.f, direction }) * lossFactor;
		}

		if (outY)
		{
			rigidBody.Velocity = Vector3::Reflect(rigidBody.Velocity, { 0.f, 1.f, 0.f }) * lossFactor;
			transform.Position.y = 0.f;
		}
	}
}
