#ifndef _YGL_TEST_CALLBACK_H_
#define _YGL_TEST_CALLBACK_H_

int init();
void RenderCallBack();
void ReshapeCallback(int width, int height);
void MouseCallBack(int button, int state, int x, int y);
void MotionCallBack(int button,int x, int y);
void PassiveMotionCallBack(int x, int y);
void MouseWheelCallBack(int rCount);
void KeyboardCallBack(unsigned char key);

#endif