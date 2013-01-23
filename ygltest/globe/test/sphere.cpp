#include "sphere.h"
#include "../ygl/ygl.h"
#define PI 3.141592653

v4* glxGenSphere(double radius, int slices, int stacks)
{
	int vtcnt=slices*stacks+2;
	v4* vts=(v4*)new double[4*vtcnt];
	vts[0][0]=vts[0][2]=0.0;vts[0][1]=radius;vts[0][3]=1.0;
	vts[vtcnt-1][0]=vts[vtcnt-1][2]=0.0;vts[vtcnt-1][1]=-radius;vts[vtcnt-1][3]=1.0;

	double unitalpha=PI/(stacks+1),unitbeta=2.0*PI/slices;
	double alpha=unitalpha,beta=0.0,x,y,z,r;
	int i=1;

	for(int st=0;st<stacks;++st,alpha+=unitalpha)
	{
		y=radius*cos(alpha);
		r=radius*sin(alpha);

		beta=0.0;
		for(int sl=0;sl<slices;++sl,beta+=unitbeta)
		{
			x=r*cos(beta);
			z=-r*sin(beta);
			vts[i][0]=x;vts[i][1]=y;vts[i][2]=z;
			vts[i++][3]=1.0;
		}
	}
	return vts;
}

void glxFreeSphere(v4* vts)
{
	if(vts)
		delete [] (double*)vts;
}

void glxSolidSphere(v4* vts,int slices, int stacks)
{
	int last=slices*stacks+1;
	int i;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3dv(vts[0]);
	glVertex3dv(vts[0]);
	for(i=1;i<=slices;++i)
	{
		glNormal3dv(vts[i]);
		glVertex3dv(vts[i]);
	}
	glNormal3dv(vts[1]);
	glVertex3dv(vts[1]);
	glEnd();

	for(int st=0;st<stacks-1;++st)
	{
		glBegin(GL_QUAD_STRIP);
		for(int sl=0;sl<slices;++sl)
		{
			glNormal3dv(vts[1+st*slices+sl]);
			glVertex3dv(vts[1+st*slices+sl]);
			glNormal3dv(vts[1+(st+1)*slices+sl]);
			glVertex3dv(vts[1+(st+1)*slices+sl]);
		}
		glNormal3dv(vts[1+st*slices]);
		glVertex3dv(vts[1+st*slices]);
		glNormal3dv(vts[1+(st+1)*slices]);
		glVertex3dv(vts[1+(st+1)*slices]);
		glEnd();
	}

	glBegin(GL_TRIANGLE_FAN);
	glNormal3dv(vts[last]);
	glVertex3dv(vts[last]);
	for(i=1;i<=slices;++i)
	{
		glNormal3dv(vts[last-i]);
		glVertex3dv(vts[last-i]);
	}
	glNormal3dv(vts[last-1]);
	glVertex3dv(vts[last-1]);
	glEnd();

	glFlush();
}

void glxWireSphere(v4* vts,int slices, int stacks)
{
	int last=slices*stacks+1;
	int i;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	for(int st=0;st<stacks;++st)
	{
		glBegin(GL_LINE_LOOP);
		for(int sl=0;sl<slices;++sl)
			glVertex3dv(vts[1+st*slices+sl]);
		glEnd();
	}


	for(int sl=0;sl<slices;++sl)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3dv(vts[0]);
		for(int st=0;st<stacks;++st)
			glVertex3dv(vts[1+st*slices+sl]);
		glVertex3dv(vts[last]);
		glEnd();
	}

	
	glFlush();
}

void glxTextureSphere(v4* vts,int slices, int stacks)
{
	int last=slices*stacks+1;
	int i;
	double uunit=1.0/((double)slices),vunit=1.0/((double)(stacks+1)),u,v;
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f,1.0f);
	glNormal3dv(vts[0]);
	glVertex3dv(vts[0]);

	u=0.0;v=1.0-vunit;
	for(i=1;i<=slices;++i,u+=uunit)
	{
		glTexCoord2f(u,v);
		glNormal3dv(vts[i]);
		glVertex3dv(vts[i]);
	}
	glTexCoord2f(1.0,v);
	glNormal3dv(vts[1]);
	glVertex3dv(vts[1]);
	glEnd();

	for(int st=0;st<stacks-1;++st,v-=vunit)
	{
		u=0.0;
		glBegin(GL_QUAD_STRIP);
		for(int sl=0;sl<slices;++sl,u+=uunit)
		{
			glTexCoord2f(u,v);
			glNormal3dv(vts[1+st*slices+sl]);
			glVertex3dv(vts[1+st*slices+sl]);
			glTexCoord2f(u,v-vunit);
			glNormal3dv(vts[1+(st+1)*slices+sl]);
			glVertex3dv(vts[1+(st+1)*slices+sl]);
		}
		glTexCoord2f(1.0,v);
		glNormal3dv(vts[1+st*slices]);
		glVertex3dv(vts[1+st*slices]);
		glTexCoord2f(1.0,v-vunit);
		glNormal3dv(vts[1+(st+1)*slices]);
		glVertex3dv(vts[1+(st+1)*slices]);
		glEnd();
	}

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f,0.0f);
	glNormal3dv(vts[last]);
	glVertex3dv(vts[last]);
	u=0.0;v=vunit;
	for(i=1;i<=slices;++i,u+=uunit)
	{
		glTexCoord2f(u,v);
		glNormal3dv(vts[last-i]);
		glVertex3dv(vts[last-i]);
	}
	glTexCoord2f(1.0,v);
	glNormal3dv(vts[last-1]);
	glVertex3dv(vts[last-1]);
	glEnd();

	glFlush();
}
