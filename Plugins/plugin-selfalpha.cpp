
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
	
	static int direction = 1;
	static int draw_max = 5;
	
	if(direction == 1) {
		alpha += 0.1f;
		
		if(alpha > draw_max) {
			direction = 0;
			++draw_max;
			if(draw_max > 10) {
				draw_max = 5;
			}
		}
		
	} else {
		alpha -= 0.1f;
		if(alpha <= 1) direction = 1;
	}
}