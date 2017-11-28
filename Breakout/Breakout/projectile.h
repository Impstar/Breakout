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

extern Sound shotEffect;
extern SoundBuffer shotEffectBuf;

class Projectile
{
public:
	Projectile();
	void InitializeProjectile(float x, float y, Vector2f target);
	CircleShape shot;

	void SetBallPosition(float x, float y);
	float GetRadius();
	float GetCurrentPositionX();
	float GetCurrentPositionY();

	void SetDt(float dt);
	void SetPosition();
	Vector2f GetPosition();

	void SetVel(Vector2f target);
	Vector2f GetVel();

	FloatRect getBoundary();

private:
	float radius;
	float deltaTime;

	Vector2f pos;
	Vector2f vel;
	Vector2f targetPos;

};

Projectile::Projectile()
{
	radius = 6;
	shot.setRadius(radius);
	shot.setFillColor(Color::Red);
}

void Projectile::InitializeProjectile(float x, float y, Vector2f target)
{
	pos.x = x;
	pos.y = y;
	targetPos = target;
	shotEffect.play();
}

void Projectile::SetBallPosition(float x, float y)
{
	pos.x = x;
	pos.y = y;
	shot.setPosition(x, y);
}

float Projectile::GetRadius()
{
	return radius;
}

float Projectile::GetCurrentPositionX()
{
	return pos.x;
}

float Projectile::GetCurrentPositionY()
{
	return pos.y;
}

void Projectile::SetDt(float dt)
{
	deltaTime = dt;
}

void Projectile::SetPosition()
{	
	Vector2f v = targetPos - pos;
	float len = sqrtf(v.x*v.x + v.y*v.y);
	v /= len;

	pos += v * 300.0f * deltaTime;

	//pos += targetPos * deltaTime;
	shot.setPosition(pos);


}

Vector2f Projectile::GetPosition()
{
	return pos;
}

void Projectile::SetVel(Vector2f target)
{
	vel = target;
}

Vector2f Projectile::GetVel()
{
	return vel;
}

FloatRect Projectile::getBoundary()
{
	FloatRect boundary;
	boundary.height = 2 * radius;
	boundary.width = 2 * radius;
	boundary.left = pos.x - radius;
	boundary.top = pos.y - radius;
	return boundary;
}