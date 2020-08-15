#include "pch.h"
#include "PhysicsSystem.h"

#include "Core/ECS/EntityComponentSystem.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/RigidBody.h"
#include "Core/ECS/Components/Gravity.h"

void PhysicsSystem::init()
{
}

void PhysicsSystem::update(ECS& ecs, float dt)
{
	for (auto const& entity : this->entities)
	{
		auto & transform	= ecs.getComponent<Transform>(entity);
		auto & rigidBody	= ecs.getComponent<RigidBody>(entity);
		auto const& gravity = ecs.getComponent<Gravity>(entity);

		transform.position += rigidBody.velocity * dt;

		rigidBody.velocity += gravity.force * dt;

		const float boundary = 10.0f;
		const float lossFactor = 0.6f;

		bool outX = abs(transform.position.x) > boundary;
		bool outY = transform.position.y < 0.f;
		bool outZ = abs(transform.position.z) > boundary;

		if (outX)
		{
			float direction = (transform.position.x / abs(transform.position.x));
			transform.position.x = boundary * direction;
			rigidBody.velocity = Vector3::Reflect(rigidBody.velocity, { direction, 0.f, 0.f }) * lossFactor;
		}
		if (outZ)
		{
			float direction = (transform.position.z / abs(transform.position.z));
			transform.position.z = boundary * direction;
			rigidBody.velocity = Vector3::Reflect(rigidBody.velocity, { 0.f, 0.f, direction }) * lossFactor;
		}

		if (outY)
			rigidBody.velocity = Vector3::Reflect(rigidBody.velocity, { 0.f, 1.f, 0.f }) * lossFactor;
	}
}
