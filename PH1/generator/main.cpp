#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "coordinateFrame.h"

double lin(double dh){
	return (1 - dh);
}

double cuple(double dh){
	return sqrt( (1  - dh*dh) + 1e-15);
}

double cil(double dh){
	return 1;
}

double smartshephere(double dh){
	if( dh <= 0.5)
		return sqrt( 1 - pow(1- 2*dh,2));
	else
		return sqrt( 1 - pow(2*dh - 1,2));
}

/* legacy
void sphere(CoordinateFrame reference, double radius, int slices, int stacks) {
	CoordinateFrame nw = mkCoordinateFrame(reference);
    frameScale(nw, radius, radius, radius);
	frameStacker(nw,slices,stacks, cuple);
	frameRotate(nw,180,1.0,0.0,0.0);
	frameStacker(nw, slices,stacks, cuple);
	unmkCoordinateFrame(nw);
}
*/

void sphere(CoordinateFrame reference, double radius, int slices, int stacks) {
	CoordinateFrame nw = mkCoordinateFrame(reference);
    frameScale(nw, radius, 2*radius, radius);
	frameStacker(nw,slices,stacks, smartshephere);
	unmkCoordinateFrame(nw);
}


void cone(CoordinateFrame reference, double radius, double height, int slices, int stacks) {
    CoordinateFrame nw = mkCoordinateFrame(reference);
    frameScale(nw, radius, height, radius);
    frameStacker(nw, slices, stacks, lin);
    unmkCoordinateFrame(nw);
}

void box(CoordinateFrame reference, int dx, int dy, int dz, int divisions) {
    CoordinateFrame nw = mkCoordinateFrame(reference);

    frameScale(reference,dx,dy,dz);
    frameCube(reference,divisions);

    unmkCoordinateFrame(nw);
}


void notEnoughArguments() {
	std::cout << "Not enough arguments";
	exit(1);
}

int main(int argc, char **argv) {
	if(argc == 1) notEnoughArguments();

	CoordinateFrame reference = mkCoordinateFrame();
	char * filename = "filename.xml";

	if(strcmp(argv[1], "plane") == 0) {
	    frameHyperplane(reference,1);
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
		sphere(reference, atof(argv[2]), atoi(argv[3]), atoi(argv[4]));
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

	frameTrace(reference,filename,"figure");
  unmkCoordinateFrame(reference);

	return 0;
}
