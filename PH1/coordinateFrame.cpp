#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "coordinateFrame.h"
#include "tinyxml2.h"

#define back(X,Y) unmkCoordinateFrame(X);X = mkCoordinateFrame(Y)

using namespace tinyxml2; 

typedef struct point {
	double p[3];
} *Point;

typedef struct l {
	Point value;
	struct l * px;
} *L;

typedef struct frame { 
	double t[4][4];
	L back;
	L front;
	struct frame * ref;
} *CoordinateFrame;


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

	XMLDocument* doc = new XMLDocument();
	XMLNode* major = doc->InsertEndChild( doc->NewElement(figure) );
	XMLElement* triangle;
	XMLElement *point;
	XMLNode* ntraingle, *npoint;
	XMLText* text;
	for(L cur = m->front; cur ; cur = cur->px){

		if(!(count%3) ){
			triangle = doc->NewElement( "triangle" );
			ntraingle = major->InsertEndChild( triangle );
		}
				
		point = doc->NewElement( "point" );
		npoint = ntraingle->InsertEndChild( point );

		point->SetAttribute("x",cur->value->p[0]);
		point->SetAttribute("y",cur->value->p[1]);
		point->SetAttribute("z",cur->value->p[2]);

		count++;
	}

	doc->SaveFile(filename);

	delete doc;
}



CoordinateFrame mkCoordinateFrame(){
	CoordinateFrame m = (CoordinateFrame) malloc( sizeof(struct frame) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = (i==j);
	
	m->ref = NULL;
	m->back = NULL;
	m->front = NULL;

	return m;
}

void unmkCoordinateFrame(CoordinateFrame m){
	L aux;
	for(L cur = m->front; cur ; cur = aux){
		unmkPoint(cur->value);
		aux = cur->px;
		free(cur);
	}
	free(m);
}

CoordinateFrame mkCoordinateFrame(CoordinateFrame mold){
	CoordinateFrame m = (CoordinateFrame) malloc( sizeof(struct frame) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = mold->t[i][j];
	
	m->ref = mold;
	m->back = m->front = NULL;

	return m;
}

void frameReference(CoordinateFrame  m, Point p){
	if( !m->ref ){
		L n =  (L) malloc( sizeof(struct l) );
		n->value = p;
		n->px = NULL;

		if( !m->back ){
			m->back = m->front = n;
		}else{
			m->back->px = n;
			m->back = n;
		}

	}else{

		frameReference(m->ref,p);
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

void frameAgregate(CoordinateFrame a, CoordinateFrame b){
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
	frameAgregate(m,tmp);
	unmkCoordinateFrame(tmp);

	tmp = mkCoordinateFrameRy(ry);
	frameAgregate(m,tmp);
	unmkCoordinateFrame(tmp);

	tmp = mkCoordinateFrameRz(rz);
	frameAgregate(m,tmp);
	unmkCoordinateFrame(tmp);


}

void frameTranslate(CoordinateFrame m, double x, double y, double z){
	CoordinateFrame mt = mkCoordinateFrame();
	
	mt->t[0][3] = x;
	mt->t[1][3] = y;
	mt->t[2][3] = z;

	frameAgregate(m, mt);

	unmkCoordinateFrame(mt);
}

void frameScale(CoordinateFrame m, double vx, double vy, double vz ){
	CoordinateFrame mt = mkCoordinateFrame();
	
	mt->t[0][0] = vx;
	mt->t[1][1] = vy;
	mt->t[2][2] = vz;

	frameAgregate(m, mt);

	unmkCoordinateFrame(mt);
}

void framePoint(CoordinateFrame m, double x, double y, double z){
	double p[4];
	double a[4];

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

void plataform(CoordinateFrame reference, int points, double bottomradius, double topradius){
	double ginner = 180 * 2 /((double)points);
	double rinner = 2 * M_PI /((double)points);

	CoordinateFrame db = mkCoordinateFrame(reference);
	CoordinateFrame ub = mkCoordinateFrame(reference);

	//frameScale(ub, 0.0, , 0.0 );
	frameTranslate(ub, 0.0, 1.0, 0.0);
	frameRotate(db, 180, 1.0, 0.0, 0.0);

	frameScale(db, bottomradius, 0.0, bottomradius);
	frameScale(ub, topradius, 0.0, topradius);

		for(int i = 0; i< points;i++){
			frameTriangle(ub, i* rinner, rinner);
			frameTriangle(db, i* rinner, rinner);
		}

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
		plataform(nw, points, f0, f1 );
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

void frameCircle(CoordinateFrame reference,int points){
	double inner = 2*M_PI/((double)points);
	CoordinateFrame mon = mkCoordinateFrame(reference);

    
	for(int i = 0; i< points;i++)
		frameTriangle(mon,i * inner, inner);

	unmkCoordinateFrame(mon);
}