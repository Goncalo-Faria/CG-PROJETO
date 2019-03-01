#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

float lin(float dh, float h){
	return (h - dh)/h;
}

float cuple(float dh, float h){
	return sqrt( (h*h  - dh*dh) )/h;
}

void sphere(MonadWindow reference, float radius, int slices, int stacks) {
	MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
	monadStacker(nw,slices,stacks,1, cuple);
	monadRotate(nw,180,1.0,0.0,0.0);
	monadStacker(nw, slices,stacks,1, cuple);
	unmkMonadWindow(nw);
}

void cone(MonadWindow reference, float radius, float height, int slices, int stacks) {
    float normalizeHeight = height/radius;
    MonadWindow nw = mkMonadWindow(reference);
    monadScale(nw, radius, radius, radius);
    monadStacker(nw, slices, stacks, normalizeHeight, lin);
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

    box(reference, 2.0,2.0,2.0,2);

    monadTrace(reference,"figure.xml","figure");

	unmkMonadWindow(reference);
	return 1;
}
