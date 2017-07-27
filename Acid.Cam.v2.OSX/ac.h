/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 * http://lostsidedead.com
 * (C) 2017 - GPL
 */
#ifndef __AC_H__
#define __AC_H__
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<algorithm>
#include<unordered_map>

/*
 * to use set appropriate variables, call the function
 *
 * ac::SelfAlphaBlend(mat);
 *
 * or use draw_func
 *
 * ac::draw_func[function_index](mat);
 *
 * then just use function_index to point to whatever filter
 *
 * for text of the filter use the following array of strings
 *
 * ac::draw_strings[function_index];
 *
 */

// exernal variables
extern int current_filterx, bytesPerSample, bytesPerRow, width, height, red, green, blue, offset, randomNumber, reverse;
extern bool negate, blend_set;
extern cv::Mat blend_image;

// acid cam namespace
namespace ac {
    // version string
    static const std::string version="2.2.4";
    extern double translation_variable, pass2_alpha;
    extern double alpha, tr;
    extern bool isNegative, noRecord,iRev;
    extern int color_order;
    extern double fps;
    extern int draw_offset;
    extern std::string fileName;
    extern cv::Mat orig_frame;
    extern cv::Mat blendW_frame;
    extern cv::Mat image_files[4];
    extern bool images_Enabled,fps_force;
    extern int snapshot_Type;
    extern bool in_custom;
    extern unsigned int swapColor_r, swapColor_g, swapColor_b;
    inline int GetFX(cv::Mat &frame, int x, int nw);
    inline int GetFY(cv::Mat &frame, int y, int nh);
    inline void invert(cv::Mat &frame, int x, int y);
    /* filter functions */
    typedef void (*DrawFunction)(cv::Mat &frame);
    
    template<typename T>
    inline T ror(T x, unsigned int m)
    {
        return (x >> m) | (x << (sizeof(T)*8 - m));
    }
    
    template<typename T>
    inline T rol(T x, unsigned int m) {
        return (x << m) | (x >> (sizeof(T)*8 -m));
    }
    
    // Acid Cam Filter Function prototypes
    void SelfAlphaBlend(cv::Mat &frame);
    void SelfScale(cv::Mat &frame);
    void StrobeEffect(cv::Mat &frame);
    void Blend3(cv::Mat &frame);
    void NegParadox(cv::Mat &frame);
    void ThoughtMode(cv::Mat &frame);
    void Pass2Blend(cv::Mat &frame);
    void RandTriBlend(cv::Mat &frame);
    void Blank(cv::Mat &frame);
    void Tri(cv::Mat &frame);
    void Distort(cv::Mat &frame);
    void CDraw(cv::Mat &frame);
    void Type(cv::Mat &frame);
    void NewOne(cv::Mat &frame);
    void blendWithImage(cv::Mat &frame);
    void triBlendWithImage(cv::Mat &frame);
    void imageStrobe(cv::Mat &frame);
    void imageDistraction(cv::Mat &frame);
    void blendFractal(cv::Mat &frame);
    void blendFractalMood(cv::Mat &frame);
    void cossinMultiply(cv::Mat &frame);
    void colorAccumulate1(cv::Mat &frame);
    void colorAccumulate2(cv::Mat &frame);
    void colorAccumulate3(cv::Mat &frame);
    void filter8(cv::Mat &frame);
    void filter3(cv::Mat &frame);
    void rainbowBlend(cv::Mat &frame);
    void randBlend(cv::Mat &frame);
    void newBlend(cv::Mat &frame);
    void alphaFlame(cv::Mat &frame);
    void custom(cv::Mat &frame);
    void pixelScale(cv::Mat &frame);
    void glitchSort(cv::Mat &frame);
    void pixelSort(cv::Mat &frame);
    void randomFilter(cv::Mat &frame);
    void randomFlash(cv::Mat &frame);
    void imageBlend(cv::Mat &frame);
    void imageBlendTwo(cv::Mat &frame);
    void imageBlendThree(cv::Mat &frame);
    void imageBlendFour(cv::Mat &frame);
    void GaussianBlur(cv::Mat &frame);
    void MedianBlur(cv::Mat &frame);
    void BlurDistortion(cv::Mat &frame);
    void DiamondPattern(cv::Mat &frame);
    void MirrorBlend(cv::Mat &frame);
    void Pulse(cv::Mat &frame);
    void SidewaysMirror(cv:: Mat &frame);
    void MirrorNoBlend(cv::Mat &frame);
    void SortFuzz(cv::Mat &frame);
    void Fuzz(cv::Mat &frame);
    void DoubleVision(cv::Mat &frame);
    void RGBShift(cv::Mat &frame);
    void RGBSep(cv::Mat &frame);
    void GradientRainbow(cv::Mat &frame);
    void GradientRainbowFlash(cv::Mat &frame);
    void Reverse(cv::Mat &frame);
    void Scanlines(cv::Mat &frame);
    void TVStatic(cv::Mat &frame);
    void MirrorAverage(cv::Mat &frame);
    void MirrorAverageMix(cv::Mat &frame);
    void Mean(cv::Mat &frame);
    void Laplacian(cv::Mat &frame);
    void Bitwise_XOR(cv::Mat &frame);
    void Bitwise_AND(cv::Mat &frame);
    void Bitwise_OR(cv::Mat &frame);
    void Equalize(cv::Mat &frame);
    void ChannelSort(cv::Mat &frame);
    void Reverse_XOR(cv::Mat &frame);
    void CombinePixels(cv::Mat &frame);
    void FlipTrip(cv::Mat &frame);
    void Canny(cv::Mat &frame);
    void Boxes(cv::Mat &frame);
    void BoxesFade(cv::Mat &frame);
    void FlashBlack(cv::Mat &frame);
    void SlideRGB(cv::Mat &frame);
    void Side2Side(cv::Mat &frame);
    void Top2Bottom(cv::Mat &frame);
    void StrobeRedGreenBlue(cv::Mat &frame);
    void Outward(cv::Mat &frame);
    void OutwardSquare(cv::Mat &frame);
    void Blend_Angle(cv::Mat &frame);
    void ShiftPixels(cv::Mat &frame);
    void ShiftPixelsDown(cv::Mat &frame);
    void XorMultiBlend(cv::Mat &frame);
    void BitwiseRotate(cv::Mat &frame);
    void BitwiseRotateDiff(cv::Mat &frame);
    void HPPD(cv::Mat &frame);
    void FuzzyLines(cv::Mat &frame);
    void GradientLines(cv::Mat &frame);
    void GradientSelf(cv::Mat &frame);
    void GradientDown(cv::Mat &frame);
    void GraidentHorizontal(cv::Mat &frame);
    void GradientSelfVertical(cv::Mat &frame);
    void GradientRGB(cv::Mat &frame);
    void Inter(cv::Mat &frame);
    void UpDown(cv::Mat &frame);
    void LeftRight(cv::Mat &frame);
    void StrobeScan(cv::Mat &frame);
    void BlendedScanLines(cv::Mat &frame);
    void GradientStripes(cv::Mat &frame);
    void XorSine(cv::Mat &frame);
    void SquareSwap(cv::Mat &frame);
    void SquareSwap4x2(cv::Mat &frame);
    void SquareSwap8x4(cv::Mat &frame);
    void SquareSwap16x8(cv::Mat &frame);
    void SquareSwap64x32(cv::Mat &frame);
    void SquareBars(cv::Mat &frame);
    void SquareBars8(cv::Mat &frame);
    void SquareVertical8(cv::Mat &frame);
    void SquareVertical16(cv::Mat &frame);
    void SquareVertical_Roll(cv::Mat &frame);
    void SquareSwapRand16x8(cv::Mat &frame);
    void SquareSwapSort_Roll(cv::Mat &frame);
    void BlendWithSource(cv::Mat &frame);
    void plugin(cv::Mat &frame);
    // draw functions / strings
    extern std::string draw_strings[];
    extern DrawFunction draw_func[];
    extern int draw_max;
    extern bool snapShot;
    extern std::unordered_map<std::string, int> filter_map;
    void fill_filter_map();
}
// custom filter function, must be defined in program so it will link
extern void custom_filter(cv::Mat &frame);

#endif
