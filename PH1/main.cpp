#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

float anglex = 0.0;
float angley = 0.0;
float anglez = 0.0;

int i=0;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void circle(MonadWindow reference,int points){
	float tangle = (float)((points - 2) * 180);
	float angle = tangle/points;
	float otherside = sqrt(2*1.0-2*1.0*cos(180-angle));
	MonadWindow mon = mkMonadWindow(reference);

	//printf("angle : %f :| %f|> l : %f : r : %f || %f \n",angle , tangle ,1.0, otherside, 180 - angle/2);

	//glBegin(GL_TRIANGLES);

		for(int i = 0; i< points;i++){
			monadTriangle(mon,angle);
		}

	//glEnd();

}


float lin( float dh, float h){
	return (h - dh)/h;
}

float cuple( float dh, float h ){
	return sqrt( (h*h  - dh*dh) )/h;
}


void sphere(MonadWindow reference,int points, int stacks ){
	MonadWindow nw = mkMonadWindow(reference);
	stacker(nw,points,stacks,1, cuple );
	monadRotate(nw,180,1.0,0.0,0.0);
	stacker(nw, points,stacks,1, cuple );
	//monadRotate(nw,180,0.0,1.0,0.0);
	unmkMonadWindow(nw);
}

void renderScene(void) {
	i++;
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(5.0,5.0,5.0, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put the geometric transformations here
	glBegin(GL_LINES);
		glColor3f(1.0f, 1.0f, 1.0f);

		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);

		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);

		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	//glRotatef(-90,1.0,0.0,0.0);

	//glTranslatef(0.5,1.0,0.5);
	glRotatef(angley,0.0,0.0,1.0);
	glRotatef(anglex,1.0,0.0,0.0);
	glRotatef(anglez,0.0,1.0,0.0);

	glBegin(GL_LINES);
		glColor3f(1.0f, 5.0f, 0.0f);

		glVertex3f(-2.0f, 0.0f, 0.0f);
		glVertex3f(2.0f, 0.0f, 0.0f);

		glVertex3f(0.0f, -2.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);

		glVertex3f(0.0f, 0.0f, -2.0f);
		glVertex3f(0.0f, 0.0f, 2.0f);
	glEnd();

	//glTranslatef(-0.5,-0.5,0.0);

// put drawing instructions here
	glColor3f(1.0f, 1.0f, 1.0f);
	int n=32;
	//cylinder(n,2.0, 2.0, 2.0);
	MonadWindow reference = mkMonadWindow();
	glColor3f(1.0f, 5.0f, 0.0f);
	
	
	stacker(reference,3,1,1,lin);

	if( i == 1)
		print_trace(reference);
	//sphere( reference, 3, 1);
	
	unmkMonadWindow(reference);

	// End of frame
	glutSwapBuffers();
}

// write function to process keyboard events

void rotate(int key_code, int x, int y){


	switch (key_code){
		case GLUT_KEY_UP: 
			angley+=15.0;
			break;
		case GLUT_KEY_DOWN:
			angley-=15.0; 
			break;
		case GLUT_KEY_LEFT:
			anglex-=15.0;
			break;
		case GLUT_KEY_RIGHT:
			anglex+=15.0; 
			break;
	}
	glutPostRedisplay();
}

void zrotate(unsigned char key_code, int x, int y){


	switch (key_code){
		case 'z': 
			anglez+=15.0;
			break;
		case 'x':
			anglez-=15.0; 
			break;
	}
	glutPostRedisplay();
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
// put here the registration of the keyboard callbacks
	//glutSpecialFunc(rotate);
	glutSpecialFunc(rotate);
	glutKeyboardFunc(zrotate);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();

	
	return 1;
}
