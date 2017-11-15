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
	Enemy(FloatRect r, int _hp);
	RectangleShape shape;

	void setDt(float dt);
	void draw(RenderWindow &window);
	void hit();
	bool isDead();
	void shoot();
	void move();

	void update();

	bool destroyAtt = false;
	Vector2f startPos;
	Vector2f enemyPos;

private:
	CircleShape attack;
	int hp;
	float deltaTime;
	bool isShooting = false;
	Vector2f attackPos;
	Vector2f attackVel;
	Vector2f enemyVel = Vector2f(100, 0);

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
}