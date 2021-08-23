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

#include "ac.h"

void ac::XorRow(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static double num = 2;
    static int num_dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if((z%static_cast<int>(num)) == 0) {
                        pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
                    } else {
                        pixel[j] ^= static_cast<unsigned char>(0.5 * (pixel[j]*alpha));
                    }
                }
            }
        }
    };
    AlphaMovementMaxMin(num, num_dir, 1.0, 50, 2.0);
    AlphaMovementMaxMin(alpha, dir, 0.001, 255.0, 1.0);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::UseOldRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size()-1);
    static int square_size = 2 + (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            int val = (rand()%10);
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(val > 7) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
}

void ac::UseEveryOtherRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size()-1);
    static int square_size = 2 + (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 2+ (rand()% (square_max));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if((row%2)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 8) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOffRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size()-1);
    static int square_size = 2+ (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 2+ (rand()% (square_max));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if((row%4)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 4) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::FrameJump(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int frame_counter = 0;
    static int fps_ = static_cast<int>(ac::fps);
    ++frame_counter;
    if((frame_counter%(fps_/6)) == 0) {
        frame = collection.frames[rand()%(collection.size()-1)].clone();
    }
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadVariable(cv::Mat &frame) {
    static constexpr int SIZE = 8;
    static MatrixCollection<SIZE> collection;
    MedianBlendMultiThreadVariable(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadVariable32(cv::Mat &frame) {
    static constexpr int SIZE = 32;
    static MatrixCollection<SIZE> collection;
    MedianBlendMultiThreadVariable(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadVariable16(cv::Mat &frame) {
    static constexpr int SIZE = 16;
    static MatrixCollection<SIZE> collection;
    MedianBlendMultiThreadVariable(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadSkip8(cv::Mat &frame) {
    static constexpr int SIZE = 8;
    static MatrixCollection<SIZE> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    static int counter = 0;
    ++counter;
    if((counter%8)==0) {
        collection.shiftFrames(frame);
    }
    MedianBlendMultiThreadNoShift(frame, &collection, 1);
    AddInvert(frame);
}

void ac::BlockyTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    static int counter = 0;
    ++counter;
    if((counter%8)==0) {
        collection.shiftFrames(frame);
    }
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    for(int q = 0; q < 3; ++q) {
        int off = rand()%3;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[off].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::BlockyTrails16(cv::Mat &frame) {
    static constexpr int SIZE = 16;
    static MatrixCollection<SIZE> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    static int counter = 0;
    ++counter;
    if((counter%8)==0) {
        collection.shiftFrames(frame);
    }
    cv::Mat frames[6];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    frames[3] = collection.frames[10].clone();
    frames[4] = collection.frames[13].clone();
    frames[5] = collection.frames[15].clone();
    for(int q = 0; q < 6; ++q) {
        int off = rand()%6;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[off].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::BlockyTrails32(cv::Mat &frame) {
    static constexpr int SIZE = 32;
    static MatrixCollection<SIZE> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    static int counter = 0;
    ++counter;
    if((counter%8)==0) {
        collection.shiftFrames(frame);
    }
    cv::Mat frames[9];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    frames[3] = collection.frames[10].clone();
    frames[4] = collection.frames[13].clone();
    frames[5] = collection.frames[15].clone();
    frames[6] = collection.frames[19].clone();
    frames[7] = collection.frames[26].clone();
    frames[8] = collection.frames[31].clone();
    for(int q = 0; q < 9; ++q) {
        int off = rand()%9;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[off].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::BlockyTrails64(cv::Mat &frame) {
    static constexpr int SIZE = 64;
    static MatrixCollection<SIZE> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    static int counter = 0;
    ++counter;
    if((counter%8)==0) {
        collection.shiftFrames(frame);
    }
    cv::Mat frames[18];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    frames[3] = collection.frames[10].clone();
    frames[4] = collection.frames[13].clone();
    frames[5] = collection.frames[15].clone();
    frames[6] = collection.frames[19].clone();
    frames[7] = collection.frames[26].clone();
    frames[8] = collection.frames[31].clone();
    frames[9] = collection.frames[34].clone();
    frames[10] = collection.frames[37].clone();
    frames[11] = collection.frames[40].clone();
    frames[12] = collection.frames[44].clone();
    frames[13] = collection.frames[47].clone();
    frames[14] = collection.frames[50].clone();
    frames[15] = collection.frames[54].clone();
    frames[16] = collection.frames[57].clone();
    frames[17] = collection.frames[63].clone();
    for(int q = 0; q < 18; ++q) {
        int off = rand()%18;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[off].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::UseOffRowVert(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.cols / collection1.size());
    static int square_size = 2 + (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.cols-1) {
        square_size = 2+ (rand()% (square_max));
        for(int i = row; i < row+square_size; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if((row%4)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 4) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOldRowVert(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.cols / collection1.size());
    static int square_size = 2+ (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.cols-1) {
        square_size = 2+ (rand()% 1+(square_max));
        for(int i = row; i < row+square_size; ++i) {
            int val = (rand()%10);
            for(int z = 0; z < frame.cols; ++z) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(val > 7) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
}

void ac::UseEveryOtherRowVert(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.cols / collection1.size());
    static int square_size = 2+ (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 2 + (rand()% (square_max));
        for(int i = row; i < row+square_size; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if((row%2)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 8) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOffRowDir(cv::Mat &frame) {
    UseOffRow(frame);
    UseOffRowVert(frame);
    AddInvert(frame);
}

void ac::UseOldRowDir(cv::Mat &frame) {
    UseOldRow(frame);
    UseOldRowVert(frame);
    AddInvert(frame);
}

void ac::UseEveryOtherRowDir(cv::Mat &frame) {
    UseEveryOtherRow(frame);
    UseEveryOtherRowVert(frame);
    AddInvert(frame);
}

void ac::SetCurrentFrameStateAsSource(cv::Mat &frame) {
    ac::orig_frame = frame.clone();
    AddInvert(frame);
}

void ac::RegularTrails(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    frames[3] = collection.frames[11].clone();
    for(int q = 0; q < 4; ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[q].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::UseOldRow64(cv::Mat &frame) {
    static MatrixCollection<64> collection1;
    static MatrixCollection<64> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.rows / collection1.size()-1);
    static int square_size = 2+ (rand()% (square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 2+ (rand()% 1+(square_max));
        for(int z = row; z < row+square_size; ++z) {
            int val = (rand()%10);
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(val > 5) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 5) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOldRowVert64(cv::Mat &frame) {
    static MatrixCollection<64> collection1;
    static MatrixCollection<64> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int square_max = (frame.cols / collection1.size());
    static int square_size = 2+ (rand()% 1+(square_max));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.cols-1) {
        square_size = 2 + (rand()% (square_max));
        for(int i = row; i < row+square_size; ++i) {
            int val = (rand()%10);
            for(int z = 0; z < frame.cols; ++z) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(val > 5) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
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
    if(++counter > 5) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOldRowDir64(cv::Mat &frame) {
    UseOldRow64(frame);
    UseOldRowVert64(frame);
    AddInvert(frame);
}

void ac::GradientColorBlend(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (alpha[j] * r_color));
            }
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadGradientGray(cv::Mat &frame) {
    GradientColorBlend(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::GradientRedBlend(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[2] = static_cast<unsigned char>((0.5 * pixel[2]) + (alpha[2] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadGradientRed(cv::Mat &frame) {
    GradientRedBlend(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::GradientGreenBlend(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[1] = static_cast<unsigned char>((0.5 * pixel[1]) + (alpha[1] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadGradientGreen(cv::Mat &frame) {
    GradientGreenBlend(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::GradientBlueBlend(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>((0.5 * pixel[0]) + (alpha[0] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadGradientBlue(cv::Mat &frame) {
    GradientBlueBlend(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::GradientColorBlendAll(cv::Mat &frame) {
    GradientRedBlend(frame);
    GradientGreenBlend(frame);
    GradientBlueBlend(frame);
    AddInvert(frame);
}
void ac::MedianBlendMultiThreadGradientAll(cv::Mat &frame) {
    GradientColorBlendAll(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::GradientXRed(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.cols;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[2] = static_cast<unsigned char>((0.5 * pixel[2]) + (alpha[2] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}
void ac::GradientXGreen(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.cols;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>((0.5 * pixel[1]) + (alpha[1] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}
void ac::GradientXBlue(cv::Mat &frame) {
    double alpha[3] = {0};
    static double r_color = rand()%255;
    static int r_dir = 1;
    double alpha_inc = 0.5/frame.cols;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>((0.5 * pixel[0]) + (alpha[0] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AlphaMovementMaxMin(r_color, r_dir, 5.0, 255.0, 1.0);
    AddInvert(frame);
}

void ac::GradientAll(cv::Mat &frame) {
    GradientRedBlend(frame);
    GradientXRed(frame);
    GradientGreenBlend(frame);
    GradientXGreen(frame);
    GradientBlueBlend(frame);
    GradientXBlue(frame);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadAllGradients(cv::Mat &frame) {
    GradientAll(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::StretchRowMatrix16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
}

void ac::StretchRowMatrix32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
}

void ac::StretchRowMatrix8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
}

