

#if defined(_WIN32)
    #include "GL/glut.h"
#else
    #include <GLUT/glut.h>
#endif

#include <math.h>
#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

vector<tuple<double, double, double>> points;

GLfloat x = 0.0f;
GLfloat y = 1.0f;
GLfloat z = 0.0f;


void changeSize(int w, int h) {
	if(h == 0) h = 1;

    double ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(	12.0,12.0,12.0,
				0.0 ,0.0 ,0.0,
				0.0 ,1.0 ,0.0
	);

	glRotatef(x, 1.0, 0.0, 0.0);
	glRotatef(y, 0.0, 1.0, 0.0);
	glRotatef(z, 0.0, 0.0, 1.0);

	glBegin(GL_TRIANGLES);
        for(auto const& value : points) {
			glColor3f(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
            glVertex3f(get<0>(value), get<1>(value), get<2>(value));
        }
	glEnd();

	glutSwapBuffers();
}


void keyboardCallback(unsigned char key_code, int xaaa, int yaaa) {
	switch (key_code) {
        case 'w':
            x += 2.0f;
            break;
        case 's':
            x -= 2.0f;
            break;
        case 'a':
            y -= 2.0f;
            break;
        case 'd':
            y += 2.0f;
            break;
        case 'e':
            z += 2.0f;
            break;
        case 'r':
            z -= 2.0f;
            break;
        default:
            break;
	}

	glutPostRedisplay();
}

void glut(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(2000,1000);
    glutCreateWindow("Solar System");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
	glutKeyboardFunc(keyboardCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutMainLoop();
}

vector<string> split(string strToSplit, char delimeter)
{
	stringstream ss(strToSplit);
	string item;
	vector<string> splittedStrings;

	while (getline(ss, item, delimeter)){
		splittedStrings.push_back(item);
	}

	return splittedStrings;
}

void parseModel(const char * filename) {
	ifstream file(filename);
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			if (line.find("point") != string::npos) {
				vector<string> s = split(line, '"');
				points.emplace_back(make_tuple(stod(s.at(1)), stod(s.at(3)), stod(s.at(5))));
			}
		}
		file.close();
	}
}

bool parseModels(const char * filename) {
	XMLDocument xml_doc;

	XMLError eResult = 
		xml_doc.LoadFile(filename);

	if (eResult != XML_SUCCESS)
		return false;

	XMLNode* root = xml_doc.FirstChildElement("scene");

	if (root == nullptr)
		return false;

	XMLElement * e = root->FirstChildElement("model");
	while( e != nullptr) {
		parseModel(e->Attribute("file"));
		e = e->NextSiblingElement("model");
	}

	return true;
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		cout << "No model file provided." << endl;
		return 2;
	}

	if (!parseModels(argv[1])) {
		return 3;
	}

	glut(argc, argv);

	return 1;
}
