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

// takes cv::Mat reference
void ac::Reverse_XOR(cv::Mat &frame) {
    static cv::Mat initial = frame;
    if(initial.cols != frame.cols || initial.rows != frame.rows) {
        initial = frame;
    }
    static double pos = 1.0;
    cv::Mat start = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &in_pixel = initial.at<cv::Vec3b>(z, i);
            pixel[0] ^= in_pixel[2];
            pixel[1] ^= in_pixel[1];
            pixel[2] ^= in_pixel[0];
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    initial = start;
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

// takes cv::Mat reference
void ac::CombinePixels(cv::Mat &frame) {
    static double pos = 1.0, pos_max = 7.0;
    static int direction = 1;
    cv::Scalar s(1.0, 100.0, 200.0);
    for(int z = 2; z < frame.rows-2; ++z) {
        for(int i = 2; i < frame.cols-2; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pixels[4];
            pixels[0] = pixelAt(frame,z, i+1);
            pixels[1] = pixelAt(frame,z+1, i);
            pixels[2] = pixelAt(frame,z+1, i+1);
            pixel[0] ^= (pixels[0][0]+pixels[1][0]+pixels[2][0]);
            pixel[1] ^= (pixels[0][1]+pixels[1][1]+pixels[2][1]);
            pixel[2] ^= (pixels[0][2]+pixels[1][2]+pixels[2][2]);
            pixel[0] *= static_cast<unsigned char>(pos);
            pixel[1] *= static_cast<unsigned char>(pos);
            pixel[2] *= static_cast<unsigned char>(pos);
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    procPos(direction, pos, pos_max);
}
// Canny takes cv::Mat reference
void ac::Canny(cv::Mat &frame) {
    cv::Mat out;
    static double x = 100, y = 100;
    cv::Canny(frame, out, x, y);
    cv::Mat converted;
    cv::cvtColor(out, converted, cv::COLOR_GRAY2BGR);
    frame = converted.clone();
    AddInvert(frame);
}

// Flip takes cv::Mat reference
// flip the iamge every other frame
void ac::FlipTrip(cv::Mat &frame) {
    static int _flip = 0;// index variable
    cv::Mat output;// output matrix
    switch(_flip){
        case 0:
            cv::flip(frame, output, 1); // flip matrix
            frame = output.clone();// frame equals output
            _flip++;// increase index
            break;
        case 1:
            // do nothing
            _flip = 0; // index equals zero
            break;
    }
}

// Loop tiled boxes
void ac::Boxes(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int pixel_size = 8; // size of each tile
    for(int z = 0; z < h; z += pixel_size) { // from top to bottom
        for(int i = 0; i < w; i += pixel_size) { // from left to right
            unsigned char rgb[3]; // 3 bytes
            rgb[0] = rand()%255; // set to random number
            rgb[1] = rand()%255;
            rgb[2] = rand()%255;
            for(int y = z; y < static_cast<int>(z+pixel_size); ++y) { // tile top to bottom
                for(int x = i; x < static_cast<int>(i+pixel_size); ++x) {// tile left to right
                    if(x < w && y < h) { // is x,y on screen?
                        // reference to pixel
                        cv::Vec3b &pixel = pixelAt(frame,y, x);
                        pixel[0] += rgb[0]; // add each component
                        pixel[1] += rgb[1];
                        pixel[2] += rgb[2];
                    }
                }
            }
        }
    }
    static int direction = 1; // current direction, grow versus shrink
    if(direction == 1) {
        ++pixel_size;// grow by 1
        // if greater than 1/6 of frame size set to zero
        if(static_cast<int>(pixel_size) > (w/6)) direction = 0;
    } else if(direction == 0) {// direction equals zero shrink
        --pixel_size;// shrink
        if(pixel_size < 24) direction = 1;
    }
}
// Loop tiled box fade
void ac::BoxesFade(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static cv::Vec3b color(rand()%255, rand()%255, rand()%255); // random color
    static int sw = 0; // with component to increase
    static int pixel_size = 8; // size of each tile
    for(int z = 0; z < h; z += pixel_size) { // from top to bottom
        ++sw;// increase
        if(sw > 2) sw = 0;//greater than 2 reset
        for(int i = 0; i < w; i += pixel_size) { // from left to right
            for(int y = z; y < z+pixel_size; ++y) { // tile top to bottom
                for(int x = i; x < i+pixel_size; ++x) {// tile left to right
                    if(x < w && y < h) { // is x,y on screen?
                        // reference to pixel
                        switch(sw) {
                            case 0: // increase B
                                ++color[0];
                                break;
                            case 1://  increase G
                                ++color[1];
                                break;
                            case 2:// increase R
                                ++color[2];
                                break;
                        }
                        if(color[0] >= 254) color[0] = rand()%255; // reset if over
                        if(color[1] >= 254) color[1] = rand()%255;
                        if(color[2] >= 254) color[2] = rand()%255;
                        
                        cv::Vec3b &pixel = pixelAt(frame,y, x);
                        pixel[0] += color[0]; // add each component
                        pixel[1] += color[1];
                        pixel[2] += color[2];
                    }
                }
            }
        }
    }
    static int direction = 1; // current direction, grow versus shrink
    if(direction == 1) {
        ++pixel_size;// grow by 1
        // if greater than 1/6 of frame size set to zero
        if(pixel_size > (w/6)) direction = 0;
    } else if(direction == 0) {// direction equals zero shrink
        --pixel_size;// shrink
        if(pixel_size < 24) direction = 1;
    }
}


void ac::FlashBlack(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static cv::Vec3b black(0, 0, 0);
    static bool flash = false;
    for(int z = 0; z < h; ++z) {
        for(int i = 0;  i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(flash == true)
                pixel = black;
            
        }
    }
    flash = !flash;
}

void ac::SlideRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int offset_x = 0;
    int color[2] = { rand()%3, rand()%3 };
    for(int z = 3; z < h-3; ++z) {
        for(int i = 3; i < w-3; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(offset_x+i < (w-1)) {
                cv::Vec3b off_pix = pixelAt(frame,z, offset_x+i);
                pixel[color[0]] += static_cast<unsigned char>(off_pix[color[0]]);
                cv::Vec3b off_red = pixelAt(frame,z, (w-(offset_x+i)));
                pixel[color[1]] += static_cast<unsigned char>(off_red[color[1]]);
            }
        }
    }
    static int direction = 1;
    if(direction == 1) {
        ++offset_x;
        if(offset_x > 5) {
            direction = 0;
        }
    } else {
        --offset_x;
        if(offset_x <= 1) {
            direction = 1;
        }
    }
}
// Blend from Side to Side
void ac::Side2Side(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 3.0;
    for(int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            total[0] += (pixel[0]/2);
            total[1] += (pixel[1]/2);
            total[2] += (pixel[2]/2);
            pixel[0] = static_cast<unsigned char>(pixel[0] + (total[0]*pos)*0.01);
            pixel[1] = static_cast<unsigned char>(pixel[1] + (total[1]*pos)*0.01);
            pixel[2] = static_cast<unsigned char>(pixel[2] + (total[2]*pos)*0.01);
            
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// Blend from Top To Bottom
void ac::Top2Bottom(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 5.0;
    for(int i = 0; i < w; ++i) {
        cv::Scalar total;
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            total[0] += (pixel[0]/2);
            total[1] += (pixel[1]/2);
            total[2] += (pixel[2]/2);
            pixel[0] = static_cast<unsigned char>(pixel[0] + (total[0]*pos)*0.01);
            pixel[1] = static_cast<unsigned char>(pixel[1] + (total[1]*pos)*0.01);
            pixel[2] = static_cast<unsigned char>(pixel[2] + (total[2]*pos)*0.01);
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::StrobeRedGreenBlue(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static unsigned color = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            switch(color) {
                case 0: // B
                    pixel[1] = pixel[2] = 0;
                    break;
                case 1:// G
                    pixel[0] = pixel[2] = 0;
                    break;
                case 2:// R
                    pixel[0] = pixel[1] = 0;
            }
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    ++color;
    if(color > 2) color = 0;
}

void ac::Blend_Angle(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 5.0;
    for(int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            total[0] += pixel[0] * 0.01;
            total[1] += pixel[1] * 0.01;
            total[2] += pixel[2] * 0.01;
            
            pixel[0] = static_cast<unsigned char>(pixel[0] + (total[0]) * (pos*0.1));
            pixel[1] = static_cast<unsigned char>(pixel[1] + (total[1]) * (pos*0.1));
            pixel[2] = static_cast<unsigned char>(pixel[2] + (total[2]) * (pos*0.1));
            
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    
    static int direction = 1;
    procPos(direction, pos,pos_max);
}

void ac::Outward(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static double start_pos = 1, pos = 1.0, pos_max = 5.0;
    
    static cv::Scalar offset(5, 50, 100);
    
    pos = start_pos;
    
    for(int y = h/2; y > 0; --y) {
        for(int x = 0; x < w; ++x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
            
        }
        pos += 0.005;
    }
    
    pos = start_pos;
    
    for(int y = h/2+1; y < h; ++y) {
        for(int x = 0; x < w; ++x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
        }
        pos += 0.005;
    }
    
    offset[0] += 12;
    offset[1] += 6;
    offset[2] += 3;
    
    for(int i = 0; i < 3; ++i) if(offset[i] > 200) offset[i] = 0;
    
    static int direction = 1;
    procPos(direction, start_pos, pos_max);
}

void ac::OutwardSquare(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    int wx = w/2;
    static double start_pos = 1, pos = 1.0, pos_max = 5.0;
    static cv::Scalar offset(5, 50, 100);
    pos = start_pos;
    
    for(int y = h/2; y > 0; --y) {
        for(int x = 0; x < wx; ++x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
        }
        pos += 0.005;
    }
    //pos = start_pos;
    for(int y = h/2; y > 0; --y) {
        for(int x = w-1; x > wx-1; --x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
        }
        pos += 0.005;
    }
    
    pos = start_pos;
    for(int y = h/2+1; y < h; ++y) {
        for(int x = 0; x < wx; ++x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
        }
        pos += 0.005;
    }
    //pos = start_pos;
    for(int y = h/2+1; y < h; ++y) {
        for(int x = w-1; x > wx-1; --x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] += static_cast<unsigned char>((pos*offset[0]));
            pixel[1] += static_cast<unsigned char>((pos*offset[1]));
            pixel[2] += static_cast<unsigned char>((pos*offset[2]));
            // swap colors
            swapColors(frame, y, x);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, y, x);
        }
        pos += 0.005;
    }
    offset[0] += 12;
    offset[1] += 6;
    offset[2] += 3;
    for(int i = 0; i < 3; ++i) if(offset[i] > 200) offset[i] = 0;
    static int direction = 1;
    procPos(direction, start_pos, pos_max);
}

void ac::ShiftPixels(cv::Mat &frame) {
    static int offset = 1;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    for(int z = 0; z < h; ++z) {
        int start = 0;
        for(int i = offset; i < w && start < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
            pixel[0] += source[0];
            pixel[1] += source[1];
            pixel[2] += source[2];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(int i = 0; i < offset-1 && start < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
            pixel[0] += source[0];
            pixel[1] += source[1];
            pixel[2] += source[2];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static int max_up = (w/16);
    if(direction == 1) {
        ++offset;
        if(offset > max_up)  {
            direction = 0;
            max_up += 4;
            if(max_up > (w/4)) {
                max_up = (w/16);
            }
        }
    } else if(direction == 0) {
        --offset;
        if(offset < 2) direction = 1;
    }
}

void ac::ShiftPixelsDown(cv::Mat &frame) {
    static int offset = 1;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    for(int i = 0; i < w; ++i) {
        int start = 0;
        for(int z = offset; z < h && start < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
            pixel[0] += static_cast<unsigned char>(source[0]*pos);
            pixel[1] += static_cast<unsigned char>(source[1]*pos);
            pixel[2] += static_cast<unsigned char>(source[2]*pos);
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(int z = 0; z < offset-1 && start < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &source = pixelAt(frame,z, start);
            pixel[0] += static_cast<unsigned char>(source[0]*pos);
            pixel[1] += static_cast<unsigned char>(source[1]*pos);
            pixel[2] += static_cast<unsigned char>( source[2]*pos);
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static int max_up = (h/8);
    if(direction == 1) {
        ++offset;
        if(offset > max_up)  {
            direction = 0;
            max_up += 4;
            if(max_up > (h/2)) {
                max_up = (h/8);
            }
        }
    } else if(direction == 0) {
        --offset;
        if(offset < 2) direction = 1;
    }
    static int dir = 1;
    procPos(dir, pos, pos_max);
    
}

void ac::XorMultiBlend(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 3.0;
    cv::Scalar s(pos, -pos, pos);
    for(int y = h-1; y > 0; --y) {
        for(int x = w-1; x > 0; --x) {
            cv::Vec3b &pixel = pixelAt(frame,y, x);
            pixel[0] = static_cast<unsigned char>((pixel[0]^(int)s[0])*pos);
            pixel[1] = static_cast<unsigned char>((pixel[1]^(int)s[1])*pos);
            pixel[2] = static_cast<unsigned char>((pixel[2]^(int)s[2])*pos);
            
            swapColors(frame, y, x);
            if(isNegative) invert(frame, y, x);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max, 4.0);
}

void ac::BitwiseRotate(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int offset = 0;
    static int direction = 1;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            
            if(direction == 1) {
                pixel[0] = ror(pixel[0], offset);
                pixel[1] = rol(pixel[1], offset);
                pixel[2] = ror(pixel[2], offset);
            } else {
                pixel[0] = rol(pixel[0], offset);
                pixel[1] = ror(pixel[1], offset);
                pixel[2] = rol(pixel[2], offset);
            }
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    if(direction == 1) {
        offset++;
        if(offset >= 7) {
            direction = 0;
        }
    } else {
        offset--;
        if(offset <= 1) {
            direction = 1;
        }
    }
}

void ac::BitwiseRotateDiff(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int offset = 1;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int q = 0; q < 3; ++q)
                pixel[q] += static_cast<unsigned char>((pixel[q]-ror(pixel[q], offset)));
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++offset;
    if(offset > 7) offset = 1;
}

void ac::HPPD(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 4.0;
    for(int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            total[0] += pixel[0];
            total[1] += pixel[1];
            total[2] += pixel[2];
            pixel[0] = static_cast<unsigned char>(pixel[0]-total[0]*pos);
            pixel[1] = static_cast<unsigned char>(pixel[1]-total[1]*pos);
            pixel[2] = static_cast<unsigned char>(pixel[2]-total[2]*pos);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::FuzzyLines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 4.0;
    cv::Scalar prev_pixel;
    double value[3] = { 0 };
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b temp = pixel;
            
            value[0] += temp[0]+temp[1]+temp[2];
            value[1] -= temp[0]+temp[1]+temp[2];
            value[2] += temp[0]+temp[1]+temp[2];
            pixel[0] += static_cast<unsigned char>((value[0]*pos)*0.001);
            pixel[1] += static_cast<unsigned char>((value[1]*pos)*0.001);
            pixel[2] += static_cast<unsigned char>((value[2]*pos)*0.001);
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
            prev_pixel[0] = pixel[0];
            prev_pixel[1] = pixel[1];
            prev_pixel[2] = pixel[2];
        }
    }
    
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::GradientLines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int count = 0, index = 0;
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] += static_cast<unsigned char>(count);
            ++count;
            if(count >= 255) {
                count = 0;
                ++index;
                if(index > 2) index = 0;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::GradientSelf(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int count = 0, index = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = static_cast<unsigned char>((pixel[index]*pos)+count);
            ++count;
            if(count >= 255) {
                count = 0;
            }
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++index;
        if(index > 2) index = 0;
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::GradientSelfVertical(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int count = 0, index = 0;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = static_cast<unsigned char>((pixel[index]*pos)+count);
            ++count;
            if(count >= 255) {
                count = 0;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++index;
        if(index > 2) index = 0;
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::GradientDown(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int count = 0, index = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = static_cast<unsigned char>((pixel[index]*pos)+count);
            ++index;
            if(index > 2) index = 0;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++count;
        if(count >= 255) {
            count = 0;
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::GraidentHorizontal(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int count = 0, index = 0;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = static_cast<unsigned char>((pixel[index]*pos)+count);
            ++index;
            if(index > 2) index = 0;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++count;
        if(count >= 255) {
            count = 0;
        }
    }
    int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::GradientRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int count = 0, index = 0;
    static int direction = 1;
    if(direction == 1) {
        for(int z = 0; z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel[index] = pixel[index]*count;
                ++count;
                if(count >= 255) {
                    ++index;
                    if(index > 2) {
                        index = 0;
                    }
                }
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
        }
    } else {
        for(int i = 0; i < w; ++i) {
            for(int z = 0; z < h; ++z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel[index] = pixel[index]*count;
                ++count;
                if(count >= 255) {
                    ++index;
                    if(index > 2) {
                        index = 0;
                    }
                }
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
        }
    }
    if(direction == 1) direction = 0; else direction = 1;
}


void ac::Inter(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int start_x = 0;
    for(int z = start_x; z < h; z += 2) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = pixel[1] = pixel[2] = 0;
        }
    }
    if(start_x == 0) start_x = 1; else start_x = 0;
}


void ac::UpDown(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    double alpha = 1.0;
    bool order = true;
    
    for(int i = 0; i < w; ++i) {
        if(order == true) {
            order = false;
            for(int z = 0; z < h; ++z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int q = 0; q < 3; ++q)
                    pixel[q] = static_cast<unsigned char>(alpha+(pixel[q]*pos));
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            alpha += 0.1;
        } else {
            order = true;
            for(int z = h-1; z > 1; --z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int q = 0; q < 3; ++q)
                    pixel[q] =static_cast<unsigned char>(alpha-(pixel[q]*pos));
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            
            alpha += 0.1;
        }
        
    }
    
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::LeftRight(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    double alpha = 1.0;
    bool order = true;
    for(int z = 0; z < h; ++z) {
        if(order == true) {
            order = false;
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int q = 0; q < 3; ++q)
                    pixel[q] = static_cast<unsigned char>(alpha+(pixel[q]*pos));
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            alpha += 0.1;
        } else {
            order = true;
            for(int i = w-1; i > 1; --i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int q = 0; q < 3; ++q)
                    pixel[q] = static_cast<unsigned char>(alpha-(pixel[q]*pos));
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            
            alpha += 0.1;
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::StrobeScan(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int color_mode = 0;
    int over = rand()%255;
    static  int cdirection = 1;
    
    for(int z = 0; z < h; ++z) {
        switch(color_mode) {
            case 0: {
                for(int i = 0; i < w; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    pixel[color_mode] = static_cast<unsigned char>(over+(pixel[color_mode]*pos));
                    swapColors(frame, z, i);
                    if(isNegative) invert(frame, z, i);
                }
            }
                break;
            case 1: {
                for(int i = w-1; i > 1; --i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    pixel[color_mode] -= static_cast<unsigned char>(over+(pixel[1]*pos));
                    swapColors(frame, z, i);
                    if(isNegative) invert(frame, z, i);
                    
                }
            }
                break;
            case 2: {
                for(int i = 0; i < w; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    pixel[color_mode] ^= static_cast<unsigned char>(over+(pixel[color_mode]*pos));
                    swapColors(frame, z, i);
                    if(isNegative) invert(frame, z, i);
                    
                }
            }
                break;
        }
        
        if(cdirection == 1) {
            ++color_mode;
            if(color_mode > 2) {
                cdirection = 0;
            }
        } else if(cdirection == 0) {
            --color_mode;
            if(color_mode < 0) {
                cdirection = 1;
            }
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::BlendedScanLines(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int cnt = 0;
    for(int z = 0; z < h; ++z) {
        int r = rand()%255;
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[cnt] += static_cast<unsigned char>(r);
            ++r;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
            ++cnt;
            if(cnt > 2) {
                cnt = 0;
            }
        }
    }
}

void ac::GradientStripes(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int offset = 0, count = 0;
    int count_i = (rand()%0xFF)+(rand()%0xFFFFFF);//color offset
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[offset] += static_cast<unsigned char>(count);
            pixel[2-offset] -= static_cast<unsigned char>(count_i);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        ++count;
        ++count_i;
    }
    ++offset;
    if(offset > 2)
        offset = 0;
}
// this one pixelates the image very heavily.
void ac::XorSine(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static cv::Scalar val(rand()%10, rand()%10, rand()%10);
    static double pos = 1.0, pos_max = 7.0;
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] ^= static_cast<unsigned char>(sin(pixel[0])*val[0]);
            pixel[1] ^= static_cast<unsigned char>(sin(pixel[1])*val[1]);
            pixel[2] ^= static_cast<unsigned char>(sin(pixel[2])*val[2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    for(int q = 0; q < 3; ++q) {
        if(direction == 1)
            val[q] += pos;
        else
            val[q] -= pos;
    }
    procPos(direction, pos, pos_max);
}



void ac::Circular(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static double deg = 0.0;
    static double rad = 50;
    
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int X_color = int(rad * cos(deg));
            int Y_color = int(rad * sin(deg));
            pixel[0] += static_cast<unsigned char>(pos*X_color);
            pixel[1] *= static_cast<unsigned char>(pos);
            pixel[2] += static_cast<unsigned char>(pos*Y_color);
            deg += 0.1;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    rad += 0.5;
    if(rad > 100) rad = 50;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}


void ac::WhitePixel(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static int pixel_count = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            if(pixel_count == 4) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel[0] = pixel[1] = pixel[2] = 255;
                pixel_count = rand()%2;
            } else ++pixel_count;
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        pixel_count = rand()%2;
    }
}

void ac::FrameBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static cv::Mat stored_frame;
    if((frame.rows != stored_frame.rows) || (frame.cols != stored_frame.cols)) {
        stored_frame = frame.clone();
    }
    cv::Mat start = frame.clone();
    // process frame
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b old_pixel = pixelAt(stored_frame,z, i);
            pixel[0] += static_cast<unsigned char>((old_pixel[0]^pixel[0])*pos);
            pixel[1] += static_cast<unsigned char>((old_pixel[1]&pixel[1])*pos);
            pixel[2] += static_cast<unsigned char>((old_pixel[2]|pixel[2])*pos);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    stored_frame = start.clone();
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::FrameBlendRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static cv::Mat stored_frame;
    if(stored_frame.empty() || frame.size() != stored_frame.size()) {
        stored_frame = frame.clone();
    }
    cv::Mat start = frame.clone();
    static int swap = 0;
    // process frame
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b old_pixel = pixelAt(stored_frame,z, i);
            switch(swap) {
                case 0:
                    pixel[0] += static_cast<unsigned char>((old_pixel[0]^pixel[0])*pos);
                    pixel[1] += static_cast<unsigned char>((old_pixel[1]&pixel[1])*pos);
                    pixel[2] += static_cast<unsigned char>((old_pixel[2]|pixel[2])*pos);
                    break;
                case 1:
                    pixel[0] += static_cast<unsigned char>((old_pixel[0]&pixel[0])*pos);
                    pixel[1] += static_cast<unsigned char>((old_pixel[1]|pixel[1])*pos);
                    pixel[2] += static_cast<unsigned char>((old_pixel[2]^pixel[2])*pos);
                    break;
                case 2:
                    pixel[0] += static_cast<unsigned char>((old_pixel[0]|pixel[0])*pos);
                    pixel[1] += static_cast<unsigned char>((old_pixel[1]^pixel[1])*pos);
                    pixel[2] += static_cast<unsigned char>((old_pixel[2]&pixel[2])*pos);
                    break;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++swap;
    if(swap > 2) swap = 0;
    stored_frame = start.clone();
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::TrailsFilter(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            pixel[0] += (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0]);
            pixel[1] += (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1]);
            pixel[2] += (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::TrailsFilterIntense(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[0].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[5].at<cv::Vec3b>(z, i);
            pixel[0] += (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0]);
            pixel[1] += (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1]);
            pixel[2] += (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::TrailsFilterSelfAlpha(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double pos = 1.0, pos_max = 7.0;
    collection.shiftFrames(frame);
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            pixel[0] += static_cast<unsigned char>((frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0])*pos);
            pixel[1] += static_cast<unsigned char>((frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1])*pos);
            pixel[2] += static_cast<unsigned char>((frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2])*pos);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::TrailsFilterXor(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    static double pos = 1.0, pos_max = 7.0;
    collection.shiftFrames(frame);
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[12];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            pixel[0] ^= (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0]);
            pixel[1] ^= (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1]);
            pixel[2] ^= (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::ColorTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            for(int q = 0; q < 6; ++q) {
                if(frame_pixels[q][0] > pixel[0]) frame_pixels[q][0] = 0;
                if(frame_pixels[q][1] < pixel[1]) frame_pixels[q][1] = 0;
                if(frame_pixels[q][2] > pixel[2]) frame_pixels[q][2] = 0;
            }
            pixel[0] = (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0]);
            pixel[1] = (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1]);
            pixel[2] = (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::MoveRed(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    static int movement = 0;
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(i+movement < (w-1)) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i+movement));
                pixel[2] += static_cast<unsigned char>((add[2]*pos));
            } else if((i-movement) > 1) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i-movement));
                pixel[2] += static_cast<unsigned char>((add[2]*pos));
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++movement;
    if(movement > (w-1)) movement = 0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::MoveRGB(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    static int rgb = 0;
    static int movement = 0;
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(i+movement < (w-1)) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i+movement));
                pixel[rgb] += static_cast<unsigned char>((add[rgb]*pos));
            } else if((i-movement) > 1) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i-movement));
                pixel[rgb] += static_cast<unsigned char>((add[rgb]*pos));
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++rgb;
    if(rgb > 2) rgb = 0;
    ++movement;
    if(movement > (w-1)) movement = 0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::MoveRedGreenBlue(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0; // position in transition, maximum value
    static int movement[4] = {0, w, 0}; // movement variable array
    static int stored_w = w; // stored_w in case the frame size changes
    if(stored_w != w) {
        movement[1] = w-1; // set movement[1] to width
        stored_w = w; // stored_w set to new width
    }
    cv::Mat frame_copy = frame.clone(); // make a copy of the frame
    for(int z = 0; z < h; ++z) { // loop from top to bottom
        for(int i = 0; i < w; ++i) { // loop from left to right
            cv::Vec3b &pixel = pixelAt(frame,z, i); // reference to current pixel
            for(int q = 0; q <= 2; ++q) { // loop from 0 to 2
                int pos_x = i+movement[q];// pixel position
                int pos_y = i-movement[q];// pixel position
                if(pos_x < (w-1) && pos_x > 0) { // if within the screen
                    cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, pos_x); // grab pixel
                    pixel[q] += static_cast<unsigned char>((add[q]*pos)); // add to current index multiplied by position
                } else if(pos_y > 0 && pos_y < (w-1)) {// if pos y within the screen
                    cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, pos_y); // grab pixel
                    pixel[q] += static_cast<unsigned char>((add[q]*pos));// add to current index multiplied by position
                }
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    movement[0] += 4; // movement position increase by 4
    if(movement[0] > (w-1)) movement[0] = 0;
    movement[1] -= 4;// movement position decrease by 4
    if(movement[1] < 1) movement[1] = w-1; // set to width -1
    movement[2] += 8;// movement position increase by 8
    if(movement[2] > (w-1)) movement[2] = 0;// if greater than widthset to zero
    static int direction = 1;// direction of transition animation
    procPos(direction, pos, pos_max);// proc the position by increasing/decreasing
}

void ac::BlurSim(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b pixels[2][2];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            bool grabbed = true;
            for(int a = 0; a < 2; a++) {
                for(int b = 0; b < 2; b++) {
                    if((a+i) < (w-1) && (b+z) < (h-1)) {
                        pixels[a][b] = pixelAt(frame,z+b, i+a);
                    } else {
                        grabbed = false;
                        break;
                    }
                }
            }
            if(grabbed == false) continue;
            unsigned char rgb[3] = {0};
            for(int q = 0; q < 3; ++q)
                for(int a = 0; a < 2; ++a) {
                    for(int b = 0; b < 2; ++b) {
                        rgb[q] += pixels[a][b][q];
                    }
                }
            pixel[0] ^= static_cast<unsigned char>((rgb[0]/4)*pos);
            pixel[1] ^= static_cast<unsigned char>((rgb[1]/4)*pos);
            pixel[2] ^= static_cast<unsigned char>((rgb[2]/4)*pos);
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::Block(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static int square = 2;
    for(int z = 0; z < h; z += square) {
        for(int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int x = 0; x < square; ++x) {
                for(int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = pixelAt(frame,y+z, i+x);
                        pix = pixel;
                    }
                }
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    static int direction = 1;
    if(direction == 1) {
        square += 2;
        if(square >= 32) direction = 0;
    } else {
        square -= 2;
        if(square <= 2) direction = 1;
    }
}

void ac::BlockXor(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 3.0;
    static int square = 2;
    for(int z = 0; z < h; z += square) {
        for(int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int x = 0; x < square; ++x) {
                for(int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = pixelAt(frame,y+z, i+x);
                        pix[0] ^= static_cast<unsigned char>(pixel[0]*pos);
                        pix[1] ^= static_cast<unsigned char>(pixel[1]*pos);
                        pix[2] ^= static_cast<unsigned char>(pixel[2]*pos);
                        
                    }
                }
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
            
        }
    }
    static int direction = 1;
    if(direction == 1) {
        square += 2;
        if(square >= 8) direction = 0;
    } else {
        square -= 2;
        if(square <= 2) direction = 1;
    }
    static int posDirection = 1;
    procPos(posDirection, pos, pos_max);
}
// BlockScale
void ac::BlockScale(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 3.0;
    static int square = 2;
    for(int z = 0; z < h; z += square) { // loop from top to bottom
        for(int i = 0; i < w; i += square) { // loop from left to right
            cv::Vec3b &pixel = pixelAt(frame,z, i);// grab pixel value
            for(int x = 0; x < square; ++x) {// draw square from left to right
                for(int y = 0; y < square; ++y) {// draw square form top to bottom
                    if(y+z < h && i+x < w) {// within bounds?
                        cv::Vec3b &pix = pixelAt(frame,y+z, i+x); // grab pixel
                        pix[0] = static_cast<unsigned char>(pixel[0]*pos); // calculate values
                        pix[1] = static_cast<unsigned char>(pixel[1]*pos);
                        pix[2] = static_cast<unsigned char>(pixel[2]*pos);
                    }
                }
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    // move in/out direction
    static int direction = 1;
    if(direction == 1) {
        square += 2;
        if(square >= 8) direction = 0;
    } else {
        square -= 2;
        if(square <= 2) direction = 1;
    }
    static int posDirection = 1;
    procPos(posDirection, pos, pos_max);
}

void ac::BlockStrobe(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    static int square = 2;
    for(int z = 0; z < h; z += square) {
        for(int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int x = 0; x < square; ++x) {
                for(int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = pixelAt(frame,y+z, i+x);
                        pix[0] += static_cast<unsigned char>(pixel[0]*(x*y));
                        pix[1] += static_cast<unsigned char>(pixel[1]*(x*y));
                        pix[2] += static_cast<unsigned char>(pixel[2]*(x*y));
                    }
                }
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    static int direction = 1;
    if(direction == 1) {
        square += 2;
        if(square >= 8) direction = 0;
    } else {
        square -= 2;
        if(square <= 2) direction = 1;
    }
}

// Prev Frame Blend
// store previous frame and manipulate with current frame
void ac::PrevFrameBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    double pos = 1.0;
    static int old_w = w;
    static cv::Mat stored = frame.clone(), temp;
    temp = frame.clone();
    if(old_w != w) {
        stored = frame.clone();
        old_w = w;
    }
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b old_pixel = stored.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((pixel[0])+(1-old_pixel[0])*pos);
            pixel[1] = static_cast<unsigned char>((pixel[1])+(1-old_pixel[1])*pos);
            pixel[2] = static_cast<unsigned char>((pixel[2])+(1-old_pixel[2])*pos);
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    stored = temp;
    static int direction = 1;
    if(direction == 1) {
        pos += 0.1;
        if(pos > 7.0) direction = 0;
    } else {
        pos -= 0.1;
        if(pos <= 1.0) direction = 1;
    }
    resetAlpha(direction, pos);
    
}

class WavePoints {
public:
    WavePoints() : x1(0), x2(0), x1_dir(0), x2_dir(0),c_dir(0),color(0) {}
    int x1,x2;
    int x1_dir, x2_dir, c_dir;
    double color;
};


void ac::Wave(cv::Mat &frame) {
    static int width = 0, height = 0;
    // uses lazy allocation when frame is resized pointer is reallocated.
    // last deallocation is done when program exits so no need to manually release
    static std::unique_ptr<WavePoints[]> points;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    const int slice = (h/16);
    
    if(width != w || height != h) {
        points.reset(new WavePoints[w]);
        width = w;
        height = h;
        
        for(int i = 0; i < w; ++i) {
            points[i].x1 = rand()%slice;
            points[i].x2 = h-rand()%slice;
            points[i].color = rand()%13;
            points[i].x1_dir = 0;
            points[i].x2_dir = 0;
            points[i].c_dir = 0;
        }
    }
    for(int z = 0; z <h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(z >= points[i].x1 && z <= points[i].x2) {
                pixel[0] += static_cast<unsigned char>(pixel[0]*points[i].color);
                pixel[1] += static_cast<unsigned char>(pixel[1]*points[i].color);
                pixel[2] += static_cast<unsigned char>(pixel[2]*points[i].color);
            }
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    for(int i = 0; i < w; ++i) {
        // color direction
        if(points[i].c_dir == 0) {
            points[i].color += 0.1;
            if(points[i].color >= 10) {
                points[i].c_dir = 1;
            }
        } else if(points[i].c_dir == 1) {
            points[i].color -= 0.1;
            if(points[i].color <= 1) {
                points[i].c_dir = 0;
            }
        }
        
        // x1 point direction/move down and up
        if(points[i].x1_dir == 0) {
            points[i].x1 ++;
            if(points[i].x1 > slice) {
                points[i].x1_dir = 1;
            }
        } else if(points[i].x1_dir == 1) {
            points[i].x1--;
            if(points[i].x1 < 1) {
                points[i].x1_dir = 0;
            }
        }
        
        // x2 point up/down
        if(points[i].x2_dir == 0) {
            points[i].x2--;
            if(points[i].x2 < (h-slice)) {
                points[i].x2_dir = 1;
            }
        } else if(points[i].x2_dir == 1) {
            points[i].x2++;
            if(points[i].x2 > (h-4)) {
                points[i].x2_dir = 0;
            }
        }
    }
}

void ac::HighWave(cv::Mat &frame) {
    static int width = 0, height = 0;
    static std::unique_ptr<WavePoints[]> points;
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    const int slice = (h/8);
    
    if(width != w || height != h) {
        points.reset(new WavePoints[w]);
        width = w;
        height = h;
        
        for(int i = 0; i < w; ++i) {
            points[i].x1 = rand()%slice;
            points[i].x2 = h-rand()%slice;
            points[i].color = rand()%13;
            points[i].x1_dir = 0;
            points[i].x2_dir = 0;
            points[i].c_dir = 0;
        }
    }
    for(int z = 0; z <h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] -= static_cast<unsigned char>(pixel[0]*points[i].color);
            pixel[1] += static_cast<unsigned char>(pixel[1]*points[i].color);
            pixel[2] -= static_cast<unsigned char>(pixel[2]*points[i].color);
            swapColors(frame, z, i);// swap colors for rgb slides
            if(isNegative) invert(frame, z, i); // if is negative
        }
    }
    for(int i = 0; i < w; ++i) {
        // color direction
        if(points[i].c_dir == 0) {
            points[i].color += 0.25;
            if(points[i].color >= 10) {
                points[i].c_dir = 1;
            }
        } else if(points[i].c_dir == 1) {
            points[i].color -= 0.25;
            if(points[i].color <= 1) {
                points[i].c_dir = 0;
            }
        }
    }
}

void ac::VerticalSort(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// static vector of int
    v.reserve(w);// reserve w bytes
    for(int i = 0; i < w; ++i) { // top to bottom
        for(int z = 0; z < h; ++z) { // left to right
            //int value = frame.at<int>(z, i);
            // grab pixel reference
            
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
            
            cv::Vec3b &value = pixelAt(frame,z, i);
            unsigned int vv = 0;
            // unsigned char * of vv
            unsigned char *cv = (unsigned char*)&vv;
            // set RGB values
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            // push back into vector v
            v.push_back(vv);
        }
        // sort vector v
        std::sort(v.begin(), v.end());
        for(int q = 0; q < h; ++q) {// left to right
            // unsigned char pointer of vector v at index i
            unsigned char *value = (unsigned char*)&v[q];
            // get pixel reference
            cv::Vec3b &pixel = pixelAt(frame,q, i);
            // add to pixel without scaling
            pixel[0] = value[0];
            pixel[1] = value[1];
            pixel[2] = value[2];
            
        }
        v.erase(v.begin(), v.end());
    }
}

void ac::VerticalChannelSort(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    std::vector<unsigned char> pixels[3];
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
            
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixels[j].push_back(pixel[j]);
        }
        
        for(int j = 0; j < 3; ++j)
            std::sort(pixels[j].begin(), pixels[j].end());
        
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = pixels[0][z];
            pixel[1] = pixels[1][z];
            pixel[2] = pixels[2][z];
        }
        for(int j = 0; j < 3; ++j)
            if(!pixels[j].empty())
                pixels[j].erase(pixels[j].begin(), pixels[j].end());
    }
    
}

void ac::HorizontalBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double alpha[3] = {1,8,16};
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>(pixel[0] * alpha[0]);
            pixel[1] = static_cast<unsigned char>(pixel[1] * alpha[1]);
            pixel[2] = static_cast<unsigned char>(pixel[2] * alpha[2]);
            
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        if((rand()%4)==0) {
            for(int i = 0; i < 3; ++i) {
                alpha[i] += 0.1;
                if(alpha[i] > 25) alpha[i] = 1;
            }
        }
    }
}

void ac::VerticalBlend(cv::Mat &frame) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    static double alpha[3] = {1,8,16};
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>(pixel[0] * alpha[0]);
            pixel[1] = static_cast<unsigned char>(pixel[1] * alpha[1]);
            pixel[2] = static_cast<unsigned char>(pixel[2] * alpha[2]);
            
            swapColors(frame, z, i);// swap colors for rgb sliders
            if(isNegative) invert(frame, z, i); // if is negative
        }
        if((rand()%4)==0) {
            for(int i = 0; i < 3; ++i) {
                alpha[i] += 0.1;
                if(alpha[i] > 25) alpha[i] = 1;
            }
        }
    }
}

