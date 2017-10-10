


#include "ccsg.h"



int main(int argc, char* argv[]) {
	
	Solid_t* c;
	
	c = solidMakeCube();
	
	SolidToMesh(c);
	
	
	
	
	
	return 0;
}

