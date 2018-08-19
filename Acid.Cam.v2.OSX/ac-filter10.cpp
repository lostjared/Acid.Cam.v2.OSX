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

void ac::ExpandSquareSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = frame_copy.at<cv::Vec3b>(z, i);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
}

void ac::ExpandSquareBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) ^ static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::ExpandSquareVerticalSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareVerticalSubFilter")
        return;
    static int start_x = frame.rows/2;
    static int stop_x = frame.rows/2;
    static int speed = frame.rows/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(i, z);
            pixel = frame_copy.at<cv::Vec3b>(i, z);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.rows-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.rows/2)-1  || stop_x <= (frame.rows/2)-1) {
            dir = 1;
        }
    }
}

void ac::DarkImageMedianBlend(cv::Mat &frame) {
    if(blend_set == true) {
    	SmoothImageAlphaBlend(frame);
    	cv::Mat frame_copy = frame.clone();
    	setGamma(frame_copy,frame,5);
    	MedianBlend(frame);
    }
}
