#include<cmath>

extern "C" void pixel(int x, int y, unsigned char *pix);
extern "C" void drawn();

static double scale_r = 1.0, scale_g = 10.0, scale_b = 1.0;

// pixel operation here, called once for every pixel each frame
void pixel(int x, int y, unsigned char *pix) {
    pix[0] *= scale_r;
    pix[1] *= scale_g;
    pix[2] *= scale_b;
}
// called when frame is done drawing
void drawn() {
    
    scale_r += 0.01f;
    scale_g -= 0.01f;
    scale_b += 0.05f;
    
    if(scale_r >= 10) scale_r = 1.0;
    if(scale_g <= 1) scale_g = 10.0;
    if(scale_b >= 5) scale_b = 1.0;
    
}

