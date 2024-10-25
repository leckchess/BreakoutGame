#pragma once
#include <iostream>
#include <gl/glm/glm.hpp>
#include <gl/glew/glew.h>

class GameObject
{
public:
	// object state
	glm::vec2   Position, Size, Velocity;
	glm::vec3   Color;
	float       Rotation;
	bool        IsSolid;
	bool        Destroyed;

	// render state
	GLuint SpriteId;
	GLuint modelMat_Location, spriteColor_Location;

	// constructor(s)
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, GLuint inSpriteId, GLuint modelMatLocation, GLuint spriteColorLocation, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	// draw sprite
	virtual void Draw(class SpriteRenderer* Renderer);
};

