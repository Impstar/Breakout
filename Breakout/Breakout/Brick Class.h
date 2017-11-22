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
	void shoot();
	void move();
	void setVel(Vector2f vel);
	FloatRect getBoundary();

	void update();

	bool destroyAtt = false;
	bool onCollisionExit = true;
	Vector2f startPos;
	Vector2f enemyPos;

protected:
	CircleShape attack;

	Texture fighter;
	Texture shieldedFighter;

	Sound hitFighter;
	SoundBuffer hitFighterBuf;
	Sound hitShield;
	SoundBuffer hitShieldBuf;
	Sound destroyed;
	SoundBuffer destroyedBuf;

	int hp;
	float deltaTime;
	bool isShooting = false;

	Vector2f attackPos;
	Vector2f attackVel;
	Vector2f enemyVel = Vector2f(0, 0);

};

Enemy::Enemy(FloatRect r, int _hp)
{
	hp = _hp;
	shape.setSize(Vector2f(r.width, r.height));
	shape.setFillColor(Color::White);

	fighter.loadFromFile("Breakout enemy basic.png");
	shieldedFighter.loadFromFile("Breakout enemy shielded.png");

	hitFighterBuf.loadFromFile("fighterHit.wav");
	hitFighter.setBuffer(hitFighterBuf);
	hitShieldBuf.loadFromFile("shield hit.wav");
	hitShield.setBuffer(hitShieldBuf);
	destroyedBuf.loadFromFile("explosion.wav");
	destroyed.setBuffer(destroyedBuf);

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
	if (destroyAtt)
		window.draw(attack);
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

void Enemy::shoot()
{
	if (isShooting)
	{
		destroyAtt = true;
		attack.setPosition(Vector2f(shape.getPosition()));
		isShooting = false;
	}
	attackVel = Vector2f(0, 300);
	attackPos += attackVel * deltaTime;
	attack.setPosition(attackPos);
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
	shoot();
	if (hp <= 2)
		shape.setTexture(&fighter);
	else if (hp > 2)
		shape.setTexture(&shieldedFighter);
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