#ifndef coordinateFrame_h
#define coordinateFrame_h

typedef struct frame *CoordinateFrame;

CoordinateFrame mkCoordinateFrame();

CoordinateFrame mkCoordinateFrame(CoordinateFrame mold);

void unmkCoordinateFrame(CoordinateFrame m);

void frameRotate(CoordinateFrame m, double angle, double vx, double vy, double vz);

void frameTranslate(CoordinateFrame m, double x, double y, double z);

void frameScale(CoordinateFrame m, double vx, double vy, double vz );

void framePoint(CoordinateFrame m, double x, double y, double z);

void frameTriangle(CoordinateFrame m, double angle, double difs);

void frameStacker(CoordinateFrame reference, int points, int stacks, double (*f)(double));

void frameHyperplane(CoordinateFrame reference, int divisions);

void frameCube(CoordinateFrame reference, int divisions);

void frameRegularPolygon(CoordinateFrame reference,int points);

void frameTrace(CoordinateFrame m, char* filename, char* figure);

#endif
