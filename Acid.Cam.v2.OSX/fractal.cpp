/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com )
 * (C) 2016 GPL
 */
#include "fractal.h"

namespace frac {
    double paramA = -1.0;//0.519;
    double paramB = 0.2;
    float zoom_x = 1.5, zoom_y = 1.5, zoom_w = 0.5, zoom_h = 0.5;
    double red_color = 6;
    double green_color = 25;
    double blue_color = 50;
    float mod_x = 0.5;
    float mod_y = 0.5;
    float radius = 180.0f;
    int dir = 1;
    long max_iter = 40;
}

void frac::FractalLogic() {
    static double alpha_r = 1.0;
    alpha_r += 0.05;
    red_color += alpha_r;
    green_color += alpha_r;
    blue_color += alpha_r;
    if(alpha_r > 255) alpha_r = 1.0;
    if(red_color > 255) red_color = rand()%255;
    if(green_color > 255) green_color = rand()%255;
    if(blue_color > 255) blue_color = rand()%255;
    
    switch(dir) {
        case 1: {
            paramA += 0.005;
            if(paramA >= 1.0) {
                dir = 2;
                paramB += 0.05;
                if(paramB >= 1.0) {
                    dir = 3;
        
                }
            }
        }
            break;
        case 2: {
            paramA -= 0.005;
            if(paramA <= -1) {
                paramB += 0.05;
                dir = 1;
                if(paramB >= 1.0) {
                    dir = 4;
                }
            }
        }
            break;
        case 3: {
            paramB -= 0.005;
            if(paramB <= -1.0) {
                dir = 3;
                paramA -= 0.05;
                if(paramA <= -1) {
                    dir = 1;
                }
            }
            
        }
            break;
        case 4: {
            paramB += 0.005;
            if(paramB >= 1) {
                paramA += 0.05;
                dir = 3;
                if(paramA >= 1) {
                    dir = 2;
                }
                
            }
        }
    
            break;
    }
   // std::cout << paramA << ": " << paramB << " :" << " dir: " << dir << "\n";
}

void frac::DrawFractal(cv::Mat &frame, bool neg)
{
    float x1=mod_x-1.0f*zoom_x;
    float x2=mod_x+1.0f*zoom_w;
    float y1=mod_y-1.0f*zoom_y;
    float y2=mod_y+1.0f*zoom_h;
    int width=frame.cols, height=frame.rows;
    std::complex<double> C (paramA, paramB);
    std::complex<double> Z;
    int i = 0;
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            //C=std::complex<double>((double)(x*(x2-x1)/width+x1), (double)(y*(y2-y1)/height+y1));
            Z=std::complex<double>((double)(x*(x2-x1)/width+x1), (double)(y*(y2-y1)/height+y1));
            //Z=std::complex<double>(0, 0);
            for (i = 0; i < max_iter && std::abs(Z) < 2; i++)
            {
                Z=Z*Z+C;
            }
            cv::Vec3b &cf = frame.at<cv::Vec3b>(y, x);
            if(i == max_iter) {
        
            } else {
                cf[2] += sin(i*red_color/100)*255;
                cf[1] += sin(i*green_color/100)*255;
                cf[0] += sin(i*blue_color/100)*255;
            }
        }
        if(x > frame.size().width) break;
    }
}

