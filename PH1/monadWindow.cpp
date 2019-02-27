#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <list>
#include "monadWindow.h"
#include "tinyxml2.h"

using namespace tinyxml2; 

typedef struct point {
	float p[3];
} *Point;

typedef struct l {
	Point value;
	struct l * px;
} *L;

typedef struct monad { 
	float t[4][4];
	L back;
	L front;
	struct monad * ref;
} *MonadWindow;


Point mkPoint(float x, float y, float z){
	Point m = (Point) malloc( sizeof(struct point) );
	m->p[0] = x;
	m->p[1] = y;
	m->p[2] = z;
	return m;
}

void unmkPoint(Point p){
	free(p);
}


void print_trace(MonadWindow m){
	int count=0;
	char buffer[4000];

	XMLDocument* doc = new XMLDocument();
	XMLNode* major = doc->InsertEndChild( doc->NewElement( "figure" ) );
	XMLElement* triangle;
	XMLElement *point;
	XMLNode* ntraingle, *npoint;
	XMLText* text;
	for(L cur = m->front; cur ; cur = cur->px){

		if(!(count%3) ){
			triangle = doc->NewElement( "triangle" );
			ntraingle = major->InsertEndChild( triangle );
		}
		
		sprintf(buffer,"(%f,%f,%f)\n",cur->value->p[0],cur->value->p[1],cur->value->p[2]);
		
		point = doc->NewElement( "point" );
		npoint = ntraingle->InsertEndChild( point );

		point->SetAttribute("x",cur->value->p[0]);
		point->SetAttribute("y",cur->value->p[1]);
		point->SetAttribute("z",cur->value->p[2]);

		count++;
	}

	doc->SaveFile("figure.xml");

	delete doc;
}



MonadWindow mkMonadWindow(){
	MonadWindow m = (MonadWindow) malloc( sizeof(struct monad) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = (i==j);
	
	m->ref = NULL;
	m->back = NULL;
	m->front = NULL;

	return m;
}

void unmkMonadWindow(MonadWindow m){
	L aux;
	for(L cur = m->front; cur ; cur = aux){
		unmkPoint(cur->value);
		aux = cur->px;
		free(cur);
	}
	free(m);
}

MonadWindow mkMonadWindow(MonadWindow mold){
	MonadWindow m = (MonadWindow) malloc( sizeof(struct monad) );
	for(int i = 0; i< 4; i++)
		for( int j=0; j<4; j++)
			m->t[i][j] = mold->t[i][j];
	
	m->ref = mold;
	m->back = m->front = NULL;

	return m;
}


void monadReference(MonadWindow  m, Point p){
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

		monadReference(m->ref,p);
	}	
}

MonadWindow mkMonadWindowRx(float angle){
	MonadWindow m = mkMonadWindow();

	float rad = (M_PI/180.0)*angle;

	m->t[1][1] = cos(rad);
	m->t[1][2] = -sin(rad);
	m->t[2][1] = sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

MonadWindow mkMonadWindowRy(float angle){
	MonadWindow m = mkMonadWindow();

	float rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][2] = sin(rad);
	m->t[2][0] = -sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

MonadWindow mkMonadWindowRz(float angle){
	MonadWindow m = mkMonadWindow();

	float rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][1] = -sin(rad);
	m->t[1][0] = sin(rad);
	m->t[1][1] = cos(rad);

	return m;
}

void monadAgregate(MonadWindow a, MonadWindow b){
	float result[4][4];

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

void monadRotate(MonadWindow m, float angle, float vx, float vy, float vz){

	float rx = vx*angle;
	float ry = vy*angle;
	float rz = vz*angle;

	MonadWindow tmp = mkMonadWindowRx(rx);
	monadAgregate(m,tmp);
	unmkMonadWindow(tmp);

	tmp = mkMonadWindowRy(ry);
	monadAgregate(m,tmp);
	unmkMonadWindow(tmp);

	tmp = mkMonadWindowRz(rz);
	monadAgregate(m,tmp);
	unmkMonadWindow(tmp);


}

void monadTranslate(MonadWindow m, float x, float y, float z){
	MonadWindow mt = mkMonadWindow();
	
	mt->t[0][3] = x;
	mt->t[1][3] = y;
	mt->t[2][3] = z;

	monadAgregate(m, mt);

	unmkMonadWindow(mt);
}

void monadScale(MonadWindow m, float vx, float vy, float vz ){
	MonadWindow mt = mkMonadWindow();
	
	mt->t[0][0] = vx;
	mt->t[1][1] = vy;
	mt->t[2][2] = vz;

	monadAgregate(m, mt);

	unmkMonadWindow(mt);
}

void monadPoint(MonadWindow m, float x, float y, float z){
	float p[4];
	float a[4];

	p[0] = x;
	p[1] = y;
	p[2] = z;
	p[3] = 1;
	for(int i=0; i< 4; i++){
		a[i] = 0;
		for(int j=0; j< 4; j++)
			a[i] += m->t[i][j] * p[j];
	}
	//glColor3f((rand()%10000)/10000.0, (rand()%10000)/10000.0, (rand()%10000)/10000.0);

	//glVertex3f(a[0], a[1], a[2]);
	monadReference(m, mkPoint(a[0], a[1], a[2]));
	//printf("monadPoint(reference,%f,%f,%f);\n",a[0], a[1], a[2]);
}

void monadTriangle(MonadWindow m, float angle){
		monadPoint(m,0,0,0);
		monadTranslate(m,1,0,0);	
		monadPoint(m,0,0,0);
		monadTranslate(m,-1,0,0);
		monadRotate(m,180 - angle,0.0f,0.0f,1.0f);
		monadTranslate(m,1,0,0);	
		monadPoint(m,0,0,0);
		monadTranslate(m,-1,0,0);

}

void plataform(MonadWindow reference, int points,float h, float bottomradius, float topradius){
	float tangle = (float)((points - 2) * 180);
	float angle = tangle/points;
	float otherside = sqrt(2*1.0-2*1.0*cos(180-angle));
	MonadWindow db = mkMonadWindow(reference);
	MonadWindow ub = mkMonadWindow(reference);

	monadTranslate(ub, 0.0, 0.0, -h);
	monadRotate(ub, 180, 1.0, 0.0, 0.0);

	monadScale(db, bottomradius, bottomradius ,1 );
	monadScale(ub, topradius,topradius ,1 );
	//glBegin(GL_TRIANGLES);

		for(int i = 0; i< points;i++){
			monadTriangle(ub, angle);
			monadTriangle(db, angle);
		}

		monadRotate(ub, 180, 1.0, 0.0, 0.0);
		monadTranslate(ub,1.0,0.0,0.0);
		monadTranslate(db,1.0,0.0,0.0);

		
		for( int i = 0; i<points; i++ ){
			
			MonadWindow walker;
			

			for(int j = 0; j< 2; j++ ){
				if( j%2 == 0 ){
					walker = ub;
				}else{
					walker = db;
				}
				monadPoint(ub,0,0,0);
				monadPoint(db,0,0,0);

				monadTranslate(walker,-1.0,0.0,0.0);
				monadRotate(walker,-(180 - angle),0.0f,0.0f,1.0f);
				monadTranslate(walker,1.0,0.0,0.0);
				monadPoint(walker,0.0,0.0,0);
			}

		}
		

		unmkMonadWindow(db);
		unmkMonadWindow(ub);
	//glEnd();

}

void stacker(MonadWindow reference,int points, int stacks, float h, float (*f)(float,float) ){
	float dh = h/stacks;
	float currenth = dh;
	float f0 = f(0.0,h);
	float f1;
	MonadWindow nw = mkMonadWindow(reference);
	for(int i = 0; i < stacks; i++){
		f1 = f(currenth,h);
		plataform(nw, points, dh, f0, f1 );
		monadTranslate(nw,0.0f,0.0f, -dh);
		f0 = f1;
		currenth += dh;
	}

	unmkMonadWindow(nw);
}