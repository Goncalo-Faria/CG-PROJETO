#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

float lin(float dh){
	return (1 - dh);
}

float cuple(float dh){
	return sqrt( (1  - dh*dh) + 1e-15);
}

void sphere(MonadWindow reference, float radius, int slices, int stacks) {
	MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
	monadStacker(nw,slices,stacks, cuple);
	monadRotate(nw,180,1.0,0.0,0.0);
	monadStacker(nw, slices,stacks, cuple);
	unmkMonadWindow(nw);
}

void cone(MonadWindow reference, float radius, float height, int slices, int stacks) {
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
    cone(reference, 1.0,2.0,8,3);

    monadTrace(reference,"figure.xml","figure");

	unmkMonadWindow(reference);
	return 1;
}
