

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

void ac::AlternateAlpha(cv::Mat &frame) {
    static double alpha = 0.5;
    static int dir = 1;
    static int rgb = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
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
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
            pixel[rgb] += source[rgb];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(int i = 0; i < offset-1 && start < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
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
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, start_x);
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
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
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
            cv::Vec3b &pixel = pixelAt(frame,pos, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
            ++pos;
        }
        for(int z = 0; z < offset && pos < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,pos, i);
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

void ac::SlitScanDir_RGB(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRow480pX2(copy1);
    IntertwineCols640(copy2);
    int val = rand()%6;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = copy2.at<cv::Vec3b>(z, i);
            if(val == 0) {
                pixel[0] = pix1[0];
                pixel[2] = pix2[2];
            } else if(val == 1){
                pixel[0] = pix2[0];
                pixel[2] = pix1[2];
            } else if(val == 2) {
                pixel[0] = pix1[0];
                pixel[1] = pix2[1];
            } else if(val == 3) {
                pixel[0] = pix2[0];
                pixel[1] = pix1[1];
            } else if(val == 4) {
                pixel[2] = pix1[2];
                pixel[0] = pix2[0];
            } else if(val == 5) {
                pixel[2] = pix2[2];
                pixel[1] = pix1[1];
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShiftDirGradient(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.rows/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            int index[3];
            index[0] = rand()%255;
            index[1] = rand()%255;
            index[2] = rand()%255;
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                
                if(on == 0) {
                    for(int i = off; i < frame.cols; ++i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        ++pos;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.3) *(0.7 * pix[j]+index[j]));
                            
                        }
                    }
                } else {
                    pos = frame.cols-1;
                    for(int i = frame.cols-1-off; i > 1; --i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        --pos;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.3) *(0.7 * pix[j]+index[j]));
                        }
                    }
                }
                for(int j = 0; j < 3; ++j)
                    index[j] += rand()%10;
            }
        }
    }
    AddInvert(frame);
}

void ac::MovementTrailsXRGB_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MovementTrailsXRGB_SubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    static int rgb = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            bool set_value = false;
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                if(abs(pixel[rgb]-pix[rgb]) > getPixelCollection()) {
                    pixel[rgb] = pix1[rgb];
                    set_value = true;
                }
                if(set_value == true)
                    break;
            }
        }
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::TwistedVision_RGB(cv::Mat &frame) {
    static int pos[3] = {2,frame.cols-1,2};
    static int sized_w = frame.size().width;
    static int rgb = 0;
    if(sized_w != frame.size().width) {
        pos[1] = frame.cols-1;
        sized_w = frame.size().width;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int cx = i+pos[0];
            if(cx >= 0 && cx < frame.cols) {
                cv::Vec3b pix = pixelAt(frame,z, cx);
                pixel[rgb] = pix[rgb]^pixel[rgb];
            } else {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = pixel[j]^pos[0];
            }
            int cx_x = i+pos[1];
            if(cx_x >= 0 && cx_x < frame.cols) {
                cv::Vec3b pix=pixelAt(frame,z, cx_x);
                pixel[rgb] = pixel[rgb]^pix[rgb];
            } else {
                pixel[rgb] = pixel[rgb]^pos[1];
            }
        }
    }
    ++pos[0];
    if(pos[0] > frame.cols/2) {
        pos[0] = 2;
    }
    --pos[1];
    if(pos[1] <= frame.cols/2)
        pos[1] = frame.cols-1;
    
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::CollectionWave_RGB(cv::Mat &frame) {
    static MatrixCollection<24> collection1;
    collection1.shiftFrames(frame);
    static int row_size = 10;
    static int rgb = 0;
    int row_index = 0;
    int row_dir = 1, row_size_dir = 1;
    for(int q = 0; q < frame.rows; q += row_size) {
        for(int z = 0; z < q+row_size && z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = collection1.frames[row_index].at<cv::Vec3b>(z, i);
                pixel[rgb] = static_cast<unsigned char>((0.5 * pixel[rgb]) + (0.5 * pix1[rgb]));
            }
            if(row_dir == 1) {
                ++row_index;
                if(row_index > (collection1.size()-1)) {
                    row_index = collection1.size()-1;
                    row_dir = 0;
                }
            } else {
                --row_index;
                if(row_index <= 1) {
                    row_index = 1;
                    row_dir = 1;
                    if(row_size_dir == 1) {
                        row_size += 20;
                        if(row_size > 100) {
                            row_size_dir = 0;
                        }
                    } else {
                        row_size -= 20;
                        if(row_size <= 10) {
                            row_size = 50;
                            row_size_dir = 1;
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

namespace {
    int blend_percent = 50;
}

void ac::setBlendPercent(int percent) {
    blend_percent = percent;
}

void ac::BlendWithSourcePercent(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        cv::Mat copy1 = frame.clone();
        double val = 0.01 * blend_percent;
        AlphaBlendDouble(copy1, orig_frame, frame, 1-val, val);
        AddInvert(frame);
    }
}

void ac::ReverseRandom(cv::Mat &frame) {
    bool rev = false;
    int rev_h = 100+(rand()%100);
    int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z){
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(rev == true) {
                pixel = copy1.at<cv::Vec3b>(z, (frame.cols-i-1));
            }
        }
        ++index;
        if(index >= rev_h) {
            rev = !rev;
            index = 0;
            rev_h = 100+(rand()%500);
        }
    }
    AddInvert(frame);
}

void ac::ReverseRandomCollection(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int val_index = 0;
    bool rev = false;
    int rev_h = 100+(rand()%100);
    int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z){
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(rev == true) {
                pixel = collection.frames[val_index].at<cv::Vec3b>(z, (frame.cols-i-1));
            }
        }
        ++index;
        if(index >= rev_h) {
            rev = !rev;
            index = 0;
            rev_h = 100+(rand()%500);
            ++val_index;
            if(val_index > (collection.size()-1))
                val_index = 0;
        }
    }
    AddInvert(frame);
}

void ac::RevesreRandomChannelCollection(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int val_index = 0;
    static int offset = 0;
    bool rev = false;
    int rev_h = 100+(rand()%100);
    int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z){
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(rev == true) {
                pixel[offset] = collection.frames[val_index].at<cv::Vec3b>(z, (frame.cols-i-1))[offset];
            }
        }
        ++index;
        if(index >= rev_h) {
            rev = !rev;
            index = 0;
            rev_h = 100+(rand()%500);
            ++val_index;
            if(val_index > (collection.size()-1))
                val_index = 0;
            ++offset;
            if(offset > 2)
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::ReverseRandomCollectionResize(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int val_index = 0;
    static int max_size = 100;
    bool rev = false;
    int rev_h = 100+(rand()%max_size);
    int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z){
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(rev == true) {
                pixel = collection.frames[val_index].at<cv::Vec3b>(z, (frame.cols-i-1));
            }
        }
        ++index;
        if(index >= rev_h) {
            rev = !rev;
            index = 0;
            rev_h = 100+(rand()%max_size);
            ++val_index;
            if(val_index > (collection.size()-1))
                val_index = 0;
            
            max_size += 10;
            if(max_size > 700) {
                max_size = 100;
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareBlockGlitch(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int off = 0;
    static int offx = 0;
    static int frame_height = frame.rows/4;
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            offx = rand()%collection.size();
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                if(on == 0) {
                    for(int i = off; i < frame.cols; ++i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = collection.frames[offx].at<cv::Vec3b>(z, pos);
                        ++pos;
                        pixel = pix;
                    }
                } else {
                    pos = frame.cols-1;
                    for(int i = frame.cols-1-off; i > 1; --i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = collection.frames[offx].at<cv::Vec3b>(z, pos);
                        --pos;
                        pixel = pix;
                    }
                }
            }
        }
    }
    ++offx;
    if(offx > (collection.size()-1))
        offx = 0;
    AddInvert(frame);
}

void ac::SquareStretchRows(cv::Mat &frame) {
    static constexpr int COPY_SIZE=4;
    cv::Mat copies[COPY_SIZE];
    for(int i = 0; i < COPY_SIZE; ++i) {
        ac_resize(frame, copies[i], cv::Size(frame.cols+((rand()%10) * 100), frame.rows));
    }
    
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/4)) {
        for(int z = row; z < row+(frame.rows/4) && z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(offset < COPY_SIZE-1 && i < frame.cols && z < frame.rows && i < copies[offset].cols && z < copies[offset].rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, i);
                    pixel = pix;
                }
            }
        }
        offset++;
        if(offset > COPY_SIZE-1)
            offset = 0;
    }
    AddInvert(frame);
}

void ac::SquareStretchRowsDelay(cv::Mat &frame) {
    cv::Mat copies[6];
    for(int i = 0; i < 6; ++i) {
        ac_resize(frame, copies[i], cv::Size(frame.cols+((rand()%10) * 100), frame.rows));
    }
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/6)) {
        
        if((rand()%50) != 0)
            continue;
        
        for(int z = row; z < row+(frame.rows/6) && z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        offset++;
    }
    AddInvert(frame);
}

void ac::SquareStretchEven(cv::Mat &frame) {
    static constexpr int COPY_SIZE=6;
    cv::Mat copies[COPY_SIZE];
    for(int i = 0; i < 6; ++i) {
        ac_resize(frame, copies[i], cv::Size(frame.cols+((rand()%5) * 30), frame.rows));
    }
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/6)) {
        for(int z = row; z < row+(frame.rows/6) && z < frame.rows; ++z) {
            if(offset < COPY_SIZE-1) {
                int start = copies[offset].cols-frame.cols;
                for(int i = 0; i < frame.cols; ++i) {
                    if(z < frame.rows-1 && i < frame.cols-1) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        if(z < copies[offset].rows-1 && start < copies[offset].cols-1) {
                            cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, start);
                            pixel = pix;
                        }
                    }
                    ++start;
                }
            }
        }
        offset++;
    }
    AddInvert(frame);
}

void ac::SketchFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copies[8];
    for(int i = 0; i < 8; ++i) {
        ac_resize(collection.frames[i], copies[i], cv::Size(frame.cols+((rand()%5) * 30), frame.rows));
    }
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/8)) {
        for(int z = row; z < row+(frame.rows/8) && z < frame.rows; ++z) {
            int start = copies[offset].cols-frame.cols;
            offset = rand()%collection.size();
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, start);
                pixel = pix;
                ++start;
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareStretchEvenCollection(cv::Mat &frame) {
    static constexpr int MAX_FRAMES=16;
    static MatrixCollection<MAX_FRAMES> collection;
    collection.shiftFrames(frame);
    cv::Mat copies[MAX_FRAMES];
    for(int i = 0; i < MAX_FRAMES; ++i) {
        ac_resize(collection.frames[i], copies[i], cv::Size(frame.cols+((rand()%5) * 30), frame.rows));
    }
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/MAX_FRAMES)) {
        offset = rand()%collection.size();
        for(int z = row; z < row+(frame.rows/MAX_FRAMES) && z < frame.rows; ++z) {
            int start = copies[offset].cols-frame.cols;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, start);
                pixel = pix;
                ++start;
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareStretchEven32(cv::Mat &frame) {
    static constexpr int MAX_FRAMES=32;
    cv::Mat copies[MAX_FRAMES];
    for(int i = 0; i < MAX_FRAMES; ++i) {
        ac_resize(frame, copies[i], cv::Size(frame.cols+((rand()%5) * 10), frame.rows));
    }
    int offset = 0;
    for(int row = 0; row < frame.rows; row += (frame.rows/MAX_FRAMES)) {
        for(int z = row; z < row+(frame.rows/MAX_FRAMES) && z < frame.rows; ++z) {
            int start = copies[offset].cols-frame.cols;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copies[offset].at<cv::Vec3b>(z, start);
                pixel = pix;
                ++start;
            }
        }
        ++offset;
        if(offset > MAX_FRAMES-1)
            offset = 0;
    }
    AddInvert(frame);
}

void ac::RGBLineFuzz(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        int offset = rand()%frame.rows;
        for(int i = 0; i < frame.cols; ++i) {
            ++offset;
            if(offset > frame.rows-1)
                offset = 0;
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(frame,offset, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        
    }
    RGBSplitFilter(frame);
    AddInvert(frame);
}

void ac::RGBLineFuzzX(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        int offset = rand()%frame.cols;
        for(int i = 0; i < frame.cols; ++i) {
            ++offset;
            if(offset > frame.cols-1)
                offset = 0;
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(frame,z, offset);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        
    }
    RGBSplitFilter(frame);
    AddInvert(frame);
}

void ac::ChannelSortCollection32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    std::vector<cv::Mat> v;
    std::vector<cv::Mat> v1;
    std::vector<cv::Mat> v2;
    cv::split(collection.frames[0], v);
    cv::split(collection.frames[15], v1);
    cv::split(collection.frames[31], v2);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v[0], channels[0],cv::SORT_ASCENDING);
    cv::sort(v1[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v2[2], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

