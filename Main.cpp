// CppTest.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include "Main.h"
#include "Ball.h"
#include <cmath>

using namespace sf;

void changeVelocities(Ball* firstBall, Ball* secondBall, float distanceBetweenBalls) {
	float radiusSum = firstBall->getRadius() + secondBall->getRadius();
	Vector2<float> positionsDifference21 = secondBall->getPosition() - firstBall->getPosition();

	//change positions to prevent intersections in the next frame (check for another?)

	//calculate how far to push the ball
	float distanceToShift = distanceBetweenBalls - radiusSum;
	Vector2<float> unitNormal ((positionsDifference21) / (radiusSum));
	Vector2<float> vectorToShift(unitNormal*(-1.0f)*distanceToShift);
	std::cout << "Position of the second ball before shift: (" << secondBall->getPosition().x << ", " << secondBall->getPosition().y << ")" << std::endl;
	secondBall->setPosition(secondBall->getPosition() + vectorToShift);
	std::cout << "Position of the second ball after shift: (" << secondBall->getPosition().x << ", " << secondBall->getPosition().y << ")" << std::endl;
	//calculating and changing velocities

	Vector2<float> unitTangent(unitNormal.y*(-1), unitNormal.x);
	//dot product of the old velocity and normal vector represent magnitude of the old velocity in normal direction
	float initVelInNormal1 = (firstBall->getVelocity().x * unitNormal.x) + (firstBall->getVelocity().y*unitNormal.y);
	float initVelInNormal2 = (secondBall->getVelocity().x * unitNormal.x) + (secondBall->getVelocity().y*unitNormal.y);
	//same for tangent
	float initVelInTangent1 = (firstBall->getVelocity().x * unitTangent.x) + (firstBall->getVelocity().y*unitTangent.y);
	//float initVelInTangent2 = (initVelocity2.x * unitTangent.x) + (initVelocity2.y*unitTangent.y);

	//find magnitude of new velocity in normal direction, using formula v1 = (u1(m1-m2)+2*m2*u2)/(m1+m2)
	float newVelInNormal1 = (initVelInNormal1*(firstBall->getRadius() - secondBall->getRadius()) + 2 * secondBall->getRadius()*initVelInNormal2) / (radiusSum);
	//new velocity in tangent dir = old velocity in tang dir

	//multiply unit vectors by founded magnitudes to get vectors of new velocities in tang/normal coordinates
	//add tang/normal vectors to get velocitiy in global coordinates
	Vector2<float> firstBallNewVel ((unitNormal*newVelInNormal1) + (unitTangent*initVelInTangent1));
	
	//find velocity for a second ball using formula v2 = (m1*u1+m2*u2-m1*v1)/m2
	Vector2<float> secondBallNewVelocity((firstBall->getRadius()*firstBall->getVelocity() + secondBall->getRadius()*secondBall->getVelocity() - firstBall->getRadius()*firstBallNewVel) / secondBall->getRadius());
	
	firstBall->setVelocity(firstBallNewVel);
	secondBall->setVelocity(secondBallNewVelocity);
}

void checkObjectCollisions(Ball* firstBall, Ball* secondBall) {
	float distanceBetweenBalls = sqrt(pow(firstBall->getPosition().x - secondBall->getPosition().x, 2) + pow(firstBall->getPosition().y - secondBall->getPosition().y, 2));
	
	if (distanceBetweenBalls < (firstBall->getRadius() + secondBall->getRadius())) {
		std::cout << "Velocity1 before collision = (" << firstBall->getVelocity().x << ", " << firstBall->getVelocity().y << "), "
				  << "Velocity2 before collision = (" << secondBall->getVelocity().x << ", " << secondBall->getVelocity().y << ")" << std::endl;
		changeVelocities(firstBall, secondBall, distanceBetweenBalls);
		//Vector2<float> firstBallNewVelocity = calcVelocityAfterCollision(firstBall->getPosition(), firstBall->getVelocity(), firstBall->getRadius(), secondBall->getPosition(), secondBall->getVelocity(), secondBall->getRadius());
		//Vector2<float> secondBallNewVelocity = calcVelocityAfterCollision(secondBall->getPosition(), secondBall->getVelocity(), secondBall->getRadius(), firstBall->getPosition(), firstBall->getVelocity(), firstBall->getRadius());
		//firstBall->setVelocity(firstBallNewVelocity);
		//secondBall->setVelocity(secondBallNewVelocity);
		std::cout << "Velocity1 after collision = (" << firstBall->getVelocity().x << ", " << firstBall->getVelocity().y << "), "
			<< "Velocity2 after collision = (" << secondBall->getVelocity().x << ", " << secondBall->getVelocity().y << ")" << std::endl;
	}
}

int main()
{
	RenderWindow window(VideoMode(800, 600), "Ball");
	window.setFramerateLimit(60);

	Ball firstBall(Color::Black, 50, 0.8f, Vector2<float>(70,80), Vector2<float>(40,50), Vector2<float>(0,150));
	Ball secondBall(Color::Black, 25, 0.8f, Vector2<float>(300, 150), Vector2<float>(-40, -50), Vector2<float>(0,150));


	Clock frameTimer; //launch timer for each frame
	Time frameTime; //time elapsed from previous frame

	bool paused = false;

	while (window.isOpen())
	{
		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if ((event.type == Event::KeyReleased) && (event.key.code == Keyboard::Space)) {
				paused = !paused;
				frameTimer.restart();
			}
		}
			window.clear(Color::White);
			
			window.draw(firstBall.getShape());
			window.draw(secondBall.getShape());

			if (!paused) {
				frameTime = frameTimer.getElapsedTime();
				firstBall.move(frameTime.asSeconds());
				secondBall.move(frameTime.asSeconds());
				//std::cout << frameTime.asMilliseconds() << std::endl;
				frameTimer.restart();
				firstBall.checkBorderCollisions(window.getSize());
				secondBall.checkBorderCollisions(window.getSize());
				//collisionCheckTime = collisionTimer.getElapsedTime();
				//if (collisionCheckTime > collisionCheckPeriod) {
				checkObjectCollisions(&firstBall,&secondBall);
					//collisionTimer.restart();
				//}
			}

			window.display();
	}
	return 0;
}


//обработка коллизий в класс
//расталкивание обоих объектов
//прикрутить к коллизиям ивенты
//не только шары
//управление машь/клава
//сдвиг пространства