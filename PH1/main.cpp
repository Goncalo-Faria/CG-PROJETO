#include <math.h>
#include <cstdio>
#include <cstdlib>
#include "monadWindow.h"

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
    monadTranslate(nw, -0.5, 0.0, -0.5);

    monadPoint(nw, 1.0, 0.0, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
    monadPoint(nw, 1.0, 0.0, 1.0);

    monadPoint(nw, 0.0, 0.0, 1.0);
    monadPoint(nw, 1.0, 0.0, 1.0);
    monadPoint(nw, 0.0, 0.0, 0.0);

    unmkMonadWindow(nw);
}

void planeXZ(MonadWindow reference, float lengthX, float lenghtY, float lenghtZ) {
    MonadWindow nw = mkMonadWindow(reference);
    monadTranslate(nw, -lengthX/2, 0.0, -lenghtZ/2);

    monadPoint(nw, lengthX, 0.0, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
    monadPoint(nw, lengthX, 0.0, lenghtZ);

    monadPoint(nw, 0.0, 0.0, lenghtZ);
    monadPoint(nw, lengthX, 0.0, lenghtZ);
    monadPoint(nw, 0.0, 0.0, 0.0);

    unmkMonadWindow(nw);
}

void planeXY(MonadWindow reference, float lengthX, float lengthY, float lenghtZ) {
    MonadWindow nw = mkMonadWindow(reference);
    monadTranslate(nw, -lengthX/2, 0.0, -lenghtZ/2);

    monadPoint(nw, lengthX, 0.0, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
    monadPoint(nw, lengthX, lengthY, 0.0);

    monadPoint(nw, 0.0, lengthY, 0.0);
    monadPoint(nw, lengthX, lengthY, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
}

void planeYZ(MonadWindow reference, float lengthX, float lengthY, float lenghtZ) {
    MonadWindow nw = mkMonadWindow(reference);
    monadTranslate(nw, -lengthX/2, 0.0, -lenghtZ/2);

    monadPoint(nw, 0.0, lengthY, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
    monadPoint(nw, 0.0, lengthY, lenghtZ);

    monadPoint(nw, 0.0, 0.0, lenghtZ);
    monadPoint(nw, 0.0, lengthY, lenghtZ);
    monadPoint(nw, 0.0, 0.0, 0.0);
}

void planeMod(MonadWindow reference, float lengthX, float lengthY, float lenghtZ, int nx, int ny, int nz) {
    MonadWindow nw = mkMonadWindow(reference);
    monadTranslate(nw, 0.0, 0.0, 0.0);

    monadPoint(nw, 0.0, lengthY, 0.0);
    monadPoint(nw, 0.0, 0.0, 0.0);
    monadPoint(nw, 0.0, lengthY, lenghtZ);

    monadPoint(nw, 0.0, 0.0, lenghtZ);
    monadPoint(nw, 0.0, lengthY, lenghtZ);
    monadPoint(nw, 0.0, 0.0, 0.0);
}

void box(MonadWindow reference, float x, float y, float z, int divisions) {
    MonadWindow nw = mkMonadWindow(reference);
    monadTranslate(nw, -0.5, 0.0, -0.5);

    monadRotate(nw, 180, 1.0, 0.0, 1.0);
    planeXZ(nw, x, y, z); //face tem que estar ao contrario

    monadTranslate(nw, 0.0, y, 0.0);
    planeXZ(nw, x, y, z);
    monadTranslate(nw, 0.0, -y, 0.0);

    planeXY(nw, x, y, z);

    monadTranslate(nw, 0.0, 0.0, z); //face tem que estar ao contrario
    planeXY(nw, x, y, z);
    monadTranslate(nw, 0.0, 0.0, -z);

    planeYZ(nw, x, y, z);

    monadTranslate(nw, x, 0.0, 0.0); //face tem que estar ao contrario
    planeYZ(nw, x, y, z);
    monadTranslate(nw, -x, 0.0, 0.0);

    /*
     *  nr de divisoes = nr de planos - 1


    for(int i = 0; i < divisions - 1; i++) {
        //planosXZ
        double novoX = x / (divisions - 1);
        double novoY = y / (divisions - 1);

        planeXZ(nw, y, z);
    }*/

}

int main(int argc, char **argv) {
	MonadWindow reference = mkMonadWindow();
    box(reference, 2.0, 2.0, 2.0, 0);
    print_trace(reference,"figure.xml","figure");

	unmkMonadWindow(reference);
	return 1;
}
