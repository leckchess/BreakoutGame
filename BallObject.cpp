#include "BallObject.h"

BallObject::BallObject() : GameObject(), Radius(12.5f), Stuck(true)
{ }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, GLuint spriteId, GLuint modelMatLocation, GLuint spriteColorLocation)
	: GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), spriteId, modelMatLocation, spriteColorLocation, glm::vec3(1.0f), velocity), Radius(radius), Stuck(true)
{ }

glm::vec2 BallObject::Move(float dt, unsigned int window_width)
{
	if (Stuck) { return Position; }

	// move the ball
	Position += Velocity * dt;
	// check if outside window bounds; if so, reverse velocity and restore at correct position
	if (Position.x <= 0.0f)
	{
		Velocity.x = -Velocity.x;
		Position.x = 0.0f;
	}
	else if (Position.x + Size.x >= window_width)
	{
		Velocity.x = -Velocity.x;
		Position.x = window_width - Size.x;
	}
	if (Position.y <= 0.0f)
	{
		Velocity.y = -Velocity.y;
		Position.y = 0.0f;
	}

	return Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	Position = position;
	Velocity = velocity;
	Stuck = true;
}
