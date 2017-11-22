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

class shielder : public Enemy
{
public:
	shielder(FloatRect r, int _hp);
	RectangleShape shapeB;

	void update();

	Texture enemyB;
	Texture shieldedEnemyB;

private:


};

shielder::shielder(FloatRect r, int _hp)
{
	hp = _hp;
	shapeB.setSize(Vector2f(r.width, r.height));
	shapeB.setFillColor(Color::White);
	enemyB.loadFromFile("Breakout big enemy.png");
	shieldedEnemyB.loadFromFile("Breakout big enemy shielded.png");
}

void shielder::update()
{
	if (hp <= 3)
		shapeB.setTexture(&enemyB);
	else if (hp > 2)
		shapeB.setTexture(&shieldedEnemyB);
}