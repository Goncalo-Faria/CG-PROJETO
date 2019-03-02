#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"
#include "tinyxml2.h"

#define back(X,Y) unmkMonadWindow(X);X = mkMonadWindow(Y)

using namespace tinyxml2; 

typedef struct point {
	double p[3];
} *Point;

typedef struct l {
	Point value;
	struct l * px;
} *L;

typedef struct monad { 
	double t[4][4];
	L back;
	L front;
	struct monad * ref;
} *MonadWindow;


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


void monadTrace(MonadWindow m, char* filename, char* figure){
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

MonadWindow mkMonadWindowRx(double angle){
	MonadWindow m = mkMonadWindow();

	double rad = (M_PI/180.0)*angle;

	m->t[1][1] = cos(rad);
	m->t[1][2] = -sin(rad);
	m->t[2][1] = sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

MonadWindow mkMonadWindowRy(double angle){
	MonadWindow m = mkMonadWindow();

	double rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][2] = sin(rad);
	m->t[2][0] = -sin(rad);
	m->t[2][2] = cos(rad);

	return m;
}

MonadWindow mkMonadWindowRz(double angle){
	MonadWindow m = mkMonadWindow();

	double rad = (M_PI/180.0)*angle;

	m->t[0][0] = cos(rad);
	m->t[0][1] = -sin(rad);
	m->t[1][0] = sin(rad);
	m->t[1][1] = cos(rad);

	return m;
}

void monadAgregate(MonadWindow a, MonadWindow b){
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

void monadRotate(MonadWindow m, double angle, double vx, double vy, double vz){

	double rx = vx*angle;
	double ry = vy*angle;
	double rz = vz*angle;

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

void monadTranslate(MonadWindow m, double x, double y, double z){
	MonadWindow mt = mkMonadWindow();
	
	mt->t[0][3] = x;
	mt->t[1][3] = y;
	mt->t[2][3] = z;

	monadAgregate(m, mt);

	unmkMonadWindow(mt);
}

void monadScale(MonadWindow m, double vx, double vy, double vz ){
	MonadWindow mt = mkMonadWindow();
	
	mt->t[0][0] = vx;
	mt->t[1][1] = vy;
	mt->t[2][2] = vz;

	monadAgregate(m, mt);

	unmkMonadWindow(mt);
}

void monadPoint(MonadWindow m, double x, double y, double z){
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

	monadReference(m, mkPoint(a[0], a[1], a[2]));
}

void monadTriangle(MonadWindow m, double angle, double difs){

	monadPoint(m, cos(angle), 0,sin(angle));
	monadPoint(m, 0, 0, 0);
	monadPoint(m, cos(angle+difs), 0,sin(angle+difs));
	
}

void plataform(MonadWindow reference, int points, double bottomradius, double topradius){
	double ginner = 180 * 2 /((double)points);
	double rinner = 2 * M_PI /((double)points);

	MonadWindow db = mkMonadWindow(reference);
	MonadWindow ub = mkMonadWindow(reference);

	//monadScale(ub, 0.0, , 0.0 );
	monadTranslate(ub, 0.0, 1.0, 0.0);
	monadRotate(db, 180, 1.0, 0.0, 0.0);

	monadScale(db, bottomradius, 0.0, bottomradius);
	monadScale(ub, topradius, 0.0, topradius);

		for(int i = 0; i< points;i++){
			monadTriangle(ub, i* rinner, rinner);
			monadTriangle(db, i* rinner, rinner);
		}

		monadRotate(db, 180, 1.0, 0.0, 0.0);

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
				monadPoint(db,0,0,0);
				monadPoint(ub,0,0,0);

				monadTranslate(walker,-1.0,0.0,0.0);
				monadRotate(walker,-ginner,0.0f,1.0f,0.0f);
				monadTranslate(walker,1.0,0.0,0.0);
				monadPoint(walker,0.0,0.0,0);
			}
		}
		unmkMonadWindow(db);
		unmkMonadWindow(ub);

}

void monadStacker(MonadWindow reference,int points, int stacks, double (*f)(double) ){
	double dh = 1.0/stacks;
	double currenth = dh;
	double f0 = f(0.0);
	double f1;
	MonadWindow nw = mkMonadWindow(reference);
	monadScale(nw,1.0f,dh,1.0f);
	for(int i = 0; i < stacks; i++){
		f1 = f(currenth);
		plataform(nw, points, f0, f1 );
		monadTranslate(nw,0.0f,1.0f,0.0f);
		f0 = f1;
		currenth += dh;
	}

	unmkMonadWindow(nw);
}

void plane(MonadWindow reference) {
    MonadWindow nw = mkMonadWindow(reference);

    monadPoint(nw,-0.5,0,-0.5);
    monadPoint(nw,-0.5,0,0.5);
    monadPoint(nw,0.5,0,-0.5);

    monadPoint(nw,-0.5,0,0.5);
    monadPoint(nw,0.5,0,0.5);
    monadPoint(nw,0.5,0,-0.5);

    unmkMonadWindow(nw);
}

void monadHyperplane(MonadWindow reference, int divisions){
    MonadWindow nw = mkMonadWindow(reference);

    monadScale(nw, 1.0/(double)divisions, 1.0/(double)divisions, 1.0/(double)divisions);
    monadTranslate(nw, -0.5*(divisions-1), 0.0, -0.5*(divisions-1));

    for(int i = 0; i < divisions; i++){
        for(int j = 0; j < divisions; j++){
            plane(nw);
            monadTranslate(nw,1,0.0,0.0);
        }
        monadTranslate(nw,-divisions,0.0,1.0);
    }

    unmkMonadWindow(nw);
}

void monadCube(MonadWindow reference, int divisions) {
    MonadWindow nw = mkMonadWindow(reference);

	for(int i = 0; i < 3; i++){
		switch(i){
			case 1: monadRotate(nw,90,1,0,0);break;
			case 2: monadRotate(nw,90,0.0,0.0,-1.0);break;
		}

    	monadTranslate(nw, 0.0, 0.5, 0.0);
    	monadHyperplane(nw, divisions);

    	monadTranslate(nw, 0.0, -1.0, 0.0);
    	monadRotate(nw,180,1.0,0.0,0.0);
    	monadHyperplane(nw, divisions);

    	back(nw,reference);
	}

    unmkMonadWindow(nw);
}

void monadCircle(MonadWindow reference,int points){
	double inner = 2*M_PI/((double)points);
	MonadWindow mon = mkMonadWindow(reference);

    
	for(int i = 0; i< points;i++)
		monadTriangle(mon,i * inner, inner);

	unmkMonadWindow(mon);
}