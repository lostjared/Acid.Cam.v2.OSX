/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared

 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2017, Jared Bruni
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
 
 
 * One quick note, most of the time when writing programs using x,y variables x goes first
 * the OpenCV Mat at function that returns a pixel is reversed.
 * y is first. Example
 * cv::Vec3b &v = frame.at<cv::Vec3b>(y, x);
 *
 */
#include "ac.h"
#include "fractal.h"

// Acid Cam namespace
namespace ac {
    // variables
    unsigned int swapColor_r = 0, swapColor_g = 0, swapColor_b = 0;
    bool isNegative = false, noRecord = false, pass2_enabled = false, blendW = false, slide_Show = false, slide_Rand = false, strobe_It = false, switch_Back = false, blur_First = false;
    bool images_Enabled = false, fps_force = false,iRev = false;
    bool blur_Second = false;
    cv::Mat orig_frame;
    cv::Mat blendW_frame;
    cv::Mat image_files[4];
    double alpha = 1.0f, tr = 0.01f;
    double fps = 29.97;
    int draw_offset = 0;
    bool snapShot = false;
    int color_order = 0;
    int snapshot_Type = 0;
    bool in_custom = false;
    std::string fileName ="VideoFile.avi";
    // draw strings (function names)
    std::string draw_strings[] = { "Self AlphaBlend", "Self Scale", "StrobeEffect", "Blend #3", "Negative Paradox", "ThoughtMode", "RandTriBlend", "Blank", "Tri", "Distort", "CDraw", "Type", "NewOne", "Blend Fractal","Blend Fractal Mood", "CosSinMultiply", "Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8","Filter3","Rainbow Blend","Rand Blend","New Blend", "Alpha Flame Filters", "Pixel Scale", "PixelSort", "GlitchSort","Random Filter", "Random Flash", "Blend with Image", "Blend with Image #2", "Blend with Image #3", "Blend with Image #4", "GaussianBlur", "Median Blur", "Blur Distortion", "Diamond Pattern", "MirrorBlend","Pulse","Sideways Mirror","Mirror No Blend","Sort Fuzz","Fuzz","Double Vision","RGB Shift","RGB Sep","Graident Rainbow","Gradient Rainbow Flash", "Reverse", "Scanlines", "TV Static", "Mirror Average", "Mirror Average Mix", "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND", "Bitwise_OR", "Equalize", "Channel Sort", "Reverse_XOR", "Combine Pixels", "FlipTrip", "Canny","Boxes","Boxes Fade", "Flash Black", "SlideRGB", "Side2Side","Top2Bottom","Strobe Red Then Green Then Blue","Blend_Angle", "Outward", "Outward Square", "ShiftPixels", "ShiftPixelsDown", "XorMultiBlend", "Bitwise_Rotate", "Bitwise_Rotate Diff", "HPPD", "FuzzyLines","GradientLines","GradientSelf","GradientSelfVertical", "GradientDown", "GraidentHorizontal", "GradientRGB","Inter", "UpDown","LeftRight","StrobeScan","BlendedScanLines","GradientStripes","XorSine","SquareSwap",
        "SquareSwap4x2","SquareSwap8x4", "SquareSwap16x8","SquareSwap64x32","SquareBars","SquareBars8","SquareSwapRand16x8",
        "SquareVertical8", "SquareVertical16","SquareVertical_Roll","SquareSwapSort_Roll","SquareVertical_RollReverse","SquareSwapSort_RollReverse","Circular","WhitePixel","FrameBlend", "FrameBlendRGB",
        "TrailsFilter",
        "TrailsFilterIntense","TrailsFilterSelfAlpha","TrailsFilterXor","ColorTrails","MoveRed","MoveRGB","MoveRedGreenBlue","BlurSim", "Block","BlockXor","BlockScale","BlockStrobe", "PrevFrameBlend", "No Filter",
        "Blend with Source", "Plugin", "Custom","Blend With Image #1",  "TriBlend with Image", "Image Strobe", "Image distraction" };
    
    // filter callback functions
    DrawFunction draw_func[] = { SelfAlphaBlend, SelfScale, StrobeEffect, Blend3, NegParadox, ThoughtMode, RandTriBlend, Blank, Tri, Distort, CDraw,Type,NewOne,blendFractal,blendFractalMood,cossinMultiply, colorAccumulate1, colorAccumulate2, colorAccumulate3,filter8,filter3,rainbowBlend,randBlend,newBlend,
        alphaFlame, pixelScale,pixelSort, glitchSort,randomFilter,randomFlash, imageBlend,imageBlendTwo,imageBlendThree,imageBlendFour, GaussianBlur, MedianBlur, BlurDistortion,DiamondPattern,MirrorBlend,Pulse,SidewaysMirror,MirrorNoBlend,SortFuzz,Fuzz,DoubleVision,RGBShift,RGBSep,GradientRainbow,GradientRainbowFlash,Reverse,Scanlines,TVStatic,MirrorAverage,MirrorAverageMix,Mean,Laplacian,Bitwise_XOR,Bitwise_AND,Bitwise_OR,Equalize,ChannelSort,Reverse_XOR,CombinePixels,FlipTrip,Canny,Boxes,BoxesFade,FlashBlack,SlideRGB,Side2Side,Top2Bottom, StrobeRedGreenBlue,Blend_Angle,Outward,OutwardSquare,ShiftPixels,ShiftPixelsDown,XorMultiBlend,BitwiseRotate,BitwiseRotateDiff,HPPD,FuzzyLines,GradientLines,GradientSelf,GradientSelfVertical,GradientDown,GraidentHorizontal,GradientRGB,Inter,UpDown,LeftRight,StrobeScan,BlendedScanLines,GradientStripes,XorSine,SquareSwap,
        SquareSwap4x2, SquareSwap8x4, SquareSwap16x8,SquareSwap64x32,SquareBars,SquareBars8,SquareSwapRand16x8,
        SquareVertical8,SquareVertical16,SquareVertical_Roll,SquareSwapSort_Roll,SquareVertical_RollReverse,SquareSwapSort_RollReverse,Circular,WhitePixel,FrameBlend,FrameBlendRGB,
        TrailsFilter,TrailsFilterIntense,TrailsFilterSelfAlpha,TrailsFilterXor,ColorTrails,MoveRed,MoveRGB,MoveRedGreenBlue,BlurSim,Block,BlockXor,BlockScale,BlockStrobe,PrevFrameBlend,
        NoFilter,BlendWithSource,plugin,custom,blendWithImage, triBlendWithImage,imageStrobe, imageDistraction,0};
    // number of filters
    
    int draw_max = 134;
    // variables
    double translation_variable = 0.001f, pass2_alpha = 0.75f;
    // swap colors inline function
    inline void swapColors(cv::Mat &frame, int x, int y);
    inline void procPos(int &direction, double &pos, double &pos_max, const double max_size = 15);
    std::unordered_map<std::string, int> filter_map;
}

void ac::fill_filter_map() {
    for(int i = 0; i < ac::draw_max; ++i) {
        filter_map[draw_strings[i]] = i;
    }
}

// swapColors inline function takes frame and x, y position
inline void ac::swapColors(cv::Mat &frame, int x, int y) {
    if(in_custom == true) return;
    if(color_order == 0 && swapColor_r == 0 && swapColor_g == 0 && swapColor_b == 0) return; // if no swap needed return
    cv::Vec3b &cur = frame.at<cv::Vec3b>(x,y);
    
    cur[0] += swapColor_b;
    cur[1] += swapColor_g;
    cur[2] += swapColor_r;
    
    cv::Vec3b temp;// temp
    temp = cur;// temp = cur
    
    // swap RGB orders
    switch(color_order) {
        case 1:
            cur[0] = temp[2];
            cur[1] = temp[1];
            cur[2] = temp[0];
            break;
        case 2:
            cur[0] = temp[1];
            cur[1] = temp[2];
            cur[2] = temp[0];
            break;
        case 3:
            cur[0] = temp[2];
            cur[1] = temp[0];
            cur[2] = temp[1];
            break;
        case 4:
            cur[0] = temp[1];
            cur[1] = temp[0];
            cur[2] = temp[2];
            break;
    }
}
// invert pixel in frame at x,y
inline void ac::invert(cv::Mat &frame, int x, int y) {
    if(in_custom == true) return;
    cv::Vec3b &cur = frame.at<cv::Vec3b>(x,y);// cur pixel
    cur[0] = ~cur[0]; // bit manipulation sets opposite
    cur[1] = ~cur[1];
    cur[2] = ~cur[2];
}

// proc position
void ac::procPos(int &direction, double &pos, double &pos_max, const double max_size) {
    // static int direction
    // pos max
    // if direction equals 1
    if(direction == 1) {
        pos += 0.05; // pos plus equal 0.05
        if(pos > pos_max) { // if pos > pos max
            pos = pos_max; // pos = pos_max
            direction = 0;// direction equals 0
            pos_max += 0.5; // pos_max increases by 0.5
        }
    } else if(direction == 0) {// direction equals 1
        pos -= 0.05;// pos -= 0.05
        if(pos <= 1.0) {// if pos <= 1.0
            if(pos_max > max_size) pos_max = 1.0;// if pos max at maxmium
            // set to 1.0
            direction = 1;// set direction back to 1
        }
    }
}

// SelfAlphaBlend - Perform out of Bounds AlphaBlend on source image
void ac::SelfAlphaBlend(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {// from top to bottom
        for(int i = 0; i < frame.cols; ++i) {// from left to right
            cv::Vec3b &colorval = frame.at<cv::Vec3b>(z, i);// at x,y
            colorval[0] += colorval[0]*alpha;
            colorval[1] += colorval[1]*alpha;
            colorval[2] += colorval[2]*alpha;
            swapColors(frame, z, i);// swap colors
            if(isNegative == true) { // if negative
                invert(frame, z, i);// invert
            }
        }
    }
    static int direction = 1;// direction equals 1
    if(direction == 1) {// if direction equals 1
        alpha += 0.1f; // plus equal 0.1
        // if alpha greater than 10
        if(alpha > 10) { alpha = 10; direction = 2; }
    } else {
        alpha -= 0.05f; // minus equal 0.05
        // if alpha <= 0.1f
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
}
// Self Scale - Scale pixel values by double
// Takes cv::Mat reference
void ac::SelfScale(cv::Mat &frame) {
    static double pos = 1.0; // pos the scale
    int w = frame.cols; // width variable
    int h = frame.rows; // height variable
    for(int z = 0; z < h; ++z) {// top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            // current pixel at x,y
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // scale each rgb value by pos
            pixel[0] = pixel[0] * pos;
            pixel[1] = pixel[1] * pos;
            pixel[2] = pixel[2] * pos;
            swapColors(frame, z, i);// swap colors
            // if is negative set, invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    // static direction variable
    static int direction = 1;
    static double pos_max = 7.0f; // position max
    if(direction == 1) { // direction equals 1
        pos += 0.05; // pos plus equal 0.05
        if(pos > pos_max) { // pos greater than pos_max
            pos = pos_max; // set pos to pos_max
            direction = 0; // set direction to zero
            pos_max += 0.5f; // add 0.5 to pos_max
        }
    } else if(direction == 0) { // direction is zero
        pos -= 0.05; // minus equal 0.05
        if(pos <= 1.0) { // pos <= 1.0
            if(pos_max > 15) pos_max = 1.0f; // reset pos if greater than 15
            direction = 1;// set direction to 1
        }
    }
}
// StrobeEffect - Change frame values by passIndex, incrememnt each frame
void ac::StrobeEffect(cv::Mat &frame) {
    static unsigned int passIndex = 0;// passIndex variable
    static double alpha = 1.0f;// alpha is 1.0
    for (int z = 0; z < frame.cols - 2; ++z) {
        for (int i = 0; i < frame.rows - 2; ++i) {
            
            cv::Vec3b &colors = frame.at<cv::Vec3b>(i, z); // current pixel
            
            switch (passIndex) {
                case 0: // pass 0 set color values
                    colors[0] = colors[0] * (-alpha);
                    colors[1] = colors[1] * alpha;
                    colors[2] = colors[2] * alpha;
                    break;
                case 1: // pass 1 set color values
                    colors[0] += colors[0] * alpha;
                    colors[1] += colors[1] * (-alpha);
                    colors[2] += colors[2] * alpha;
                    break;
                case 2: // pass 2 set color values
                    colors[0] = colors[0] * alpha;
                    colors[1] = colors[1] * alpha;
                    colors[2] = colors[2] * (-alpha);
                    break;
                case 3: { // pass 3 grab pixels +1, and +2 ahead and use for colors
                    cv::Vec3b &color1 = frame.at<cv::Vec3b>(i + 1, z);// x,y + 1
                    cv::Vec3b &color2 = frame.at<cv::Vec3b>(i + 2, z);// x,y + 2
                    // set colors accordingly
                    colors[0] += colors[0] * alpha;
                    colors[1] += color1[1] * alpha;
                    colors[2] += color2[2] * alpha;
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
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z); // get pixel value
            for (int j = 0; j < 3; ++j)
                color_value[j] += color_value[j] * rValue[j]; // loop through each color multiply by rValue
            // swap colors
            swapColors(frame, i, z);
            if(isNegative == true) {// if isNegative true
                invert(frame, i, z);// invert pixel
            }
        }
    }
    // change rValue array based on random function
    // set to either -0.1 or 0.1
    for (int q = 0; q < 3; ++q)
        rValue[q] += ((rand() % 10) > 5) ? -0.1f : 0.1f;
}
// takes cv::Mat reference
void ac::NegParadox(cv::Mat &frame) {
    static double alpha = 1.0f; // alpha equals 1.0
    for (int z = 0; z < frame.cols - 3; ++z) { // left to right
        for (int i = 0; i < frame.rows - 3; ++i) { // top to bottom
            cv::Vec3b colors[4];// vector array
            colors[0] = frame.at<cv::Vec3b>(i, z);// grab pixels
            colors[1] = frame.at<cv::Vec3b>(i + 1, z);
            colors[2] = frame.at<cv::Vec3b>(i + 2, z);
            colors[3] = frame.at<cv::Vec3b>(i + 3, z);
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);// grab pixel
            // set final pixel color values
            color_value[0] += (colors[0][0] * alpha) + (colors[1][0] * alpha);
            color_value[1] += (colors[1][1] * alpha) + (colors[3][1] * alpha);
            color_value[2] += (colors[1][2] * alpha) + (colors[2][2] * alpha);
            swapColors(frame, i, z); // swap the colors
            if(isNegative == true) { // if negative is true
                invert(frame, i, z);// invert pixel
            }
        }
    }
    static double trans_var = 0.1f; // translation variable
    if (alpha < 0)
        trans_var = translation_variable;// increase
    else if (alpha > 15)
        trans_var = -translation_variable; // decrease
    alpha += trans_var; // add variable
}

// Thought Mode
// takes cv::Mat reference
void ac::ThoughtMode(cv::Mat &frame) {
    static double alpha = 1.0f, trans_var = 0.1f; // alpha
    static int mode = 0;// current mode
    static int sw = 1, tr = 1;
    for(int z = 2; z < frame.cols-2; ++z) {
        for(int i = 2; i < frame.rows-4; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z); // current pixel
            // set pixel rgb values
            if(sw == 1) color_value[0]+= color_value[mode]*alpha;
            if(tr == 0) color_value[mode] -= color_value[rand()%2]*alpha;
            color_value[mode] += color_value[mode]*alpha;
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
}
// blend with original pixel
void ac::Pass2Blend(cv::Mat &frame) {
    for(int z = 0;  z < frame.rows; ++z) { // top to bottom
        for(int i = 0; i < frame.cols; ++i) { // left to right
            cv::Vec3b &color1 = frame.at<cv::Vec3b>(z, i);// current pixel
            cv::Vec3b color2 = orig_frame.at<cv::Vec3b>(z, i);// original frame pixel
            for(int q = 0; q < 3; ++q)
                color1[q] = color2[q]+(color1[q]*ac::pass2_alpha);// multiply
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
            colors[0] = frame.at<cv::Vec3b>(i, z);
            colors[1] = frame.at<cv::Vec3b>(i + 1, z);
            colors[2] = frame.at<cv::Vec3b>(i + 2, z);
            // chaos
            counter = rand() % 3;
            if (counter == 0) { // if counter equals zero
                // set pixel values
                colors[3][0] = (colors[0][0] + colors[1][0] + colors[2][0])
                * alpha;
                colors[3][1] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][2] = (colors[0][2]) * alpha;
                counter++; // increase counter
            } else if (counter == 1) { // if counter equals one
                // set pixel values
                colors[3][0] = (colors[0][0]) * alpha;
                colors[3][1] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][2] = (colors[0][2] + colors[1][2] + colors[2][2]) * alpha;
                counter++; // increase counter
            } else {
                // set pixel values
                colors[3][0] = (colors[0][0]) * alpha;
                colors[3][2] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][1] = (colors[0][2] + colors[1][2] + colors[2][2]) * alpha;
            }
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);// grab current pixel
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
}

// Blank
// takes cv::Mat reference
void ac::Blank(cv::Mat &frame) {
    static double alpha = 1.0f; // static alpha set to 1.0
    static unsigned char val[3] = { 0 };// val array set to zero
    static bool color_switch = false;// color switch set to false
    for(int z = 0; z < frame.cols; ++z) {// left to right
        for(int i = 0; i < frame.rows; ++i) { // top to bottom
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z); // current pixel
            for(int j = 0; j < 3; ++j) {
                // process pixel values
                val[j] = (alpha*color_value[j]) / (2-j+1);
                color_value[j] += val[2-j] / (j+1);
                if(color_switch == true) color_value[j] = -color_value[j];
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
}
// Tri
// takes cv::Mat reference
void ac::Tri(cv::Mat &frame) {
    static double alpha = 1.0f;// static alpha set to 1
    for(int z = 0; z < frame.cols-3; ++z) {// left to right
        for(int i = 0; i < frame.rows-3; ++i) {// top to bottom
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);// current pixel
            cv::Vec3b colors[2];// colors
            // grab pixels
            colors[0] = frame.at<cv::Vec3b>(i+1, z);
            colors[1] = frame.at<cv::Vec3b>(i+2, z);
            // set pixels
            color_value[0] += color_value[0]*alpha;
            color_value[1] += color_value[1]+colors[0][1]+colors[1][1]*alpha;
            color_value[2] += color_value[2]+colors[0][2]+colors[1][2]*alpha;
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
}
// Distort
// takes cv::Mat reference
void ac::Distort(cv::Mat &frame) {
    static double alpha = 1.0f; // static alpha set to 1
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) { // left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            // set pixel values
            color_value[0] = (i*alpha)+color_value[0];
            color_value[2] = (z*alpha)+color_value[2];
            color_value[1] = (alpha*color_value[1]);
            if(strobe_It == true) color_value[1] = ((i+z)*alpha)+color_value[1];
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
}
// takes cv::Mat reference
void ac::CDraw(cv::Mat &frame) {
    static int i=0,z=0;// loop variables
    static double rad = 80.0f;// radius
    static double deg = 1.0f;// degrees
    for(z = 0; z < frame.cols; ++z) { // left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            int cX = int(rad * cosf(deg));
            int cY = int(rad * sinf(deg));
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z); // grab pixel reference
            // set values
            color_value[0] = color_value[0]*(cX * alpha);
            color_value[1] = color_value[1]*(cY * alpha);
            color_value[2] = color_value[2]*alpha;
            deg += 0.1f;
            swapColors(frame, i, z);// swap
            if(isNegative) invert(frame, i, z);// if isNegative invert
        }
    }
    alpha += 0.1f;// add to alpha
    rad += 0.1f;// add to rad
    if(rad > 90) rad = 0;// greater than 90 reset
    if(alpha > 20) alpha = 0;// greater than 20 reset
}
// Light Strobe
// first cycle through the image
// add a variable to each individual pixel (the input sould be different each frame)
// reason for this is adding to the captured image each frame causes a animation a distortion
// each frame the largest value is calculated by adding the rgb values together for one element each iteration.
// test this first
void ac::Type(cv::Mat &frame) {
    signed int i = 0, z = 0;// loop variables
    static double add_r = 1.0; // add_r
    static int off = 0;// off variable
    for(z = 0; z < frame.rows; ++z) { // top to bottom
        for(i = 0; i < frame.cols; ++i) {// left to right
            cv::Vec3b &current = frame.at<cv::Vec3b>(z, i); // grab pixel reference
            // set pixel values
            current[0] += add_r+current[0];
            current[1] += add_r+current[1];
            current[2] += add_r+current[2];
            // set value indexed by off which changes each frame
            current[off] = current[0]+current[1]+current[2];
            swapColors(frame, i, z);// swap the colors
            if(isNegative) invert(frame, i, z); // invert pixel
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
            cv::Vec3b &colv = frame.at<cv::Vec3b>(i, z);// get pixels
            cv::Vec3b &cola = frame.at<cv::Vec3b>((frame.rows-1)-i, (frame.cols-1)-z);//frame.at<cv::Vec3b>((frame.cols-1)-z, (frame.rows-1)-i);
            // set arrays
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            // loop through arrays
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            // set pixel values
            colv[0] += alpha*R;
            colv[1] += alpha*G;
            colv[2] += alpha*B;
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
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);// grab pixel
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
            cv::Vec3b &frame_one = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b &frame_two = blendW_frame.at<cv::Vec3b>(q, j);
            // set pixel values
            for(int p = 0; p < 3; ++p)
                frame_one[p] += (frame_one[p]*alpha)+(frame_two[p]*beta);
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
// triBlend with Image unused
void ac::triBlendWithImage(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f, beta = 1.0f;
    static int i = 0, z = 0, j = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b colors[3];
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cx[3] =  { ac::GetFX(image_files[0], i, frame.rows), ac::GetFX(image_files[1], i, frame.rows), ac::GetFX(image_files[2], i, frame.rows) };
            int cy[3] =  { ac::GetFY(image_files[0], z, frame.cols),  ac::GetFY(image_files[1], z, frame.cols), ac::GetFY(image_files[2], z, frame.cols) };
            for(j = 0; j < 3; ++j) {
                colors[j] = image_files[j].at<cv::Vec3b>(cx[j], cy[j]);
            }
            color_value[0] = ((color_value[0]+colors[0][0])/2)*alpha;
            color_value[1] += ((color_value[1]+colors[0][1])/2)*alpha;
            color_value[2] = ((color_value[2]+colors[0][2]+colors[1][2]+colors[2][2])/3)*beta;
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
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

// Image Strobe - unused
void ac::imageStrobe(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f;
    static int i = 0, z = 0, j = 0, image_offset = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b colors[3];
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cx[3] =  { ac::GetFX(image_files[0], i, frame.rows), ac::GetFX(image_files[1], i, frame.rows), ac::GetFX(image_files[2], i, frame.rows) };
            int cy[3] =  { ac::GetFY(image_files[0], z, frame.cols),  ac::GetFY(image_files[1], z, frame.cols), ac::GetFY(image_files[2], z, frame.cols) };
            for(j = 0; j < 3; ++j)
                colors[j] = image_files[j].at<cv::Vec3b>(cx[j], cy[j]);
            for(j = 0; j < 3; ++j)
                color_value[j] += (colors[image_offset][j]*alpha);
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    ++image_offset;
    if(image_offset >= 4) image_offset = 0;
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
}
// Image distraction - unused
void ac::imageDistraction(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f;
    static int i = 0, z = 0, im_off = 2;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cX = GetFX(image_files[im_off], i, frame.rows), cY = GetFY(image_files[im_off], z, frame.cols);
            cv::Vec3b manip_color = image_files[im_off].at<cv::Vec3b>(cX, cY);
            color_value[0] = (z*alpha)+color_value[0];
            color_value[1] = manip_color[1]*alpha;
            color_value[2] = (i*alpha)+color_value[2];
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    ++im_off;
    if(im_off >= 4) im_off = 0;
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

// Cos Sin Mulitply draw gradients
// takes cv::Mat reference
void ac::cossinMultiply(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z); // grab pixel
            // set pixel values
            buffer[0] += 1+(sinf(alpha))*z;
            buffer[1] += 1+(cosf(alpha))*i;
            buffer[2] += (buffer[0]+buffer[1]+buffer[2])/3;
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// invert pixel
        }
    }
    // add alpha up to 24 return to zero when greater
    static double trans_var = 0.05f;
    if(alpha > 24) alpha = 1.0f;
    alpha += trans_var;
}
// Color Accumulate 1
void ac::colorAccumulate1(cv::Mat &frame) {
    static double alpha = 1.0f; // alpha to 1.0
    static int i = 0, z = 0; // static loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);// current pixel
            // set pixel values
            buffer[0] += (buffer[2]*alpha);
            buffer[1] += (buffer[0]*alpha);
            buffer[2] += (buffer[1]*alpha);
            swapColors(frame, i, z); // swap colors
            if(isNegative) invert(frame, i, z);// invert pixel
        }
    }
    // increase alpha until 24 then reset
    static double trans_var = 0.05f;
    alpha += trans_var;
    if(alpha > 24) alpha = 1.0f;
}
// Color Accumulate 2
void ac::colorAccumulate2(cv::Mat &frame) {
    static double alpha = 1.0f;// static alpha set to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            // grab pixel
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            // set pixel rgb values
            buffer[0] += (buffer[2]*alpha)+i;
            buffer[1] += (buffer[0]*alpha)+z;
            buffer[2] += (buffer[1]*alpha)+i-z;
            swapColors(frame, i, z);// swap
            if(isNegative) invert(frame, i, z);// if isNegative invert
        }
    }
    static double trans_var = 0.05f;// translation variable
    alpha += trans_var;// alpha plus equal translation variable
    if(alpha > 24) alpha = 1.0f;// if alpha greater than 24 reset to 1
}
// Color Accumulate #3
// takes cv::Mat reference
void ac::colorAccumulate3(cv::Mat &frame) {
    static double alpha = 1.0f;// set alpha to 1.0
    static int i = 0, z = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// from left to right
        for(i = 0; i < frame.rows; ++i) {// from top to bottom
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);// grab pixel reference
            // set rgb values
            buffer[0] = (-buffer[2])+z;
            buffer[1] = (-buffer[0])+i;
            buffer[2] = (-buffer[1])+alpha;
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if isNegative true invert pixel
        }
    }
    static double trans_var = 0.05f;// 0.05 variable
    alpha += trans_var;// alpha plus equal translation variable
    if(alpha > 24) alpha = 1.0f;// alpha greater than 24 set to 1 (reset)
}

// takes cv::Mat reference
void ac::filter8(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0, q = 0;// loop variable
    for(z = 0; z < frame.cols; ++z) {// from left to right
        for(i = 0; i < frame.rows; ++i) {// from top to bottom
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);// grab pixel
            for(q = 0; q < 3; ++q) {// loop each rgb value
                buffer[q] = buffer[q]+((i+z)*alpha);// preform calculation
                
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
}

// takes cv::Mat reference
void ac::filter3(cv::Mat &frame) {
    static double alpha = 1.0f;// set static alpha to 1.0
    static int i = 0, z = 0, q = 0;// loop variables
    for(z = 0; z < frame.cols; ++z) {// left to right
        for(i = 0; i < frame.rows; ++i) {// top to bottom
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);// grab pixel reference
            for(q = 0; q < 3; ++q) {// loop through rgb values
                buffer[q] = buffer[0]+(buffer[q])*(alpha);// preform calculation
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            // add to rgb values alpha * rb,gb,bb variables
            buffer[0] += alpha*rb;
            buffer[1] += alpha*gb;
            buffer[2] += alpha*bb;
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);// pixel at
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            // set pixel RGB values
            buffer[0] = buffer[2]+(1+(i*z)/pos);
            buffer[1] = buffer[1]+(1+(i*z)/pos);
            buffer[2] = buffer[0]+(1+(i*z)/pos);
            swapColors(frame, i, z);// swap colors
            if(isNegative) invert(frame, i, z);// if(isNegative) invert pixel
        }
    }
    static unsigned int dir = 1;// static direction equals 1
    if(dir == 1) {// dir equals 1
        pos += 25;// pos plus equal 25
        if(pos > 1024) {// greater than 1024
            pos = 1024;
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b buf = buffer;// temp pixel
            // set RGB pixel values
            buffer[0] = (buf[0]*pos)+(buf[0]-buffer[2]);
            buffer[1] = (buf[1]*pos)+(buf[1]+buffer[1]);
            buffer[2] = (buf[2]*pos)+(buf[2]-buffer[0]);
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
}
// glitchSort
// takes cv::Mat reference
void ac::glitchSort(cv::Mat &frame) {
    static double pos = 1.0f; // static pos set to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// static vector of unsigned int
    v.reserve(w);// reserve at least w bytes
    for(int z = 0; z < h; ++z) {// top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            // grab current pixel value reference
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i);
            // temporary unsigned int variable
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
        std::sort(v.begin(), v.end());// sort the row of pixels
        for(int i = 0; i < w; ++i) {// left to right
            // pointer to unsigned integer stored at index i
            unsigned char *value = (unsigned char*)&v[i];
            // grab current pixel reference as cv::Vec3b
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // alphablend pixel with values from v at index i
            pixel[0] = pixel[0] + (pos)*value[0];
            pixel[1] = pixel[1] + (pos)*value[1];
            pixel[2] = pixel[2] + (pos)*value[2];
            // swap the colors
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i); // if isNegative invert pixel
            
        }
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
    static std::vector<unsigned int> v;// static vector of unsigned int
    v.reserve(w);// reserve w bytes
    for(int z = 0; z < h; ++z) { // top to bottom
        for(int i = 0; i < w; ++i) { // left to right
            //unsigned int value = frame.at<unsigned int>(z, i);
            // grab pixel reference
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i);
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
        for(int i = 0; i < w; ++i) {// left to right
            // unsigned char pointer of vector v at index i
            unsigned char *value = (unsigned char*)&v[i];
            // get pixel reference
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // add to pixel without scaling
            pixel[0] += value[0];
            pixel[1] += value[1];
            pixel[2] += value[2];
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// invert pixel
        }
        v.erase(v.begin(), v.end());
    }
}
// preform a random filter
void ac::randomFilter(cv::Mat &frame) {
    int num;
    do {
        num = rand()%(draw_max-6);
    } while(ac::draw_strings[num] == "Random Filter");
    draw_func[num](frame);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // calculate RGB values
            pixel[0] += pos*random_r;
            pixel[1] += pos*random_g;
            pixel[2] += pos*random_b;
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

// variables for changePixel
int current_filterx = 0;
int bytesPerSample = 0;
int bytesPerRow = 0;
int width = 0;
int height = 0;
int red = 0;
int green = 0;
int blue = 0;
int offset = 0;
int randomNumber = 0;
int reverse = 0;
bool negate = false;

// changePixel for Alpha Flame Filters
// this function is called once for each pixel in the source image
void changePixel(cv::Mat &full_buffer, int i, int z, cv::Vec3b &buffer, double pos, double *count) {
    //each case is a different operation on the RGB pixel values stored in buffer
    switch(current_filterx) {
        case 0:
        {
            double value = pos;
            buffer[0] = (unsigned char) value*buffer[0];
            buffer[1] = (unsigned char) value*buffer[1];
            buffer[2] = (unsigned char) value*buffer[2];
        }
            break;
        case 1:
        {
            double value = pos;
            buffer[0] = (unsigned char) value*buffer[0];
            buffer[1] = (unsigned char) (-value)*buffer[1];
            buffer[2] = (unsigned char) value*buffer[2];
        }
            break;
        case 2:
        {
            buffer[0] += buffer[0]*-pos;
            buffer[1] += buffer[1]*pos;
            buffer[2] += buffer[2]*-pos;
        }
            break;
        case 3:
        {
            int current_pos = pos*0.2f;
            buffer[0] = (i*current_pos)+buffer[0];
            buffer[2] = (z*current_pos)+buffer[2];
            buffer[1] = (current_pos*buffer[1]);
        }
            break;
        case 4:
        {
            int current_pos = pos*0.2f;
            buffer[0] = (z*current_pos)+buffer[0];
            buffer[1] = (i*current_pos)+buffer[1];
            buffer[2] = ((i+z)*current_pos)+buffer[2];
        }
            break;
        case 5:
        {
            int current_pos = pos*0.2f;
            buffer[0] = -(z*current_pos)+buffer[0];
            buffer[1] = -(i*current_pos)+buffer[1];
            buffer[2] = -((i+z)*current_pos)+buffer[2];
        }
            break;
            
        case 6:
        {
            int zq = z+1, iq = i+1;
            if(zq > height-1 || iq > width-1) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            buffer[0] += (i*pos)+temp[0];
            buffer[1] += (z*pos)+temp[1];
            buffer[2] += (i/(z+1))+temp[2];
        }
            break;
        case 7:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            cola[0] = buffer[2];
            cola[1] = buffer[1];
            cola[2] = buffer[0];
            unsigned int alpha = (int)pos;
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += alpha*R;
            buffer[1] += alpha*G;
            buffer[2] += alpha*B;
        }
            break;
        case 8:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            int iq = (width-i-1);
            int zq = (height-z-1);
            cv::Vec3b &t = full_buffer.at<cv::Vec3b>(zq, iq);
            cola[0] = t[0];
            cola[1] = t[1];
            cola[2] = t[2];
            unsigned int alpha = (int)pos;
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += alpha*R;
            buffer[1] += alpha*G;
            buffer[2] += alpha*B;
        }
            break;
        case 9:
        {
            double alpha = pos;
            unsigned char colorz[3][3];
            colorz[0][0] = buffer[0];
            colorz[0][1] = buffer[1];
            colorz[0][2] = buffer[2];
            int total_r = colorz[0][0] +colorz[0][1]+colorz[0][2];
            total_r /= 3;
            total_r *= alpha;
            int iq = i+1;
            if(iq > width) return;
            int zq = z;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            colorz[1][0] = temp[0];
            colorz[1][1] = temp[1];
            colorz[1][2] = temp[2];
            int total_g = colorz[1][0]+colorz[1][1]+colorz[1][2];
            total_g /= 3;
            total_g *= alpha;
            buffer[0] = (unsigned char)total_r;
            buffer[1] = (unsigned char)total_g;
            buffer[2] = (unsigned char)total_r+total_g*alpha;
            
        }
            break;
        case 10:
        {
            buffer[0] = ((i+z)*pos)/(i+z+1)+buffer[0]*pos;
            buffer[1] += ((i*pos)/(z+1))+buffer[1];
            buffer[2] += ((z*pos)/(i+1))+buffer[2];
        }
            break;
        case 11:
        {
            buffer[0] += (buffer[2]+(i*pos))/(pos+1);
            buffer[1] += (buffer[1]+(z*pos))/(pos+1);
            buffer[2] += buffer[0];
        }
            break;
        case 12:
        {
            buffer[0] += (i/(z+1))*pos+buffer[0];
            buffer[1] += (z/(i+1))*pos+buffer[1];
            buffer[2] += ((i+z)/(pos+1)+buffer[2]);
        }
            break;
        case 13:
        {
            buffer[0] += (pos*(i/(pos+1))+buffer[2]);
            buffer[1] += (pos*(z/(pos+1))+buffer[1]);
            buffer[2] += (pos*((i*z)/(pos+1)+buffer[0]));
        }
            break;
        case 14:
        {
            buffer[0] = ((i+z)*pos)/(i+z+1)+buffer[0]*pos;
            buffer[1] += (buffer[1]+(z*pos))/(pos+1);
            buffer[2] += ((i+z)/(pos+1)+buffer[2]);
        }
            break;
        case 15:
        {
            buffer[0] = (i%(z+1))*pos+buffer[0];
            buffer[1] = (z%(i+1))*pos+buffer[1];
            buffer[2] = (i+z%((int)pos+1))+buffer[2];
        }
            break;
        case 16:
        {
            unsigned int r = 0;
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[0] += pos*r;
            buffer[1] += -(pos*r);
            buffer[2] += pos*r;
        }
            break;
        case 17:
        {
            unsigned long r = 0;;
            r = (buffer[0]+buffer[1]+buffer[2])/(pos+1);
            buffer[0] += r*pos;
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[1] += r*pos;
            r = (buffer[0]+buffer[1]+buffer[2])/5;
            buffer[2] += r*pos;
        }
            break;
        case 18:
        {
            buffer[0] += 1+(sinf(pos))*z;
            buffer[1] += 1+(cosf(pos))*i;
            buffer[2] += (buffer[0]+buffer[1]+buffer[2])/3;
        }
            break;
        case 19:
        {
            buffer[0] += (buffer[2]-i)*((((int)pos+1)%15)+2);
            buffer[1] += (buffer[1]-z)*((((int)pos+1)%15)+2);
            buffer[2] += buffer[0]-(i+z)*((((int)pos+1)%15)+2);
        }
            break;
        case 20:
        {
            buffer[0] += (buffer[0]+buffer[1]-buffer[2])/3*pos;
            buffer[1] -= (buffer[0]-buffer[1]+buffer[2])/6*pos;
            buffer[2] += (buffer[0]-buffer[1]-buffer[2])/9*pos;
        }
            break;
        case 21:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            int ir, ig, ib;
            ir = buffer[0]+temp[0]-temp2[0];
            ig = buffer[1]-temp[1]+temp2[1];
            ib = buffer[2]-temp[2]-temp2[2];
            if(z%2 == 0) {
                if(i%2 == 0) {
                    buffer[0] = ir+(0.5*pos);
                    buffer[1] = ig+(0.5*pos);
                    buffer[2] = ib+(0.5*pos);
                } else {
                    buffer[0] = ir+(1.5*pos);
                    buffer[1] = ig+(1.5*pos);
                    buffer[2] = ib+(1.5*pos);
                }
            } else {
                if(i%2 == 0) {
                    buffer[0] += ir+(0.1*pos);
                    buffer[1] += ig+(0.1*pos);
                    buffer[2] += ib+(0.1*pos);
                } else {
                    buffer[0] -= ir+(i*pos);
                    buffer[1] -= ig+(z*pos);
                    buffer[2] -= ib+(0.1*pos);
                }
            }
        }
            break;
        case 22:
        {
            if((i%2) == 0) {
                if((z%2) == 0) {
                    buffer[0] = 1-pos*buffer[0];
                    buffer[2] = (i+z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i-z)*pos;
                }
            } else {
                if((z%2) == 0) {
                    buffer[0] = pos*buffer[0]-i;
                    buffer[2] = (i-z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i+z)*pos;
                }
            }
        }
            break;
        case 23:
        {
            buffer[0] = buffer[0]+buffer[1]*2+pos;
            buffer[1] = buffer[1]+buffer[0]*2+pos;
            buffer[2] = buffer[2]+buffer[0]+pos;
        }
            break;
        case 24:
        {
            buffer[0] += buffer[2]+pos;
            buffer[1] += buffer[1]+pos;
            buffer[2] += buffer[0]+pos;
        }
            break;
        case 25:
        {
            buffer[0] += (buffer[2]*pos);
            buffer[1] += (buffer[0]*pos);
            buffer[2] += (buffer[1]*pos);
        }
            break;
        case 26:
        {
            buffer[0] += (buffer[2]*pos)+i;
            buffer[1] += (buffer[0]*pos)+z;
            buffer[2] += (buffer[1]*pos)+i-z;
        }
            break;
        case 27:
        {
            buffer[0] = (-buffer[2])+z;
            buffer[1] = (-buffer[0])+i;
            buffer[2] = (-buffer[1])+pos;
        }
            break;
        case 28:
        {
            buffer[0] = buffer[2]+(1+(i*z)/pos);
            buffer[1] = buffer[1]+(1+(i*z)/pos);
            buffer[2] = buffer[0]+(1+(i*z)/pos);
        }
            break;
        case 29:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+3;
            if(zq > height-2) return;
            cv::Vec3b &temp3 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+4;
            if(zq > height-2) return;
            cv::Vec3b &temp4 = full_buffer.at<cv::Vec3b>(zq, iq);
            unsigned char col[4];
            col[0] = (temp[0]+temp2[0]+temp3[0]+temp4[0])/4;
            col[1] = (temp[1]+temp2[1]+temp3[1]+temp4[1])/4;
            col[2] = (temp[2]+temp2[2]+temp3[2]+temp4[2])/4;
            buffer[0] = col[0]*pos;
            buffer[1] = col[1]*pos;
            buffer[2] = col[2]*pos;
        }
            break;
        case 30:
        {
            double rad = 100.0;
            double degree = 0.01*pos;
            int x = (int)rad * cos(degree);
            int y = (int)rad * sin(degree);
            int z = (int)rad * tanf((double)degree);
            buffer[0] = buffer[0]+x;
            buffer[2] = buffer[1]+y;
            buffer[1] = buffer[1]+z;
        }
            break;
        case 31:
        {
            int average= (buffer[0]+buffer[1]+buffer[2]+1)/3;
            buffer[0] += buffer[2]+average*(pos);
            buffer[1] += buffer[0]+average*(pos);
            buffer[2] += buffer[1]+average*(pos);
        }
            break;
        case 32:
        {
            unsigned int value = 0;
            value  = ~buffer[0] + ~buffer[1] + ~buffer[2];
            value /= 2;
            buffer[0] = buffer[0]+value*pos;
            value /= 2;
            buffer[1] = buffer[1]+value*pos;
            value /= 2;
            buffer[2] = buffer[2]+value*pos;
        }
            break;
        case 33:
        {
            buffer[0] += *count*pos;
            buffer[1] += *count*pos;
            buffer[2] += *count*pos;
            *count += 0.00001f;
            if(*count > 255) *count = 0;
        }
            break;
        case 34:
        {
            buffer[0] += pos*(randomNumber+pos);
            buffer[1] += pos*(randomNumber+z);
            buffer[2] += pos*(randomNumber+i);
        }
            break;
        case 35:
        {
            buffer[0] += *count *z;
            buffer[1] += *count *pos;
            buffer[2] += *count *z;
            *count += 0.0000001f;
        }
            break;
        case 36:
        {
            buffer[0] += sinf(3.14+pos)*pos;
            buffer[1] += cosf(3.14+pos)*pos;
            buffer[2] += tanf(3.14+pos)*pos;
        }
            break;
    }
    buffer[0] += red;
    buffer[1] += green;
    buffer[2] += blue;
    if(negate == true) {
        buffer[0] = ~buffer[0];
        buffer[1] = ~buffer[1];
        buffer[2] = ~buffer[2];
    }
    unsigned char buf[3];
    buf[0] = buffer[0];
    buf[1] = buffer[1];
    buf[2] = buffer[2];
    switch(reverse) {
        case 0://normal
            break;
        case 1:
            buffer[0] = buf[2];
            buffer[1] = buf[1];
            buffer[2] = buf[0];
            break;
        case 2:
            buffer[0] = buf[1];
            buffer[1] = buf[2];
            buffer[2] = buf[0];
            break;
        case 3:
            buffer[0] = buf[2];
            buffer[1] = buf[0];
            buffer[2] = buf[1];
            break;
        case 4:
            buffer[0] = buf[1];
            buffer[1] = buf[0];
            buffer[2] = buf[2];
            break;
    }
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            // call change pixel function
            changePixel(frame, z, i, buffer, pos, &count);
        }
    }
    // static direction set to 1
    static int direction = 1;
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

// Image blend
// cv::Mat as reference
void ac::imageBlend(cv::Mat &frame) {
    static double pos = 1.0f;// static pos set to 1
    if(blend_set == true) {// if image is set
        int i,z;
        for(i = 0; i < frame.cols; ++i) { // top to bottom
            for(z = 0; z < frame.rows; ++z) {// left to right
                // get resized x,y
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);// get current pixel
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);// get pixel to blend from resized x,y
                // set pixel values
                current[0] = current[0]+(im[0]*pos);
                current[1] = current[1]+(im[1]*pos);
                current[2] = current[2]+(im[2]*pos);
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// invert pixel
            }
        }
    }
    static double pos_max = 7.0f;// max pos value
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// takes cv::Mat reference
void ac::imageBlendTwo(cv::Mat &frame) {
    static double pos = 1.0f; // static pos equal 1.0
    if(blend_set == true) {// if image is set to blend with
        int i,z;// loop variables
        for(i = 0; i < frame.cols; ++i) { // left to right
            for(z = 0; z < frame.rows; ++z) {// top to bottom
                // resize x,y
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // grab pixels
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                // set pixel values
                current[0] = im[0]+(current[0]*pos);
                current[1] = im[1]+(current[1]*pos);
                current[2] = im[2]+(current[2]*pos);
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
    }
    static double pos_max = 7.0f;// max position set to 7.0
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// blend with Image
// takes cv::Mat reference
void ac::imageBlendThree(cv::Mat &frame) {
    if(blend_set == true) { // if blend_set is true (image selected)
        static double pos = 1.0f;// static pos equals 1.0
        for(int i = 0; i < frame.cols; ++i) { // from top to bottom
            for(int z = 0; z < frame.rows; ++z) {// from left to right
                // calculate x,y pixel position
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // get pixel to manipulate reference
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                // get image pixel from calculated x,y positions
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                // calculate pixel data
                pixel[0] += (pixel[0]^im[0]);
                pixel[1] += (pixel[1]^im[1]);
                pixel[2] += (pixel[2]^im[2])*pos;
                swapColors(frame, z, i);//swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
                
            }
        }
        // static int directione quals 1
        static int direction = 1;
        // static pos_max equals 7.0
        static double pos_max = 7.0f;
        if(direction == 1) {// if direction equals 1
            pos += 0.005;// pos plus equal 0.005
            if(pos > pos_max) {// pos greater than pos_max
                pos = pos_max;// pos set to pos_max
                direction = 0;// direction set to zero
                pos_max += 0.5f;// pos_max plus equal 0.5
            }
        } else if(direction == 0) {// direction is set to 0
            pos -= 0.005;// pos minus equal 0.005
            if(pos <= 1) {/// pos less than equal 1
                if(pos_max > 15) pos_max = 1.0f;//reset pos_max if greater than 15
                direction = 1;// direction set to 1
            }
        }
    }
}

// imageblend4
void ac::imageBlendFour(cv::Mat &frame) {
    if(blend_set == true) {
        static unsigned int state = 0;
        static double pos = 1.0;
        int w = frame.cols;// frame width
        int h = frame.rows;// frame height
        int cw = blend_image.cols;
        int ch = blend_image.rows;
        for(int z = 3; z < h-3; ++z) {// top to bottom
            for(int i = 3; i < w-3; ++i) {// left to right
                // current pixel by reference
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                // calculate resized image based x,y positions
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // grab pixel refernces from blend_image
                cv::Vec3b &pr = blend_image.at<cv::Vec3b>((ch-cY), (cw-cX));
                cv::Vec3b &pg = blend_image.at<cv::Vec3b>((ch-cY), cX);
                cv::Vec3b &pb = blend_image.at<cv::Vec3b>(cY, (cw-cX));
                // perform operation based on current state variable
                switch(state) {
                    case 0:
                        pixel[0] += (pr[0]+pg[1]+pb[2])*pos;
                        pixel[1] += (pr[2]+pg[1]+pb[0])*pos;
                        break;
                    case 1:
                        pixel[1] += (pr[2]+pg[1]+pb[0])*pos;
                        pixel[2] += (pr[0]+pg[1]+pb[2])*pos;
                        break;
                    case 2:
                        pixel[2] += (pr[0]+pg[1]+pb[2])*pos;
                        pixel[0] += (pr[2]+pg[1]+pb[0])*pos;
                        break;
                }
                
            }
            
        }
        ++state;// increase state
        if(state > 2) state = 0; // greater than 2 reset state
        static int direction = 1;
        // static pos_max equals 7.0
        static double pos_max = 3.0f;
        if(direction == 1) {// if direction equals 1
            pos += 0.005;// pos plus equal 0.005
            if(pos > pos_max) {// pos greater than pos_max
                pos = pos_max;// pos set to pos_max
                direction = 0;// direction set to zero
                pos_max += 0.5f;// pos_max plus equal 0.5
            }
        } else if(direction == 0) {// direction is set to 0
            pos -= 0.005;// pos minus equal 0.005
            if(pos <= 1) {/// pos less than equal 1
                if(pos_max > 3) pos_max = 1.0f;//reset pos_max if greater than 15
                direction = 1;// direction set to 1
            }
        }
    }
}

// preform GaussianBlur
void ac::GaussianBlur(cv::Mat &frame) {
    cv::Mat out;
    cv::GaussianBlur(frame, out, cv::Size(5, 5), 0, 0);
    frame = out;
}
// preform MedianBlur
void ac::MedianBlur(cv::Mat &frame) {
    cv::Mat out;
    cv::medianBlur(frame, out, 5);
    frame = out;
}
// Increase / Decrease GaussianBlur
// takes cv::Mat reference
void ac::BlurDistortion(cv::Mat &frame) {
    cv::Mat out;// output
    static unsigned int index = 1, direction = 1;
    cv::GaussianBlur(frame, out, cv::Size(index, index), 0, 0);// output
    if(direction == 1) {// if direction equals 1
        if(index >= 51) direction = 0;// if greater than 51 set to zero go
        // opposite direction
        else index += 2;// increase
    } else {
        if(index <= 1) direction = 1;// go opposite direction
        else index -= 2;// decrease
    }
    frame = out;// frame equals out
}

// Draw gradient diamonds that grow and shrink and blend with source image
// takes cv::Mat reference
void ac::DiamondPattern(cv::Mat &frame) {
    static double pos = 1.0;// set pos to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    for(int z = 0; z < h; ++z) {// from top to bottom
        for(int i = 0; i < w; ++i) {// from left to right
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);// get current pixel
            // calculate the colors of the gradient diamonds
            if((i%2) == 0) {// if i % 2 equals 0
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[0] = 1-pos*buffer[0];
                    buffer[2] = (i+z)*pos;
                } else {
                    // set pixel coomponent values
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i-z)*pos;
                }
            } else {
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[0] = pos*buffer[0]-i;
                    buffer[2] = (i-z)*pos;
                } else {
                    // set pixel component values
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i+z)*pos;
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i); // get pixel at i,z
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));// get pixel at w-i, h-z
            // set pixel rgb components
            buffer[0] += pix1[0]*pos;
            buffer[1] += pix1[1]*pos;
            buffer[2] += pix1[2]*pos;
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i); // invert if isNegative true
        }
    }
    // static direction variable
    static int direction = 1;
    static double pos_max = 2.0f; // position maximum
    if(direction == 1) {// if direction is equal to 1
        pos += 0.1;// pos plus equal 0.1
        if(pos > pos_max) {// pos greater than pos max
            pos = pos_max;// pos = pos max
            direction = 0;// direction equals 0
            pos_max += 1.0f;// pos max pluse qual 1.0
        }
    } else if(direction == 0) {// if direction equals zero
        pos -= 0.1;// pos plus equal 0.1
        if(pos <= 1.0) {// pos less than 1.0
            if(pos_max > 2.0f) pos_max = 1.0f;// pos max greater than 2, pos_max set to 1
            direction = 1;// direction set back to 1
        }
    }
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            // pixel rgb components plus equal multiplied by pos
            buffer[0] += buffer[0]*pos;
            buffer[1] += buffer[1]*pos;
            buffer[2] += buffer[2]*pos;
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            // h minus y, width minus x positioned pixel
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            // y and width minus x pixel
            cv::Vec3b &pix2 = orig.at<cv::Vec3b>(z, (w-i));
            // current pixel compponents equal
            // pix1[0] plus pix2[0] multiplied by kernel
            buffer[0] += (pix1[0]+pix2[0])*pos;
            // do the same for each component
            buffer[1] += (pix1[1]+pix2[1])*pos;
            buffer[2] += (pix1[2]+pix2[2])*pos;
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);// current pixel
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
    unsigned int r = rand()%255; // random number betwen 0-254
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// vector for row of bytes info
    v.reserve(w);// reserve at least width bytes
    for(int z = 0; z < h; ++z) { //  loop: top to bottom
        for(int i = 0; i < w; ++i) { // loop: left ro right
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i); // current pixel
            unsigned int vv = 0; // unsigned integer
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);// pixel at i,z
            // pixel values plus equal value plus r
            pixel[0] += value[0]+r;
            pixel[1] += value[1]+r;
            pixel[2] += value[2]+r;
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);// grab pixel
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &g = orig.at<cv::Vec3b>((h-z), i); // pixel at h-y, x
            cv::Vec3b &b = orig.at<cv::Vec3b>(z, (w-i)); // pixel at y, w-x
            // this is what gives the diamond image
            if((i%2) == 0) {// if modulus i by two returns zero
                if((z%2) == 0) {// modulus z by two returns zero
                    buffer[2] += (i+z)*pos;// buffer[2] plus equals (i plus z) multiplied by pos
                } else {
                    buffer[2] += (i-z)*pos; // buffer[2] plus equals (i minus z) mulitplied by pos
                }
            } else {
                if((z%2) == 0) {// modulus z by two equals zero
                    buffer[2] += (i-z)*pos; // buffer[2] plus equals (i minus z) multiplied by pos
                } else {
                    buffer[2] += (i+z)*pos; // buffer[2] plus equals (i plus z) multiplied by pos
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // color RGB variables
            int color_R = start_color * 4, color_G = start_color * 6, color_B = start_color * 8;
            // add to pixel color
            pixel[0] += color_R;
            pixel[1] += color_G;
            pixel[2] += color_B;
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            // color RGB variables
            int color_R = start_color * 4, color_G = start_color * 6, color_B = start_color * 8;
            // add to pixel colors
            pixel[2] += color_R;
            pixel[1] += color_G;
            pixel[0] += color_B;
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
}
// Reverse Frame
// takes cv::Mat reference
void ac::Reverse(cv::Mat &frame) {
    cv::Mat output;//output matrix
    cv::flip(frame, output, 1); // flip image
    frame = output; // set frame to output
}
// Scanlines - Draws scanlines like a CRT.
void ac::Scanlines(cv::Mat &frame) {
    int w = frame.cols;// width
    int h = frame.rows;// height
    for(int z = 0; z < h; z += 2) {// top to bottom step by 2
        for(int i = 0; i < w; ++i) {// left to right
            cv::Vec3b &pix = frame.at<cv::Vec3b>(z, i);// current pixel
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
            cv::Vec3b &pix = frame.at<cv::Vec3b>(z, i);// current pixel
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b mir_pix[3]; // array of Vec3b variables
            mir_pix[0] = orig.at<cv::Vec3b>((h-z), (w-i)); // pixel at w-i, h-z
            mir_pix[1] = orig.at<cv::Vec3b>((h-z), i); // pixel at i, h-z
            mir_pix[2] = orig.at<cv::Vec3b>(z,(w-i)); // pixel at w-i, z
            // take each component from mir_pix and find the average
            // with the same index from each variable in the mir_pix array
            // then multiply it by the position index (pos) then add it
            // to current pixel
            pixel[0] += ((mir_pix[0][0]+mir_pix[1][0]+mir_pix[2][0])/3)*pos;
            pixel[1] += ((mir_pix[0][1]+mir_pix[1][1]+mir_pix[2][1])/3)*pos;
            pixel[2] += ((mir_pix[0][2]+mir_pix[1][2]+mir_pix[2][2])/3)*pos;
            
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i); // current pixel at i,z
            cv::Vec3b mir_pix[3]; // array of 3 cv::Vec3b vectors
            mir_pix[0] = orig.at<cv::Vec3b>((h-z), (w-i)); // pixel at w-i, h-z
            mir_pix[1] = orig.at<cv::Vec3b>((h-z), i); // pixel at i, h-z
            mir_pix[2] = orig.at<cv::Vec3b>(z,(w-i)); // pixel at w-i, z
            // take each pixel and average together mulitply by pos
            // and add its value to different components in
            // pixel reference vector
            pixel[0] += ((mir_pix[0][0]+mir_pix[0][1]+mir_pix[0][2])/3)*pos;
            pixel[1] += ((mir_pix[1][0]+mir_pix[1][1]+mir_pix[1][2])/3)*pos;
            pixel[2] += ((mir_pix[2][0]+mir_pix[2][1]+mir_pix[2][2])/3)*pos;
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
    static double pos = 1.0;
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    cv::Scalar s = cv::mean(frame);
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] += pos*s[0];
            pixel[1] += pos*s[1];
            pixel[2] += pos*s[2];
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    
    static double pos_max = 7.0;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

// Laplacian - takes cv::Mat reference
void ac::Laplacian(cv::Mat &frame) {
    cv::Mat out;
    cv::Laplacian(frame, out, CV_8U);
    frame = out;
}

// XOR - takes cv::Mat reference
void ac::Bitwise_XOR(cv::Mat &frame) {
    static cv::Mat initial = frame;
    if(initial.cols != frame.cols || initial.rows != frame.rows) {
        initial = frame;
    }
    cv::Mat start = frame.clone();
    cv::Mat output;
    cv::bitwise_xor(frame, initial, output);
    initial = start;
    frame = output;
}

// And takes cv::Mat reference
void ac::Bitwise_AND(cv::Mat &frame) {
    static cv::Mat initial = frame;
    if(initial.cols != frame.cols || initial.rows != frame.rows) {
        initial = frame;
    }
    cv::Mat start = frame.clone();
    cv::Mat output;
    cv::bitwise_and(frame, initial, output);
    initial = start;
    frame = output;
}
// takes cv::Mat reference
void ac::Bitwise_OR(cv::Mat &frame) {
    static cv::Mat initial = frame;
    if(initial.cols != frame.cols || initial.rows != frame.rows) {
        initial = frame;
    }
    cv::Mat start = frame.clone();
    cv::Mat output;
    cv::bitwise_or(frame, initial, output);
    initial = start;
    frame = output;
}
// takes cv::Mat reference
// Equalize image
void ac::Equalize(cv::Mat &frame) {
    cv::Mat output[3];
    std::vector<cv::Mat> v;
    cv::split(frame, v);
    cv::equalizeHist(v[0], output[0]);
    cv::equalizeHist(v[1], output[1]);
    cv::equalizeHist(v[2], output[2]);
    cv::merge(output,3,frame);
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
    for(int z = 0; z < frame.rows; ++z) { // top to bottom
        for(int i = 0; i < frame.cols; ++i) { // left to right
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i); // get reference to pixel
            cv::Vec3b &ch_pixel = output.at<cv::Vec3b>(z, i); // get reference to pixel
            // add and multiply components to channels
            pixel[0] += ch_pixel[0]*pos;
            pixel[1] += ch_pixel[1]*pos;
            pixel[2] += ch_pixel[2]*pos;
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pixels[4];
            pixels[0] = frame.at<cv::Vec3b>(z, i+1);
            pixels[1] = frame.at<cv::Vec3b>(z+1, i);
            pixels[2] = frame.at<cv::Vec3b>(z+1, i+1);
            pixel[0] ^= (pixels[0][0]+pixels[1][0]+pixels[2][0]);
            pixel[1] ^= (pixels[0][1]+pixels[1][1]+pixels[2][1]);
            pixel[2] ^= (pixels[0][2]+pixels[1][2]+pixels[2][2]);
            pixel[0] *= pos;
            pixel[1] *= pos;
            pixel[2] *= pos;
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
    static double x = 50, y = 10;
    cv::Canny(frame, out, x, y);
    cv::Mat converted;
    cv::cvtColor(out, converted, cv::COLOR_GRAY2BGR);
    frame = converted;
}

// Flip takes cv::Mat reference
// flip the iamge every other frame
void ac::FlipTrip(cv::Mat &frame) {
    static int _flip = 0;// index variable
    cv::Mat output;// output matrix
    switch(_flip){
        case 0:
            cv::flip(frame, output, 1); // flip matrix
            frame = output;// frame equals output
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int pixel_size = 8; // size of each tile
    for(unsigned int z = 0; z < h; z += pixel_size) { // from top to bottom
        for(unsigned int i = 0; i < w; i += pixel_size) { // from left to right
            unsigned char rgb[3]; // 3 bytes
            rgb[0] = rand()%255; // set to random number
            rgb[1] = rand()%255;
            rgb[2] = rand()%255;
            for(unsigned int y = z; y < z+pixel_size; ++y) { // tile top to bottom
                for(unsigned int x = i; x < i+pixel_size; ++x) {// tile left to right
                    if(x < w && y < h) { // is x,y on screen?
                        // reference to pixel
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
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
        if(pixel_size > (w/6)) direction = 0;
    } else if(direction == 0) {// direction equals zero shrink
        --pixel_size;// shrink
        if(pixel_size < 24) direction = 1;
    }
}
// Loop tiled box fade
void ac::BoxesFade(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static cv::Vec3b color(rand()%255, rand()%255, rand()%255); // random color
    static int sw = 0; // with component to increase
    static unsigned int pixel_size = 8; // size of each tile
    for(unsigned int z = 0; z < h; z += pixel_size) { // from top to bottom
        ++sw;// increase
        if(sw > 2) sw = 0;//greater than 2 reset
        for(unsigned int i = 0; i < w; i += pixel_size) { // from left to right
            for(unsigned int y = z; y < z+pixel_size; ++y) { // tile top to bottom
                for(unsigned int x = i; x < i+pixel_size; ++x) {// tile left to right
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
                        
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static cv::Vec3b black(0, 0, 0);
    static bool flash = false;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0;  i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(flash == true)
                pixel = black;
            
        }
    }
    flash = !flash;
}

void ac::SlideRGB(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static int offset_x = 0;
    int color[2] = { rand()%3, rand()%3 };
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(offset_x+i < (w-1)) {
                cv::Vec3b off_pix = frame.at<cv::Vec3b>(z, offset_x+i);
                pixel[color[0]] += off_pix[color[0]];
                cv::Vec3b off_red = frame.at<cv::Vec3b>(z, (w-(offset_x+i)));
                pixel[color[1]] += off_red[color[1]];
            }
        }
    }
    static unsigned int direction = 1;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 3.0;
    for(unsigned int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            total[0] += (pixel[0]/2);
            total[1] += (pixel[1]/2);
            total[2] += (pixel[2]/2);
            pixel[0] = pixel[0] + (total[0]*pos)*0.01;
            pixel[1] = pixel[1] + (total[1]*pos)*0.01;
            pixel[2] = pixel[2] + (total[2]*pos)*0.01;
            
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 5.0;
    for(unsigned int i = 0; i < w; ++i) {
        cv::Scalar total;
        for(unsigned int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            total[0] += (pixel[0]/2);
            total[1] += (pixel[1]/2);
            total[2] += (pixel[2]/2);
            pixel[0] = pixel[0] + (total[0]*pos)*0.01;
            pixel[1] = pixel[1] + (total[1]*pos)*0.01;
            pixel[2] = pixel[2] + (total[2]*pos)*0.01;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned color = 0;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 5.0;
    for(unsigned int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            total[0] += pixel[0] * 0.01;
            total[1] += pixel[1] * 0.01;
            total[2] += pixel[2] * 0.01;
            
            pixel[0] = pixel[0] + (total[0]) * (pos*0.1);
            pixel[1] = pixel[1] + (total[1]) * (pos*0.1);
            pixel[2] = pixel[2] + (total[2]) * (pos*0.1);
            
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] += (pos*offset[0]);
            pixel[1] += (pos*offset[1]);
            pixel[2] += (pos*offset[2]);
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
    static unsigned int offset = 1;
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    for(unsigned int z = 0; z < h; ++z) {
        unsigned int start = 0;
        for(unsigned int i = offset; i < w && start < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
            pixel[0] += source[0];
            pixel[1] += source[1];
            pixel[2] += source[2];
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(unsigned int i = 0; i < offset-1 && start < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
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
    static unsigned int max_up = (w/16);
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
    static unsigned int offset = 1;
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    for(unsigned int i = 0; i < w; ++i) {
        unsigned int start = 0;
        for(unsigned int z = offset; z < h && start < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
            pixel[0] += source[0]*pos;
            pixel[1] += source[1]*pos;
            pixel[2] += source[2]*pos;
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
        for(unsigned int z = 0; z < offset-1 && start < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &source = frame.at<cv::Vec3b>(z, start);
            pixel[0] += source[0]*pos;
            pixel[1] += source[1]*pos;
            pixel[2] += source[2]*pos;
            ++start;
            // swap colors
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static unsigned int max_up = (h/8);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
            pixel[0] = (pixel[0]^(int)s[0])*pos;
            pixel[1] = (pixel[1]^(int)s[1])*pos;
            pixel[2] = (pixel[2]^(int)s[2])*pos;
            
            swapColors(frame, y, x);
            if(isNegative) invert(frame, y, x);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max, 4.0);
}

void ac::BitwiseRotate(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int offset = 0;
    static int direction = 1;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int offset = 1;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int q = 0; q < 3; ++q)
                pixel[q] += (pixel[q]-ror(pixel[q], offset));
            
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++offset;
    if(offset > 7) offset = 1;
}

void ac::HPPD(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 4.0;
    for(unsigned int z = 0; z < h; ++z) {
        cv::Scalar total;
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            total[0] += pixel[0];
            total[1] += pixel[1];
            total[2] += pixel[2];
            pixel[0] = pixel[0]-total[0]*pos;
            pixel[1] = pixel[1]-total[1]*pos;
            pixel[2] = pixel[2]-total[2]*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::FuzzyLines(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 4.0;
    cv::Scalar prev_pixel;
    double value[3] = { 0 };
    
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b temp = pixel;
            
            value[0] += temp[0]+temp[1]+temp[2];
            value[1] -= temp[0]+temp[1]+temp[2];
            value[2] += temp[0]+temp[1]+temp[2];
            pixel[0] += (value[0]*pos)*0.001;
            pixel[1] += (value[1]*pos)*0.001;
            pixel[2] += (value[2]*pos)*0.001;
            
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int count = 0, index = 0;
    
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] += count;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int count = 0, index = 0;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = (pixel[index]*pos)+count;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int count = 0, index = 0;
    for(unsigned int i = 0; i < w; ++i) {
        for(unsigned int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = (pixel[index]*pos)+count;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int count = 0, index = 0;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = (pixel[index]*pos)+count;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int count = 0, index = 0;
    for(unsigned int i = 0; i < w; ++i) {
        for(unsigned int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = (pixel[index]*pos)+count;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int count = 0, index = 0;
    static unsigned int direction = 1;
    if(direction == 1) {
        for(unsigned int z = 0; z < h; ++z) {
            for(unsigned int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
        for(unsigned int i = 0; i < w; ++i) {
            for(unsigned int z = 0; z < h; ++z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int start_x = 0;
    for(unsigned int z = start_x; z < h; z += 2) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = pixel[1] = pixel[2] = 0;
        }
    }
    if(start_x == 0) start_x = 1; else start_x = 0;
}


void ac::UpDown(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    double alpha = 1.0;
    bool order = true;
    
    for(unsigned int i = 0; i < w; ++i) {
        if(order == true) {
            order = false;
            for(unsigned int z = 0; z < h; ++z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q)
                    pixel[q] = alpha+(pixel[q]*pos);
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            alpha += 0.1;
        } else {
            order = true;
            for(unsigned int z = h-1; z > 1; --z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q)
                    pixel[q] = alpha-(pixel[q]*pos);
                
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    double alpha = 1.0;
    bool order = true;
    for(unsigned int z = 0; z < h; ++z) {
        if(order == true) {
            order = false;
            for(unsigned int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q)
                    pixel[q] = alpha+(pixel[q]*pos);
                
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
            alpha += 0.1;
        } else {
            order = true;
            for(unsigned int i = w-1; i > 1; --i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int q = 0; q < 3; ++q)
                    pixel[q] = alpha-(pixel[q]*pos);
                
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static int color_mode = 0;
    unsigned int over = rand()%255;
    static unsigned int cdirection = 1;
    
    for(unsigned int z = 0; z < h; ++z) {
        switch(color_mode) {
            case 0: {
                for(unsigned int i = 0; i < w; ++i) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    pixel[color_mode] = over+(pixel[color_mode]*pos);
                    swapColors(frame, z, i);
                    if(isNegative) invert(frame, z, i);
                }
            }
                break;
            case 1: {
                for(unsigned int i = w-1; i > 1; --i) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    pixel[color_mode] -= over+(pixel[1]*pos);
                    swapColors(frame, z, i);
                    if(isNegative) invert(frame, z, i);
                    
                }
            }
                break;
            case 2: {
                for(unsigned int i = 0; i < w; ++i) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int cnt = 0;
    for(unsigned int z = 0; z < h; ++z) {
        int r = rand()%255;
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[cnt] += r;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int offset = 0, count = 0;
    unsigned int count_i = (rand()%0xFF)+(rand()%0xFFFFFF);//color offset
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[offset] += count;
            pixel[2-offset] -= count_i;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static cv::Scalar val(rand()%10, rand()%10, rand()%10);
    static double pos = 1.0, pos_max = 7.0;
    for(unsigned int i = 0; i < w; ++i) {
        for(unsigned int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] ^= static_cast<unsigned char>(sin(pixel[0])*val[0]);
            pixel[1] ^= static_cast<unsigned char>(sin(pixel[1])*val[1]);
            pixel[2] ^= static_cast<unsigned char>(sin(pixel[2])*val[2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    for(unsigned int q = 0; q < 3; ++q) {
        if(direction == 1)
            val[q] += pos;
        else
            val[q] -= pos;
    }
    procPos(direction, pos, pos_max);
}

class Square {
public:
    Square() : pos(0), width(0), height(0), x(0), y(0) {}
    void setSize(const int &xx, const int &yy, const int &w, const int &h) {
        x = xx;
        y = yy;
        if(width != w || height != h) {
            width = w;
            height = h;
            image.create(cvSize(w, h), CV_8UC3);
        }
    }
    void setPos(const int &p) {
        pos = p;
    }
    void copyImage(const cv::Mat &f) {
        for(int i = 0, src_x = x; i < width; ++i, ++src_x) {
            for(int z = 0, src_y = y; z < height; ++z, ++src_y) {
                cv::Vec3b &pixel = image.at<cv::Vec3b>(z, i);
                cv::Vec3b src = f.at<cv::Vec3b>(src_y, src_x);
                pixel = src;
            }
        }
    }
    void copyImageToTarget(int xx, int yy, cv::Mat &f) {
        for(int i = 0, dst_x = xx; i < width; ++i, ++dst_x) {
            for(int z = 0, dst_y = yy; z < height; ++z, ++dst_y) {
                cv::Vec3b &pixel = f.at<cv::Vec3b>(dst_y, dst_x);
                cv::Vec3b src = image.at<cv::Vec3b>(z, i);
                pixel = src;
            }
        }
    }
    
    int getPos() const { return pos; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
protected:
    int pos,width,height,x,y;
    cv::Mat image;
};

struct Point {
    int x, y;
};

void Square_Swap(Square *squares, int num_w, int num_h, cv::Mat &frame, bool random = false) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    unsigned int square_w=(w/num_w), square_h=(h/num_h);
    int pos = 0;
    Point points[num_w*num_h];
    std::vector<Square *> square_vec;
    for(int rx = 0; rx < num_w; ++rx) {
        for(int ry = 0; ry < num_h; ++ry) {
            int cx = rx*square_w;
            int cy = ry*square_h;
            points[pos].x = cx;
            points[pos].y = cy;
            squares[pos].setPos(pos);
            squares[pos].setSize(cx, cy, square_w, square_h);
            squares[pos].copyImage(frame);
            square_vec.push_back(&squares[pos]);
            ++pos;
        }
    }
    // shuffle instead of randomize
    if(random == false) std::random_shuffle(square_vec.begin(), square_vec.end());
    for(int i = 0; i < pos; ++i) {
        if(random == false)
            // use shuffled
            square_vec[i]->copyImageToTarget(points[i].x, points[i].y,frame);
        else
            // use random
            square_vec[rand()%pos]->copyImageToTarget(points[i].x, points[i].y,frame);
    }
}


// SquareSwap
void ac::SquareSwap(cv::Mat &frame) {
    static unsigned int cnt = 0;
    switch(cnt) {
        case 0:
            SquareSwap4x2(frame);
            break;
        case 1:
            SquareSwap8x4(frame);
            break;
        case 2:
            SquareSwap16x8(frame);
            break;
        case 3:
            SquareSwap64x32(frame);
            break;
    }
    ++cnt;
    if(cnt > 3) cnt = 0;
}

void ac::SquareSwap4x2(cv::Mat &frame) {
    const unsigned int num_w = 4, num_h = 2;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap8x4(cv::Mat &frame) {
    const unsigned int num_w = 8, num_h = 4;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap16x8(cv::Mat &frame) {
    const unsigned int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap64x32(cv::Mat &frame) {
    const unsigned int num_w = 64, num_h = 32;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareBars(cv::Mat &frame) {
    const unsigned int num_w = 16, num_h = 1;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareBars8(cv::Mat &frame) {
    const unsigned int num_w = 8, num_h = 1;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwapRand16x8(cv::Mat &frame) {
    const unsigned int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame, true);
}

void ac::SquareVertical8(cv::Mat &frame) {
    const unsigned int num_w = 1, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareVertical16(cv::Mat &frame) {
    const unsigned int num_w = 1, num_h = 16;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}


void ShiftSquares(std::vector<Square *> &s, int pos, bool direction=true) {
    if(direction == true) {
        for(unsigned int i = 0; i < s.size(); ++i) {
            int p = s[i]->getPos();
            if(p+1 > (int)s.size()-1) {
                s[i]->setPos(0);
            } else {
                ++p;
                s[i]->setPos(p);
            }
        }
    } else {
        for(int i = 0; i < pos; ++i) {
            int p = s[i]->getPos();
            --p;
            s[i]->setPos(p);
            if(p < 0) {
                s[i]->setPos(pos-1);
            }
        }
    }
}

void SquareVertical(const unsigned int num_w, const unsigned int num_h, Square *squares, cv::Mat &frame, bool direction=true) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    int square_w=(w/num_w), square_h=(h/num_h);
    int pos = 0;
    Point points[num_w*num_h];
    std::vector<Square *> square_vec;
    for(int rx = 0; rx < (int)num_w; ++rx) {
        for(int ry = 0; ry < (int)num_h; ++ry) {
            int cx = rx*square_w;
            int cy = ry*square_h;
            points[pos].x = cx;
            points[pos].y = cy;
            squares[pos].setSize(cx, cy, square_w, square_h);
            squares[pos].copyImage(frame);
            square_vec.push_back(&squares[pos]);
            ++pos;
        }
    }
    ShiftSquares(square_vec,pos,direction);
    for(int i = 0; i < pos; ++i) {
        const int p = square_vec[i]->getPos();
        square_vec[i]->copyImageToTarget(points[p].x, points[p].y, frame);
    }
}

void ac::SquareVertical_Roll(cv::Mat &frame) {
    const int num_w = 1, num_h = 20;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                ++cpos;
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame);
}

void ac::SquareSwapSort_Roll(cv::Mat &frame) {
    const int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                ++cpos;
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame);
}

void ac::SquareVertical_RollReverse(cv::Mat &frame) {
    const int num_w = 1, num_h = 20;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                cpos++;
                
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame, false);
}

void ac::SquareSwapSort_RollReverse(cv::Mat &frame) {
    const int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                ++cpos;
                squares[cpos].setPos(cpos);
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame,false);
}

void ac::Circular(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static double deg = 0.0;
    static double rad = 50;
    
    for(unsigned int i = 0; i < w; ++i) {
        for(unsigned int z = 0; z < h; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int X_color = int(rad * cos(deg));
            int Y_color = int(rad * sin(deg));
            pixel[0] += pos*X_color;
            pixel[1] *= pos;
            pixel[2] += pos*Y_color;
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static unsigned int pixel_count = 0;
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            if(pixel_count == 4) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static cv::Mat stored_frame;
    if((frame.rows != stored_frame.rows) || (frame.cols != stored_frame.cols)) {
        stored_frame = frame.clone();
    }
    cv::Mat start = frame.clone();
    // process frame
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b old_pixel = stored_frame.at<cv::Vec3b>(z, i);
            pixel[0] += (old_pixel[0]^pixel[0])*pos;
            pixel[1] += (old_pixel[1]&pixel[1])*pos;
            pixel[2] += (old_pixel[2]|pixel[2])*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    stored_frame = start;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::FrameBlendRGB(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    static double pos = 1.0, pos_max = 7.0;
    static cv::Mat stored_frame;
    if((frame.rows != stored_frame.rows) || (frame.cols != stored_frame.cols)) {
        stored_frame = frame.clone();
    }
    cv::Mat start = frame.clone();
    static int swap = 0;
    // process frame
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b old_pixel = stored_frame.at<cv::Vec3b>(z, i);
            switch(swap) {
                case 0:
                    pixel[0] += (old_pixel[0]^pixel[0])*pos;
                    pixel[1] += (old_pixel[1]&pixel[1])*pos;
                    pixel[2] += (old_pixel[2]|pixel[2])*pos;
                    break;
                case 1:
                    pixel[0] += (old_pixel[0]&pixel[0])*pos;
                    pixel[1] += (old_pixel[1]|pixel[1])*pos;
                    pixel[2] += (old_pixel[2]^pixel[2])*pos;
                    break;
                case 2:
                    pixel[0] += (old_pixel[0]|pixel[0])*pos;
                    pixel[1] += (old_pixel[1]^pixel[1])*pos;
                    pixel[2] += (old_pixel[2]&pixel[2])*pos;
                    break;
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    ++swap;
    if(swap > 2) swap = 0;
    stored_frame = start;
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

template<int Size>
class MatrixCollection {
public:
    MatrixCollection() : w(0), h(0) {}
    cv::Mat frames[Size+1];
    int w, h;
    void shiftFrames(cv::Mat &frame) {
        int wx = frame.cols;
        int wh = frame.rows;
        if(w != wx || h != wh) {
            for(unsigned int i = 0; i < Size; ++i)
                frames[i] = frame.clone();
            w = wx;
            h = wh;
            return;
        }
        
        for(int i = Size-1; i > 0; --i) {
            frames[i] = frames[i-1];
        }
        frames[0] = frame.clone();
    }
};

void ac::TrailsFilterIntense(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            pixel[0] += (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0]);
            pixel[1] += (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1]);
            pixel[2] += (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
}

void ac::TrailsFilter(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[4];
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

void ac::TrailsFilterSelfAlpha(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double pos = 1.0, pos_max = 7.0;
    collection.shiftFrames(frame);
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            pixel[0] += (frame_pixels[0][0] + frame_pixels[1][0] + frame_pixels[2][0] + frame_pixels[3][0] + frame_pixels[4][0] + frame_pixels[5][0])*pos;
            pixel[1] += (frame_pixels[0][1] + frame_pixels[1][1] + frame_pixels[2][1] + frame_pixels[3][1] + frame_pixels[4][1] + frame_pixels[5][1])*pos;
            pixel[2] += (frame_pixels[0][2] + frame_pixels[1][2] + frame_pixels[2][2] + frame_pixels[3][2] + frame_pixels[4][2] + frame_pixels[5][2])*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::TrailsFilterXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double pos = 1.0, pos_max = 7.0;
    collection.shiftFrames(frame);
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar s;
            cv::Vec3b frame_pixels[8];
            frame_pixels[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            frame_pixels[1] = collection.frames[2].at<cv::Vec3b>(z, i);
            frame_pixels[2] = collection.frames[3].at<cv::Vec3b>(z, i);
            frame_pixels[3] = collection.frames[4].at<cv::Vec3b>(z, i);
            frame_pixels[4] = collection.frames[5].at<cv::Vec3b>(z, i);
            frame_pixels[5] = collection.frames[6].at<cv::Vec3b>(z, i);
            for(unsigned int q = 0; q < 6; ++q) {
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int movement = 0;
    cv::Mat frame_copy = frame.clone();
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(i+movement < (w-1)) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i+movement));
                pixel[2] += (add[2]*pos);
            } else if((i-movement) > 1) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i-movement));
                pixel[2] += (add[2]*pos);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    static unsigned int rgb = 0;
    static unsigned int movement = 0;
    cv::Mat frame_copy = frame.clone();
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(i+movement < (w-1)) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i+movement));
                pixel[rgb] += (add[rgb]*pos);
            } else if((i-movement) > 1) {
                cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, (i-movement));
                pixel[rgb] += (add[rgb]*pos);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0; // position in transition, maximum value
    static unsigned int movement[4] = {0, w, 0}; // movement variable array
    static unsigned int stored_w = w; // stored_w in case the frame size changes
    if(stored_w != w) {
        movement[1] = w-1; // set movement[1] to width
        stored_w = w; // stored_w set to new width
    }
    cv::Mat frame_copy = frame.clone(); // make a copy of the frame
    for(unsigned int z = 0; z < h; ++z) { // loop from top to bottom
        for(unsigned int i = 0; i < w; ++i) { // loop from left to right
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i); // reference to current pixel
            for(unsigned int q = 0; q <= 2; ++q) { // loop from 0 to 2
                unsigned int pos_x = i+movement[q];// pixel position
                unsigned int pos_y = i-movement[q];// pixel position
                if(pos_x < (w-1) && pos_x > 0) { // if within the screen
                    cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, pos_x); // grab pixel
                    pixel[q] += (add[q]*pos); // add to current index multiplied by position
                } else if(pos_y > 0 && pos_y < (w-1)) {// if pos y within the screen
                    cv::Vec3b add = frame_copy.at<cv::Vec3b>(z, pos_y); // grab pixel
                    pixel[q] += (add[q]*pos);// add to current index multiplied by position
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 7.0;
    
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b pixels[2][2];
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            bool grabbed = true;
            for(unsigned int a = 0; a < 2; a++) {
                for(unsigned int b = 0; b < 2; b++) {
                    if((a+i) < (w-1) && (b+z) < (h-1)) {
                        pixels[a][b] = frame.at<cv::Vec3b>(z+b, i+a);
                    } else {
                        grabbed = false;
                        break;
                    }
                }
            }
            if(grabbed == false) continue;
            unsigned char rgb[3] = {0};
            for(unsigned int q = 0; q < 3; ++q)
                for(unsigned int a = 0; a < 2; ++a) {
                    for(unsigned int b = 0; b < 2; ++b) {
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
	static unsigned int square = 2;
    for(unsigned int z = 0; z < h; z += square) {
        for(unsigned int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int x = 0; x < square; ++x) {
                for(unsigned int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                    	cv::Vec3b &pix = frame.at<cv::Vec3b>(y+z, i+x);
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
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 3.0;
    static unsigned int square = 2;
    for(unsigned int z = 0; z < h; z += square) {
        for(unsigned int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int x = 0; x < square; ++x) {
                for(unsigned int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = frame.at<cv::Vec3b>(y+z, i+x);
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

void ac::BlockScale(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static double pos = 1.0, pos_max = 3.0;
    static unsigned int square = 2;
    for(unsigned int z = 0; z < h; z += square) {
        for(unsigned int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int x = 0; x < square; ++x) {
                for(unsigned int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = frame.at<cv::Vec3b>(y+z, i+x);
                        pix[0] = static_cast<unsigned char>(pixel[0]*pos);
                        pix[1] = static_cast<unsigned char>(pixel[1]*pos);
                        pix[2] = static_cast<unsigned char>(pixel[2]*pos);
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

void ac::BlockStrobe(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame heigh
    static unsigned int square = 2;
    for(unsigned int z = 0; z < h; z += square) {
        for(unsigned int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(unsigned int x = 0; x < square; ++x) {
                for(unsigned int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = frame.at<cv::Vec3b>(y+z, i+x);
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

void ac::PrevFrameBlend(cv::Mat &frame) {
    unsigned int w = frame.cols;// frame width
    unsigned int h = frame.rows;// frame height
    double pos = 1.0;
    static unsigned int old_w = w;
    static cv::Mat stored = frame.clone(), temp;
    temp = frame.clone();
    if(old_w != w) {
        stored = frame.clone();
        old_w = w;
    }
    for(unsigned int z = 0; z < h; ++z) {
        for(unsigned int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b old_pixel = stored.at<cv::Vec3b>(z, i);
            pixel[0] = (pixel[0])+(1-old_pixel[0])*pos;
            pixel[1] = (pixel[1])+(1-old_pixel[1])*pos;
            pixel[2] = (pixel[2])+(1-old_pixel[2])*pos;
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
}

// No Filter
void ac::NoFilter(cv::Mat &frame) {}

// Alpha Blend with Original Frame
void ac::BlendWithSource(cv::Mat &frame) {
    ac::pass2_alpha = 0.50; // set to 50%
    Pass2Blend(frame);// call Pass2 function
}
// call custom fitler defined elsewhere
void ac::custom(cv::Mat &frame) {
    custom_filter(frame);
}
