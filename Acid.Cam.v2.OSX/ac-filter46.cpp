

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

void ac::AlternateAlpha(cv::Mat &frame) {
    static double alpha = 0.5;
    static int dir = 1;
    static int rgb = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[rgb] = static_cast<unsigned char>((pixel[rgb] * 0.5) * alpha);
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 0.5, 0.1);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::Square_Block_Resize_Vertical_RGB(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    static int dir = 1;
    static int rgb = 0;
    
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        pixel[rgb] = static_cast<unsigned char>((0.5 * pixel[rgb]) + (0.5 * pix[rgb]));
                       
                    }
                }
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
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;

}

void ac::DiagSquareRGB(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int rgb = 0;
    for(int z = 0; z < frame.rows; z += 32) {
        for(int i = 0; i < frame.cols; i += 32) {
            for(int x = 0; x+i < frame.cols && x < 32; ++x) {
                for(int y = 0; z+y < frame.rows && y < 32; ++y) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel[rgb] = pix[rgb];
                }
            }
             ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    ++rgb;
    if(rgb > 2)
        rgb = 0;
    
    AddInvert(frame);
}

void ac::ShiftPixelsRGB(cv::Mat &frame) {
    static int offset = 1;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int rgb = 0;
    
    for(int z = 0; z < h; ++z) {
        int start = 0;
        for(int i = offset; i < w && start < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
            pixel[rgb] += source[rgb];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(int i = 0; i < offset-1 && start < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
            pixel[rgb] += source[rgb];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static int max_up = (w/16);
    if(direction == 1) {
        ++offset;
        if(offset > max_up)  {
            direction = 0;
            max_up += 4;
            if(max_up > (w/4)) {
                max_up = (w/16);
            }
        }
    } else if(direction == 0) {
        --offset;
        if(offset < 2) direction = 1;
    }
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::UseOldRowVertRGB(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.cols / collection1.size());
    static int square_size = 25 + (rand()% 1+(square_max - 25));
    static int rgb = 0;
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.cols-1) {
        square_size = 25 + (rand()% 1+(square_max - 25));
        for(int i = row; i < row+square_size; ++i) {
            int val = (rand()%10);
            for(int z = 0; z < frame.cols; ++z) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    if(val > 7) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            pixel[rgb] = static_cast<unsigned char>((0.5 * pixel[rgb]) + (0.5 * pix[rgb]));
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            pixel[rgb] = static_cast<unsigned char>((0.5 * pixel[rgb]) + (0.5 * pix[rgb]));
                        }
                    }
                }
            }
        }
        row += square_size;
        size_past += square_size;
        if(size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection1.size()-1))
                break;
        }
    }
    
    static int counter = 0;
    if(++counter > 10) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::ColorWaveTrailsRGB(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    MatrixCollectionAuraTrails(copy1);
    collection.shiftFrames(copy1);
    int index = 0, dir = 1;
    static int rgb = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel[rgb] = static_cast<unsigned char>((0.3 * pixel[rgb]) + (pix[rgb] * 0.7));
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
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::MirrorDiamondRGB(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int index = 0;
    static int rgb = 0;
    int start_x = 0;
    int off_x = index;
    for(int z = 0; z < frame.rows; ++z) {
        start_x = off_x;
        for(int i = 0; i < frame.cols; ++i) {
            if(start_x > frame.cols-1) {
                start_x = 0;
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, start_x);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
            ++start_x;
        }
        ++off_x;
        if(off_x > frame.cols)
            off_x = 0;
    }
    ++index;
    if(index > frame.cols)
        index = 0;
    
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::Square_Block_Resize_RGB(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    static int dir = 1;
    static int rgb = 0;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        pixel[rgb] = static_cast<unsigned char>((0.5 * pixel[rgb]) + (0.5 * pix[rgb]));
                    }
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
    }
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
    
}

void ac::VariableLinesY_RGB(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.rows;
    static int rgb = 0;
    for(int i = 0; i < frame.cols; i++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.rows-1) {
                offset = frame.rows-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int z = offset; z < frame.rows && pos < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(pos, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
            ++pos;
        }
        for(int z = 0; z < offset && pos < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(pos, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
            ++pos;
        }
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::MedianBlendMultiThread_RGB(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    static int rgb = 0;
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection.size(); ++j) {
                    cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                    value[rgb] += pixel[rgb];
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                int val = 1+static_cast<int>(value[rgb]);
                pixel[rgb] = static_cast<unsigned char>(pixel[rgb] ^ val);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}
