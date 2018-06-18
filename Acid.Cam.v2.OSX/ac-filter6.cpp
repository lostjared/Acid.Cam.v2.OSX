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

#include"ac.h"

double blend_percentage = 0.5;

void ac::RandomXorFlash(cv::Mat &frame) {
    static int index = 0;
    int col_p[3] = { rand()%255, rand()%255, rand()%255 };
    int col_x[3] = { rand()%255, rand()%255, rand()%255 };
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ ((col_p[j] ^ col_x[j]) * static_cast<unsigned int>(alpha)));
                        break;
                    case 1:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ col_p[j]);
                        break;
                    case 2:
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ (col_p[j]+col_x[j]));
                        break;
                }
            }
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
    ++index;
    if(index > 2) index = 0;
}

void ac::RandomAmountMedianBlur(cv::Mat &frame) {
    int random = rand()%10;
    for(int j = 0; j < random; ++j)
        MedianBlur(frame);
}

void ac::SoftXor(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    RandomAmountMedianBlur(frame);
    collection.shiftFrames(frame);
    double alpha = 1.0, alpha_max = 3.0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b val = collection.frames[q].at<cv::Vec3b>(z, i);
                unsigned char v[3] = { static_cast<unsigned char>(s[0]), static_cast<unsigned char>(s[1]), static_cast<unsigned char>(s[2])};
                s[0] = (v[0] + val[0]) ^ static_cast<unsigned char>(s[0]);
                s[1] = (v[1] + val[1]) ^ static_cast<unsigned char>(s[1]);
                s[2] = (v[2] + val[2]) ^ static_cast<unsigned char>(s[2]);
            }
            
            s[0] /= collection.size();
            s[1] /= collection.size();
            s[2] /= collection.size();
            
            for(int j = 0; j < 3; ++j) {
                unsigned char v = static_cast<unsigned char>(s[j]);
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ v) * static_cast<unsigned char>(alpha));
            }
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 8.0, 0.1);
    
}

void ac::SelfXorBlend(cv::Mat &frame) {
    static unsigned char index[3] = {static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255)};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ index[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    for(int j = 0; j < 3; ++j)
        ++index[j];
}

void ac::SelfXorDoubleFlash(cv::Mat &frame) {
    static double alpha = 1.0;
    int r[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j] ^ (static_cast<unsigned int>(alpha)+r[j]);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    if(dir == 1) {
        alpha += alpha_increase;
        if(alpha > 255)
            dir = 0;
    } else {
        alpha -= alpha_increase;
        if(alpha <= 0)
            dir = 1;
    }
}

void ac::SelfOrDoubleFlash(cv::Mat &frame) {
    static double alpha = 1.0;
    int r[3] = {rand()%255,rand()%255,rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j] | (static_cast<unsigned int>(alpha) ^ r[j]);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    if(dir == 1) {
        alpha += alpha_increase;
        if(alpha > 255)
            dir = 0;
    } else {
        alpha -= alpha_increase;
        if(alpha <= 0)
            dir = 1;
    }
}

void ac::BlendRowCurvedSqrt(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                double amount = sqrt(i*z);
                pixel[j] = static_cast<unsigned char>((pixel[j]^static_cast<unsigned char>(amount))*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::CycleShiftRGB(cv::Mat &frame) {
    static int offset[3] = {0, ((frame.cols/2)/2), (frame.cols/2)};
    if(reset_filter == true) {
        offset[0] = 0;
        offset[1] = ((frame.cols/2)/2);
        offset[2] = (frame.cols/2);
        reset_filter = false;
    }
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int j = 0; j < 3; ++j) {
            for(int i = 0; i < offset[j]; ++i) {
                if(i >= 0 && i < frame.cols) {
                    if(offset[j]-i >= 0) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, offset[j]-i);
                        cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, i);
                        pixel[j] = pix_copy[j];
                        swapColors(frame, z, i);// swap colors
                        if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
                    }
                }
            }
            int index = 0;
            for(int i = offset[j]; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, index);
                pixel[j] = pix_copy[j];
                ++index;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
            
        }
    }
    for(int j = 0; j < 3; ++j) {
        ++offset[j] += 50;
        if(offset[j] > frame.cols)
            offset[j] = 0;
    }
}

void ac::CycleShiftRandomRGB(cv::Mat &frame) {
    static int offset[3] = {0, ((frame.cols/2)/2), (frame.cols/2)};
    static bool lazy = false;
    if(reset_filter == true || lazy == false) {
        switch(rand()%2) {
            case 0:
                offset[0] = 0;
                offset[1] = ((frame.cols/2)/2);
                offset[2] = (frame.cols/2);
                break;
            case 1:
                offset[2] = 0;
                offset[0] = ((frame.cols/2)/2);
                offset[1] = (frame.cols/2);
                break;
            default:
                offset[1] = 0;
                offset[2] = ((frame.cols/2)/2);
                offset[0] = (frame.cols/2);
                break;
        }
        lazy = true;
        reset_filter = false;
    }
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int j = 0; j < 3; ++j) {
            for(int i = 0; i < offset[j]; ++i) {
                if(i >= 0 && i < frame.cols) {
                    if(offset[j]-i >= 0) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, offset[j]-i);
                        cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, i);
                        pixel[j] = pix_copy[j];
                    }
                    swapColors(frame, z, i);// swap colors
                    if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
                }
            }
            int index = 0;
            for(int i = offset[j]; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, index);
                pixel[j] = pix_copy[j];
                ++index;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        ++offset[j] += 50;
        if(offset[j] > frame.cols)
            offset[j] = 0;
    }
}

void ac::CycleShiftRandomRGB_XorBlend(cv::Mat &frame) {
    static int offset[3] = {0, ((frame.cols/2)/2), (frame.cols/2)};
    static bool lazy = false;
    if(reset_filter == true || lazy == false) {
        switch(rand()%2) {
            case 0:
                offset[0] = 0;
                offset[1] = ((frame.cols/2)/2);
                offset[2] = (frame.cols/2);
                break;
            case 1:
                offset[2] = 0;
                offset[0] = ((frame.cols/2)/2);
                offset[1] = (frame.cols/2);
                break;
            default:
                offset[1] = 0;
                offset[2] = ((frame.cols/2)/2);
                offset[0] = (frame.cols/2);
                break;
        }
        lazy = true;
        reset_filter = false;
    }
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int j = 0; j < 3; ++j) {
            for(int i = 0; i < offset[j]; ++i) {
                if(i >= 0 && i < frame.cols) {
                    if(offset[j]-i >= 0) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, offset[j]-i);
                        cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, i);
                        pixel[j] ^= pix_copy[j];
                        swapColors(frame, z, i);// swap colors
                        if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
                    }
                }
            }
            int index = 0;
            for(int i = offset[j]; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, index);
                pixel[j] ^= pix_copy[j];
                ++index;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        ++offset[j] += 50;
        if(offset[j] > frame.cols)
            offset[j] = 0;
    }
}

void ac::CycleShiftRandomAlphaBlend(cv::Mat &frame) {
    static double alpha[3] = {1.0,4.0,2.5}, alpha_max = 3.0;
    static int offset[3] = {0, ((frame.cols/2)/2), (frame.cols/2)};
    static bool lazy = false;
    if(reset_filter == true || lazy == false) {
        switch(rand()%2) {
            case 0:
                offset[0] = 0;
                offset[1] = ((frame.cols/2)/2);
                offset[2] = (frame.cols/2);
                break;
            case 1:
                offset[2] = 0;
                offset[0] = ((frame.cols/2)/2);
                offset[1] = (frame.cols/2);
                break;
            default:
                offset[1] = 0;
                offset[2] = ((frame.cols/2)/2);
                offset[0] = (frame.cols/2);
                break;
        }
        lazy = true;
        reset_filter = false;
        alpha[0] = 1.0;
        alpha[1] = 4.0;
        alpha[2] = 2.5;
    }
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int j = 0; j < 3; ++j) {
            for(int i = 0; i < offset[j]; ++i) {
                if(i >= 0 && i < frame.cols) {
                    if(offset[j]-i >= 0) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, offset[j]-i);
                        cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, i);
                        pixel[j] = static_cast<unsigned char>((pixel[j] * alpha[j]) + (pix_copy[j] * alpha[j]));
                        swapColors(frame, z, i);// swap colors
                        if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
                    }
                }
            }
            int index = 0;
            for(int i = offset[j]; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix_copy = frame_copy.at<cv::Vec3b>(z, index);
                pixel[j] = static_cast<unsigned char>((pixel[j] * alpha[j]) + (pix_copy[j] * alpha[j]));
                ++index;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
    }
    
    static int dir[3] = {1,0,1};
    for(int j = 0; j < 3; ++j) {
        ++offset[j] += 50;
        if(offset[j] > frame.cols)
            offset[j] = 0;
        
        procPos(dir[j], alpha[j], alpha_max, 6, 0.03);
    }
}

void ac::VerticalColorBars(cv::Mat &frame) {
    static double alpha[3] = { 1.0, 3.0, 7.0}, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]^(i)*static_cast<unsigned char>(alpha[j]));
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir[3] = { 1,1,0 };
    for(int j = 0; j < 3; ++j)
        procPos(dir[j], alpha[j], alpha_max,10.0, 0.3);
}

void ac::GradientLeftRight(cv::Mat &frame) {
    static double inc[3] = {1, 25, 50}, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        double amt[3] = {5, 16, 30};
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(amt[j]);
                amt[j] += inc[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] -= static_cast<unsigned char>(amt[j]); //static_cast<unsigned char>(amt);
                amt[j] -= inc[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir[3] = {1, 1, 1};
    for(int j = 0; j < 3; ++j) {
        procPos(dir[j], inc[j], alpha_max, 10, 0.1);
    }
}

void ac::GraidentUpDown(cv::Mat &frame) {
    static double inc[3] = {1, 25, 50}, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        double amt[3] = {5, 16, 30};
        for(int z = 0; z < frame.rows/2; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(amt[j]);
                amt[j] += inc[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int z = frame.rows/2; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] -= static_cast<unsigned char>(amt[j]);
                amt[j] -= inc[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir[3] = {1, 1, 1};
    for(int j = 0; j < 3; ++j) {
        procPos(dir[j], inc[j], alpha_max, 10, 0.1);
    }
}

void ac::GradientLeftRightInOut(cv::Mat &frame) {
    static double inc[3] = {1, 25, 50}, alpha_max = 3.0;
    static int dir[3] = {1, 0, 1};
    for(int z = 0; z < frame.rows; ++z) {
        double amt[3] = {5, 16, 30};
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(dir[j] == 0) {
                	pixel[j] += static_cast<unsigned char>(amt[j]);
                	amt[j] += inc[j];
                } else {
                    pixel[j] -= static_cast<unsigned char>(amt[j]);
                    amt[j] -= inc[j];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(dir[j] == 1) {
                    pixel[j] += static_cast<unsigned char>(amt[j]);
                    amt[j] += inc[j];
                } else {
                    pixel[j] -= static_cast<unsigned char>(amt[j]);
                    amt[j] -= inc[j];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    for(int j = 0; j < 3; ++j) {
        procPos(dir[j], inc[j], alpha_max, 10, 0.1);
    }
}

void ac::GradientUpDownInOut(cv::Mat &frame) {
    static double inc[3] = {1, 25, 50}, alpha_max = 3.0;
    static int dir[3] = {1, 0, 1};
    for(int i = 0; i < frame.cols; ++i) {
        double amt[3] = {5, 16, 30};
        for(int z = 0; z < frame.rows/2; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(dir[j] == 0) {
                    pixel[j] += static_cast<unsigned char>(amt[j]);
                    amt[j] += inc[j];
                } else {
                    pixel[j] -= static_cast<unsigned char>(amt[j]);
                    amt[j] -= inc[j];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int z = frame.rows/2; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(dir[j] == 1) {
                    pixel[j] += static_cast<unsigned char>(amt[j]);
                    amt[j] += inc[j];
                } else {
                    pixel[j] -= static_cast<unsigned char>(amt[j]);
                    amt[j] -= inc[j];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    for(int j = 0; j < 3; ++j) {
        procPos(dir[j], inc[j], alpha_max, 10, 0.1);
    }
}

void ac::Lines(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        int r_start = (rand()%(frame.cols/2));
        int r_stop = (frame.cols/2)+(rand()%(frame.cols/2));
        int r_height = 1+rand()%3;
        bool sw = true;
        for(int q = z; q < frame.rows && q < z+r_height; ++q) {
        	for(int i = r_start; i < frame.cols && i < r_stop; ++i) {
                if(q >= 0 && q < frame.rows && i >= 0 && i < frame.cols) {
        	    	cv::Vec3b &pixel = frame.at<cv::Vec3b>(q, i);
    	        	for(int j = 0; j < 3; ++j) {
	                    pixel[j] = (sw == true) ? 255 : 0;
            		}
                    swapColors(frame, q, i);// swap colors
                    if(isNegative) invert(frame, q, i);// if isNegative invert pixel */
                }
        	}
            sw = (sw == true) ? false : true;
        }
        z += r_height-1;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::ColorLines(cv::Mat &frame) {
    int diff = frame.cols/255;
    static double alpha = 1.0, alpha_max = 2.0;
    for(int z = 0; z < frame.rows; ++z) {
        int total[3] = {rand()%16, rand()%32, rand()%64};
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^total[j])*alpha;
                if((i%diff)==0) {
                    ++total[j];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 8.0, 0.01);
}

void ac::WhiteLines(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; z += rand()%20) {
        int num  = rand()%25, skip = rand()%10;
        int count = 0, skip_count = 0;
        for(int i = 0; i < frame.cols; ++i) {
            if(count < num) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
            		cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            		pixel[0] = pixel[1] = pixel[2] = 255;
                }
                ++count;
            } else {
                if(skip_count >= skip) {
                    skip_count = 0;
                    count = 0;
                    num = rand()%25;
                    skip = rand()%10;
                } else {
                    ++skip_count;
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::ThickWhiteLines(cv::Mat &frame) {
    for(int j = 0; j < 5; ++j) {
        int start_y = 0;
        if(j > 0) start_y = rand()%frame.rows;
        for(int z = start_y; z < frame.rows; z += rand()%30) {
            int num  = rand()%50, skip = rand()%25;
            int count = 0, skip_count = 0;
            for(int i = 0; i < frame.cols; ++i) {
                if(count < num) {
                    if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    	cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    	pixel[0] = pixel[1] = pixel[2] = 255;
                    }
                    ++count;
                } else {
                    if(skip_count >= skip) {
                        skip_count = 0;
                        count = 0;
                        num = rand()%50;
                        skip = rand()%20;
                    } else {
                        ++skip_count;
                    }
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
    }
}

void ac::UseLineObject(cv::Mat &frame) {
    static HLine obj_lines;
    if(obj_lines.empty()) {
        obj_lines.createLines(100, frame.cols, frame.rows);
    }
    obj_lines.drawLines(frame);
}

void ac::TanAlphaGrid(cv::Mat &frame) {
    static double alpha[3] = {1.0, 32.0, 64.0}, alpha_max = 7.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((tan(i)*alpha[0])+pixel[0]);
            pixel[1] = static_cast<unsigned char>((tan(z)*alpha[1])+pixel[1]);
            pixel[2] = static_cast<unsigned char>((tan(z-i)*alpha[2])+pixel[2]);
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir[3] = {1,1,1};
    for(int j = 0; j < 3; ++j)
    	procPos(dir[j], alpha[j], alpha_max);
}

// No Filter
void ac::NoFilter(cv::Mat &) {}
// Alpha Blend with Original Frame
void ac::BlendWithSource(cv::Mat &frame) {
    ac::pass2_alpha = blend_percentage; // set to 50%
    Pass2Blend(frame);// call Pass2 function
}

void ac::setBlendPercentage(const double &value) {
    blend_percentage = value;
}

