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


void ac::ImageFadeInOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    AlphaBlendDouble(copy1, reimage, copy2, 1.0, alpha);
    Smooth(copy2, &collection);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AlphaBlendDouble(copy1, copy2, frame, 0.3, 0.8);
}

void ac::ImageFadeBlackInOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    AlphaBlendDouble(copy1, reimage, frame, 1.0, alpha);
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ImageFadeBlackInOutSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageFadeBlackInOutSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    ImageFadeBlackInOut(copy1);
    CallFilter(subfilter, copy1);
    static double alpha = 1.0;
    AlphaBlendDouble(copy2, copy1, frame, 1.0, alpha);
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ImageFadeFrameInOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    AlphaBlendDouble(copy1, reimage, frame, alpha, 1.0);
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ImageFadeFrameInOutSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageFadeFrameInOutSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static double alpha = 1.0;
    AlphaBlendDouble(copy1, reimage, frame, alpha, 1.0);
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ImageFadeDouble(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static double alpha1 = 1.0, alpha2 = 1.0;
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
    static int dir1 = 1,  dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 1.0, 0.1);
}

void ac::BlendSubFilterAndImage(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "BlendSubFilterAndImage")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.5, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 1.5, 0.1);
    CallFilter(subfilter, copy1);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
}
