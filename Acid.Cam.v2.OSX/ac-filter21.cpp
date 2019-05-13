/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/lostsidedead
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2019, Jared Bruni
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

/*
 //Basic Multithreaded Filter
 auto callback = [&](cv::Mat frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
    for(int i = 0; i < frame.cols; ++i) {
        }
    }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
*/

#include "ac.h"

void ac::ColorShiftXor(cv::Mat &frame) {
    static int rgb_value[3] = { 32, 64, 128 };
    static int dir[3] = {1,1,1};
    static int speed = 6;
    static int speed_dir[3] = {1,1,1};
    static auto callback = [&](cv::Mat frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] -= (pixel[j]^rgb_value[j]);
                }
                
            }
        }
    };
    
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_value[j] += (rand()%speed);
            if(rgb_value[j] >= 255) {
                rgb_value[j] = rand()%255;
                dir[j] = 0;
                if(speed_dir[j] == 1) {
                    speed += 2;
                    if(speed > 15) {
                        speed_dir[j] = 0;
                        speed = 15;
                    }
                } else {
                    speed -= 2;
                    if(speed <= 3) {
                        speed_dir[j] = 1;
                        speed = 3;
                    }
                }
            }
        } else {
            rgb_value[j] -= (rand()%speed);
            if(rgb_value[j] <= 0) {
                rgb_value[j] = rand()%255;
                dir[j] = 1;
                if(speed_dir[j] == 1) {
                    speed += 2;
                    if(speed > 15) {
                        speed_dir[j] = 0;
                        speed = 15;
                    }
                } else {
                    speed -= 2;
                    if(speed <= 3) {
                        speed_dir[j] = 1;
                        speed = 3;
                    }
                }
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RandomSquares(cv::Mat &frame) {
    cv::Vec3b color(rand()%255, rand()%255, rand()%255);
    int num_values = 10+(rand()%frame.rows/10);
    for(int j = 0; j < num_values; ++j) {
        ac::Rect rc(5+(rand()%frame.cols-10), 5+(rand()%frame.rows-10),10+(rand()%200), 10+(rand()%50));
        for(int i = rc.x; i < rc.x+rc.w; ++i) {
            for(int z = rc.y; z < rc.y+rc.h; ++z) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        pixel[q] = pixel[q]^color[q];
                    }
                }
            }
        }
                    
    }
}

void ac::RandomImageSquares(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Vec3b color(rand()%255, rand()%255, rand()%255);
    int num_values = 10+(rand()%frame.rows/10);
    cv::Mat reimage;
    cv::resize(blend_image, reimage, frame.size());
    for(int j = 0; j < num_values; ++j) {
        ac::Rect rc(5+(rand()%frame.cols-10), 5+(rand()%frame.rows-10),10+(rand()%200), 10+(rand()%50));
        for(int i = rc.x; i < rc.x+rc.w; ++i) {
            for(int z = rc.y; z < rc.y+rc.h; ++z) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        pixel[q] = pixel[q]^pix[q];
                    }
                }
            }
        }
        
    }
}
