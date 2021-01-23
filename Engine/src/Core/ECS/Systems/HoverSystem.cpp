#include "pch.h"
#include "HoverSystem.h"

#include "Core/Math/AnkMath.h"

#include "Core/ECS/EntityComponentSystem.h"

#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Gravity.h"
#include "Core/ECS/Components/RigidBody.h"

void HoverSystem::init()
{
	this->timer = 0;
}

void HoverSystem::update(ECS& ecs, float dt)
{
	for (auto const& entity : this->entities)
	{
		auto const& transform	= ecs.getComponent<Transform>(entity);
		auto & rigidBody	= ecs.getComponent<RigidBody>(entity);
		
		this->timer += dt;

		float r = transform.Position.Length();
		float force = 2.f / (r * r);

		Vec3 direction = Vec3(0.f, 3.f, 0.f) - transform.Position;
		direction.Normalize();

		rigidBody.Velocity += direction * force;
	}

}
