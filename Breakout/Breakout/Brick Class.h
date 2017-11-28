#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

using namespace std;
using namespace sf;

extern Texture fighter_tex;
extern Texture shieldedFighter_tex;

extern Sound hitFighter;
extern SoundBuffer hitFighterBuf;
extern Sound hitShield;
extern SoundBuffer hitShieldBuf;
extern Sound destroyed;
extern SoundBuffer destroyedBuf;
extern Sound shotEffect;
extern SoundBuffer shotEffectBuf;

class Enemy
{
public:
	Enemy()
	{
		return;
	}
	Enemy(FloatRect r, int _hp);
	RectangleShape shape;

	void setDt(float dt);
	void draw(RenderWindow &window);
	void hit();
	bool isDead();
	void move();
	void setVel(Vector2f vel);
	FloatRect getBoundary();

	void update();

	bool onCollisionExit = true;
	Vector2f startPos;
	Vector2f enemyPos;

protected:
	int hp;
	float deltaTime;
	Vector2f enemyVel = Vector2f(0, 0);

};

Enemy::Enemy(FloatRect r, int _hp)
{
	hp = _hp;
	shape.setSize(Vector2f(r.width, r.height));
	shape.setFillColor(Color::White);
}

void Enemy::setDt(float dt)
{
	deltaTime = dt;
}

void Enemy::setVel(Vector2f vel)
{
	enemyVel = vel;
}

void Enemy::draw(RenderWindow &window)
{
	if (!isDead())
		window.draw(shape);
}

void Enemy::hit()
{
	hp--;
	if (hp == 1)
		hitFighter.play();
	else if (hp > 1)
		hitShield.play();
	else
		destroyed.play();
}

bool Enemy::isDead()
{
	if (hp <= 0)
		return true;
	else
		return false;
}

void Enemy::move()
{
	if (!isDead())
	{
		if (enemyVel.x > 0 && enemyPos.x >= startPos.x + 80)
		{
			enemyVel = -enemyVel;
		}
		else if (enemyVel.x < 0 && enemyPos.x <= startPos.x)
		{
			enemyVel = -enemyVel;
		}
		enemyPos += enemyVel * deltaTime;
		shape.setPosition(enemyPos);
	}
}

void Enemy::update()
{
	move();
	if (hp <= 2)
		shape.setTexture(&fighter_tex);
	else if (hp > 2)
		shape.setTexture(&shieldedFighter_tex);
}

FloatRect Enemy::getBoundary()
{
	FloatRect boundary;
	boundary.height = shape.getSize().y;
	boundary.width = shape.getSize().x;
	boundary.left = enemyPos.x;
	boundary.top = enemyPos.y;
	return boundary;
}