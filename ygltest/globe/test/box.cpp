#include "box.h"
#include "../ygl/ygl.h"

void fastset(float x,float y,float z)
{
	glNormal3f(x,y,z);
	glVertex3f(x,y,z);
}

void drawWireBox(float halfSize)
{
	glBegin(GL_QUADS);
	fastset( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Top)
	fastset(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Top)
	fastset(-halfSize, halfSize, halfSize);					// Bottom Left Of The Quad (Top)
	fastset( halfSize, halfSize, halfSize);					// Bottom Right Of The Quad (Top)

	fastset( halfSize,-halfSize, halfSize);					// Top Right Of The Quad (Bottom)
	fastset(-halfSize,-halfSize, halfSize);					// Top Left Of The Quad (Bottom)
	fastset(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Bottom)
	fastset( halfSize,-halfSize,-halfSize);					// Bottom Right Of The Quad (Bottom)

	fastset( halfSize, halfSize, halfSize);					// Top Right Of The Quad (Front)
	fastset(-halfSize, halfSize, halfSize);					// Top Left Of The Quad (Front)
	fastset(-halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Front)
	fastset( halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Front)

	fastset( halfSize,-halfSize,-halfSize);					// Top Right Of The Quad (Back)
	fastset(-halfSize,-halfSize,-halfSize);					// Top Left Of The Quad (Back)
	fastset(-halfSize, halfSize,-halfSize);					// Bottom Left Of The Quad (Back)
	fastset( halfSize, halfSize,-halfSize);					// Bottom Right Of The Quad (Back)

	fastset(-halfSize, halfSize, halfSize);					// Top Right Of The Quad (Left)
	fastset(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Left)
	fastset(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Left)
	fastset(-halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Left)

	fastset( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Right)
	fastset( halfSize, halfSize, halfSize);					// Top Left Of The Quad (Right)
	fastset( halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Right)
	fastset( halfSize,-halfSize,-halfSize);	
	glEnd();
}