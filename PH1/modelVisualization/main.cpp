

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

#include "coordinateFrame.h"

#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

GLfloat x = 0.0f;
GLfloat y = 1.0f;
GLfloat z = 0.0f;

CoordinateFrame mainframe;

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
	gluLookAt(	10.0,10.0,10.0,
				0.0 ,0.0 ,0.0,
				0.0 ,1.0 ,0.0
	);

	glRotatef(x, 1.0, 0.0, 0.0);
	glRotatef(y, 0.0, 1.0, 0.0);
	glRotatef(z, 0.0, 0.0, 1.0);

	glBegin(GL_TRIANGLES);
        frameFigure(mainframe);
	glEnd();

	glutSwapBuffers();
}

void keyboardCallback(unsigned char key_code, int xaaa, int yaaa) {
	switch (key_code) {
        case 'w':
            x += 10.0f;
            break;
        case 's':
            x -= 10.0f;
            break;
        case 'a':
            y -= 10.0f;
            break;
        case 'd':
            y += 10.0f;
            break;
        case 'e':
            z += 10.0f;
            break;
        case 'r':
            z -= 10.0f;
            break;
        default:
            break;
	}

	glutPostRedisplay();
}

void glut(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("PHASE 1");

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

void parseModel(const char * filename, CoordinateFrame state) {
	ifstream file(filename);
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			if (line.find("point") != string::npos) {
				vector<string> s = split(line, '"');
				framePoint(state, stod(s.at(1)), stod(s.at(3)), stod(s.at(5)));
			}
		}
		file.close();
	}
}

CoordinateFrame parseGroups(XMLNode * group, CoordinateFrame state){
	cout << "parseGroups" << endl;

	for(XMLNode * g = group->FirstChild();
		g != nullptr;
		g = g->NextSibling()
		)
	{
		const char * name = g->Value();
		cout << name << endl;
		if(!strcmp(name,"models")){
			XMLElement * e = g->FirstChildElement("model");
	
			while(e != nullptr) {
				parseModel(e->Attribute("file"),state);
				e = e->NextSiblingElement("model");
				cout << "." << endl;
			}

		}else if(!strcmp(name,"translate")){
			//parse, alter state| #< |
			XMLElement *e = (XMLElement*) g;
			frameTranslate(	state,
					 	   	e->DoubleAttribute("X"),
							e->DoubleAttribute("Y"),
							e->DoubleAttribute("Z")
							);
		}else if(!strcmp(name,"rotate")){
			//parse, alter state| #< |
			XMLElement *e = (XMLElement*) g;
			frameRotate(state,
						e->DoubleAttribute("angle"),
						e->DoubleAttribute("axisX"),
						e->DoubleAttribute("axisY"),
						e->DoubleAttribute("axisZ")
						);
		}else if(!strcmp(name,"scale")){
			//parse, alter state| #< |
			XMLElement *e = (XMLElement*) g;
			frameScale(state,
							   e->DoubleAttribute("stretchX", 1.0),
							   e->DoubleAttribute("stretchY", 1.0),
							   e->DoubleAttribute("stretchZ", 1.0)
			);
		}else if(!strcmp(name,"group")){
			state = parseGroups(g, state);
		}else{
			cout << "wrong" << endl;
		}
	
	}
	
	cout << "parseGroups leave" << endl;

	return state;
}

CoordinateFrame parse(const char * filename) {
	XMLDocument xml_doc;
	cout << "parse" << endl;
	XMLError eResult = 
		xml_doc.LoadFile(filename);

	if (eResult != XML_SUCCESS)
		return NULL;

	XMLNode* root = xml_doc.FirstChildElement("scene");

	if (root == nullptr)
		return NULL;
	
	CoordinateFrame frame = mkCoordinateFrame();

	frame = parseGroups(root, frame);

	xml_doc.Clear();

	return frame;
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		cout << "No scene file provided." << endl;
		return 2;
	}
	mainframe = parse(argv[1]);
	if (!mainframe) {
		return 3;
	}

	glut(argc, argv);

	unmkCoordinateFrame(mainframe);

	return 1;
}
