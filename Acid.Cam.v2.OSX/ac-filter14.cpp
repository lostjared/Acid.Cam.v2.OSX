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
    pushSubFilter(getFilterByName("ExactImage"));
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
    pushSubFilter(getFilterByName("ExactImage"));
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    pushSubFilter(getFilterByName("RandomColorMap"));
    TrailsSubFilter(copy1);
    popSubFilter();
    MedianBlend(copy2);
    AlphaBlend(copy1,copy2,frame,0.5);
    AddInvert(frame);
}

void ac::SmoothMedianBlend(cv::Mat &frame) {
    pushSubFilter(getFilterByName("MatrixCollectionXor"));
    SmoothSubFilter(frame);
    popSubFilter();
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ColorTransition(cv::Mat &frame) {
    static int val[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            if(val[j] <= 1) {
                val[j] = rand()%255;
                dir[j] = 1;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorTransitionMedian(cv::Mat &frame) {
    ColorTransition(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ColorTransitionRandom(cv::Mat &frame) {
    static int val[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            if(val[j] <= 1) {
                val[j] = rand()%255;
                dir[j] = rand()%2;
                ++speed[j];
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorTransitionRandomMedian(cv::Mat &frame) {
    ColorTransitionRandom(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ColorTransitionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ColorTransitionSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    ColorTransitionRandomMedian(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ColorTransitionImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ColorTransitionImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2;
    ac_resize(blend_image, copy2, frame.size());
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::CurtainSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "CurtainSubFilter")
        return;
    if(testSize(frame) == false)
        return;
    static int start = 0;
    static int direction = 1;
    static double alpha = 1.0, alpha_max = 7.0;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        if(direction == 1) {
            for(int i = 0; i < start; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b copy_pix = frame_copy.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] ^= static_cast<unsigned char>(copy_pix[j]+pixel[j]);
                }
            }
        } else {
            
            for(int i = frame.cols-1; i > start; --i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b copy_pix = frame_copy.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] ^= static_cast<unsigned char>(copy_pix[j]+pixel[j]);
            }
        }
    }
    if(direction == 1) {
        start += 40;
        if(start > frame.cols-1) {
            direction = 0;
        }
    } else {
        start -= 40;
        if(start <= 1) {
            direction = 1;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}


void ac::RandomTrails(cv::Mat &frame) {
    pushSubFilter(getFilterByName("Random Filter"));
    TrailsSubFilter(frame);
    popSubFilter();
    AddInvert(frame);
}

void ac::RandomTrailsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomTrailsSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    pushSubFilter(getFilterByName("Random Filter"));
    TrailsSubFilter(copy1);
    popSubFilter();
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::CosSinMedianBlend(cv::Mat &frame) {
    pushSubFilter(getFilterByName("CosSinMultiply"));
    SmoothSubFilter(frame);
    popSubFilter();
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::TrailsRGB(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixTrailsXorRandom(copy1);
    StrobeXor(copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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

void ac::MatrixTrailsXorRandom(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            if(copypix != pixel) {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = rand()%256;
                }
            }
        }
    }
    AddInvert(frame);
}


void ac::CosSinMultiplyCollectionXor(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    cossinMultiply(copy1);
    MatrixCollectionXor(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::Filter8_Blend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    filter8(copy1);
    Smooth(copy1, &collection);
    MedianBlend(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::Filter8_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "Filter8_SubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    filter8(copy1);
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::RandomSmoothAlphaMedian(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    randomFilter(copy1);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::RandomAlphaBlendFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone(), copy4, copy5;
    randomFilter(copy1);
    Smooth(copy1, &collection1);
    randomFilter(copy2);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy3, copy4, 0.5);
    AlphaBlend(copy2, copy3, copy5, 0.5);
    AlphaBlend(copy4, copy5, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::RandomMirrorBitwiseXor(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    randomFilter(copy1);
    Smooth(copy1, &collection);
    MirrorBitwiseXor(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SquareDivideSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareDivideSubFilter")
        return;
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    int pos_x = 0, pos_y = 0;
    int index = 0;
    int size_w = frame.cols/2;
    int size_h = frame.rows/2;
    cv::Mat resized[4];
    for(int i = 0; i < 2; ++i) {
        for(int z = 0; z < 2; ++z) {
            pos_x = i*size_w;
            pos_y = z*size_h;
            ac_resize(collection.frames[index], resized[index], cv::Size(size_w, size_h));
            randomFilter(resized[index]);
            copyMat(resized[index], 0, 0, frame, pos_x, pos_y, size_w, size_h);
            ++index;
        }
    }
    CallFilter(subfilter, copy1);
    cv::Mat fcopy = frame.clone();
    AlphaBlend(copy1, frame, fcopy, 0.5);
    frame = fcopy.clone();
    AddInvert(frame);
}

void ac::SquareSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int pos_x = 0, pos_y = 0;
    int index = 0;
    int size_w = frame.cols/2;
    int size_h = frame.rows/2;
    cv::Mat resized[8];
    for(int i = 0; i < 2; ++i) {
        for(int z = 0; z < 2; ++z) {
            pos_x = i*size_w;
            pos_y = z*size_h;
            ac_resize(collection.frames[index], resized[index], cv::Size(size_w, size_h));
            CallFilter(subfilter, resized[index]);
            copyMat(resized[index], 0, 0, frame, pos_x, pos_y, size_w, size_h);
            ++index;
        }
    }
    AddInvert(frame);
}

void ac::SquareSubFilter8(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareSubFilter8")
        return;
    constexpr int num_s = 4;
    static MatrixCollection<num_s*num_s> collection;
    collection.shiftFrames(frame);
    int pos_x = 0, pos_y = 0;
    int index = 0;
    int size_w = frame.cols/num_s;
    int size_h = frame.rows/num_s;
    cv::Mat resized[num_s*num_s];
    for(int i = 0; i < num_s; ++i) {
        for(int z = 0; z < num_s; ++z) {
            pos_x = i*size_w;
            pos_y = z*size_h;
            ac_resize(collection.frames[index], resized[index], cv::Size(size_w, size_h));
            CallFilter(subfilter, resized[index]);
            copyMat(resized[index], 0, 0, frame, pos_x, pos_y, size_w, size_h);
            ++index;
        }
    }
    AddInvert(frame);
}

void ac::ColorExpand(cv::Mat &frame) {
    static int color_value[3] = {rand()%255,rand()%255,rand()%255};
    static int dir[3] = {rand()%2, rand()%2, rand()%2};
    static int max_dir[3] = {rand()%2, rand()%2, rand()%2};
    static int s_max[3] = {rand()%255, rand()%255, rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += color_value[j];
        }
    }
    for(int q = 0; q < 3; ++q) {
        if(dir[q] == 0) {
            ++color_value[q];
            if(color_value[q] > s_max[q]) {
                dir[q] = 1;
                if(max_dir[q] == 0) {
                    s_max[q] += 5;
                    if(s_max[q] > 255) {
                        max_dir[q] = 1;
                    }
                } else {
                    s_max[q] -= 5;
                    if(s_max[q] <= 1) {
                        max_dir[q] = 0;
                    }
                }
            }
            
        } else {
            --color_value[q];
            if(color_value[q] <= 1) {
                dir[q] = 0;
                if(max_dir[q] == 0) {
                    s_max[q] += 5;
                    if(s_max[q] > 255) {
                        max_dir[q] = 1;
                    }
                } else {
                    s_max[q] -= 5;
                    if(s_max[q] <= 1) {
                        max_dir[q] = 0;
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorExpandSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ColorExpandSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    ColorExpand(copy2);
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[2];
            static int pixval[3] = {0,0,0};
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy2.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixval[j] = pix[0][j] ^ pix[1][j];
                pixval[j] /= 3;
                pixel[j] =  pixval[j] ^ static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::RotateImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int fcode = -1;
    cv::Mat img, copy1;
    ac_resize(blend_image, img, frame.size());
    cv::flip(img, copy1, fcode);
    frame = copy1.clone();
    ++fcode;
    if(fcode > 1)
        fcode = -1;
    AddInvert(frame);
}

void ac::RotateBlendImage(cv::Mat &frame) {
    cv::Mat img_copy = frame.clone(), image1 = frame.clone();
    RotateImage(img_copy);
    AlphaBlend(img_copy, image1, frame, 0.5);
    AddInvert(frame);
}

void ac::RotateImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "RotateImageSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    RotateBlendImage(copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::RotateAlphaBlendImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    RotateImage(copy1);
    static double alpha = 1.0, alpha_max = 2.5;
    AlphaBlend(copy1, copy2, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 2.4, 0.01);
    AddInvert(frame);
}

void ac::FlipShuffle(cv::Mat &frame) {
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<int> flip_codes {-1, 0, 1};
    static unsigned int offset = 0;
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, flip_codes[offset]);
    ++offset;
    if(offset > flip_codes.size()-1) {
        offset = 0;
        std::shuffle(flip_codes.begin(), flip_codes.end(), rng);
    }
    AddInvert(frame);
}

void ac::FlipRandom(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int offset = -1;
    offset += rand()%3;
    cv::flip(copy1, frame, offset);
    AddInvert(frame);
}

void ac::FlipOrder(cv::Mat &frame) {
    static int offset = -1;
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, offset);
    ++offset;
    if(offset > 1)
        offset = -1;
    AddInvert(frame);
}

void ac::FlipStrobeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FlipStrobeSubFilter")
        return;
    static int offset = 0;
    if(offset == 0) {
        FlipOrder(frame);
        CallFilter(subfilter,frame);
        offset = 1;
    } else {
        Negate(frame);
        offset = 0;
    }
    AddInvert(frame);
}


void ac::MirrorBlendFrame(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2;
    cv::flip(copy1, copy2, 1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorBlendVertical(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2;
    cv::flip(copy1, copy2, 0);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorVerticalAndHorizontal(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorBlendFrame(copy1);
    MirrorBlendVertical(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlendFor360(cv::Mat &frame) {
    const int width = 100;
    double inc_val = 1.0/width;
    double alpha1 = 1.0;
    double alpha2 = 0.0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        alpha1 = 1.0;
        alpha2 = 0.0;
        for(int i = width-1; i >= 0; --i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, copy1.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]*alpha1)+(pix[j]*alpha2));
            }
            alpha1 -= inc_val;
            alpha2 += inc_val;
        }
    }
    /*
    for(int z = 0; z < frame.rows; ++z) {
        alpha1 = 0.1;
        alpha2 = 0.9;
        for(int i = width-1; i >= 0; --i) {
            cv::Vec3b &pixel = pixelAt(frame,z, copy1.cols-i-1);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]*alpha1)+(pix[j]*alpha2));
            }
            alpha1 += inc_val;
            alpha2 -= inc_val;
        }
    }
    */
    AddInvert(frame);
}

void ac::MirrorSidesMedian(cv::Mat &frame) {
    MirrorXorAll(frame);
    MirrorVerticalAndHorizontal(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorSidesSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorSidesSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorXorAll(copy1);
    MirrorVerticalAndHorizontal(frame);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MedianFrameAlphaBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianFrameAlphaBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2,frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MedianSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianSubFilter")
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();;
    CallFilter(subfilter, copy1);
    randomFilter(copy2);
    collection1.shiftFrames(copy1);
    collection2.shiftFrames(copy2);
    Smooth(copy1, &collection1, false);
    Smooth(copy2, &collection2, false);
    MedianBlend(copy1);
    MedianBlend(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ColorXorScale(cv::Mat &frame) {
    static int rgb[3] = {rand()%255, rand()%255, rand()%255};
    static int dir[3] = {1,1,1};
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] * (1+alpha)) ^ static_cast<unsigned char>(rgb[j] * alpha);
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            ++rgb[j];
            if(rgb[j] >= 255)
                dir[j] = 0;
                
        } else {
            --rgb[j];
            if(rgb[j] <= 1)
                dir[j] = 1;
        }
        
    }
    static int d = 1;
    procPos(d, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::ColorXorScaleSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ColorXorScaleSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ColorXorScale(copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}
