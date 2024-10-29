#include "GameLevel.h"
#include <fstream>
#include <sstream>
#include "SpriteRenderer.h"

using namespace std;

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight, unsigned int solid_block, unsigned int block, unsigned int modelMatLocation, unsigned int spriteColorLocation)
{
	// clear old data
	Bricks.clear();

	// load from file
	unsigned int tileCode;
	GameLevel level;
	string line;
	ifstream fstream(file);
	vector<vector<unsigned int>> tileData;

	if (!fstream.is_open())
	{
		cerr << "\nOpenFileError File:" << file;
		return;
	}

	cerr << "\nLevel File Loaded:" << file;

	while (getline(fstream, line)) // read each line from level file
	{
		istringstream sstream(line);
		vector<unsigned int> row;
		while (sstream >> tileCode) // read each word separated by spaces
			row.push_back(tileCode);
		tileData.push_back(row);
	}
	if (tileData.size() > 0)
		init(tileData, levelWidth, levelHeight, solid_block, block, modelMatLocation, spriteColorLocation);
}

void GameLevel::Draw(SpriteRenderer* Renderer)
{
	for (GameObject& tile : Bricks)
		if (!tile.Destroyed)
			tile.Draw(Renderer);
}

bool GameLevel::IsCompleted()
{
	for (GameObject& tile : Bricks)
		if (!tile.IsSolid && !tile.Destroyed)
			return false;
	return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight, unsigned int solid_block, unsigned int block, unsigned int modelMatLocation, unsigned int spriteColorLocation)
{
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unit_width = levelWidth / static_cast<float>(width);
	float unit_height = levelHeight / height;
	// initialize level tiles based on tileData		
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			// check block type from level data (2D level array)
			if (tileData[y][x] == 1) // solid
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size,
					solid_block, modelMatLocation, spriteColorLocation,
					glm::vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				Bricks.push_back(obj);
			}
			else if (tileData[y][x] > 1)
			{
				glm::vec3 color = glm::vec3(1.0f); // original: white
				if (tileData[y][x] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[y][x] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[y][x] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[y][x] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				Bricks.push_back(
					GameObject(pos, size, block, modelMatLocation, spriteColorLocation, color)
				);
			}
		}
	}
}
