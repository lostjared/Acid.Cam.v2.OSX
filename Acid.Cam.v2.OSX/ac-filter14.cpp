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
