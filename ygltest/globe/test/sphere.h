#ifndef _YGL_SPHERE_TEST_H_
#define _YGL_SPHERE_TEST_H_

typedef double v4[4];
v4* glxGenSphere(double radius, int slices, int stacks);
void glxFreeSphere(v4* vts);
void glxSolidSphere(v4* vts,int slices, int stacks);
void glxTextureSphere(v4* vts,int slices, int stacks);
void glxWireSphere(v4* vts,int slices, int stacks);


#endif