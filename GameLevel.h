#pragma once
#include <iostream>
#include<vector>
#include <gl/glew/glew.h>
#include <gl/glm/glm.hpp>
#include "GameObject.h"

class GameLevel
{

public:
	// level state
	std::vector<GameObject> Bricks;

	// constructor
	GameLevel() { }

	// loads level from file
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight,unsigned int solid_block, unsigned int block, unsigned int modelMatLocation, unsigned int spriteColorLocation);

	// render level
	void Draw(class SpriteRenderer* Renderer);

	// check if the level is completed (all non-solid tiles are destroyed)
	bool IsCompleted();

private:
	// initialize level from tile data
	void init(std::vector<std::vector<unsigned int>> tileData,
		unsigned int levelWidth, unsigned int levelHeightko, unsigned int solid_block, unsigned int block, unsigned int modelMatLocation, unsigned int spriteColorLocation);
};

