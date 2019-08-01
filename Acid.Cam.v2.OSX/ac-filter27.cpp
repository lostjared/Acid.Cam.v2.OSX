
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
 auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
 for(int i = 0; i < cols; ++i) {
 }
 }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
 
 */

#include"ac.h"

void ac::SingleFrameGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    if(collection.empty() || collection.bounds() != frame.size()) {
        collection.resetFrame(frame);
    }
    static int frame_counter = 0;
    static double seconds = 0;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        ++seconds;
        frame_counter = 0;
    }
    if(seconds >= 1) {
        seconds = 0;
        collection.shiftFrames(frame);
        Smooth(frame, &collection);
    }
    AddInvert(frame);
}

void ac::ColorFadeSlow(cv::Mat &frame) {
    static double val[3] = {-1.0, -1.0, -1.0};
    static int init = -1;
    // lazy
    if(init == -1) {
        val[0] = rand()%255;
        val[1] = rand()%255;
        val[2] = rand()%255;
        init = 0;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(val[j]);
            }
        }
    }
    static int dir[3] = {1,1,1};
    static double speed[3] = {0.33, 0.33, 0.33};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            val[j] += speed[j];
            if(val[j] >= 255) {
                val[j] = rand()%255;
                dir[j] = 0;
                speed[j] += 0.33;
                if(speed[j] > 5) speed[j] = 1;
            }
        } else if(dir[j] == 0) {
            val[j] -= speed[j];
            if(val[j] <= 1) {
                val[j] = rand()%255;
                dir[j] = 1;
                speed[j] += 0.33;
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
}

void ac::FadeBetweenSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "FadeBetweenSubFilter")
        return;
    static cv::Mat stored_copy = frame.clone();
    cv::Mat copy1 = frame.clone();
    if(stored_copy.empty() || (stored_copy.size() != frame.size())) {
        stored_copy = frame.clone();
        CallFilter(subfilter, stored_copy);
    }
    static double alpha1 = 1.0;
    static int dir = 1;
    if(alpha1 >= 0.9) {
        stored_copy = frame.clone();
        CallFilter(subfilter, stored_copy);
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, stored_copy, frame, alpha1, (1-alpha));
    AddInvert(frame);
}
