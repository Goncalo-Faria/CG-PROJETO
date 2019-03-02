#include <math.h>
#include <cstdio>
#include <cstdlib>
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

void sphere(CoordinateFrame reference, double radius, int slices, int stacks) {
	CoordinateFrame nw = mkCoordinateFrame(reference);
    frameScale(nw, radius, radius, radius);
	frameStacker(nw,slices,stacks, cuple);
	frameRotate(nw,180,1.0,0.0,0.0);
	frameStacker(nw, slices,stacks, cuple);
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

int main(int argc, char **argv) {
	CoordinateFrame reference = mkCoordinateFrame();

    //box(reference, 2.0,2.0,2.0,2);

    cone(reference, 0.5,2.0,4,3);
    //sphere(reference,1.0,10,10);
    //frameCircle(reference,6);

    frameTrace(reference,"figure.xml","figure");

	unmkCoordinateFrame(reference);
	return 1;
}
