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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);
}

void ac::FadeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FadeSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy2);
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}

void ac::FadeSubFilterRev(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FadeSubFilterRev")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    DarkenFilter(copy1);
    DarkenFilter(copy1);
    CallFilter(subfilter, copy1);
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}

void ac::ImageBlendSubFilterMedianBlend(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageBlendSubFilterMedianBlend")
        return;
    DarkenFilter(frame);
    DarkenFilter(frame);
    BlendSubFilterAndImage(frame);
    BlendWithSource(frame);
    BlendWithSource(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FadeSubFilterXor(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FadeSubFilterXor")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy2);
    static double alpha1 = 1.1, alpha2 = 4.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 4.0, 1.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 4.0, 1.1);
    AlphaXorBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}

void ac::BlurXorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurXorSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        MedianBlur(copy1);
        MedianBlur(copy2);
    }
    CallFilter(subfilter, copy1);
    static double alpha1 = 2.0, alpha2 = 6.0;
    static int dir1 = 0, dir2 = 1;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 6.0, 2.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.1, 6.0, 2.0);
    AlphaXorBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}


void ac::ColorFlashIncrease(cv::Mat &frame) {
    static unsigned int max_value = 2;
    cv::Vec3b value(rand()%max_value, rand()%max_value, rand()%max_value);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MedianBlur(copy1);
    MedianBlur(copy1);
    MedianBlur(copy1);
    DarkenFilter(copy1);
    DarkenFilter(copy1);
    for(int z = 0; z < copy2.rows; ++z) {
        for(int i = 0; i < copy2.cols; ++i) {
            cv::Vec3b &pixel = copy2.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^value[j]^pix[j];
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++max_value;
        if(max_value >= 255)
            dir = 0;
    } else {
        --max_value;
        if(max_value <= 1)
            dir = 1;
    }
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}

void ac::ScaleFilter(cv::Mat &frame) {
    static double amt = 0.1;
    static int dir = 1;
    PixelScaleAlpha(frame, amt);
    AlphaMovementMaxMin(amt, dir, 0.01, 1.0, 0.1);
}

void ac::NegativeDarkenXor(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static double alpha1 = 1.0;
    static int dir = 1;
    MedianBlur(copy1);
    MedianBlur(copy1);
    DarkenFilter(copy1);
    DarkenFilter(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>((-pix[j])*alpha1);
            }
        }
    }
    AlphaMovementMaxMin(alpha1, dir, 0.1, 6.0, 1.0);
    AddInvert(frame);
}

void ac::ImageXor_SubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageXor_SubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    Negate(copy1);
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = reimage.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pix1[j]^pix2[j]^pixel[j];
        }
    }
}

void ac::NegateBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "NegateBlendSubFilter")
        return;
    static double alpha1 = 1.0, alpha2 = 3.0;
    static int dir1 = 1, dir2 = 0;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Negate(copy2);
    CallFilter(subfilter, copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.08, 4.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.08, 4.0, 1.0);
}

void ac::StrobeNegatePixel(cv::Mat &frame) {
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = ~pixel[index];
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::StrobeNegateInOut(cv::Mat &frame) {
    static int dir = 1;
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = ~pixel[index];
        }
    }
    if(dir == 1) {
        ++index;
        if(index > 2) {
            dir = 0;
            index = 2;
        }
    } else {
        --index;
        if(index < 0) {
            dir = 1;
            index = 0;
        }
    }
}

void ac::ImageStrobeOnOff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static int index = 0;
    if(index == 0) {
        Negate(reimage);
        index = 1;
    } else {
        Negate(copy1);
        index = 0;
    }
    AlphaBlend(copy1, reimage, frame, 0.5);
}
