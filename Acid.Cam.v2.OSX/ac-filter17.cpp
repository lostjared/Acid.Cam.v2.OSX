/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
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

#include "ac.h"

void ac::DarkNegateRainbowMedian(cv::Mat &frame) {
    static MatrixCollection<8> collection1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    DarkNegate(copy1);
    SmoothRGB(copy1, &collection1);
    rainbowBlend(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::IncreaseQuick(cv::Mat &frame) {
    static double speed = 1.0;
    static double pixel_color[] = {0.1*(rand()%10), (0.1*(rand()%10)), (0.1*(rand()%10))};
    static int dir = 1, speed_dir = 1;
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += pixel[j]*pixel_color[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir == 1) {
            pixel_color[j] += speed;
            if(pixel_color[j] >= 5) {
                dir = 0;
            }
        } else {
            pixel_color[j] -= speed;
            if(pixel_color[j] <= 1.0) {
                dir = 1;
            }
        }
    }
    if(speed_dir == 1) {
        speed += 0.1;
        if(speed > 2.0) {
            speed_dir = 0;
            for(int j = 0; j < 3; ++j) {
                pixel_color[j] = 3.0;
            }
        }
    } else {
        speed -= 0.1;
        if(speed <= 1.0) {
            speed_dir = 1;
            for(int j = 0; j < 3; ++j) {
                pixel_color[j] = 1.0;
            }
        }
    }
}


void ac::IncreaseRandomIndex(cv::Mat &frame) {
    static int index = rand()%2, speed = 10;
    static int colors[3] = {0,75,150};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] += colors[index];
        }
    }
    for(int j = 0; j < 3; ++j ){
        colors[j] += speed;
        if(colors[j] >= 255) {
            colors[j] = 0;
            ++index;
            if(index > 2)
                index = rand()%2;
        }
    }
    AddInvert(frame);
}

void ac::ImageChannelSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageChannelSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    CallFilter(subfilter, copy1);
    cv::resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    cv::Mat chan[3], output;
    static int index = 0;
    static int values[3] = {0,1,2};
    switch(index) {
        case 0:
            values[0] = 0;
            values[1] = 1;
            values[2] = 2;
            break;
        case 1:
            values[0] = 1;
            values[1] = 2;
            values[2] = 0;
            break;
        case 2:
            values[0] = 2;
            values[1] = 0;
            values[2] = 1;
            break;
    }
    ++index;
    if(index > 2) index = 0;
    cv::extractChannel(copy1, chan[values[0]], 0);
    cv::extractChannel(reimage, chan[values[1]],1);
    cv::extractChannel(copy2, chan[values[2]], 2);
    cv::merge(chan, 3, output);
    AlphaBlend(output, copy2, frame, 0.5);
}
