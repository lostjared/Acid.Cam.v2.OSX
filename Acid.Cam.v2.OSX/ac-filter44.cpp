
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

void ac::AddCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AddCollectionSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int colors[3] = {0};
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pix = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q)
                    colors[q] += pix[q];
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = colors[j]/collection.size();
            }
        }
    }
    AddInvert(frame);
}

void ac::AddCollectionXor_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AddCollectionXor_SubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int colors[3] = {0};
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pix = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q)
                    colors[q] += pix[q];
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= colors[j]/collection.size();
            }
        }
    }
    AddInvert(frame);
}

void ac::ProperTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7],out,0.5, 0.5);
    collection.shiftFrames(out);
    frame = out.clone();
    AddInvert(frame);
}

void ac::ProperTrails_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ProperTrails_SubFilter")
        return;
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7],out,0.5, 0.5);
    CallFilter(subfilter, out);
    collection.shiftFrames(out);
    frame = out.clone();
    AddInvert(frame);
}

void ac::StuckFrame_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "StuckFrame_SubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7], out, 0.5, 0.5);
    frame = out.clone();
    AddInvert(frame);
}

void ac::XorLag(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat &cp = collection.frames[31];
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = cp.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^pix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelateBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    VariableRectanglesExtra(copy1);
    Square_Block_Resize_Vertical(copy1);
    collection.shiftFrames(copy1);
    int index = 0;
    int counter = 0;
    int wait = 1+rand()%50;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
            ++counter;
            if(counter > wait) {
                wait = 1+(rand()%50);
                ++index;
                counter = 0;
                if(index > collection.size()-1)
                    index = 0;
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelateRect(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    StretchRowMatrix16(copy1);
    StretchColMatrix16(copy1);
    collection.shiftFrames(copy1);
    for(int j = 0; j < 100; ++j) {
        int index = rand()%collection.size();
        int start_x = rand()%frame.cols;
        int stop_x = rand()%frame.cols;
        for(int i = start_x; i < frame.cols && i < start_x+stop_x; ++i) {
            int start_y = rand()%frame.rows;
            int stop_y = rand()%frame.rows;
            for(int z = start_y; z < frame.rows && z < start_y+stop_y; ++z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::RGBSplitFilter(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b rgb[3];
            rgb[0] = collection.frames[15].at<cv::Vec3b>(z, i);
            rgb[1] = collection.frames[7].at<cv::Vec3b>(z, i);
            rgb[2] = collection.frames[0].at<cv::Vec3b>(z, i);
            pixel[0] = rgb[0][0];
            pixel[1] = rgb[1][1];
            pixel[2] = rgb[2][2];
        }
    }
    AddInvert(frame);
}

void ac::DiagPixel(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(i < frame.cols) {
                cv::Vec3b pix = collection.frames[off].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j])+(0.5 * pix[j]));
                }
            }
        }
        ++off;
        if(off > collection.size()-1)
            off = 0;
    }
    AddInvert(frame);
}

void ac::DiagPixelY(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[off].at<cv::Vec3b>(z, i);
            pixel = pix;
            
            ++off;
            if(off > collection.size()-1)
                off = 0;

        }
    }
    AddInvert(frame);
}

void ac::DiagPixelY2(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        int off = rand()%(collection.size()-1);
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[off].at<cv::Vec3b>(z,i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::DiagPixelY3(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int off = rand()%(collection.size()-1);
            cv::Vec3b pix = collection.frames[off].at<cv::Vec3b>(z,i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::DiagPixelY4(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    if(collection.empty()) collection.shiftFrames(frame);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[rand()%(collection.size()-1)], out, 0.5, 0.5);
    collection.shiftFrames(out);
    int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        index = rand()%(collection.size()-1);
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 0.1, 1.0);
    AddInvert(frame);
}

void ac::ExpandLeftRight(cv::Mat &frame) {
    static int off = rand()%frame.cols;
    static int off_dir = 1;
    static int counter = 0;
    static int cdir = 1;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(i+off < frame.cols) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i+off);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            } else if(off-i > 0) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, off-i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
        if(cdir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                cdir = 0;
            }
        } else {
            --index;
            if(index <= 1) {
                index = 1;
                cdir = 1;
            }
        }
        ++counter;
        if(counter > 3) {
            counter = 0;
            if(off_dir == 1) {
                ++off;
                if(off > frame.cols) {
                    off = frame.cols-1;
                    off_dir = 0;
                }
            } else {
                --off;
                if(off <= 1) {
                    off = 1;
                    off_dir = 1;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::DiagSquare(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    for(int z = 0; z < frame.rows; z += 32) {
        for(int i = 0; i < frame.cols; i += 32) {
            for(int x = 0; x+i < frame.cols && x < 32; ++x) {
                for(int y = 0; z+y < frame.rows && y < 32; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::DiagSquareLarge(cv::Mat &frame) {
    static constexpr int SIZE=32;
    static MatrixCollection<SIZE> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int square = 64;
    for(int z = 0; z < frame.rows; z += square) {
        for(int i = 0; i < frame.cols; i += square) {
            for(int x = 0; x+i < frame.cols && x < square; ++x) {
                for(int y = 0; z+y < frame.rows && y < square; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::DiagonalSquareCollection(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    for(int z = 0; z < frame.rows; z += 32) {
        for(int i = 0; i < frame.cols; i += 32) {
            for(int x = 0; x+i < frame.cols && x < 32; ++x) {
                for(int y = 0; z+y < frame.rows && y < 32; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
                ++offset;
                if(offset > (collection.size()-1))
                    offset = 0;

            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::DiagnoalSquareSize(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 32;
    static int offset = 0;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int x = 0; x+i < frame.cols && x < square_size; ++x) {
                for(int y = 0; z+y < frame.rows && y < square_size; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++square_size;
        if(square_size >= 128) {
            square_size = 128;
            dir = 0;
        }
    } else {
        --square_size;
        if(square_size <= 8) {
            square_size = 8;
            dir = 1;
        }
    }
}

void ac::DiagonalSquareSizeOnOff(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 32;
    static int offset = 0;
    bool on = true;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int x = 0; x+i < frame.cols && x < square_size; ++x) {
                for(int y = 0; z+y < frame.rows && y < square_size; ++y) {
                    if(on == true) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                        pixel = pix;
                    }
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
            on = (on == true) ? false : true;
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++square_size;
        if(square_size >= 128) {
            square_size = 128;
            dir = 0;
        }
    } else {
        --square_size;
        if(square_size <= 8) {
            square_size = 8;
            dir = 1;
        }
    }
}

void ac::DiagonalSquareSizeOnOffRandom(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 32;
    static int offset = 0;
    bool on = true;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int x = 0; x+i < frame.cols && x < square_size; ++x) {
                for(int y = 0; z+y < frame.rows && y < square_size; ++y) {
                    if(on == true) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                        pixel = pix;
                    }
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;

            if( (rand()%25) > 15)
                on = true;
            else
                on = false;
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++square_size;
        if(square_size >= 128) {
            square_size = 128;
            dir = 0;
        }
    } else {
        --square_size;
        if(square_size <= 8) {
            square_size = 8;
            dir = 1;
        }
    }
}

namespace ac {
    int num_frames = 0;
    int slit_width = 0;
    int slit_height = 0;
    int slit_repeat = 1;
    int slit_delay = 0;
    int slit_on = 0;
    ac::DynamicMatrixCollection collection(480);
}

std::mutex value;
bool values_set = false;

void ac::slitScanSet(int num, int width, int height, int repeat, int delay, int on) {
    value.lock();
    if(num != num_frames) {
        collection.resize(num);
        num_frames = num;
    }
    slit_width = width;
    slit_height = height;
    slit_repeat = repeat;
    slit_delay = delay;
    slit_on = on;
    values_set = true;
    value.unlock();
}

void ac::SlitScanGUI(cv::Mat &frame) {
    
    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    static int time_count = 0;
    static int seconds = 0;
    static bool add = true;
    
    ++time_count;
    if(time_count > static_cast<int>(ac::fps)) {
        ++seconds;
        time_count = 0;
    }
    if(add == false && ((seconds>=slit_delay))) {
        add = true;
        seconds = 0;
    }
    
    if(add == true) {
        static int stop_timer = 0;
        ++stop_timer;
        static int stop_seconds = 0;
        if(stop_timer > static_cast<int>(ac::fps)) {
            ++stop_seconds;
            if(stop_seconds >= slit_on) {
                add = false;
                stop_seconds = 0;
            }
        }
    }
    if(slit_delay == 0 || add == true)
        collection.shiftFrames(copy1);
    
    int index = 0;
    int counter = 0;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::SlitScanXGUI(cv::Mat &frame) {
    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    static int time_count = 0;
    static int seconds = 0;
    static bool add = true;
    
    ++time_count;
    if(time_count > static_cast<int>(ac::fps)) {
        ++seconds;
        time_count = 0;
    }
    if(add == false && ((seconds>=slit_delay))) {
        add = true;
        seconds = 0;
    }
    
    if(add == true) {
        static int stop_timer = 0;
        ++stop_timer;
        static int stop_seconds = 0;
        if(stop_timer > static_cast<int>(ac::fps)) {
            ++stop_seconds;
            if(stop_seconds >= slit_on) {
                add = false;
                stop_seconds = 0;
            }
        }
    }
    if(slit_delay == 0 || add == true)
        collection.shiftFrames(copy1);
    
    int index = 0;
    int counter = 0;
    for(int i = 0; i < copy1.cols; ++i) {
        for(int z = 0; z < copy1.rows; ++z) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::SlitScanGUI_RGB(cv::Mat &frame) {

    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    static int time_count = 0;
    static int seconds = 0;
    static bool add = true;
    
    int rgb = rand()%3;
    
    ++time_count;
    if(time_count > static_cast<int>(ac::fps)) {
        ++seconds;
        time_count = 0;
    }
    if(add == false && ((seconds>=slit_delay))) {
        add = true;
        seconds = 0;
    }
    
    if(add == true) {
        static int stop_timer = 0;
        ++stop_timer;
        static int stop_seconds = 0;
        if(stop_timer > static_cast<int>(ac::fps)) {
            ++stop_seconds;
            if(stop_seconds >= slit_on) {
                add = false;
                stop_seconds = 0;
            }
        }
    }
    if(slit_delay == 0 || add == true)
        collection.shiftFrames(copy1);
    
    int index = 0;
    int counter = 0;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
        }
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::SlitScan_Colors(cv::Mat &frame) {
    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    static int time_count = 0;
    static int seconds = 0;
    static bool add = true;
    
    static int rgb = 0;
    
    ++time_count;
    if(time_count > static_cast<int>(ac::fps)) {
        ++seconds;
        time_count = 0;
    }
    if(add == false && ((seconds>=slit_delay))) {
        add = true;
        seconds = 0;
    }
    
    if(add == true) {
        static int stop_timer = 0;
        ++stop_timer;
        static int stop_seconds = 0;
        if(stop_timer > static_cast<int>(ac::fps)) {
            ++stop_seconds;
            if(stop_seconds >= slit_on) {
                add = false;
                stop_seconds = 0;
            }
        }
    }
    if(slit_delay == 0 || add == true)
        collection.shiftFrames(copy1);
    
    int index = 0;
    int counter = 0;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
        }
        ++rgb;
        if(rgb > 2)
            rgb = 0;
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::SlitScanXGUI_RGB(cv::Mat &frame) {
    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    static int time_count = 0;
    static int seconds = 0;
    static bool add = true;
    
    ++time_count;
    if(time_count > static_cast<int>(ac::fps)) {
        ++seconds;
        time_count = 0;
    }
    if(add == false && ((seconds>=slit_delay))) {
        add = true;
        seconds = 0;
    }
    
    if(add == true) {
        static int stop_timer = 0;
        ++stop_timer;
        static int stop_seconds = 0;
        if(stop_timer > static_cast<int>(ac::fps)) {
            ++stop_seconds;
            if(stop_seconds >= slit_on) {
                add = false;
                stop_seconds = 0;
            }
        }
    }
    if(slit_delay == 0 || add == true)
        collection.shiftFrames(copy1);
    
    int index = 0;
    int counter = 0;
    int rgb = rand()%3;
    for(int i = 0; i < copy1.cols; ++i) {
        for(int z = 0; z < copy1.rows; ++z) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel[rgb] = pix[rgb];
        }
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::SlitScanRandom(cv::Mat &frame) {
    
    if(values_set == false)
        return;
    
    value.lock();
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(slit_width, slit_height));
    
    if(collection.empty())
        collection.shiftFrames(copy1);
    
    cv::Mat copy2 = collection.frames[collection.size()-2].clone();
    
    if((rand()%50) > 25)
        collection.shiftFrames(copy1);
    else
        collection.shiftFrames(copy2);
    
    int index = 0;
    int counter = 0;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        ++counter;
        if(counter >= slit_repeat) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            counter = 0;
        }
    }
    ac_resize(copy1, frame, frame.size());
    value.unlock();
    AddInvert(frame);
}

void ac::VideoInterlacedRandom(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols-(rand()%50); ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
            ++z;
            for(int i = frame.cols-1; i > rand()%50; --i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

// requires allocation of at least 2,000 frames
void ac::VideoSlitScan(cv::Mat &frame) {
    if(values_set == false)
        return;
    static MatrixCollection<360> collection1;
    static MatrixCollection<360> collection2;
    if(collection1.empty()) {
        cv::Mat val;
        cv::resize(frame, val, cv::Size(640, 360));
        collection1.shiftFrames(val);
        collection2.shiftFrames(val);
    }
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat copy1;
        ac_resize(frame, copy1, cv::Size(640, 360));
        collection1.shiftFrames(copy1);
        cv::Mat reframe;
        cv::resize(vframe, reframe, cv::Size(640, 360));
        collection2.shiftFrames(reframe);
        int index = 0;
        for(int z = 0; z < copy1.rows; ++z) {
            for(int i = 0; i < copy1.cols; ++i) {
                cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b pix1 = collection1.frames[index].at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = collection2.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pix1[j]) + (0.5 * pix2[j]));
                }
            }
            ++index;
            if(index > collection1.size()-1)
                index = 0;
        }
        ac_resize(copy1, frame, frame.size());
    }
    AddInvert(frame);
}

void ac::FloatFade(cv::Mat &frame) {
    static float start[3] = {0}, goal[3] = {0};
    static int init = 0, speed = 1.0;
    if(init == 0) {
        for(int j = 0; j < 3; ++j) {
            start[j] = rand()%255;
            goal[j] = rand()%255;
        }
        init = 1;
    }
    for(int j = 0; j < 3; ++j) {
        if(start[j] > goal[j]) {
            start[j] += speed;
        } else if(start[j] < goal[j]) {
            start[j] -= speed;
        } else {
            goal[j] = rand()%255;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) + (0.8*(pixel[j]+start[j])));
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShift(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.rows/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                for(int i = off; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                    ++pos;
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShift8(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.rows/8;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                for(int i = off; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                    ++pos;
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShift16(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.rows/16;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                for(int i = off; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                    ++pos;
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::FloatFadeRandomIncrease(cv::Mat &frame) {
    static float start[3] = {0}, goal[3] = {0};
    static int init = 0, speed = 1.0;
    if(init == 0) {
        for(int j = 0; j < 3; ++j) {
            start[j] = rand()%255;
            goal[j] = rand()%255;
        }
        init = 1;
    }
    
    speed = (rand()%10) * 0.3;
    
    for(int j = 0; j < 3; ++j) {
        if(start[j] > goal[j]) {
            start[j] += speed;
        } else if(start[j] < goal[j]) {
            start[j] -= speed;
        } else {
            goal[j] = rand()%255;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) + (0.8*(pixel[j]+start[j])));
            }
        }
    }
    AddInvert(frame);
}

void ac::FloatFadeRGB(cv::Mat &frame) {
    static float start[3] = {0}, goal[3] = {0};
    static int init = 0, speed = 2.0;
    static int offset = rand()%2;
    if(init == 0) {
        for(int j = 0; j < 3; ++j) {
            start[j] = rand()%255;
            goal[j] = rand()%255;
        }
        init = 1;
    }
    if(start[offset] > goal[offset]) {
        start[offset] += speed;
    } else if(start[offset] < goal[offset]) {
        start[offset] -= speed;
    } else {
        goal[offset] = rand()%255;
        ++offset;
        if(offset > 2)
            offset = 0;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) + (0.8*(pixel[j]+start[j])));
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShiftDir(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.rows/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                if(on == 0) {
                    for(int i = off; i < frame.cols; ++i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        ++pos;
                        pixel = pix;
                    }
                } else {
                    pos = frame.cols-1;
                    for(int i = frame.cols-1-off; i > 1; --i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        --pos;
                        pixel = pix;
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareShiftExpand(cv::Mat &frame) {
    static int off = 0;
    static int num_squares = 2;
    int frame_height = frame.rows/num_squares;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            for(int z = row; z < (row+frame_height) && (z < frame.rows); ++z) {
                int pos = 0;
                if(on == 0) {
                    for(int i = off; i < frame.cols; ++i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        ++pos;
                        pixel = pix;
                    }
                } else {
                    pos = frame.cols-1;
                    for(int i = frame.cols-1-off; i > 1; --i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        --pos;
                        pixel = pix;
                    }
                }
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++num_squares;
        if(num_squares >= 32) {
            dir = 0;
        }
    } else {
        --num_squares;
        if(num_squares <= 2) {
            dir = 1;
        }
    }
}


