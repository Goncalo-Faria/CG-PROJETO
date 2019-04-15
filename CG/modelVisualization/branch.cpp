#if defined(_WIN32)
    #include "GL/glut.h"
#else
	#include <GLUT/glut.h>
#endif

#include "branch.h"
#include "assembler.h"
#include "../common/point.h"
#include "../common/matop.h"
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

typedef struct animation{
    vector< Branch > * subbranch;
    float period;
    vector< Point > * auxpoints;
    AnimationType type;
} * Animation;

typedef struct model{
    long starti;/* including */
    long endi;/* not including */
} *Model;

typedef struct transformation{
    vector< Branch > * subbranch;
    float **mat;
} * Transformation;

/* Transformation*/
Transformation mkTransformation( float ** mat){
    Transformation t = (Transformation)malloc( sizeof(struct transformation) );
    t->subbranch = new vector<Branch>();
    t->mat = mat;
    return t;
}

void unmkTransformation( Transformation t){
    for( Branch b : *(t->subbranch) )
        unmkBranch(b);

    delete t->subbranch;

    for(int i=0; i< 4; i++)
        free(t->mat[i]);
    free(t->mat);

    free(t);
}

void assemblerTransformate( Assembler ass, float ** mat){

    switch( view(ass)->type ){
        case EMPTY: {
            view(ass)->node = mkTransformation(mat);
            view(ass)->type = TRANSFORMATION;
            break;
        }

        case ANIMATION: {
            addDescendentAndGo(ass, mkBranch(mkTransformation(mat)));
            break;
        }

        case TRANSFORMATION: {
            Transformation tmp3 = (Transformation) view(ass)->node;
            float **tmp2 = matmul(tmp3->mat, mat);
            freeMat(tmp3->mat);
            tmp3->mat = tmp2;
            break;
        }

        default: {
            printf("error: models must be the terminal nodes\n");
            exit(2);
        }
    }

}

/* Model */

Model mkModel(long start, long end){
    Model m = (Model)malloc( sizeof(struct model) );
    m->starti = start;
    m->endi = end;
    return m;
}

void unmkModel(Model model){
    free(model);
}

void assemblerModelate( Assembler ass, float x, float y, float z){

    switch( view(ass)->type ){
        case EMPTY: {
            view(ass)->node = mkModel(assemblerNumberOfPoints(ass), assemblerNumberOfPoints(ass));
            view(ass)->type = MODEL;
            break;
        }

        case ANIMATION: {
            addDescendentAndGo(ass, mkBranch(mkModel(assemblerNumberOfPoints(ass), assemblerNumberOfPoints(ass))));
            break;
        }

        case TRANSFORMATION: {
            addDescendentAndGo(ass, mkBranch(mkModel(assemblerNumberOfPoints(ass), assemblerNumberOfPoints(ass))));
            break;
        }

        case MODEL : break;

        default : {
            printf("error: models must be the terminal nodes\n");
            exit(2);
        }
    }

    Model mo = (Model)view(ass)->node;

    Point p;
    p.p[0]=x;
    p.p[1]=y;
    p.p[2]=z;

    assemblerPoint(ass, p);

    mo->endi++;
}

/* Animation */

Animation mkAnimation(float period, vector<Point> * controlpoints, AnimationType type){
    Animation ani = (Animation)malloc( sizeof(struct animation) );
    ani->type = type;
    ani->period = period;
    ani->auxpoints = controlpoints;
    ani->subbranch = new vector< Branch >();
    return ani;
}

void unmkAnimation(Animation ani){
    for( Branch b : *(ani->subbranch) )
        unmkBranch(b);

    delete ani->subbranch;
    delete ani->auxpoints;
    
    free(ani);
}

void assemblerAnimate( Assembler ass, float period, vector<Point> * controlpoints, AnimationType type ){

    switch( view(ass)->type ){
        case EMPTY: {
            view(ass)->node = mkAnimation(period, controlpoints, type);
            view(ass)->type = ANIMATION;
            break;
        }

        case ANIMATION: {
            addDescendentAndGo(ass, mkBranch(mkAnimation(period, controlpoints, type)));
            break;
        }

        case TRANSFORMATION: {
            addDescendentAndGo(ass, mkBranch(mkAnimation(period, controlpoints, type)));
            break;
        }

        default : {
            printf("error: models must be the terminal nodes\n");
            exit(2);
        }
    }
}

/* Branch */

void addDescendent( Animation ani, Branch descendent ){
    ani->subbranch->emplace_back(descendent);
}

void addDescendent( Transformation t, Branch descendent ){
    t->subbranch->emplace_back(descendent);
}

Branch mkBranch( Transformation t ){
    Branch b =  (Branch)malloc( sizeof(struct branch) );
    b->type = TRANSFORMATION;
    b->node = t;
    return b; 
}

Branch mkBranch( Animation a ){
    Branch b = (Branch)malloc( sizeof(struct branch) );
    b->type = ANIMATION;
    b->node = a;
    return b;
}

Branch mkBranch( Model m ){
    Branch b = (Branch)malloc( sizeof(struct branch) );
    b->type = MODEL;
    b->node = m;
    return b;
}

void unmkBranch( struct branch b ){
    switch( b.type ){
        case EMPTY: {
            break;
        }
        case ANIMATION: {
            unmkAnimation((Animation) b.node);
            break;
        }
        case TRANSFORMATION: {
            unmkTransformation((Transformation) b.node);
            break;
        }
        case MODEL: {
            unmkModel((Model) b.node);
            break;
        }
    }
}

void unmkBranch( Branch b ){
    unmkBranch(*b);
    free(b);
}