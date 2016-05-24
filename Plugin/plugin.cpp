
#include<iostream>

extern "C" void pixel(int x, int y, unsigned char *pix);



void pixel(int x, int y, unsigned char *pix) {
	static float alpha = 1.0f;
	pix[0] = pix[0]*alpha;
	pix[1] = pix[1]*y;
	pix[2] = pix[2]*x;

	alpha += 0.001f;
}

