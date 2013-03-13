#ifndef _YGL_UNITTEST_CALLBACKS_
#define _YGL_UNITTEST_CALLBACKS_

namespace conflict{
	void swapbuf();
}

void reshape(int width, int height);
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

#endif