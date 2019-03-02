#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

double lin(double dh){
	return (1 - dh);
}

double cuple(double dh){
	return sqrt( (1  - dh*dh) + 1e-15);
}

double cil(double dh){
	return 1;
}

void sphere(MonadWindow reference, double radius, int slices, int stacks) {
	MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
	monadStacker(nw,slices,stacks, cuple);
	monadRotate(nw,180,1.0,0.0,0.0);
	monadStacker(nw, slices,stacks, cuple);
	unmkMonadWindow(nw);
}

void cone(MonadWindow reference, double radius, double height, int slices, int stacks) {
    MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, height, radius);
    monadStacker(nw, slices, stacks, lin);
    unmkMonadWindow(nw);
}

void box(MonadWindow reference, int dx, int dy, int dz, int divisions) {
    MonadWindow nw = mkMonadWindow(reference);

    monadScale(reference,dx,dy,dz);
    monadCube(reference,divisions);

    unmkMonadWindow(nw);
}

int main(int argc, char **argv) {
	MonadWindow reference = mkMonadWindow();

    //box(reference, 2.0,2.0,2.0,2);

    monadStacker(reference,100, 100, cil);
    //monadCircle(reference,6);

    monadTrace(reference,"figure.xml","figure");

	unmkMonadWindow(reference);
	return 1;
}
