#include <iostream>
#include <gl/glew/glew.h>
#include <gl/glm/glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "SpriteRenderer.h"
#include "GameLevel.h"

using namespace std;
using namespace glm;

// Game State Enum
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

const GLint WIDTH = 1000, HEIGHT = 700;
GLuint programId;
GLuint modelMat_Location, projMat_Location;
GLuint spriteColor_Location;
GameState currentGameState;

// Game
GLuint bg_TextureID;
GLuint sloid_block_TextureID;
GLuint block_TextureID;
GLuint player_TextureID;

vector<GameLevel> Levels;
unsigned int currentLevel = 0;
SpriteRenderer* Renderer;
GameObject* Player;
vec2 player_size(100, 20);
const float player_velocity(500);
const float DELTA_TIME(1 / 60.0f);

#pragma region Shaders
GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
void CompileShader(const char* vertex_shader, const char* fragment_shader, GLuint& inProgramId)
{
	inProgramId = InitShader(vertex_shader, fragment_shader);
}
void CompileAndUseShader(const char* vertex_shader, const char* fragment_shader, GLuint& inProgramId)
{
	CompileShader(vertex_shader, fragment_shader, inProgramId);
	glUseProgram(inProgramId);
}
void AssignLocations(GLuint inProgramId, GLuint& Location, const GLchar* Name)
{
	Location = glGetUniformLocation(inProgramId, Name);
}
#pragma endregion

void LoadSprite(string spriteName, GLuint& textureId)
{
	// load Sprite using SFML
	sf::Image img;
	if (!img.loadFromFile(spriteName))
		cout << "Error Loading Image" << endl;
	else
	{
		cout << "Image Loaded: " << spriteName << endl;
		// read image using openGL
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error" << endl;
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// camera
	CompileAndUseShader("VS.glsl", "FS.glsl", programId);
	AssignLocations(programId, modelMat_Location, "model_matrex");
	AssignLocations(programId, projMat_Location, "proj_matrex");
	AssignLocations(programId, spriteColor_Location, "spriteColor");

	mat4 projection = ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(projMat_Location, 1, GL_FALSE, glm::value_ptr(projection));

	Renderer = new SpriteRenderer();
	Renderer->initRenderData();

	LoadSprite("bg.jpg", bg_TextureID);
	LoadSprite("block_solid.png", sloid_block_TextureID);
	LoadSprite("block.png", block_TextureID);
	LoadSprite("paddle.png", player_TextureID);

	GameLevel level;
	level.Load("1.level", WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
	Levels.push_back(level);

	level.Load("2.level", WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
	Levels.push_back(level);

	level.Load("3.level", WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
	Levels.push_back(level);

	level.Load("4.level", WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
	Levels.push_back(level);

	Player = new GameObject(vec2((float)WIDTH / 2 - player_size.x / 2, (float)HEIGHT - player_size.y), player_size, player_TextureID, modelMat_Location, spriteColor_Location);
	glClearColor(0, 0, 0, 1);

	return 0;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (currentGameState == GAME_ACTIVE)
	{
		// draw background
		Renderer->DrawSprite(bg_TextureID, glm::vec2(0.0f, 0.0f), glm::vec2(WIDTH, HEIGHT), 0.0f, vec3(1, 1, 1), modelMat_Location, spriteColor_Location);

		// draw level
		Levels[currentLevel].Draw(Renderer);

		Player->Draw(Renderer);
	}
}

void Update()
{
	if (currentGameState == GAME_ACTIVE)
	{
		float velocity = player_velocity * DELTA_TIME;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			if (Player->Position.x > 0.0f)
				Player->Position.x -= velocity;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			if (Player->Position.x < (float)WIDTH - Player->Size.x - 5)
				Player->Position.x += velocity;
		}
	}
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{

			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		Update();
		Render();

		window.display();
	}
	return 0;
}