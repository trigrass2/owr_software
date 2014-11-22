// catkin_make
// source devel/setup.bash
// rosrun owr_gui glutgui
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <GL/glut.h>
#include "comms.h"
#include "GpsGUI.h"

using namespace std;

#define PI 3.1415926535897932384626433832795

// default window size
#define WINDOW_W 1000
#define WINDOW_H 500

void init();
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);
void display();

// function to display some text
void drawText(char *text, int x, int y);
// function to insert a co-ordinate to the front of the path list
void GPSAddRandPos();
// function to print the path
void printGPSPath();

// default status values
float owr_battery = 5;
float owr_signal = 5;
float tiltX = 30; // degrees
float tiltY = 30; // degrees
double longitude = 0;
double latitude = 0;

// GPS related variables
ListNode path = NULL;
vector2D target;

// OpenGL control related variables
unsigned int currentWindowH = WINDOW_H;
unsigned int currentWindowW = WINDOW_W;
unsigned int frame = 0;

void updateConstants(float bat, float sig, ListNode points, vector2D tar) {
	owr_battery = bat;
	owr_signal = sig;
	path = points;
	target = tar;
	ROS_INFO("Updated");
}

int main(int argc, char **argv) {
	srand(time(NULL));
	ros::init(argc, argv, "GUI");
	GPSGUI *gpsnode = new GPSGUI(updateConstants);
	//gpsnode->spin();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OWR GUI");
	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}

void GPSAddRandPos() {
	ListNode rest = path;
	path = (ListNode) malloc(sizeof(_vector2D));
	double randn;
	randn = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) / 1000.0;
	path->x = randn + 151.139;
	randn = static_cast <double> (rand()) / static_cast <double> (RAND_MAX) / 1000.0;
	path->y = randn - 33.718;
	path->next = rest;
}

void printGPSPath() {
	ListNode curr = path;
	cout << "Start" << endl;
	while (curr != NULL) {
		printf("%.15f, %.15f\n", curr->x, curr->y);
		curr = curr->next;
	}
	cout << "End" << endl;
}

// draws GPS path and co-ordinates near the centre of the window
void drawGPS() {
	char GPSLat[30];
	char GPSLong[30];
	glPushMatrix();
	glTranslated(425, -250, 0);
	glColor3ub(255, 0, 0);
	
	// draw the rover as a red triangle
	/*glBegin(GL_POLYGON);
	glVertex2i(-15, 0);
	glVertex2i(15, 0);
	glVertex2i(0, 45);
	glEnd();*/
	
	if (frame == 0 || frame % 60 == 0) GPSAddRandPos(); // debug - randomly generate GPS values
		
	if (path != NULL) {
		longitude = path->x;
		latitude = path->y;
		//printGPSPath(); // debug - print out the list of GPS co-ordinates
		
		// draw out the scale of the path
		glPushMatrix();
		glTranslated(0, -120, 0);
		glColor3f(0,0,0);
		glScaled(100000,100000,1);
		glBegin(GL_LINE_STRIP);
		glColor3f(0,0,0);
		glVertex2d(0,0);
		glVertex2d(0.0001 / 0.9059, 0); // scale - this is 1 metre (approx)
		glEnd();
		glPopMatrix();
		
		// draws out the path so that the forward direction of the rover always facing up on the screen
		glPushMatrix();
		glScaled(100000,100000,1);
		
		glColor3f(0, 0, 1);
		double xoff = path->x, yoff = path->y;
		if (path->next != NULL) {
			double angle = -atan2(path->next->y - path->y, path->next->x - path->x) * 180.0/PI - 90;
			glRotated(angle, 0, 0, 1);
		}
		glTranslated(-xoff, -yoff, 0);
		ListNode curr = path;
		
		glPointSize(10);
		glBegin(GL_POINTS);
		glVertex2d(curr->x, curr->y);
		glEnd();
		glPointSize(1);
		
		glBegin(GL_LINE_STRIP);
		while (curr != NULL) {
			glVertex2d(curr->x, curr->y);
			curr = curr->next;
		}
		glEnd();
		glPopMatrix();
		
		// draw text for GPS co-ordinates
		glColor3f(0, 0, 0);
		glTranslated(-50, -175, 0);
		sprintf(GPSLat, "Lat: %.10f", latitude);
		sprintf(GPSLong, "Lon: %.10f", longitude);
		drawText(GPSLat, 0, 0);
		glTranslated(0, -20, 0);
		drawText(GPSLong, 0, 0);
	}
	glPopMatrix();
}

// draws feeds boxes on the left side of the window
void drawFeeds(void) {
	glPushMatrix();
	
	glColor3ub(0, 153, 0);
	glTranslated(50, -50, 0);
	glRectd(-30, -25, 30, 25);
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2i(-5, -6);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '1');
	
	glColor3ub(255, 133, 51);
	glTranslated(0, -75, 0);
	glRectd(-30, -25, 30, 25);
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2i(-5, -6);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '2');
	
	glColor3ub(255, 133, 51);
	glTranslated(0, -75, 0);
	glRectd(-30, -25, 30, 25);
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2i(-5, -6);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '3');
	
	glColor3ub(230, 0, 0);
	glTranslated(0, -75, 0);
	glRectd(-30, -25, 30, 25);
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos2i(-5, -6);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '4');
	
	glPopMatrix();
}

// draws the tilt angles for the left-right and front-back directions near the right of the screen
void drawTilt() {
	char text[30];
	glPushMatrix();
	
	glTranslated(700, -100, 0);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2d(0, 0);
	glVertex2d(100, 0);
	glVertex2d(100-100*cos(tiltX * PI / 180), 100*sin(tiltX * PI / 180));
	glEnd();
	
	glPushMatrix();
	glTranslated(-30, -50, 0);
	sprintf(text, "Left");
	drawText(text, 0, 0);
	glTranslated(100, 0, 0);
	sprintf(text, "Right");
	drawText(text, 0, 0);
	glPopMatrix();
	
	glTranslated(0, -100, 0);
	sprintf(text, "X :%.2fdeg", tiltX);
	drawText(text, 0, 0);
	
	glTranslated(0, -150, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2d(0, 0);
	glVertex2d(100, 0);
	glVertex2d(100-100*cos(tiltY * PI / 180), 100*sin(tiltY * PI / 180));
	glEnd();
	
	glPushMatrix();
	glTranslated(-30, -50, 0);
	sprintf(text, "Back");
	drawText(text, 0, 0);
	glTranslated(100, 0, 0);
	sprintf(text, "Front");
	drawText(text, 0, 0);
	glPopMatrix();
	
	glTranslated(0, -100, 0);
	sprintf(text, "Y :%.2fdeg", tiltY);
	drawText(text, 0, 0);
	glPopMatrix();
}

// draws the battery level near the top-right of the window
void drawBattery() {
	glPushMatrix();
	
	if (owr_battery < 3)
		glColor3f(1,0,0);
	else
		glColor3f(0,1,0);
	if (owr_battery < 0)
		owr_battery = 0;
	if (owr_battery > 10)
		owr_battery = 10;
	
	glTranslated(currentWindowW - 125, -50, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2i(0, 30);
	glVertex2i(0, -30);
	glVertex2i(100, -30);
	glVertex2i(100, 30);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2i(0, 30);
	glVertex2i(0, -30);
	glVertex2i(owr_battery * 10, -30);
	glVertex2i(owr_battery * 10, 30);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(100, 15);
	glVertex2i(100, -15);
	glVertex2i(110, -15);
	glVertex2i(110, 15);
	glEnd();
	
	glTranslated(0, -50, 0);
	glColor3f(0, 0, 0);
	char text[] = "battery";
	drawText(text, 15, 0);
	
	glPopMatrix();
}

// draw the signal level near the bottom-right of the window
void drawSignal() {
	glPushMatrix();
	
	if (owr_signal < 3)
		glColor3f(1,0,0);
	else
		glColor3f(0,1,0);
	if (owr_signal < 0)
		owr_signal = 0;
	if (owr_signal > 10)
		owr_signal = 10;
	
	glTranslated(currentWindowW - 125, -((int) currentWindowH - 100), 0);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(0, 0);
	glVertex2i(100, 0);
	glVertex2i(100, 50);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex2i(0, 0);
	glVertex2i(owr_signal * 10, 0);
	glVertex2i(owr_signal * 10, 5 * owr_signal);
	glEnd();
	
	glTranslated(0, -20, 0);
	glColor3f(0, 0, 0);
	char text[] = "signal";
	drawText(text, 25, 0);
	
	glPopMatrix();
}

void init(void) {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
	}
}

void reshape(int w, int h) {
	currentWindowH = h;
	currentWindowW = w;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (GLdouble)w, -(GLdouble)h, 0); // (0,0) is top left corner of window, use cartesian co-ordinates
	glMatrixMode(GL_MODELVIEW);
}

void display(void) {
	ros::spinOnce();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	drawFeeds();
	drawGPS();
	drawTilt();
	drawBattery();
	drawSignal();
	glFlush();
	glutSwapBuffers();
	usleep(16666);
	frame++;
	if (frame > 6001)
		frame -= 6000;
}

void drawText(char *text, int x, int y) {
	for (unsigned int i = 0; i < strlen(text); i++) {
		glRasterPos3f(x, y, 0);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
		x += 10;
	}
}
