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
#include "fractal.h"


void ac::OppositeBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    cv::Mat temp = frame.clone();
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b temp_pixel = temp.at<cv::Vec3b>(z, w-i-1);
            pixel[0] = static_cast<unsigned char>((pixel[0]+temp_pixel[0]));
            pixel[1] = static_cast<unsigned char>((pixel[1]+temp_pixel[1]));
            pixel[2] = static_cast<unsigned char>((pixel[2]+temp_pixel[2]));
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
}

void ac::DiagonalLines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    cv::Mat temp = frame.clone();
    static double pos = 1.0;
    
    for(int i = 0; i < w-1; ++i) {
        for(int z = 0; z < h-1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b temp_pixel = temp.at<cv::Vec3b>(h-z-1, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]+temp_pixel[j])+pos);
                ++pos;
                if(pos > 100) pos = 0;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
}

void ac::HorizontalLines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos[3] = {1.0, 16.0, 32.0};
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] + pos[j]);
                pos[j] += 0.1;
                if(pos[j] > 100)
                    pos[j] = 0;
                
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
}

void ac::InvertedScanlines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    
    static unsigned int index = 0;
    static double alpha = 1.0;
    static double pos_max = 14.0;
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0: {
                    for(int j = 0; j < 3; ++j)
                        pixel[j] = static_cast<unsigned char>((~pixel[j])*alpha);
                    index++;
                }
                case 1: {
                    cv::Vec3b temp = pixel;
                    pixel[0] = static_cast<unsigned char>(temp[2]*alpha);
                    pixel[1] = static_cast<unsigned char>(temp[1]*alpha);
                    pixel[2] = static_cast<unsigned char>(temp[0]*alpha);
                    index++;
                }
                    break;
                case 2:
                    index = 0;
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    
    static int direction = 1;
    procPos(direction, alpha, pos_max);
}

void ac::Soft_Mirror(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned int index = 0;
    cv::Mat temp = frame.clone();
    for(int z = 1; z < h-1; ++z) {
        for(int i = 1; i < w-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    index++;
                    break;
                case 1: {
                    cv::Vec3b pix = frame.at<cv::Vec3b>(h-z-1, w-i-1);
                    pixel[0] = pix[0];
                    pixel[1] = pix[1];
                    pixel[2] = pix[2];
                    index = 0;
                }
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
}

void ac::KanapaTrip(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned int start_index = 0;
    unsigned int index = start_index;
    cv::Mat temp = frame.clone();
    for(int z = 1; z < h-1; ++z) {
        for(int i = 1; i < w-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    index++;
                    break;
                case 1: {
                    cv::Vec3b pix = frame.at<cv::Vec3b>(h-z-1, w-i-1);
                    pixel[0] = pix[0];
                    pixel[1] = pix[1];
                    pixel[2] = pix[2];
                    index = 0;
                }
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    if(start_index == 0)
        start_index = 1;
    else
        start_index = 0;
}

void ac::ColorMorphing(cv::Mat &frame) {
    KanapaTrip(frame);
    SidewaysMirror(frame);
}


void ac::ScanSwitch(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned int start_index = 0;
    unsigned int index = start_index;
    for(int z = 3; z < h-3; ++z) {
        for(int i = 3; i < w-3; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    index++;
                    break;
                case 1: {
                    pixel[0] = ~pixel[0];
                    pixel[1] = ~pixel[1];
                    pixel[2] = ~pixel[2];
                    index = 0;
                }
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    if(start_index == 0)
        start_index = 1;
    else
        start_index = 0;
}


void ac::ScanAlphaSwitch(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned int start_index = 0;
    static double alpha = 1.0, alpha_max = 10.0;
    unsigned int index = start_index;
    for(int z = 3; z < h-3; ++z) {
        for(int i = 3; i < w-3; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    index++;
                    pixel[0] = ~pixel[0];
                    pixel[1] = ~pixel[1];
                    pixel[2] = ~pixel[2];
                    break;
                case 1: {
                    pixel[0] += static_cast<unsigned char>(pixel[0]*alpha);
                    pixel[1] += static_cast<unsigned char>(pixel[1]*alpha);
                    pixel[2] += static_cast<unsigned char>(pixel[2]*alpha);
                    index = 0;
                }
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    if(start_index == 0)
        start_index = 1;
    else
        start_index = 0;
    
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::NegativeStrobe(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned int flash = 1;
    if(flash == 1) {
        for(int z = 0; z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                pixel[0] = ~pixel[0];
                pixel[1] = ~pixel[1];
                pixel[2] = ~pixel[2];
            }
        }
    }
    if(flash == 1) {
        flash = 0;
    } else {
        flash = 1;
    }
}

void ac::XorAddMul(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double blend = 1.0, blend_max = 13.0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            unsigned int b = static_cast<unsigned int>(blend);
            pixel[0] += static_cast<unsigned char>(pixel[0]^b);
            pixel[1] += static_cast<unsigned char>(pixel[1]+b);
            pixel[2] += static_cast<unsigned char>(pixel[2]*b);
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    static int direction = 1;
    procPos(direction, blend, blend_max);
    //if(blend > 255) blend = 1.0;
}


void ac::BlendSwitch(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static unsigned int pos = 0;
    static unsigned char blend_pixel = 0;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[pos] *= blend_pixel++;
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        pos++;
        if(pos > 2) pos = 0;
    }
}

// set all color components other than red to zero
void ac::AllRed(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = pixel[1] = 0;
        }
    }
}
// set all color components other than green to zero
void ac::AllGreen(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = pixel[2] = 0;
        }
    }
}
// set all color components other than blue to zero
void ac::AllBlue(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[2] = pixel[1] = 0;
        }
    }
}
// set colors to zero based on counter
// increment counter after each nested loop
void ac::LineRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static unsigned int counter = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(counter) {
                case 0:
                    pixel[0] = pixel[1] = 0;
                    break;
                case 1:
                    pixel[0] = pixel[2] = 0;
                    break;
                case 2:
                    pixel[2] = pixel[1] = 0;
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        ++counter;
        if(counter > 2) counter = 0;
    }
}
// set colors to zero based on counter
// increment counter each iteration of nested loop
void ac::PixelRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static unsigned int counter = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(counter) {
                case 0:
                    pixel[0] = pixel[1] = 0;
                    break;
                case 1:
                    pixel[0] = pixel[2] = 0;
                    break;
                case 2:
                    pixel[2] = pixel[1] = 0;
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
            ++counter;
            if(counter > 2) counter = 0;
        }
    }
}

// Boxed RGB
void ac::BoxedRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static int row_counter = 0; // row counter
    
    for(int z = 0; z < h; ++z) { // from top to bottom
        for(int i = 0; i < w; ++i) { // from left to right
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i); // pixel
            switch(row_counter) {// row counter iterate between red,green,and blue
                case 0:
                    pixel[0] = pixel[1] = 0; // red
                    break;
                case 1:
                    pixel[0] = pixel[2] = 0; // green
                    break;
                case 2:
                    pixel[2] = pixel[1] = 0; // blue
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        // if z is evenly divideable by 32
        if((z%32) == 0) {
            ++row_counter;// increment row counter
            if(row_counter > 3) row_counter = 0;
        }
    }
}

// joke filter
// color the image with red/green bars switching color each frame
void ac::KruegerSweater(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static int row_counter = 0;// row counter
    static unsigned int rg = 0;// row counter start variable
    row_counter = rg; // set row counter to start
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // set the colors other than red or green to zero based on row counter
            switch(row_counter) {
                case 0:
                    pixel[0] = pixel[1] = 0; // red
                    break;
                case 1:
                    pixel[0] = pixel[2] = 0; // green
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        if((z%32) == 0) {
            ++row_counter; // increment row counter
            if(row_counter >= 2) { // if greater than or equal 2
                row_counter = 0; // set to row_counter to zero
            }
        }
    }
    rg = (rg == 0) ? 1 : 0; // swap back and forth rg between zero and one.
}

void ac::RGBFlash(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static unsigned int counter = 0; // counter for setting the pixel
    static unsigned int start = 0; // start position
    for(int z = start; z < h; z += 2) { // top to bottom, skipping 1 each time
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // set pixel a certain color based on the counter
            switch(counter) {
                case 0:
                    pixel[2] = 255;// set red
                    break;
                case 1:
                    pixel[1] = 255; // set green
                    break;
                case 2:
                    pixel[0] = 255;// set blue
                    break;
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    ++counter;// increment counter
    if(counter > 2) counter = 0; // if greater than 2 reset to zero
    start = (start == 0) ? 1 : 0; // swap start back and forth between 0 and 1
}

void ac::IncreaseBlendHorizontal(cv::Mat &frame) {
    ac::orig_frame = frame.clone();
    const int w = frame.cols;
    const int h = frame.rows;
    for(int i = 0; i < w; ++i) {
        cv::Vec3b pix;
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix[0] += pixel[0]/2;
            pix[1] += pixel[1]/4;
            pix[2] += pixel[2]/6;
            pixel[0] += pixel[0] * (pix[0]/32);
            pixel[1] += pixel[1] * (pix[1]/32);
            pixel[2] += pixel[2] * (pix[2]/32);
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    ac::pass2_alpha = 0.75;
    Pass2Blend(frame);
}
// blend increase
void ac::BlendIncrease(cv::Mat &frame) {
    static int blend_r = rand()%255, blend_g = rand()%255, blend_b = rand()%255;
    static bool cblend_r = true, cblend_g = true, cblend_b = true;
    static unsigned int increase_value_r = 2, increase_value_g = 2, increase_value_b = 2;
    const int w = frame.cols;
    const int h = frame.rows;
    if(blend_r > 255) {
        blend_r = rand()%255;
        if(cblend_r == true) {
            blend_r = -blend_r;
            cblend_r = false;
        } else {
            cblend_r = true;
        }
    }
    if(blend_g > 255) {
        blend_g = rand()%255;
        if(cblend_g == true) {
            blend_g = -blend_g;
            cblend_g = false;
        } else {
            cblend_g = true;
        }
    }
    if(blend_b > 255) {
        blend_b = rand()%255;
        if(cblend_b == true) {
            blend_b = -blend_b;
            cblend_b = false;
        } else {
            cblend_b = true;
        }
    }
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[2] += static_cast<unsigned char>(blend_r);
            pixel[1] += static_cast<unsigned char>(blend_g);
            pixel[0] += static_cast<unsigned char>(blend_b);
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    blend_r += increase_value_r;
    blend_g += increase_value_g;
    blend_b += increase_value_b;
    increase_value_r += rand()%5;
    increase_value_g += rand()%5;
    increase_value_b += rand()%5;
    if(increase_value_r > 20) {
        increase_value_r = 2;
    }
    if(increase_value_g  > 20) {
        increase_value_g = 2;
    }
    if(increase_value_b > 20) {
        increase_value_b = 2;
    }
}

void ac::GradientReverse(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static bool direction = true;
    static double alpha = 1.0, alpha_max = 8;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(direction == true)
                    pixel[j] += static_cast<unsigned char>(i*alpha);
                else
                    pixel[j] -= static_cast<unsigned char>(i*alpha);
            }
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
        direction = (direction == true) ? false : true;
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}

void ac::GradientReverseBox(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static bool direction = true;
    static double alpha = 1.0, alpha_max = 8;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(direction == true)
                    pixel[j] += static_cast<unsigned char>((i*alpha));
                else
                    pixel[j] -= static_cast<unsigned char>((z*alpha));
            }
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
        direction = (direction == true) ? false : true;
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}

void ac::GradientReverseVertical(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static bool direction = true;
    static double alpha = 1.0, alpha_max = 8;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(direction == true)
                    pixel[j] += static_cast<unsigned char>((z*alpha));
                else
                    pixel[j] -= static_cast<unsigned char>((z*alpha));
            }
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
        direction = (direction == true) ? false : true;
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}

void ac::GradientNewFilter(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static unsigned int index = 0;
    static double alpha = 1.0, alpha_max = 9;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = static_cast<unsigned char>((pixel[j] ^ (unsigned int)(alpha*z)));
                        break;
                    case 1:
                        pixel[j] = static_cast<unsigned char>((pixel[j] & (unsigned int)(alpha*i)));
                        break;
                    case 2:
                        pixel[j] = static_cast<unsigned char>((pixel[j] ^ (unsigned int)alpha));
                        break;
                }
            }
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
            ++index;
            if(index > 2) index = 0;
        }
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}

void ac::ReinterpretDouble(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double alpha = 1.0, alpha_max = 8;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            unsigned char *value = reinterpret_cast<unsigned char*>(&alpha);
            for(int j = 0; j < 3; ++j)
                pixel[j] = cv::saturate_cast<uchar>(pixel[j] ^ value[j]);
            
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}


void ac::ReinterpSelfScale(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static unsigned int index = 0;
    static double alpha = 1.0, alpha_max = 8;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            unsigned char *value = reinterpret_cast<unsigned char*>(&alpha);
            switch(index) {
                case 0: {
                    pixel[0] = static_cast<unsigned char>(pixel[0]*alpha)^value[0];
                    pixel[1] = static_cast<unsigned char>(pixel[1]*alpha);
                    pixel[2] = static_cast<unsigned char>(pixel[2]*alpha);
                }
                    break;
                case 1: {
                    pixel[0] = static_cast<unsigned char>(pixel[0]*alpha);
                    pixel[1] = static_cast<unsigned char>(pixel[0]*alpha)^value[1];
                    pixel[2] = static_cast<unsigned char>(pixel[2]*alpha);
                }
                    break;
                case 2: {
                    pixel[0] = static_cast<unsigned char>(pixel[0]*alpha);
                    pixel[1] = static_cast<unsigned char>(pixel[1]*alpha);
                    pixel[2] = static_cast<unsigned char>(pixel[2]*alpha)^value[2];
                }
                    break;
            }
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
        ++index;
        if(index > 2) index = 0;
    }
    static int direction_ = 1;
    procPos(direction_, alpha, alpha_max);
}

void ac::AverageLines(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    unsigned char average[3];
    static double alpha = 1.0, alpha_max = 11;
    for(int z = 0; z < h; ++z) {
        cv::Scalar s(1,1,1);
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            s[0] += pixel[0];
            s[1] += pixel[1];
            s[2] += pixel[2];
            pixel[0] = static_cast<unsigned char>((pixel[0]^average[0])*alpha);
            pixel[1] = static_cast<unsigned char>((pixel[1]^average[1])*alpha);
            pixel[2] = static_cast<unsigned char>((pixel[2]^average[2])*alpha);
            ac::swapColors(frame, z, i);
            if(isNegative) ac::invert(frame, z, i);
        }
        average[0] = cv::saturate_cast<unsigned char>((s[0]/w));
        average[1] = cv::saturate_cast<unsigned char>((s[1]/w));
        average[2] = cv::saturate_cast<unsigned char>((s[2]/w));
    }
    int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::ImageFile(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                pixel[0] += add_i[0];
                pixel[1] += add_i[1];
                pixel[1] += add_i[2];
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
    }
    
}
void ac::ImageXor(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        static double alpha = 1.0, alpha_max = 4.0;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^add_i[j])*alpha;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}

void ac::ImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        static double alpha = 1.0, alpha_max = 2.0;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*alpha) + (add_i[j] * alpha));
                
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}


void ac::ColorRange(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double alpha = 1.0, alpha_max = 6;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]+colors[j])*alpha);
        }
    }
    static unsigned int direction[3] = {1, 0, 1};
    for(int j = 0; j < 3; ++j) {
        if(direction[j] == 1) {
            colors[j] ++;
            if(colors[j] >= 255) {
                direction[j] = 0;
                colors[j] = 255;
            }
        } else if(direction[j] == 0) {
            colors[j] --;
            if(colors[j] <= 0) {
                direction[j] = 1;
                colors[j] = 0;
            }
        }
    }
    
    static int _direction = 1;
    procPos(_direction, alpha, alpha_max);
}

void ac::ImageInter(cv::Mat &frame) {
    if(blend_set == true) {
        static int start = 0, restart = 0;
        const int w = frame.cols;
        const int h = frame.rows;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                if(start == 0) {
                    pixel = add_i;
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
            start = (start == 0) ? 1 : 0;
        }
        if(restart == 0) {
            start = 1;
            restart = 1;
        } else {
            start = 0;
            restart = 0;
        }
    }
}

void ac::TrailsInter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int counter = 0;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = collection.frames[counter+1].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter > 6) counter = 0;
    }
}

void ac::TrailsBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat new_frame = frame.clone();
    ac::SelfAlphaBlend(new_frame);
    collection.shiftFrames(new_frame);
    static int counter = 0;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = collection.frames[counter+1].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter > 6) counter = 0;
    }
}

void ac::TrailsNegate(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static unsigned int index = 0;
    cv::Mat new_frame = frame.clone();
    if(index == 0) {
        ac::Negate(new_frame);
        index = 1;
    } else {
        index = 0;
    }
    collection.shiftFrames(new_frame);
    static int counter = 0;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h-1; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = collection.frames[counter+1].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter > 6) counter = 0;
    }
}

void ac::InterReverse(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    bool get_value = true;
    static bool value_start = true;
    get_value = value_start;
    cv::Mat frame_copy = frame.clone();
    for(int z = 2; z < h-2; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(get_value == true) {
                cv::Vec3b value;
                value = frame_copy.at<cv::Vec3b>(h-z-1, i);
                pixel = value;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        get_value = (get_value == true) ? false : true;
    }
    value_start = (value_start == true) ? false : true;
}

void ac::InterMirror(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    bool get_value = true;
    static bool value_start = true;
    get_value = value_start;
    cv::Mat frame_copy = frame.clone();
    for(int z = 2; z < h-2; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(get_value == true) {
                cv::Vec3b value;
                value = frame_copy.at<cv::Vec3b>(h-z-1, i);
                cv::Vec3b value2;
                value2 = frame_copy.at<cv::Vec3b>(z, w-i-1);
                cv::Vec3b value3;
                value3 = frame_copy.at<cv::Vec3b>(h-z-1, w-i-1);
                pixel[0] = static_cast<unsigned char>(value[0]+value2[0]+value3[0]);
                pixel[1] = static_cast<unsigned char>(value[1]+value2[1]+value3[1]);
                pixel[2] = static_cast<unsigned char>(value[2]+value2[2]+value3[2]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        get_value = (get_value == true) ? false : true;
    }
    value_start = (value_start == true) ? false : true;
}

void ac::InterFullMirror(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    unsigned int index = 0;
    cv::Mat frame_copy = frame.clone();
    for(int z = 2; z < h-2; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    continue;
                case 1:
                    pixel = frame_copy.at<cv::Vec3b>(h-z-1, i);
                    break;
                case 2:
                    pixel = frame_copy.at<cv::Vec3b>(z, w-i-1);
                    break;
                case 3:
                    pixel = frame_copy.at<cv::Vec3b>(h-z-1, w-i-1);
                    break;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++index;
        if(index > 3) index = 0;
    }
}

void ac::MirrorRGB(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    cv::Mat frame_copy = frame.clone();
    for(int z = 2; z < h-2; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b frame_pixels[4] = {frame_copy.at<cv::Vec3b>(h-z-1, i), frame_copy.at<cv::Vec3b>(z, w-i-1), frame_copy.at<cv::Vec3b>(h-z-1, w-i-1)};
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(frame_pixels[j][j]);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::RGBStatic1(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double pos = 0.25;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b add(rand()%255, rand()%255, rand()%255);
            for(int j = 0; j < 3; ++j)
                pixel[j] += static_cast<unsigned char>(add[j] * pos);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double direction_max = 0.4;
    if(direction == 1) {
        pos += 0.005;
        if(pos > direction_max) {
            direction = 0;
            direction_max += 0.05;
            if(direction_max >= 0.8) {
                direction_max = 0.5;
            }
        }
    } else if(direction == 0) {
        pos -= 0.005;
        if(pos <= 0.25)
            direction = 1;
    }
}

void ac::RGBStatic2(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double pos = 0.05;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b add(rand()%255, rand()%255, rand()%255);
            for(int j = 0; j < 3; ++j)
                pixel[j] += static_cast<unsigned char>(add[j] * pos);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double direction_max = 0.4;
    if(direction == 1) {
        pos += 0.005;
        if(pos > direction_max) {
            direction = 0;
            direction_max += 0.05;
            if(direction_max >= 0.9) {
                direction_max = 0.4;
            }
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 0.05)
            direction = 1;
    }
    
}

void ac::VectorIncrease(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double pos = 0.25;
    static cv::Vec3b value(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]*value[j]) * pos);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    for(int j = 0; j < 3; ++j)
        value[j] += rand()%8;
}

void ac::LineByLineReverse(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    bool rev = false;
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < h; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(rev == true)
                break;
            else
                pixel = frame_copy.at<cv::Vec3b>(z, (w-i-1));
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        rev = (rev == true) ? false : true;
    }
}

void ac::RandomIntertwine(cv::Mat &frame) {
    cv::Mat frame1 = frame.clone(), frame2 = frame.clone();
    randomFilter(frame1);
    randomFilter(frame2);
    const int w = frame.cols;
    const int h = frame.rows;
    bool rev = false;
    for(int z = 0; z < h; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(rev == true)
                pixel = frame1.at<cv::Vec3b>(z, i);
            else
                pixel = frame2.at<cv::Vec3b>(z, (w-i-1));
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        rev = (rev == true) ? false : true;
    }
}

void ac::RandomFour(cv::Mat &frame) {
    cv::Mat frames[4];
    frames[0] = frame;
    for(int j = 1; j < 4; ++j) {
        frames[j] = frame.clone();
        randomFilter(frames[j]);
    }
    const int w = frame.cols, h = frame.rows;
    static unsigned int row = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(row == 0) break;
            else {
                pixel = frames[row].at<cv::Vec3b>(z, i);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++row;
        if(row > 3) row = 0;
    }
}

void ac::BlendThree(cv::Mat &frame) {
    static double pos = 1.0, pos_max = 8.0;
    cv::Mat frames[3];
    frames[0] = frame;
    for(int j = 1; j < 3; ++j) {
        frames[j] = frame.clone();
        randomFilter(frames[j]);
    }
    const int w = frame.cols, h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b values[4];
            for(int j = 0; j < 3; ++j)
                values[j] = frames[j].at<cv::Vec3b>(z, i);
            
            pixel[0] = values[0][0] + values[1][0] + values[2][0];
            pixel[1] = values[0][1] + values[1][1] + values[2][1];
            pixel[2] = values[0][2] + values[1][2] + values[2][2];
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] /= 3;
                pixel[j] *= static_cast<unsigned char>(pos);
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::AcidTrails(cv::Mat &frame) {
    const int w = frame.cols, h = frame.rows;
    cv::Mat frame_copies[3];
    
    for(int j = 0; j < 3; ++j)
        frame_copies[j] = frame.clone();
    
    ac::SelfScale(frame_copies[0]);
    ac::TrailsInter(frame_copies[0]);
    ac::Type(frame_copies[1]);
    ac::Outward(frame_copies[2]);
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = frame_copies[0].at<cv::Vec3b>(z, i);
            cv::Vec3b copy2 = frame_copies[1].at<cv::Vec3b>(z, i);
            cv::Vec3b copy3 = frame_copies[2].at<cv::Vec3b>(z, i);
            cv::Vec3b value;
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(((copy1[j] ^ copy2[j]) + copy3[j]));
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::RandomTwo(cv::Mat &frame) {
    cv::Mat frames[2];
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    randomFilter(frames[0]);
    randomFilter(frames[1]);
    static unsigned int index = 0;
    unsigned int w = frame.cols, h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = frames[index].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++index;
        if(index > 1) index = 0;
    }
}

void ac::HorizontalTrailsInter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int counter = 0;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = collection.frames[counter+1].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter > 6) counter = 0;
    }
}

void ac::Trails(cv::Mat &frame) {
    TrailsInter(frame);
    HorizontalTrailsInter(frame);
}

void ac::BlendTrails(cv::Mat &frame) {
    Negate(frame);
    rainbowBlend(frame);
    Trails(frame);
}

unsigned int ac::Box::frame_width = 0;
unsigned int ac::Box::frame_height = 0;

void ac::Box::initBox(int width, int height) {
    x = (rand()%width);
    y = (rand()%height);
    w = rand()%25;
    h = rand()%25;
    steps = rand()%10;
    index = 0;
    do {
        frame_index = rand()%28;
    } while(frame_index == 13 || frame_index == 14);
}

void ac::Box::drawBox(cv::Mat &frame) {
    cv::Mat temp;
    temp.create(cvSize(w, h), CV_8UC3);
    for(int yy = y, pos_y = 0; yy < y+h && yy < frame_height; ++yy, ++pos_y) {
        for(int ii = x,pos_x = 0; ii < x+w && ii < frame_width; ++ii, ++pos_x) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(yy, ii);
            cv::Vec3b &target = temp.at<cv::Vec3b>(pos_y, pos_x);
            target = pixel;
        }
    }
    ac::draw_func[frame_index](temp);
    for(int z = y, pos_y = 0; z < y+h && z < frame_height; ++z, ++pos_y) {
        for(int i = x, pos_x = 0; i < x+w && i < frame_width; ++i, ++pos_x) {
            if(i < frame.cols && z < frame.rows) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                pixel = temp.at<cv::Vec3b>(pos_y,pos_x);
            }
        }
    }
}

void ac::Box::sizeBox() {
    if(index > steps) {
        initBox(frame_width, frame_height);
        return;
    }
    ++index;
    unsigned int r1 = rand()%10;
    unsigned int r2 = rand()%10;
    if(w+r1 > frame_width) {
        initBox(frame_width, frame_height);
        return;
    } else {
        w += r1;
    }
    if(h+r2 > frame_height) {
        initBox(frame_width, frame_height);
        return;
    } else {
        h += r2;
    }
}

void ac::RandomFilteredSquare(cv::Mat &frame) {
    static std::unique_ptr<Box[]> boxes;
    unsigned int num_boxes = frame.cols/0.5;
    if(boxes == 0 || (frame.cols != Box::frame_width)) {
        boxes.reset(new Box[num_boxes]);
        Box::frame_width = frame.cols;
        Box::frame_height = frame.rows;
        for(int i = 0; i < num_boxes; ++i)
            boxes.get()[i].initBox(frame.cols, frame.rows);
    }
    
    for(int i = 0; i < num_boxes; ++i) {
        boxes.get()[i].sizeBox();
        boxes.get()[i].drawBox(frame);
    }
}
// blend with image 
void ac::ImageX(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 8.0;
        static cv::Mat frame_blend = blend_image.clone();
        for(int i = 0; i < frame.cols-1; ++i) {
            for(int z = 0; z < frame.rows-1; ++z) {
                int cX = AC_GetFX(frame_blend.cols, i, frame.cols);
                int cY = AC_GetFZ(frame_blend.rows, z, frame.rows);
                
                if(cX >= frame_blend.cols || cY >= frame_blend.rows)
                    continue;
                
                cv::Vec3b &pixel = blend_image.at<cv::Vec3b>(cY, cX);
                cv::Vec3b pix = blend_image.at<cv::Vec3b>(cY+1, cX+1);
                pixel = pix;
                cv::Vec3b &pix_value = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pix_value[j] = static_cast<unsigned char>(pixel[j]+(pix_value[j]*alpha));
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}

void ac::RandomQuads(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int j = 0; j < 4; ++j) {
        unsigned int frame_index = 0;
        do {
            frame_index = rand()%28;
        } while(frame_index == 13 || frame_index == 14);
        ac::draw_func[frame_index](collection.frames[j]);
    }
    cv::Size quarter(frame.cols/2, frame.rows/2);
    ac::copyMat(collection.frames[0],0, 0, frame, ac::Rect(0, 0, quarter));
    ac::copyMat(collection.frames[1],frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    ac::copyMat(collection.frames[2],frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    ac::copyMat(collection.frames[3],0, frame.rows/2, frame, ac::Rect(0,frame.rows/2, quarter));
}

void ac::QuadCosSinMultiply(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    cv::Size quarter(frame.cols/2, frame.rows/2);
    DrawFunction procFunc = ac::draw_func[15];
    procFunc(frame_copy);
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, 0, frame, ac::Rect(0, 0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, frame.rows/2, frame, ac::Rect(0,frame.rows/2, quarter));
}

void ac::QuadRandomFilter(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    cv::Size quarter(frame.cols/2, frame.rows/2);
    unsigned int base_index = 0, index = 0;
    DrawFunction baseFilter = getRandomFilter(base_index);
    baseFilter(frame_copy);
    DrawFunction procFunc = getRandomFilter(index);
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, 0, frame, ac::Rect(0, 0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, frame.rows/2, frame,ac::Rect(0,frame.rows/2, quarter));
}

void ac::RollRandom(cv::Mat &frame) {
    SquareVertical_Roll(frame);
    unsigned int index = 0;
    DrawFunction rand_func = getRandomFilter(index);
    rand_func(frame);
}

void ac::AverageRandom(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    cv::Mat frame_copy = frame.clone(), frame_copy2 = frame.clone();
    unsigned int index = 0;
    DrawFunction func = getRandomFilter(index);
    func(frame_copy);
    func(frame_copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = frame_copy.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = frame_copy2.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] + pix1[j] + pix2[j];
                pixel[j] /= 1.5;
                pixel[j] = static_cast<unsigned char>(pixel[j] * alpha);
            }
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::HorizontalStripes(cv::Mat &frame) {
    static cv::Scalar value(1.0, 1.0, 1.0);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                double rval = rand()%10;
                double val = rval * 0.001;
                value[j] += val;
                if(value[j] > 255) value[j] = 0;
                pixel[j] = pixel[j] ^ static_cast<unsigned int>(value[j]);
            }
        }
    }
    MedianBlur(frame);
    BlendWithSource(frame);
    Bitwise_XOR(frame);
}

void ac::DiamondStrobe(cv::Mat &frame) {
    static double pos = 1.0;// set pos to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static unsigned int index1 = 0, index2 = 2;
    
    ++index1;
    if(index1 > 2) index1 = 0;
    ++index2;
    if(index2 > 2) index2 = 0;
    
    for(int z = 0; z < h; ++z) {// from top to bottom
        for(int i = 0; i < w; ++i) {// from left to right
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);// get current pixel
            // calculate the colors of the gradient diamonds
            if((i%2) == 0) {// if i % 2 equals 0
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(1-pos*buffer[0]);
                    buffer[index2] = static_cast<unsigned char>((i+z)*pos);
                } else {
                    // set pixel coomponent values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[index2] = static_cast<unsigned char>((i-z)*pos);
                }
            } else {
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-i);
                    buffer[index2] = static_cast<unsigned char>((i-z)*pos);
                } else {
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[index2] = static_cast<unsigned char>((i+z)*pos);
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    // static direction starts off with 1
    static double pos_max = 7.0f;// pos maximum
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::SmoothTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    Smooth(frame, &collection);
}


void ac::Dual_SelfAlphaRainbow(cv::Mat &frame) {
    static double alpha1 = 2.0, alpha2 = 10.0, pos_max = 10.0;
    static bool  switch_on = true;
    rainbowBlend(frame);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(switch_on == true) {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha1);
            } else {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha2);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
        switch_on = !switch_on;
    }
    static int direction[2] = { 1, 0 };
    procPos(direction[0], alpha1, pos_max);
    procPos(direction[1], alpha2, pos_max);
}

void ac::Dual_SelfAlphaBlur(cv::Mat &frame) {
    Dual_SelfAlphaRainbow(frame);
    MedianBlur(frame);
    MedianBlur(frame);
    Bitwise_XOR(frame);
}

void ac::SurroundPixelXor(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows-3; ++z) {
        for(int i = 0; i < frame.cols-3; ++i) {
            cv::Vec3b pix[3];
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix[0] = frame.at<cv::Vec3b>(z+1, i);
            pix[1] = frame.at<cv::Vec3b>(z, i+1);
            pix[2] = frame.at<cv::Vec3b>(z+1, i+1);
            cv::Scalar value;
            value[0] = pix[0][0]+pix[1][0]+pix[2][0];
            value[1] = pix[0][1]+pix[1][1]+pix[2][1];
            value[3] = pix[0][2]+pix[1][2]+pix[2][2];
            for(int j = 0; j < 3; ++j) {
                unsigned int val = static_cast<unsigned int>(value[j]);
                pixel[j] = static_cast<unsigned char>((val^pixel[j])*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::Darken(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] /= 6;
            pixel[1] /= 6;
            pixel[2] /= 6;
        }
    }
}

void ac::WeakBlend(cv::Mat &frame) {
    static unsigned int index = 0;
    static cv::Scalar value((rand()%5)+1,(rand()%5)+1,(rand()%5)+1);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned int val = pixel[j]+(pixel[j]*value[index]);
                val /= 2;
                pixel[j] = static_cast<unsigned char>(val);
            }
            index ++;
            if(index > 2)
                index = 0;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    for(int j = 0; j < 3; ++j) {
        value[j] += 1+((rand()%5) * 0.5);
        if(value[j] > 10) {
            value[j] = rand()%10;
        }
    }
}

void ac::AverageVertical(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    std::unique_ptr<cv::Scalar[]> values(new cv::Scalar[frame.rows]);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                values.get()[z][j] += pixel[j];
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar val = values.get()[z];
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                val[j] /= frame.rows;
                pixel[j] += static_cast<unsigned char>(val[j]*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}


void ac::RandomCollectionAverage(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    unsigned int index = 0;
    DrawFunction randF = getRandomFilter(index);
    randF(frame);
    Smooth(frame, &collection);
}

void ac::RandomCollectionAverageMax(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    unsigned int index = 0;
    DrawFunction randF = getRandomFilter(index);
    randF(frame);
    Smooth(frame, &collection);
}

void ac::SmoothTrailsSelfAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    SelfAlphaBlend(frame);
    Smooth(frame, &collection);
}

void ac::SmoothTrailsRainbowBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    rainbowBlend(frame);
    Smooth(frame, &collection);
}

void ac::MedianBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = rand()%10;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int j = 0; j < collection.size()-1; ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += pixel[q];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned int val = 1+static_cast<unsigned int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::SmoothRandomImageBlend(cv::Mat &frame) {
    if(blend_set == true) {
        static MatrixCollection<8> collection;
        unsigned int index = 0;
        DrawFunction rfunc = getRandomFilter(index);
        cv::Mat temp_frame;
        cv::resize(blend_image, temp_frame, frame.size());
        rfunc(temp_frame);
        collection.shiftFrames(temp_frame);
        Smooth(frame, &collection);
    }
}

void ac::SmoothImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 2.0;
        static MatrixCollection<8> collection;
        cv::Mat temp_frame;
        cv::Mat temp_image;
        cv::Mat blend_image_scaled;
        cv::resize(blend_image, blend_image_scaled, frame.size());
        temp_frame = frame.clone();
        AlphaBlend(temp_frame,blend_image_scaled,frame,alpha);
        collection.shiftFrames(frame);
        Smooth(temp_frame, &collection);
        frame = temp_frame;
        static int direction = 1;
        procPos(direction, alpha, alpha_max, 8, 0.05);
    }
}

void ac::RandomAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    double alpha = 1.0, alpha_max = 6.0;
    unsigned int index = 0;
    DrawFunction randFunc = getRandomFilter(index);
    cv::Mat temp = frame.clone(), rand_frame = frame.clone();
    randFunc(rand_frame);
    collection.shiftFrames(rand_frame);
    AlphaBlend(temp,rand_frame, frame, alpha);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::RandomTwoFilterAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 5.0;
    static MatrixCollection<8> collection;
    cv::Mat one, two, output;
    one = frame.clone();
    two = frame.clone();
    unsigned int index = 0;
    DrawFunction randFunc1 = getRandomFilter(index);
    DrawFunction randFunc2 = getRandomFilter(index);
    randFunc1(one);
    randFunc2(two);
    AlphaBlend(one, two, output, alpha);
    collection.shiftFrames(output);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::PixelatedSquare(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 3.0;
    cv::Mat filter_1 = frame.clone();
    cv::Mat frame_copy;
    SurroundPixelXor(filter_1);
    GridFilter16x(filter_1);
    
    AlphaBlend(filter_1,frame,frame_copy,alpha);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::AlphaBlendPosition(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    unsigned int pos_x = 0;
    for(int z = 0; z < frame.rows; ++z) {
        cv::Vec3b pix = frame.at<cv::Vec3b>(z, pos_x);
        ++pos_x;
        if(pos_x > frame.cols-1) pos_x = 0;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]*alpha)+(pix[j]*alpha));
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::BlendRowAlpha(cv::Mat &frame) {
    static int row = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ row)*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::BlendRow(cv::Mat &frame) {
    static int row = 0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ row);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
}

void ac::BlendRowByVar(cv::Mat &frame) {
    static int row = 0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]+(z-i)) ^ row);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
}

void ac::BlendRowByDirection(cv::Mat &frame) {
    static int row = 0;
    static int direction = 1;
    for(int i = 0; i < frame.cols; ++i) {
        if(direction == 1) {
            ++row;
            if(row > frame.cols)
                direction = 0;
        } else if(direction == 0) {
            --row;
            if(row <= 0) {
                direction = 1;
            }
        }
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                if(direction == 1)
                    pixel[j] = static_cast<unsigned char>((pixel[j]+i+z) & row);
                else if(direction == 0)
                    pixel[j] = static_cast<unsigned char>((pixel[j]^row));
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::BlendAlphaXor(cv::Mat &frame) {
    static int r = 3;
    static int direction = 1;
    static double alpha = 1.0, alpha_max = 10.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned char val = static_cast<unsigned char>(r*alpha);
                pixel[j] += cv::saturate_cast<unsigned char>(pixel[j]^val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    if(direction == 1) {
        ++r;
        if(r >= 255)
            direction = 0;
    } else if(direction == 0) {
        --r;
        if(r <= 3)
            direction = 1;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::SelfXorScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    static unsigned int value = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^value)*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int direction = 1;
    if(direction == 1)  {
        ++value;
        if(value > 254)
            direction = 0;
    } else if(direction == 0) {
        --value;
        if(value <= 1)
            direction = 1;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::BitwiseXorScale(cv::Mat &frame) {
    static cv::Mat frame1 = frame.clone();
    cv::Mat temp = frame.clone();
    
    if(frame1.size()!=frame.size())
        frame1 = temp.clone();
    
    static double alpha = 1.0, alpha_max = 2.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    frame1 = temp;
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::XorTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b value;
            for(unsigned int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q) {
                    value[q] ^= frame_val[q];
                }
            }
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^value[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::RainbowTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b value;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);;
            for(unsigned int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q) {
                    value[q] += frame_val[q];
                }
            }
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^value[j];
            }
        }
    }
}
void ac::NegativeTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 8.0;
    collection.shiftFrames(frame);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b value;
            for(unsigned int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q) {
                    pixel[q] ^= pixel[q]+frame_val[q];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::IntenseTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 8.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b value = pixel;
            for(unsigned int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(unsigned int q =0; q < 3; ++q) {
                    value[q] += static_cast<unsigned char>(frame_val[q]*alpha);
                    
                }
            }
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] ^= value[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::SelfAlphaRGB(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    static int index = 0;
    for(int i = 0; i < frame.cols-2; ++i) {
        for(int z = 0;  z < frame.rows-2; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[4];
            pix[0] = frame.at<cv::Vec3b>(z+1, i+1);
            pix[1] = frame.at<cv::Vec3b>(z+1, i);
            pix[2] = frame.at<cv::Vec3b>(z, i+1);
            pix[3] = pixel;
            switch(index) {
                case 0:
                    pixel[0] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 1:
                    pixel[2] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[0] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 2:
                    pixel[1] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[0] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 3:
                    pixel[0] = pixel[0]^static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = pixel[1]^static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = pixel[2]^static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
    ++index;
    if(index > 3) index = 0;
}

void ac::BlendImageOnOff(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 4.0;
        static int index = 0;
        for(int z = 3; z < frame.rows-3; ++z) {
            for(int i = 3; i < frame.cols-3; ++i) {
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b pix[4];
                pix[0] = blend_image.at<cv::Vec3b>(cY, cX);
                pix[1] = blend_image.at<cv::Vec3b>(cY+1, cX);
                pix[2] = blend_image.at<cv::Vec3b>(cY, cX+1);
                pix[3] = blend_image.at<cv::Vec3b>(cY+1, cX+1);
                cv::Scalar value;
                for(unsigned int j = 0; j < 4; ++j) {
                    for(unsigned int q = 0; q < 3; ++q) {
                        value[q] += pix[j][q];
                    }
                }
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int j = 0; j < 3; ++j) {
                    value[j] /= 4;
                    unsigned char val = static_cast<unsigned char>(value[j]);
                    switch(index) {
                        case 0:
                            pixel[j] += static_cast<unsigned char>(val*alpha);
                            break;
                        case 1:
                            pixel[j] -= static_cast<unsigned char>(val*alpha);
                            break;
                    }
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
        ++index;
        if(index > 1) index = 0;
        
        static int dir = 1;
        procPos(dir, alpha, alpha_max);
    }
}

void ac::XorSelfAlphaImage(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 2.0;
        static double alpha_r = 14.0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b pix = blend_image.at<cv::Vec3b>(cY, cX);
                for(unsigned int j = 0; j < 3; ++j) {
                    //pixel[j] ^= (1-((pixel[j] + pix[j])) * (2+static_cast<unsigned char>(alpha)));
                    pixel[j] = static_cast<unsigned char>(pixel[j] * alpha) + (pix[j] * alpha_r);
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
        static int dir = 1, dir_r = 0;
        procPos(dir, alpha, alpha_max);
        procPos(dir_r, alpha_r, alpha_max);
    }
}

void ac::BitwiseXorStrobe(cv::Mat &frame) {
    static int index = 0;
    static double alpha1 = 1.0, alpha2 = 10.0, alpha3 = 5.0,alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
                case 1:
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
                case 2:
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir1 = 1, dir2 = 0, dir3 = 1;
    procPos(dir1, alpha1, alpha_max);
    procPos(dir2, alpha2, alpha_max);
    procPos(dir3, alpha3, alpha_max);
    
    ++index;
    if(index > 2) index = 0;
    Bitwise_XOR(frame);
}


void ac::AlphaBlendRandom(cv::Mat &frame) {
    static double val = 0.30;
    static int val_dir = 1;
    unsigned int index = 0;
    DrawFunction func[2];
    func[0] = getRandomFilter(index);
    func[1] = getRandomFilter(index);
    cv::Mat copy[4];
    copy[0] = frame.clone();
    copy[1] = frame.clone();
    copy[2] = frame.clone();
    func[0](copy[0]);
    func[1](copy[1]);
    AlphaBlend(copy[0], copy[1], copy[3], 0.5);
    AlphaBlend(copy[2], copy[3], frame, val);
    if(val_dir == 1) {
        val += 0.05;
        if(val >= 1.0)
            val_dir = 0;
    } else {
        val -= 0.05;
        if(val <= 0.30)
            val_dir = 1;
    }
}

void ac::ChannelSortAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    static unsigned int index = 0;
    std::vector<cv::Mat> v; // to hold the Matrix for split
    cv::split(frame, v);// split the channels into seperate matrices
    cv::Mat channels[3]; // output channels
    cv::Mat output; // for merge
    cv::sort(v[0], channels[0],cv::SORT_ASCENDING); // sort each matrix
    cv::sort(v[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v[2], channels[2],cv::SORT_ASCENDING);
    cv::sort(v[index], channels[index], cv::SORT_DESCENDING);
    cv::merge(channels, 3, output);
    ++index;
    if(index > 2) index = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pixadd = output.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
            	//pixel += pixadd;
                pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pixadd[j] * alpha));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.01);
}

void ac::XorChannelSort(cv::Mat &frame) {
    std::vector<cv::Mat> v; // to hold the Matrix for split
    cv::split(frame, v);// split the channels into seperate matrices
    cv::Mat channels[3]; // output channels
    cv::Mat output; // for merge
    cv::sort(v[0], channels[0],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::sort(v[1], channels[1],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::sort(v[2], channels[2],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::merge(channels, 3, output);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b sorted = output.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] = pixel[j] ^ sorted[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::GradientColors(cv::Mat &frame) {
    static unsigned int index = 0;
    static unsigned char val = 0;
    unsigned int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
    	for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = val;
            if((z%inc) == 0)
                ++val;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::GradientColorsVertical(cv::Mat &frame) {
    static unsigned int index = 0;
    static unsigned char val = 0;
    unsigned int inc = (frame.cols/255)+1;
    for(int z = 0; z < frame.rows; ++z) {
        val = 1;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = val;
         
            if((i%inc) == 0)
                ++val;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::Bitwise_XOR_Average(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    for(int z = 0; z < frame.rows; ++z) {
        cv::Scalar sval;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j) {
                sval[j] += pixel[j];
            }
        }
        
        for(unsigned int j = 0; j < 3; ++j)
            sval[j] /= frame.cols;
        
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] = (pixel[j] * alpha) + (static_cast<unsigned char>(sval[j])*alpha);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
     }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 0.01);
    Bitwise_XOR(frame);
}

void ac::NotEqual(cv::Mat &frame) {
    
    static MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 3.0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            bool same_value =  true;
            cv::Vec3b value;
            for(unsigned int j = 0; j < collection.size(); ++j) {
                value = collection.frames[j].at<cv::Vec3b>(z, i);
                if(value != pixel) {
                    same_value = false;
                    break;
                }
            }
            if(same_value == false) {
                for(unsigned int j = 0; j < 3; ++j) {
                    pixel[j] = (pixel[j] * alpha) + (value[j] * alpha);
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 0.10);
}

void ac::ImageShiftUpLeft(cv::Mat &frame) {
    if(blend_set) {
        static double alpha = 1.0, alpha_max = 3.0;
        static cv::Mat image = blend_image.clone();
        if(reset_filter == true) {
            reset_filter = false;
            image = blend_image.clone();
        }
        for(int i = 0; i < image.cols-1; ++i) {
            for(int z = 0; z < image.rows-1; ++z) {
                cv::Vec3b val = image.at<cv::Vec3b>(z+1, i+1);
                cv::Vec3b &target = image.at<cv::Vec3b>(z, i);
                target = val;
            }
        }
        for(int i = 0; i < frame.cols; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                int cX = AC_GetFX(image.cols, i, frame.cols);
                int cY = AC_GetFZ(image.rows, z, frame.rows);
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b src_pixel = image.at<cv::Vec3b>(cY, cX);
                for(unsigned int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j])) ^ src_pixel[j];
                
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
        static int dir = 1;
        procPos(dir, alpha, alpha_max);
    }
}

void ac::GradientXorSelfScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame.at<cv::Vec3b>(z+1, i+1);
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j] * alpha)) ^ pix[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.03);
}

void ac::SmoothSourcePixel(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            unsigned int total = pixel[0]+pixel[1]+pixel[2]/3;
            for(unsigned int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(((pixel[j] ^ total) * static_cast<unsigned char>(alpha)));
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
}

void ac::StrobeBlend(cv::Mat &frame) {
    unsigned int value1 = ((frame.cols/2)/255)+1;
    unsigned int num = 1, num2 = 1;
    static double alpha = 1.0, alpha_max = 8.0;
    static int index1 = 0, index2 = 2;
    static int frame_num = 0;
    
    ++frame_num;
    if(frame_num > 1) frame_num = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        num = 1;
        num2 = 1;
    	for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if((i%value1)==0)
                num++;
            if(frame_num == 0) {
            	for(unsigned int j = 0; j < 3; ++j) {
                	pixel[j] = pixel[j] ^ static_cast<unsigned char>(num);
            	}
            } else {
                for(unsigned int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num/(alpha+1));
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            //pixel[index1]= 255;
        }
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if((i%value1)==0)
                num2++;
            if(frame_num == 0) {
            	for(unsigned int j = 0; j < 3; ++j) {
                	pixel[j] = pixel[j] ^ static_cast<unsigned char>(num2);
            	}
            } else {
                for(unsigned int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num2/(alpha+1));
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            //pixel[index2] = 0;
        }
    }
    if(++index1 > 2) index1 = 0;
    if(--index2 < 0) index2 = 2;
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.03);
}

void ac::FrameBars(cv::Mat &frame) {
    unsigned int diff_i = (frame.cols/255)+1;
    unsigned int diff_z = (frame.rows/255)+1;
    unsigned char val[2] = {0,0};
    static double alpha = 1.0, alpha_max = 8.0;
    static MatrixCollection<4> collection;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] += static_cast<unsigned char>(val[0]*alpha) + static_cast<unsigned char>(val[1]*alpha);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            
            if((i%diff_i) == 0) {
                val[0]++;
            }
        }
        if((z%diff_z) == 0) {
            val[1]++;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
}

void ac::Sort_Vertical_Horizontal(cv::Mat &frame) {
    cv::Mat value = frame.clone();
    VerticalChannelSort(value);
    ChannelSort(value);
    Add(frame, value, false);
    
}

void ac::Sort_Vertical_Horizontal_Bitwise_XOR(cv::Mat &frame) {
    cv::Mat value = frame.clone();
    VerticalChannelSort(value);
    ChannelSort(value);
    static bool sub = false;
    if(sub == false)
        Add(frame, value, false);
    else
        Sub(frame, value, false);
    sub = (sub == true) ? false : true;
    Bitwise_XOR(frame);
}


template<typename Func>
void ac::Transform(const cv::Mat &source, cv::Mat &output, Func func) {
    
    if(output.empty() || output.size() != source.size())
        output.create(source.size(), CV_8UC3);
    
    for(int z = 0; z < source.rows; ++z) {
        for(int i = 0; i < source.cols; ++i) {
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            cv::Vec3b value = source.at<cv::Vec3b>(z, i);
            func(value, i, z);
            pixel = value;
            ac::swapColors(output, z, i);
            if(isNegative) ac::invert(output, z, i);
        }
    }
}

void ac::Scalar_Average_Multiply(cv::Mat &frame) {
    cv::Mat copy = frame.clone();
    VerticalChannelSort(frame);
    cv::Scalar average;
    ScalarAverage(frame, average);
    Transform(copy, frame, [&](cv::Vec3b &pixel, int i, int z) {
        for(unsigned int j = 0; j < 3; ++j) {
            pixel[j] *= average[j];
        }
    });
    cv::Mat out = frame.clone();
    AlphaBlend(out, copy, frame, 0.5);
}

void ac::Scalar_Average(cv::Mat &frame) {
    cv::Mat copy = frame.clone();
    cv::Scalar value;
    ScalarAverage(frame, value);
    Transform(copy, frame,[&](cv::Vec3b &pixel, int i, int z) {
        for(unsigned int j = 0; j < 3; ++j)
            pixel[j] *= value[j];
    });
    cv::Mat out = frame.clone();
    AlphaBlend(copy, out, frame, 0.5);
}

void ac::Total_Average(cv::Mat &frame) {
    cv::Mat frames[3];
    static double alpha = 1.0, alpha_max = 8.0;
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    frames[2] = frame.clone();

    SelfScale(frames[0]);
    rainbowBlend(frames[1]);
    SelfAlphaRGB(frames[2]);
    
    unsigned long total[3];
    
    for(unsigned int j = 0; j < 3; ++j)
    	TotalAverageOffset(frames[j], total[j]);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int j = 0; j < 3; ++j)
                pixel[j] += (alpha*total[j]);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
}

void ac::AlphaBlendImageXor(cv::Mat &frame) {
    if(blend_set == true) {
        static MatrixCollection<8> collection;
    	SmoothImageAlphaBlend(frame);
    	Bitwise_XOR(frame);
        collection.shiftFrames(frame);
        Smooth(frame, &collection);
    }
}

void ac::FlashWhite(cv::Mat &frame) {
    static cv::Vec3b white(255,255,255);
    static bool state = false;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(state)
                frame.at<cv::Vec3b>(z, i) = white;
        }
    }
    state = (state == true) ? false : true;
}

void ac::FlashBlackAndWhite(cv::Mat &frame) {
    static int index = 0;
    static cv::Vec3b white(255,255,255), black(0,0,0);
    if(index != 1 && index != 3) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                switch(index) {
                    case 0:
                        frame.at<cv::Vec3b>(z, i) = white;
                        break;
                    case 2:
                        frame.at<cv::Vec3b>(z, i) = black;
                }
            }
        }
    }
    ++index;
    if(index > 3) index = 0;
}

template<int Size>
void ac::Smooth(cv::Mat &frame, MatrixCollection<Size> *collection) {
    collection->shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar test;
            for(int q = 0; q < collection->size()-1; ++q) {
                cv::Mat &framev = collection->frames[q];
                cv::Vec3b pix = framev.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    test[j] += pix[j];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                test[j] /= (collection->size()-1);
                pixel[j] = cv::saturate_cast<unsigned char>(test[j]);
            }
            ac::swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
}

// No Filter
void ac::NoFilter(cv::Mat &) {}

// Alpha Blend with Original Frame
void ac::BlendWithSource(cv::Mat &frame) {
    ac::pass2_alpha = 0.50; // set to 50%
    Pass2Blend(frame);// call Pass2 function
}

