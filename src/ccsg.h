#include <stdlib.h>
#include <stdint.h>

#include "ds.h"
#include "../../c3dlas/c3dlas.h"






typedef struct PolyVertex {
	Vector pos;
	Vector normal;
} PolyVertex_t;


typedef VEC(struct PolyVertex) PolyVertexList_t;

typedef struct Polygon {
	void* shared;
	PolyVertexList_t vertices; 
	
} Polygon_t;

typedef VEC(struct Polygon*) PolygonList_t;



typedef struct BSPNode {
	Plane plane;
	
	struct BSPNode* front, *back;
	PolygonList_t coplanar;
	
} BSPNode_t;



typedef struct Solid {
	
	int type;
	char* name;
	
	PolygonList_t polygons;
	BSPNode_t* bsp;
	
} Solid_t;

Solid_t* solidUnion(Solid_t* sa, Solid_t* sb);
Solid_t* solidMakeCube(float lx, float ly, float lz);
Solid_t* solidFromPolygons(PolygonList_t* polys) ;
void solidCopy(Solid_t* copy, Solid_t* orig);
Polygon_t* polygonCreate(PolyVertex_t* verts, int vertcnt);

void solidInit(Solid_t* s);
Solid_t* solidIntersect(Solid_t* sa, Solid_t* sb);
Solid_t* solidMakeSphere(float radius) ;
Solid_t* solidSubtract(Solid_t* from, Solid_t* what);

Solid_t* solidInverse(Solid_t* so);

Solid_t* solidMakeCylinder(float radius, float length);
void polygonInit(Polygon_t* poly, PolyVertexList_t* vertices, void* shared);



Polygon_t* polygonCopy(Polygon_t* orig);
void polygonFlip(Polygon_t* p) ;
int polygonClassifyByPlane(Polygon_t* poly, Plane* p);
void polygonSliceByPlane(Polygon_t* poly, Plane* plane, PolygonList_t* cpfront, PolygonList_t* cpback, PolygonList_t* front, PolygonList_t* back);

void polygonCalcPlane(Polygon_t* poly, Plane* p);
struct BSPNode* bspNodeInit(PolygonList_t* polys); 


struct BSPNode* bspNodeCopy(struct BSPNode* old);
void bspNodeInvert(struct BSPNode* n);

void polyListConcat(PolygonList_t* head, PolygonList_t* tail);
void bspNodeBuild(BSPNode_t* node, PolygonList_t* polys);
void bspNodeAllPolygons(struct BSPNode* n, PolygonList_t* out);
void bspNodeClipPolygons(struct BSPNode* n, PolygonList_t* polys);
void bspNodeClipTo(struct BSPNode* n, struct BSPNode* clipper);
