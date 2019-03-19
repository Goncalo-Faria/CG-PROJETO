#define _USE_MATH_DEFINES

#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "coordinateFrame.h"
#include "tinyxml2.h"

#include <vector>
#include <tuple>

#if defined(_WIN32)
    #include "GL/glut.h"
#else
    #include <GLUT/glut.h>
#endif

#define back(X,Y) unmkCoordinateFrame(X);X = mkCoordinateFrame(Y)

using namespace tinyxml2; 
using namespace std;

typedef struct point {
	double p[3];
} *Point;

typedef struct l {
	Point value;
	struct l * px;
} *L;

typedef struct frame { 
	double t[4][4];
	vector<Point> points;
	struct frame * ref;
} *CoordinateFrame;

/*API*/
CoordinateFrame mkCoordinateFrame();
CoordinateFrame mkCoordinateFrame(CoordinateFrame mold);
void unmkCoordinateFrame(CoordinateFrame m);

void frameRotate(CoordinateFrame m, double angle, double vx, double vy, double vz);
void frameTranslate(CoordinateFrame m, double x, double y, double z);
void frameScale(CoordinateFrame m, double vx, double vy, double vz );

void framePoint(CoordinateFrame m, double x, double y, double z);
void frameTriangle(CoordinateFrame m, double angle, double difs);
void frameRegularPolygon(CoordinateFrame reference,int points);

void frameStacker(CoordinateFrame reference, int points, int stacks, double (*f)(double));
void frameHyperplane(CoordinateFrame reference, int divisions);
void frameCube(CoordinateFrame reference, int divisions);

void frameTrace(CoordinateFrame m, char* filename, char* figure);
void frameFigure(CoordinateFrame reference);

/*Internal auxiliary procedures*/
Point mkPoint(double x, double y, double z);
void unmkPoint(Point p);

CoordinateFrame mkCoordinateFrameRx(double angle);
CoordinateFrame mkCoordinateFrameRy(double angle);
CoordinateFrame mkCoordinateFrameRz(double angle);

void frameAggregate(CoordinateFrame a, CoordinateFrame b);

void plataform(CoordinateFrame reference, int points, double bottomradius, double topradius, int downface, int upface);
void plane(CoordinateFrame reference);

/* Implementation */

Point mkPoint(double x, double y, double z){
	Point m = (Point) malloc( sizeof(struct point) );
	m->p[0] = x;
	m->p[1] = y;
	m->p[2] = z;
	return m;
}

void unmkPoint(Point p){
	free(p);
}

void frameTrace(CoordinateFrame m, char* filename, char* figure){
	int count=0;

	XMLDocument doc;
	XMLNode* major = doc.InsertEndChild( doc.NewElement(figure) );
	XMLElement* triangle,*point;
	XMLNode* nTriangle;
	for (Point p : m->points){
		if(!(count%3) ){
			triangle = doc.NewElement( "triangle" );
            nTriangle = major->InsertEndChild( triangle );
		}
		point = doc.NewElement( "point" );
        nTriangle->InsertEndChild( point );

		point->SetAttribute("x",p->p[0]);
		point->SetAttribute("y",p->p[1]);
		point->SetAttribute("z",p->p[2]);

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

	return m;
}

void unmkCoordinateFrame(CoordinateFrame m){

	for (Point attack : m->points) 
    	unmkPoint(attack);

	free(m);
}

CoordinateFrame mkCoordinateFrame(CoordinateFrame mold){
	auto m = (CoordinateFrame) malloc( sizeof(struct frame) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = mold->t[i][j];
	
	m->ref = mold;

	return m;
}

void frameReference(CoordinateFrame  m, Point p){
	if( !m->ref )
		m->points.__emplace_back(p);
	else
		frameReference(m->ref,p);	
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

	frameReference(m, mkPoint(a[0], a[1], a[2]));
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

void frameFigure(CoordinateFrame reference){
	for(Point value : reference->points ) {
		glColor3f(rand() / double(RAND_MAX), rand() / double(RAND_MAX), rand() / double(RAND_MAX));
        glVertex3f(value->p[0], value->p[1], value->p[2]);
    }
}