#include "bazierPatch.h"
#include <stdlib.h>
#include <vector>
#include <array>

using namespace std;

typedef struct patch {
    int numPatches;
    vector< array<int, 16> > indices;
    int numCtrlPoints; 
    vector< Point > ctrlPoints;
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

    for( int i = 0; i<patch->numPatches; i++){
        array<int, 16> v;
        for( int j=0; j<(3+1)*(3+1); j++)
            fscanf(source,"%d",&v[j]);
        
        patch->indices.push_back(v);
    }

    fscanf(source,"%d",&(patch->numCtrlPoints));

    for( int i = 0; i< patch->numCtrlPoints; i++ ){
        double v[3];
        for(int j = 0 ; j < 3; j++){
            float tmp1;
            fscanf(source,"%f",&tmp1);
            v[j] = (double) tmp1;    
        }

        patch->ctrlPoints.push_back(*mkPoint(v[0],v[1],v[2]));
    }

    return patch;
}

void unmkBazierPatch(BazierPatch m){
    free(m);
}
