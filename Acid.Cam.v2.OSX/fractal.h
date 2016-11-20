/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com )
 * (C) 2016 GPL
 */

#ifndef __FRACTAL__H_
#define __FRACTAL__H_

#include<complex>
#include"opencv2/opencv.hpp"
#include<string>

namespace frac {
	void FractalLogic();
	void DrawFractal(cv::Mat &frame, bool neg=false);
}

#endif

