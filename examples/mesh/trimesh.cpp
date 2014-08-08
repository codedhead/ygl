#include "trimesh.h"

#include <stdlib.h>
#include <ctype.h>
#include <vector>



TriMesh::TriMesh(int ntriangles, int nv, int nn, const int *viptr, const int *niptr,
		const Point *P, const Normal *N, const float *UV)
		:ntris(ntriangles),nverts(nv),vi(0),ni(0),n(0),uv(0),p(0)
{
	Assert(ntris&&viptr&&P);
	vi=new int[3*ntris];
	memcpy(vi,viptr,3*ntris*sizeof(int));
	if(niptr)
	{
		ni=new int[3*ntris];
		memcpy(ni,niptr,3*ntris*sizeof(int));
	}

	if(UV)
	{
		uv=new float[2*nverts];
		memcpy(uv,UV,2*nverts*sizeof(float));
	}
	if(N)
	{
		// todo: transform normals?????
		n=new Normal[nn];
		memcpy(n,N,sizeof(Normal)*nn);
// 		for(int i=0;i<nn;++i)
// 		{
// 			n[i]=o2w(N[i]);
// 		}
	}
	p=new Point[nverts];
	memcpy(p,P,sizeof(Point)*nverts);
	for(int i=0;i<nverts;++i)
	{
		objbound+=P[i];
	}
}
TriMesh::~TriMesh()
{
	if(vi) delete[] vi;
	if(ni) delete[] ni;
	if(p) delete[] p;
	if(n) delete[] n;
	if(uv) delete[] uv;
}


namespace MeshLoader
{
	void compute_normals(const std::vector<Point>& verts,const std::vector<int>& faces,std::vector<Normal>& norms)
	{
		int nverts=verts.size();
		int nfaces=faces.size();
		int i;
		Normal nrm;

		/* zero out all normal information at vertices */
		norms.reserve(nverts);
		for (i = 0; i < nverts; ++i) {
			norms.push_back(Normal());
		}

		/* find normal of each face and add it to each vertex adjacent to the face */

		for (i = 0; i+2 < nfaces; i+=3)
		{
			nrm=Normal((verts[faces[i+1]]-verts[faces[i]])^(verts[faces[i+2]]-verts[faces[i+1]]));
			//nrm.normalize();
			norms[faces[i]]+=nrm;
			norms[faces[i+1]]+=nrm;
			norms[faces[i+2]]+=nrm;
		}

		/* normalize all the normals at the vertices */

		//for (i = 0; i < nverts; ++i)
			//norms[i].normalize();
	}

	
	TriMesh* readOffMesh(const char* filename)
	{
		int i;
		// Open file
		FILE *fp;
		if (!(fp = fopen(filename, "r"))) {
			printf("Unable to open OFF file %s\n", filename);
			return NULL;
		}

		// Read file
		int nverts = 0;
		int nfaces = 0;
		int nedges = 0;
		int line_count = 0;
		char buffer[1024];

		std::vector<Point> verts;
		std::vector<int> faces;
		std::vector<Normal> norms;
		Point vt;
		
		while (fgets(buffer, 1023, fp)) {
			// Increment line counter
			line_count++;

			// Skip white space
			char *bufferp = buffer;
			while (isspace(*bufferp)) bufferp++;

			// Skip blank lines and comments
			if (*bufferp == '#') continue;
			if (*bufferp == '\0') continue;

			// Check section
			if (nverts == 0) {
				// Read header 
				if (!strstr(bufferp, "OFF")) {
					// Read mesh counts
					if ((sscanf(bufferp, "%d%d%d", &nverts, &nfaces, &nedges) != 3) || (nverts == 0)) {
						printf("Syntax error reading header on line %d in file %s\n", line_count, filename);
						fclose(fp);
						return NULL;
					}
					verts.reserve(nverts);
					faces.reserve(nfaces*3);
				}
			}
			else if (verts.size() < nverts) {
				// Read vertex coordinates
				if (sscanf(bufferp, "%f%f%f", &(vt.x), &(vt.y), &(vt.z)) != 3) {
					printf("Syntax error with vertex coordinates on line %d in file %s\n", line_count, filename);
					fclose(fp);
					return NULL;
				}
				verts.push_back(vt);
			}
			else if (faces.size() < 3*nfaces) {
				// Get next face
				int fverts=0,ivert;

				// Read number of vertices in face 
				bufferp = strtok(bufferp, " \t");
				if (bufferp) fverts = atoi(bufferp);
				else {
					printf("Syntax error with face on line %d in file %s\n", line_count, filename);
					fclose(fp);
					return NULL;
				}
				assert(fverts==3);
				if(fverts!=3)
				{
					printf("Fail read OFF file, not triangle polygon found on line %d in file %s\n",line_count,filename);
					fclose(fp);
					return NULL;
				}

				for (i = 0; i < fverts; i++) {
					bufferp = strtok(NULL, " \t");
					if (bufferp) ivert = atoi(bufferp);
					else {
						printf("Syntax error with face on line %d in file %s\n", line_count, filename);
						fclose(fp);
						return NULL;
					}
					faces.push_back(ivert);
				}
			}
			else {
				// Should never get here
				printf("Found extra text starting at line %d in file %s\n", line_count, filename);
				break;
			}
		}

		// Close file
		fclose(fp);
		
		// Check whether read all faces
		if (nverts != verts.size()) {
			printf("Expected %d verts, but read only %d verts in file %s\n", nverts, verts.size(), filename);
			return NULL;
		}
		if (nfaces*3 != faces.size()) {
			printf("Expected %d faces, but read only %d faces in file %s\n", nfaces, faces.size(), filename);
			return NULL;
		}
		
		compute_normals(verts,faces,norms);
		Assert(norms.size()==verts.size());
		return new TriMesh(nfaces, nverts, nverts, &faces[0], 0/* n index */, &verts[0],
			&norms[0],  NULL);
	}
}