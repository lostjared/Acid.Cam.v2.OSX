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


void ac::LightBlur(cv::Mat &frame) {
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::GaussianBlur(blur, out, cv::Size(3, 3), 0, 0);
    out.copyTo(frame);
    AddInvert(frame);
}

void ac::LightMedianBlendMultiThread(cv::Mat &frame) {
    static constexpr int Size = 8;
    int r = rand()%2;
    for(int j = 0; j < r; ++j)
        LightBlur(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::ColorFadeMedianBlendMultiThread(cv::Mat &frame) {
    ColorIncreaseFadeRGB(frame);
    static constexpr int Size = 8;
    MedianBlur(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::ColorFadeBlendMedianBlendMultiThread(cv::Mat &frame) {
    static constexpr int Size = 8;
    ColorIncreaseFadeRGB(frame);
    BlendWithColor(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread32(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MedianBlur(frame);
    MatrixBlend(frame, &collection, 4);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread4(cv::Mat &frame) {
    static constexpr int Size = 4;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MedianBlur(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendFadeWithColor(cv::Mat &frame) {
    ColorIncreaseFadeRGB(frame);
    BlendWithColor(frame);
    MedianBlendMultiThread4(frame);
    AddInvert(frame);
}

void ac::GaussianBlurMild(cv::Mat &frame) {
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::GaussianBlur(blur, out, cv::Size(3, 3), 0, 0);
    out.copyTo(frame);
    AddInvert(frame);
}

void ac::MedianBlendWithColorInOutRGBColor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    ColorIncreaseFadeRGB(frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendWithColorInOutRGB(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    BlendWithColor(frame);
    ColorIncreaseInOutRGB(frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}
