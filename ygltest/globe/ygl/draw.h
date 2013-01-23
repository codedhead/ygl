#ifndef _YGL_DRAW_H_
#define _YGL_DRAW_H_

#include "gltypes.h"

extern GLVertex draw_vertex_buffer1[];
extern GLVertex draw_vertex_buffer2[];
extern GLVertex* draw_vertex_buffer;
extern int draw_vertex_cnt;

void draw(int type,GLVertex* vertexs,int pcnt);

#endif