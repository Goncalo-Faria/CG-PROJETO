#ifndef point_h
#define point_h

typedef struct point { double p[3]; } Point;

Point* mkPoint(double x, double y, double z);
void unmkPoint(Point* p);

#endif