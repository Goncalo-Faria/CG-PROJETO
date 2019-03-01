#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

#define back(X,Y) unmkMonadWindow(X);X = mkMonadWindow(Y)

void circle(MonadWindow reference,int points){
	float tangle = (float)((points - 2) * 180);
	float angle = tangle/points;
	MonadWindow mon = mkMonadWindow(reference);

	for(int i = 0; i< points;i++)
		monadTriangle(mon,angle);

	unmkMonadWindow(mon);
}

float lin(float dh, float h){
	return (h - dh)/h;
}

float cuple(float dh, float h){
	return sqrt( (h*h  - dh*dh) )/h;
}

void sphere(MonadWindow reference, float radius, int slices, int stacks) {
	MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
	stacker(nw,slices,stacks,1, cuple);
	monadRotate(nw,180,1.0,0.0,0.0);
	stacker(nw, slices,stacks,1, cuple);
	unmkMonadWindow(nw);
}

void cone(MonadWindow reference, float radius, float height, int slices, int stacks) {
    float normalizeHeight = height/radius;
    MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
    stacker(nw, slices, stacks, normalizeHeight, lin);
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

void hyperplane(MonadWindow reference, int divisions){
    MonadWindow nw = mkMonadWindow(reference);

    monadScale(nw, 1.0/(float)divisions, 1.0/(float)divisions, 1.0/(float)divisions);
    monadTranslate(nw, -0.5*(divisions-1), 0.0, -0.5*(divisions-1));

    for(int i = 0; i < divisions; i++){
        plane(nw);
        for(int j = 0; j < (divisions - 1); j++){
            monadTranslate(nw,1,0.0,0.0);
            plane(nw);
        }
        monadTranslate(nw,-divisions+1,0.0,1.0);
    }

    unmkMonadWindow(nw);
}

void cubeCube(MonadWindow reference, int divisions) {
    MonadWindow nw = mkMonadWindow(reference);

    monadTranslate(nw, 0.0, -0.5, 0.0);
    monadRotate(nw, 180, 1.0, 0.0, 0.0);
    hyperplane(nw, divisions);
    
    back(nw,reference);
    monadTranslate(nw, 0.0, 0.5, 0.0);
    hyperplane(nw, divisions);
    
    back(nw,reference);
    monadRotate(nw,90,1,0,0);
    monadTranslate(nw, 0.0, 0.5, 0.0);
    hyperplane(nw, divisions);

    monadTranslate(nw, 0.0, -1.0, 0.0);
    monadRotate(nw,180,1.0,0.0,0.0);
    hyperplane(nw, divisions);

    back(nw,reference);
    monadRotate(nw,90,0.0,0.0,-1.0);
    monadTranslate(nw, 0.0, 0.5, 0.0);
    hyperplane(nw, divisions);

    monadTranslate(nw, 0.0, -1.0, 0.0);
    monadRotate(nw,180,0,0,-1);
    hyperplane(nw, divisions);

    unmkMonadWindow(nw);
}

void box(MonadWindow reference, int dx, int dy, int dz, int divisions) {
    MonadWindow nw = mkMonadWindow(reference);

    monadScale(reference,dx,dy,dz);
    cubeCube(reference,divisions);

    unmkMonadWindow(nw);
}

int main(int argc, char **argv) {
	MonadWindow reference = mkMonadWindow();

    box(reference, 2.0,2.0,2.0,2);

    print_trace(reference,"figure.xml","figure");

	unmkMonadWindow(reference);
	return 1;
}
