
#include <string.h>

#include "ccsg.h"


// mostly a port of http://evanw.github.io/csg.js/docs/

#define COPLANAR 0
#define FRONT    1
#define BACK     2
#define SPANNING 3



void polyVertexCopy(PolyVertex_t* copy, PolyVertex_t* orig) {
	memcpy(copy, orig, sizeof(&orig));
}


void polyVertexFlip(PolyVertex_t* pv) {
	vInverse(&pv->normal, &pv->normal);
}

void polyVertexLERP(PolyVertex_t* a, PolyVertex_t* b, double t, PolyVertex_t* out) {
	vLerp(&a->pos, &b->pos, t, &out->pos);
	vLerp(&a->normal, &b->normal, t, &out->normal);
}


// for feeding struct literals into
Polygon_t* polygonCreate(PolyVertex_t* verts, int vertcnt) {
	int i;
	Polygon_t* p;
	
	p = calloc(1, sizeof(*p));
	CHECK_OOM(p);
	
	VEC_INIT(&p->vertices);
	
	if(!verts) return p;
	
	for(i = 0; i < vertcnt; i++) {
		VEC_PUSH(&p->vertices, verts[i]);
	}
	
	return p;
}


// done
void solidInit(Solid_t* s) {
	VEC_INIT(&s->polygons);
	
}

// done
void solidCopy(Solid_t* copy, Solid_t* orig) {
	solidInit(copy);
	VEC_COPY(&copy->polygons, &orig->polygons);
}

Solid_t* solidFromPolygons(PolygonList_t* polys) {
	Solid_t* s;
	
	s = calloc(1, sizeof(*s));
	CHECK_OOM(s);
	
	solidInit(s);
	
	if(polys) {
		polyListCopy(s->polygons, polys);
	}
	
	return s;
}

Solid_t* solidMakeCube(float lx, float ly, float lz) {
	Solid_t* s;
	PolyVertex_t* verts;
	
	s = calloc(1, sizeof(*s));
	CHECK_OOM(s);
	
	verts = (PolyVertex_t[]){
		{.pos = {1,1,1}},
		{.pos = {2,4,1}},
		{.pos = {2,7,1}},
		{.pos = {2,7,8}}
	};
	
	VEC_PUSH(&s->polygons, polygonCreate(verts, 4));
	// TODO
}

Solid_t* solidMakeCylinder(float radius, float length) {
	// TODO
}
Solid_t* solidMakeSphere(float radius) {
	// TODO
}

// TODO needs some adjustment for argument passing
Solid_t* solidUnion(Solid_t* sa, Solid_t* sb) {
	
	Solid_t* out;
	struct BSPNode* a, *b;
	PolygonList_t bpolys, apolys;
	
	a = bspNodeInit(&sa->polygons);
	b = bspNodeInit(&sb->polygons);
	bspNodeClipTo(a, b);
	bspNodeClipTo(b, a);
	bspNodeInvert(b);
	bspNodeClipTo(b, a);
	bspNodeInvert(b);
	
	VEC_INIT(&bpolys);
	bspNodeAllPolygons(b, &bpolys);
	bspNodeBuild(a, &bpolys);

	VEC_INIT(&apolys);
	bspNodeAllPolygons(a, &apolys);
	
	
	out = solidFromPolygons(&apolys);
	
	VEC_FREE(&apolys);
	VEC_FREE(&bpolys);
	
	return out;
}


Solid_t* solidSubtract(Solid_t* from, Solid_t* what) {
	
	Solid_t* out;
	struct BSPNode* a, *b;
	PolygonList_t bpolys, apolys;
	
	a = bspNodeInit(&from->polygons);
	b = bspNodeInit(&what->polygons);
	bspNodeInvert(a);
	bspNodeClipTo(a, b);
	bspNodeClipTo(b, a);
	bspNodeInvert(b);
	bspNodeClipTo(b, a);
	bspNodeInvert(b);
	
	VEC_INIT(&bpolys);
	bspNodeAllPolygons(b, &bpolys);
	bspNodeBuild(a, &bpolys);
	
	bspNodeInvert(a);
	
	VEC_INIT(&apolys);
	bspNodeAllPolygons(a, &apolys);
	
	out = solidFromPolygons(&apolys);
	
	VEC_FREE(&apolys);
	VEC_FREE(&bpolys);
	
	return out;
}




Solid_t* solidIntersect(Solid_t* sa, Solid_t* sb) {
	
	Solid_t* out;
	struct BSPNode* a, *b;
	PolygonList_t bpolys, apolys;
	
	a = bspNodeInit(&sa->polygons);
	b = bspNodeInit(&sb->polygons);
	
	bspNodeInvert(a);
	bspNodeClipTo(b, a);
	bspNodeInvert(b);
	bspNodeClipTo(a, b);
	bspNodeClipTo(b, a);
	
	
	VEC_INIT(&bpolys);
	bspNodeAllPolygons(b, &bpolys);
	bspNodeBuild(a, &bpolys);
	
	bspNodeInvert(a);
	
	VEC_INIT(&apolys);
	bspNodeAllPolygons(a, &apolys);
	
	out = solidFromPolygons(&apolys);
	
	VEC_FREE(&apolys);
	VEC_FREE(&bpolys);
	
	return out;
}



Solid_t* solidInverse(Solid_t* so) {
	
	Solid_t* si;
	
	si = calloc(1, sizeof(*si));
	CHECK_OOM(si);
	
	// TODO
	
	
	
	return si;
}



void polygonInit(Polygon_t* poly, PolyVertexList_t* vertices, void* shared) {
	VEC_INIT(&poly->vertices);
	
	if(vertices) VEC_COPY(&poly->vertices, vertices);
	poly->shared = shared;
}


Polygon_t* polygonCopy(Polygon_t* orig) {
	Polygon_t* copy;
	
	copy = calloc(1, sizeof(*copy));
	CHECK_OOM(copy);
	
	polygonInit(copy, &orig->vertices, orig->shared);
	
	return copy;
}


void polygonFlip(Polygon_t* p) {
	size_t i;
	
	VEC_REVERSE(&p->vertices);
	
	for(i = 0; i < VEC_LEN(&p->vertices); i++) {
		polyVertexFlip(&VEC_DATA(&p->vertices)[i]);
	}
}


int polygonClassifyByPlane(Polygon_t* poly, Plane* p) {
	
	printf("NIH %s:%d \n", __FILE__, __LINE__);
}

void polygonSliceByPlane(Polygon_t* poly, Plane* plane, PolygonList_t* cpfront, PolygonList_t* cpback, PolygonList_t* front, PolygonList_t* back) {
	Plane polyPlane;
	int fixed[6];
	int* types = fixed;
	size_t vlen, i;
	PolyVertex_t* verts;
	Polygon_t* f, *b;
	int polyType = COPLANAR;
	
	verts = VEC_DATA(&poly->vertices);
	polygonCalcPlane(poly, &polyPlane);
	
	vlen = VEC_LEN(&poly->vertices);
	if(vlen > 6) {
		types = calloc(1, vlen * sizeof(*types));
	}
	else {
		memset(types, 0, sizeof(fixed));
	}
	
	// figure out where all ther vertices are with respect to the slicing plane
	for(i = 0; i < vlen; i++) {
		types[i] = planeClassifyPoint(plane, &verts[i].pos);
		polyType |= types[i];
	}
	
	switch(polyType) {
		case COPLANAR:
			if(vDot(&plane->n, &polyPlane.n) > 0) {
				VEC_PUSH(cpfront, poly);// front
			}
			else {
				VEC_PUSH(cpback, poly); // back
			}
			break;
		case FRONT: VEC_PUSH(front, poly); break;
		case BACK: VEC_PUSH(back, poly); break;
		case SPANNING:
			// chop the polygon across the plane
			
			
			f = calloc(1, sizeof(*f));
			b = calloc(1, sizeof(*b));
			
			for(i = 0; i < vlen; i++) {
				size_t j = (i + 1) % vlen;
				int ti = types[i];
				int tj = types[j];
				float t;
				Vector tmp;
				
				if(ti != BACK) VEC_PUSH(&f->vertices, verts[i]);
				if(ti != FRONT) VEC_PUSH(&b->vertices, verts[i]);
				if((ti | tj) == SPANNING) {
					
					vSub(&verts[j].pos, &verts[i].pos, &tmp); 
					// TODO: div/0 check?
					// BUG: the line below may be semantically wrong
					t = (plane->d - vDot(&plane->n, &verts[i].pos)) / vDot(&plane->n, &tmp); 
					
					vLerp(&verts[j].pos, &verts[i].pos, t, &tmp); 
					
					// TODO: lerp normal
					
					// push to front and back
					VEC_PUSH(&f->vertices, (PolyVertex_t){.pos = tmp});
					VEC_PUSH(&b->vertices, (PolyVertex_t){.pos = tmp});
				}
			}
			
			break;
	}
	
	if(vlen > 6) free(types);
}

void polygonCalcPlane(Polygon_t* poly, Plane* p) {
	if(VEC_LEN(&poly->vertices) < 3) {
		fprintf(stderr, "Polygon with less than 3 vertices when calculating plane.");
		exit(1);
	}
	
	planeFromTriangle(
		&VEC_DATA(&poly->vertices)[0].pos, 
		&VEC_DATA(&poly->vertices)[1].pos, 
		&VEC_DATA(&poly->vertices)[2].pos, 
		p);
}


// done
struct BSPNode* bspNodeInit(PolygonList_t* polys) {
	
	struct BSPNode* new;
	
	new = calloc(1, sizeof(*new));
	CHECK_OOM(new);
	
	VEC_INIT(&new->coplanar);
	
	if(polys) {
		bspNodeBuild(new, polys);
	}
	
	return new;
}





struct BSPNode* bspNodeCopy(struct BSPNode* old) {
	
	struct BSPNode* new;
	
	new = calloc(1, sizeof(*new));
	
	// TODO this function looks like it's wrong
	
	// copy coplanar polys
	VEC_COPY(&new->coplanar, &old->coplanar);
	
	memcpy(&new->plane, &old->plane, sizeof(old->plane));
	
	new->front = bspNodeCopy(old->front);
	new->back = bspNodeCopy(old->back);
	
	return new;
}


void bspNodeInvert(struct BSPNode* n) {
	struct BSPNode* temp;
	
	// TODO
	// flip coplanar polygons
	
	// flip plane
	
	
	if(n->front) bspNodeInvert(n->front);
	if(n->back) bspNodeInvert(n->back);
	
	temp = n->front;
	n->front = n->back;
	n->back = temp;
}

// done
// deep concat
void polyListConcat(PolygonList_t* head, PolygonList_t* tail) {
	int i;
	
	// TODO: fix below
	if(!tail) return head;
	if(!head) return tail;
	
	for(i = 0; i < VEC_LEN(tail); i++) {
		VEC_INC(head);
		VEC_COPY(VEC_TAIL(head), &VEC_DATA(tail)[i]);
	}
}


void bspNodeClipPolygons(struct BSPNode* n, PolygonList_t* polys) {
	
	int i;
	PolygonList_t front, back;
	PolygonList_t* fo, *bo;
	
	fo = bo = NULL;
	
	//if (!this.plane) return polygons.slice();
	
	VEC_INIT(&front);
	VEC_INIT(&back);
	
	for(i = 0; i < VEC_LEN(polys); i++) {
		polygonSliceByPlane(VEC_DATA(polys)[i], &n->plane, &front, &back, &front, &back);
	}
	
	
	
	// TODO ... um? return value?
	if(VEC_LEN(&front)) fo = bspNodeClipPolygons(n->front, &front) 
	if(VEC_LEN(&back)) bo = bspNodeClipPolygons(n->back, &back)
	
	
	
	VEC_FREE(&front);
	VEC_FREE(&back);
	
	polyListConcat(fo, bo);
	
	VEC_FREE(bo);
	
	return fo;
	
	/*
    
    if (this.front) front = this.front.clipPolygons(front);
    if (this.back) back = this.back.clipPolygons(back);
    else back = [];
    return front.concat(back);
	*/
	// TODO
}

void bspNodeClipTo(struct BSPNode* n, struct BSPNode* clipper) {
	
	
	// TODO: clip polygons, need to figure out good argument passing for bspNodeClipPolygons
	
	if(n->front) bspNodeClipTo(n->front, clipper);
	if(n->back) bspNodeClipTo(n->back, clipper);
}

// appends to out, does not initialize
void bspNodeAllPolygons(struct BSPNode* n, PolygonList_t* out) {
	int i;
	
	for(i = 0; i < VEC_LEN(&n->coplanar); i++) {
		VEC_PUSH(out, VEC_DATA(&n->coplanar)[i]);
	}
	
	if(n->front) bspNodeAllPolygons(n->front, out);
	if(n->back) bspNodeAllPolygons(n->back, out);
}


// done
void bspNodeBuild(BSPNode_t* node, PolygonList_t* polys) {
	int i;
	PolygonList_t front, back;
	
	
	if(!VEC_LEN(polys)) {
		return;
	}
	
	// if not already calculated...
	if(node->plane.n.x == 0.0 && node->plane.n.y == 0.0 && node->plane.n.z == 0.0) {
		polygonCalcPlane(VEC_DATA(&node->coplanar)[0], &node->plane);
	}
	
	VEC_INIT(&front);
	VEC_INIT(&back);
	
	    
	for(i = 0; i < VEC_LEN(polys); i++) {
		polygonSliceByPlane(VEC_DATA(polys)[i], &node->plane, &node->coplanar, &node->coplanar, &front, &back);
	}
	
	
	if(VEC_LEN(&front)) {
		node->front = bspNodeInit(&front);
	}
	
	if(VEC_LEN(&back)) {
		node->back = bspNodeInit(&back);
	}
	
	// BUG free the vertex lists too
	VEC_FREE(&front);
	VEC_FREE(&back);
}





