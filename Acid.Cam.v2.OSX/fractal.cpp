#include "fractal.h"

namespace frac {
	double paramA = 0.519;
	double paramB = 0.688;
	float zoom = 1;
	double red_color = 3.3;
	double green_color = 3.3;
	float mod_x = 0.5;
	float mod_y = 0.5;
	double color_r = 0;
	double color_g = 100;
	double color_base = 0.0;
	int dir = 1;
}

void frac::FractalLogic() {
	static double speed = 0.20;
	if (dir == 1) {
		if (paramA < 1.5f)
			paramA += speed;

		if (paramB > 0)
			paramB -= speed;

		if (paramA >= 1.5f)
			dir = 2;

		if (red_color < 4.4)
			red_color += 0.1;
		else
			red_color = 0;

		if (color_r < 100)
			color_r += 0.1;
		else
			color_r = 0;
		if (color_g > 0)
			color_g -= 0.1;
		else
			color_g = 100;
	} else {
		if (paramA > 0)
			paramA -= speed;

		if (paramB < 5.5)
			paramB += speed;

		if (paramA <= 0) {
			dir = 1;
			color_base += 5;
		}
	}
	if (zoom < 6)
		zoom += 0.1;
	else
		zoom = 0.1;
}


void frac::DrawFractal(cv::Mat &frame, bool neg)
{
	static float alpha = 1.0f;
	float x1=mod_x-1.0f*zoom;
	float x2=mod_x+1.0f*zoom;
	float y1=mod_y-1.0f*zoom;
	float y2=mod_y+1.0f*zoom;
	static float radius = 4.0f;
	int Width=frame.cols, Height=frame.rows;
	std::complex<double> C ((double)-paramA, (double)+paramB);
	std::complex<double> Z, Z0, Zt;
	int i;
	bool Fini;
	for (int x=0; x<Width;++x)
	{
		for (int y=0; y<Height; ++y)
		{
			Z=Z0=std::complex<double>((double)(x*(x2-x1)/Width+x1), (double)(y*(y2-y1)/Height+y1));
			Fini=false;
			for (i=0; i<100 && (!Fini); i++)
			{
				Z=Z*Z+C;
				Zt=Z-Z0;
				if( (sqrt(Zt.real()*Zt.real()+Zt.imag()*Zt.imag())) > radius) Fini = true;
			}
				cv::Vec3b &cf = frame.at<cv::Vec3b>(y, x);
				cf[2] += (255-i*2-color_base);
				cf[1] += sin(i*green_color/100)*200-color_g;
				cf[0] += sin(i*red_color/100)*255-color_r;
				cf[2] *= alpha;
				cf[1] *= -alpha;
				cf[0] *= alpha;
				if(neg == true) {
					cf[2] = -cf[2];
					cf[1] = -cf[1];
					cf[0] = -cf[2];
				}
		}
		if(x > frame.size().width) break;
	}
	static bool direction = true;
	if(direction == true) {
		radius += 1.0f;
		alpha += 0.01f;
	}
	else {
		alpha -= 0.01f;
		radius -= 1.0f;
	}
	if(alpha > 6) { direction = false; }
	if(alpha <= -6) { direction = true; }

}

