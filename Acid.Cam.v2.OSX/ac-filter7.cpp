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


#include "ac.h"


void ac::SlideUpDown(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
                }
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideUpDownXor(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] ^= static_cast<unsigned char>(pixel[j]*(alpha+1));
                }
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] ^= static_cast<unsigned char>(pixel[j]*(alpha+1));
                
            }
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideUpDownRandom(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    DrawFunction f1, f2;
    f1 = getRandomFilter();
    f2 = getRandomFilter();
    cv::Mat frames[2];
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    f1(frames[0]);
    f2(frames[1]);
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[0].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
                }
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            if(i >= 0 && i < frame.cols && z >= 0 & z < frame.rows) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frames[1].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
                
            }
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideSubFilter(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.cols-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 7.0;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlideSubFilter") return;
    cv::Mat frame_x;
    frame_x = frame.clone();
    if(ac::subfilter != -1) {
        CallFilter(subfilter, frame_x);
    } else return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < start_1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int i =(frame.cols-1); i > start_2; --i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.cols-1)) {
            direction_1 = 0;
            start_1 = (frame.cols-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.cols-1)) {
            direction_2 = 0;
            start_2 = (frame.cols-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::SlideSubUpDownFilter(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlideSubUpDownFilter") return;
    cv::Mat frame_x;
    frame_x = frame.clone();
    if(ac::subfilter != -1) {
        CallFilter(subfilter, frame_x);
    } else return;
    
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::BlendInAndOut(cv::Mat &frame) {
    static cv::Scalar color(rand()%255, rand()%255, rand()%255);
    static int step[3] = {1,1,1};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(color[j]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            color[j] += step[j];
            if(color[j] >= 255) {
                dir[j] = 0;
            }
            
        } else if(dir[j] == 0) {
            color[j] -= step[j];
            if(color[j] <= 0) {
                step[j] = 1+(rand()%10);
                dir[j] = 1;
                color[j] = rand()%255;
            }
        }
    }
}

void ac::BlendScaleInAndOut(cv::Mat &frame) {
    static cv::Scalar color(rand()%255, rand()%255, rand()%255);
    static double alpha = 1.0, alpha_max = 6.0;
    static int step[3] = {1,1,1};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(color[j]*alpha);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            color[j] += step[j];
            if(color[j] >= 255) {
                dir[j] = 0;
            }
        } else if(dir[j] == 0) {
            color[j] -= step[j];
            if(color[j] <= 0) {
                step[j] = 1+(rand()%10);
                dir[j] = 1;
                color[j] = rand()%255;
            }
        }
    }
    static int alpha_direction = 1;
    procPos(alpha_direction, alpha, alpha_max, 10, 0.09);
}
void ac::AcidGlitch(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 6.0;
    static int step[3] = {1,1,1};
    static cv::Scalar color(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix_copy = pixel;
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(((pix_copy[3-j-1] ^ static_cast<unsigned char>(color[j]))) * alpha);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            color[j] += step[j];
            if(color[j] >= 255) {
                dir[j] = 0;
            }
        } else if(dir[j] == 0) {
            color[j] -= step[j];
            if(color[j] <= 0) {
                step[j] = 1+(rand()%10);
                dir[j] = 1;
                color[j] = rand()%255;
            }
        }
    }
    static int alpha_direction = 1;
    procPos(alpha_direction, alpha, alpha_max, 10, 0.09);
}

void ac::XorBackwards(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 6.0;
    static cv::Mat frame_copy = frame.clone();
    cv::Mat orig = frame.clone();
    if(frame_copy.size()!=frame.size()) {
        frame_copy = frame.clone();
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                double val = 0;
                val = (pixel[j]^pix[3-j-1])*alpha;
                pixel[j] += static_cast<unsigned char>(val);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    frame_copy = orig;
}

void ac::LiquidFilter(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 7.0;
    static MatrixCollection<3> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b frame_pix[3];
            for(int j = 0; j < 3; ++j) {
                frame_pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                double value = 0;
                for(int q = 0; q < 3; ++q) {
                    value += frame_pix[j][q] * alpha;
                }
                pixel[j] = pixel[j]^static_cast<unsigned char>(value);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::MatrixXorAnd(cv::Mat &frame) {
    static MatrixCollection<3> collection;
    static double alpha = 1.0, alpha_max = 7.0;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b frame_pix[3];
            for(int j = 0; j < 3; ++j)
                frame_pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                static bool dir = true;
                double value = 0;
                for(int q = 0; q < 3; ++q) {
                    if(dir == true)
                        value += frame_pix[j][q] * alpha;
                    else
                        value += frame_pix[3-j-1][q] * alpha;
                    dir = (dir == true) ? false : true;
                }
                pixel[j] = (cv::saturate_cast<unsigned char>(pixel[j]&(static_cast<unsigned char>(value))))^static_cast<unsigned char>(alpha);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 8.0, 0.1);
}


void ac::XorAlpha(cv::Mat &frame) {
    static cv::Scalar color_value(rand()%255, rand()%255, rand()%255);
    static int index = 0;
    static double alpha = 1.0, alpha_max = 7.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(color_value[j]*alpha);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    if(color_value[index] >= 255) {
        color_value[index] = rand()%255;
        ++index;
        if(index > 2)
            index = 0;
    } else {
        color_value[index] += 5;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::AlphaAcidTrails(cv::Mat &frame) {
    XorAlpha(frame);
    MedianBlend(frame);
}

void ac::SelfXorAverage(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 7.0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[3];
            for(int j = 0; j < 3; ++j) {
                pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                double value = ((pix[0][j] ^ pix[1][j] ^ pix[2][j])/3) * alpha;
                pixel[j] ^= static_cast<unsigned char>(value) ^ static_cast<unsigned char>(alpha);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RandomXorBlend(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^pix[j]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::RGBVerticalXor(cv::Mat &frame) {
    static int pos[3] = {0, 0, 0};
    static cv::Size old_size;
    if(frame.size() != old_size) {
        pos[0] = pos[1] = pos[2] = 0;
        old_size = frame.size();
    }
    static int index = 0;
    static double alpha = 1.0, alpha_max = 6.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < pos[index]; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[3-index-1] ^= static_cast<unsigned char>(pixel[index]*alpha);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        for(int z = pos[index]; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] ^= static_cast<unsigned char>(pixel[index]*alpha);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    
    pos[index] += 100;
    
    if(pos[index] > frame.rows) {
        pos[index] = 0;
        ++index;
        if(index > 2)
            index = 0;
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RGBVerticalXorScale(cv::Mat &frame) {
    RGBVerticalXor(frame);
    BlendScaleInAndOut(frame);
}

void ac::RGBHorizontalXor(cv::Mat &frame) {
    static int pos[3] = {0, 0, 0};
    static cv::Size old_size;
    if(frame.size() != old_size) {
        pos[0] = pos[1] = pos[2] = 0;
        old_size = frame.size();
    }
    static int index = 0;
    static double alpha = 1.0, alpha_max = 6.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < pos[index]; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[3-index-1] ^= static_cast<unsigned char>(pixel[index]*alpha);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        for(int i = pos[index]; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] ^= static_cast<unsigned char>(pixel[index]*alpha);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    pos[index] += 100;
    if(pos[index] > frame.cols) {
        pos[index] = 0;
        ++index;
        if(index > 2)
            index = 0;
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RGBHorizontalXorScale(cv::Mat &frame) {
    RGBHorizontalXor(frame);
    BlendScaleInAndOut(frame);
}

void ac::FadeStrobe(cv::Mat &frame) {
    static cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    if(frames_released == true || reset_alpha == true) {
        colorval = cv::Scalar(rand()%255, rand()%255, rand()%255);
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^static_cast<unsigned char>(1+colorval[j]));
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    for(int j = 0; j < 3; ++j) {
        colorval[j] += 20;
        if(colorval[j] >= 255) {
            colorval[j] = 0;
        }
    }
}

void ac::RGBMirror(cv::Mat &frame) {
    cv::Mat f_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b f_pixel[4];
            f_pixel[0] = f_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            f_pixel[1] = f_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            f_pixel[2] = f_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^f_pixel[j][j]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::MirrorStrobe(cv::Mat &frame) {
    cv::Mat copy_f = frame.clone();
    static cv::Scalar value(rand()%255, rand()%255, rand()%255);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy_f.at<cv::Vec3b>(copy_f.rows-z-1, copy_f.cols-i-1);
            pixel[index] = cv::saturate_cast<unsigned char>((pixel[index]^pix[index])+value[index]);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    
    value[index] += 50;
    if(value[index] > 255)
        value[index] = 0;
    
    ++index;
    if(index > 2)
        index = 0;
}

void ac::AndStrobe(cv::Mat &frame) {
    cv::Vec3b colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]&colorval[j];
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::AndStrobeScale(cv::Mat &frame) {
    static double alpha = 1.0, pos_max = 7.0;
    cv::Vec3b colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]&colorval[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, pos_max);
}

void ac::AndPixelStrobe(cv::Mat &frame) {
    static cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] & static_cast<unsigned char>(colorval[j]);
                pixel[j] = pixel[j] ^ static_cast<unsigned char>(colorval[j]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    colorval[index] += 50;
    if(colorval[index] > 255) {
        colorval[index] = rand()%255;
        ++index;
        if(index > 2)
            index = 0;
    }
}

void ac::AndOrXorStrobe(cv::Mat &frame) {
    static int index = 0;
    cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = pixel[j] & static_cast<unsigned char>(colorval[j]);
                        break;
                    case 1:
                        pixel[j] = pixel[j] | static_cast<unsigned char>(colorval[j]);
                        break;
                    case 2:
                        pixel[j] = pixel[j] ^ static_cast<unsigned char>(colorval[j]);
                        break;
                }
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    
}

void ac::AndOrXorStrobeScale(cv::Mat &frame) {
    static int index = 0;
    static double alpha = 1.0, alpha_max = 7.0;
    cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = (pixel[j] & static_cast<unsigned char>(colorval[j]))*static_cast<unsigned char>(1+alpha);
                        break;
                    case 1:
                        pixel[j] = (pixel[j] | static_cast<unsigned char>(colorval[j]))*static_cast<unsigned char>(1+alpha);
                        break;
                    case 2:
                        pixel[j] = (pixel[j] ^ static_cast<unsigned char>(colorval[j]))*static_cast<unsigned char>(1+alpha);
                        break;
                }
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::FadeInAndOut(cv::Mat &frame) {
    static int speed[4] = {5+rand()%10, 5+rand()%10, 5+rand()%10};
    static cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    static int dir[4] = {rand()%2, rand()%2, rand()%2};
    if(frames_released == true || reset_alpha == true) {
        colorval = cv::Scalar(rand()%255, rand()%255, rand()%255);
        for(int j = 0; j < 3; ++j) {
            dir[j] = rand()%2;
            speed[j] = 5+rand()%10;
        }
    }
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= cv::saturate_cast<unsigned char>(colorval[j]);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            colorval[j] += speed[j];
            if(colorval[j] > 255) {
                colorval[j] = 255;
                dir[j] = 0;
                speed[j] = 5+rand()%10;
            }
        } else if(dir[j] == 0) {
            colorval[j] -= speed[j];
            if(colorval[j] <= 0) {
                colorval[j] = 0;
                dir[j] = 1;
                speed[j] = 5+rand()%10;
            }
        }
    }
}

void ac::BrightStrobe(cv::Mat &frame) {
    static int speed[3] = {1,6,13};
    static int dir[3] = {1,1,1};
    cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>((colorval[j]*speed[j]));
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            colorval[j] += speed[j];
            if(colorval[j] > 255) {
                dir[j] = 0;
            }
            ++speed[j];
            if(speed[j] > 25) {
                speed[j] = 25;
            }
        } else {
            colorval[j] -= speed[j];
            if(colorval[j] <= 0) {
                colorval[j] = speed[j]--;
                dir[j] = 1;
            }
            --speed[j];
            if(speed[j] <= 0) {
                speed[j] = 1;
            }
        }
    }
    
}

void ac::DarkStrobe(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 7.0;
    cv::Scalar colorval(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i ){
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] &= static_cast<unsigned char>((pixel[j]%(1+static_cast<unsigned char>(colorval[j])))*alpha);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RandomXorOpposite(cv::Mat &frame) {
    cv::Vec3b colorval(rand()%255, rand()%255, rand()%255);
    cv::Vec3b colorval2(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z){
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = ~pixel[j] ^ colorval[j] ^ ~colorval2[j];
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::StrobeTransform(cv::Mat &frame) {
    cv::Vec3b colorval(rand()%255, rand()%255, rand()%255);
    static double value = 25.0, alpha = 1.0, alpha_max = 7.0;
    static int dir = 1, speed_dir = 1;
    static int speed = 2, speed_increase = 5;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += ((pixel[j] + (static_cast<unsigned char>(value))) ^ static_cast<unsigned char>(alpha)) ^ colorval[j];
            }
        }
    }
    if(dir == 1)
        value += speed;
    else
        value -= speed;
    
    if(dir == 1) {
        if(value > (255)) {
            dir = 0;
            if(speed_dir == 1) {
                speed += speed_increase;
                if(speed > 25)
                    speed_dir = 0;
            } else if(speed_dir == 0) {
                speed -= speed_increase;
                if(speed <= 1)
                    speed_dir = 1;
            }
        }
    } else if(dir == 0) {
        if(value <= 25) {
            dir = 1;
        }
    }
    static int cdir = 1;
    procPos(cdir, alpha, alpha_max);
}

void ac::InitBlend(cv::Mat &frame) {
    static cv::Vec3b colorval(rand()%255, rand()%255, rand()%255);
    static double value = 25.0, alpha = 1.0, alpha_max = 7.0;
    static int dir = 1, speed_dir = 1;
    static int speed = 2, speed_increase = 5;
    if(reset_alpha == true || frames_released == true) {
        colorval = cv::Vec3b(rand()%255, rand()%255, rand()%255);
        value = 25.0;
        alpha = 1.0;
        alpha_max = 7.0;
        dir = 1;
        speed_dir = 1;
        speed = 2;
        speed_increase = 5;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += ((pixel[j] + (static_cast<unsigned char>(value))) ^ static_cast<unsigned char>(alpha)) ^ colorval[j];
            }
        }
    }
    if(dir == 1)
        value += speed;
    else
        value -= speed;
    if(dir == 1) {
        if(value > (255)) {
            dir = 0;
            if(speed_dir == 1) {
                speed += speed_increase;
                if(speed > 25)
                    speed_dir = 0;
            } else if(speed_dir == 0) {
                speed -= speed_increase;
                if(speed <= 1)
                    speed_dir = 1;
            }
        }
    } else if(dir == 0) {
        if(value <= 25) {
            dir = 1;
        }
    }
    static int cdir = 1;
    procPos(cdir, alpha, alpha_max);
}

