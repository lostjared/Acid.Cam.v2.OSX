

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


void ac::ParticleSlide(cv::Mat &frame) {
    static Point **position = 0;
    static cv::Size sz = frame.size();
    if(position == 0 || sz != frame.size()) {
        if(position != 0) {
            for(int i = 0; i < sz.width; ++i) {
                delete [] position[i];
            }
            delete [] position;
            position = 0;
        }
        position = new Point*[frame.cols];
        for(int i = 0; i < frame.cols; ++i) {
            position[i] = new Point[frame.rows];
        }
        for(int i = 0; i < frame.cols; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                position[i][z].setPoint(i, z);
            }
        }
        sz = frame.size();
    }
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            Point &p = position[i][z];
            if(p.x >= 0 && p.x < frame.cols && p.y >= 0 && p.y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[offset], p.y, p.x);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
                if((rand()%10)==0) offset ++;
                if(offset > (collection.size()-1))
                    offset = 0;
                
            }
            switch((rand()%5)) {
                case 0:
                    p.x += rand()%100;
                    break;
                case 1:
                    p.x -= rand()%100;
                    break;
                case 2:
                    p.y += rand()%100;
                    break;
                case 3:
                    p.y -= rand()%100;
                    break;
            }
            
            if(p.x > frame.cols-1)
                p.x = 0;
            if(p.x <= 1)
                p.x = frame.cols;
            if(p.y <= 1)
                p.x = frame.cols-1;
            if(p.y > frame.rows-1)
                p.y = 0;
        }
    }
    AddInvert(frame);
}

void ac::DiagPixelated(cv::Mat &frame) {
    static int square_w = 16, square_h = 16;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; z += square_h) {
        for(int i = 0; i < frame.cols; i += square_w) {
            for(int x = 0; x < square_w; ++x) {
                for(int y = 0; y < square_h; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame, z+y, i+x);
                    cv::Vec3b pix = pixelAt(copy1,z,i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::DiagPixelatedResize(cv::Mat &frame) {
    static int square_w = 16, square_h = 16;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; z += square_h) {
        for(int i = 0; i < frame.cols; i += square_w) {
            for(int x = 0; x < square_w; ++x) {
                for(int y = 0; y < square_h; ++y) {
                    if(i >= 0 && z >= 0 && z+y > 0 && i+x > 0 && z+y < frame.rows && i+x < frame.cols && i < frame.cols && z < frame.rows) {
                        cv::Vec3b &pixel = pixelAt(frame, z+y, i+x);
                        cv::Vec3b pix = pixelAt(copy1,z,i);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        square_w ++;
        if(square_w > 63)
            dir = 0;
    } else {
        square_w --;;
        if(square_w < 2)
            dir = 1;
    }
    static int dir1 = 1;
    if(dir1 == 1) {
        square_h ++;
        if(square_h > 63)
            dir1 = 0;
    } else {
        square_h --;
        if(square_h < 2)
            dir1 = 1;
    }
}

void ac::DiagPixelRGB_Collection(cv::Mat &frame) {
   
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z,i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
            static int dir = 1;
            if(dir == 1) {
                ++offset;
                if(offset > (collection.size()-1))
                    dir = 0;
            } else {
                --offset;
                if(offset <= 1)
                    dir = 1;
            }
        }
    }
    RGBSplitFilter(frame);
    AddInvert(frame);
}

void ac::RGBShiftTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int off[3] = {0,1,2};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix[3];
            pix[0] = pixelAt(collection.frames[0], z, i);
            pix[1] = pixelAt(collection.frames[7], z, i);
            pix[2] = pixelAt(collection.frames[15], z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pix[off[j]][j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        off[j]++;
        if(off[j] > 2)
            off[j] = 0;
    }
    AddInvert(frame);
}

void ac::PictureShiftDown(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFZ(frame.rows, z, new_row);
            if(row >= 0 && row < frame.rows) {
                cv::Vec3b pix = pixelAt(copy, row, i);
                pixel = pix;
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.rows)
        new_row = frame.rows+(640+rand()%frame.rows);
    AddInvert(frame);
}

void ac::PictureShiftRight(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFX(frame.cols, i, new_row);
            if(row >= 0 && row < frame.cols) {
                cv::Vec3b pix = pixelAt(copy, z, row);
                pixel = pix;
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.cols)
        new_row = frame.cols+(640+rand()%frame.cols);
    AddInvert(frame);
}

void ac::PictureShiftDownRight(cv::Mat &frame) {
    PictureShiftDown(frame);
    PictureShiftRight(frame);
    AddInvert(frame);
}

void ac::FlipPictureShift(cv::Mat &frame) {
    static int counter = 0;
    static int index = 0;
    ++counter;
    if(counter > static_cast<int>(ac::fps/2)) {
        index = (index == 0) ? 1 : 0;
        counter = 0;
    }
    
    if(index == 0)
        PictureShiftRight(frame);
    else
        PictureShiftDown(frame);
    
    AddInvert(frame);
}

void ac::FlipPictureRandomMirror(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorRandomNow(copy1);
    FlipPictureShift(copy1);
    MirrorRandomNow(copy2);
    AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    AddInvert(frame);
}

void ac::PictureShiftVariable(cv::Mat &frame) {
    static int new_row = 0;
    static int new_col = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFZ(frame.rows, z, new_row);
            int col = AC_GetFX(frame.cols, i, new_col);
            if(row >= 0 && row < frame.rows && col >= 0 && col < frame.cols) {
                cv::Vec3b pix = pixelAt(copy, row, col);
                pixel = pix;
            }
        }
    }
    new_row -= rand()%100;
    new_col -= rand()%25;
    
    if(new_col <= frame.cols)
        new_col = frame.cols+(640+rand()%frame.cols);
    
    if(new_row <= frame.rows)
        new_row = frame.rows+(640+rand()%frame.rows);
    AddInvert(frame);

}

void ac::RGBWideTrails(cv::Mat &frame) {
    static MatrixCollection<17> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b pix[8];
            for(int q = 0; q < 8; ++q) {
                pix[q] = pixelAt(collection.frames[q*2], z, i);
            }
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix1, pix2;
            for(int j = 0; j < 3; ++j) {
                pix1[j] = pix[j][j];
                pix2[j] = pix[3+j][j];
                pixel[j] = static_cast<unsigned char>((0.5 * pix1[j]) + (0.5 * pix2[j]));
            }
        }
    }
    AddInvert(frame);
}

void ac::StretchR_Right(cv::Mat &frame) {
    
}
