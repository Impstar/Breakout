#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <memory>
#include "Ball Class.h"
#include "Paddle Class.h"
#include "Brick Class.h"

using namespace sf;
using namespace std;

void update_state(float dt);
void render_frame();
bool rects_overlap(FloatRect r1, FloatRect r2);
Vector2f bounce(Ball pong, Paddle bump);

vector<unique_ptr<Enemy>> bricks;
RenderWindow window;
Font font;

void create_bricks();
void populate();

Ball gameBall;
Paddle player;

bool isReset = true;
bool firstServe = true;
float difficulty = 1.0;
int level = 1;
int lives = 3;

bool onCollisionExitPaddle = false;

int main()
{
	window.create(VideoMode(800, 600), "SFML Example");
	Clock clock;

	//ball parameters
	gameBall.ball.setFillColor(Color::Green);
	gameBall.ball.setOrigin(8, 8);
	gameBall.SetStartingPosition(window.getSize().x / 2, window.getSize().y - 40);
	gameBall.SetVel(250, 250);

	//player parameters
	player.SetSize(60, 15);
	player.SetStartingPosition(window.getSize().x/2 - player.bumper.getSize().x/2, window.getSize().y - 25);
	player.bumper.setFillColor(Color::White);
	player.bumper.setPosition(player.GetPosition());

	//building enemies
	create_bricks();
	populate();

	font.loadFromFile("arial.TTF");

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		float dt = clock.restart().asSeconds();

		update_state(dt);
		render_frame();
	}

	return 0;
}

void update_state(float dt)
{
	for (int i = 0; i < bricks.size(); i++)
	{
		bricks[i]->setDt(dt);
	}

	//ball collision with edges of screen
	if (gameBall.GetPosition().x > window.getSize().x && gameBall.GetVel().x > 0)
		gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
	if (gameBall.GetPosition().x < 0 && gameBall.GetVel().x < 0)
		gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
	if (gameBall.GetPosition().y < 0 && gameBall.GetVel().y < 0)
		gameBall.SetVel(gameBall.GetVel().x, -gameBall.GetVel().y);
	if (gameBall.GetPosition().y > window.getSize().y && gameBall.GetVel().y > 0 && lives > 0)
	{
		lives--;
		isReset = true;
		firstServe = true;
	}

	//player movement
	if (Keyboard::isKeyPressed(Keyboard::Left) && player.GetPosition().x > 0)
	{
		player.SetSpeed(-600);
		player.SetPositionX(dt);
	}
	else if (Keyboard::isKeyPressed(Keyboard::Right) && player.GetPosition().x < window.getSize().x - 60)
	{
		player.SetSpeed(600);
		player.SetPositionX(dt);
	}
	else
	{
		player.SetSpeed(0);
	}
	
	//releasing ball upon pressing space
	if (Keyboard::isKeyPressed(Keyboard::Space) && isReset)
	{
		isReset = false;
		gameBall.SetPosition();
		gameBall.SetDt(dt);
		if (firstServe)
		{
			if (player.GetSpeed() < 0)
			{
				gameBall.SetVel(-400 * difficulty, -400 * difficulty);
			}
			else if (player.GetSpeed() > 0)
			{
				gameBall.SetVel(400 * difficulty, -400 * difficulty);
			}
			else
			{
				gameBall.SetVel(0, -400 * difficulty);
			}
			firstServe = false;
		}
	}
	//collision with player
	if (rects_overlap(gameBall.getBoundary(), player.getBoundary()))
	{
		if (onCollisionExitPaddle)
		{
			onCollisionExitPaddle = false;

			Vector2f direction(bounce(gameBall, player).y, bounce(gameBall, player).x);
			float magnitude = sqrt(gameBall.GetVel().x * gameBall.GetVel().x + gameBall.GetVel().y * gameBall.GetVel().y);
			Vector2f newVelocity = direction * magnitude;
			gameBall.SetVel(newVelocity.x, newVelocity.y);
		}
	}
	else
	{
		onCollisionExitPaddle = true;
	}

	if (lives > 0)
	{
		if (isReset)
		{
			gameBall.SetStartingPosition(player.GetPosition().x + 30, player.GetPosition().y - 15);
		}
		if (!isReset)
		{
			gameBall.SetPosition();
			gameBall.SetDt(dt);
		}
		for (int i = 0; i < bricks.size(); i++)
		{
			bricks[i]->update();
		}
	}
}

void render_frame()
{
	window.clear();

	if (lives > 0)
	{
		window.draw(gameBall.ball);
		window.draw(player.bumper);
	}

	for (int i = 0; i < bricks.size(); ++i)
	{
		bricks[i]->draw(window);
	}
	
	Text livesLeft;
	livesLeft.setFont(font);
	livesLeft.setCharacterSize(50);
	livesLeft.setString(to_string(lives));
	livesLeft.setFillColor(Color::White);
	livesLeft.setPosition(50, window.getSize().y - 75);
	window.draw(livesLeft);
	window.display();
}

bool rects_overlap(FloatRect r1, FloatRect r2)
{
	return !(r1.left + r1.width <= r2.left ||
		r1.left >= r2.left + r2.width ||
		r1.top + r1.height <= r2.top ||
		r1.top >= r2.top + r2.height);
}

Vector2f bounce(Ball pong, Paddle bump)
{
	float min = bump.GetPosition().x - pong.GetRadius();
	float max = bump.GetPosition().x + bump.bumper.getSize().x + pong.GetRadius();
	float f = ((pong.GetPosition().x - min) / (max - min) * 2) - 1;
	float angle = f * (70.0 / 360.0 * 3.14159 - 3.14159 / 2);
	Vector2f bounceAngle(-cos(angle), -sin(angle));
	return bounceAngle;
}

void create_bricks()
{
	for (int i = 0; i < 5 * 3 + level - 1; ++i)
	{
		FloatRect r;
		r.width = 80;
		r.height = 80;
		int hp = 1;
		Enemy* b = new Enemy(r, hp);
		bricks.push_back(unique_ptr<Enemy>(b));
	}

}

void populate()
{
	int rows = 2 + level;
	int x = 0;
	
	while (x < bricks.size())
	{
		for (int i = 0; i < bricks.size() / 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				bricks[x]->shape.setPosition(Vector2f((j * 160), (i * 100)));
				bricks[x]->startPos = Vector2f((j * 160), (i * 100));
				bricks[x]->enemyPos = bricks[x]->startPos;
				x++;
			}
		}
	}
}