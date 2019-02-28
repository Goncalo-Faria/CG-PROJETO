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

float lin( float dh, float h){
	return (h - dh)/h;
}

float cuple( float dh, float h ){
	return sqrt( (h*h  - dh*dh) )/h;
}

void sphere(MonadWindow reference,int points, int stacks ){
	MonadWindow nw = mkMonadWindow(reference);
	stacker(nw,points,stacks,1, cuple );
	monadRotate(nw,180,1.0,0.0,0.0);
	stacker(nw, points,stacks,1, cuple );
	//monadRotate(nw,180,0.0,1.0,0.0);
	unmkMonadWindow(nw);
}

int main(int argc, char **argv) {

	MonadWindow reference = mkMonadWindow();
	stacker(reference,3,1,1,lin);
	print_trace(reference,"figure.xml","figure");
	
	unmkMonadWindow(reference);
	return 1;
}
