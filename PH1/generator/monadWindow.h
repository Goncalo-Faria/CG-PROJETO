#ifndef monadWindow_h
#define monadWindow_h

typedef struct monad *MonadWindow;

MonadWindow mkMonadWindow();

MonadWindow mkMonadWindow(MonadWindow mold);

void unmkMonadWindow(MonadWindow m);

void monadRotate(MonadWindow m, double angle, double vx, double vy, double vz);

void monadAgregate(MonadWindow a, MonadWindow b);

void monadTranslate(MonadWindow m, double x, double y, double z);

void monadScale(MonadWindow m, double vx, double vy, double vz );

void monadPoint(MonadWindow m, double x, double y, double z);

void monadTriangle(MonadWindow m, double angle);

void stacker(MonadWindow reference,int points, int stacks, double h, double (*f)(double,double) );

void platform(MonadWindow reference, int points,double h, double bottomRadius, double topRadius);

void print_trace(MonadWindow m, const char* filename, char* figure);

#endif
