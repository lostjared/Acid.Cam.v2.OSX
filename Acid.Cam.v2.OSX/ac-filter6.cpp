/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
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

#include"ac.h"

void ac::RandomXorFlash(cv::Mat &frame) {
    static int index = 0;
    int col_p[3] = { rand()%255, rand()%255, rand()%255 };
    int col_x[3] = { rand()%255, rand()%255, rand()%255 };
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ ((col_p[j] ^ col_x[j]) * static_cast<unsigned int>(alpha)));
                        break;
                    case 1:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ col_p[j]);
                        break;
                    case 2:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ (col_p[j]+col_x[j]));
                        break;
                }
            }
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
    ++index;
    if(index > 2) index = 0;
}

void ac::RandomAmountMedianBlur(cv::Mat &frame) {
    int random = rand()%10;
    for(int j = 0; j < random; ++j)
        MedianBlur(frame);
}

// No Filter
void ac::NoFilter(cv::Mat &) {}

// Alpha Blend with Original Frame
void ac::BlendWithSource(cv::Mat &frame) {
    ac::pass2_alpha = 0.50; // set to 50%
    Pass2Blend(frame);// call Pass2 function
}

