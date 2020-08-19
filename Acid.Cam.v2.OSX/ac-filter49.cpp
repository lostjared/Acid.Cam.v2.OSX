

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
                    if(z >= 0 && z+y < frame.rows-1 && i+x < frame.cols-1 && i >= 0) {
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
}

void ac::DiagPixelatedResize(cv::Mat &frame) {
    static int square_w = 16, square_h = 16;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; z += square_h) {
        for(int i = 0; i < frame.cols; i += square_w) {
            for(int x = 0; x < square_w; ++x) {
                for(int y = 0; y < square_h; ++y) {
                    if(i >= 0 && z >= 0 && z+y > 0 && i+x > 0 && z+y < frame.rows-1 && i+x < frame.cols-1 && i < frame.cols-1 && z < frame.rows-1) {
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
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFX(frame.cols, i, new_row);
            if(row >= 0 && row < frame.cols) {
                cv::Vec3b pix = pixelAt(copy, z, row);
                pixel[2] = pix[2];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.cols)
        new_row = frame.cols+(640+rand()%frame.cols);
    AddInvert(frame);
}

void ac::StretchG_Right(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFX(frame.cols, i, new_row);
            if(row >= 0 && row < frame.cols) {
                cv::Vec3b pix = pixelAt(copy, z, row);
                pixel[1] = pix[1];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.cols)
        new_row = frame.cols+(640+rand()%frame.cols);
    AddInvert(frame);
}

void ac::StretchB_Right(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFX(frame.cols, i, new_row);
            if(row >= 0 && row < frame.cols) {
                cv::Vec3b pix = pixelAt(copy, z, row);
                pixel[0] = pix[0];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.cols)
        new_row = frame.cols+(640+rand()%frame.cols);
    AddInvert(frame);
}

void ac::StretchR_Down(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFZ(frame.rows, z, new_row);
            if(row >= 0 && row < frame.rows) {
                cv::Vec3b pix = pixelAt(copy, row, i);
                pixel[2] = pix[2];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.rows)
        new_row = frame.rows+(640+rand()%frame.rows);
    AddInvert(frame);
}
void ac::StretchG_Down(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFZ(frame.rows, z, new_row);
            if(row >= 0 && row < frame.rows) {
                cv::Vec3b pix = pixelAt(copy, row, i);
                pixel[1] = pix[1];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.rows)
        new_row = frame.rows+(640+rand()%frame.rows);
    AddInvert(frame);
}
void ac::StretchB_Down(cv::Mat &frame) {
    static int new_row = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int row = AC_GetFZ(frame.rows, z, new_row);
            if(row >= 0 && row < frame.rows) {
                cv::Vec3b pix = pixelAt(copy, row, i);
                pixel[0] = pix[0];
            }
        }
    }
    new_row -= 100;
    if(new_row <= frame.rows)
        new_row = frame.rows+(640+rand()%frame.rows);
    AddInvert(frame);
}

void ac::Distorted_LinesY(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int rand_dir = rand()%2;
    for(int i = 16; i < frame.cols; ++i) {
        static int counter = 0;
        ++counter;
        if(counter > 4) {
            rand_dir = rand()%2;
            counter = 0;
        }
        for(int z = 0; z < frame.rows; ++z) {
            int offset = (rand_dir == 1) ? i : i-16;
            int rand_len = rand()%16;
            for(int q = 0; q < rand_len; ++q) {
                if(z >= 0 && z < frame.rows-1 && i >= 0 && i < frame.cols-1 && offset+q < frame.cols-1) {
                    cv::Vec3b &pixel = pixelAt(frame, z, i);
                    cv::Vec3b pix = pixelAt(copy1, z, offset+q);
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}


void ac::Distorted_LinesX(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int rand_dir = rand()%2;
    for(int z = 16; z < frame.rows; ++z) {
        static int counter = 0;
        ++counter;
        if(counter > 4) {
            rand_dir = rand()%2;
            counter = 0;
        }
        for(int i = 0; i < frame.cols; ++i) {
            int offset = (rand_dir == 1) ? z : z-16;
            int rand_len = rand()%16;
            for(int q = 0; q < rand_len; ++q) {
                if(q >= 0 && q < frame.cols-1 && z >= 0 && z < frame.rows-1 && i >= 0 && i < frame.cols-1 && offset+q < frame.rows-1) {
                    cv::Vec3b &pixel = pixelAt(frame, z, i);
                    cv::Vec3b pix = pixelAt(copy1, offset+q, i);
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}




void ac::TripHSV(cv::Mat &frame) {
    cv::Mat copy1;
    static unsigned char ch = 100;
    cv::cvtColor(frame, copy1, cv::COLOR_BGR2HSV);
    cv::Mat channels[3];
    cv::split(copy1, channels);
    static int dir = 1;
    if(dir == 1) {
        ++ch;
        if(ch >= 254)
            dir = 0;
    } else {
        --ch;
        if(ch <= 100)
            dir = 1;
    }
    channels[2] = ch;
    cv::merge(channels, 3, frame);
    cv::cvtColor(frame, frame, cv::COLOR_HSV2BGR);
    AddInvert(frame);
}

void ac::Diag_Line_InOut(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int LINE_SIZE=16;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; i += LINE_SIZE) {
            for(int j = 0; j < LINE_SIZE; ++j) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows && i+j < frame.cols && z+j < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, i+j);
                    cv::Vec3b pix = pixelAt(copy1, z+j, i);
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
    if(dir == 1) {
        ++LINE_SIZE;
        if(LINE_SIZE > 128)
            dir = 0;
    } else {
        --LINE_SIZE;
        if(LINE_SIZE <= 1)
            dir = 1;
    }
}

void ac::Histogram(cv::Mat &frame) {
    Histogram_Type hst;
    cv::Mat copy = hst.createGraph(frame);
    frame = copy.clone();
    AddInvert(frame);
}

void ac::ImageHistogramLookup(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat image;
    ac_resize(blend_image, image, frame.size());
    Histogram_Type hst;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = hst.createHistogram(image);
    cv::LUT(copy1, hst.b_hist, frame);
    AddInvert(frame);
}

void ac::MovementRange_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MovementRange_SubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                int value1 = pixel[0]+pixel[1]+pixel[2];
                int value2 = pix[0]+pix[1]+pix[2];
                if(abs(value1-value2) > getPixelCollection()) {
                    pixel = pix1;
                    break;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::XorSumStrobe(cv::Mat &frame) {
    cv::Scalar values(cv::sum(frame));
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z , i);
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^val;
            }
        }
    }
    AddInvert(frame);
}


int threshold_value = 15;

void ac::setThreshold(int t) {
    threshold_value = t;
}

void ac::DetectEdges(cv::Mat &frame) {
    cv::Mat result;
    cv::morphologyEx(frame, result, cv::MORPH_GRADIENT, cv::Mat());
    cv::threshold(result, frame, threshold_value, 255, cv::THRESH_BINARY);
    AddInvert(frame);
}

void ac::SobelNorm(cv::Mat &frame) {
    cv::Mat sobel_x, sobel_y, sobel;
    cv::Sobel(frame, sobel_x, CV_16S,1,0);
    cv::Sobel(frame, sobel_y, CV_16S,0,1);
    sobel = abs(sobel_x)+abs(sobel_y);
    double min, max;
    cv::minMaxLoc(sobel, &min, &max);
    sobel.convertTo(frame, CV_8UC3, -255.0/max,255);
    AddInvert(frame);
}

void ac::SobelThreshold(cv::Mat &frame) {
    cv::Mat sobel_x, sobel_y, sobel;
    cv::Sobel(frame, sobel_x, CV_16S,1,0);
    cv::Sobel(frame, sobel_y, CV_16S,0,1);
    sobel = abs(sobel_x)+abs(sobel_y);
    double smin, smax;
    cv::minMaxLoc(sobel, &smin, &smax);
    cv::Mat sobel_i;
    sobel.convertTo(sobel_i, CV_8U, -255./smax,255);
    cv::threshold(sobel_i, frame, threshold_value, 255, cv::THRESH_BINARY);
    AddInvert(frame);
}

void ac::EdgeFill_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "EdgeFill_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    DetectEdges(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] >= 225 && pixel[0] <= 255 && pixel[1] >= 225 && pixel[1] <= 255 && pixel[2] >= 225 && pixel[2] <= 255) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::FillWhite_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillWhite_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] >= 225 && pixel[0] <= 255 && pixel[1] >= 225 && pixel[1] <= 255 && pixel[2] >= 225 && pixel[2] <= 255) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}
void ac::FillBlack_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillBlack_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] >= 0 && pixel[0] <= 25 && pixel[1] >= 0 && pixel[1] <= 25 && pixel[2] >= 0 && pixel[2] <= 25) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::FillPink_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillPink_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] >= 225 && pixel[0] <= 255 && pixel[1] >= 0 && pixel[1] <= 25 && pixel[2] >= 225 && pixel[2] <= 255) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::FillGreen_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillGreen_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] >= 0 && pixel[0] <= 25 && pixel[1] >= 225 && pixel[1] <= 255 && pixel[2] >= 0 && pixel[2] <= 25) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

// Slow
void ac::MedianBlurHigherLevel(cv::Mat &frame) {
#ifdef _WIN32
    cv::Mat copy = frame.clone();
    cv::medianBlur(copy, frame, 15);
#else
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::medianBlur(blur, out, 15);
    out.copyTo(frame);
#endif
}

void ac::FillNonWhite_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillNonWhite_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(!(pixel[0] >= 225 && pixel[0] <= 255 && pixel[1] >= 225 && pixel[1] <= 255 && pixel[2] >= 225 && pixel[2] <= 255)) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::FillNonBlack_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillNonBlack_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(!(pixel[0] >= 0 && pixel[0] <= 25 && pixel[1] >= 0 && pixel[1] <= 25 && pixel[2] >= 0 && pixel[2] <= 25)) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::FillNonPink_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FillNonPink_SubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(!(pixel[0] >= 225 && pixel[0] <= 255 && pixel[1] >= 0 && pixel[1] <= 25 && pixel[2] >= 225 && pixel[2] <= 255)) {
                cv::Vec3b pix;
                pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::LineInLineOut(cv::Mat &frame) {
    static int offset = 0, index = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > 100)
                dir = 0;
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_Increase(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut2_Increase(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 5;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 250)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 5)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        static int idir = 1;
        if(idir == 1) {
            ++index;
            if(index > collection.size()-2)
                idir = 0;
        } else {
            --index;
            if(index <= 1)
                idir = 1;
        }
    }
    AddInvert(frame);
}

void ac::LineInLineOut3_Increase(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 5;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
            static int idir = 1;
            if(idir == 1) {
                ++index;
                if(index > collection.size()-2)
                    idir = 0;
            } else {
                --index;
                if(index <= 1)
                    idir = 1;
            }
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            offset += rand()%10;
            if(offset > max) {
                offset = max;
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 250)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 5)
                        max_dir = 1;
                }
            }
        } else {
            offset -= rand()%10;
            if(offset <= 1) {
                dir = 1;
                offset = 1;
            }
        }
    }
    AddInvert(frame);
}

