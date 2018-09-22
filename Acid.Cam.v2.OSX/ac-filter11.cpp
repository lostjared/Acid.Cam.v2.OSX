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
    AddInvert(frame);
}

void ac::DarkCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DarkCollectionSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    DarkenFilter(copyf);
    collection.shiftFrames(copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar s;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int q = 1; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                    s[j] += pixel[j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                s[j] = s[j]/collection.size();
                pixel[j] = static_cast<unsigned char>(s[j]);
            }
        }
    }
    AddInvert(frame);
}


void ac::ChannelSort_NoBlend_Descending(cv::Mat &frame) {
    std::vector<cv::Mat> v;
    cv::split(frame, v);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v[0], channels[0],cv::SORT_DESCENDING);
    cv::sort(v[1], channels[1],cv::SORT_DESCENDING);
    cv::sort(v[2], channels[2],cv::SORT_DESCENDING);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::ChannelSort_NoBlend_Ascending(cv::Mat &frame) {
    std::vector<cv::Mat> v;
    cv::split(frame, v);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v[0], channels[0],cv::SORT_ASCENDING);
    cv::sort(v[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v[2], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::Headrush(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    static int index = 0;
    cv::Mat copyf = frame.clone();
    if(index == 0) {
        ChannelSort_NoBlend_Descending(copyf);
        index = 1;
    } else {
        ChannelSort_NoBlend_Ascending(copyf);
        index = 0;
    }
    collection.shiftFrames(copyf);
    cv::Mat total = frame.clone();
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::DarkSmooth_Filter(cv::Mat &frame) {
    static int dark = 2;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int dir = 1;
    if(dir == 1) {
        ++dark;
        if(dark > 12)
            dir = 0;
    } else if(dir == 0) {
        --dark;
        if(dark <= 2)
            dir = 1;
    }
    
    DarkSmooth(frame, &collection, dark);
    AddInvert(frame);
}

void ac::DarkSelfAlpha(cv::Mat &frame) {
    cv::Mat copyf = frame.clone();
    double alpha = 1.0, alpha_max = 4.0;
    DarkenImage(copyf, 2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b darkpixel = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) ^ static_cast<unsigned char>(darkpixel[j]*alpha);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::FlipMedian(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    static int index = 0;
    switch(index) {
        case 0:
            FlipBlendAll(copyf);
            break;
        case 1:
            FlipBlendW(copyf);
            break;
        case 2:
            FlipBlendH(copyf);
            break;
        case 3:
            FlipBlendWH(copyf);
            break;
    }
    ++index;
    if(index > 3) index = 0;
    collection.shiftFrames(copyf);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FlipMedianSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FlipMedianSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    FlipBlendAll(copyf);
    collection.shiftFrames(copyf);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FlipMirror(cv::Mat &frame) {
    DarkenFilter(frame);
    MirrorXorAll(frame);
    FlipBlendAll(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FlipMirrorAverage(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    DarkenFilter(copyf);
    MirrorXorAll(copyf);
    collection.shiftFrames(copyf);
    Smooth(frame, &collection);
    FlipBlendAll(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FlipMirrorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FlipMirrorSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    DarkenFilter(copyf);
    CallFilter(subfilter, copyf);
    collection.shiftFrames(copyf);
    Smooth(frame, &collection);
    FlipBlendAll(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ShuffleMedian(cv::Mat &frame) {
    static std::vector<std::string> filter_array{"MedianBlend","MedianBlurXor", "MirrorMedianBlend", "FlipMedian"};
    static int index = 0;
    Shuffle(index, frame, filter_array);

}

void ac::ShuffleRGB(cv::Mat &frame) {
    std::vector<std::string> filter_array { "RGB Shift", "RGB Sep", "SlideRGB", "GradientRGB", "FrameBlendRGB", "MoveRGB", "LineRGB", "PixelRGB", "RGBFlash", "MirrorRGB", "RGBStatic1", "RGBStatic2", "SelfAlphaRGB", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "RGBMirror", "RGBTrails", "RGBTrailsDark", "RGBTrailsAlpha", "RGBTrailsNegativeAlpha", "MovementRGBTrails", "RGBTrailsXor", "ShadeRGB"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
}

void ac::RandomPixels(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += rand()%255;
        }
    }
}

void ac::DarkRandomPixels(cv::Mat &frame) {
    int max = 1+(rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rand()%max;
                pixel[j] /= 4;
            }
        }
    }
}

void ac::MedianBlurSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianBlurSubFilter")
        return;
    static MatrixCollection<8> collection;
    static int counter = 1;
    cv::Mat copyf = frame.clone();
    for(int i = 0; i < counter; ++i) {
        MedianBlur(copyf);
    }
    CallFilter(subfilter, copyf);
    collection.shiftFrames(copyf);
    ++counter;
    if(counter > 3)
        counter = 1;
    Smooth(frame, &collection);
}

void ac::Bars(cv::Mat &frame) {
    static int start = rand()%3;
    for(int z = 0; z < frame.rows; ++z) {
        int index = start;
        for(int i = 0; i < frame.cols; i += 3) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = 255;
            ++index;
            if(index > 2)
                index = 0;
        }
    }
    ++start;
    if(start > 2)
        start = rand()%3;
}

void ac::ShuffleAlpha(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend","ScanAlphaSwitch", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","BlendAlphaXor","SmoothTrailsSelfAlphaBlend","SelfAlphaRGB","XorAlpha","SelfAlphaScale", "SelfScaleAlpha","DarkSelfAlpha"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
}

void ac::AlphaMorph(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    ShuffleAlpha(copyf);
    DarkenFilter(copyf);
    collection.shiftFrames(copyf);
    cv::Mat copyr = frame.clone();
    Smooth(copyf, &collection);
    static double alpha = 1.0, alpha_max = 4.0;
    static int dir = 1;
    AlphaBlend(copyf, copyr, frame, alpha);
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
}

void ac::ShuffleSelf(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend", "Self Scale", "ReinterpSelfScale", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","SelfXorScale", "SelfAlphaRGB", "GradientXorSelfScale", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "SelfXorAverage", "SelfAlphaScale", "SelfScaleAlpha", "SelfAlphaScaleBlend", "SelfScaleXorIncrease", "DarkSelfAlpha"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
}
