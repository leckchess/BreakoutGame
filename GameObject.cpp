#include "GameObject.h"
#include "SpriteRenderer.h"

GameObject::GameObject()
	: Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), SpriteId(0), modelMat_Location(0), spriteColor_Location(0), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, GLuint inSpriteId, GLuint modelMatLocation, GLuint spriteColorLocation, glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), SpriteId(inSpriteId), modelMat_Location(modelMatLocation), spriteColor_Location(spriteColorLocation), IsSolid(false), Destroyed(false)
{
}

void GameObject::Draw(SpriteRenderer* Renderer)
{
	Renderer->DrawSprite(SpriteId, Position, Size, Rotation, Color, modelMat_Location, spriteColor_Location);
}