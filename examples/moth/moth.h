#ifndef _MOTH__
#define _MOTH__

/*sssssssssssssssss Make Satellite Body and Shadow ssssssssssssssssssssssss*/

#define glCallList_satellite1 \
	glPushMatrix();\
	drawSatellite(satellite_diffuse, satellite_ambient, satellite_specular, satellite_shiny);\
	glPopMatrix();

#define glCallList_satellite2 \
	glPushMatrix();\
	drawSatellite(shadow_diffuse, shadow_ambient, shadow_specular, shadow_shiny);\
	glPopMatrix();

/*sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss*/

/*@@@@@@@@@@ Make Lamp Post and Lamp @@@@@@@@@@@@*/

#define glCallList_21 \
	glPushMatrix();glPushAttrib(GL_LIGHTING_BIT);glMaterialfv(GL_FRONT, GL_AMBIENT, lamp_post_specular);glScalef(0.07, 1.45, 0.07);drawCube(lamp_post_diffuse, lamp_post_ambient);glPopAttrib();glPopMatrix();glPushMatrix();glTranslatef(0.0, -1.45, -5.0);glScalef(0.3, 0.05, 0.3);drawCube(wall_color, cube_ambient);glPopMatrix();


#define glCallList_22 \
	glPushMatrix();\
	glPushAttrib(GL_LIGHTING_BIT); \
	glMaterialfv(GL_FRONT, GL_AMBIENT, lamp_ambient);\
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lamp_diffuse);	\
	glMaterialfv(GL_FRONT, GL_SPECULAR, lamp_specular);	\
	glTranslatef(0.0, 1.6, -5.0);\
	glutSolidSphere(0.3, 20.0, 20.0);\
	glPopAttrib();\
	glPopMatrix();

/*** Lamp post base shadow ***/

#define glCallList_501 \
	glPushMatrix();\
	glPushAttrib(GL_LIGHTING_BIT); \
	glMaterialfv(GL_FRONT, GL_AMBIENT, shadow_ambient);\
	glMaterialfv(GL_FRONT, GL_DIFFUSE, shadow_diffuse);\
	glMaterialfv(GL_FRONT, GL_SPECULAR, shadow_specular);\
	glMaterialfv(GL_FRONT, GL_SHININESS, shadow_shiny);\
	glTranslatef(0.0, -1.49, -5.0);\
	glRotatef(-90.0, 1.0, 0.0, 0.0);\
	glScalef(0.7, 0.7, 1.0);\
	drawOct();\
	glPopAttrib();\
	glPopMatrix();





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

/*ppppppppppppppppppppppppppp Make Solar Panels and Shadows pppppppppppppppppp*/

#define glCallList_panel1 \
	glPushMatrix();\
	drawPanels(panel_color, panel_ambient);\
	glPopMatrix();

#define glCallList_panel2 \
	glPushMatrix();\
	drawPanels(shadow_diffuse, shadow_ambient);\
	glPopMatrix();


/*pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp*/

/*========= Make Floor ==============*/ 

#define glCallList_ground \
	glPushMatrix();\
	glPushAttrib(GL_LIGHTING_BIT);\
	glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_color);\
	glMaterialfv(GL_FRONT, GL_AMBIENT, shadow_ambient);\
	glTranslatef(0.0, -1.5, -5.0);\
	glRotatef(-90.0, 1, 0, 0);\
	glScalef(5.0, 5.0, 1.0);\
	drawGround();\
	glPopAttrib();\
	glPopMatrix();

/*==================================*/



/*||||||||||| Make Left Wall |||||||||||||*/

#define glCallList_left_wall \
	glPushMatrix();\
	glPushAttrib(GL_LIGHTING_BIT);\
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);\
	glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);\
	glTranslatef(0.0, -1.5, 0.0);\
	glTranslatef(0.0, 1.2, 0.0);\
	glTranslatef(0.0, 0.0, -5.0);\
	glTranslatef(-5.0, 0.0, 0.0);\
	glRotatef(90.0, 0, 1, 0);\
	glScalef(4.5, 1.2, 1.0);\
	glNormal3f (0.0, 0.0, 1.0);\
	drawGround();\
	glPopAttrib();\
	glPopMatrix();

/*||||||||||||||||||||||||||||||||||||||||*/


#define glCallList_right_wall \
	glPushMatrix();\
	glPushAttrib(GL_LIGHTING_BIT);\
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);\
	glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);\
	glTranslatef(0.0, -1.5, 0.0);\
	glTranslatef(0.0, 1.2, 0.0);\
	glTranslatef(0.0, 0.0, -5.0);\
	glTranslatef(5.0, 0.0, 0.0);\
	glRotatef(270.0, 0, 1, 0);\
	glScalef(4.5, 1.2, 1.0);\
	glNormal3f (0.0, 0.0, 1.0);\
	drawGround();\
	glPopAttrib();\
	glPopMatrix();



/*[[[[[[[[[[[ Build Columns ]]]]]]]]]]]*/


#define glCallList_1 \
	glPushMatrix();\
	glScalef(0.4, 1.4, 0.4);\
	drawCube(column_color, column_ambient);\
	glPopMatrix();


#define glCallList_2 \
	glPushMatrix();\
	glTranslatef(0.0, -1.45, 0.0);\
	glScalef(0.5, 0.1, 0.5);\
	drawCube(wall_color, cube_ambient);\
	glPopMatrix();\
	glPushMatrix();\
	glTranslatef(0.0, 1.45, 0.0);\
	glScalef(0.5, 0.1, 0.5);\
	drawCube(wall_color, cube_ambient);\
	glPopMatrix();



#define glCallList_column \
	glPushMatrix();\
	glCallList_1;\
	glCallList_2;\
	glPopMatrix();


/***** Place columns at front of scene. *****/

#define glCallList_4 \
	glPushMatrix();\
	glTranslatef(-5.0, 0.0, -0.5);\
	glCallList_column;\
	glPopMatrix();


#define glCallList_5 \
	glPushMatrix();\
	glTranslatef(-1.75, 0.0, -0.5);\
	glCallList_column;\
	glPopMatrix();


#define glCallList_6 \
	glPushMatrix();\
	glTranslatef(1.75, 0.0, -0.5);\
	glCallList_column;\
	glPopMatrix();


#define glCallList_17 \
	glPushMatrix();\
	glTranslatef(5.0, 0.0, -0.5);\
	glCallList_column;\
	glPopMatrix();

#define glCallList_four_columns \
	glPushMatrix();\
	glCallList_4;\
	glCallList_5;\
	glCallList_6;\
	glCallList_17;\
	glPopMatrix();


/***** Make two columns for sides of scene *****/

#define glCallList_two_columns \
	glPushMatrix();\
	glRotatef(90.0, 0.0, 1.0, 0.0);\
	glTranslatef(5.0, 0.0, -5.0);\
	glPushMatrix();\
	glTranslatef(0.0, 0.0, -0.3);\
	glCallList_column;\
	glPopMatrix();\
	glPushMatrix();\
	glTranslatef(0.0, 0.0, 10.3);\
	glCallList_column;\
	glPopMatrix();\
	glPopMatrix();





/*[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]*/


/* .......................Make shadows .........................*/

#define glCallList_8 \
	glPushMatrix();\
	glScalef(0.4, 1.4, 0.4);\
	drawCube(shadow_diffuse, shadow_ambient);\
	glPopMatrix();



#define glCallList_9 \
	glPushMatrix();\
	glTranslatef(0.0, -1.45, 0.0);\
	glScalef(0.5, 0.1, 0.5);\
	drawCube(shadow_diffuse, shadow_ambient);\
	glPopMatrix();\
	glPushMatrix();\
	glTranslatef(0.0, 1.45, 0.0);\
	glScalef(0.5, 0.1, 0.5);\
	drawCube(shadow_diffuse, shadow_ambient);\
	glPopMatrix();


#define glCallList_10 \
	glPushMatrix();\
	glCallList_8;\
	glCallList_9;\
	glPopMatrix();


#define glCallList_11 \
	glPushMatrix();\
	glTranslatef(-5.0, 0.0, -0.5);\
	glCallList_10;\
	glPopMatrix();


#define glCallList_12 \
	glPushMatrix();\
	glTranslatef(-1.75, 0.0, -0.5);\
	glCallList_10;\
	glPopMatrix();


#define glCallList_13 \
	glPushMatrix();\
	glTranslatef(1.75, 0.0, -0.5 );\
	glCallList_10;\
	glPopMatrix();


#define glCallList_14 \
	glPushMatrix();\
	glTranslatef(5.0, 0.0, -0.5 );\
	glCallList_10;\
	glPopMatrix();


#define glCallList_15 \
	glPushMatrix();\
	glCallList_11;\
	glCallList_12;\
	glCallList_13;\
	glCallList_14;\
	glPopMatrix();


#define glCallList_100 \
	glPushMatrix();\
	glMultMatrixf((const GLfloat *) shadowMat_ground);\
	glTranslatef(-mvt_x, -mvt_y, -mvt_z);\
	glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);\
	glMultMatrixf((const GLfloat *) four_columnsXform);\
	glCallList_15;\
	glPopMatrix();


#define glCallList_101 \
	glPushMatrix();\
	glMultMatrixf((const GLfloat *) shadowMat_left);\
	glTranslatef(-mvt_x, -mvt_y, -mvt_z);\
	glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);\
	glMultMatrixf((const GLfloat *) four_columnsXform);\
	glCallList_15;\
	glPopMatrix();


#define glCallList_102 \
	glPushMatrix();\
	glMultMatrixf((const GLfloat *) shadowMat_back);\
	glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);\
	glTranslatef(-mvt_x, -mvt_y, -mvt_z);\
	glMultMatrixf((const GLfloat *) four_columnsXform);\
	glCallList_15;\
	glPopMatrix();


#define glCallList_103 \
	glPushMatrix();\
	glMultMatrixf((const GLfloat *) shadowMat_right);\
	glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);\
	glTranslatef(-mvt_x, -mvt_y, -mvt_z);\
	glMultMatrixf((const GLfloat *) four_columnsXform);\
	glCallList_15;\
	glPopMatrix();



/* ......................................................*/


#endif