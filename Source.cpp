#include <iostream>
#include <gl/glew/glew.h>
#include <gl/glm/glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "BallObject.h"

using namespace std;
using namespace glm;

// Game State Enum
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

const char* LevelsNames[]
{
"1.level",
"2.level",
"3.level",
"4.level"
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
GLuint ball_TextureID;

vector<GameLevel> Levels;
unsigned int currentLevel = 0;
SpriteRenderer* Renderer;
GameObject* Player;
const vec2 PLAYER_SIZE(100, 20);
const float PLAYER_VELOCITY(50);
const float DELTA_TIME(1 / 60.0f);

// ball
BallObject* Ball;
const vec2 INITIAL_BALL_VELOCITY(5, -10.0f);
const float BALL_RADIUS = 12.5f;

// collision
typedef tuple<bool, Direction, vec2> Collision;

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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
}

//float clamp(float value, float min, float max)
//{
//	return std::max(min, std::min(max, value));
//}

Direction VectorDirection(vec2 target)
{
	vec2 compass[] = {
		vec2(0.0f, 1.0f),	// up
		vec2(1.0f, 0.0f),	// right
		vec2(0.0f, -1.0f),	// down
		vec2(-1.0f, 0.0f)	// left
	};

	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = dot(normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision CheckCollision(BallObject& ball, GameObject& box) // AABB - AABB collision
{
	vec2 ball_center(ball.Position + ball.Radius);
	vec2 box_half_extents(box.Size.x / 2.0f, box.Size.y / 2.0f);
	vec2 box_center(box.Position.x + box_half_extents.x, box.Position.y + box_half_extents.y);

	vec2 difference = ball_center - box_center;
	vec2 clamped = clamp(difference, -box_half_extents, box_half_extents);

	vec2 closest = box_center + clamped;
	difference = closest - ball_center;

	if (length(difference) <= ball.Radius)
		return make_tuple(true, VectorDirection(difference), difference);
	else
		return make_tuple(false, UP, vec2(0.0f, 0.0f));
}

void DoCollisions()
{
	for (GameObject& box : Levels[currentLevel].Bricks)
	{
		if (box.Destroyed)
		{
			continue;
		}

		Collision collision = CheckCollision(*Ball, box);
		if (get<0>(collision))
		{
			if (!box.IsSolid)
			{
				box.Destroyed = true;
			}

			Direction dir = get<1>(collision);
			vec2 diff_vector = get<2>(collision);

			if (dir == LEFT || dir == RIGHT) // horizontal collision
			{
				Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity

				// relocate
				float penetration = Ball->Radius - abs(diff_vector.x);
				if (dir == LEFT)
					Ball->Position.x += penetration; // move ball to right
				else
					Ball->Position.x -= penetration; // move ball to left
			}
			else // vertical collision
			{
				Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity

				// relocate
				float penetration = Ball->Radius - std::abs(diff_vector.y);
				if (dir == UP)
					Ball->Position.y -= penetration; // move ball back up
				else
					Ball->Position.y += penetration; // move ball back down
			}
		}
	}

	// player collision
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && get<0>(result))
	{
		// check where it hit the board, and change velocity based on where it hit the board
		float player_center = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - player_center;
		float percentage = distance / (Player->Size.x / 2.0f);

		// then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
		Ball->Velocity = normalize(Ball->Velocity) * length(oldVelocity);
	}
}

void ResetLevel()
{
	Levels[currentLevel].Load(LevelsNames[currentLevel], WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
}

void ResetPlayer()
{
	// reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(WIDTH / 2.0f - PLAYER_SIZE.x / 2.0f, HEIGHT - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
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
	LoadSprite("ball.png", ball_TextureID);

	GameLevel level;
	for (const char* name : LevelsNames)
	{
		level.Load(LevelsNames[currentLevel], WIDTH, HEIGHT / 2.0f, sloid_block_TextureID, block_TextureID, modelMat_Location, spriteColor_Location);
		Levels.push_back(level);
	}

	Player = new GameObject(vec2((float)WIDTH / 2 - PLAYER_SIZE.x / 2, (float)HEIGHT - PLAYER_SIZE.y), PLAYER_SIZE, player_TextureID, modelMat_Location, spriteColor_Location);

	vec2 ballPos = Player->Position + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ball_TextureID, modelMat_Location, spriteColor_Location);
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

		Ball->Draw(Renderer);
	}
}

void Update()
{
	if (currentGameState == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * DELTA_TIME;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			if (Player->Position.x > 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x -= velocity;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			if (Player->Position.x < (float)WIDTH - Player->Size.x - 5)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
				{
					Ball->Position.x += velocity;
				}
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			Ball->Stuck = false;
		}

		Ball->Move(DELTA_TIME, WIDTH);

		DoCollisions();

		if (Ball->Position.y >= HEIGHT) // did ball reach bottom edge?
		{
			ResetLevel();
			ResetPlayer();
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