#pragma once

#include <iostream>
#include <gl/glew/glew.h>
#include <gl/glm/glm.hpp>

class SpriteRenderer
{
private:
	GLuint quad_VBO, quad_VAO;

public:
	void initRenderData();
	void DrawSprite(GLuint textureId, glm::vec2 position, glm::vec2 size, float rotatation, glm::vec3 color, GLuint modelMatLocation, GLuint spriteColorLocation);
};