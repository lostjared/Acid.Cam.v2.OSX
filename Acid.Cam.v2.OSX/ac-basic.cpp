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

#include"ac.h"
#include"fractal.h"

// SelfAlphaBlend - Perform out of Bounds AlphaBlend on source image
void ac::SelfAlphaBlend(cv::Mat &frame) {
    double alpha_inc = 0.1;
    if(alpha_increase != 0) {
        alpha_inc = alpha_increase;
    } else {
        alpha_inc = 0.1;
    }
    for(int z = 0; z < frame.rows; ++z) {// from top to bottom
        for(int i = 0; i < frame.cols; ++i) {// from left to right
            cv::Vec3b &colorval = pixelAt(frame,z, i);// at x,y
            colorval[0] += static_cast<unsigned char>(colorval[0]*alpha);
            colorval[1] += static_cast<unsigned char>(colorval[1]*alpha);
            colorval[2] += static_cast<unsigned char>(colorval[2]*alpha);
            swapColors(frame, z, i);// swap colors
            if(isNegative == true) { // if negative
                invert(frame, z, i);// invert
            }
        }
    }
    static int direction = 1;// direction equals 1
    if(direction == 1) {// if direction equals 1
        alpha += alpha_inc; // plus equal 0.1
        // if alpha greater than 10
        if(alpha > 10) { alpha = 10; direction = 2; }
    } else {
        alpha -= alpha_inc; // minus equal 0.05
        // if alpha <= 0.1f
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
    resetAlpha(direction, alpha);
}
// Self Scale - Scale pixel values by double
// Takes cv::Mat reference
void ac::SelfScale(cv::Mat &frame) {
    double inc_alpha = 0.05;
    if(alpha_increase != 0)
        inc_alpha = alpha_increase;
    else
        inc_alpha = 0.05;
    static double pos = 1.0; // pos the scale
    int w = frame.cols; // width variable
    int h = frame.rows; // height variable
    for(int z = 0; z < h; ++z) {// top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            // current pixel at x,y
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            // scale each rgb value by pos
            pixel[0] = static_cast<unsigned char>(pixel[0] * pos);
            pixel[1] = static_cast<unsigned char>(pixel[1] * pos);
            pixel[2] = static_cast<unsigned char>(pixel[2] * pos);
            swapColors(frame, z, i);// swap colors
            // if is negative set, invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // static direction variable
    static int direction = 1;
    static double pos_max = 7.0f; // position max
    if(direction == 1) { // direction equals 1
        pos += inc_alpha; // pos plus equal 0.05
        if(pos > pos_max) { // pos greater than pos_max
            pos = pos_max; // set pos to pos_max
            direction = 0; // set direction to zero
            pos_max += 0.5f; // add 0.5 to pos_max
        }
    } else if(direction == 0) { // direction is zero
        pos += -inc_alpha; // minus equal 0.05
        if(pos <= 1.0) { // pos <= 1.0
            if(pos_max > 15) pos_max = 1.0f; // reset pos if greater than 15
            direction = 1;// set direction to 1
        }
    }
    resetAlpha(direction, pos);
}
// StrobeEffect - Change frame values by passIndex, incrememnt each frame
void ac::StrobeEffect(cv::Mat &frame) {
    static int passIndex = 0;// passIndex variable
    static double alpha = 1.0f;// alpha is 1.0
    for (int z = 0; z < frame.cols - 2; ++z) {
        for (int i = 0; i < frame.rows - 2; ++i) {
            
            cv::Vec3b &colors = pixelAt(frame,i, z); // current pixel
            
            switch (passIndex) {
                case 0: // pass 0 set color values
                    colors[0] = static_cast<unsigned char>(colors[0] * (-alpha));
                    colors[1] = static_cast<unsigned char>(colors[1] * alpha);
                    colors[2] = static_cast<unsigned char>(colors[2] * alpha);
                    break;
                case 1: // pass 1 set color values
                    colors[0] += static_cast<unsigned char>(colors[0] * alpha);
                    colors[1] += static_cast<unsigned char>(colors[1] * (-alpha));
                    colors[2] += static_cast<unsigned char>(colors[2] * alpha);
                    break;
                case 2: // pass 2 set color values
                    colors[0] = static_cast<unsigned char>(colors[0] * alpha);
                    colors[1] = static_cast<unsigned char>(colors[1] * alpha);
                    colors[2] = static_cast<unsigned char>(colors[2] * (-alpha));
                    break;
                case 3: { // pass 3 grab pixels +1, and +2 ahead and use for colors
                    cv::Vec3b &color1 = pixelAt(frame,i + 1, z);// x,y + 1
                    cv::Vec3b &color2 = pixelAt(frame,i + 2, z);// x,y + 2
                    // set colors accordingly
                    colors[0] += static_cast<unsigned char>(colors[0] * alpha);
                    colors[1] += static_cast<unsigned char>(color1[1] * alpha);
                    colors[2] += static_cast<unsigned char>(color2[2] * alpha);
                    break;
                }
            }
            // swap colors
            swapColors(frame, i, z);
            if(isNegative == true) { // if negative variable set
                invert(frame, i, z);//invert pixel
            }
        }
    }
    ++passIndex; // pass index increased once each frame
    if(passIndex > 3) passIndex = 0; // if greater than 3 set back to zero
    static double max = 4.0f;// max
    if(alpha < 0) // alpha less than zero
        tr = translation_variable; // set as translation variable
    else if(alpha > max) { // greater than max
        tr = -translation_variable; // negative translation variable
        max += 3.0f;// max plus equal 3.0
        if(max > 23) max = 4.0f;// max greater than twenty three max equal four
    }
    alpha += tr; // change position
}

// Blend3
// takes cv::Mat as reference
void ac::Blend3(cv::Mat &frame) {
    static int i=0,z=0;// set i,z to zero
    static double rValue[3] = { 0, 0, 0 };
    for (z = 0; z < frame.cols; ++z) {
        for (i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = pixelAt(frame,i, z); // get pixel value
            for (int j = 0; j < 3; ++j)
                color_value[j] += static_cast<unsigned char>(color_value[j] * rValue[j]); // loop through each color multiply by rValue
            // swap colors
            swapColors(frame, i, z);
            if(isNegative == true) {// if isNegative true
                invert(frame, i, z);// invert pixel
            }
        }
    }
    // change rValue array based on random function
    // set to either -0.1 or 0.1
    double alpha_inc = 0.1;
    if(alpha_increase != 0)
        alpha_inc = alpha_increase;
    else
        alpha_inc = 0.1;
    
    
    for (int q = 0; q < 3; ++q)
        rValue[q] += ((rand() % 10) > 5) ? -alpha_inc : alpha_inc;
}
// takes cv::Mat reference
void ac::NegParadox(cv::Mat &frame) {
    static double alpha = 1.0f; // alpha equals 1.0
    for (int z = 0; z < frame.cols - 3; ++z) { // left to right
        for (int i = 0; i < frame.rows - 3; ++i) { // top to bottom
            cv::Vec3b colors[4];// vector array
            colors[0] = pixelAt(frame,i, z);// grab pixels
            colors[1] = pixelAt(frame,i + 1, z);
            colors[2] = pixelAt(frame,i + 2, z);
            colors[3] = pixelAt(frame,i + 3, z);
            cv::Vec3b &color_value = pixelAt(frame,i, z);// grab pixel
            // set final pixel color values
            color_value[0] += static_cast<unsigned char>((colors[0][0] * alpha) + (colors[1][0] * alpha));
            color_value[1] += static_cast<unsigned char>((colors[1][1] * alpha) + (colors[3][1] * alpha));
            color_value[2] += static_cast<unsigned char>((colors[1][2] * alpha) + (colors[2][2] * alpha));
            swapColors(frame, i, z); // swap the colors
            if(isNegative == true) { // if negative is true
                invert(frame, i, z);// invert pixel
            }
        }
    }
    static double trans_var = 0.1f; // translation variable
    double translation_variable = (alpha_increase != 0) ? alpha_increase :0.1;
    if (alpha < 0)
        trans_var = translation_variable;// increase
    else if (alpha > 15)
        trans_var = -translation_variable; // decrease
    alpha += trans_var; // add variable
    
    resetAlpha(alpha);
}

// Thought Mode
// takes cv::Mat reference
void ac::ThoughtMode(cv::Mat &frame) {
    static double alpha = 1.0f, trans_var = 0.1f; // alpha
    static int mode = 0;// current mode
    static int sw = 1, tr = 1;
    for(int z = 2; z < frame.cols-2; ++z) {
        for(int i = 2; i < frame.rows-4; ++i) {
            cv::Vec3b &color_value = pixelAt(frame,i, z); // current pixel
            // set pixel rgb values
            if(sw == 1) color_value[0] += static_cast<unsigned char>(color_value[mode]*alpha);
            if(tr == 0) color_value[mode] -= static_cast<unsigned char>(color_value[rand()%2]*alpha);
            color_value[mode] += static_cast<unsigned char>(color_value[mode]*alpha);
            mode++; // increase mode
            if(mode >= 3) mode = 0; // reset mode if greater than equal three
            swapColors(frame, i, z);// swap colors
            if(isNegative == true) { // if is negative true
                invert(frame, i, z);// invert pixel
            }
        }
    }
    sw = !sw;// not sw
    tr = !tr;// not tr
    static double max = 4.0f;
    if(alpha < 0)
        trans_var = translation_variable;
    else if(alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if(max > 23) max = 4.0f;
    }
    alpha += trans_var; // add to alpha
    resetAlpha(alpha);
}
// blend with original pixel
void ac::Pass2Blend(cv::Mat &frame) {
    if(orig_frame.empty())
        return;
    for(int z = 0;  z < frame.rows; ++z) { // top to bottom
        for(int i = 0; i < frame.cols; ++i) { // left to right
            if(!frame.empty() && !orig_frame.empty()) {
                cv::Vec3b &color1 = pixelAt(frame,z, i);// current pixel
                cv::Vec3b color2 = pixelAt(orig_frame,z, i);// original frame pixel
                for(int q = 0; q < 3; ++q)
                    color1[q] = static_cast<unsigned char>(color2[q] * ac::pass2_alpha) + static_cast<unsigned char>(color1[q] * ac::pass2_alpha);
                    //color1[q] = static_cast<unsigned char>(color2[q]+(color1[q]*ac::pass2_alpha));// multiply
            }
        }
    }
}

// Takes cv::Mat reference
void ac::RandTriBlend(cv::Mat &frame) {
    static double alpha = 1.0f;//alpha equals 1.0
    static int i = 0, z = 0;// i,z loop variables
    int counter = 0;// center variable
    cv::Vec3b colors[6];// array of six colors
    for (z = 2; z < frame.cols - 2; ++z) {
        for (i = 2; i < frame.rows - 2; ++i) {
            // grab pixels
            colors[0] = pixelAt(frame,i, z);
            colors[1] = pixelAt(frame,i + 1, z);
            colors[2] = pixelAt(frame,i + 2, z);
            // chaos
            counter = rand() % 3;
            if (counter == 0) { // if counter equals zero
                // set pixel values
                colors[3][0] = static_cast<unsigned char>((colors[0][0] + colors[1][0] + colors[2][0]*alpha));
                colors[3][1] = static_cast<unsigned char>((colors[0][1] + colors[1][1]) * alpha);
                colors[3][2] = static_cast<unsigned char>((colors[0][2]) * alpha);
                counter++; // increase counter
            } else if (counter == 1) { // if counter equals one
                // set pixel values
                colors[3][0] = static_cast<unsigned char>((colors[0][0]) * alpha);
                colors[3][1] = static_cast<unsigned char>((colors[0][1] + colors[1][1]) * alpha);
                colors[3][2] = static_cast<unsigned char>((colors[0][2] + colors[1][2] + colors[2][2]) * alpha);
                counter++; // increase counter
            } else {
                // set pixel values
                colors[3][0] = static_cast<unsigned char>((colors[0][0]) * alpha);
                colors[3][2] = static_cast<unsigned char>((colors[0][1] + colors[1][1]) * alpha);
                colors[3][1] = static_cast<unsigned char>((colors[0][2] + colors[1][2] + colors[2][2]) * alpha);
            }
            cv::Vec3b &color_value = pixelAt(frame,i, z);// grab current pixel
            color_value = colors[3];// assign pixel
            swapColors(frame, i, z);// swap colors
            if(isNegative == true) { // if isNegative
                invert(frame, i, z);// invert pixel
            }
        }
    }
    static double max = 4.0f, trans_var = translation_variable;// max, translation variable
    if (alpha < 0) // if alpha less than zero
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;// add to alpha translation variable
    resetAlpha(alpha);
}

// Blank
// takes cv::Mat reference
void ac::Blank(cv::Mat &frame) {
    static double alpha = 1.0f; // static alpha set to 1.0
    static unsigned char val[3] = { 0 };// val array set to zero
    static bool color_switch = false;// color switch set to false
    for(int z = 0; z < frame.cols; ++z) {// left to right
        for(int i = 0; i < frame.rows; ++i) { // top to bottom
            cv::Vec3b &color_value = pixelAt(frame,i, z); // current pixel
            for(int j = 0; j < 3; ++j) {
                // process pixel values
                val[j] = static_cast<unsigned char>((alpha*color_value[j]) / (2-j+1));
                color_value[j] += static_cast<unsigned char>(val[2-j] / (j+1));
                if(color_switch == true) color_value[j] = static_cast<unsigned char>(-color_value[j]);
            }
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z); // invert pixel
            }
        }
    }
    color_switch = !color_switch;// not color switch
    static double max = 4.0f, trans_var = translation_variable;
    if (alpha < 0)
        trans_var = translation_variable; // positive (up)
    else if (alpha > max) {
        trans_var = -translation_variable; // negative (down)
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var; // add to alpha trans_Var
    resetAlpha(alpha);
}
// Tri
// takes cv::Mat reference
void ac::Tri(cv::Mat &frame) {
    static double alpha = 1.0f;// static alpha set to 1
    for(int z = 0; z < frame.cols-3; ++z) {// left to right
        for(int i = 0; i < frame.rows-3; ++i) {// top to bottom
            cv::Vec3b &color_value = pixelAt(frame,i, z);// current pixel
            cv::Vec3b colors[2];// colors
            // grab pixels
            colors[0] = pixelAt(frame,i+1, z);
            colors[1] = pixelAt(frame,i+2, z);
            // set pixels
            color_value[0] += static_cast<unsigned char>(color_value[0]*alpha);
            color_value[1] += static_cast<unsigned char>(color_value[1]+colors[0][1]+colors[1][1]*alpha);
            color_value[2] += static_cast<unsigned char>(color_value[2]+colors[0][2]+colors[1][2]*alpha);
            swapColors(frame, i, z);// swap
            if(isNegative == true) {
                invert(frame, i, z); // invert pixel
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable; // positive (up)
    else if (alpha > max) {
        trans_var = -translation_variable; // negative (down)
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;// add to alpha trans var
    resetAlpha(alpha);
}
// Distort
// takes cv::Mat reference
void ac::Distort(cv::Mat &frame) {
    static double alpha = 1.0f; // static alpha set to 1
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) { // left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &color_value = pixelAt(frame,i, z);
            // set pixel values
            color_value[0] = static_cast<unsigned char>((i*alpha)+color_value[0]);
            color_value[2] = static_cast<unsigned char>((z*alpha)+color_value[2]);
            color_value[1] = static_cast<unsigned char>((alpha*color_value[1]));
            if(strobe_It == true) color_value[1] = static_cast<unsigned char>(((i+z)*alpha)+color_value[1]);
            swapColors(frame, i, z); //swap
            if(isNegative == true) {
                invert(frame, i, z);// invert
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;// add translation to alpha
    resetAlpha(alpha);
}
// takes cv::Mat reference
void ac::CDraw(cv::Mat &frame) {
    static int i=0,z=0;// loop variables
    static double rad = 80.0f;// radius
    static double deg = 1.0f;// degrees
    for(z = 0; z < frame.cols; ++z) { // left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            int cX = static_cast<int>((rad * cos(deg)));
            int cY = static_cast<int>((rad * sin(deg)));
            cv::Vec3b &color_value = pixelAt(frame,i, z); // grab pixel reference
            // set values
            color_value[0] = static_cast<unsigned char>(color_value[0]*(cX * alpha));
            color_value[1] = static_cast<unsigned char>(color_value[1]*(cY * alpha));
            color_value[2] = static_cast<unsigned char>(color_value[2]*alpha);
            deg += 0.1f;
            swapColors(frame, i, z);// swap
            if(isNegative) invert(frame, i, z);// if isNegative invert
        }
    }
    alpha += 0.1f;// add to alpha
    rad += 0.1f;// add to rad
    if(rad > 90) rad = 0;// greater than 90 reset
    if(alpha > 20) alpha = 0;// greater than 20 reset
    resetAlpha(alpha);
}
// Light Strobe
// first cycle through the image
// add a variable to each individual pixel (the input sould be different each frame)
// reason for this is adding to the captured image each frame causes a animation a distortion
// each frame the largest value is calculated by adding the rgb values together for one element each iteration.
// test this first
void ac::Type(cv::Mat &frame) {
    int i = 0, z = 0;// loop variables
    static double add_r = 1.0; // add_r
    static int off = 0;// off variable
    for(z = 0; z < frame.rows; ++z) { // top to bottom
        for(i = 0; i < frame.cols; ++i) {// left to right
            cv::Vec3b &current = pixelAt(frame,z, i); // grab pixel reference
            // set pixel values
            current[0] += static_cast<unsigned char>(add_r+current[0]);
            current[1] += static_cast<unsigned char>(add_r+current[1]);
            current[2] += static_cast<unsigned char>(add_r+current[2]);
            // set value indexed by off which changes each frame
            current[off] = current[0]+current[1]+current[2];
            swapColors(frame, z, i);// swap the colors
            if(isNegative) invert(frame, z, i); // invert pixel
        }
    }
    ++off;// increase off
    if(off > 2) off = 0;// greater than two set to zero
    add_r += rand()%255;// random distortion plus equals random number
    if(add_r > 255) add_r = 0;// greater than 255 set to zero
}
// New One
// takes cv::Mat reference
void ac::NewOne(cv::Mat &frame) {
    for(int z = 0; z < frame.cols; ++z) {// left to right
        for(int i = 1; i < frame.rows-1; ++i) {// top to bottom
            cv::Vec3b &colv = pixelAt(frame,i, z);// get pixels
            cv::Vec3b &cola = pixelAt(frame,(frame.rows-1)-i, (frame.cols-1)-z);//pixelAt(frame,(frame.cols-1)-z, (frame.rows-1)-i);
            // set arrays
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            // loop through arrays
            for(int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            // set pixel values
            colv[0] += static_cast<unsigned char>(alpha*R);
            colv[1] += static_cast<unsigned char>(alpha*G);
            colv[2] += static_cast<unsigned char>(alpha*B);
            swapColors(frame, i, z);//swap colors
            if(isNegative) invert(frame, i, z); // if isNegative invert pixel
        }
    }
    static double max = 8.0f, trans_var = 0.1f;// max and translation
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;// add translation variable
    resetAlpha(alpha);
    
}
// draw a fractal
void ac::blendFractal(cv::Mat &frame) {
    frac::FractalLogic();
    frac::DrawFractal(frame, ac::isNegative);
}

// draw a fractal with background color blended
void ac::blendFractalMood(cv::Mat &frame) {
    // random
    unsigned char color = 0;
    color = rand()%255;
    static bool shift = true;
    static bool shift_value = true;
    for(int z = 0; z < frame.cols; ++z) {// left to right
        for(int i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &color_value = pixelAt(frame,i, z);// grab pixel
            // set pixel values
            color_value[0] += (shift == shift_value) ? color : -color;
            color_value[1] += (shift == shift_value) ? -color : color;
            color_value[2] += (shift == shift_value) ? color : -color;
            shift_value = !shift_value;// not shift value
        }
    }
    shift = ! shift;// not shift value
    frac::FractalLogic();
    frac::DrawFractal(frame, ac::isNegative); // draw fractal
}

// blend with Image functions Resize X
inline int ac::GetFX(cv::Mat &frame, int x, int nw) {
    double xp = (double)x * (double)frame.rows / (double)nw;
    return (int)xp;
}
// blend with Image function Resize Y
inline int ac::GetFY(cv::Mat &frame, int y, int nh) {
    double yp = (double)y * (double)frame.cols / (double)nh;
    return (int)yp;
}
// blend with Image function
// takes cv::Mat as reference
void ac::blendWithImage(cv::Mat &frame) {
    if(!blendW_frame.data) // if image not loaded return
        return;
    static double alpha = 1.0f; // set alpha to 1
    static double beta = 1.0f; // set beta to 1
    for(int z = 0; z < frame.cols; ++z) {// left to right
        for(int i = 0; i < frame.rows; ++i) {// top to bottom
            // get resized pixel values
            int q = GetFX(blendW_frame, i, frame.rows);
            int j = GetFY(blendW_frame, z, frame.cols);
            // grab pixels
            cv::Vec3b &frame_one = pixelAt(frame,i, z);
            cv::Vec3b &frame_two = pixelAt(blendW_frame,q, j);
            // set pixel values
            for(int p = 0; p < 3; ++p)
                frame_one[p] += static_cast<unsigned char>((frame_one[p]*alpha)+(frame_two[p]*beta));
            swapColors(frame, i, z); // swap colors
            if(isNegative == true) {
                invert(frame, i, z);// invert pixel
            }
        }
    }
    // move alpha and beta values up and down
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
    beta += -trans_var;
}

// Cos Sin Mulitply draw gradients
// takes cv::Mat reference
void ac::cossinMultiply(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z); // grab pixel
            // set pixel values
            buffer[0] += static_cast<unsigned char>(1+static_cast<int>((sin(alpha))*z));
            buffer[1] += static_cast<unsigned char>(1+static_cast<int>((cos(alpha))*i));
            buffer[2] += static_cast<unsigned char>((buffer[0]+buffer[1]+buffer[2])/3);
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// invert pixel
        }
    }
    // add alpha up to 24 return to zero when greater
    static double trans_var = 0.05f;
    if(alpha > 24) alpha = 1.0f;
    alpha += trans_var;
    resetAlpha(alpha);
}
// Color Accumulate 1
void ac::colorAccumulate1(cv::Mat &frame) {
    static double alpha = 1.0f; // alpha to 1.0
    static int i = 0, z = 0; // static loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z);// current pixel
            // set pixel values
            buffer[0] += static_cast<unsigned char>((buffer[2]*alpha));
            buffer[1] += static_cast<unsigned char>((buffer[0]*alpha));
            buffer[2] += static_cast<unsigned char>((buffer[1]*alpha));
            swapColors(frame, i, z); // swap colors
            if(isNegative) invert(frame, i, z);// invert pixel
        }
    }
    // increase alpha until 24 then reset
    static double trans_var = 0.05f;
    alpha += trans_var;
    if(alpha > 24) alpha = 1.0f;
    resetAlpha(alpha);
}
// Color Accumulate 2
void ac::colorAccumulate2(cv::Mat &frame) {
    static double alpha = 1.0f;// static alpha set to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            // grab pixel
            cv::Vec3b &buffer = pixelAt(frame,i, z);
            // set pixel rgb values
            buffer[0] += static_cast<unsigned char>((buffer[2]*alpha)+i);
            buffer[1] += static_cast<unsigned char>((buffer[0]*alpha)+z);
            buffer[2] += static_cast<unsigned char>((buffer[1]*alpha)+i-z);
            swapColors(frame, i, z);// swap
            if(isNegative) invert(frame, i, z);// if isNegative invert
        }
    }
    static double trans_var = 0.05f;// translation variable
    alpha += trans_var;// alpha plus equal translation variable
    if(alpha > 24) alpha = 1.0f;// if alpha greater than 24 reset to 1
    resetAlpha(alpha);
}
// Color Accumulate #3
// takes cv::Mat reference
void ac::colorAccumulate3(cv::Mat &frame) {
    static double alpha = 1.0f;// set alpha to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// from left to right
        for(i = 0; i < frame.rows; ++i) {// from top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z);// grab pixel reference
            // set rgb values
            buffer[0] = static_cast<unsigned char>((-buffer[2])+z);
            buffer[1] = static_cast<unsigned char>((-buffer[0])+i);
            buffer[2] = static_cast<unsigned char>((-buffer[1])+alpha);
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative true invert pixel
        }
    }
    static double trans_var = 0.05f;// 0.05 variable
    alpha += trans_var;// alpha plus equal translation variable
    if(alpha > 24) alpha = 1.0f;// alpha greater than 24 set to 1 (reset)
    resetAlpha(alpha);
}

// takes cv::Mat reference
void ac::filter8(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0, q = 0;// loop variable
    for(z = 0; z < frame.cols; ++z) {// from left to right
        for(i = 0; i < frame.rows; ++i) {// from top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z);// grab pixel
            for(q = 0; q < 3; ++q) {// loop each rgb value
                buffer[q] = static_cast<unsigned char>(buffer[q]+((i+z)*alpha));// preform calculation
                
            }
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative invert
        }
    }
    // static direction equals 1
    static int direction = 1;
    if(direction == 1) {// if direction equals 1
        alpha += 0.05f;// alpha plus equal 0.05
        if(alpha > 3) { alpha = 3; direction = 2; }// alpha greater than 3 set direction to 2
    } else {
        alpha -= 0.05f;// alpha minus equal 0.05
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }//alpha greater than 3 set direction to 1
    }
    resetAlpha(direction, alpha);
}

// takes cv::Mat reference
void ac::filter3(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0, q = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z);// grab pixel reference
            for(q = 0; q < 3; ++q) {// loop through rgb values
                buffer[q] = static_cast<unsigned char>(buffer[0]+(buffer[q])*(alpha));// preform calculation
            }
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative invert pixel
        }
    }
    // direction equals 1
    static int direction = 1;
    if(direction == 1) { // if direction equals 1
        alpha += 0.1f;// alpha plus equal 0.1
        if(alpha > 6) { alpha = 6; direction = 2; } // if alpha greater than 6 set alpha to 6 direction to 2
    } else {
        alpha -= 0.05f;// alpha minus equal 0.1
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; } // if alpha lses than equal 0.1 set to 0.1 direction equals 1
    }
    resetAlpha(direction, alpha);
}

// takes cv::Mat as reference
// uses 3 static variables to represent the RGB values
// to mulitply by the alpha variable
// each frame they either increase or are reset to a random number when set to zero
// when they reach a number greater than 255 they are reset to zero
void ac::rainbowBlend(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int rb = 0, gb = 0, bb = 0;// set static integer r,g,b values
    if(rb == 0) // if rb equals 0
        rb = rand()%255;// set rb to random number
    else ++rb;// else increase rb
    if(gb == 0) // if gb equals 0
        gb = rand()%255;// gb equals random number
    else ++gb;// else gb increases
    if(bb == 0) // if bb equals 0
        bb = rand()%255;// bb equals random number
    else ++bb;// else increase bb
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            // grab pixel as cv::Vec3b reference
            cv::Vec3b &buffer = pixelAt(frame,i, z);
            // add to rgb values alpha * rb,gb,bb variables
            buffer[0] += static_cast<unsigned char>(alpha*rb);
            buffer[1] += static_cast<unsigned char>(alpha*gb);
            buffer[2] += static_cast<unsigned char>(alpha*bb);
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative invert pixel
        }
    }
    // if rb greater than 255 set to zero
    if(rb > 255) rb = 0;
    // if gb greater than 255 set to zero
    if(gb > 255) gb = 0;
    // if bb greater than 255 set to zero
    if(bb > 255) bb = 0;
    // static int direction equals 1
    static int direction = 1;
    if(direction == 1) {// if direction equals 1
        alpha += 0.1f;// increase alpha
        // alpha greater than 6 change direction
        if(alpha > 6) { alpha = 6; direction = 2; }
    } else {
        // decrease alpha
        alpha -= 0.05f;
        // if alpha <= 0.1 change direction
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
    resetAlpha(direction, alpha);
}

// random pixel value added to each pixel RGB value each frame
// takes cv::Mat reference
void ac::randBlend(cv::Mat &frame) {
    unsigned char rr = rand()%255;// random Red
    unsigned char rg = rand()%255;// random Green
    unsigned char rb = rand()%255;// random Blue
    static int i = 0, z = 0;// i,z loop variables
    for(z = 0; z < frame.cols; ++z) {// from left to right
        for(i = 0; i < frame.rows; ++i) {// from top to bottom
            cv::Vec3b &buffer = pixelAt(frame,i, z);// pixel at
            buffer[0] += rr;// add random R
            buffer[1] += rg;// add random G
            buffer[2] += rb;// add random B
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if negative, invert pixel
        }
    }
}

// takes cv::Mat reference
void ac::newBlend(cv::Mat &frame) {
    static int pos = 300; // static int pos equal 300
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            // grab pixel
            cv::Vec3b &buffer = pixelAt(frame,i, z);
            // set pixel RGB values
            buffer[0] = static_cast<unsigned char>(buffer[2]+(1+(i*z)/pos));
            buffer[1] = static_cast<unsigned char>(buffer[1]+(1+(i*z)/pos));
            buffer[2] = static_cast<unsigned char>(buffer[0]+(1+(i*z)/pos));
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if(isNegative) invert pixel
        }
    }
    static int dir = 1;// static direction equals 1
    if(dir == 1) {// dir equals 1
        pos += 25;// pos plus equal 25
        if(pos > 1024) {// greater than 1000
            pos = 1023;
            dir = 2;// set direction to 2
        }
    }
    else {// direction != 1
        pos -= 25;// minus 25
        if(pos < 100) {// less than 100
            pos = 100;
            dir = 1;// set direction to 1
        }
    }
}
// pixelScale
// takes cv::Mat reference
void ac::pixelScale(cv::Mat &frame) {
    static double pos = 1.0f;// pos equals 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            // grab pixel reference
            cv::Vec3b &buffer = pixelAt(frame,i, z);
            cv::Vec3b buf = buffer;// temp pixel
            // set RGB pixel values
            buffer[0] = static_cast<unsigned char>((buf[0]*pos)+(buf[0]-buffer[2]));
            buffer[1] = static_cast<unsigned char>((buf[1]*pos)+(buf[1]+buffer[1]));
            buffer[2] = static_cast<unsigned char>((buf[2]*pos)+(buf[2]-buffer[0]));
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative invert pixel
        }
    }
    static int direction = 1;// direction equals 1
    static double pos_max = 3.0f;// pos_max equals 3.0
    if(direction == 1) {// direction equals 1
        pos += 0.1f;// plus equal 0.1
        if(pos > pos_max) {// greater than pos_max
            pos = pos_max;
            direction = 0;// set direction to zero
            pos_max += 0.5f;// increase pos_max
        }
    } else if(direction == 0) {// direction equals 0
        pos -= 0.1f;// pos minus equal 0.1
        if(pos <= 0) {// pos less than equal 0
            if(pos_max > 15) pos_max = 1.0f;// pos_max > 14 reset
            direction = 1;// direction set back to 1
        }
    }
    resetAlpha(direction, pos);
    
}
