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
    AddInvert(frame);
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
    AddInvert(frame);
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
    AddInvert(frame);

}

void ac::ImageAlphaXorMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    SmoothImageAlphaBlend(copy1);
    ImageSmoothMedianBlend(copy2);
    static double alpha = 1.0, alpha_max = 4.0;
    AlphaXorBlend(copy1, copy2, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

// use with Custom as Subfilter
void ac::MatrixCollectionBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    collection.shiftFrames(copy1);
    for(int i = 0; i < collection.size(); ++i) {
        ShuffleAlpha(collection.frames[i]);
        MedianBlur(collection.frames[i]);
    }
    Smooth(frame, &collection, false);
    AddInvert(frame);
}

// use with Custom goes good with RainbowXorBlend
void ac::MatrixCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    for(int i  = 0; i < collection.size(); ++i) {
        CallFilter(subfilter, collection.frames[i]);
    }
    Smooth(copy2, &collection, false);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "MatrixCollectionImageSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    collection.shiftFrames(reimage);
    collection.shiftFrames(copy1);
    Smooth(copy1, &collection, false);
    AlphaBlend(copy1, reimage, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionBlurAlpha(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copy1 = frame.clone();
    MedianBlend(copy1);
    collection.shiftFrames(copy1);
    Smooth(frame, &collection);
    for(int z = 0; z < frame.rows; ++z) {
        cv::Vec3b values[4];
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int q = 0; q < collection.size(); ++q) {
                values[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (values[0][j] ^ values[1][j] ^ values[2][j])^pixel[j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::MatrixCollectionXor(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[16];
            cv::Vec3b copypix = pixel;
            for(int q = 0; q < collection.size(); ++q) {
                pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                for(int r = 0; r < collection.size(); ++r) {
                    copypix[j] ^= pix[r][j];
                }
                pixel[j] = copypix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionXor32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[32];
            cv::Vec3b copypix = pixel;
            for(int q = 0; q < collection.size(); ++q) {
                pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                for(int r = 0; r < collection.size(); ++r) {
                    copypix[j] ^= pix[r][j];
                }
                pixel[j] = copypix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionRandomColorMap(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    RandomColorMap(copy1);
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[16];
            cv::Vec3b copypix = pixel;
            for(int q = 0; q < collection.size(); ++q) {
                pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                for(int r = 0; r < collection.size(); ++r) {
                    copypix[j] ^= pix[r][j];
                }
                pixel[j] = copypix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionDarkXor(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[32];
            cv::Vec3b copypix = pixel;
            for(int q = 0; q < collection.size(); ++q) {
                pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                for(int r = 0; r < collection.size(); ++r) {
                    copypix[j] ^= pix[r][j];
                }
               pixel[j] = copypix[j]^pixel[j];
            }
        }
    }
    AddInvert(frame);

}

void ac::MatrixCollectionRGB(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    static int counter = 0;
    cv::Mat copy1 = frame.clone();
    switch(counter) {
        case 0:
            AllRed(copy1);
            break;
        case 1:
            AllGreen(copy1);
            break;
        case 2:
            AllBlue(copy1);
            break;
    }
    ++counter;
    if(counter > 2)
        counter = 0;
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[32];
            cv::Vec3b copypix = pixel;
            for(int q = 0; q < collection.size(); ++q) {
                pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                for(int r = 0; r < collection.size(); ++r) {
                    copypix[j] ^= pix[r][j];
                }
                pixel[j] = copypix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::TrailsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "TrailsSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixCollectionXor(copy1);
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i), pix2 = copy2.at<cv::Vec3b>(z, i);
            cv::Vec3b lv(100, 100, 100);
            cv::Vec3b hv(100, 100, 100);
            if(colorBounds(pix,pixel,lv, hv)) {
                pixel = pix;
            } else {
                pixel = pix2;
            }
        }
    }
    AddInvert(frame);
}

// difference between frames
void ac::TrailsSubFilter32(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DifferenceSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixCollectionXor32(copy1);
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i), pix2 = copy2.at<cv::Vec3b>(z, i);
            cv::Vec3b lv(100, 100, 100);
            cv::Vec3b hv(100, 100, 100);
            if(colorBounds(pix,pixel,lv, hv)) {
                pixel = pix;
            } else {
                pixel = pix2;
            }
        }
    }
    AddInvert(frame);
}

void ac::CompareWithSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DifferenceSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i), pix2 = copy2.at<cv::Vec3b>(z, i);
            cv::Vec3b lv(100, 100, 100);
            cv::Vec3b hv(100, 100, 100);
            if(colorBounds(pix1,pix2,lv, hv)) {
                pixel = pix2;
            } else {
                pixel = pix1;
            }
        }
    }
    AddInvert(frame);
}

void ac::MedianTrails(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    pushSubFilter(filter_map["RandomColorMap"]);
    TrailsSubFilter(copy1);
    popSubFilter();
    MedianBlend(copy2);
    AlphaBlend(copy1,copy2,frame,0.5);
    AddInvert(frame);
}

void ac::SmoothMedianBlend(cv::Mat &frame) {
    pushSubFilter(filter_map["MatrixCollectionXor"]);
    SmoothSubFilter(frame);
    popSubFilter();
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ColorTransition(cv::Mat &frame) {
    static int val[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += val[j];
            }
        }
    }
    static int dir[3] = {1,1,1};
    static int speed[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            val[j] += speed[j];
            if(val[j] >= 255) {
                val[j] = rand()%255;
                dir[j] = 0;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        } else if(dir[j] == 0) {
            val[j] -= speed[j];
            if(val[j] <= 0) {
                val[j] = rand()%255;
                dir[j] = 1;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
}

void ac::ColorTransitionMedian(cv::Mat &frame) {
    ColorTransition(frame);
    MedianBlend(frame);
}

void ac::ColorTransitionRandom(cv::Mat &frame) {
    static int val[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += val[j];
            }
        }
    }
    static int dir[3] = {rand()%2,rand()%2,rand()%2};
    static int speed[3] = {rand()%5,rand()%5,rand()%5};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            val[j] += speed[j];
            if(val[j] >= 255) {
                val[j] = rand()%255;
                dir[j] = rand()%2;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        } else if(dir[j] == 0) {
            val[j] -= speed[j];
            if(val[j] <= 0) {
                val[j] = rand()%255;
                dir[j] = rand()%2;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
}

void ac::ColorTransitionRandomMedian(cv::Mat &frame) {
    ColorTransitionRandom(frame);
    MedianBlend(frame);
}

void ac::TestFilter101SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "TestFilter101SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    ColorTransitionRandomMedian(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
}
