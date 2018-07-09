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

// Acid Cam namespace
namespace ac {
    const std::string version="2.4.6";
    // variables
    int swapColor_r = 0, swapColor_g = 0, swapColor_b = 0;
    bool isNegative = false, noRecord = false, pass2_enabled = false, blendW = false, slide_Show = false, slide_Rand = false, strobe_It = false, switch_Back = false, blur_First = false;
    bool images_Enabled = false, fps_force = false,iRev = false;
    bool blur_Second = false;
    int set_color_map = 0;
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
    cv::Size resolution(0, 0);
    std::string fileName ="VideoFile.avi";
    std::string draw_strings_value[] = { "Self AlphaBlend", "Self Scale", "StrobeEffect", "Blend #3", "Negative Paradox","ThoughtMode", "RandTriBlend", "Blank", "Tri", "Distort", "CDraw", "Type", "NewOne", "Blend Fractal","Blend Fractal Mood", "CosSinMultiply", "Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8","Filter3","Rainbow Blend","Rand Blend","New Blend", "Alpha Flame Filters", "Pixel Scale", "PixelSort", "GlitchSort","Random Filter", "Random Flash", "Blend with Image", "Blend with Image #2", "Blend with Image #3", "Blend with Image #4", "GaussianBlur", "Median Blur", "Blur Distortion", "Diamond Pattern","MirrorBlend","Pulse","Sideways Mirror","Mirror No Blend","Sort Fuzz","Fuzz","Double Vision","RGB Shift","RGB Sep","Graident Rainbow","Gradient Rainbow Flash", "Reverse", "Scanlines", "TV Static", "Mirror Average", "Mirror Average Mix", "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND","Bitwise_OR","Equalize", "Channel Sort", "Reverse_XOR", "Combine Pixels", "FlipTrip", "Canny","Boxes","Boxes Fade", "Flash Black","SlideRGB","Side2Side","Top2Bottom","Strobe Red Then Green Then Blue","Blend_Angle", "Outward", "Outward Square","ShiftPixels", "ShiftPixelsDown", "XorMultiBlend", "Bitwise_Rotate", "Bitwise_Rotate Diff","HPPD","FuzzyLines","GradientLines","GradientSelf","GradientSelfVertical","GradientDown","GraidentHorizontal","GradientRGB","Inter","UpDown","LeftRight","StrobeScan","BlendedScanLines","GradientStripes","XorSine","SquareSwap","SquareSwap4x2","SquareSwap8x4","SquareSwap16x8","SquareSwap64x32","SquareBars","SquareBars8","SquareSwapRand16x8","SquareVertical8","SquareVertical16","SquareVertical_Roll","SquareSwapSort_Roll","SquareVertical_RollReverse","SquareSwapSort_RollReverse","Circular","WhitePixel","FrameBlend","FrameBlendRGB","TrailsFilter","TrailsFilterIntense","TrailsFilterSelfAlpha","TrailsFilterXor","ColorTrails","MoveRed","MoveRGB","MoveRedGreenBlue","BlurSim","Block","BlockXor","BlockScale","BlockStrobe","PrevFrameBlend","Wave","HighWave","VerticalSort","VerticalChannelSort","HorizontalBlend","VerticalBlend","OppositeBlend","DiagonalLines","HorizontalLines","InvertedScanlines","Soft_Mirror","KanapaTrip","ColorMorphing","ScanSwitch","ScanAlphaSwitch","NegativeStrobe", "XorAddMul","ParticleRelease", "BlendSwitch", "All Red", "All Green", "All Blue","LineRGB","PixelRGB","BoxedRGB","KruegerSweater","RGBFlash","IncreaseBlendHorizontal","BlendIncrease","GradientReverse","GradientReverseVertical","GradientReverseBox","GradientNewFilter","ReinterpretDouble","ReinterpSelfScale","AverageLines","ImageFile","ImageXor","ImageAlphaBlend","ColorRange","ImageInter","TrailsInter","TrailsBlend","TrailsNegate","InterReverse","InterMirror","InterFullMirror","MirrorRGB","RGBStatic1","RGBStatic2","VectorIncrease","LineByLineReverse","RandomIntertwine","RandomFour","BlendThree","AcidTrails","RandomTwo","HorizontalTrailsInter","Trails","BlendTrails","Negate","RandomFilteredSquare","ImageX","RandomQuads","QuadCosSinMultiply","QuadRandomFilter","RollRandom","AverageRandom","HorizontalStripes","DiamondStrobe","SmoothTrails","GridFilter8x","GridFilter16x","GridFilter8xBlend","GridRandom","GridRandomPixel","Dual_SelfAlphaRainbow","Dual_SelfAlphaBlur","SurroundPixelXor","Darken","WeakBlend","AverageVertical","RandomCollectionAverage","RandomCollectionAverageMax","SmoothTrailsSelfAlphaBlend","SmoothTrailsRainbowBlend","MedianBlend","SmoothRandomImageBlend","SmoothImageAlphaBlend","RandomAlphaBlend","RandomTwoFilterAlphaBlend","PixelatedSquare","AlphaBlendPosition","BlendRowAlpha", "BlendRow","BlendRowByVar","BlendRowByDirection","BlendAlphaXor","SelfXorScale","BitwiseXorScale","XorTrails", "RainbowTrails","NegativeTrails","IntenseTrails","SelfAlphaRGB","BlendImageOnOff","XorSelfAlphaImage","BitwiseXorStrobe","AlphaBlendRandom","ChannelSortAlphaBlend","XorChannelSort","GradientColors","GradientColorsVertical","Bitwise_XOR_Average","NotEqual","ImageShiftUpLeft","GradientXorSelfScale","SmoothSourcePixel","StrobeBlend","FrameBars","Sort_Vertical_Horizontal","Sort_Vertical_Horizontal_Bitwise_XOR","Scalar_Average_Multiply","Scalar_Average","Total_Average","AlphaBlendImageXor","FlashWhite","FlashBlackAndWhite","GaussianBlend","RandomXor","RandomXorFlash","RandomAmountMedianBlur","SoftXor","SelfXorBlend","SelfXorDoubleFlash","SelfOrDoubleFlash","BlendRowCurvedSqrt","CycleShiftRGB","CycleShiftRandomRGB","CycleShiftRandomRGB_XorBlend","CycleShiftRandomAlphaBlend","VerticalColorBars","GradientLeftRight","GraidentUpDown","GradientLeftRightInOut","GradientUpDownInOut","Lines","ColorLines","WhiteLines","ThickWhiteLines","UseLineObject","TanAlphaGrid","MedianBlendAnimation","FibFlash", "ScaleFlash","LeftLines","Curtain","RandomCurtain","CurtainVertical","RandomCurtainVertical","inOrder","inOrderBySecond","DarkenFilter","RandomFilterBySecond","ThreeRandom","inOrderAlpha","inOrderAlphaXor","SlideFilter","SlideFilterXor", "RandomSlideFilter","SlideUpDown","SlideUpDownXor", "SlideUpDownRandom","SlideSubFilter", "SlideSubUpDownFilter","ParticleBlend","ParticleFlash","ExactImage","ParticleAlpha","BlendInAndOut","BlendScaleInAndOut","AcidGlitch","XorBackwards","LiquidFilter","MatrixXorAnd","XorAlpha","AlphaAcidTrails","SelfXorAverage","RandomXorBlend", "No Filter","Blend with Source", "Plugin", "Custom","Blend With Image #1","TriBlend with Image", "Image Strobe", "Image distraction" };
    // draw strings
    std::string *draw_strings = draw_strings_value;
    // filter callback functions
    DrawFunction draw_func_value[] = { SelfAlphaBlend, SelfScale, StrobeEffect, Blend3, NegParadox, ThoughtMode, RandTriBlend, Blank,Tri,Distort,CDraw,Type,NewOne,blendFractal,blendFractalMood,cossinMultiply,colorAccumulate1,colorAccumulate2,colorAccumulate3,filter8,filter3,rainbowBlend,randBlend,newBlend,alphaFlame,pixelScale,pixelSort,glitchSort,randomFilter,randomFlash,imageBlend,imageBlendTwo,imageBlendThree,imageBlendFour,GaussianBlur,MedianBlur,BlurDistortion,DiamondPattern,MirrorBlend,Pulse,SidewaysMirror,MirrorNoBlend,SortFuzz,Fuzz,DoubleVision,RGBShift,RGBSep,GradientRainbow,GradientRainbowFlash,Reverse,Scanlines,TVStatic,MirrorAverage,MirrorAverageMix,Mean,Laplacian,Bitwise_XOR,Bitwise_AND,Bitwise_OR,Equalize,ChannelSort,Reverse_XOR,CombinePixels,FlipTrip,Canny,Boxes,BoxesFade,FlashBlack,SlideRGB,Side2Side,Top2Bottom,StrobeRedGreenBlue,Blend_Angle,Outward,OutwardSquare,ShiftPixels,ShiftPixelsDown,XorMultiBlend,BitwiseRotate,BitwiseRotateDiff,HPPD,FuzzyLines,GradientLines,GradientSelf,GradientSelfVertical,GradientDown,GraidentHorizontal,GradientRGB,Inter,UpDown,LeftRight,StrobeScan,BlendedScanLines,GradientStripes,XorSine,SquareSwap,SquareSwap4x2,SquareSwap8x4,SquareSwap16x8,SquareSwap64x32,SquareBars,SquareBars8,SquareSwapRand16x8,SquareVertical8,SquareVertical16,SquareVertical_Roll,SquareSwapSort_Roll,SquareVertical_RollReverse,SquareSwapSort_RollReverse,Circular,WhitePixel,FrameBlend,FrameBlendRGB,TrailsFilter,TrailsFilterIntense,TrailsFilterSelfAlpha,TrailsFilterXor,ColorTrails,MoveRed,MoveRGB,MoveRedGreenBlue,BlurSim,Block,BlockXor,BlockScale,BlockStrobe,PrevFrameBlend,Wave,HighWave,VerticalSort,VerticalChannelSort,HorizontalBlend,VerticalBlend,OppositeBlend,DiagonalLines,HorizontalLines,InvertedScanlines,Soft_Mirror,KanapaTrip,ColorMorphing,ScanSwitch,ScanAlphaSwitch,NegativeStrobe,XorAddMul,ParticleRelease,BlendSwitch,AllRed,AllGreen,AllBlue,LineRGB,PixelRGB,BoxedRGB,KruegerSweater,RGBFlash,IncreaseBlendHorizontal,BlendIncrease,GradientReverse,GradientReverseVertical,GradientReverseBox,GradientNewFilter,ReinterpretDouble,ReinterpSelfScale,AverageLines,ImageFile,ImageXor,ImageAlphaBlend,ColorRange,ImageInter,TrailsInter,TrailsBlend,TrailsNegate,InterReverse,InterMirror,InterFullMirror,MirrorRGB,RGBStatic1,RGBStatic2,VectorIncrease,LineByLineReverse,RandomIntertwine,RandomFour,BlendThree,AcidTrails,RandomTwo,HorizontalTrailsInter,Trails,BlendTrails,Negate,RandomFilteredSquare,ImageX,RandomQuads,QuadCosSinMultiply,QuadRandomFilter,RollRandom,AverageRandom,HorizontalStripes,DiamondStrobe,SmoothTrails,GridFilter8x,GridFilter16x,GridFilter8xBlend,GridRandom,GridRandomPixel,Dual_SelfAlphaRainbow,Dual_SelfAlphaBlur,SurroundPixelXor,Darken,WeakBlend,AverageVertical,RandomCollectionAverage,RandomCollectionAverageMax,SmoothTrailsSelfAlphaBlend,SmoothTrailsRainbowBlend,MedianBlend,SmoothRandomImageBlend,SmoothImageAlphaBlend,RandomAlphaBlend,RandomTwoFilterAlphaBlend,PixelatedSquare,AlphaBlendPosition,BlendRowAlpha,BlendRow,BlendRowByVar,BlendRowByDirection,BlendAlphaXor,SelfXorScale,BitwiseXorScale,XorTrails,RainbowTrails,NegativeTrails,IntenseTrails,SelfAlphaRGB,BlendImageOnOff,XorSelfAlphaImage,BitwiseXorStrobe,AlphaBlendRandom,ChannelSortAlphaBlend,XorChannelSort,GradientColors,GradientColorsVertical,Bitwise_XOR_Average,NotEqual,ImageShiftUpLeft,GradientXorSelfScale,SmoothSourcePixel,StrobeBlend,FrameBars,Sort_Vertical_Horizontal,Sort_Vertical_Horizontal_Bitwise_XOR,Scalar_Average_Multiply,Scalar_Average,Total_Average,AlphaBlendImageXor,FlashWhite,FlashBlackAndWhite,GaussianBlend,RandomXor,RandomXorFlash,RandomAmountMedianBlur,SoftXor,SelfXorBlend,SelfXorDoubleFlash,SelfOrDoubleFlash,BlendRowCurvedSqrt,CycleShiftRGB,CycleShiftRandomRGB,CycleShiftRandomRGB_XorBlend,CycleShiftRandomAlphaBlend,VerticalColorBars,GradientLeftRight,GraidentUpDown,GradientLeftRightInOut,GradientUpDownInOut,Lines,ColorLines,WhiteLines,ThickWhiteLines,UseLineObject,TanAlphaGrid,MedianBlendAnimation,FibFlash,ScaleFlash,LeftLines,Curtain,RandomCurtain,CurtainVertical,RandomCurtainVertical,inOrder,inOrderBySecond,DarkenFilter,RandomFilterBySecond,ThreeRandom,inOrderAlpha,inOrderAlphaXor,SlideFilter,SlideFilterXor,RandomSlideFilter,SlideUpDown,SlideUpDownXor,SlideUpDownRandom,SlideSubFilter,SlideSubUpDownFilter,ParticleBlend,ParticleFlash,ExactImage,ParticleAlpha,BlendInAndOut,BlendScaleInAndOut,AcidGlitch,XorBackwards,LiquidFilter,MatrixXorAnd,XorAlpha,AlphaAcidTrails,SelfXorAverage,RandomXorBlend,NoFilter,BlendWithSource,plugin,custom,blendWithImage, triBlendWithImage,imageStrobe, imageDistraction,0};
    // draw functions
    DrawFunction *draw_func = draw_func_value;
    // number of filters
    int draw_max = 323;
    // variables
    double translation_variable = 0.001f, pass2_alpha = 0.75f;
    // swap colors inline function
    std::unordered_map<std::string, int> filter_map;
    bool color_map_set = false;
    DrawFunction custom_callback = 0;
    DrawFunction plugin_func = 0;
    int colors[3] = {rand()%255, rand()%255, rand()%255};
    int proc_mode = 0;
    bool reset_filter = false;
    double alpha_increase = 0;
}

cv::Mat blend_image, color_image;
bool blend_set = false;
bool colorkey_set = false;


std::string ac::getVersion() {
    return version;
}

void ac::fill_filter_map() {
    for(int i = 0; i < ac::draw_max; ++i) {
        filter_map[draw_strings[i]] = i;
    }
}

void ac::DrawFilter(const std::string &name, const cv::Mat &frame, cv::Mat &outframe) {
    outframe = frame.clone();
    ac::draw_func[filter_map[name]](outframe);
}

void ac::DrawFilter(int index, const cv::Mat &frame, cv::Mat &outframe) {
    outframe = frame.clone();
    ac::draw_func[index](outframe);
}
void ac::DrawFilter(int index, cv::Mat &frame) {
    ac::draw_func[index](frame);
    
}
void ac::DrawFilter(const std::string &name, cv::Mat &frame) {
    ac::draw_func[filter_map[name]](frame);
}


void ac::swapColors(cv::Mat &frame, int y, int x) {
    if(in_custom == true) return;
    if(color_order == 0 && swapColor_r == 0 && swapColor_g == 0 && swapColor_b == 0) return; // if no swap needed return
    if(set_color_map > 0 && color_map_set == false) {
        return;
    }
    swapColors_(frame, y, x);
}

void ac::swapColors_(cv::Mat &frame, int y, int x) {
    cv::Vec3b &cur = frame.at<cv::Vec3b>(y,x);
    cur[0] += swapColor_b;
    cur[1] += swapColor_g;
    cur[2] += swapColor_r;
    cv::Vec3b temp;// temp
    temp = cur;// temp = cur
    // Default color order is BGR
    // swap RGB orders
    switch(color_order) {
        case 1: // RGB
            cur[0] = temp[2];
            cur[1] = temp[1];
            cur[2] = temp[0];
            break;
        case 2:// GBR
            cur[0] = temp[1];
            cur[1] = temp[0];
            break;
        case 3:// BRG
            cur[1] = temp[2];
            cur[2] = temp[1];
            break;
        case 4: // GRB
            cur[0] = temp[1];
            cur[1] = temp[2];
            cur[2] = temp[0];
            break;
    }
}
// invert pixel in frame at x,y
void ac::invert(cv::Mat &frame, int y, int x) {
    if(in_custom == true) return;
    cv::Vec3b &cur = frame.at<cv::Vec3b>(y,x);// cur pixel
    cur[0] = ~cur[0]; // bit manipulation sets opposite
    cur[1] = ~cur[1];
    cur[2] = ~cur[2];
}

// proc position
void ac::procPos(int &direction, double &pos, double &pos_max, const double max_size, double iter) {
    if(alpha_increase != 0) iter = alpha_increase;
    switch(proc_mode) {
        case 0: { // move in - increase move out movin - increase move out
            // static int direction
            // pos max
            // if direction equals 1
            if(direction == 1) {
                pos += iter; // pos plus equal 0.05
                if(pos > pos_max) { // if pos > pos max
                    pos = pos_max; // pos = pos_max
                    direction = 0;// direction equals 0
                    pos_max += 0.5; // pos_max increases by 0.5
                }
            } else if(direction == 0) {// direction equals 0
                pos -= iter;// pos -= 0.05
                if(pos <= 1.0) {// if pos <= 1.0
                    if(pos_max > max_size) pos_max = 1.0;// if pos max at maxmium
                    // set to 1.0
                    direction = 1;// set direction back to 1
                }
            }
        }
            break;
        case 1: { // flat fade in fade out
            if(direction == 1) {
                pos += iter;
                if(pos > max_size) direction = 0;
                
            } else if(direction == 0) {
                pos -= iter;
                if(pos <= 1) direction = 1;
            }
            
        }
            break;
        case 2: {
            pos += iter;
            if(pos >= pos_max) {
                pos = 1.0;
            }
        }
            break;
    }
    resetAlpha(direction, pos);
}


void ac::setProcMode(int value) {
    proc_mode = value;
}
