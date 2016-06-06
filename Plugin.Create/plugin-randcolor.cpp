#include<iostream>


extern "C" void pixel(int x, int y, unsigned char *pix);
extern "C" void drawn();


int pixel_shift = 0;
float alpha = 1.0f;
bool switch_value = false;

void pixel(int x, int y, unsigned char *pix) {
	pix[0] = pix[0]*alpha;
	pix[1] = pix[1]*alpha;
	pix[2] = pix[2]*alpha;
	pix[pixel_shift] = ~pix[pixel_shift];
}

void drawn() {
	
	switch_value = !switch_value;
	
	++pixel_shift;
	if(pixel_shift >= 3) {
		pixel_shift = 0;
	}
	
	static int direction = 1;
	static float out_offset = 0, in_offset = 10;
	
	if(direction == 1) {
		alpha += 0.1f;
		if(alpha >= out_offset) {
			direction = 2;
			++out_offset;
			if(out_offset >= 10) out_offset = 0;
		}
		
		
	} else if(direction == 2) {
		alpha -= 0.1f;
		if(alpha <= in_offset) {
			direction = 1;
			--in_offset;
			if(in_offset <= 1) {
				in_offset = 10;
			}
		}
	}
}
