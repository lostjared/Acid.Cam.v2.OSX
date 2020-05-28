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
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(copied_frame,z, i);
            cv::Vec3b pix = pixelAt(copy_frame,z, i);
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

void ac::ImageSourceOrigXor(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = orig_frame.clone(), copy3;
    AlphaBlendDouble(copy1, copy2, copy3, alpha, (1-alpha));
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int val1 = static_cast<int>(pixel[j] * alpha);
                    int val2 = static_cast<int>(pix[j] * (1-alpha));
                    pixel[j] = static_cast<unsigned char>(((1-alpha) * pixel[j]) + (alpha *(val1 ^ val2)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MatrixCollectionAuraTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionAuraTrails32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    static int frame_counter = 0;
    ++frame_counter;
    if(frame_counter > 2 || collection.empty()) {
        collection.shiftFrames(frame);
        frame_counter = 0;
    }
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelIntensityFillSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelIntensityFillSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat copy3;
    AlphaBlend(copy1, copy2, copy3, 0.5);
    MedianBlendMultiThread(copy3);
    static double alpha = 1.0;
    static int dir = 1;
    cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix_val = collection.frames[q].at<cv::Vec3b>(z, i);
                cv::Vec3b pixels[3];
                pixels[0] = frames[0].at<cv::Vec3b>(z, i);
                pixels[1] = frames[1].at<cv::Vec3b>(z, i);
                pixels[2] = frames[2].at<cv::Vec3b>(z, i);
                cv::Vec3b pix(pixels[0][0], pixels[1][1], pixels[2][2]);
                cv::Vec3b val;
                if(colorBounds(pixel, pix, intensity, intensity)) {
                    val = copy1.at<cv::Vec3b>(z, i);
                } else {
                    val = pix;
                }
                for(int j = 0; j < 3; ++j) {
                    double alpha_val = alpha/3;
                    pixel[j] = static_cast<unsigned char>((pix_val[j] * 0.33) + ((alpha_val * pixel[j]) - ((1-alpha_val) * val[j])));
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::SmoothImageFrameSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "SmoothImageFrameSubFilter")
        return;
    static MatrixCollection<8> collection;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage, copy1, copy2;
    ac_resize(blend_image, reimage, frame.size());
    copy1 = frame.clone();
    AlphaBlendDouble(copy1, reimage, copy2, alpha, (1-alpha));
    CallFilter(subfilter, copy2);
    collection.shiftFrames(copy2);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::ImageCycleBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = reimage.clone(), copy2 = frame.clone();
    static MatrixCollection<8> collection1;
    static MatrixCollection<8> collection2;
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageCycleAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    AlphaBlendDouble(copy1, reimage, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageCycleXor(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int q = 0; q < 3; ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b cpix = collection.frames[q].at<cv::Vec3b>(z, i);
                cv::Vec3b ipix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int val1 = static_cast<int>(cpix[j] * alpha);
                    int val2 = static_cast<int>(ipix[j] * (1-alpha));
                    pixel[j] ^= static_cast<unsigned char>(val1 ^ val2);
                    if(pixel[j] == 0)
                        pixel[j] = ipix[j];
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

// works best with Image Cycle
void ac::ImageCycleMedian(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    static int div_index = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    collection.shiftFrames(frame);
    MedianBlendMultiThread(frame, &collection, div_index);
    cv::Mat copy1 = frame.clone();
    AlphaBlendDouble(copy1, reimage, frame, 0.5, 0.5);
    ++div_index;
    if(div_index > 7)
        div_index = 1;
    AddInvert(frame);
}

void ac::ImageCycleAlphaSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageCycleAlphaSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat temp1;
    AlphaBlendDouble(copy1, reimage, temp1, alpha, (1-alpha));
    CallFilter(subfilter, temp1);
    AlphaBlendDouble(copy1, temp1, frame, 0.5, 0.5);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ImageCycleSmoothAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    AlphaBlend(reimage, copy1, frame, 0.5);
    if(collection.empty())
        collection.shiftFrames(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::ImageCycleSmoothAlphaBlend32(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    AlphaBlend(reimage, copy1, frame, 0.5);
    if(collection.empty())
        collection.shiftFrames(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlendWithSourceMedianBlendMultiThreadEight(cv::Mat &frame) {
    BlendWithSource75(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::BlendWithImageMedianMutliThreadEight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    BlendWithImage75(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::GradientBlendWithImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    GradientColors(reimage);
    cv::Mat copy1  = frame.clone();
    GradientColors(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::DarkColorMovement(cv::Mat &frame) {
    ColorPulseIncrease(frame);
    ColorTransition(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::DarkColorFibonacci(cv::Mat &frame) {
    ColorFibonacci(frame);
    ColorTransition(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::UseFilterOnAndOffSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "UseFilterOnAndOffSubFilter")
        return;
    static int counter = 0;
    ++counter;
    if((counter%2) == 0) {
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::UseFilterOnAndOffEachQuarterSecondSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "UseFilterOnAndOffSubFilter")
        return;
    static int counter = 0;
    int fps = static_cast<int>(ac::fps)/4;
    ++counter;
    if(counter > fps) {
        counter = 0;
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}


void ac::UseFilterOnAndOffByEightSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "UseFilterOnAndOffByEightSubFilter")
        return;
    static int counter = 0;
    int fps = static_cast<int>(ac::fps)/8;
    ++counter;
    if(counter > fps) {
        counter = 0;
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::UseImageOnAndOffByEight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int counter = 0;
    int fps = static_cast<int>(ac::fps)/8;
    ++counter;
    if(counter > fps) {
        counter = 0;
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        reimage.copyTo(frame);
    }
}

void ac::UseImageOnAndOff(cv::Mat &frame) {
    static int counter = 0;
    ++counter;
    if((counter%2) == 0) {
        ExactImage(frame);
    }
}

void ac::SelfAlphaScale_Down(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    bool reset_values = false;
    static int counter = 0;
    int fps = static_cast<int>(ac::fps);
    ++counter;
    if((counter > (fps*6))) {
        counter = 0;
        reset_values = true;
    }
    if(reset_values == true|| image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[i][z].col[j] -= speed;
                    int value = pixel[j] - pix_container.pix_values[i][z].col[j];
                    if(value <= 0) {
                        pix_container.pix_values[i][z].col[j] = pixel[j];
                        pixel[j] = 0;
                    } else {
                        pixel[j] = pixel[j] - pix_container.pix_values[i][z].col[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 1.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::PsychoticVision(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    bool reset_values = false;
    if(reset_values == true|| image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create_empty(frame.cols, frame.rows);
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_container.setColorValues([](int , int) { return rand()%5; });
    }
    MedianBlendMultiThread_2160p(frame);
    static int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) - (0.3 * alpha) * p.col[j]);
                    if(p.dir[j] == 0) {
                        p.col[j] -= speed;
                        if(p.col[j] <= 0) {
                            p.dir[j] = 1;
                            p.col[j] = 1;
                        }
                    } else if(p.dir[j] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] > 200) {
                            p.dir[j] = 0;
                            p.col[j] = 200;
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    AddInvert(frame);
}

void ac::DarkPsychoticVision(cv::Mat &frame) {
    PsychoticVision(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::Mirror_ReverseColor(cv::Mat &frame) {
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                int pos_x = (frame->cols-i-1);
                int pos_y = (frame->rows-z-1);
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pixels[4];
                pixels[0] = frame->at<cv::Vec3b>(pos_y, i);
                pixels[1] = frame->at<cv::Vec3b>(pos_y, pos_x);
                pixels[2] = frame->at<cv::Vec3b>(z, pos_x);
                for(int j = 0; j < 3; ++j) {
                    pixel[3-j-1] = static_cast<unsigned char>((pixel[j] * 0.25) + (pixels[0][j] * 0.25) + (pixels[1][j] * 0.25) + (pixels[2][j] * 0.25));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageColorCycleMorph(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_cycle_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(reimage, reimage.cols, reimage.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    };
    static int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                            p.col[j] = 255;
                        }
                    } else if(p.dir[j] == 0) {
                        p.col[j] -= speed;
                        if(p.col[j] <= 0) {
                            p.col[j] = 0;
                            p.dir[j] = 1;
                        }
                    }
                    pixel[j] = pixel[j] ^ p.col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageBlendAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageBlendAndSubFilter")
        return;
    static MatrixCollection<16> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
    }
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_cycle_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(reimage, reimage.cols, reimage.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    };
    static int speed = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat images = reimage.clone();
    CallFilter(subfilter, images);
    Smooth(images, &collection);
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b ipix = images.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                            p.col[j] = 255;
                        }
                    } else if(p.dir[j] == 0) {
                        p.col[j] -= speed;
                        if(p.col[j] <= 0) {
                            p.col[j] = 0;
                            p.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * (pixel[j] ^ p.col[j])) +((0.25 * ipix[j] + (0.25 * pix[j]))));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    BlendWithSource75(frame);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::ImageSquareShrink(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 32;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
        
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
}

void ac::ImageSquareShrinkFast(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 64;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
        
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
}

