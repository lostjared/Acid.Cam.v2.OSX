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


void ac::ImageAlphaXorSubFilter(cv::Mat &frame) {
    if(blend_set == true && subfilter != -1 && ac::draw_strings[subfilter] != "ImageAlphaXorSubFilter") {
        cv::Mat copyf = frame.clone(), copyi = frame.clone();;
        cv::Mat resized;
        cv::resize(blend_image, resized, frame.size());
        cv::Mat out;
        CallFilter(subfilter, copyi);
        AlphaBlend(resized, copyi, out, 0.5);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = out.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SmoothTrailsBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copyf = frame.clone();
    cv::Mat copyi = frame.clone();
    Smooth(copyf, &collection);
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionRGBXor(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    cv::Mat copyf = frame.clone();
    MovementRGBTrails(copyf);
    collection.shiftFrames(copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar sc;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    sc[j] += pix[j];
            }
            for(int j = 0; j < 3; ++j) {
            	int val = static_cast<int>(sc[j]);
                pixel[j] = pixel[j]^val;
            }
        }
    }
    BlendWithSource(frame);
    AddInvert(frame);
}

void ac::RainbowGlitch(cv::Mat &frame) {
    SmoothTrailsBlend(frame);
    MovementRGBTrails(frame);
    AddInvert(frame);
}
