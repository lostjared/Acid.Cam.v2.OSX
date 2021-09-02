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

// glitchSort
// takes cv::Mat reference
void ac::glitchSort(cv::Mat &frame) {
    static double pos = 1.0f; // static pos set to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// static vector of int
    v.reserve(w);// reserve at least w bytes
    for(int z = 0; z < h; ++z) {// top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            // grab current pixel value reference
            cv::Vec3b &value = pixelAt(frame,z, i);
            // temporary int variable
            unsigned int vv = 0;
            // pointer to unsigned char * of vv variable
            unsigned char *cv = (unsigned char*)&vv;
            // set RGB values
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            v.push_back(vv);// push back into vector
        }
        if(!v.empty())
            std::sort(v.begin(), v.end());// sort the row of pixels
        for(int i = 0; i < w; ++i) {// left to right
            // pointer to integer stored at index i
            unsigned char *value = (unsigned char*)&v[i];
            // grab current pixel reference as cv::Vec3b
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // alphablend pixel with values from v at index i
            pixel[0] = static_cast<unsigned char>(pixel[0] + (pos)*value[0]);
            pixel[1] = static_cast<unsigned char>(pixel[1] + (pos)*value[1]);
            pixel[2] = static_cast<unsigned char>(pixel[2] + (pos)*value[2]);
            // swap the colors
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i); // if isNegative invert pixel
            
        }
        if(!v.empty())
            v.erase(v.begin(), v.end());// erase pixel data
    }
    static double pos_max = 7.0f;// pos_max = 7.0
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

// takes cv::Mat reference
void ac::pixelSort(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// static vector of int
    v.reserve(w);// reserve w bytes
    for(int z = 0; z < h; ++z) { // top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            //int value = frame.at<int>(z, i);
            // grab pixel reference
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
        if(!v.empty())
            std::sort(v.begin(), v.end());
        
        for(int i = 0; i < w; ++i) {// left to right
            // unsigned char pointer of vector v at index i
            unsigned char *value = (unsigned char*)&v[i];
            // get pixel reference
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // add to pixel without scaling
            pixel[0] = value[0];
            pixel[1] = value[1];
            pixel[2] = value[2];
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// invert pixel
        }
        if(!v.empty())
            v.erase(v.begin(), v.end());
    }
    
}
// preform a random filter
void ac::randomFilter(cv::Mat &frame) {
    /*
    int index = rand()%(svDistort.size()/4);
    if(index >= 0 && index < svDistort.size() && index != ac::draw_offset) {
        CallFilter(svDistort[index], frame);
    }*/
    SelfAlphaBlend(frame);
}

void ac::randomFlash(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static double pos = 1.0;// pos index
    // a random red,green,blue value
    int random_r = rand()%255, random_g = rand()%255, random_b = rand()%255;
    // top to bottom
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {// left to right
            // get pixel reference
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // calculate RGB values
            pixel[0] += static_cast<unsigned char>(pos*random_r);
            pixel[1] += static_cast<unsigned char>(pos*random_g);
            pixel[2] += static_cast<unsigned char>(pos*random_b);
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static double pos_max = 7.0f;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}



// alpha flame filters
// a collection of filters
void ac::alphaFlame(cv::Mat &frame) {
    static double pos = 1.0f;// pos set to 1
    double count = 1.0f;// count set to 1
    static int i = 0, z = 0;// i,z variables
    width = frame.cols;// frame width
    height = frame.rows;// frame height
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            // grab pixel reference as cv::Vec3b
            cv::Vec3b &buffer = pixelAt(frame,i, z);
            // call change pixel function
            changePixel(frame, z, i, buffer, pos, &count);
        }
    }
    
    static int dir = 1;
    AlphaMovementMaxMin(pos, dir, 0.1, 512, 1);
    // static direction set to 1
   /* static int direction = 1;
    if(direction == 1) {// if direction is equal to 1
        pos += 0.1f;// pos plus equal 0.1
        if(pos > 512) {// pos greater than 512
            pos = 512;// pos equal 512
            direction = 0;// direction equals 0
        }
    }
    else {
        pos -= 0.1f; // pos minus equal 0.1
        if(pos < 1) {// if pos less than 1
            pos = 1;// pos equal 1
            direction = 1;// direction set back to 1
        }
    }
    resetAlpha(direction, pos); */
}

// Resize X variable
int AC_GetFX(int oldw,int x, int nw) {
    float xp = (float)x * (float)oldw / (float)nw;
    return (int)xp;
}
// Resize Y Variable
int AC_GetFZ(int oldh, int y, int nh) {
    float yp = (float)y * (float)oldh / (float)nh;
    return (int)yp;
}


// preform GaussianBlur
void ac::GaussianBlur(cv::Mat &frame) {
#ifdef _WIN32
    cv::Mat copy = frame.clone();
    cv::GaussianBlur(copy, frame, cv::Size(5, 5), 0, 0);
#else
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::GaussianBlur(blur, out, cv::Size(5, 5), 0, 0);
    out.copyTo(frame);
#endif
}
// preform MedianBlur
void ac::MedianBlur(cv::Mat &frame) {
#ifdef _WIN32
    cv::Mat copy = frame.clone();
    cv::medianBlur(copy, frame, 5);
#else
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::medianBlur(blur, out, 5);
    out.copyTo(frame);
#endif
}
// Increase / Decrease GaussianBlur
// takes cv::Mat reference
void ac::BlurDistortion(cv::Mat &frame) {
    cv::Mat out;// output
    static int index = 1, direction = 1;
    cv::GaussianBlur(frame, out, cv::Size(index, index), 0, 0);// output
    if(direction == 1) {// if direction equals 1
        if(index >= 51) direction = 0;// if greater than 51 set to zero go
        // opposite direction
        else index += 2;// increase
    } else {
        if(index <= 1) direction = 1;// go opposite direction
        else index -= 2;// decrease
    }
    frame = out.clone();// frame equals out
}

// Draw gradient diamonds that grow and shrink and blend with source image
// takes cv::Mat reference
void ac::DiamondPattern(cv::Mat &frame) {
    static double pos = 1.0;// set pos to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    for(int z = 0; z < h; ++z) {// from top to bottom
        for(int i = 0; i < w; ++i) {// from left to right
            cv::Vec3b &buffer = pixelAt(frame,z, i);// get current pixel
            // calculate the colors of the gradient diamonds
            if((i%2) == 0) {// if i % 2 equals 0
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[0] = static_cast<unsigned char>(1-pos*buffer[0]);
                    buffer[2] = static_cast<unsigned char>((i+z)*pos);
                } else {
                    // set pixel coomponent values
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[2] = static_cast<unsigned char>((i-z)*pos);
                }
            } else {
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-i);
                    buffer[2] = static_cast<unsigned char>((i-z)*pos);
                } else {
                    // set pixel component values
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[2] = static_cast<unsigned char>((i+z)*pos);
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
// Mirror blend
// blend current pixel in loop with current pixel
// on opposite side of image (width-x), (height-y)
// then increase / decrease the pixel colors
// takes cv::Mat reference
void ac::MirrorBlend(cv::Mat &frame) {
    static double pos = 1.0; // pos set to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Mat orig;// unaltered image
    orig = frame.clone();// clone to orig
    for(int z = 2; z < h-3; ++z) { // from top to bottom
        for(int i = 2; i < w-3; ++i) {// from left to right
            cv::Vec3b &buffer = pixelAt(frame,z, i); // get pixel at i,z
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));// get pixel at w-i, h-z
            // set pixel rgb components
            buffer[0] += static_cast<unsigned char>(pix1[0]*pos);
            buffer[1] += static_cast<unsigned char>(pix1[1]*pos);
            buffer[2] += static_cast<unsigned char>(pix1[2]*pos);
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i); // invert if isNegative true
        }
    }
    // static direction variable
    static int direction = 1;
    double inc_double = (alpha_increase != 0) ? alpha_increase : 0.1;
    
    static double pos_max = 2.0f; // position maximum
    if(direction == 1) {// if direction is equal to 1
        pos += inc_double;// pos plus equal 0.1
        if(pos > pos_max) {// pos greater than pos max
            pos = pos_max;// pos = pos max
            direction = 0;// direction equals 0
            pos_max += 1.0f;// pos max pluse qual 1.0
        }
    } else if(direction == 0) {// if direction equals zero
        pos -= inc_double;// pos plus equal 0.1
        if(pos <= 1.0) {// pos less than 1.0
            if(pos_max > 2.0f) pos_max = 1.0f;// pos max greater than 2, pos_max set to 1
            direction = 1;// direction set back to 1
        }
    }
    resetAlpha(direction, pos);
}

// Pulse color in and out
// takes cv::Mat reference
void ac::Pulse(cv::Mat &frame) {
    static double pos = 1.0; // index
    int w = frame.cols;// width variable
    int h = frame.rows;// height variable
    for(int z = 0; z < h; ++z) { // from top to bottom
        for(int i = 0; i < w; ++i) { // from left to right
            // current pixel reference cv::Vec3b
            cv::Vec3b &buffer = pixelAt(frame,z, i);
            // pixel rgb components plus equal multiplied by pos
            buffer[0] += static_cast<unsigned char>(buffer[0]*pos);
            buffer[1] += static_cast<unsigned char>(buffer[1]*pos);
            buffer[2] += static_cast<unsigned char>(buffer[2]*pos);
            // swap colors
            swapColors(frame, z, i);
            // if negative variable true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1; // current direction
    static double pos_max = 3.0f; // maximum
    if(direction == 1) { // direction equals 1
        pos += 0.1; // pos plus equal 0.1
        if(pos > pos_max) {// pos greater than pos max
            pos = pos_max;// pos equals pox max
            direction = 0; // direction is zero
            pos_max += 1.0f; // pos max plus equal 1.0
        }
    } else if(direction == 0) { // direction is 0
        pos -= 0.1; // pos minus equal 0.1
        if(pos <= 1.0) { // less thane equal 1
            // reset pos max
            if(pos_max > 3.0f) pos_max = 1.0f;
            direction = 1; // direction set to 1
        }
    }
    resetAlpha(direction, pos);
}

// Sideways Mirror function
// takes reference cv::Mat (an image)
void ac::SidewaysMirror(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;// frame image width
    int h = frame.rows;// frame image height
    cv::Mat orig;// unaltered image matrix
    orig = frame.clone();// clone frame to orig
    for(int z = 2; z < h-3; ++z) {// loop from top to bottom
        for(int i = 2; i < w-3; ++i) {// loop each row from left
            // to right
            // current pixel
            cv::Vec3b &buffer = pixelAt(frame,z, i);
            // h minus y, width minus x positioned pixel
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            // y and width minus x pixel
            cv::Vec3b &pix2 = orig.at<cv::Vec3b>(z, (w-i));
            // current pixel compponents equal
            // pix1[0] plus pix2[0] multiplied by kernel
            buffer[0] += static_cast<unsigned char>((pix1[0]+pix2[0])*pos);
            // do the same for each component
            buffer[1] += static_cast<unsigned char>((pix1[1]+pix2[1])*pos);
            buffer[2] += static_cast<unsigned char>((pix1[2]+pix2[2])*pos);
            // swap colors
            swapColors(frame, z, i);
            // if negative flag set invert frame
            if(isNegative) invert(frame, z, i);
        }
    }
    // max size
    static double pos_max = 4.0f;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

// Mirror function without blending
void ac::MirrorNoBlend(cv::Mat &frame) {
    int w = frame.cols; // width of frame
    int h = frame.rows; // height of frame
    cv::Mat orig;// original image unaltered
    orig = frame.clone(); // clone the frame to orig
    for(int z = 2; z < h-3; ++z) { // loop through the height
        for(int i = 2; i < w-3; ++i) {// go across each row
            cv::Vec3b &buffer = pixelAt(frame,z, i);// current pixel
            // opposite of current pixel
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            // opposite width, same height
            cv::Vec3b &pix2 = orig.at<cv::Vec3b>(z, (w-i));
            // opposite height, same width
            cv::Vec3b &pix3 = orig.at<cv::Vec3b>((h-z), i);
            // current pixel components equal
            // add each pixel value together
            buffer[0] = (pix1[0]+pix2[0]+pix3[0]);
            buffer[1] = (pix1[1]+pix2[1]+pix3[1]);
            buffer[2] = (pix1[2]+pix2[2]+pix3[2]);
            // swap RGB positions
            swapColors(frame, z, i);
            // if the negative switch is on, invert
            if(isNegative) invert(frame, z, i);
        }
    }
}
// Sort the Fuzz
void ac::SortFuzz(cv::Mat &frame) {
    int r = rand()%255; // random number betwen 0-254
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// vector for row of bytes info
    v.reserve(w);// reserve at least width bytes
    for(int z = 0; z < h; ++z) { //  loop: top to bottom
        for(int i = 0; i < w; ++i) { // loop: left ro right
            cv::Vec3b &value = pixelAt(frame,z, i); // current pixel
            unsigned int vv = 0; // integer
            unsigned char *cv = (unsigned char*)&vv; // pointer to unsigned char*
            // set each byte
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            v.push_back(vv); // push back
        }
        std::sort(v.begin(), v.end());// sort greater
        for(int i = 0; i < w; ++i) { // left to right
            unsigned char *value = (unsigned char*)&v[i];
            cv::Vec3b &pixel = pixelAt(frame,z, i);// pixel at i,z
            // pixel values plus equal value plus r
            pixel[0] += static_cast<unsigned char>(value[0]+r);
            pixel[1] += static_cast<unsigned char>(value[1]+r);
            pixel[2] += static_cast<unsigned char>(value[2]+r);
            // swap colors
            swapColors(frame, z, i);
            // if negative variable set invert pixel
            if(isNegative) invert(frame, z, i);
            
        }
        v.erase(v.begin(), v.end()); // erase row
        // repeat
    }
}
// Fuzz filter
// takes cv::Mat reference
void ac::Fuzz(cv::Mat &frame) {
    int w = frame.cols;// width of frame
    int h = frame.rows;// height of frame
    static int amount = 5; // num pixel distortion
    for(int z = 0; z < h; ++z) {// loop top to bottom
        for(int i = 0; i < w; ++i) { // loop from left ro gith
            if((rand()%amount)==1) {// if random is true
                cv::Vec3b &pixel = pixelAt(frame,z, i);// grab pixel
                pixel[0] += rand()%255;// add random numbers
                pixel[1] += rand()%255;
                pixel[2] += rand()%255;
            }
            // swap colors
            swapColors(frame, z, i);
            // if negative invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // direction equals 1 to start
    static int direction = 1;
    if(direction == 1) {// if direction equals 1
        ++amount; // increase amount
        if(amount >= 10) direction = 0; // greater than ten lower to zero
    } else {
        --amount;// decrease amount
        if(amount <= 5) direction = 1;// less than five direction equals 1
    }
}

// Double vision
// takes cv::Mat by refrence
void ac::DoubleVision(cv::Mat &frame) {
    static double pos = 1.0; // index
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Mat orig = frame.clone(); // clone frame to orig
    for(int z = 3; z < h-3; ++z) {// top to bottom
        for(int i = 3; i < w-3; ++i) { // left to right
            // current pixel
            cv::Vec3b &buffer = pixelAt(frame,z, i);
            cv::Vec3b &g = orig.at<cv::Vec3b>((h-z), i); // pixel at h-y, x
            cv::Vec3b &b = orig.at<cv::Vec3b>(z, (w-i)); // pixel at y, w-x
            // this is what gives the diamond image
            if((i%2) == 0) {// if modulus i by two returns zero
                if((z%2) == 0) {// modulus z by two returns zero
                    buffer[2] += static_cast<unsigned char>((i+z)*pos);// buffer[2] plus equals (i plus z) multiplied by pos
                } else {
                    buffer[2] += static_cast<unsigned char>((i-z)*pos); // buffer[2] plus equals (i minus z) mulitplied by pos
                }
            } else {
                if((z%2) == 0) {// modulus z by two equals zero
                    buffer[2] += static_cast<unsigned char>((i-z)*pos); // buffer[2] plus equals (i minus z) multiplied by pos
                } else {
                    buffer[2] += static_cast<unsigned char>((i+z)*pos); // buffer[2] plus equals (i plus z) multiplied by pos
                }
            }
            // this is what adds the rgb from other positions
            buffer[0] += g[0];
            buffer[1] += b[1];
            // swap colors
            swapColors(frame, z, i);
            // if negative variable set invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // static int direction
    // pos max
    static double pos_max = 7.0f;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// RGB Shift
// takes cv::Mat reference
void ac::RGBShift(cv::Mat &frame) {
    int w = frame.cols; // frame width
    int h = frame.rows;// frame height
    cv::Mat orig = frame.clone();// clone frame to orig
    static int shift = 0;// shift equals 0
    for(int z = 3; z < h-3; ++z) {// top to bottom
        for(int i = 3; i < w-3; ++i) {// left to right
            // grab pixel values
            cv::Vec3b &buffer = pixelAt(frame,z, i);
            cv::Vec3b &g = orig.at<cv::Vec3b>((h-z), i);
            cv::Vec3b &b = orig.at<cv::Vec3b>(z, (w-i));
            cv::Vec3b &r = orig.at<cv::Vec3b>((h-z), (w-i));
            // switch shift, each state preforms addition on different
            // pixel component values
            switch(shift) {
                case 0:
                    buffer[0] += r[0];
                    buffer[1] += g[1];
                    buffer[2] += b[2];
                case 1:
                    buffer[0] += g[0];
                    buffer[1] += b[1];
                    buffer[2] += r[2];
                    break;
                case 2:
                    buffer[0] += b[0];
                    buffer[1] += r[1];
                    buffer[2] += g[2];
                    break;
            }
            swapColors(frame, z, i);// swap the colors
            if(isNegative) invert(frame, z, i);// invert current pixel
        }
    }
    ++shift;// increase shift
    if(shift > 2) shift = 0;// shift greater than two reset shift
}
// RGB Seperation
// takes cv::Mat
void ac::RGBSep(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Mat orig = frame.clone();// orig is clone of frame
    for(int z = 3; z < h-3; ++z) {// top to bottom
        for(int i = 3; i < w-3; ++i) {// left to right
            // grab pixel values
            cv::Vec3b &buffer = pixelAt(frame,z, i);
            cv::Vec3b &g = orig.at<cv::Vec3b>((h-z), i);
            cv::Vec3b &b = orig.at<cv::Vec3b>(z, (w-i));
            // set pixel values
            buffer[0] += g[0];
            buffer[2] += b[2];
            swapColors(frame, z, i); // swap colors
            if(isNegative) invert(frame, z, i); // invert pixel
        }
    }
}
// Gradient Rainbow
// takes cv::Mat reference
void ac::GradientRainbow(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    // start color double
    double start_color = (1+(rand()%255))* 0.5;
    
    for(int z = 0; z < h; ++z) { // top to bottom
        for(int i = 0; i < w; ++i) {// left to right
            // reference to current pixel
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // color RGB variables
            int color_R = static_cast<int>(start_color * 4), color_G = static_cast<int>(start_color * 6), color_B = static_cast<int>(start_color * 8);
            // add to pixel color
            pixel[0] += static_cast<unsigned char>(color_R);
            pixel[1] += static_cast<unsigned char>(color_G);
            pixel[2] += static_cast<unsigned char>(color_B);
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        start_color += 0.1;// increase start_color
    }
}
// Flash
// takes cv::Mat
void ac::GradientRainbowFlash(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static double pos = 0.1;
    static int shift = 0;
    // start color double
    double start_color = (1+(rand()%255)) * pos;
    for(int z = 0; z < h; ++z) { // top to bottom
        for(int i = 0; i < w; ++i) {// left to right
            // reference to current pixel
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // color RGB variables
            int color_R = static_cast<int>(start_color * 4), color_G = static_cast<int>(start_color * 6), color_B = static_cast<int>(start_color * 8);
            // add to pixel colors
            pixel[2] += static_cast<unsigned char>(color_R);
            pixel[1] += static_cast<unsigned char>(color_G);
            pixel[0] += static_cast<unsigned char>(color_B);
            // flash
            if(shift == 0) {
                pixel[2] = ~pixel[2];
                pixel[1] = ~pixel[1];
                pixel[0] = ~pixel[0];
            }
            
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        start_color += 0.050;// increase start_color
    }
    shift = !shift;
    // static int direction
    static int direction = 1;
    // pos max
    // if direction equals 1
    if(direction == 1) {
        pos += 0.05; // pos plus equal 0.05
        if(pos > 1.0) { // if pos > pos max
            pos = 1.0;
            direction = 0;// direction equals 0
        }
    } else if(direction == 0) {// direction equals 1
        pos -= 0.05;// pos -= 0.05
        if(pos <= 0.1) {// if pos <= 1.0
            // set to 1.0
            direction = 1;// set direction back to 1
            pos = 0.1;
        }
    }
    resetAlpha(direction, pos);
}
// Reverse Frame
// takes cv::Mat reference
void ac::Reverse(cv::Mat &frame) {
    cv::Mat output;//output matrix
    cv::flip(frame, output, 1); // flip image
    frame = output.clone(); // set frame to output
}
// Scanlines - Draws scanlines like a CRT.
void ac::Scanlines(cv::Mat &frame) {
    int w = frame.cols;// width
    int h = frame.rows;// height
    for(int z = 0; z < h; z += 2) {// top to bottom step by 2
        for(int i = 0; i < w; ++i) {// left to right
            cv::Vec3b &pix = pixelAt(frame,z, i);// current pixel
            pix[0] = pix[1] = pix[2] = 0;// set to zero
        }
    }
}
// Random Pixels
void ac::TVStatic(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static int dir = 0;
    for(int z = dir; z < h; z += 2) {// top to bottom step by 2 pixels
        for(int i = 0; i < w; ++i) {// left to right
            cv::Vec3b &pix = pixelAt(frame,z, i);// current pixel
            if(rand()%2>0) {
                pix[0] = pix[1] = pix[2] = 0;
            } else {
                pix[0] = pix[1] = pix[2] = 255;
            }
        }
    }
    ++dir;
    if(dir >= 2) dir = 0;
}
// Mirror Average
// takes cv::Mat reference
void ac::MirrorAverage(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Mat orig = frame.clone(); // clone original frame (make a copy)
    static double pos = 1.0f; // current index
    for(int z = 1; z < h-1; ++z) { // top to bottom
        for(int i = 1; i < w-1; ++i) {// left to right
            // refernce to current pixel located at i,z
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b mir_pix[3]; // array of Vec3b variables
            mir_pix[0] = orig.at<cv::Vec3b>((h-z), (w-i)); // pixel at w-i, h-z
            mir_pix[1] = orig.at<cv::Vec3b>((h-z), i); // pixel at i, h-z
            mir_pix[2] = orig.at<cv::Vec3b>(z,(w-i)); // pixel at w-i, z
            // take each component from mir_pix and find the average
            // with the same index from each variable in the mir_pix array
            // then multiply it by the position index (pos) then add it
            // to current pixel
            pixel[0] += static_cast<unsigned char>(((mir_pix[0][0]+mir_pix[1][0]+mir_pix[2][0])/3)*pos);
            pixel[1] += static_cast<unsigned char>(((mir_pix[0][1]+mir_pix[1][1]+mir_pix[2][1])/3)*pos);
            pixel[2] += static_cast<unsigned char>(((mir_pix[0][2]+mir_pix[1][2]+mir_pix[2][2])/3)*pos);
            
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // move up and down the color scale
    
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// Mirror Average Mix
// Takes cv::Mat matrix
void ac::MirrorAverageMix(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Mat orig = frame.clone(); // clone original frame
    static double pos = 1.0; // position index floating point
    for(int z = 1; z < h-1; ++z) { // loop from top to bottom
        for(int i = 1; i < w-1; ++i) { // loop from left to right
            cv::Vec3b &pixel = pixelAt(frame,z, i); // current pixel at i,z
            cv::Vec3b mir_pix[3]; // array of 3 cv::Vec3b vectors
            mir_pix[0] = orig.at<cv::Vec3b>((h-z), (w-i)); // pixel at w-i, h-z
            mir_pix[1] = orig.at<cv::Vec3b>((h-z), i); // pixel at i, h-z
            mir_pix[2] = orig.at<cv::Vec3b>(z,(w-i)); // pixel at w-i, z
            // take each pixel and average together mulitply by pos
            // and add its value to different components in
            // pixel reference vector
            pixel[0] += static_cast<unsigned char>(((mir_pix[0][0]+mir_pix[0][1]+mir_pix[0][2])/3)*pos);
            pixel[1] += static_cast<unsigned char>(((mir_pix[1][0]+mir_pix[1][1]+mir_pix[1][2])/3)*pos);
            pixel[2] += static_cast<unsigned char>(((mir_pix[2][0]+mir_pix[2][1]+mir_pix[2][2])/3)*pos);
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // pos max
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// Mean takes cv::Mat reference
void ac::Mean(cv::Mat &frame) {
    static double pos = 1.0; // position index floating point
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Scalar s = cv::mean(frame);
    for(int z = 0; z < h; ++z) { // from top to bottom
        for(int i = 0; i < w; ++i) {// from left to right
            cv::Vec3b &pixel = pixelAt(frame,z, i); // pixel at (i,z)
            // add to pixel values
            pixel[0] += static_cast<unsigned char>(pos*s[0]);
            pixel[1] += static_cast<unsigned char>(pos*s[1]);
            pixel[2] += static_cast<unsigned char>(pos*s[2]);
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // position movement
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

// Laplacian - takes cv::Mat reference
void ac::Laplacian(cv::Mat &frame) {
    cv::UMat uout, f = frame.getUMat(cv::ACCESS_FAST);
    cv::Laplacian(frame,uout, CV_8U);
    uout.copyTo(frame);
    AddInvert(frame);
}

// XOR - takes cv::Mat reference
void ac::Bitwise_XOR(cv::Mat &frame) {
    static cv::Mat initial;// set initial frame
    if(reset_filter == true || frames_released == true || initial.empty() || (initial.size() != frame.size())) {
        initial = frame.clone(); // did frame resize? if so set the new frame value
        reset_filter = false;
    }
    cv::Mat start = frame.clone(); // clone frame (make a copy)
    cv::Mat output = frame.clone();// output variable
    cv::bitwise_xor(frame, initial, output); // OpenCV function bitwise_and
    initial = start.clone();// set initial to start
    frame = output.clone(); // set frame to output
    AddInvert(frame);
}

// And takes cv::Mat reference
void ac::Bitwise_AND(cv::Mat &frame) {
    static cv::Mat initial;// set initial frame
    if(reset_filter == true || frames_released == true || initial.empty() || (initial.size() != frame.size())) {
        initial = frame.clone(); // did frame resize? if so set the new frame value
        reset_filter = false;
    }
    cv::Mat start = frame.clone(); // clone frame (make a copy)
    cv::Mat output = frame.clone();// output variable
    cv::bitwise_and(frame, initial, output); // OpenCV function bitwise_and
    initial = start.clone();// set initial to start
    frame = output.clone(); // set frame to output
    AddInvert(frame);
}
// takes cv::Mat reference
void ac::Bitwise_OR(cv::Mat &frame) {
    static cv::Mat initial;// set initial frame
    if(reset_filter == true || frames_released == true || initial.empty() || (initial.size() != frame.size())) {
        initial = frame.clone(); // did frame resize? if so set the new frame value
        reset_filter = false;
    }
    cv::Mat start = frame.clone(); // clone frame (make a copy)
    cv::Mat output = frame.clone();// output variable
    cv::bitwise_or(frame, initial, output); // OpenCV function bitwise_and
    initial = start.clone();// set initial to start
    frame = output.clone(); // set frame to output
    AddInvert(frame);
}
// takes cv::Mat reference
// Equalize image
void ac::Equalize(cv::Mat &frame) {
    cv::Mat output[3]; // array of cv::Mat
    std::vector<cv::Mat> v; // vector to hold cv::Mat values
    cv::split(frame, v);// split b,g,r values
    cv::equalizeHist(v[0], output[0]);// equalize
    cv::equalizeHist(v[1], output[1]);
    cv::equalizeHist(v[2], output[2]);
    cv::merge(output,3,frame);// merge back to create final output
    AddInvert(frame);
}

// Channel sort - takes cv::Mat reference
void ac::ChannelSort(cv::Mat &frame) {
    static double pos = 1.0; // color scale
    std::vector<cv::Mat> v; // to hold the Matrix for split
    cv::split(frame, v);// split the channels into seperate matrices
    cv::Mat channels[3]; // output channels
    cv::Mat output; // for merge
    cv::sort(v[0], channels[0],cv::SORT_ASCENDING); // sort each matrix
    cv::sort(v[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v[2], channels[2],cv::SORT_ASCENDING);
    
    cv::merge(channels, 3, output); // combine the matrices
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i); // get reference to pixel
                cv::Vec3b &ch_pixel = output.at<cv::Vec3b>(z, i); // get reference to pixel
                // add and multiply components to channels
                pixel[0] += static_cast<unsigned char>(ch_pixel[0]*pos);
                pixel[1] += static_cast<unsigned char>(ch_pixel[1]*pos);
                pixel[2] += static_cast<unsigned char>(ch_pixel[2]*pos);
                // swap colors
                swapColors(*frame, z, i);
                // if isNegative true invert pixel
                if(isNegative) invert(*frame, z, i);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    // pos max
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

