#include <Windows.h>
#include<GL/glut.h>
#include<GL/GL.h>
#include<GL/freeglut.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <map>
//spaceship variables
#define SPACESHIP_MAX_SPEED 0.35//max speed obtainable by the spaceship
#define SPACESHIP_DECELERATION -0.001//fixed deceleration of the spaceship to make more fluid gameplay
#define SPACESHIP_ACCELERATION 0.006//the acceleration of the thruster
#define SPACESHIP_ROTATION 6 //the speed at which the spaceship can rotate
//bullet variables
#define BULLET_VELOCITY 0.6//the speed of the bullet
#define BULLET_MAX_DISTANCE 80//the max distance reachable by the bullet**
//asteroid varaibles
#define LARGE_ASTEROID_RADIUS 3// the size of the asteroids
#define ASTEROID_VELOCITY 0.1 // the speed at which the asteropids travel
#define INITIAL_ASTEROIDS 4// number of large asteroids that spawn
#define ASTEROID_ROTATION 0.5 //the rotation speed of the asteroid

std::map< unsigned char, bool > state;//used for the keyboard state

int game_state = 0;//this determines the state of the game, 1 the game is being played, 0 the game is not
int lives = 3; //amount of lives of the player

int myt = (int)time(0); //vairable fro random number generation
int start = (int)time(0);//the time the game is started
int shoottime = (int)time(0); //the time a bullet is fired
#define PI 3.14159265 //the constant pi

int score = 0;//the palyers score

float stars_x[40];// the star x values, we use 40 stars
float stars_y[40];// the star y values, we use 40 stars

//funtion to hand edge of screen interactions, when an object travels to the edge of the screen, they get warped to the other side
//function takes pointers to the vcariables and changes them
void edgeOfScreenHandler(float* x_pos, float* y_pos) { //takes the x and y position and check that they are within bounds of the map: -30:30
	if (*y_pos > 30)
		*y_pos -= 60; //if they are not in bounds we change the value by 60 so they reach the other side of the screen
	if (*y_pos < -30)
		*y_pos += 60;
	if (*x_pos > 30)
		*x_pos -= 60;
	if (*x_pos < -30)
		*x_pos += 60;
}

//class for the players spaceship
class Spaceship
{
public: //all variables are public for easier access, we know this is not secure but this is not the point of the project
	float x_position; //x position of the spaceship
	float y_position; //y position of the spaceship
	float angle; //the direction the spaeship is facing
	float movingAngle; // direction the spaceship is moving
	bool moving; // whether or not the spaecship is firing its thrusters actively
	float velocity; //the speed at which the spaceship is moving
	boolean invulnerable = false; //whether or not the spaceship is invlunerable

	//coonstructor for the spaceshp
	Spaceship(float x_pos = 0.0, float y_pos = 0.0) // constructor with the x and y position, default values is 0,0
	{
		//intialize all the variables previously defined
		x_position = x_pos;
		y_position = y_pos;
		angle = 0.0;
		movingAngle = 0.0;
		moving = false;
		velocity = 0.0;
	}

	//this is used to calculate the spaeships position every game tick
	void travel() {
		if (velocity > 0) { //check that the spaceship is moving
			if (velocity > SPACESHIP_MAX_SPEED) { //reduces tghe spaceships speed if it is going too fast
				velocity = (float)SPACESHIP_MAX_SPEED;
			}
			x_position += (float)velocity * sin((180 + (double)movingAngle) * (PI / 180)); //the xposition is the sin function of 180 degress+ the direction 
			//the ship is moving, converted to radians and multiplied by velocity, without the 180 the direction is inversed
			y_position += (float)velocity * cos((movingAngle) * (PI / 180)); // similar to the x_position
			edgeOfScreenHandler(&x_position, &y_position); //we then check to see the spaceship is still on the screen and change the variables if it is not
			if (velocity > 0) {
				velocity += (float)SPACESHIP_DECELERATION; //we then decrease the spaceships speed
			}
			//glutPostRedisplay();
		}
	}

	//funtion to draw the spaceship
	void draw() {
		glColor3f(255, 255, 255); //white color
		glPushMatrix(); //push matrix to ensure the spaceship is an isolated entity
		glTranslatef(x_position, y_position, -5.0); //move the spaceship to its x and y position
		glRotatef(angle, 0.0, 0.0, 1.0); //rotate the spaceship
		glBegin(GL_POLYGON); //draw the polygon
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, -1.0, 0.0);
		glVertex3f(0.0, 2.0, 0.0);
		glVertex3f(-1.0, -1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		if (moving) { //if the thruster are active draw the read thrusters
			glColor3f(255.0, 0, 0.0);
			glVertex3f(-0.5, -0.5, 0.0);
			glVertex3f(0.0, -2.0, 0.0);
			glVertex3f(0.5, -0.5, 0.0);
			moving = 0;//set the thruster indicator to 0
		}
		glEnd();

		if (invulnerable) { //if the spaceship is invulnerable draw the blue shield
			glColor3f(0, 216, 230); 
			glBegin(GL_LINE_LOOP);
			glVertex3f(0.0, 0.0, 0.0); 
			glVertex3f(1.2, -1.2, 0.0);
			glVertex3f(0.0, 2.2, 0.0);
			glVertex3f(-1.2, -1.2, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glEnd();
		}
		glPopMatrix(); 
	}

	//when the thrusters are activated we call this function to calculate the moving angle and velocity
	void move() {
		moving = 1; //we set the thruster indicator to 1
		if (velocity == 0) { //we set initial values if the spaceship is stationary
			velocity = 0.02;
			movingAngle = angle;
		}
		float y = velocity * sin((movingAngle) * (PI / 180)) + SPACESHIP_ACCELERATION * sin((angle) * (PI / 180)); //calculation for the y component of the vector:
		//its a sum of 2  components, the initial y component that the spaceship is travelling in, summed with the users input to change the value
		//the sum of these 2 components results in the new y component for vector representing the spaceships movement
		float x = velocity * cos((movingAngle) * (PI / 180)) + SPACESHIP_ACCELERATION * cos((angle) * (PI / 180)); //similar conecpt as the x position
		movingAngle = atan2(y, x) * (180 / PI); //we use the x and y components to get the new angle that the spaceship is moving in, using arctangent
		velocity = sqrt((double)pow(x,2) + (double)pow( y,2)); //we get the new velocity using the principals of pythagoras h^2 = sqrt(x^2+y^2)
	}

	//function to rotate the ship right
	void rotateRight() {
		angle -= SPACESHIP_ROTATION;
	}

	//function to rotate the ship left
	void rotateLeft() {
		angle += SPACESHIP_ROTATION;
	}
};

Spaceship spaceship; //we create the spaceship
std::vector<Spaceship> livesSpaceship; //we create a vector of spaceship to represent the lives of the player

//a class to represent the bullet/missile of the spaceship
class Bullet
{
public:
	float x_position; //bullet x position
	float y_position; //bullet y position
	float angle; // angle the bnullet is going to
	int shoot;  //1 if the bullet is being fired
	int shootAnimation; //1 if the bullet needs to be animated
	int shootCounter; // counter to track bullet distance

	Bullet() {
		x_position = 0.0;
		y_position = 0.0;
		angle = 0.0;
		shoot = 0;
		shootAnimation = 0;
		shootCounter = 0;
	}

	void shooting() {
		if (shootAnimation == 0) { //checks if there is not a bullet already being fired
			shoot = 1; //sets the shoot value to 1 
			shoottime = time(0); //resets the bullet cooldown
		}
	}

	void fire() { //fires the bullet
		if (shoot == 1) { //checks if the bullet was fired
			angle = spaceship.angle; //sets the angle to that of the spaceship
			x_position = spaceship.x_position; //sets x and y postion to the spaceship
			y_position = spaceship.y_position;
			shootAnimation = 1; //we must animate the bullet
			shoot = 0; //the bullet is no longer being shot
		}
	}

	//animates the bullet accorss the screen
	void animateBullet() {
		if (shootAnimation) {
			x_position += BULLET_VELOCITY * sin((double)((180 + (double)angle) * (PI / 180))); //get the x and y position using trigonemetry
			y_position += BULLET_VELOCITY * cos((angle) * (PI / 180));
			edgeOfScreenHandler(&x_position, &y_position); //checks the bullet is not n the edge of the screen
			shootCounter += 1; //increase the counter of the bullet
			if (shootCounter == BULLET_MAX_DISTANCE) { //if the bulle has reached max distance we stop the animation and reset the counter
				shootCounter = 0;
				shootAnimation = 0;
			}
			this->draw();//we then draw the bullet
		}
	}
	void draw() { //just a function to draw the bullet
		glPushMatrix();
		glColor3f(0, 255, 0);
		glTranslatef(x_position, y_position, -5.0);
		glRotatef(angle, 0.0, 0.0, 1.0);

		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0.5, 0);
		glEnd();
		glPopMatrix();
	}
};

//define the class for the asteroid
class Asteroid
{
public:
	float x_position; //the asteroids x position
	float y_position;//the asteroids y position
	float angle;//the angle the asteroid is moving in
	float rotation; //the angle the asteroid is currently rotating in
	float velocity; //the speed of the asteroid
	float x_points[12]; //the x points of the asteroid to be drawn
	float y_points[12]; //the y points of the asteroid to be drawn
	int radius; // the radius of the asteroid to be generated
	boolean breakup; //whether or not the asteroid can break up

	Asteroid(int r = LARGE_ASTEROID_RADIUS, float v = ASTEROID_VELOCITY, float x_pos = (-30 + rand() % 60), float y_pos = (-30 + rand() % 60), bool b = true) {
		
		srand((unsigned)myt++);//reset the random number generator
		x_position = x_pos; //set the x and y position, default is a random value
		y_position = y_pos;
		angle = rand() % 360; //randomise the angle 
		velocity = v;
		rotation = 0.0;
		radius = r; //radius, default it the radius defined above
		breakup = b; //the breakup value, default is true 
		
		srand((unsigned)myt++); //reset the random number generator
		//the code below is to generate random points to draw the asteroids, the points range from between 1 radius and 2,6* radius, depending on the random number
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
		srand((unsigned)myt++); //reset the random number generator again
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

	//function to move the asteroid
	void move() { //eveything that is her has been covered before
		x_position += velocity * sin((double)((180 + (double)angle) * (PI / 180)));
		y_position += velocity * cos((angle) * (PI / 180));
		rotation += ASTEROID_ROTATION;
		edgeOfScreenHandler(&x_position, &y_position);
	}

	//draw the asteroid
	void draw() {
		//the only thing that is different here is the for loop for drawing points
		glColor3f(255, 255, 255);
		glPushMatrix();		
		glTranslatef(x_position, y_position, -5.0);
		glRotatef(rotation+=0.2, 0.0, 0.0, 1.0);		
		glBegin(GL_LINE_LOOP);
		for (size_t i = 0; i < 12; i++) {
			glVertex3f(x_points[i], y_points[i], 0.0);
		}
		glEnd();
		glPopMatrix();
	}
};

Bullet bullet; //create a bullet
std::vector<Asteroid> asteroids; //creata a vector of asteroids

//funtion to handle the collision with all the asteroids and the spaceship
void asteroidCollisionWithSpasehip() {
	if (difftime(time(0), start) > 3) { //checks if the time is apssed the 3 second invlunerability time for the spaceship
		spaceship.invulnerable = false; //changes invulnerability to false
		for (auto i = asteroids.begin(); i != asteroids.end(); ++i) { //iterate through all the asteroids
			//checks if the spaceshup is within 2 radius' of the asteroid
			if (!(
				spaceship.x_position + 2 * sin((180 + (double)spaceship.angle) * (PI / 180)) <= (double)i->x_position - (double)i->radius * 2 ||
				spaceship.x_position - 1 * sin((180 + (double)spaceship.angle) * (PI / 180)) >= (double)i->x_position + (double)i->radius * 2 ||
				spaceship.y_position + 1 * cos((spaceship.angle) * (PI / 180)) <= (double)i->y_position - (double)i->radius * 2 ||
				spaceship.y_position - 1 * cos((spaceship.angle) * (PI / 180)) >= (double)i->y_position + (double)i->radius * 2
				)) {

				game_state = 0; //if there is a collision stop the game
				lives--; //reduce the players lives
				spaceship = Spaceship(); //reset the spaceship
				if (lives > 0) {
					livesSpaceship.erase(livesSpaceship.begin() + lives); //remove a life from the lives vector
				}
			}
		}
	}
	else
	{
		spaceship.invulnerable = true;
	}
}

//function to handle collsision with the bullet and the asteroid
void bulletCollisionWithAsteroid() {
	if (bullet.shootAnimation != 0) {// checks that a bullet is being animated
		for (auto i = asteroids.begin(); i != asteroids.end(); ++i) { //iterates through each asteroid and checks if it collides with the asteroid
			if (!(
				bullet.x_position <= i->x_position - i->radius * 2 ||
				bullet.x_position >= i->x_position + i->radius * 2 ||
				bullet.y_position <= i->y_position - i->radius * 2 ||
				bullet.y_position >= i->y_position + i->radius * 2
				))
			{
				bullet.shootAnimation = 0; //stop the animation
				bullet.shootCounter = 0; //reset the bullet counter
				bool repeat = i->breakup; //stores the breakup value of the asteorid in a temp value
				asteroids.erase(i); //removes the asteroid that was hit
				if (repeat == true) { //if the asteroid breaks up
					score += 50; //increase the score by 50 and create 3 asteroids at the same x and y position of the original bullet
					asteroids.push_back(Asteroid(2, 0.15, bullet.x_position, bullet.y_position, false));
					asteroids.push_back(Asteroid(2, 0.15, bullet.x_position, bullet.y_position, false));
					asteroids.push_back(Asteroid(2, 0.15, bullet.x_position, bullet.y_position, false));
				}
				else { // if a small asteroid is destroyed increase score by 100
					score += 100;
				}


				if (asteroids.size() == 0) {//if there are no asteroids left
					for (size_t i = 0; i < INITIAL_ASTEROIDS; i++) { //respawn intital asteroids
						asteroids.push_back(Asteroid());
						start = time(0); //reset start time for invulnerability
					}
				}
				break;
			}
		}
	}
}

//funtion to draw text on the screen at a specific x and y value, default is top riht corner
void drawText(const char* message, float x = 30, float y = 30)
{
	glPushMatrix();//push the matrix
	glColor3f(255, 255, 255); //set color to white
	glRasterPos3f((GLfloat)x, (GLfloat)y, -5);//set the position of the text
	while (*message) { //write message using bitmap and stroke chars
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message);
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *message++);
	}
	glPopMatrix();
}

//funciton to handle better keyboard input
void keyboard() {
	if (state[27])// esc key
		glutLeaveMainLoop();//leave the game
	if (state['a']) // a key
		spaceship.rotateLeft(); //rotate left
	if (state['d'])// d key
		spaceship.rotateRight(); //rotate right
	if (state['w'])// w key
		spaceship.move(); //move the spaceship
	if (state[' ']) //space key
		if(difftime(time(0), shoottime) > 1){ //check the cooldown for the bullet has passed
			bullet.shooting();	//fire a bullet
		}
	if (state[13]) {// enter key pressed	
		if (game_state == 0) { //if the game state is disabled, we start the game and reset the time
			game_state++;
			start = time(0);
		}
		if(lives == 0) //if the player has ran out of lives and enter is pressed we exit the game
			glutLeaveMainLoop();
	}
}

//render function for drawing thing in glut
void render() {
	glLineWidth(2); //set the line width to 2
	keyboard(); //handle keyboard inputs
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the glut buffers
	glLoadIdentity(); //load the identity
	glColor3f(255, 255, 255); //set the color to white and draw all the sars
	glBegin(GL_POINTS);
	for (int i = 0; i < 40; i++)
	{
		glVertex3f(stars_x[i], stars_y[i], -5);
	}
	glEnd();
	//handle game logic
	if (game_state != 0) { //if the game is active
		spaceship.travel(); //move the spaceship
		bullet.fire(); //handle the bullet logic
		bullet.animateBullet(); //draw the bullet
		spaceship.draw(); //draw the sapaceship
		asteroidCollisionWithSpasehip(); //handle collisions
		bulletCollisionWithAsteroid();
		char scoreArr[10]; //define a temp string to store the score
		snprintf(scoreArr, 10, "%d", score);//copy the score tp the string
		drawText(scoreArr);//display the score
		for (auto i = livesSpaceship.begin(); i != livesSpaceship.end(); ++i) //draw the spaceship lives
			i->draw();
	}
	else if (game_state == 0 && lives == 0) { //if the user ran out of lives
		//displat the score and a messagge
		drawText("GAME OVER : ", -6, 0);
		char str[10];
		_itoa_s(score, str, 10);
		drawText(str, 6, 0);
	}
	else if (game_state == 0 && lives != 3) { //if the user still has lives but died display respawn
		drawText("RESPAWN", -3, 0);
	}
	else { //if the game hasnt started display the title screen with instructions
		drawText("A S T E R O I D S", -5, 0);
		drawText("INSTRUCTIONS: ", -30, -6);
		drawText("Destroy All the Asteroids!", -30, -8);
		drawText("You have a blue shield for 3 seconds when you respawn", -30, -10);
		drawText("W: Forward Thrust", -30, -13);
		drawText("A: Turn Left", -30, -15);
		drawText("D: Turn Reft", -30, -17);
		drawText("SPACE: Fire Laser", -30, -19);
		drawText("ENTER: Start/Respawn", -30, -21);
	}
	//always display the asteroids for aesthetics
	for (auto i = asteroids.begin(); i != asteroids.end(); ++i)
		i->draw();
	glFlush();
	glutSwapBuffers();
}


//the next 2 functions are used in combination eith the keybaord() function to provide more responsive input handling
//keybaord pressed funtion
void keyboard_down(unsigned char key, int x, int y) {
	state[key] = true;
}
//keyboard released funtion
void keyboard_up(unsigned char key, int x, int y) {
	state[key] = false;
}

int sec_counter = 0; //auxiliary variable to count to a second

//timer function
void timer(int) {
	glutPostRedisplay();//redraw the screen
	glutTimerFunc(1000 / 60, timer, 0);//60hz
	if (sec_counter == 60) {// if a second has passed
		score++; //increase the score by 1
		sec_counter = 0; //reset the second counter
	}
	if (game_state != 0)//if the game state is active then we increase the second counter, 
		//this is to prevent the score from increasing when the state = 0
		sec_counter++;
	for (auto i = asteroids.begin(); i != asteroids.end(); ++i)//move all the asteroids
		i->move();
}

//initialize the variables
void initialize() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
}

//resize the window
void resize(int width, int height) {
	if (height == 0)
		height = 1;
	if (width == 0)
		width = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-30.0, 30.0, -30.0, 30.0); //use a square grid of -30 to 30
	gluPerspective(30.0, (float)width / (float)height, 0.1, 5); //change the perspective
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//main funciton
void main(int argc, char** argv) {
	//create the initail asteroids
	for (size_t i = 0; i < INITIAL_ASTEROIDS; i++)	{
		asteroids.push_back(Asteroid());
	}
	//draw the initial livees
	for (int i = 0; i < lives; i++)	{
		livesSpaceship.push_back(Spaceship(-30 + (3 * i), 30));
	}
	//create new random number seed
	srand((unsigned)time(0));
	//randomise star x and y positions
	for (int i = 0; i < 40; i++)
	{
		stars_x[i] = -30 + rand() % 60;
	}
	for (int i = 0; i < 40; i++)
	{
		stars_y[i] = -30 + rand() % 60;
	}
	//initialise glut functions
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1080, 1080);//square 1080p window
	glutCreateWindow("Asteroids");// name the window
	initialize();
	glutDisplayFunc(render); //define render funciton
	glutReshapeFunc(resize); //define reize funciotn
	glutKeyboardFunc(keyboard_down); //define keyboard down funciton
	glutKeyboardUpFunc(keyboard_up); //define keyboard up funcitone
	glutTimerFunc(0, timer, 0); //define the timer funciton
	glutMainLoop(); 
}