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

void ac::StrobePixelTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    pushSubFilter(getFilterByName("RandomStrobeFlash"));
    TrailsSubFilter(copy1);
    popSubFilter();
    collection.shiftFrames(copy1);
    int index = 0, dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.6));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                dir = 0;
            }
        } else {
            --index;
            if(index <= 0) {
                index = 0;
                dir = 1;
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::RectangleTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int index = 0, dir = 1;
    int y = 0;
    int height = 25+(rand()%75);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.6));
            }
        }
        ++y;
        if(y > height) {
            y = 0;
            height = 25+(rand()%75);
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
                    dir = 0;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = 0;
                    dir = 1;
                }
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::RectangleXY_Trails(cv::Mat &frame) {
    static MatrixCollection<16> collection, collection2;
    collection.shiftFrames(frame);
    
    if(collection2.empty())
        collection2.shiftFrames(frame);
    
    int index = 0, dir = 1, index2 = 0, dir2 = 1;
    int x = 0, y = 0;
    int height = 25+(rand()%75);
    int width =  25+(rand()%75);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = collection2.frames[index2].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.3)+(pix2[j] * 0.3));
            }
            ++x;
            if(x > width) {
                x = 0;
                width = 25+(rand()%75);
                if(dir2 == 1) {
                    ++index2;
                    if(index2 > (collection2.size()-1)) {
                        index2 = collection2.size()-1;
                        dir2 = 0;
                    }
                } else {
                    --index2;
                    if(index2 <= 0) {
                        index2 = 0;
                        dir2 = 1;
                    }
                }
            }
        }
        ++y;
        if(y > height) {
            y = 0;
            height = 25+(rand()%75);
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
                    dir = 0;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = 0;
                    dir = 1;
                }
            }
        }
    }
    static int counter = 0;
    ++counter;
    int fps_ = static_cast<int>(ac::fps);
    
    if ((counter%fps_) == 0)
        collection2.shiftFrames(frame);
    
    collection2.shiftFrames(frame);
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::MedianBlendImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static constexpr int SIZE=8;
    static MatrixCollection<SIZE> collection;
    collection.shiftFrames(frame);
    int r = 3+rand()%4;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Scalar total;
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = collection.frames[q].at<cv::Vec3b>(z, i);
                cv::Vec3b ipix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    total[j] += static_cast<unsigned char>((pixel[j] * 0.5) + (ipix[j] * 0.5));
                }
            }
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int t = 1+static_cast<int>(total[j]);
                    pixel[j] = static_cast<unsigned char>((pixel[j]^t)^img[j]);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::GhostWaveTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    GhostTrails(copy1);
    collection.shiftFrames(copy1);
    int index = 0, dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (pix[j] * 0.7));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                dir = 0;
            }
        } else {
            --index;
            if(index <= 0) {
                index = 0;
                dir = 1;
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}
