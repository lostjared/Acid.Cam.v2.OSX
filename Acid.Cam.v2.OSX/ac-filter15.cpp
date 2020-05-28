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

void ac::ImageXorScale(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int scale = rand()%255;
    static int dir = rand()%2;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b copy_pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^copy_pix[j]^scale;
            }
        }
    }
    if(dir == 1) {
        ++scale;
        if(scale >= 255)
            dir = 0;
    } else {
        --scale;
        if(scale <= 1)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::MatrixCollectionShiftSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionShiftSubFilter")
        return;
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection);
    cv::Mat copy2 = frame.clone();
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionImageShiftSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionImageShiftSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static MatrixCollection<32> collection;
    Smooth(reimage, &collection);
    AlphaBlend(reimage, copy1, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionXorBlendAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionBlurImageXorAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    static MatrixCollection<8> image_collection;
    cv::Mat copy1 = frame.clone(), copy2;
    ac_resize(blend_image, copy2, frame.size());
    XorAlpha(copy2);
    Smooth(copy2, &image_collection);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionBlurImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionBlurImageSubFilter")
        return;
    static MatrixCollection<8> collection;
    static MatrixCollection<8> image_collection;
    cv::Mat copy1 = frame.clone(), copy2;
    ac_resize(blend_image, copy2, frame.size());
    CallFilter(subfilter, copy2);
    Smooth(copy2, &image_collection);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionBlurImageSubFilter16(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionBlurImageSubFilter16")
        return;
    static MatrixCollection<16> collection;
    static MatrixCollection<16> image_collection;
    cv::Mat copy1 = frame.clone(), copy2;
    ac_resize(blend_image, copy2, frame.size());
    CallFilter(subfilter, copy2);
    Smooth(copy2, &image_collection);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageAlphaBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageAlphaBlendSubFilter")
        return;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    cv::Mat copy1 = frame.clone();
    AlphaBlend(copy1, reimage, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::MultipleMatrixCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MultipleMatrixCollectionSubFilter")
        return;
    static MatrixCollection<16> collection;
    static MatrixCollection<16> filter_collection;
    static MatrixCollection<16> fcollection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &filter_collection);
    Smooth(copy2, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    Smooth(frame, &fcollection);
    MedianBlend(frame);
    AddInvert(frame);
}

// use MedianBlend as Subfilter for cool efffect
void ac::BlurAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurAlphaSubFilter")
        return;
    static MatrixCollection<8> collection1,collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::BlurImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "BlurImageSubFilter")
        return;
    static MatrixCollection<8> collection1, collection2, collection3;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, reimage);
    Smooth(copy1, &collection1);
    Smooth(reimage, &collection2);
    AlphaBlend(copy1, reimage, frame, 0.5);
    Smooth(frame, &collection3);
    AddInvert(frame);
}

void ac::MedianBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianBlendSubFilter")
        return;
    static MatrixCollection<8> collection;
    int r = 3+(rand()%3);
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    CallFilter(subfilter, frame);
    cv::Mat copy1 = frame.clone();
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    cv::Mat copy2 = frame.clone();
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MedianBlendImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MedianBlendImageSubFilter")
        return;
    static MatrixCollection<8> collection;
    int r = 3+(rand()%3);
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    cv::Mat copy1 = frame.clone();
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::MedianBlendSelfBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MedianBlend(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlendHalfSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendHalfSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    int r = 3+(rand()%3);
    for(int j = 0; j < r; ++j) {
        MedianBlur(copy1);
        MedianBlur(reimage);
    }
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurImageAlphaBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "BlurImageAlphaBlendSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    int r = 3+(rand()%3);
    for(int j = 0; j < r; ++j) {
        MedianBlur(copy1);
        MedianBlur(reimage);
    }
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurImageAlphaBlendScaleSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "BlurImageAlphaBlendScaleSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    int r = 3+(rand()%3);
    for(int j = 0; j < r; ++j) {
        MedianBlur(copy1);
        MedianBlur(reimage);
    }
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, reimage);
    double alpha = 1.0, alpha_max = 4.0;
    static int dir = 1;
    AlphaBlend(copy1, reimage, frame, alpha);
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::RandomAmountOfMedianBlur(cv::Mat &frame) {
    int r = 1+(rand()%5);
    for(int j = 0; j < r; ++j)
        MedianBlur(frame);
    AddInvert(frame);
}

void ac::Bitwise_XOR_BlendFrame(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Bitwise_XOR(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::AlphaBlendWithSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AlphaBlendWithSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::AlphaBlendScaleWithSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AlphaBlendScaleWithSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    static double alpha = 1.0, alpha_max = 3.0;
    static int dir = 1;
    AlphaBlend(copy1, copy2, frame, alpha);
    procPos(dir, alpha, alpha_max, 3.1, 0.01);
    AddInvert(frame);
}

void ac::GaussianBlendEx(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 3+(rand()%3);
    for(int i = 0; i < r; ++i)
        GaussianBlur(frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::SimpleMatrixBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MatrixBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixBlendSubFilter")
        return;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::SmoothMatrixBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothMatrixBlendSubFilter")
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Smooth(copy1, &collection1);
    CallFilter(subfilter, copy1);
    for(int i = 0; i < 3; ++i)
    	MedianBlur(copy2);
    CallFilter(subfilter, copy2);
    collection2.shiftFrames(copy2);
    MatrixBlend(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurSmoothSubFilterAlphaBlend(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurSmoothSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurSmoothAlphaXorBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurSmoothAlphaXorBlendSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaXorBlend(copy1, copy2, frame, 0.5);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurTwiceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurTwiceSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        MedianBlur(copy1);
        GaussianBlur(copy2);
    }
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurFrameBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurFrameBlendSubFilter")
        return;
    static int index = 0;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        if(index == 0) {
        	MedianBlur(copy1);
        	GaussianBlur(copy2);
        } else {
            MedianBlur(copy2);
            GaussianBlur(copy1);
        }
    }
    ++index;
    if(index > 1)
        index = 0;
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurFrameSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurFrameSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        MedianBlur(copy1);
    }
    static MatrixCollection<8> collection;
    Smooth(copy1, &collection);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurSmoothMatrix(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    GaussianBlur(frame);
    MedianBlur(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlurInc(cv::Mat &frame) {
    int fps = static_cast<int>(ac::fps);
    static int dir = 1;
    static int offset = 1, frame_counter = 1;

    for(int i = 0; i < offset; ++i)
        MedianBlur(frame);

    if((frame_counter%fps) == 0) {
        if(dir == 1) {
            ++offset;
            if(offset > 8) {
                dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                dir = 1;
            }
        }
    }
    ++frame_counter;
    AddInvert(frame);
}

void ac::GaussianBlurInc(cv::Mat &frame) {
    int fps = static_cast<int>(ac::fps);
    static int dir = 1;
    static int offset = 1, frame_counter = 1;
    
    for(int i = 0; i < offset; ++i)
        GaussianBlur(frame);
    
    if((frame_counter%fps) == 0) {
        if(dir == 1) {
            ++offset;
            if(offset > 8) {
                dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                dir = 1;
            }
        }
    }
    ++frame_counter;
    AddInvert(frame);
}

void ac::BlurSmoothMedianInc(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    MedianBlurInc(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurSmoothGaussianInc(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    GaussianBlurInc(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurMatrixCollectionXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    MedianBlurInc(copy1);
    Smooth(copy1, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar values;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    values[j] += pix[j];
            }
            cv::Vec3b cpix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] /= 3;
                int val = static_cast<int>(values[j]);
                pixel[j] = (val^cpix[j])^pixel[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollection8XorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollection8XorSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar values;
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    values[j] += pix[j];
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= collection.size();
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    cv::Mat copy2 = frame.clone();
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlurSmoothRevFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    BlurSmoothMatrix(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            pix[2] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            pix[3] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j)
                    values[j] += pix[q][j];
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 3;
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    AddInvert(frame);
}

void ac::SurroundingPixels(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, i+1);
            pix[2] = copy1.at<cv::Vec3b>(z+1, i);
            pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[q][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 4;
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    AddInvert(frame);
}

void ac::SurroundingPixelsAlpha(cv::Mat &frame) {
    double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, i+1);
            pix[2] = copy1.at<cv::Vec3b>(z+1, i);
            pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[q][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 3;
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1))+static_cast<unsigned char>(values[j]*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::MatrixCollectionSurroundingPixels(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, i+1);
            pix[2] = copy1.at<cv::Vec3b>(z+1, i);
            pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[q][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 4;
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    Smooth(frame, &collection);
    MedianBlend(frame);
    cv::Mat copy3 = frame.clone();
    AlphaBlend(copy2, copy3, frame, 0.5);
    AddInvert(frame);
}


void ac::MatrixCollectionSurroundingPixelsSubFilter(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionSurroundingPixelsSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, i+1);
            pix[2] = copy1.at<cv::Vec3b>(z+1, i);
            pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[q][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 4;
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    CallFilter(subfilter, frame);
    Smooth(frame, &collection);
    MedianBlend(frame);
    cv::Mat copy3 = frame.clone();
    AlphaBlend(copy2, copy3, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionSurroundingPixelsImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copyx = frame.clone();
    AlphaBlend(copyx, reimage, frame, 0.5);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            if(i >= 0 && i+1 < frame.cols && z >= 0 && z+1 < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix[6];
                pix[0] = pixel;
                pix[1] = copy1.at<cv::Vec3b>(z, i+1);
                pix[2] = copy1.at<cv::Vec3b>(z+1, i);
                pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
                cv::Scalar values;
                
                for(int q = 0; q < 4; ++q) {
                    for(int j = 0; j < 3; ++j) {
                        values[j] += pix[q][j];
                    }
                }
                for(int j = 0; j < 3; ++j) {
                    values[j] /= 4;
                    int value = static_cast<int>(values[j]);
                    pixel[j] = pixel[j]^value;
                }
            }
        }
    }
    cv::Mat copy3 = frame.clone();
    AlphaBlend(copy2, copy3, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionSurroundingPixelsImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionSurroundingPixelsImageSubFilter")
        return;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copyx = frame.clone();
    CallFilter(subfilter, copyx);
    AlphaBlend(copyx, reimage, frame, 0.5);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixel;
            pix[1] = copy1.at<cv::Vec3b>(z, i+1);
            pix[2] = copy1.at<cv::Vec3b>(z+1, i);
            pix[3] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar values;
            for(int q = 0; q < 4; ++q) {
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[q][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= 4;
                int value = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^value;
            }
        }
    }
    cv::Mat copy3 = frame.clone();
    AlphaBlend(copy2, copy3, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageTransparent(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage, copy2 = frame.clone();
    ac_resize(blend_image, reimage, copy1.size());
    AlphaBlend(copy1, reimage, frame, 0.5);
    static MatrixCollection<8> collection;
    Smooth(frame, &collection);
    cv::Mat cp1 = frame.clone();
    AlphaBlend(cp1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixImageAlphaBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageSubFilter1")
        return;
    cv::Mat copy1 = frame.clone(), copy2, reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    AlphaBlend(copy1, reimage, copy2, 0.5);
    static MatrixCollection<8> collection;
    Smooth(copy2, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    CallFilter(subfilter, frame);
}

void ac::ImageAlphaCollectionSmoothBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), reimage, copy2;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage, copy2, 0.8);
    Smooth(copy2, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    static MatrixCollection<8> collection2;
    Smooth(frame, &collection2);
    AddInvert(frame);
}

void ac::ImageRandomColormap(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1;
    ac_resize(blend_image, copy1, frame.size());
    ShuffleColorMap(copy1);
    frame = copy1.clone();
    AddInvert(frame);
}

void ac::ImageRandomColormapAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copyimg;
    ac_resize(blend_image, copyimg, frame.size());
    ShuffleColorMap(copyimg);
    AlphaBlend(copy1, copyimg, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageRandomColormapAlphaScale(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copy1 = frame.clone(), copyimg;
    ac_resize(blend_image, copyimg, frame.size());
    ShuffleColorMap(copyimg);
    AlphaBlend(copy1, copyimg, frame, alpha);
    AddInvert(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
}

void ac::ImageRandomColormapSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageRandomColormapSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copyimg;
    ac_resize(blend_image, copyimg, frame.size());
    CallFilter(subfilter, copyimg);
    ShuffleColorMap(copyimg);
    AlphaBlend(copy1, copyimg, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageShuffle(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat imgcopy;
    ac_resize(blend_image, imgcopy, frame.size());
    randomFilter(imgcopy);
    frame = imgcopy.clone();
    AddInvert(frame);
}

void ac::ImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    frame = reimage.clone();
    AddInvert(frame);
}

void ac::ImageAlphaBlendWithFrameSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageAlphaBlendWithFrameSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageFadeInOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
