#pragma once
#include <rpcndr.h>
#include <stdlib.h>
#include "Asteroids.cpp"
#define LARGE_ASTEROID_RADIUS 3
#define ASTEROID_VELOCITY 0.1
#define INITIAL_ASTEROIDS 4
#define ASTEROID_ROTATION 0.5


class Asteroid
{
public:
	float x_position;
	float y_position;
	float angle;
	float rotation;
	float velocity;
	float x_points[12];
	float y_points[12];
	int radius;
	boolean breakup;
	//float y_points[8];

	Asteroid(int r = LARGE_ASTEROID_RADIUS, float v = ASTEROID_VELOCITY, float x_pos = (-30 + rand() % 60), float y_pos = (-30 + rand() % 60), bool b = true) {
		srand((unsigned)myt++);


		x_position = x_pos;
		y_position = y_pos;
		angle = rand() % 360;;
		velocity = v;
		rotation = 0.0;
		radius = r;
		breakup = b;



		srand((unsigned)myt++);

		x_points[0] = rand() % radius + radius * 1.3;
		x_points[1] = rand() % radius + radius;

		x_points[2] = 0;

		x_points[3] = -rand() % radius - radius;
		x_points[4] = -rand() % radius - radius * 1.3;

		x_points[5] = -rand() % radius - radius * 1.6;

		x_points[6] = -rand() % radius - radius * 1.3;
		x_points[7] = -rand() % radius - radius;

		x_points[8] = 0;

		x_points[9] = rand() % radius + radius;
		x_points[10] = rand() % radius + radius * 1.3;

		x_points[11] = rand() % radius + radius * 1.6;

		srand((unsigned)myt++);

		y_points[0] = rand() % radius + radius;
		y_points[1] = rand() % radius + radius * 1.3;

		y_points[2] = rand() % radius + radius * 1.6;

		y_points[3] = rand() % radius + radius * 1.3;
		y_points[4] = rand() % radius + radius;

		y_points[5] = 0;

		y_points[6] = -rand() % radius - radius;
		y_points[7] = -rand() % radius - radius * 1.3;
		y_points[8] = -rand() % radius - radius * 1.6;
		y_points[9] = -rand() % radius - radius * 1.3;
		y_points[10] = -rand() % radius - radius;
		y_points[11] = 0;
		srand((unsigned)myt++);

	}

	void move() {
		x_position += velocity * sin((180 + angle) * (PI / 180));
		y_position += velocity * cos((angle) * (PI / 180));
		rotation += ASTEROID_ROTATION;
		edgeOfScreenHandler(&x_position, &y_position);
	}

	void draw() {
		glColor3f(255, 255, 255);
		glPushMatrix();
		glTranslatef(x_position, y_position, -5.0);
		//glRotatef(rotation, 0.0, 0.0, 1.0);
		//glScalef(scale, scale, scale);

		glBegin(GL_LINE_LOOP);
		for (size_t i = 0; i < 12; i++)
		{
			glVertex3f(x_points[i], y_points[i], 0.0);
		}


		glEnd();
		/*glColor3f(255, 0, 0);
		glBegin(GL_QUADS);
		glVertex3f(7,0 , 0.0);
		glVertex3f(0,7 , 0.0);
		glVertex3f(-7,0 , 0.0);
		glVertex3f(0,-7 , 0.0);

		glEnd();*/

		glPopMatrix();
	}


private:

};
