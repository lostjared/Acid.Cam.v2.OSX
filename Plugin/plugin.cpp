
#include<iostream>

extern "C" void pixel(int x, int y, unsigned char *pix);


void pixel(int x, int y, unsigned char *pix) {
	static float alpha = 1.0f;
	pix[0] = pix[0]*alpha;
	pix[1] = pix[1]*alpha;
	pix[2] = pix[2]*alpha;
	alpha += 0.1f;
	std::cout << x << ":" << y << " = {" << (unsigned int)pix[0] << " " << (unsigned int)pix[1] << " " << (unsigned int) pix[2] << "\n";
	
}