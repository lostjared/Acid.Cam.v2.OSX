/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
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

void frac::DrawFractal(cv::Mat &frame, bool)
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
                cf[2] += static_cast<unsigned char>(sin(i*red_color/100)*255);
                cf[1] += static_cast<unsigned char>(sin(i*green_color/100)*255);
                cf[0] += static_cast<unsigned char>(sin(i*blue_color/100)*255);
            }
        }
        if(x > frame.size().width) break;
    }
}

