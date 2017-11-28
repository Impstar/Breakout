#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include "Brick Class.h"

using namespace std;
using namespace sf;

extern Texture enemyB;
extern Texture shieldedEnemyB;

class shielder : public Enemy
{
public:
	shielder(FloatRect r, int _hp);
	RectangleShape shapeB;

	void update();
	void draw(RenderWindow &window);
	void hit();
	FloatRect getBoundary();

private:


};

shielder::shielder(FloatRect r, int _hp)
{
	hp = _hp;
	shapeB.setSize(Vector2f(r.width, r.height));
	shapeB.setFillColor(Color::White);

}

void shielder::update()
{
	if (hp <= 3)
		shapeB.setTexture(&enemyB);
	else if (hp > 2)
		shapeB.setTexture(&shieldedEnemyB);
}

void shielder::draw(RenderWindow &window)
{
	if (!isDead())
		window.draw(shapeB);
}

void shielder::hit()
{
	hp--;
	if (hp == 2 || hp == 1)
		hitFighter.play();
	else if (hp > 2)
		hitShield.play();
	else
		destroyed.play();
}

FloatRect shielder::getBoundary()
{
	FloatRect boundary;
	boundary.height = shapeB.getSize().y;
	boundary.width = shapeB.getSize().x;
	boundary.left = enemyPos.x;
	boundary.top = enemyPos.y;
	return boundary;
}