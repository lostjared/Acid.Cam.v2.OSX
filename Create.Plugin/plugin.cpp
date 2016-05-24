
#include<iostream>

extern "C" void pixel(int x, int y, unsigned char *pix);
extern "C" void drawn();

static float alpha = 1.0f;


void pixel(int x, int y, unsigned char *pix) {
	pix[0] = pix[0]*alpha;
	pix[1] = pix[1]*alpha;
	pix[2] = pix[2]*alpha;
}

void drawn() {
	alpha += 0.01f;
}