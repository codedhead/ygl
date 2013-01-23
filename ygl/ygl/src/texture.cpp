#include "../include/texture.h"
#include "../include/gltypes.h"

void tex2DLinear(GLdouble s,GLdouble t,byte* ret_color);
void tex2DNearest(GLdouble s,GLdouble t,byte* ret_color);
void tex2DLinearf(GLdouble s,GLdouble t,float* ret_color);
void tex2DNearestf(GLdouble s,GLdouble t,float* ret_color);

TEXFUNC tex2DFuncs[]={tex2DNearest,tex2DLinear};
TEXFUNCf tex2DFuncsf[]={tex2DNearestf,tex2DLinearf};

extern GLContext gl_context;

void tex2DLinear(GLdouble s,GLdouble t,byte* ret_color)
{
	GLTextureObject* obj=gl_context.current_tex_object;
	if(s>1.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=1.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s-=(int)s;
	}
	else if(s<0.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=0.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s=(int)(-s-1e-9)+1.0-s;
	}
	if(t>1.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=1.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t-=(int)t;
	}
	else if(t<0.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=0.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t=(int)(-t-1e-9)+1.0-t;
	}

	int x=(obj->tex_image_2d[0].width-1)*s,y=(obj->tex_image_2d[0].height-1)*t;
	int sum[4]={0},cnt=1,ncnt;

	byte* neighbor[5];

	neighbor[0]=(byte*)(obj->tex_image_2d[0].texels)+(y*obj->width_bytes+x*obj->units);
	if(x>0)
		neighbor[cnt++]=neighbor[0]-obj->units;
	if(x<obj->tex_image_2d[0].width-1)
		neighbor[cnt++]=neighbor[0]+obj->units;
	if(y>0)
		neighbor[cnt++]=neighbor[0]-obj->width_bytes;
	if(y<obj->tex_image_2d[0].height-1)
		neighbor[cnt++]=neighbor[0]+obj->width_bytes;
	ncnt=cnt;

	switch(obj->tex_image_2d[0].format)
	{
	case GL_RGBA:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:	
			while(cnt--)
			{
				sum[0]+=*neighbor[cnt];
				sum[1]+=*(neighbor[cnt]+1);
				sum[2]+=*(neighbor[cnt]+2);
				sum[3]+=*(neighbor[cnt]+3);
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:			
			while(cnt--)
			{
				sum[0]+=*((unsigned short*)neighbor[cnt]);
				sum[1]+=*(((unsigned short*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned short*)neighbor[cnt])+2);
				sum[3]+=*(((unsigned short*)neighbor[cnt])+3);
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			while(cnt--)
			{
				sum[0]+=*((unsigned int*)neighbor[cnt]);
				sum[1]+=*(((unsigned int*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned int*)neighbor[cnt])+2);
				sum[3]+=*(((unsigned int*)neighbor[cnt])+3);
			}
			break;
		case GL_FLOAT:
			while(cnt--)
			{
				sum[0]+=255**((float*)neighbor[cnt]);
				sum[1]+=255**(((float*)neighbor[cnt])+1);
				sum[2]+=255**(((float*)neighbor[cnt])+2);
				sum[3]+=255**(((float*)neighbor[cnt])+3);
			}
			break;
		}
		ret_color[0]=sum[0]/ncnt;
		ret_color[1]=sum[1]/ncnt;
		ret_color[2]=sum[2]/ncnt;
		ret_color[3]=sum[3]/ncnt;
		break;
	case GL_RGB:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:	
			while(cnt--)
			{
				sum[0]+=*neighbor[cnt];
				sum[1]+=*(neighbor[cnt]+1);
				sum[2]+=*(neighbor[cnt]+2);
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:			
			while(cnt--)
			{
				sum[0]+=*((unsigned short*)neighbor[cnt]);
				sum[1]+=*(((unsigned short*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned short*)neighbor[cnt])+2);
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			while(cnt--)
			{
				sum[0]+=*((unsigned int*)neighbor[cnt]);
				sum[1]+=*(((unsigned int*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned int*)neighbor[cnt])+2);
			}
			break;
		case GL_FLOAT:
			while(cnt--)
			{
				sum[0]+=255**((float*)neighbor[cnt]);
				sum[1]+=255**(((float*)neighbor[cnt])+1);
				sum[2]+=255**(((float*)neighbor[cnt])+2);
			}
			break;
		}
		ret_color[0]=sum[0]/ncnt;
		ret_color[1]=sum[1]/ncnt;
		ret_color[2]=sum[2]/ncnt;
		ret_color[3]=255;
		break;
	}
}
void tex2DNearest(GLdouble s,GLdouble t,byte* ret_color)
{
	GLTextureObject* obj=gl_context.current_tex_object;
	if(s>1.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=1.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s-=(int)s;
	}
	else if(s<0.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=0.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s=(int)(-s-1e-9)+1.0-s;
	}
	if(t>1.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=1.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t-=(int)t;
	}
	else if(t<0.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=0.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t=(int)(-t-1e-9)+1.0-t;
	}

	byte* offset=((byte*)(obj->tex_image_2d[0].texels))+
		(((int)((obj->tex_image_2d[0].height-1)*t+0.5))*obj->width_bytes+
		((int)((obj->tex_image_2d[0].width-1)*s+0.5))*obj->units);

	switch(obj->tex_image_2d[0].format)
	{
	case GL_RGBA:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:				
			{
				byte* entry=(byte*)offset;				
				ret_color[0]=entry[0];
				ret_color[1]=entry[1];
				ret_color[2]=entry[2];
				ret_color[3]=entry[3];
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			{
				unsigned short* entry=(unsigned short*)offset;					
				ret_color[0]=entry[0];
				ret_color[1]=entry[1];
				ret_color[2]=entry[2];
				ret_color[3]=entry[3];
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			{unsigned int* entry=(unsigned int*)offset;					
			ret_color[0]=entry[0];
			ret_color[1]=entry[1];
			ret_color[2]=entry[2];
			ret_color[3]=entry[3];
			}
			break;
		case GL_FLOAT:
			{float* entry=(float*)offset;				
			ret_color[0]=entry[0]*255;
			ret_color[1]=entry[1]*255;
			ret_color[2]=entry[2]*255;
			ret_color[3]=entry[3]*255;
			}
			break;
		}
		break;
	case GL_RGB:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:				
			{
				byte* entry=(byte*)offset;					
				ret_color[0]=entry[0];
				ret_color[1]=entry[1];
				ret_color[2]=entry[2];
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			{
				unsigned short* entry=(unsigned short*)offset;					
				ret_color[0]=entry[0];
				ret_color[1]=entry[1];
				ret_color[2]=entry[2];
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			{unsigned int* entry=(unsigned int*)offset;				
			ret_color[0]=entry[0];
			ret_color[1]=entry[1];
			ret_color[2]=entry[2];
			}
			break;
		case GL_FLOAT:
			{float* entry=(float*)offset;				
			ret_color[0]=entry[0]*255;
			ret_color[1]=entry[1]*255;
			ret_color[2]=entry[2]*255;
			}
			break;
		}
		ret_color[3]=255;
		break;
	}
}





void tex2DLinearf(GLdouble s,GLdouble t,float* ret_color)
{
	GLTextureObject* obj=gl_context.current_tex_object;
	if(s>1.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=1.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s-=(int)s;
	}
	else if(s<0.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=0.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s=(int)(-s-1e-9)+1.0-s;
	}
	if(t>1.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=1.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t-=(int)t;
	}
	else if(t<0.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=0.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t=(int)(-t-1e-9)+1.0-t;
	}

	int x=(obj->tex_image_2d[0].width-1)*s,y=(obj->tex_image_2d[0].height-1)*t;
	int sum[4]={0},cnt=1,ncnt;

	byte* neighbor[5];

	neighbor[0]=(byte*)obj->tex_image_2d[0].texels+(y*obj->width_bytes+x*obj->units);
	if(x>0)
		neighbor[cnt++]=neighbor[0]-obj->units;
	if(x<obj->tex_image_2d[0].width-1)
		neighbor[cnt++]=neighbor[0]+obj->units;
	if(y>0)
		neighbor[cnt++]=neighbor[0]-obj->width_bytes;
	if(y<obj->tex_image_2d[0].height-1)
		neighbor[cnt++]=neighbor[0]+obj->width_bytes;
	ncnt=cnt;

	switch(obj->tex_image_2d[0].format)
	{
	case GL_RGBA:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:	
			while(cnt--)
			{
				sum[0]+=*neighbor[cnt];
				sum[1]+=*(neighbor[cnt]+1);
				sum[2]+=*(neighbor[cnt]+2);
				sum[3]+=*(neighbor[cnt]+3);
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:			
			while(cnt--)
			{
				sum[0]+=*((unsigned short*)neighbor[cnt]);
				sum[1]+=*(((unsigned short*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned short*)neighbor[cnt])+2);
				sum[3]+=*(((unsigned short*)neighbor[cnt])+3);
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			while(cnt--)
			{
				sum[0]+=*((unsigned int*)neighbor[cnt]);
				sum[1]+=*(((unsigned int*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned int*)neighbor[cnt])+2);
				sum[3]+=*(((unsigned int*)neighbor[cnt])+3);
			}
			break;
		case GL_FLOAT:
			ret_color[0]=ret_color[1]=ret_color[2]=ret_color[3]=1.0;
			while(cnt--)
			{
				ret_color[0]+=*((float*)neighbor[cnt]);
				ret_color[1]+=*(((float*)neighbor[cnt])+1);
				ret_color[2]+=*(((float*)neighbor[cnt])+2);
				ret_color[2]+=*(((float*)neighbor[cnt])+3);
			}
			ret_color[0]/=ncnt;ret_color[1]/=ncnt;ret_color[2]/=ncnt;ret_color[3]/=ncnt;
			goto _lfend;
			break;
		}
		ret_color[0]=float(sum[0])/(255.0*ncnt);
		ret_color[1]=float(sum[1])/(255.0*ncnt);
		ret_color[2]=float(sum[2])/(255.0*ncnt);
		ret_color[3]=float(sum[3])/(255.0*ncnt);
		break;
	case GL_RGB:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:	
			while(cnt--)
			{
				sum[0]+=*neighbor[cnt];
				sum[1]+=*(neighbor[cnt]+1);
				sum[2]+=*(neighbor[cnt]+2);
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:			
			while(cnt--)
			{
				sum[0]+=*((unsigned short*)neighbor[cnt]);
				sum[1]+=*(((unsigned short*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned short*)neighbor[cnt])+2);
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			while(cnt--)
			{
				sum[0]+=*((unsigned int*)neighbor[cnt]);
				sum[1]+=*(((unsigned int*)neighbor[cnt])+1);
				sum[2]+=*(((unsigned int*)neighbor[cnt])+2);
			}
			break;
		case GL_FLOAT:
			ret_color[0]=ret_color[1]=ret_color[2]=0.0;ret_color[3]=1.0;
			while(cnt--)
			{
				ret_color[0]+=*((float*)neighbor[cnt]);
				ret_color[1]+=*(((float*)neighbor[cnt])+1);
				ret_color[2]+=*(((float*)neighbor[cnt])+2);
			}
			ret_color[0]/=ncnt;ret_color[1]/=ncnt;ret_color[2]/=ncnt;
			goto _lfend;
			break;
		}
		ret_color[0]=float(sum[0])/(255.0*ncnt);
		ret_color[1]=float(sum[1])/(255.0*ncnt);
		ret_color[2]=float(sum[2])/(255.0*ncnt);
		ret_color[3]=1.0;
		break;
	}
_lfend:;
}
void tex2DNearestf(GLdouble s,GLdouble t,float* ret_color)
{
	GLTextureObject* obj=gl_context.current_tex_object;
	if(s>1.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=1.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s-=(int)s;
	}
	else if(s<0.0)
	{
		if(obj->wrap_s_2d==GL_CLAMP) s=0.0;
		else if(obj->wrap_s_2d==GL_REPEAT) s=(int)(-s-1e-9)+1.0-s;
	}
	if(t>1.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=1.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t-=(int)t;
	}
	else if(t<0.0)
	{
		if(obj->wrap_t_2d==GL_CLAMP) t=0.0;
		else if(obj->wrap_t_2d==GL_REPEAT) t=(int)(-t-1e-9)+1.0-t;
	}

	byte* offset=((byte*)(obj->tex_image_2d[0].texels))+
		(((int)((obj->tex_image_2d[0].height-1)*t+0.5))*obj->width_bytes+
		((int)((obj->tex_image_2d[0].width-1)*s+0.5))*obj->units);

	switch(obj->tex_image_2d[0].format)
	{
	case GL_RGBA:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:				
			{
				byte* entry=(byte*)offset;				
				ret_color[0]=entry[0]/255.0;
				ret_color[1]=entry[1]/255.0;
				ret_color[2]=entry[2]/255.0;
				ret_color[3]=entry[3]/255.0;
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			{
				unsigned short* entry=(unsigned short*)offset;					
				ret_color[0]=entry[0]/255.0;
				ret_color[1]=entry[1]/255.0;
				ret_color[2]=entry[2]/255.0;
				ret_color[3]=entry[3]/255.0;
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			{unsigned int* entry=(unsigned int*)offset;					
			ret_color[0]=entry[0]/255.0;
			ret_color[1]=entry[1]/255.0;
			ret_color[2]=entry[2]/255.0;
			ret_color[3]=entry[3]/255.0;
			}
			break;
		case GL_FLOAT:
			{float* entry=(float*)offset;				
			ret_color[0]=entry[0];
			ret_color[1]=entry[1];
			ret_color[2]=entry[2];
			ret_color[3]=entry[3];
			}
			break;
		}
		break;
	case GL_RGB:
		switch(obj->tex_image_2d[0].type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:				
			{
				byte* entry=(byte*)offset;					
				ret_color[0]=entry[0]/255.0;
				ret_color[1]=entry[1]/255.0;
				ret_color[2]=entry[2]/255.0;
			}
			break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			{
				unsigned short* entry=(unsigned short*)offset;					
				ret_color[0]=entry[0]/255.0;
				ret_color[1]=entry[1]/255.0;
				ret_color[2]=entry[2]/255.0;
			}
			break;
		case GL_UNSIGNED_INT:
		case GL_INT:
			{unsigned int* entry=(unsigned int*)offset;				
			ret_color[0]=entry[0]/255.0;
			ret_color[1]=entry[1]/255.0;
			ret_color[2]=entry[2]/255.0;
			}
			break;
		case GL_FLOAT:
			{float* entry=(float*)offset;				
			ret_color[0]=entry[0];
			ret_color[1]=entry[1];
			ret_color[2]=entry[2];
			}
			break;
		}
		ret_color[3]=1.0;
		break;
	}
}

