#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include "GL/glut.h"
#else
	#include <GLUT/glut.h>
#endif

#include "coordinateFrame.h"
#include "tinyxml2.h"
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
	double t[4][4];
	vector<Point>* points;
	struct frame * ref;
	GLuint buffer;
} *CoordinateFrame;


/*API*/
CoordinateFrame mkCoordinateFrame();
CoordinateFrame mkCoordinateFrame(CoordinateFrame mold);
void unmkCoordinateFrame(CoordinateFrame m);

void frameRotate(CoordinateFrame m, double angle, double vx, double vy, double vz);
void frameTranslate(CoordinateFrame m, double x, double y, double z);
void frameScale(CoordinateFrame m, double vx, double vy, double vz );

void frameBazierPatch(CoordinateFrame reference, Point * points, int tesselation);
void frameTriangle(CoordinateFrame m, double angle, double difs);
void frameRegularPolygon(CoordinateFrame reference,int points);

void frameStacker(CoordinateFrame reference, int points, int stacks, double (*f)(double));
void frameHyperplane(CoordinateFrame reference, int divisions);
void frameCube(CoordinateFrame reference, int divisions);

void frameTrace(CoordinateFrame m, char* filename, char* figure);
void frameDraw(CoordinateFrame reference);
void frameBufferData(CoordinateFrame reference);

CoordinateFrame parse(const char * filename);

/*Internal auxiliary procedures*/

CoordinateFrame mkCoordinateFrameRx(double angle);
CoordinateFrame mkCoordinateFrameRy(double angle);
CoordinateFrame mkCoordinateFrameRz(double angle);

void frameAggregate(CoordinateFrame a, CoordinateFrame b);

void plataform(CoordinateFrame reference, int points, double bottomradius, double topradius, int downface, int upface);
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
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = (i==j);
	
	m->ref = NULL;
	m->buffer= -1;
	m->points = new vector<Point>();

	return m;
}

void unmkCoordinateFrame(CoordinateFrame m){
    if( m->points != NULL)
        delete m->points;
    free(m);
}

CoordinateFrame mkCoordinateFrame(CoordinateFrame mold){
	CoordinateFrame m = (CoordinateFrame) malloc( sizeof(struct frame) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = mold->t[i][j];
	
	m->ref = mold;
	m->buffer= -1;
    m->points = NULL;

	return m;
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

CoordinateFrame mkCoordinateFrameRx(double angle){
	CoordinateFrame m = mkCoordinateFrame();

	double rad = (M_PI/180.0)*angle;

	m->t[1][1] = cos(rad);
	m->t[1][2] = -sin(rad);
	m->t[2][1] = sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

CoordinateFrame mkCoordinateFrameRy(double angle){
	CoordinateFrame m = mkCoordinateFrame();

	double rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][2] = sin(rad);
	m->t[2][0] = -sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

CoordinateFrame mkCoordinateFrameRz(double angle){
	CoordinateFrame m = mkCoordinateFrame();

	double rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][1] = -sin(rad);
	m->t[1][0] = sin(rad);
	m->t[1][1] = cos(rad);

	return m;
}

void frameAggregate(CoordinateFrame a, CoordinateFrame b){
	double result[4][4];

	for( int i = 0; i<4; i++ )
		for(int j = 0;j<4; j++)
			result[i][j] = 0;
	
	for( int i = 0; i<4; i++ )
		for(int j = 0;j<4; j++)
			for(int k = 0; k<4; k++)
				result[i][j] += a->t[i][k] *  b->t[k][j];

	for( int i = 0; i<4; i++ )
		for(int j = 0;j<4; j++)
			a->t[i][j] = result[i][j];
	
}

void frameRotate(CoordinateFrame m, double angle, double vx, double vy, double vz){
	double rx = vx*angle;
	double ry = vy*angle;
	double rz = vz*angle;

	CoordinateFrame tmp = mkCoordinateFrameRx(rx);
	frameAggregate(m,tmp);
	unmkCoordinateFrame(tmp);

	tmp = mkCoordinateFrameRy(ry);
	frameAggregate(m,tmp);
	unmkCoordinateFrame(tmp);

	tmp = mkCoordinateFrameRz(rz);
	frameAggregate(m,tmp);
	unmkCoordinateFrame(tmp);
}

void frameTranslate(CoordinateFrame m, double x, double y, double z){
	CoordinateFrame mt = mkCoordinateFrame();
	
	mt->t[0][3] = x;
	mt->t[1][3] = y;
	mt->t[2][3] = z;

	frameAggregate(m, mt);

	unmkCoordinateFrame(mt);
}

void frameScale(CoordinateFrame m, double vx, double vy, double vz ){
	CoordinateFrame mt = mkCoordinateFrame();
	
	mt->t[0][0] = vx;
	mt->t[1][1] = vy;
	mt->t[2][2] = vz;

	frameAggregate(m, mt);

	unmkCoordinateFrame(mt);
}

void framePoint(CoordinateFrame m, double x, double y, double z){
	double p[4];
	double a[4];
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

	Point * po = mkPoint(a[0], a[1], a[2]);
	frameReference(m, *po );
	unmkPoint(po);
}

void frameTriangle(CoordinateFrame m, double angle, double difs){
	framePoint(m, cos(angle), 0,sin(angle));
	framePoint(m, 0, 0, 0);
	framePoint(m, cos(angle+difs), 0,sin(angle+difs));
}

void plataform(CoordinateFrame reference, int points, double bottomradius, double topradius, int downface, int upface){
	double rinner = 2 * M_PI /((double)points);

	CoordinateFrame db = mkCoordinateFrame(reference);
	CoordinateFrame ub = mkCoordinateFrame(reference);

	frameTranslate(ub, 0.0, 1.0, 0.0);
	frameRotate(db, 180, 1.0, 0.0, 0.0);

	frameScale(db, bottomradius, 0.0, bottomradius);
	frameScale(ub, topradius, 0.0, topradius);

	if (upface)
		frameRegularPolygon(ub, points);

	if (downface)
		frameRegularPolygon(db, points);
	
	frameRotate(db, 180, 1.0, 0.0, 0.0);
		
	for( int i = 0; i<points; i++ ){

		framePoint(db,cos(i* rinner),0.0,sin(i* rinner));
		framePoint(ub,cos(i* rinner),0.0,sin(i* rinner));
		framePoint(db,cos((i+1)*rinner),0.0,sin((i+1)*rinner));

		framePoint(db,cos((i+1)* rinner),0.0,sin((i+1)* rinner));
		framePoint(ub,cos(i* rinner),0.0,sin(i* rinner));
		framePoint(ub,cos((i+1)*rinner),0.0,sin((i+1)*rinner));			
		
	}
	
	unmkCoordinateFrame(db);
	unmkCoordinateFrame(ub);
}

void frameStacker(CoordinateFrame reference,int points, int stacks, double (*f)(double) ){
	double dh = 1.0/stacks;
	double currenth = dh;
	double f0 = f(0.0);
	double f1;
	CoordinateFrame nw = mkCoordinateFrame(reference);
	frameScale(nw,1.0f,dh,1.0f);
	for(int i = 0; i < stacks; i++){
		f1 = f(currenth);
		plataform(nw, points, f0, f1 ,(i==0),(i == stacks-1));
		frameTranslate(nw,0.0f,1.0f,0.0f);
		f0 = f1;
		currenth += dh;
	}

	unmkCoordinateFrame(nw);
}

void plane(CoordinateFrame reference) {
    CoordinateFrame nw = mkCoordinateFrame(reference);

    framePoint(nw,-0.5,0,-0.5);
    framePoint(nw,-0.5,0,0.5);
    framePoint(nw,0.5,0,-0.5);

    framePoint(nw,-0.5,0,0.5);
    framePoint(nw,0.5,0,0.5);
    framePoint(nw,0.5,0,-0.5);

    unmkCoordinateFrame(nw);
}


void frameHyperplane(CoordinateFrame reference, int divisions){
    CoordinateFrame nw = mkCoordinateFrame(reference);

    frameScale(nw, 1.0/(double)divisions, 1.0/(double)divisions, 1.0/(double)divisions);
    frameTranslate(nw, -0.5*(divisions-1), 0.0, -0.5*(divisions-1));

    for(int i = 0; i < divisions; i++){
        for(int j = 0; j < divisions; j++){
            plane(nw);
            frameTranslate(nw,1,0.0,0.0);
        }
        frameTranslate(nw,-divisions,0.0,1.0);
    }

    unmkCoordinateFrame(nw);
}

void frameCube(CoordinateFrame reference, int divisions) {
    CoordinateFrame nw = mkCoordinateFrame(reference);

	for(int i = 0; i < 3; i++){
		switch(i){
			case 1: frameRotate(nw,90,1,0,0);break;
			case 2: frameRotate(nw,90,0.0,0.0,-1.0);break;
            default:break;
		}

    	frameTranslate(nw, 0.0, 0.5, 0.0);
    	frameHyperplane(nw, divisions);

    	frameTranslate(nw, 0.0, -1.0, 0.0);
    	frameRotate(nw,180,1.0,0.0,0.0);
    	frameHyperplane(nw, divisions);

    	back(nw,reference);
	}

    unmkCoordinateFrame(nw);
}

void frameRegularPolygon(CoordinateFrame reference,int points){
	double inner = 2*M_PI/((double)points);
	CoordinateFrame mon = mkCoordinateFrame(reference);

    
	for(int i = 0; i< points;i++)
		frameTriangle(mon,i * inner, inner);

	unmkCoordinateFrame(mon);
}

void frameDraw(CoordinateFrame reference){
	glBindBuffer(GL_ARRAY_BUFFER,reference->buffer);
	glVertexPointer(3,GL_DOUBLE,0,0);
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
					 	   	e->DoubleAttribute("X"),
							e->DoubleAttribute("Y"),
							e->DoubleAttribute("Z")
							);
		}else if(!strcmp(name,"rotate")){
			
			XMLElement *e = (XMLElement*) g;
			frameRotate(state,
						e->DoubleAttribute("angle"),
						e->DoubleAttribute("axisX"),
						e->DoubleAttribute("axisY"),
						e->DoubleAttribute("axisZ")
						);
		}else if(!strcmp(name,"scale")){
		
			XMLElement *e = (XMLElement*) g;
			frameScale(state,
							   e->DoubleAttribute("stretchX", 1.0),
							   e->DoubleAttribute("stretchY", 1.0),
							   e->DoubleAttribute("stretchZ", 1.0)
			);
		
		}else if(!strcmp(name,"group")){
			unmkCoordinateFrame(parseGroups(g, mkCoordinateFrame(state)));
		}
	
	}

	return state;
}

CoordinateFrame parse(const char * filename) {
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

int factorial(int n){ return (n < 2) ? 1 : n*factorial(n-1); }

double bernstein(int i, int n, double t){
	double r = (double) factorial(n) / (double) (factorial(i) * factorial(n - i));
	r *= pow(t,i);
	r *= pow(1-t,n-i);
	return r;
}

void frameBazierPatch(CoordinateFrame reference, Point * points, int tesselation){
    /*superfice de bazie com bicubic patches (4,4)
     *
     * Uma forma numericamente estavel de fazer o algoritmo de casteljau's
     * através de polinomis na forma Bernstein
     *
     * */
	Point curvePoints[tesselation+1][tesselation+1];

	for(int ui = 0; ui <= tesselation; ui++){
		double u = double(ui)/double(tesselation);// calcular ut
		for(int vi = 0; vi <= tesselation; vi++){
			double v = double(vi)/double(tesselation);// calcular vt
			Point point;
			point.p[0] = 0;
			point.p[1] = 0;
			point.p[2] = 0;
			for(int m = 0; m < 4; m++){
				for(int n =0; n< 4; n++){
					double bm = bernstein(m, 3, u);
					double bn = bernstein(n, 3, v);
					double b = bm * bn;// calculo do coeficiente da formula
					point.p[0] += b *  points[4*m + n].p[0];
					point.p[1] += b *  points[4*m + n].p[1];
					point.p[2] += b *  points[4*m + n].p[2];
				}
			}
			curvePoints[ui][vi] = point;
		}
	}

	for(int ui = 0; ui < tesselation; ui++) 
        for(int vi = 0; vi < tesselation; vi++) {

            framePoint(
                    reference,
                    curvePoints[ui][vi+1].p[0],
                    curvePoints[ui][vi+1].p[1],
                    curvePoints[ui][vi+1].p[2]);

            framePoint(
                    reference,
                    curvePoints[ui+1][vi].p[0],
                    curvePoints[ui+1][vi].p[1],
                    curvePoints[ui+1][vi].p[2]);

            framePoint(
                    reference,
                    curvePoints[ui][vi].p[0],
                    curvePoints[ui][vi].p[1],
                    curvePoints[ui][vi].p[2]);

			// sencond triangle
            /*
            framePoint(
                    reference,
                    curvePoints[ui][vi+1].p[0],
                    curvePoints[ui][vi+1].p[1],
                    curvePoints[ui][vi+1].p[2]);

            framePoint(
                    reference,
                    curvePoints[ui+1][vi].p[0],
                    curvePoints[ui+1][vi].p[1],
                    curvePoints[ui+1][vi].p[2]);



            framePoint(
                    reference,
                    curvePoints[ui+1][vi+1].p[0],
                    curvePoints[ui+1][vi+1].p[1],
                    curvePoints[ui+1][vi+1].p[2]);
            */

            // third
            framePoint(
                    reference,
                    curvePoints[ui+1][vi].p[0],
                    curvePoints[ui+1][vi].p[1],
                    curvePoints[ui+1][vi].p[2]);


            framePoint(
                    reference,
                    curvePoints[ui][vi+1].p[0],
                    curvePoints[ui][vi+1].p[1],
                    curvePoints[ui][vi+1].p[2]);

            framePoint(
                    reference,
                    curvePoints[ui+1][vi+1].p[0],
                    curvePoints[ui+1][vi+1].p[1],
                    curvePoints[ui+1][vi+1].p[2]);
            // forth
            /*
            framePoint(
                    reference,
                    curvePoints[ui+1][vi].p[0],
                    curvePoints[ui+1][vi].p[1],
                    curvePoints[ui+1][vi].p[2]);


            framePoint(
                    reference,
                    curvePoints[ui][vi+1].p[0],
                    curvePoints[ui][vi+1].p[1],
                    curvePoints[ui][vi+1].p[2]);

            framePoint(
                    reference,
                    curvePoints[ui][vi].p[0],
                    curvePoints[ui][vi].p[1],
                    curvePoints[ui][vi].p[2]);

			*/

        }
	
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