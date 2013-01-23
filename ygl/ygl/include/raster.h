#ifndef _YGL_RASTER_H_
#define _YGL_RASTER_H_
#include "gltypes.h"

#define RESET_LINE_STIPPLE line_factor=line_pattern_bit=0
extern int line_factor,line_pattern_bit;

void line(GLVertex* p0,GLVertex* pEnd);
void linestipple(GLVertex* p0,GLVertex* p1);
void texfill(int ymin,int ymax);
void fill(int ymin,int ymax);
void smoothfill(int ymin,int ymax);
void blendfill(int ymin,int ymax);
void polyline(int ymin,int ymax);
void polypoints(int ymin,int ymax);

#endif

