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

#include "ac.h"

void ac::resizeImageWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        if(resize_x > (frame.cols*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        if(resize_x <= frame.cols) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_y += 50;
        if(resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_y -= 50;
        if(resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        resize_y += 50;
        if(resize_x >  (frame.cols*6) || resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        resize_y -= 50;
        if(resize_x <= frame.cols || resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageAndFrameBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    resizeFrameWidthAndHeight(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageWidthAndHeightSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "resizeImageWidthAndHeightSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage, copy3;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    CallFilter(subfilter, reimage);
    resizeFrameWidthAndHeight(copy1);
    AlphaBlendDouble(copy1,reimage,copy3, 0.3, 0.7);
    AlphaBlend(copy3, orig_frame, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidth(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}
