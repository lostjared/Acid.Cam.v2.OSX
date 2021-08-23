
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

void ac::FloatFadeVertical(cv::Mat &frame) {
    static float index[3] {-1, -1, -1};
    static float speed[3] = {0.1, 0.2, 0.3};
    static float dir[3] = {1,1,1};
    if(index[0] == -1 && index[1] == -1 && index[2] == -1) {
        index[0] = rand()%255;
        index[1] = rand()%255;
        index[2] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) +  (0.8 * (pixel[j]+index[j])));
            }
        }
        for(int j = 0; j < 3; ++j) {
            if(dir[j] == 1) {
                if(index[j] > 255) {
                    dir[j]  = 0;
                } else {
                    index[j] += speed[j];
                }
            }
            else {
                if(index[j] <= 1) {
                    dir[j] = 1;
                } else {
                    index[j] -= speed[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::LineTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        bool on = true;
        int r = rand()%20;
        int in = 0;
        for(int i = 0; i < frame.cols; ++i) {
            ++in;
            if(in > r) {
                on = !on;
                r = rand()%20;
                in = 0;
            }
            if(on == true) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    ++index;
    if(index > collection.size()-1)
        index = 0;
    AddInvert(frame);
}

void ac::SquareShiftDirVertical(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.cols/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.cols; row += frame_height) {
        off = rand()%frame.rows/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            for(int i = row; i < (row+frame_height) && (i < frame.cols); ++i) {
                int pos = 0;
                if(on == 0) {
                    for(int z = off; z < frame.rows; ++z) {
                        if(z < frame.rows && i < frame.cols) {
                            cv::Vec3b &pixel = pixelAt(frame,z, i);
                            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                            ++pos;
                            pixel = pix;
                        }
                    }
                } else {
                    pos = frame.cols-1;
                    for(int z = frame.rows-1-off; z > 1; --z) {
                        if(i < frame.cols && z < frame.rows) {
                            cv::Vec3b &pixel = pixelAt(frame,z, i);
                            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                            --pos;
                            pixel = pix;
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::StretchLineRow(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFX(frame.cols, i, stretch_x);
            if(nw >= 0 && nw < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, nw);
                pixel = pix;
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%12;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%12;
        }
    }
    AddInvert(frame);
}

void ac::StretchLineCol(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.rows;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFZ(frame.rows, z, stretch_x);
            if(nw >= 0 && nw < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(nw, i);
                pixel = pix;
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%12;
            if(stretch_x < frame.rows)
                stretch_x = frame.rows;
        } else {
            stretch_x += rand()%12;
        }
    }
    AddInvert(frame);
}

namespace ac {
    int max_stretch_ = 20;
}

void ac::setMaxStretch(int max) {
    max_stretch_ = max;
}

int  ac::getMaxStretch() {
    return max_stretch_;
}


void ac::StretchLineRowInc(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    static int max_x = 1;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                int nw = AC_GetFX(frame.cols, i, stretch_x);
                if(nw >= 0 && nw < frame.cols && i >= 0 && i < copy1.cols && z >= 0 &&  z < copy1.rows) {
                    cv::Vec3b pix = copy1.at<cv::Vec3b>(z, nw);
                    pixel = pix;
                }
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%max_x;
        }
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
}

void ac::StretchLineColInc(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.rows;
    static int max_x = 1;
    static int dir = 1;
    
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                int nw = AC_GetFZ(frame.rows, z, stretch_x);
                if(nw >= 0 && nw < frame.rows) {
                    cv::Vec3b pix = copy1.at<cv::Vec3b>(nw, i);
                    pixel = pix;
                }
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.rows)
                stretch_x = frame.rows;
        } else {
            stretch_x += rand()%max_x;
        }
    }
    AddInvert(frame);
    
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
}

void ac::StretchRowSplit(cv::Mat &frame) {
    int index = 0;
    std::vector<cv::Mat> values;

    for(int z = 0; z < frame.rows; ++z) {
        int spot = 10+rand()%100;
        int over = 0;
        if(index+spot < frame.cols) {
            cv::Mat mat;
            mat.create(frame.rows, index+spot, CV_8UC3);
            for(int zz = 0; zz < frame.rows; ++zz) {
                for(int ii = over; ii < index+spot; ++ii) {
                    if(ii+spot < frame.cols && ii >= 0 && ii < frame.cols && zz >= 0 && zz < frame.rows) {
                        cv::Vec3b pixel = pixelAt(frame,zz, ii+spot);
                        cv::Vec3b &pix = mat.at<cv::Vec3b>(zz, ii);
                        pix = pixel;
                    }
                }
            }
            over += index+spot;
            values.push_back(mat);
        }
    }
    int start_x = 0, start_y = 0;
    int pos = 0;
    
    for(int i = 0; i < static_cast<int>(values.size()); ++i) {
        int stretch = 10+rand()%400;
        for(int x = 0; x < values[i].cols; ++x) {
            start_y = 0;
            for(int y = 0; y < values[i].rows; ++y) {
                int nx = AC_GetFX(values[i].cols, pos, stretch);
                if(nx >= 0 && nx < values[i].cols && x >= 0 && x<values[i].cols && y >= 0 && y < values[i].rows) {
                    cv::Vec3b pix =  values[i].at<cv::Vec3b>(y, nx);
                    if(start_y >= 0 && start_y < frame.rows && start_x >= 0 && start_x < frame.cols) {
                        cv::Vec3b &pixel = pixelAt(frame,start_y, start_x);
                        start_y++;
                        pixel = pix;
                    }
                }
            }
            start_x++;
        }
    }
    AddInvert(frame);
}

void ac::VideoAddBlend(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel += pixelAt(reframe,z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoFadeInAndOut(cv::Mat &frame) {
    cv::Mat vframe;
    static double alpha = 1.0;
    static int dir = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
}

void ac::VideoFadeRGB(cv::Mat &frame) {
    static float rgb[3] = {-1,-1,-1};
    static int speed = 5;
    int rgb_index = 0;
    if(rgb[0] == -1 && rgb[1] == -1 && rgb[2] == -1) {
        rgb[0] = rand()%255;
        rgb[1] = rand()%255;
        rgb[2] = rand()%255;
    }
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reimage;
        ac_resize(vframe, reimage, frame.size());
        rgb[rgb_index] += speed;
        if(rgb[rgb_index] > 255) {
            rgb[rgb_index] = rand()%255;
            rgb_index = rand()%3;
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((rgb[j]+pixel[j])*0.5) + (0.5*pix[j]));
                }
            }
        }
        
    }
    AddInvert(frame);
}

void ac::VideoSubtract(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reimage;
        ac_resize(vframe, reimage, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned int>((0.5 * pixel[j]) - (0.3 * pix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::StretchLineRowLeftRight(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    static int max_x = 1;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFX(frame.cols, i, stretch_x);
            if(nw >= 0 && nw < frame.cols) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, nw);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%max_x;
        }
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
}

void ac::FadeInOutBlendRGB(cv::Mat &frame) {
    static float col[] = {-1,-1,-1};
    static float speed = 5;
    if(col[0] == -1 && col[1] == -1 && col[2] == -1) {
        col[0] = rand()%255;
        col[1] = rand()%255;
        col[2] = rand()%255;
    }
    for(int j = 0; j < 3; ++j) {
        if(col[j] < 255/2) {
            col[j] -= speed;
            if(col[j] <= 0)
                col[j] = rand()%255;
        } else {
            col[j] += speed;
            if(col[j] >= 255)
                col[j] = rand()%255;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) + (0.8 * (pixel[j]+col[j])));
            }
        }
    }
    AddInvert(frame);
}

void ac::FadeInOutReverseRGB(cv::Mat &frame) {
    static float col[] = {-1,-1,-1};
    static float stop[] = {-1, -1, -1};
    static float speed = 5;
    if(col[0] == -1 && col[1] == -1 && col[2] == -1) {
        col[0] = rand()%255;
        col[1] = rand()%255;
        col[2] = rand()%255;
        stop[0] = rand()%255;
        stop[1] = rand()%255;
        stop[2] = rand()%255;
    }
    for(int j = 0; j < 3; ++j) {
        if(col[j] < stop[j]) {
            col[j] -= speed;
            stop[j] -= speed;
            if(col[j] <= 0) {
                col[j] = rand()%255;
                stop[j] = rand()%255;
            }
        } else {
            col[j] += speed;
            stop[j] += speed;
            if(col[j] >= 255) {
                col[j] = rand()%255;
                stop[j] = rand()%255;
            }
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * (pixel[j]+stop[j])) + (0.5 * (pixel[j]+col[j])));
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelateRandom(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelateFillRandom(cv::Mat &frame) {
    static int f_w = 8, f_h = 8;
    for(int z = 0; z < frame.rows-1; z += f_h) {
        for(int i = 0; i < frame.cols-1; i += f_w) {
            cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
            for(int x = 0; x < f_w; ++x) {
                for(int y = 0; y < f_h; ++y) {
                    if(z+y >= 0 && i+x >= 0 && z+y < frame.rows-1 && i+x < frame.cols-1) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                        }
                    }
                }
            }
        }
    }
}

void ac::JaggedLine(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int space = 0;
    bool on = true;
    bool dir = true;
    static int offset = 0;
    static int max_value = 4;
    static int max_dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            int x = rand()%max_value;
            ++space;
            if(space > 25) {
                space = 0;
                on = !on;
                dir = !dir;
            }
            if(on == true) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(i+x < frame.cols && dir == true) {
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z, i+x);
                    pixel = pix;
                } else if(i-x > 0 && dir == false) {
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z, i-x);
                    pixel = pix;
                }
            }
        }
        ++offset;
        if(offset > (collection.size()-1))
            offset = 0;
    }
    AddInvert(frame);
    if(max_dir == 1) {
        max_value += 5;
        if(max_value >= 200)
            max_dir = 0;
    } else {
        max_value -= 5;
        if(max_value <= 5)
            max_dir = 1;
    }
}

void ac::ScratchyTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int offset = 0;
    for(int z = 0; z < frame.rows-2; ++z) {
        for(int i = 0; i < frame.cols-2; ++i) {
            if(rand()%25==0) {
                ++offset;
                if(offset > (collection.size()-1))
                    offset = 0;
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
    }
    AddInvert(frame);
}

void ac::ExpandPixelate(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int max = 5;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            int off_y = z+(rand()%max);
            if(off_y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(off_y, i);
                pixel = pix;
            }
            offset++;
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        max += 5;
        if(max > 150) {
            dir = 0;
        }
    } else {
        max -= 5;
        if(max <= 5)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::VideoRandom(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe, copy1 = frame.clone();
        ac_resize(vframe, reframe, frame.size());
        RandomFadeDelay(reframe);
        RandomSolo(copy1);
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoCollectionTwitch(cv::Mat &frame) {
    static MatrixCollection<32> collection, recollection;
    int offset = rand()%collection.size();
    int offsetx = rand()%recollection.size();
    if(collection.empty())
        collection.shiftFrames(frame);
    if(recollection.empty())
        collection.shiftFrames(frame);
    
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection.shiftFrames(frame);
        recollection.shiftFrames(reframe);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b cpx = collection.frames[offset].at<cv::Vec3b>(z, i);
                cv::Vec3b vpix = recollection.frames[offsetx].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((0.5*cpx[j]))+(0.5*vpix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::DiagSquare8(cv::Mat &frame) {
    static constexpr int SIZE=32;
    static MatrixCollection<SIZE> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int square = 8;
    for(int z = 0; z < frame.rows-1; z += square) {
        for(int i = 0; i < frame.cols-1; i += square) {
            for(int x = 0; x+i < frame.cols-1 && x < square; ++x) {
                for(int y = 0; z+y < frame.rows-1 && y < square; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
        }
        ++offset;
        if(offset > (collection.size()-1))
            offset = 0;
    }
    AddInvert(frame);
    
}

void ac::DiagSquareRandom(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    for(int z = 0; z < frame.rows; z += 32) {
        for(int i = 0; i < frame.cols; i += 32) {
            offset = rand()%collection.size();
            for(int x = 0; x+i < frame.cols-1 && x < 32; ++x) {
                for(int y = 0; z+y < frame.rows-1 && y < 32; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::DiagSquareX(cv::Mat &frame) {
    static constexpr int SIZE=32;
    static MatrixCollection<SIZE> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int square = 4;
    for(int i = 0; i < frame.cols; i += square) {
        for(int z = 0; z < frame.rows; z += square) {
            for(int x = 0; x+i < frame.cols-1 && x < square; ++x) {
                for(int y = 0; z+y < frame.rows-1 && y < square; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            if((rand()%8)==0)
                ++offset;
            
            if(offset > (collection.size()-1))
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::SlitScanDir(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRow480pX2(copy1);
    IntertwineCols640(copy2);
    AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    AddInvert(frame);
}

void ac::VideoScanline(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        bool on = true;
        static int start_x = 0, stop_x = rand()%50;
        for(int z = 0; z < frame.rows-2; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                ++start_x;
                if(start_x > stop_x) {
                    on = !on;
                    start_x = 0;
                    stop_x = rand()%50;
                }
                if(on) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pixel[j]^pix[j];
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoSquareRandom(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<32> collection;
        collection.shiftFrames(frame);
        collection.shiftFrames(reframe);
        static int start_off = 0;
        int offset = start_off;
        start_off = (start_off == 0) ? 1 : 0;
        for(int z = 0; z < frame.rows; z += 32) {
            for(int i = 0; i < frame.cols; i += 32) {
                for(int x = 0; x+i < frame.cols-1 && x < 32; ++x) {
                    for(int y = 0; z+y < frame.rows-1 && y < 32; ++y) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                        pixel = pix;
                    }
                }
                ++offset;
                if(offset > (collection.size()-1))
                    offset = 0;
            }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;

        }
        AddInvert(frame);
    }
}

void ac::VideoScanlineBlend(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        bool on = true;
        static int start_x = 0, stop_x = rand()%50;
        for(int z = 0; z < frame.rows-2; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                ++start_x;
                if(start_x > stop_x) {
                    on = !on;
                    start_x = 0;
                    stop_x = rand()%50;
                }
                if(on) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoSplitColumn(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static int off_x = 1;
        static int max_x = 300;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = off_x; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                pixel = pix;
            }
            ++off_x;
            if(off_x > max_x) {
                off_x = rand()%max_x;
                max_x += 100;
                if(max_x > frame.cols)
                    max_x = 300;
            }
        }
        AddInvert(frame);
    }
}

void ac::VideoXStatic(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if((rand()%2)==0) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = pixelAt(reframe,z, i);
                    pixel = pix;
                }
            }
        }
        AddInvert(frame);
    }
}

void ac::ScanlineCollection(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    bool on = true;
    static int start_x = 0, stop_x = rand()%50;
    static int offset = 0;
    for(int z = 0; z < frame.rows-2; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z, i);
            ++start_x;
            if(start_x > stop_x) {
                on = !on;
                start_x = 0;
                stop_x = rand()%50;
            }
            if(on) {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
        ++offset;
        if(offset > (collection.size()-1))
            offset = 0;
    }
    AddInvert(frame);
}

void ac::SquareShiftDirRGB(cv::Mat &frame) {
    static int off = 0, rgb = 0;
    static int frame_height = frame.rows/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.rows; row += frame_height) {
        off = rand()%frame.cols/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            for(int z = row; z < (row+frame_height) && (z < frame.rows-1); ++z) {
                int pos = 0;
                if(on == 0) {
                    for(int i = off; i < frame.cols; ++i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        ++pos;
                        pixel[rgb] = pix[rgb];
                    }
                } else {
                    pos = frame.cols-1;
                    for(int i = frame.cols-1-off; i > 1; --i) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                        --pos;
                        pixel[rgb] = pix[rgb];
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

void ac::StretchLineRowIncRGB(cv::Mat &frame) {
    static int rgb = 0;
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    static int max_x = 1;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFX(frame.cols, i, stretch_x);
            if(nw >= 0 && nw < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, nw);
                pixel[rgb] = pix[rgb];
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%max_x;
        }
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
    
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::StretchLineColIncRGB(cv::Mat &frame) {
    static int rgb = 0;
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.rows;
    static int max_x = 1;
    static int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFZ(frame.rows, z, stretch_x);
            if(nw >= 0 && nw < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(nw, i);
                pixel[rgb] = pix[rgb];
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.rows)
                stretch_x = frame.rows;
        } else {
            stretch_x += rand()%max_x;
        }
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
    ++rgb;
    if(rgb > 2)
        rgb = 0;
}

void ac::StretchLineRowIncSource(cv::Mat &frame) {
    static int rgb = 0;
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    static int max_x = 1;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFX(frame.cols, i, stretch_x);
            if(nw >= 0 && nw < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, nw);
                pixel[rgb] = pix[rgb];
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%max_x;
        }
        
        ++rgb;
        if(rgb > 2)
            rgb = 0;
        
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
}

void ac::StretchLineColIncSource(cv::Mat &frame) {
    static int rgb = 0;
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.rows;
    static int max_x = 1;
    static int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int nw = AC_GetFZ(frame.rows, z, stretch_x);
            if(nw >= 0 && nw < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(nw, i);
                pixel[rgb] = pix[rgb];
            }
        }
        if(rand()%2==0) {
            stretch_x -= rand()%max_x;
            if(stretch_x < frame.rows)
                stretch_x = frame.rows;
        } else {
            stretch_x += rand()%max_x;
        }
        ++rgb;
        if(rgb > 2)
            rgb = 0;
    }
    AddInvert(frame);
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > static_cast<int>(ac::fps/4)) {
        frame_count = 0;
        if(dir == 1) {
            ++max_x;
            if(max_x > max_stretch_)
                dir = 0;
        } else {
            --max_x;
            if(max_x <= 1)
                dir = 1;
        }
    }
}
