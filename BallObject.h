#pragma once
#include "GameObject.h"

class BallObject : public GameObject
{

public:
	// ball state	
	float     Radius;
	bool      Stuck;


	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, GLuint spriteId, GLuint modelMatLocation, GLuint spriteColorLocation);

	glm::vec2 Move(float dt, unsigned int window_width);

	void Reset(glm::vec2 position, glm::vec2 velocity);
};

