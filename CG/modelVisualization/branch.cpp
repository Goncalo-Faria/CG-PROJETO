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

#define max(A,B) (A > B) ? A : B;
#define min(A,B) (A < B) ? A : B;

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

void applyTransformation( Transformation transformation, Point* outgoing, long start, long end){

    Point point;
    float p[4];

    p[3] = 1;

    for( long s = start; s< end; s++ ){
        p[0] = outgoing[s].p[0];
        p[1] = outgoing[s].p[1];
        p[2] = outgoing[s].p[2];

        for(int i=0; i< 3; i++){
            point.p[i] = 0;
            for(int j=0; j< 4; j++)
                point.p[i] += transformation->mat[i][j] * p[j];
        }

        outgoing[s] = point;
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

void applyAnimation( Animation animation, Point* outgoing, long start, long end){


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

Model recInterpret(Branch b, vector<Point>* inpoints, Point* outpoints){

    switch( b->type ){

        case ANIMATION: {
            // printf("Animation\n");
            Animation ani = (Animation)b->node;
            long minv = inpoints->size();
            long maxv = -1;

            for(Branch desbranch : *(ani->subbranch) ) {
                Model mod = recInterpret(desbranch, inpoints, outpoints);
                minv = min(minv,mod->starti);
                maxv = max(maxv,mod->endi);
                unmkModel(mod);
            }

            applyAnimation(ani, outpoints, minv, maxv);

            if( minv > maxv )
                return mkModel(minv,maxv);
            else
                return mkModel(0,0);

            break;
        }

        case TRANSFORMATION: {
            //printf("Transformation\n");
            Transformation t = (Transformation)b->node;
            long minv = inpoints->size();
            long maxv = -1;

            for(Branch desbranch : *(t->subbranch) ) {
                Model mod = recInterpret(desbranch, inpoints, outpoints);
                minv = min(minv,mod->starti);
                maxv = max(maxv,mod->endi);
                unmkModel(mod);
            }

            applyTransformation(t, outpoints, minv, maxv);

            if( minv < maxv )
                return mkModel(minv,maxv);
            else
                return mkModel(0,0);

            break;
        }

        case MODEL: {
            //printf("Model\n");
            Model mo = (Model)b->node;
            //printf(" %ld - %ld   \n", mo->starti, mo->endi);
            for(long i = mo->starti; i < mo->endi; i++)
                outpoints[i] = inpoints->at(i);

            return mkModel(mo->starti,mo->endi);
        }

        default:
            //printf("sometype \n");
            return mkModel(inpoints->size(),-1);
    }
}

void branchInterpret(Branch b, vector<Point>* inpoints, Point* outpoints){
    unmkModel(recInterpret(b, inpoints, outpoints));
}

void branchOptimizeTransf( Branch b ){
    switch( b->type ){
        case EMPTY: {
            break;
        }
        case ANIMATION: {
            Animation t = (Animation)b->node;
            for(long i=0; i < t->subbranch->size(); i++) {

                Branch tmpb = t->subbranch->at(i);

                branchOptimizeTransf(tmpb);

                if( tmpb->type == MODEL ){
                    Model moo = (Model)tmpb->node;
                    if(moo->endi == moo->starti){
                        //printf("chop\n");
                        /* eliminar este ramo*/
                        Branch* hidbuff = t->subbranch->data();
                        hidbuff[i] = hidbuff[ t->subbranch->size() - 1 ];
                        t->subbranch->pop_back();
                        unmkModel(moo);
                        free(tmpb);
                    }
                }
            }

            if( t->subbranch->empty() ){
                //printf("chop\n");
                /*tranformações que não afetam ninguêm são eliminadas*/
                unmkAnimation(t);
                b->type = MODEL;
                b->node = mkModel(0L,0L);
            }


            break;
        }
        case TRANSFORMATION: {
            Transformation t = (Transformation)b->node;

            for(long i=0; i < t->subbranch->size(); i++) {

                Branch tmpb = t->subbranch->at(i);

                branchOptimizeTransf(tmpb);

                if( tmpb->type == MODEL ){
                    Model moo = (Model)tmpb->node;
                    if(moo->endi == moo->starti){
                        //printf("chop\n");
                        /* eliminar este ramo*/
                        Branch* hidbuff = t->subbranch->data();
                        hidbuff[i] = hidbuff[ t->subbranch->size() - 1 ];
                        t->subbranch->pop_back();
                        unmkModel(moo);
                        free(tmpb);
                    }
                }
            }

            if( t->subbranch->size() == 1 && t->subbranch->at(0)->type == TRANSFORMATION ){
                /* transformações seguidas são compostas*/
                //printf("chop\n");
                Transformation ut = (Transformation) t->subbranch->at(0)->node;

                float** nmat = matmul(t->mat, ut->mat); /* a transformação torna-se a composição das duas transformações*/

                freeMat(t->mat);/* apaga matrizes antigas*/
                freeMat(ut->mat);/* apaga matrizes antigas*/

                free(t->subbranch->at(0));/* apaga o ramo do filho*/

                delete t->subbranch;/* apaga a lista de filhos*/


                t->mat = nmat;/* a nova matriz é a composição*/
                t->subbranch = ut->subbranch;/* netos tornam-se filhos*/
                free(ut);/* apaga memória do filho*/

            }else if( t->subbranch->empty() ){
                //printf("chop\n");
                /*tranformações que não afetam ninguêm são eliminadas*/
                unmkTransformation(t);
                b->type = MODEL;
                b->node = mkModel(0L,0L);
            }

            break;
        }
        case MODEL: {
            break;
        }
    }
}

void branchOptimizeModels( vector<Point> * points, Branch b){
    switch( b->type ){
        case EMPTY: { break; }

        case ANIMATION: {
            Animation t = (Animation)b->node;

            for( Branch nb : *(t->subbranch) )
                branchOptimizeModels(points, nb);

            break;
        }

        case TRANSFORMATION: {
            Transformation t = (Transformation)b->node;

            bool allmodels = true;

            for( Branch nb : *(t->subbranch) ) {
                branchOptimizeModels(points, nb);
                allmodels = allmodels && (nb->type == MODEL);
            }

            if( allmodels ){ /* every descendent is a model*/

                long minv = points->size();
                long maxv = -1;

                for( long i =0; i < t->subbranch->size(); i++ ) { /* transforms the models and combines them*/
                    Model model = (Model)(t->subbranch->at(i)->node);
                    minv = min(minv,model->starti);
                    maxv = max(maxv,model->endi);
                    applyTransformation(t, points->data(), model->starti, model->endi);
                    unmkBranch(t->subbranch->at(i));
                }

                delete t->subbranch;
                freeMat(t->mat);
                free(t);

                b->node = mkModel(minv,maxv);
                b->type = MODEL;/*agregates everything in a single model.*/
            }

            break;
        }

        case MODEL: { break; }
    }
}

void branchOptimize(vector<Point> * points, Branch root){
    branchOptimizeTransf(root);
    branchOptimizeModels(points,root);

}