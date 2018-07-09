/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
            
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
            
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
    DrawFunction f1, f2;
	static int index[2];
    f1 = getRandomFilter(index[0]);
    f2 = getRandomFilter(index[1]);
    if(ac::draw_strings[index[0]] == "SlideUpDownRandom") return;
    if(ac::draw_strings[index[1]] == "SlideUpDownRandom") return;
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frames[0].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frames[1].at<cv::Vec3b>(z, i);
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

void ac::SlideSubFilter(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.cols-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 7.0;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlideSubFilter") return;
    cv::Mat frame_x;
    frame_x = frame.clone();
    if(ac::subfilter != -1) {
        ac::draw_func[ac::subfilter](frame_x);
    } else return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < start_1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int i =(frame.cols-1); i > start_2; --i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
        ac::draw_func[ac::subfilter](frame_x);
    } else return;
    
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix_copy = pixel;
            for(int j = 0; j < 3; ++j) {
                pixel[j] += (((pix_copy[3-j-1] ^ static_cast<unsigned char>(color[j]))) * alpha);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(color_value[j]*alpha);
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
    static MatrixCollection<3> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 7.0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[3];
            for(int j = 0; j < 3; ++j) {
                pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
            }
            for(int j = 0; j < 3; ++j) {
                double value = ((pix[0][j] ^ pix[1][j] ^ pix[2][j])/3) * alpha;
                pixel[j] ^= static_cast<unsigned char>(value) ^ static_cast<unsigned char>(alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RandomXorBlend(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^pix[j]);
            }
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[3-index-1] ^= static_cast<unsigned char>(pixel[index]*alpha);
        }
        for(int z = pos[index]; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] ^= static_cast<unsigned char>(pixel[index]*alpha);
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







