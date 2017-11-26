#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <memory>
#include <algorithm>
#include <vector>
#include <ctime>
#include "Ball Class.h"
#include "Paddle Class.h"
#include "Brick Class.h"
#include "Shield Brick.h"
#include "projectile.h";

using namespace sf;
using namespace std;

void update_state(float dt);
int enemy_contact(FloatRect r1, FloatRect r2);
void render_frame();
bool rects_overlap(FloatRect r1, FloatRect r2);
Vector2f bounce(Ball pong, Paddle bump);
void level_up();

vector<unique_ptr<Enemy>> bricks;
vector<unique_ptr<shielder>> barriers;
Projectile attack;
RenderWindow window;
Font font;

void create_bricks();
void populate();

Ball gameBall;
Paddle player;
Texture background;
RectangleShape back;

Texture fighter_tex;
Texture shieldedFighter_tex;
Texture enemyB;
Texture shieldedEnemyB;

Sound music;
SoundBuffer musicBuf;
Sound thud;
SoundBuffer thudBuf;
Sound bumperHit;
SoundBuffer bumperHitBuf;
Sound lostLife;
SoundBuffer lostLifeBuf;
Sound loseGame;
SoundBuffer loseGameBuf;

Sound hitFighter;
SoundBuffer hitFighterBuf;
Sound hitShield;
SoundBuffer hitShieldBuf;
Sound destroyed;
SoundBuffer destroyedBuf;

bool isReset = true;
bool firstServe = true;
bool playLoseSound = false;
bool isShooting = false;
float difficulty = 1.0;
float ballSpeed = 300;
int level = 1;
int lives = 3;
int shipShooting;
float timeToShoot = 3.0;

bool onCollisionExitPaddle = false;

int main()
{
	window.create(VideoMode(800, 600), "SFML Example");

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
	background.loadFromFile("background.png");

	//load enemy textures
	fighter_tex.loadFromFile("Breakout enemy basic.png");
	shieldedFighter_tex.loadFromFile("Breakout enemy shielded.png");
	enemyB.loadFromFile("Breakout big enemy.png");
	shieldedEnemyB.loadFromFile("Breakout big enemy shielded.png");

	//load and position background
	back.setSize(Vector2f(window.getSize().x, window.getSize().y));
	back.setPosition(0, 0);
	back.setTexture(&background);

	//load sound effects
	musicBuf.loadFromFile("music.wav");
	music.setBuffer(musicBuf);
	thudBuf.loadFromFile("thud.wav");
	thud.setBuffer(thudBuf);
	bumperHitBuf.loadFromFile("bumperHit.wav");
	bumperHit.setBuffer(bumperHitBuf);
	lostLifeBuf.loadFromFile("lost life.wav");
	lostLife.setBuffer(lostLifeBuf);
	loseGameBuf.loadFromFile("lost game.wav");
	loseGame.setBuffer(loseGameBuf);

	//enemy sound effects
	hitFighterBuf.loadFromFile("fighterHit.wav");
	hitFighter.setBuffer(hitFighterBuf);
	hitShieldBuf.loadFromFile("shield hit.wav");
	hitShield.setBuffer(hitShieldBuf);
	destroyedBuf.loadFromFile("explosion.wav");
	destroyed.setBuffer(destroyedBuf);

	Clock clock;

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
	if (music.getStatus() != SoundSource::Playing)
		music.play();

	srand(time(0));

	for (int i = 0; i < bricks.size(); i++)
	{
		bricks[i]->setDt(dt);
	}

	//ball collision with edges of screen
	if (gameBall.GetPosition().x > window.getSize().x && gameBall.GetVel().x > 0)
	{
		gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
		thud.play();
	}
	if (gameBall.GetPosition().x < 0 && gameBall.GetVel().x < 0)
	{
		gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
		thud.play();
	}
	if (gameBall.GetPosition().y < 0 && gameBall.GetVel().y < 0)
	{
		gameBall.SetVel(gameBall.GetVel().x, -gameBall.GetVel().y);
		thud.play();
	}
	if (gameBall.GetPosition().y > window.getSize().y && gameBall.GetVel().y > 0 && lives > 0)
	{
		lives--;
		isReset = true;
		firstServe = true;
		lostLife.play();
		if (lives <= 0)
			playLoseSound = true;
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
		cout << level << endl;
		isReset = false;
		gameBall.SetPosition();
		gameBall.SetDt(dt);
		if (firstServe)
		{
			if (player.GetSpeed() < 0)
			{
				gameBall.SetVel(-ballSpeed * difficulty, -ballSpeed * difficulty);
			}
			else if (player.GetSpeed() > 0)
			{
				gameBall.SetVel(ballSpeed * difficulty, -ballSpeed * difficulty);
			}
			else
			{
				gameBall.SetVel(0, -ballSpeed * difficulty);
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
			bumperHit.play();
		}
	}
	else
	{
		onCollisionExitPaddle = true;
	}

	if (lives > 0) //while not gameover and not next round
	{
		//if the ball is reset
		if (isReset)
		{
			gameBall.SetStartingPosition(player.GetPosition().x + 30, player.GetPosition().y - 15);
		}
		if (!isReset)
		{
			gameBall.SetPosition();
			gameBall.SetDt(dt);
		}

		//updates all the enemies
		for (int i = 0; i < bricks.size(); i++)
		{
			bricks[i]->update();
		}

		for (int i = 0; i < barriers.size(); i++)
		{
			barriers[i]->update();
		}

		//ball collides with enemy
		for (int i = 0; i < bricks.size(); i++) //normal enemy
		{
			if (enemy_contact(gameBall.getBoundary(), bricks[i]->getBoundary()) == 1 || enemy_contact(gameBall.getBoundary(), bricks[i]->getBoundary()) == 2)
			{
				if (bricks[i]->onCollisionExit)
				{
					bricks[i]->onCollisionExit = false;
					bricks[i]->hit();
					if (bricks[i]->isDead())
					{
						bricks.erase(bricks.begin() + i);
					}
					gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
				}
			}
			else if (enemy_contact(gameBall.getBoundary(), bricks[i]->getBoundary()) >= 3)
			{
				if (bricks[i]->onCollisionExit)
				{
					bricks[i]->onCollisionExit = false;
					bricks[i]->hit();
					if (bricks[i]->isDead())
					{
						bricks.erase(bricks.begin() + i);
					}
					gameBall.SetVel(gameBall.GetVel().x, -gameBall.GetVel().y);
				}
			}
			else
			{
				bricks[i]->onCollisionExit = true;
			}

		}
		for (int i = 0; i < barriers.size(); i++) //shield enemy
		{
			if (enemy_contact(gameBall.getBoundary(), barriers[i]->getBoundary()) == 1 || enemy_contact(gameBall.getBoundary(), barriers[i]->getBoundary()) == 2)
			{
				if (barriers[i]->onCollisionExit)
				{
					barriers[i]->onCollisionExit = false;
					barriers[i]->hit();
					if (barriers[i]->isDead())
					{
						barriers.erase(barriers.begin() + i);
					}
					gameBall.SetVel(-gameBall.GetVel().x, gameBall.GetVel().y);
				}
			}
			else if (enemy_contact(gameBall.getBoundary(), barriers[i]->getBoundary()) >= 3)
			{
				if (barriers[i]->onCollisionExit)
				{
					barriers[i]->onCollisionExit = false;
					barriers[i]->hit();
					if (barriers[i]->isDead())
					{
						barriers.erase(barriers.begin() + i);
					}
					gameBall.SetVel(gameBall.GetVel().x, -gameBall.GetVel().y);
				}
			}
			else
			{
				barriers[i]->onCollisionExit = true;
			}
		}

		//rebuilds level to next
		if (bricks.size() <= 0)
		{
			level++;
			create_bricks();
			populate();
			lives = 3;
			isReset = true;
		}

		//a random enemy shoots periodically
		if (bricks.size() > 0)
			shipShooting = rand() % bricks.size();

		timeToShoot -= dt;
		if (timeToShoot <= 0)
		{
			timeToShoot = 3;
			isShooting = true;
			attack.InitializeProjectile(static_cast<float>(bricks[shipShooting]->shape.getPosition().x + 0.5 * bricks[shipShooting]->shape.getSize().x),
				bricks[shipShooting]->shape.getPosition().y + bricks[shipShooting]->shape.getSize().y, player.GetPosition());
			cout << "x: " << attack.GetPosition().x << ", y: " << attack.GetPosition().y << endl;
		}
		if (isShooting)
		{
			attack.SetDt(dt);
			attack.SetPosition();
		}
		if (rects_overlap(attack.getBoundary(), player.getBoundary()) && isShooting)
		{
			isShooting = false;
			lives--;
		}
		else if (attack.GetCurrentPositionY() >= window.getSize().y)
		{
			isShooting = false;
		}

	}
	if (lives <= 0)
	{
		if (playLoseSound)
			loseGame.play();
		playLoseSound = false;

	}

	if (Keyboard::isKeyPressed(Keyboard::LShift))
		level_up();

}

void render_frame()
{
	window.clear();
	window.draw(back);

	Text livesLeft;
	livesLeft.setFont(font);
	livesLeft.setCharacterSize(50);
	livesLeft.setString(to_string(lives));
	livesLeft.setFillColor(Color::Red);
	livesLeft.setPosition(50, window.getSize().y - 75);
	window.draw(livesLeft);

	if (lives > 0)
	{
		window.draw(gameBall.ball);
		window.draw(player.bumper);
		if (isShooting)
			window.draw(attack.shot);
	}

	for (int i = 0; i < bricks.size(); ++i)
	{
		bricks[i]->draw(window);
	}
	for (int i = 0; i < barriers.size(); i++)
	{
		barriers[i]->draw(window);
	}

	window.display();

}

bool rects_overlap(FloatRect r1, FloatRect r2)
{
	return !(r1.left + r1.width <= r2.left ||
		r1.left >= r2.left + r2.width ||
		r1.top + r1.height <= r2.top ||
		r1.top >= r2.top + r2.height);
}

int enemy_contact(FloatRect r1, FloatRect r2)
{
	if (rects_overlap(r1, r2)) //if there is a collision
	{
		float closestSide[4];
		float closest;
		Vector2f line;
		Vector2f r1Center = Vector2f(r1.left + 0.5 * r1.width, r1.top + 0.5 * r1.height); //center of colliding object

		//finds the center of each of the four sides of the collided with object
		Vector2f r2LCenter = Vector2f(r2.left, r2.top + 0.5 * r2.height);
		Vector2f r2RCenter = Vector2f(r2.left + r2.width, r2.top + 0.5 * r2.height);
		Vector2f r2TCenter = Vector2f(r2.left + 0.5 * r2.width, r2.top);
		Vector2f r2BCenter = Vector2f(r2.left + 0.5 * r2.width, r2.top + r2.height);

		//finds the shortest line between the center of the colliding objects and a side
		line = r2LCenter - r1Center;;
		closestSide[0] = sqrt(line.x * line.x + line.y * line.y);
		line = r2RCenter - r1Center;
		closestSide[1] = sqrt(line.x * line.x + line.y * line.y);
		line = r2TCenter - r1Center;
		closestSide[2] = sqrt(line.x * line.x + line.y * line.y);
		line = r2BCenter - r1Center;
		closestSide[3] = sqrt(line.x * line.x + line.y * line.y);
		closest = *min_element(closestSide, closestSide + 4);

		for (int i = 0; i < 4; i++)
		{
			if (closest == closestSide[i])
				return i + 1;
		}
	}
	else
		return 0;
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
	//build level 1
	if (level == 1)
	{
		for (int i = 0; i < 5 * 3; ++i)
		{
			FloatRect r;
			r.width = 80;
			r.height = 80;
			int hp = 2;
			Enemy* b = new Enemy(r, hp);
			bricks.push_back(unique_ptr<Enemy>(b));
		}
	}
	//build level 2
	if (level == 2)
	{
		for (int i = 0; i < 5 * 3; ++i)
		{
			FloatRect r;
			r.width = 80;
			r.height = 80;
			int hp;
			if (rand() % 2 == 0)
				hp = 2;
			else
				hp = 4;
			Enemy* b = new Enemy(r, hp);
			bricks.push_back(unique_ptr<Enemy>(b));
		}
	}
	//build level 3
	if (level == 3)
	{
		for (int i = 0; i < 5 * 2; ++i)
		{
			FloatRect r;
			r.width = 80;
			r.height = 80;
			int hp = 2;
			Enemy* b = new Enemy(r, hp);
			bricks.push_back(unique_ptr<Enemy>(b));
		}
		for (int i = 0; i < 5; i++)
		{
			FloatRect r;
			r.width = 160;
			r.height = 80;
			int hp = 7;
			shielder* b = new shielder(r, hp);
			barriers.push_back(unique_ptr<shielder>(b));
		}
	}

}

void populate()
{
	int x = 0;
	
	while (x < bricks.size())
	{
		for (int i = 0; i < bricks.size() / 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				bricks[x]->shape.setPosition(Vector2f((j * 160), (i * 100)));
				bricks[x]->startPos = bricks[x]->shape.getPosition();
				bricks[x]->enemyPos = bricks[x]->startPos;
				bricks[x]->setVel(Vector2f(100, 0));
				x++;
			}
		}
	}
	if (level == 3)
	{
		for (int i = 0; i < barriers.size(); i++)
		{
			barriers[i]->shapeB.setPosition(Vector2f(i * 160, 200));
			barriers[i]->startPos = barriers[i]->shapeB.getPosition();
			barriers[i]->enemyPos = barriers[i]->startPos;
		}
	}
}

void level_up()
{
	bricks.clear();
	level++;
	create_bricks();
	populate();
	isReset = true;
}