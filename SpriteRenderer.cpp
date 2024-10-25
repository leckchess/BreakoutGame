
#include "SpriteRenderer.h"
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace glm;

void SpriteRenderer::initRenderData()
{
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &quad_VAO);
	glGenBuffers(1, &quad_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(quad_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(GLuint textureId, vec2 position, vec2 size, float rotatation, vec3 color, GLuint modelMatLocation, GLuint spriteColorLocation)
{
	mat4 model = glm::mat4(1.0f);
	model = translate(model, glm::vec3(position, 0.0f));

	model = translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = rotate(model, glm::radians(rotatation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = scale(model, glm::vec3(size, 1.0f));

	glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(spriteColorLocation, color.r, color.g, color.b);

	glBindTexture(GL_TEXTURE_2D, textureId);

	glBindVertexArray(quad_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
