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

#include "ac.h"


void ac::MirrorMedianBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat mirror = frame.clone();
    RandomMirror(mirror);
    collection.shiftFrames(mirror);
    Smooth(frame, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = collection.frames[7].at<cv::Vec3b>(z, i);
            cv::Vec3b fpixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^fpixel[j])/2;
            }
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SubFilterMedianBlend(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SubFilterMedianBlend")
        return;
    static MatrixCollection<8> collection;
    cv::Mat mirror = frame.clone();
    CallFilter(subfilter, mirror);
    collection.shiftFrames(mirror);
    Smooth(frame, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = collection.frames[7].at<cv::Vec3b>(z, i);
            cv::Vec3b fpixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^fpixel[j])/2;
            }
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::DarkenBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static int dark = 2;
    cv::Mat copyf = frame.clone();
    cv::Mat copyo = frame.clone();
    DarkenImage(copyf, dark);
    ++dark;
    if(dark > 8)
        dark = 2;
    
    collection.shiftFrames(copyf);
    Smooth(copyo, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b cpypix = copyo.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = ((pixel[j]^cpypix[j])/2);
            }
        }
    }
}
