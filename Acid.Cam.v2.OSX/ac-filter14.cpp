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


void ac::MirrorAlphaBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone(), copy4;
    MirrorBitwiseXor(copy1);
    MirrorXorAll(copy2);
    AlphaBlend(copy1, copy2, copy4, 0.5);
    AlphaBlend(copy3, copy4, frame, 0.5);
    BlendWithSource(frame);
    DarkenFilter(frame);
    MirrorBitwiseXor(frame);
}

void ac::ImageSmoothMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    
    rainbowBlend(frame);
    pushSubFilter(filter_map["ExactImage"]);
    SmoothSubFilter32(frame);
    popSubFilter();
    DarkenFilter(frame);
    MedianBlend(frame);
}

void ac::ImageSmoothMedianSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageSmoothMedianSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    rainbowBlend(copy1);
    pushSubFilter(filter_map["ExactImage"]);
    SmoothSubFilter32(copy1);
    popSubFilter();
    DarkenFilter(copy1);
    MedianBlend(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    CallFilter(subfilter, frame);
}

void ac::ImageAlphaXorMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    SmoothImageAlphaBlend(copy1);
    ImageSmoothMedianBlend(copy2);
    static double alpha = 1.0, alpha_max = 4.0;
    AlphaXorBlend(copy1, copy2, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
}

// use with Custom as Subfilter
void ac::MatrixCollectionBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    collection.shiftFrames(copy1);
    for(int i = 0; i < collection.size(); ++i) {
        ShuffleAlpha(collection.frames[i]);
        MedianBlur(collection.frames[i]);
    }
    Smooth(frame, &collection, false);
}

// use with Custom goes good with RainbowXorBlend
void ac::MatrixCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    for(int i  = 0; i < collection.size(); ++i) {
        CallFilter(subfilter, collection.frames[i]);
    }
    Smooth(copy2, &collection, false);
    AlphaBlend(copy1, copy2, frame, 0.5);
}

void ac::MatrixCollectionImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionImageSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    collection.shiftFrames(reimage);
    collection.shiftFrames(copy1);
    Smooth(copy1, &collection, false);
    AlphaBlend(copy1, reimage, frame, 0.5);
    MedianBlend(frame);
}

void ac::MatrixCollectionBlurAlpha(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copy1 = frame.clone();
    MedianBlend(copy1);
    collection.shiftFrames(copy1);
    Smooth(frame, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        cv::Vec3b values[4];
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int q = 0; q < collection.size(); ++q) {
                values[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (values[0][j] ^ values[1][j] ^ values[2][j])^pixel[j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
}
