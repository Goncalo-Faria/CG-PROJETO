#include "bazierPatch.h"
#include <stdlib.h>
#include <vector>
#include <list>
#include <array>


using namespace std;

typedef struct patch {
    int numPatches;
    int ** indices;
    int numCtrlPoints; 
    Point * ctrlPoints;
}*BazierPatch;

BazierPatch mkBazierPatch( FILE* source );
void unmkBazierPatch(BazierPatch m);

BazierPatch mkBazierPatch( FILE* source ){
    /* parse filetype::
    degree(n,m) :- (n+1)(m+1) control points. 
    bicubic patches :- degree(3,3)

    # number of patches 
    [indice]*
    # number of control points
    [points]*
    
    */
    BazierPatch patch = (BazierPatch)malloc(sizeof(struct patch));
    fscanf(source,"%d",&(patch->numPatches));

    //printf("%d \n",patch->numPatches);

    int ** v = (int**)malloc(sizeof(int*)*patch->numPatches);


    for( int i = 0; i<patch->numPatches; i++){

        v[i] = (int*)malloc( sizeof(int) * 16 );
        int tmp;
        
        for( int j=0; j<(3+1)*(3+1)-1; j++){
            fscanf(source, "%d, ",&tmp);

            v[i][j] = tmp;

            //printf("%d, ",tmp);
        }

        fscanf(source,"%d",&tmp);
        
        v[i][15] = tmp;

        //printf("%d \n",tmp);
        

    }

    patch->indices = v;

        //printf("go\n");

    fscanf(source,"%d",&(patch->numCtrlPoints));

    patch->ctrlPoints = (Point*) malloc( sizeof(Point) * patch->numCtrlPoints );

    for( int i = 0; i< patch->numCtrlPoints; i++ ){
        double pv[3];
        float tmp1;
        for(int j = 0 ; j < 3 - 1; j++){
            fscanf(source,"%f, ",&tmp1);
            pv[j] = (double) tmp1;    

            //printf(" %d | ",j);
        }

        fscanf(source,"%f",&tmp1);
        pv[2] = (double) tmp1; 

        Point* point =  mkPoint(pv[0],pv[1],pv[2]);

        //printf(" %f :: %f :: %f \n", (float) pv[0], (float) pv[1], (float) pv[2]);
        
        //printf("%d \n",i);

        patch->ctrlPoints[i] = *point;

        //printf(" %f :: %f :: %f \n", (float)patch->ctrlPoints[i].p[0], (float)patch->ctrlPoints[i].p[1], (float)patch->ctrlPoints[i].p[2]);


        unmkPoint(point);

    }

        //printf("its working \n");

    return patch;
}

void unmkBazierPatch(BazierPatch m){
    free(m);
}

Point * getPatch(BazierPatch bazier, int ind){
    
    Point * r = (Point*)malloc(sizeof(Point) * 16);
    //printf(" aftermalloc : ");
    for(int i= 0; i< 16; i++)
        r[i] = bazier->ctrlPoints[bazier->indices[ind][i]];

    //printf(" end: \n");

    return r;
}

int getNumPatches(BazierPatch bazier){
    //printf(" fail : ");
    //printf("%d \n",bazier->numPatches);
    return bazier->numPatches;
}
