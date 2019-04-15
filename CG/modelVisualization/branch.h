#ifndef branch_h
#define branch_h

typedef unsigned char BranchType;
    #define EMPTY 0    
    #define FAKE 1
    #define ANIMATION 2
    #define TRANSFORMATION 3
    #define MODEL 4

typedef unsigned char AnimationType;
    #define CURVED_TRANSLATION 9
    #define ROTATION 8

typedef struct branch{
    BranchType type;
    void * node;
} *Branch;

typedef struct transformation *Transformation;
typedef struct animation *Animation;
typedef struct model *Model;

#include "assembler.h"
#include <vector>
#include "../common/point.h"

Branch mkBranch( Transformation t );
Branch mkBranch( Animation a );
Branch mkBranch( Model m );

void unmkBranch( struct branch b );
void unmkBranch( Branch b );

void addDescendent( Animation ani, Branch descendent );
void addDescendent( Transformation t, Branch descendent );

Transformation mkTransformation(float ** mat);
void unmkTransformation( Transformation t);
void assemblerTransformate( Assembler ass, float ** mat);

Model mkModel(long start, long end);
void unmkModel(Model model);
void assemblerModelate( Assembler ass, float x, float y, float z);

Animation mkAnimation(float period, std::vector<Point> * controlpoints, AnimationType type);
void unmkAnimation(Animation ani);
void assemblerAnimate( Assembler ass, float period, std::vector<Point> * controlpoints, AnimationType type );

#endif