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

void ac::SoftXor(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    RandomAmountMedianBlur(frame);
    collection.shiftFrames(frame);
    double alpha = 1.0, alpha_max = 3.0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b val = collection.frames[q].at<cv::Vec3b>(z, i);
                unsigned char v[3] = { static_cast<unsigned char>(s[0]), static_cast<unsigned char>(s[1]), static_cast<unsigned char>(s[2])};
                s[0] = (v[0] + val[0]) ^ static_cast<unsigned char>(s[0]);
                s[1] = (v[1] + val[1]) ^ static_cast<unsigned char>(s[1]);
                s[2] = (v[2] + val[2]) ^ static_cast<unsigned char>(s[2]);
            }
            
            s[0] /= collection.size();
            s[1] /= collection.size();
            s[2] /= collection.size();
            
            for(int j = 0; j < 3; ++j) {
                unsigned char v = static_cast<unsigned char>(s[j]);
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ v) * static_cast<unsigned char>(alpha));
            }
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 8.0, 0.1);
    
}

void ac::SelfXorBlend(cv::Mat &frame) {
    static unsigned char index[3] = {static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255)};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ index[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    for(int j = 0; j < 3; ++j)
        ++index[j];
}

void ac::SelfXorDoubleFlash(cv::Mat &frame) {
    static double alpha = 1.0;
    int r[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j] ^ (static_cast<unsigned int>(alpha)+r[j]);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    if(dir == 1) {
        alpha += alpha_increase;
        if(alpha > 255)
            dir = 0;
    } else {
        alpha -= alpha_increase;
        if(alpha <= 0)
            dir = 1;
    }
}

// No Filter
void ac::NoFilter(cv::Mat &) {}

// Alpha Blend with Original Frame
void ac::BlendWithSource(cv::Mat &frame) {
    ac::pass2_alpha = 0.50; // set to 50%
    Pass2Blend(frame);// call Pass2 function
}

