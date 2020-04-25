

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

#include "ac.h"

void ac::LinesAcrossX(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 1; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b prev = frame.at<cv::Vec3b>(z, i-1);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^prev[j];
        }
    }
    AddInvert(frame);
}

void ac::XorLineX(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        static int rgb[3] = {0};
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                rgb[j] += pixel[j];
            }
            
        }
        rgb[0] /= frame.cols;
        rgb[1] /= frame.cols;
        rgb[2] /= frame.cols;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(0.5 * (pixel[j]^rgb[j])) + static_cast<unsigned char>(0.5 * pixel[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::AlphaComponentIncrease(cv::Mat &frame) {
    static int index = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] ^= static_cast<unsigned char>(alpha * pixel[index]);
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    static int dir = 1;
    if(dir == 1) {
        alpha += 0.01;
        if(alpha > 1.0)
            dir = 0;
    } else {
        alpha -= 0.01;
        if(alpha <= 0.1)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::ExpandContract(cv::Mat &frame) {
    static int nw = frame.cols;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
            pixel = pix;
        }
        static int dir = 1;
        if(dir == 1) {
            ++nw;
            if(nw > frame.cols * 3)
                dir = 0;
        } else {
            --nw;
            if(nw < frame.cols)
                dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MoveInThenMoveOut(cv::Mat &frame) {
    static int nw = frame.cols/3;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
            pixel = pix;
        }
        static int dir = 1;
        if(dir == 1) {
            ++nw;
            if(nw > frame.cols)
                dir = 0;
        } else {
            --nw;
            if(nw < frame.cols/3)
                dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MoveInThenMoveOutFast(cv::Mat &frame) {
    static int nw = frame.cols/8;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
            pixel = pix;
        }
        static int dir = 1;
        if(dir == 1) {
            nw += (rand()%20);
            if(nw > frame.cols)
                dir = 0;
        } else {
            nw -= (rand()%20);
            if(nw < frame.cols/8)
                dir = 1;
        }
    }
    AddInvert(frame);
}


#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
extern void ScreenGrab(cv::Mat &frame);
#endif

void ac::CurrentDesktop(cv::Mat &frame) {
#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
    cv::Mat cap;
    ScreenGrab(cap);
    cv::Mat temp;
    ac_resize(cap, temp, frame.size());
    cv::cvtColor(temp, frame, cv::COLOR_RGBA2BGR);
#endif
}

void ac::DistortionFuzz(cv::Mat &frame) {
    static int nw = frame.cols/8;
    cv::Mat copy1 = frame.clone();
    int rand_height = rand()%100;
    int rand_start = rand()%100;
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
            pixel = pix;
        }
        static int dir = 1;
        if(dir == 1) {
            nw += (rand()%20);
            if(nw > rand_height) {
                dir = 0;
                rand_height = rand()%100;
            }
        } else {
            nw -= (rand()%20);
            if(nw < (frame.cols/12)+rand_start) {
                dir = 1;
                rand_start = rand()%100;
            }
        }
    }
    AddInvert(frame);
}

void ac::DistortionByRow(cv::Mat &frame) {
    static int nw = frame.rows/16;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int col = AC_GetFZ(frame.rows, z, nw);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(col, i);
                pixel = pix;
            }
        }
        ++nw;
        if(nw > frame.rows-1)
            nw = frame.rows/16;
    }
    AddInvert(frame);
}
