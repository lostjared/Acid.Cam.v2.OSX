
#include<iostream>

extern "C" void pixel(int x, int y, unsigned char *pix);
extern "C" void drawn();


unsigned int pos = 10;


void pixel(int i, int z, unsigned char *buffer) {
	
	buffer[0] = ((i+z)*pos)/(i+z+1)+buffer[0]*pos;
	buffer[1] += ((i*pos)/(z+1))+buffer[1];
	buffer[2] += ((z*pos)/(i+1))+buffer[2];
	
}

void drawn() {
	++pos;
}