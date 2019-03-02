#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "monadWindow.h"

#define back(X,Y) unmkMonadWindow(X);X = mkMonadWindow(Y)

void circle(MonadWindow reference,int points){
	auto tangle = (double)((points - 2) * 180);
	double angle = tangle/points;
	MonadWindow mon = mkMonadWindow(reference);

	for(int i = 0; i< points;i++)
		monadTriangle(mon,angle);

	unmkMonadWindow(mon);
}

double lin(double dh, double h){
	return (h - dh)/h;
}

double cuple(double dh, double h){
	return sqrt( (h*h  - dh*dh) )/h;
}

void sphere(MonadWindow reference, double radius, int slices, int stacks) {
	MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
	stacker(nw,slices,stacks,1, cuple);
	monadRotate(nw,180,1.0,0.0,0.0);
	stacker(nw, slices,stacks,1, cuple);
	unmkMonadWindow(nw);
}

void cone(MonadWindow reference, double radius, double height, int slices, int stacks) {
    double normalizeHeight = height/radius;
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

    monadScale(nw, 1.0/(double)divisions, 1.0/(double)divisions, 1.0/(double)divisions);
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

void notEnoughArguments() {
	std::cout << "Not enough arguments";
	exit(1);
}

int main(int argc, char **argv) {
	if(argc == 1) notEnoughArguments();

	MonadWindow reference = mkMonadWindow();
	const char * filename = "filename.xml";

	if(strcmp(argv[1], "plane") == 0) {
	    plane(reference);
        filename = argv[2];
	} else if(strcmp(argv[1], "box") == 0) {
	    // x , y , z , divisions(optional)
        if(argc < 5) notEnoughArguments();
        if(argc == 5) {
            box(reference, atof(argv[2]),atof(argv[3]),atof(argv[4]),1);
            filename = argv[5];
        } else {
            box(reference, atof(argv[2]),atof(argv[3]),atof(argv[4]),atoi(argv[5]));
            filename = argv[6];
        }
    } else if(strcmp(argv[1], "sphere") == 0) {
		if(argc < 5) notEnoughArguments();
		//radius, slices, stacks
		sphere(reference, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
		filename = argv[5];
	} else if(strcmp(argv[1], "cone") == 0) {
		if(argc < 6) notEnoughArguments();
		//bottom radius, height, slices and stacks
		cone(reference, atof(argv[2]), atof(argv[3]), atoi(argv[4]), atoi(argv[5]));
		filename = argv[6];
	} else {
		std::cout << "Unknown model";
		return 2;
	}

	print_trace(reference,filename,"figure");
    unmkMonadWindow(reference);

	return 0;
}
