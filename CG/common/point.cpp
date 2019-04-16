#include "point.h"
#include <stdlib.h>
#include <cmath>

Point* mkPoint(float x, float y, float z){
	Point* m = (Point*) malloc( sizeof(struct point) );
	m->p[0] = x;
	m->p[1] = y;
	m->p[2] = z;
	return m;
}

Point pointNormalize(Point point){

    float l = sqrt(point.p[0]*point.p[0] + point.p[1] * point.p[1] + point.p[2] * point.p[2]);

    point.p[0] = point.p[0]/l;
    point.p[1] = point.p[1]/l;
    point.p[2] = point.p[2]/l;

    return point;
}

void unmkPoint(Point* p){
	free(p);
}