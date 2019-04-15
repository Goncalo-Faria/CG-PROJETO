#if defined(_WIN32)
    #include "GL/glut.h"
#else
	#include <GLUT/glut.h>
#endif

#include "assembler.h"
#include "branch.h"
#include "../common/point.h"
#include "../common/tinyxml2.h"
#include "../common/matop.h"
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace tinyxml2;

typedef struct assembler{
    struct branch root;
    Branch cur;
    vector< Point > * points;
    GLuint buffer;
} *Assembler;

void parseModel(const char * filename, Assembler state);
Assembler parseGroups(XMLNode * group, Assembler state);
vector<string> split(string strToSplit, char delimeter);

Assembler mkAssembler(){
    Assembler ass = (Assembler)malloc( sizeof(struct assembler) );
    ass->cur = &(ass->root);
    ass->points = new vector< Point >();
    ass->buffer= -1;

    ass->root.type = EMPTY;
    ass->root.node = NULL;
    return ass;
}

Assembler mkAssembler( Assembler origin ){
	Assembler r = (Assembler)malloc( sizeof(struct assembler) );
	r->root.type = FAKE;
	r->points = origin->points;
	r->buffer = origin->buffer;

    Branch bn = mkBranch(mkTransformation(identity()));

	switch( origin->cur->type ){
        case EMPTY: {
            origin->cur->node = mkTransformation(identity());
            origin->cur->type = TRANSFORMATION;
            addDescendent((Transformation)origin->cur->node, bn);

        }

        case ANIMATION: {
            addDescendent((Animation)origin->cur->node, bn);
            r->cur = bn;
            break;
        }

        case TRANSFORMATION: {
            addDescendent((Transformation)origin->cur->node, bn);
            r->cur = bn;
            break;
        }

        default : {
            printf("error: models must be the terminal nodes\n");
            exit(2);
        }
	}

	return r;
}

Assembler mkAssemblerModelView(Assembler origin){
    Assembler r = (Assembler)malloc( sizeof(struct assembler) );
    r->root.type = FAKE;
    r->points = origin->points;
    r->buffer = origin->buffer;

    Model mn = mkModel(assemblerNumberOfPoints(origin), assemblerNumberOfPoints(origin));

    switch( origin->cur->type ){
        case EMPTY: {
            origin->cur->node = mn;
            origin->cur->type = MODEL;
            r->cur = origin->cur;
        }

        case ANIMATION: {
            Branch tmp =  mkBranch(mn);
            addDescendent((Animation)origin->cur->node, tmp );
            r->cur = tmp;
            break;
        }

        case TRANSFORMATION: {
            Branch tmp =  mkBranch(mn);
            addDescendent((Transformation)origin->cur->node, tmp );
            r->cur = tmp;
            break;
        }

        case MODEL: {
            r->cur = origin->cur;
        }

        default : {
            printf("error: models must be the terminal nodes\n");
            exit(2);
        }
    }

    return r;
}

Assembler mkAssembler(const char * filename){
    XMLDocument xml_doc;
    //cout << "parse" << endl;
    XMLError eResult =
            xml_doc.LoadFile(filename);

    if (eResult != XML_SUCCESS) {
        //cout << "parse" << endl;
        return NULL;
    }

    XMLNode* root = xml_doc.FirstChildElement("scene");

    if (root == nullptr)
        return NULL;

    Assembler frame = mkAssembler();

    frame = parseGroups(root, frame);

    xml_doc.Clear();

    return frame;
}

Branch view(Assembler source){
    return source->cur;
}

void addDescendentAndGo(Assembler source, Branch descendent){

    switch( source->cur->type ) {
        case ANIMATION: {
            addDescendent((Animation) source->cur->node, descendent);
            break;
        }

        case TRANSFORMATION: {
            addDescendent((Transformation) source->cur->node, descendent);
            break;
        }
    }

    source->cur = descendent;
}

void unmkAssembler( Assembler target ){

	if( target->root.type != FAKE ){
		unmkBranch(target->root);
		delete target->points;
	}
	free(target);
}

void assemblerPoint(Assembler ass, Point point){
    ass->points->emplace_back(point);
}

long assemblerNumberOfPoints(Assembler ass){
    return ass->points->size();
}

void assemblerRotate(Assembler ass, float angle, float vx, float vy, float vz){
    float rx = vx*angle;
	float ry = vy*angle;
	float rz = vz*angle;

	float** mrx = matRx(rx);
    float** mry = matRy(ry);
    float** mrz = matRz(rz);

    float** mr1 = matmul(mrx,mry);
    float** mr2 = matmul(mr1,mrz);
    freeMat(mr1);

    assemblerTransformate(ass,mr2);
}

void assemblerTranslate(Assembler ass, float x, float y, float z){
    float** t = identity();
	
	t[0][3] = x;
	t[1][3] = y;
	t[2][3] = z;

	assemblerTransformate(ass,t);
}

void assemblerScale(Assembler ass, float xaxis, float yaxis, float zaxis){
    float** t = identity();

	t[0][0] = xaxis;
	t[1][1] = yaxis;
	t[2][2] = zaxis;

    assemblerTransformate(ass,t);
}

vector<string> split(string strToSplit, char delimeter)
{
	stringstream ss(strToSplit);
	string item;
	vector<string> splittedStrings;

	while (getline(ss, item, delimeter)){
		splittedStrings.push_back(item);
	}

	return splittedStrings;
}

void parseModel(const char * filename, Assembler state)
{
	ifstream file(filename);
	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			if (line.find("point") != string::npos) {
				vector<string> s = split(line, '"');
				assemblerModelate(state, stod(s.at(1)), stod(s.at(3)), stod(s.at(5)));
			}
		}
		file.close();
	}
}

Assembler parseGroups(XMLNode * group, Assembler state)
{
	for(XMLNode * g = group->FirstChild();
		g != nullptr;
		g = g->NextSibling()
		)
	{
		const char * name = g->Value();
		//printf("%s \n",name);
		if(!strcmp(name,"models")){
            Assembler other = mkAssemblerModelView(state);
			XMLElement * e = g->FirstChildElement("model");
	
			while(e != nullptr) {
				parseModel(e->Attribute("file"),other);
				e = e->NextSiblingElement("model");
			}

			unmkAssembler(other);
		}else if(!strcmp(name,"translate")){

			XMLElement *e = (XMLElement*) g;
			assemblerTranslate(	state,
					 	e->FloatAttribute("X"),
						e->FloatAttribute("Y"),
						e->FloatAttribute("Z")
						);
		}else if(!strcmp(name,"rotate")){
			
			XMLElement *e = (XMLElement*) g;
			assemblerRotate( state,
						e->FloatAttribute("angle"),
						e->FloatAttribute("axisX"),
						e->FloatAttribute("axisY"),
						e->FloatAttribute("axisZ")
						);
		}else if(!strcmp(name,"scale")){
		
			XMLElement *e = (XMLElement*) g;
			assemblerScale(state,
						e->FloatAttribute("stretchX", 1.0),
						e->FloatAttribute("stretchY", 1.0),
						e->FloatAttribute("stretchZ", 1.0)
			);
		
		}else if(!strcmp(name,"group")){
			unmkAssembler(parseGroups(g, mkAssembler(state)));
		}
	
	}

	return state;
}

void assemblerDraw(Assembler reference){
    glBindBuffer(GL_ARRAY_BUFFER,reference->buffer);
    glVertexPointer(3,GL_FLOAT,0,0);
    glDrawArrays(GL_TRIANGLES, 0, reference->points->size());
}

void assemblerBufferData(Assembler reference){
    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(1, &(reference->buffer) );
    glBindBuffer(GL_ARRAY_BUFFER,reference->buffer);
    glBufferData(
            GL_ARRAY_BUFFER,
            reference->points->size() * sizeof(Point),
            &( (*(reference->points))[0] ),
            GL_STATIC_DRAW
    );
}

