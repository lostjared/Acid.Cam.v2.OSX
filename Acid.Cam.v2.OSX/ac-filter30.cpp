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

#include"ac.h"

void ac::StrangeGlitch64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < collection.size(); ++q) {
        cv::Mat &copy_frame = collection.frames[q];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
                if(colorBounds(pixel, pix, cv::Vec3b(30, 30, 30), cv::Vec3b(30, 30, 30))) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::StrangeGlitch16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < collection.size(); ++q) {
        cv::Mat &copy_frame = collection.frames[q];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
                if(colorBounds(pixel, pix, cv::Vec3b(30, 30, 30), cv::Vec3b(30, 30, 30))) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionMatrixOutline(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    ColorTransition(frame);
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    MedianBlendIncrease(copy1);
    static int val = 4;
    cv::Mat &copy_frame = collection.frames[val];
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CollectionMatrixSubFilter")
        return;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    MedianBlendIncrease(copy1);
    static int val = 4;
    cv::Mat &copy_frame = collection.frames[val];
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionMatrixRandomMedianBlend(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    static std::vector<std::string> array_filter({"ColorCollection","ColorCollectionRandom","ColorCollectionStrobeShift","ColorCollectionRandom_Filter","ColorCollectionShift","ColorCollectionStrobeShake","ColorCollectionSubFilter","ColorCollectionShiftSubFilter",
        "ColorCollectionSubtle","ColorCollection64","ColorCollectionSubtleStrobe","ColorCollection64X","ColorCollectionSwitch","ColorCollectionRGB_Index","ColorCollectionRGBStrobeSubFilter","ColorCollectionGhostTrails","ColorCollectionScale","ColorCollectionReverseStrobe",
        "ColorCollectionXorPixel","ColorCollectionXorOffsetFlash","ColorCollectionMatrixGhost","ColorCollectionPixelXor","ColorCollectionTwitchSubFilter","ColorCollectionMovementIndex","ColorCollectionPositionStrobe","ColorCollectionStrobeBlend",
        "ColorCollectionShuffle","ColorCollectionAlphaBlendArray"});
    cv::Mat copy1 = frame.clone();
    static int filter_index = 0;
    Shuffle(filter_index, copy1, array_filter);
    collection.shiftFrames(copy1);
    static int index = 2, dir = 1;
    if(dir == 1) {
        ++index;
        if(index > 16) {
            index = 16;
            dir = 0;
        }
    } else {
        --index;
        if(index < 2) {
            index = 2;
            dir = 1;
        }
    }
    cv::Mat copy2 = frame.clone(), copy3 = frame.clone();
    MedianBlendMultiThread(copy2, &collection, index);
    static double alpha = 1.0;
    static int dir1 = 1;
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.3);
    AlphaBlendDouble(copy2, copy3, frame, alpha, (1-alpha));
}

void ac::ImageCollectionMatrixOutline(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1;
    ac_resize(blend_image, copy1, frame.size());
    static MatrixCollection<8> collection;
    ColorPulseIncrease(frame);
    collection.shiftFrames(frame);
    MedianBlendIncrease(copy1);
    static constexpr int val = 4;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageCollectionMatrixOutlineSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCollectionMatrixOutlineSubFilter")
        return;
    cv::Mat copy1;
    ac_resize(blend_image, copy1, frame.size());
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    MedianBlendIncrease(copy1);
    static constexpr int val = 4;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageCollectionMatrixFillSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCollectionMatrixFillSubFilter")
        return;
    cv::Mat copy1, reimage, copy2, output;
    ac_resize(blend_image, reimage, frame.size());
    copy1 = reimage.clone();
    copy2 = frame.clone();
    static MatrixCollection<8> collection;
    AlphaBlend(copy1,copy2, output, 0.5);
    collection.shiftFrames(frame);
    CallFilter(subfilter, output);
    static constexpr int val = 4;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0, 0, 0);
            } else {
                pixel = output.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageCollectionMatrixFadeInOutSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCollectionMatrixFadeInOutSubFilter")
        return;
    cv::Mat copy1, reimage, copy2, output;
    ac_resize(blend_image, reimage, frame.size());
    copy1 = reimage.clone();
    copy2 = frame.clone();
    static MatrixCollection<8> collection;
    AlphaBlend(copy1,copy2, output, 0.5);
    collection.shiftFrames(frame);
    CallFilter(subfilter, output);
    static constexpr int val = 4;
    static double detect_val = 10;
    static int dir = 1;
    cv::Mat &copy_frame = collection.frames[val];
    int detect = static_cast<int>(detect_val);
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, cv::Vec3b(detect, detect, detect), cv::Vec3b(detect, detect, detect))) {
                pixel = cv::Vec3b(0, 0, 0);
            } else {
                pixel = output.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(detect_val, dir, 0.1, 75.0, 10.0);
}

void ac::ImageCollectionMatrixIntensitySubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCollectionMatrixIntensitySubFilter")
        return;
    cv::Mat copy1, reimage, copy2, output;
    ac_resize(blend_image, reimage, frame.size());
    copy1 = reimage.clone();
    copy2 = frame.clone();
    static MatrixCollection<8> collection;
    AlphaBlend(copy1,copy2, output, 0.5);
    collection.shiftFrames(frame);
    CallFilter(subfilter, output);
    static constexpr int val = 4;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0, 0, 0);
            } else {
                pixel = output.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageCollectionMatrixMedianSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCollectionMatrixMedianSubFilter")
        return;
    cv::Mat copy1, reimage, copy2, output;
    ac_resize(blend_image, reimage, frame.size());
    copy1 = reimage.clone();
    copy2 = frame.clone();
    static MatrixCollection<32> collection;
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copy1,copy2, output, alpha, (1-alpha));
    collection.shiftFrames(frame);
    CallFilter(subfilter, output);
    MedianBlendIncrease(output);
    static const int val = 4;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0, 0, 0);
            } else {
                pixel = output.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::CollectionMatrxOutlineAlphaMedianBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    ColorTransition(frame);
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    MedianBlendIncrease(copy1);
    static int val = 4;
    cv::Mat &copy_frame = collection.frames[val];
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    cv::Mat copied_frame = frame.clone(), copy_val = frame.clone();
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = copied_frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, intensity, intensity)) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copied_frame, copy_val, frame, alpha, (1-alpha));
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}
