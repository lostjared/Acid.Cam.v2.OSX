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

void ac::StretchColMatrix8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}
void ac::StretchColMatrix16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}
void ac::StretchColMatrix32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}



void ac::GradientRandomSwitch(cv::Mat &frame) {
    static std::vector<std::string> gradient_filter {"GradientColorBlend", "GradientRedBlend", "GradientGreenBlend", "GradientBlueBlend", "GradientXRed", "GradientXGreen", "GradientXBlue"};
    static std::default_random_engine random(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static int index = 0, num_filter = 1+(rand()%3);
    if(index == 0) {
        std::shuffle(gradient_filter.begin(), gradient_filter.end(), random);
        index = 1;
        num_filter = 1+(rand()%4);
    }
    static int frame_counter = 0, frame_rand = 0, c_fps = static_cast<int>(ac::fps);
    frame_rand = c_fps+(rand()%50);
    ++frame_counter;
    if(frame_counter > frame_rand) {
        frame_counter = 0;
        frame_rand = c_fps+(rand()%50);
        index = 0;
    }
    for(int i = 0; i < num_filter; ++i) {
        CallFilter(gradient_filter[i], frame);
    }
    AddInvert(frame);
}

void ac::GradientFlashColor(cv::Mat &frame) {
    double alpha[3] = {0};
    double r_color = rand()%255;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (alpha[j] * r_color));
            }
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AddInvert(frame);
}

void ac::GradientFlashComponent(cv::Mat &frame) {
    double alpha[3] = {0};
    double r_color = rand()%255;
    double alpha_inc = 0.5/frame.rows;
    int offset = rand()%3;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[offset] = static_cast<unsigned char>((0.5 * pixel[offset]) + (alpha[offset] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AddInvert(frame);
}
