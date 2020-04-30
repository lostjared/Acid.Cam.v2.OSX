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
 
 Copyright (c) 2020, Jared Bruni
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

int ac::Box::frame_width = 0;
int ac::Box::frame_height = 0;

void ac::Box::initBox(int width, int height) {
    x = (rand()%width);
    y = (rand()%height);
    w = rand()%25;
    h = rand()%25;
    steps = rand()%10;
    index = 0;
    do {
        frame_index = rand()%28;
    } while(frame_index == 13 || frame_index == 14);
}

void ac::Box::drawBox(cv::Mat &frame) {
    cv::Mat temp;
    temp.create(cv::Size(w, h), CV_8UC3);
    for(int yy = y, pos_y = 0; yy < y+h && yy < frame_height; ++yy, ++pos_y) {
        for(int ii = x,pos_x = 0; ii < x+w && ii < frame_width; ++ii, ++pos_x) {
            cv::Vec3b pixel = pixelAt(frame,yy, ii);
            cv::Vec3b &target = temp.at<cv::Vec3b>(pos_y, pos_x);
            target = pixel;
        }
    }
    CallFilter(frame_index, temp);
    for(int z = y, pos_y = 0; z < y+h && z < frame_height; ++z, ++pos_y) {
        for(int i = x, pos_x = 0; i < x+w && i < frame_width; ++i, ++pos_x) {
            if(i < frame.cols && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel = temp.at<cv::Vec3b>(pos_y,pos_x);
            }
        }
    }
}

void ac::Box::sizeBox() {
    if(index > steps) {
        initBox(frame_width, frame_height);
        return;
    }
    ++index;
    int r1 = rand()%10;
    int r2 = rand()%10;
    if(w+r1 > frame_width) {
        initBox(frame_width, frame_height);
        return;
    } else {
        w += r1;
    }
    if(h+r2 > frame_height) {
        initBox(frame_width, frame_height);
        return;
    } else {
        h += r2;
    }
}

