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
 
 Copyright (c) 2020, Jared Bruni
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
            cv::Vec3b fpixel = pixelAt(frame,z, i);
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
            cv::Vec3b fpixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b darkpixel = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1)) ^ static_cast<unsigned char>(darkpixel[j]*alpha);
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
    AddInvert(frame);
}

void ac::ShuffleRGB(cv::Mat &frame) {
    std::vector<std::string> filter_array { "RGB Shift", "RGB Sep", "SlideRGB", "GradientRGB", "FrameBlendRGB", "MoveRGB", "LineRGB", "PixelRGB", "RGBFlash", "MirrorRGB", "RGBStatic1", "RGBStatic2", "SelfAlphaRGB", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "RGBMirror", "RGBTrails", "RGBTrailsDark", "RGBTrailsAlpha", "RGBTrailsNegativeAlpha", "MovementRGBTrails", "RGBTrailsXor", "ShadeRGB"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::RandomPixels(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += rand()%255;
        }
    }
    AddInvert(frame);
}

void ac::DarkRandomPixels(cv::Mat &frame) {
    int max = 1+(rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rand()%max;
                pixel[j] /= 4;
            }
        }
    }
    AddInvert(frame);
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
    AddInvert(frame);
}

void ac::Bars(cv::Mat &frame) {
    static int start = rand()%3;
    for(int z = 0; z < frame.rows; ++z) {
        int index = start;
        for(int i = 0; i < frame.cols; i += 3) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = 255;
            ++index;
            if(index > 2)
                index = 0;
        }
    }
    ++start;
    if(start > 2)
        start = rand()%3;
    AddInvert(frame);
}

void ac::ShuffleAlpha(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend","ScanAlphaSwitch", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","BlendAlphaXor","SmoothTrailsSelfAlphaBlend","SelfAlphaRGB","XorAlpha","SelfAlphaScale", "SelfScaleAlpha","DarkSelfAlpha"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
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
    AddInvert(frame);
}

void ac::ShuffleSelf(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend", "Self Scale", "ReinterpSelfScale", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","SelfXorScale", "SelfAlphaRGB", "GradientXorSelfScale", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "SelfXorAverage", "SelfAlphaScale", "SelfScaleAlpha", "SelfAlphaScaleBlend", "SelfScaleXorIncrease", "DarkSelfAlpha"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::PixelatedHorizontalLines(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    static double alpha = 1.0, alpha_max = 4.0;
    for(int c = 0; c < frame.rows; ++c) {
        int start = rand()%frame.cols;
        for(int q = start; q < frame.cols; ++q) {
            cv::Vec3b &pixel = pixelAt(frame,c, q);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::PixelatedVerticalLines(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    static double alpha = 1.0, alpha_max = 4.0;
    for(int c = 0; c < frame.cols; ++c) {
        int start = rand()%frame.rows;
        for(int q = start; q < frame.rows; ++q) {
            cv::Vec3b &pixel = pixelAt(frame,q,c);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::StrobeShuffle(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"StrobeEffect", "StrobeScan", "BlockStrobe", "NegativeStrobe", "BitwiseXorStrobe", "StrobeBlend", "FadeStrobe", "MirrorStrobe", "AndStrobe", "AndStrobeScale", "AndPixelStrobe", "AndOrXorStrobe", "AndOrXorStrobeScale", "BrightStrobe", "DarkStrobe", "StrobeTransform", "RandomStrobe", "StuckStrobe", "OrStrobe", "DifferenceStrobe","RandomStrobeFlash", "GaussianStrobe", "StrobeSort", "GlitchSortStrobe", "StrobeXor"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::BlendBurred(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    cv::Mat blur_copy = frame.clone();
    MedianBlur(blur_copy);
    DarkenFilter(frame);
    MedianBlur(blur_copy);
    DarkenFilter(blur_copy);
    MedianBlur(blur_copy);
    collection.shiftFrames(blur_copy);
    cv::Scalar value;
    for(int q = 1; q < collection.size(); ++q) {
        cv::Mat &frame_ref = collection.frames[q];
        for(int z = 0; z < frame_ref.rows; ++z) {
            for(int i = 0; i < frame_ref.cols; ++i) {
                cv::Vec3b color = frame_ref.at<cv::Vec3b>(z, i);
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    value[j] += color[j];
                    value[j] /= 3;
                    pixel[j] = pixel[j]^static_cast<unsigned char>(value[j]);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::BlendCombinedValues(cv::Mat &frame) {
    static cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j];
                values[j] /= 3;
                pixel[j] = pixel[j]^static_cast<unsigned char>(values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::RGBColorTrails(cv::Mat &frame) {
    RGBTrails(frame);
    BlendCombinedValues(frame);
    AddInvert(frame);
}

void ac::BlendCombinedValueSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendCombinedValueSubFilter")
        return;
    static cv::Scalar values;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b val = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += val[j];
                values[j] /= 3;
                pixel[j] = pixel[j]^static_cast<unsigned char>(values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::BlendSubFilterAlpha(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendSubFilterAlpha")
        return;
    
    static double alpha = 1.0, alpha_max = 2.0;
    
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    
    for(int z =  0; z < frame.rows; ++z) {
        cv::Scalar values;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j];
                values[j] = values[j]/1.77;
                pixel[j] = pixel[j] ^ (static_cast<unsigned char>(values[j]*3.14));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 1.5, 0.001);
    cv::Mat copy_frame = frame.clone();
    AlphaBlend(copy_frame, copyf, frame, alpha);
    AddInvert(frame);
}

void ac::GradientXorPixels(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j]+i;
                values[j] /= 6.14;
                pixel[j] = pixel[j]^static_cast<unsigned char>(values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::PurpleRain(cv::Mat &frame) {
    cv::Scalar values;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z  = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j];
                values[j] /= (j+1);
                pixel[j] = pixel[j] ^ static_cast<unsigned char>(values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelByPixelXor(cv::Mat &frame) {
    cv::Vec3b pix;
    static int counter = 2;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pix[j] += pixel[j];
                pix[j] /= counter;
                pixel[j] = pixel[j]^pix[j];
            }
        }
    }
    ++counter;
    if(counter > 8)
        counter = 2;
    
    AddInvert(frame);
}

void ac::CopyXorAlpha(cv::Mat &frame) {
    static cv::Mat frame_copy;
    if(frame_copy.empty() || frame_copy.size() != frame.size())
        frame_copy = frame.clone();

    cv::Mat backup = frame.clone();
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows-2; ++z) {
        for(int i = 0; i < frame.cols-2; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = frame_copy.at<cv::Vec3b>(z, i+1);
            pix[1] = frame_copy.at<cv::Vec3b>(z+1, i);
            pix[2] = frame_copy.at<cv::Vec3b>(z+1, i+1);
            
            cv::Vec3b color;
            for(int j = 0; j < 3; ++j) {
                color[j] += pix[0][j] + pix[1][j] + pix[2][j];
                color[j] /= 6;
                pixel[j] = static_cast<unsigned char>(color[j] * alpha) ^ static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.1);
    frame_copy = backup.clone();
    AddInvert(frame);
}

void ac::AveragePixelsXor(cv::Mat &frame) {
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b color = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                values[j] += color[j];
        }
    }
    cv::Vec3b pix;
    for(int j = 0; j < 3; ++j) {
        values[j] /= (frame.cols * frame.rows);
        pix[j] = static_cast<unsigned char>(values[j]);
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^pix[j];
        }
    }
    AddInvert(frame);
}

void ac::AveragePixelAlpha(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b color = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                values[j] += color[j];
        }
    }
    cv::Vec3b pix;
    for(int j = 0; j < 3; ++j) {
        values[j] /= (frame.cols * frame.rows);
        pix[j] = static_cast<unsigned char>(values[j]);
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1))^static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::NegativeByRow(cv::Mat &frame) {
    bool neg = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(neg == false) break;
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                    pixel[j] = ~pixel[j];
            
        }
        neg = (neg == true) ? false : true;
    }
    AddInvert(frame);
}

void ac::AveragePixelCollection(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    MedianBlur(frame);
    collection.shiftFrames(frame);
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int q = 0; q < collection.size(); ++q) {
            	cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
            	for(int j = 0; j < 3; ++j)
                	values[j] += pix[j];
            }
        }
    }
    cv::Vec3b fpix;
    for(int j = 0; j < 3; ++j) {
        values[j] = values[j] / ((frame.rows * frame.cols) * 3.14);
        fpix[j] = static_cast<unsigned char>(values[j]);
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^fpix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::IncorrectLine(cv::Mat &frame) {
    cv::Vec3b xval;
    xval[0] = rand()%255;
    xval[1] = rand()%255;
    xval[2] = rand()%255;
    for(int z = 0; z < frame.rows; ++z) {
        cv::Scalar values;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j];
                pixel[j] = pixel[j] ^ xval[j];
            }
        }
        for(int j = 0; j < 3; ++j) {
            xval[j] = static_cast<unsigned char>(values[j]/frame.cols/3.14);
        }
    }
    AddInvert(frame);
}

void ac::XorShift(cv::Mat &frame) {
    int sw = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            values[sw] += pixel[sw];
            ++sw;
            if(sw > 2)
                sw = 0;
        }
    }
    
    cv::Vec3b pix;
    for(int j = 0; j < 3; ++j) {
        values[j] = values[j] / (frame.rows * frame.cols);
        pix[j] = static_cast<unsigned char>((values[j] * alpha)/3.14);
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ pix[j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::StrobeXorAndOr(cv::Mat &frame) {
    cv::Vec3b rnd(rand()%255, rand()%255, rand()%255);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                switch(index) {
                    case 0:
                        pixel[j] = pixel[j] ^ rnd[j];
                        break;
                    case 1:
                        pixel[j] = pixel[j] & rnd[j];
                        break;
                    case 2:
                        pixel[j] = pixel[j] | rnd[j];
                        break;
                }
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::XorWithSource(cv::Mat &frame) {
    if(orig_frame.empty())
        return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b orig = pixelAt(orig_frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^orig[j];
            }
        }
    }
}

void ac::AlphaBlendWithSource(cv::Mat &frame) {
    if(orig_frame.empty())
        return;
    
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copy_f = frame.clone();
    AlphaBlend(copy_f, orig_frame, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
}

void ac::RGBSep1x(cv::Mat &frame) {
    cv::Mat copy_f = frame.clone();
    static int offset = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = offset, start_i = 0, x = offset/2; x < frame.cols && start_i < frame.cols && i < frame.cols; ++i, ++start_i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b copy_pix = copy_f.at<cv::Vec3b>(z, start_i);
            cv::Vec3b copy_pix2 = copy_f.at<cv::Vec3b>(z, x);
            pixel[2] += copy_pix[2];
            pixel[0] += copy_pix2[0];
        }
        for(int i = 0; i < offset; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b off_x = copy_f.at<cv::Vec3b>(z, frame.cols-i-1);
            pixel[1] += off_x[1];
        }
    }
    ++offset;
    if(offset > frame.cols)
        offset = 2;
    AddInvert(frame);
}

void ac::RGBMedianBlend(cv::Mat &frame) {
    RGBSep1x(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::RGBMirror1(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pixels[4];
            pixels[0] = pixelAt(frame,frame.rows-z-1, frame.cols-i-1);
            pixels[1] = pixelAt(frame,frame.rows-z-1, i);
            pixels[2] = pixelAt(frame,z, frame.cols-i-1);
            pixels[3] = pixel;
            pixel[0] += pixels[0][0];
            pixel[1] += pixels[1][1];
            pixel[2] += pixels[2][2];
        }
    }
    
    AddInvert(frame);
}

void ac::RGBMirror1Median(cv::Mat &frame) {
    RGBMirror1(frame);
    MedianBlend(frame);
}

void ac::FlashMirror(cv::Mat &frame) {
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixelAt(frame,z, frame.cols-i-1);
            pix[1] = pixelAt(frame,frame.rows-z-1, i);
            pix[2] = pixelAt(frame,frame.rows-z-1, frame.cols-i-1);
            pix[3] = pixel;
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ pix[index][j];
            }
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::CollectionXorSourceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "CollectionXorSourceSubFilter" || orig_frame.empty())
        return;
    static MatrixCollection<6> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame_copy, &collection, false);
    AlphaXorBlend(frame_copy, orig_frame, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::ReverseMirrorX(cv::Mat &frame) {
    cv::Mat copyf = frame.clone();
    Reverse(copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ pix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorXorAll_Reverse(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    Reverse(frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= (pixel[j] ^ values[0][j] ^ values[1][j] ^ values[2][j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorRGBReverse(cv::Mat &frame) {
    cv::Mat copyf[3];
    cv::flip(frame, copyf[0], 0);
    cv::flip(frame, copyf[1], 1);
    cv::flip(frame, copyf[2], -1);
    static int index = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[3];
            pix[0] = copyf[0].at<cv::Vec3b>(z, i);
            pix[1] = copyf[1].at<cv::Vec3b>(z, i);
            pix[2] = copyf[2].at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    pixel[0] = pix[0][0];
                    pixel[1] = pix[1][1];
                    pixel[2] = pix[2][2];
                    break;
                case 1:
                    pixel[0] = pix[2][0];
                    pixel[1] = pix[0][1];
                    pixel[2] = pix[1][2];
                    break;
                case 2:
                    pixel[0] = pix[1][0];
                    pixel[1] = pix[2][1];
                    pixel[2] = pix[0][2];
                    break;
            }
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::MirrorRGBReverseBlend(cv::Mat &frame) {
    cv::Mat copyf[3];
    cv::flip(frame, copyf[0], 0);
    cv::flip(frame, copyf[1], 1);
    cv::flip(frame, copyf[2], -1);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[3];
            pix[0] = copyf[0].at<cv::Vec3b>(z, i);
            pix[1] = copyf[1].at<cv::Vec3b>(z, i);
            pix[2] = copyf[2].at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    pixel[0] ^= pix[0][0];
                    pixel[1] ^= pix[1][1];
                    pixel[2] ^= pix[2][2];
                    break;
                case 1:
                    pixel[0] ^= pix[2][0];
                    pixel[1] ^= pix[0][1];
                    pixel[2] ^= pix[1][2];
                    break;
                case 2:
                    pixel[0] ^= pix[1][0];
                    pixel[1] ^= pix[2][1];
                    pixel[2] ^= pix[0][2];
                    break;
            }
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    
    AddInvert(frame);
}

