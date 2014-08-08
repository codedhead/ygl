#ifndef _YGL_TRIMESH_H_
#define _YGL_TRIMESH_H_

#include "geom.h"

class TriMesh
{
public:
	TriMesh(int ntris, int nverts, int nnorms, const int *viptr, const int *niptr,
		const Point *P, const Normal *N, const float *uv);
	~TriMesh();

	int ntris, nverts;
	int *vi,*ni;
	Point *p;
	Normal *n;
	float *uv;
	BBox objbound;
};

namespace MeshLoader
{
	//TriMesh* readObjMesh(const char* fname,const char* gname);
	TriMesh* readOffMesh(const char* fname);
	//TriMesh* readPlyMesh(const char* fname);
}


#endif