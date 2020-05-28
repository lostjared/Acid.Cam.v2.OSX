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
#include "fractal.h"


void ac::OppositeBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    cv::Mat temp = frame.clone();
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    
    static int index = 0;
    static double alpha = 1.0;
    static double pos_max = 14.0;
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int index = 0;
    cv::Mat temp = frame.clone();
    for(int z = 1; z < h-1; ++z) {
        for(int i = 1; i < w-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            switch(index) {
                case 0:
                    index++;
                    break;
                case 1: {
                    cv::Vec3b pix = pixelAt(frame,h-z-1, w-i-1);
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
    static int start_index = 0;
    int index = start_index;
    cv::Mat temp = frame.clone();
    for(int z = 1; z < h-1; ++z) {
        for(int i = 1; i < w-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            switch(index) {
                case 0:
                    index++;
                    break;
                case 1: {
                    cv::Vec3b pix = pixelAt(frame,h-z-1, w-i-1);
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
    static int start_index = 0;
    int index = start_index;
    for(int z = 3; z < h-3; ++z) {
        for(int i = 3; i < w-3; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int start_index = 0;
    static double alpha = 1.0, alpha_max = 10.0;
    int index = start_index;
    for(int z = 3; z < h-3; ++z) {
        for(int i = 3; i < w-3; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int flash = 1;
    if(flash == 1) {
        for(int z = 0; z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int b = static_cast<int>(blend);
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
    static int pos = 0;
    static unsigned char blend_pixel = 0;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[2] = pixel[1] = 0;
        }
    }
}
// set colors to zero based on counter
// increment counter after each nested loop
void ac::LineRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static int counter = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int counter = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i); // pixel
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
    static int rg = 0;// row counter start variable
    row_counter = rg; // set row counter to start
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int counter = 0; // counter for setting the pixel
    static int start = 0; // start position
    for(int z = start; z < h; z += 2) { // top to bottom, skipping 1 each time
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int increase_value_r = 2, increase_value_g = 2, increase_value_b = 2;
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int index = 0;
    static double alpha = 1.0, alpha_max = 9;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                switch(index) {
                    case 0:
                        pixel[j] = static_cast<unsigned char>((pixel[j] ^ (int)(alpha*z)));
                        break;
                    case 1:
                        pixel[j] = static_cast<unsigned char>((pixel[j] & (int)(alpha*i)));
                        break;
                    case 2:
                        pixel[j] = static_cast<unsigned char>((pixel[j] ^ (int)alpha));
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int index = 0;
    static double alpha = 1.0, alpha_max = 8;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    unsigned char average[3] = {0};
    static double alpha = 1.0, alpha_max = 11;
    for(int z = 0; z < h; ++z) {
        cv::Scalar s(1,1,1);
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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



void ac::ColorRange(cv::Mat &frame) {
    const int w = frame.cols;
    const int h = frame.rows;
    static double alpha = 1.0, alpha_max = 6;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]+colors[j])*alpha);
        }
    }
    static int direction[3] = {1, 0, 1};
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


void ac::TrailsInter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int counter = 0;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = collection.frames[counter+1].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter > 5) counter = 0;
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = collection.frames[counter].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter >= 5) counter = 0;
    }
}

void ac::TrailsNegate(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static int index = 0;
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = collection.frames[counter].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter >= 5) counter = 0;
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    int index = 0;
    cv::Mat frame_copy = frame.clone();
    for(int z = 2; z < h-2; ++z) {
        for(int i = 2; i < w-2; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int row = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    frames[0] = frame.clone();
    for(int j = 1; j < 3; ++j) {
        frames[j] = frame.clone();
        randomFilter(frames[j]);
    }
    const int w = frame.cols, h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static int index = 0;
    int w = frame.cols, h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = collection.frames[counter].at<cv::Vec3b>(z, i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++counter;
        if(counter >= 3) counter = 0;
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

