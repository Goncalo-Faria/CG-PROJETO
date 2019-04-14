#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include "GL/glut.h"
#else
	#include <GLUT/glut.h>
#endif

#include "coordinateFrame.h"
#include "tinyxml2.h"
#include "matop.h"
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>

#define back(X,Y) unmkCoordinateFrame(X);X = mkCoordinateFrame(Y)

using namespace tinyxml2; 
using namespace std;

typedef struct frame { 
	float t[4][4];
	vector<Point>* points;
	struct frame * ref;
	GLuint buffer;
} *CoordinateFrame;

/*API*/

/*Internal auxiliary procedures*/

CoordinateFrame mkCoordinateFrameRx(float angle);
CoordinateFrame mkCoordinateFrameRy(float angle);
CoordinateFrame mkCoordinateFrameRz(float angle);

void frameAggregate(CoordinateFrame a, CoordinateFrame b);

void plataform(CoordinateFrame reference, int points, float bottomradius, float topradius, int downface, int upface);
void plane(CoordinateFrame reference);

CoordinateFrame parseGroups(XMLNode * group, CoordinateFrame state);
void parseModel(const char * filename, CoordinateFrame state);
vector<string> split(string strToSplit, char delimeter);

/* Implementation */

void frameTrace(CoordinateFrame m, char* filename, char* figure){
    int count=0;

    XMLDocument doc;
    XMLNode* major = doc.InsertEndChild( doc.NewElement(figure) );
    XMLElement* triangle,*point;
    XMLNode* nTriangle;
    for (Point p : *m->points){
        if(!(count%3) ){
            triangle = doc.NewElement( "triangle" );
            nTriangle = major->InsertEndChild( triangle );
        }
        point = doc.NewElement( "point" );
        nTriangle->InsertEndChild( point );

        point->SetAttribute("x",p.p[0]);
        point->SetAttribute("y",p.p[1]);
        point->SetAttribute("z",p.p[2]);

        count++;
    }

    doc.SaveFile(filename);
}

CoordinateFrame mkCoordinateFrame(){
	CoordinateFrame m = (CoordinateFrame) malloc( sizeof(struct frame) );

	float **tmp = identity();
	matAssign( m->t, tmp);
	freeMat(tmp);

	m->ref = NULL;
	m->buffer= -1;
	m->points = new vector<Point>();

	return m;
}

CoordinateFrame mkCoordinateFrame(CoordinateFrame mold){
	CoordinateFrame m = (CoordinateFrame) malloc( sizeof(struct frame) );
	
	matAssign( m->t, mold->t);
	
	m->ref = mold;
	m->buffer= -1;
    m->points = NULL;

	return m;
}

CoordinateFrame mkCoordinateFrame(const char * filename) {
	XMLDocument xml_doc;
	//cout << "parse" << endl;
	XMLError eResult = 
		xml_doc.LoadFile(filename);

	if (eResult != XML_SUCCESS) {
        //cout << "parse" << endl;
        return NULL;
    }

	XMLNode* root = xml_doc.FirstChildElement("scene");

	if (root == nullptr)
		return NULL;
	
	CoordinateFrame frame = mkCoordinateFrame();

	frame = parseGroups(root, frame);

	xml_doc.Clear();

	return frame;
}

void unmkCoordinateFrame(CoordinateFrame m){
    if( m->points != NULL)
        delete m->points;
    free(m);
}

void frameReference(CoordinateFrame  m, Point p){

	if( !m->ref ) {
	    //printf("ka ");
        m->points->push_back(p);
        //printf("bomm \n");
    }else {
        //printf("oo ");
        frameReference(m->ref, p);
    }
}

void frameTransform(CoordinateFrame m, float** matt){
        float** tmp = matmul( m->t , matt);
        matAssign( m->t, tmp);
        freeMat(tmp);
}

void frameRotate(CoordinateFrame m, float angle, float vx, float vy, float vz){
	float rx = vx*angle;
	float ry = vy*angle;
	float rz = vz*angle;

	float** mrx = matRx(rx);
    float** mry = matRy(ry);
    float** mrz = matRz(rz);

    float** mr1 = matmul(mrx,mry);
    float** mr2 = matmul(mr1,mrz);
    freeMat(mr1);

    frameTransform(m,mr2);
    freeMat(mr2);
}

void frameTranslate(CoordinateFrame m, float x, float y, float z){
	float** t = identity();
	
	t[0][3] = x;
	t[1][3] = y;
	t[2][3] = z;

	frameTransform(m,t);
	freeMat(t);
}

void frameScale(CoordinateFrame m, float vx, float vy, float vz ){
	float** t = identity();

	t[0][0] = vx;
	t[1][1] = vy;
	t[2][2] = vz;

    frameTransform(m,t);
    freeMat(t);
}

void framePoint(CoordinateFrame m, float x, float y, float z){
	float p[4];
	float a[4];
	//printf("%G - %G - %G \n",x,y,z); 
	p[0] = x;
	p[1] = y;
	p[2] = z;
	p[3] = 1;

	for(int i=0; i< 4; i++){
		a[i] = 0;
		for(int j=0; j< 4; j++)
			a[i] += m->t[i][j] * p[j];
	}

	Point po;
	po.p[0] = a[0];
	po.p[1] = a[1];
	po.p[2] = a[2];

	frameReference(m, po );
}

void frameTriangle(CoordinateFrame m, float angle, float difs){
	framePoint(m, cos(angle), 0,sin(angle));
	framePoint(m, 0, 0, 0);
	framePoint(m, cos(angle+difs), 0,sin(angle+difs));
}

void frameDraw(CoordinateFrame reference){
	glBindBuffer(GL_ARRAY_BUFFER,reference->buffer);
	glVertexPointer(3,GL_FLOAT,0,0);
	glDrawArrays(GL_TRIANGLES, 0, reference->points->size());
}

void frameBufferData(CoordinateFrame reference){
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glGenBuffers(1, &(reference->buffer) );
	glBindBuffer(GL_ARRAY_BUFFER,reference->buffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		reference->points->size() * sizeof(Point),
		&( (*(reference->points))[0] ),
		GL_STATIC_DRAW
	);
}

CoordinateFrame parseGroups(XMLNode * group, CoordinateFrame state){

	for(XMLNode * g = group->FirstChild();
		g != nullptr;
		g = g->NextSibling()
		)
	{
		const char * name = g->Value();
		if(!strcmp(name,"models")){
			XMLElement * e = g->FirstChildElement("model");
	
			while(e != nullptr) {
				parseModel(e->Attribute("file"),state);
				e = e->NextSiblingElement("model");
			}
		}else if(!strcmp(name,"translate")){

			XMLElement *e = (XMLElement*) g;
			frameTranslate(	state,
					 	   	e->FloatAttribute("X"),
							e->FloatAttribute("Y"),
							e->FloatAttribute("Z")
							);
		}else if(!strcmp(name,"rotate")){
			
			XMLElement *e = (XMLElement*) g;
			frameRotate(state,
						e->FloatAttribute("angle"),
						e->FloatAttribute("axisX"),
						e->FloatAttribute("axisY"),
						e->FloatAttribute("axisZ")
						);
		}else if(!strcmp(name,"scale")){
		
			XMLElement *e = (XMLElement*) g;
			frameScale(state,
							   e->FloatAttribute("stretchX", 1.0),
							   e->FloatAttribute("stretchY", 1.0),
							   e->FloatAttribute("stretchZ", 1.0)
			);
		
		}else if(!strcmp(name,"group")){
			unmkCoordinateFrame(parseGroups(g, mkCoordinateFrame(state)));
		}
	
	}

	return state;
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