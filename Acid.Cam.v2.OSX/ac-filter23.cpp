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

void ac::AlphaBlendImageDownUp(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat img, copy1 = frame.clone();
    ac_resize(blend_image, img, frame.size());
    BlendWithSource25(copy1);
    double alpha = 1.0;
    int dir = 1;
    AlphaBlend(copy1, img, frame, alpha);
    for(int i = 0; i < 2; ++i) {
        AlphaMovementMaxMin(alpha, dir, 0.005, 2.0, 1.0);
    }
    AddInvert(frame);
}

void ac::BlendWithImageAndSource(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha1 = 2.0, alpha2 = 1.0;
    static int dir1 = 0, dir2 = 1;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    BlendWithImage25(copy1);
    BlendWithSource25(reimage);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.05, 2.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 2.0, 1.0);
    AddInvert(frame);
}

void ac::PixelSourceFrameBlend256(cv::Mat &frame) {
    static MatrixCollection<256> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                unsigned int rgb[3] = {pixel[0], pixel[1], pixel[2]};
                cv::Vec3b pix[3];
                for(int q = 0; q < 3; ++q) {
                    pix[q] = collection.frames[rgb[q]].at<cv::Vec3b>(z, i);
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pix[j][j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SplitMatrixCollection(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int index = 0;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
                ++index;
                if(index > collection.size()-1)
                    index = 0;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RectangleGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0, dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                if(dir == 1) {
                    ++index;
                    if(index > collection.size()-1) {
                        index = collection.size()-1;
                        dir = 0;
                    } else {
                        --index;
                        if(index <= 1) {
                            index = 1;
                            dir = 1;
                        }
                    }
                }
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    
}

void ac::PositionShift(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = collection.frames[7].clone();
    int val_offset = rand()%(frame.cols-1);
    for(int z = 0; z < frame.rows; ++z) {
        int index = val_offset;
        for(int i = 0; i < frame.cols; ++i) {
            if(index < frame.cols-1) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, index);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++index;
            } else
                break;
        }
    }
    AddInvert(frame);
}

void ac::ColorCollectionMovementIndex(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[collection.size()/2].clone();
    frames[2] = collection.frames[collection.size()-1].clone();
    static int index = 0, dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                int values[3];
                InitArrayPosition(values, index);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[values[j]];
                }
                if(dir == 1) {
                    ++index;
                    if(index > 2) {
                        index = 2;
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
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::Shake(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[collection.size()/2].clone();
    frames[2] = collection.frames[collection.size()-1].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        int counter = rand()%3;
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                int values[3];
                InitArrayPosition(values, index);
                cv::Vec3b cpix = pixel;
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[values[j]].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
                pixel[values[counter]] = cpix[values[counter]];
                ++counter;
                if(counter > 2)
                    counter = 0;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2) {
        index = 0;
    }
}
