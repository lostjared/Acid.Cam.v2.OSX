

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

void ac::ShiftLinesDown(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int index = 0;
    static int counter = 0;
    static int counter_max = 32;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[index], z, i);
            pixel = pix;
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            if(rand()%50==0) {
                counter_max += 10;
                if(counter_max > frame.rows/4)
                    counter_max = 32;
            }
            ++index;
            if(index > collection.size()-1) {
                index = 0;
            }
        }
    }
    AddInvert(frame);
}

void ac::PictureStretch(cv::Mat &frame) {
    
    int index = 0;
    cv::Mat copy1 = frame.clone();
    int offset = 50+rand()%150;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
                index = 1;
            } else {
                value_offset = frame.cols-offset;
                index = 0;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::PictureStretchPieces(cv::Mat &frame) {
    static int counter = 0;
    static int index = 0;
    static int counter_max = 2;
    int offset = 50+rand()%100;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
            } else {
                value_offset = frame.cols-offset;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                counter_max = 2;
                index = (index == 0) ? 1 : 0;
            }
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            counter_max = 2;
            index = (index == 0) ? 1 : 0;
        }
    }
    AddInvert(frame);
}

void ac::VisualSnow(cv::Mat &frame) {
    static int max = 2;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = rand()%max;
            int ry = rand()%max;
            if(index == 0) {
                rx = -rx;
                ry = -ry;
            }
            int offset_x = frame.cols+rx;
            int offset_y = frame.rows+ry;
            int col_x = AC_GetFX(frame.cols, i, offset_x);
            int col_y = AC_GetFZ(frame.rows, z, offset_y);
            if(col_x >= 0 && col_x < frame.cols && col_y >= 0 && col_y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy1, col_y, col_x);
                //pixel = pix;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    index = (index == 0) ? 1 : 0;
    static int dir = 1;
    if(dir == 1) {
        max += 10;
        if(max > frame.cols/6) {
            dir = 0;
        }
    } else {
        max -= 10;
        if(max <= 10) {
            max = 10;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::VisualSnowX2(cv::Mat &frame) {
    static int max = 2;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = rand()%max;
            int ry = rand()%max;
            if(index == 0) {
                rx = -rx;
            } else {
                ry = -ry;
            }
            int offset_x = frame.cols+rx;
            int offset_y = frame.rows+ry;
            int col_x = AC_GetFX(frame.cols, i, offset_x);
            int col_y = AC_GetFZ(frame.rows, z, offset_y);
            if(col_x >= 0 && col_x < frame.cols && col_y >= 0 && col_y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy1, col_y, col_x);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    index = (index == 0) ? 1 : 0;
    static int dir = 1;
    if(dir == 1) {
        max += 50;
        if(max > frame.cols/4) {
            dir = 0;
        }
    } else {
        max -= 50;
        if(max <= 10) {
            max = 10;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::LineGlitch(cv::Mat &frame) {
    static int length = 1;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
            ++counter;
            if(counter > length) {
                counter = 0;
                ++offset;
                if(offset > collection.size()-1)
                    offset = 0;
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        length += 10;
        if(length > 400)
            dir = 0;
    } else {
        length -= 10;
        if(length <= 10)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::SlitReverse64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
        if(dir == 1) {
            ++offset;
            if(offset > (collection.size()-1))
                dir = rand()%2;
        } else {
            --offset;
            if(offset <= 1)
                dir = rand()%2;
        }
    }
    AddInvert(frame);
}

void ac::SlitReverse64_Increase(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int dir = 1;
    static int max = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = rand()%2;
                static int m_dir = 1;
                if(m_dir == 1) {
                    ++max;
                    if(max > (collection.size()-1))
                        m_dir = 0;
                } else {
                    --max;
                    if(max <= 1)
                        m_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = rand()%2;
            
        }
    }
    AddInvert(frame);
}

void ac::SlitStretch(cv::Mat &frame) {
    static int counter = 0;
    static int index = 0;
    static int counter_max = 2;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int m_index = 0;
    int offset = 50+rand()%100;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
            } else {
                value_offset = frame.cols-offset;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[m_index], z, col);
                pixel = pix;
            }
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                counter_max = 2;
                index = (index == 0) ? 1 : 0;
                m_index++;
                if(m_index > (collection.size()-1))
                    m_index = 0;
            }
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            counter_max = 2;
            index = (index == 0) ? 1 : 0;
            m_index++;
            if(m_index > (collection.size()-1))
                m_index = 0;
        }
    }
    AddInvert(frame);
}
