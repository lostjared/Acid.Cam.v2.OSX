/*
 * Acid Cam Filters for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
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
#ifndef __AC_H__
#define __AC_H__
#ifdef __APPLE__
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/core/ocl.hpp>
#else
#include<opencv2/opencv.hpp>
#include<opencv2/core/ocl.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#endif
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<algorithm>
#include<random>
#include<unordered_map>
#include<memory>
#include<cassert>
#include<utility>
#include<thread>
#include<mutex>
#include<chrono>
#include<deque>
#include<atomic>
#include"ac-filtercat.h"
//#define ASSERT_CHECK
//Macro for assert testing
#ifdef ASSERT_CHECK
#define ASSERT(X) assert(X)
#else
#define ASSERT(X)
#endif
// leave defined for safe array access (slower for testing)
// comment out below if building release build
//#ifndef DONT_TEST_BOUNDS
//#define ARRAY_BOUNDS_TEST
//#endif
// comment out below if making release
//#define DEBUG_MODE
/*
 *
 * Be sure to call fill_filter_map or init
 * to use set appropriate variables, call the function
 *
 * ac::SelfAlphaBlend(mat);
 *
 * or use CallFilter
 *
 * CallFilter(function_index, mat);
 *
 * then just use function_index to point to whatever filter
 *
 * for text of the filter use the following array of strings
 *
 * ac::draw_strings[function_index];
 *
 * When writing C++ do not call another Mulithreaded filter
 * from within a mulithreaded filter within its loop callback
 *
 */
// exernal variables
extern std::atomic<int> current_filterx, bytesPerSample, bytesPerRow, width, height, red, green, blue, offset, randomNumber, reverse;
extern std::atomic<bool> negate, blend_set, colorkey_set,colorkey_bg, colorkey_replace, colorkey_filter;
extern cv::Mat blend_image,color_image,color_bg_image, color_replace_image, color_filter;
int AC_GetFX(int oldw,int x, int nw);
int AC_GetFZ(int oldh, int y, int nh);

// acid cam namespace
namespace ac {
    
    inline cv::Vec3b &pixelAt(cv::Mat &frame, int y, int x) {
#ifdef ARRAY_BOUNDS_TEST
        if(y >= 0 && x >= 0 && y < frame.rows && x < frame.cols)
             return frame.at<cv::Vec3b>(y, x);
        
        std::cout << "Error: Array Position Out Of Range: " << x << "," << y << "\n";
        return frame.at<cv::Vec3b>(0, 0);
#endif
        return frame.at<cv::Vec3b>(y, x);
    }
    
    class FileT {
    public:
        std::string fname;
        std::vector<std::string> name;
        std::vector<std::string> subname;
        std::vector<int> filter_on;
        std::unordered_map<std::string, std::string> options;
    };
    
    typedef void (*pixel)(cv::Mat &frame);
    
    class UserFilter {
    public:
        int index, subfilter;
        int sort_num;
        pixel func;
        std::string name, other_name, filename, plug_file;
        FileT custom_filter;
        void *library;
        UserFilter(): index(0), subfilter(-1), func(0), library(0) {}
    };
    
    bool checkFilter(std::string name);
    extern unsigned char wrap_cast(double d);
    // get version string
    extern std::string getVersion();
    extern int getFilterCount();
    extern int getFilterByName(const std::string &n);
    extern std::vector<std::string> buildDrawStrings();
    
    // mutex for threads
    extern std::mutex col_lock;
    // video capture for key
    extern cv::VideoCapture v_cap;
    // version string
    extern const std::string version;
    extern double translation_variable, pass2_alpha;
    extern std::atomic<double> alpha, tr;
    extern std::atomic<bool> isNegative, noRecord,iRev;
    extern std::atomic<int> color_order;
    extern std::atomic<double> fps;
    extern std::atomic<int> draw_offset;
    extern std::atomic<int> subfilter;
    extern std::string fileName;
    extern cv::Mat orig_frame;
    extern cv::Mat blendW_frame;
    extern cv::Mat image_files[4];
    extern std::atomic<bool> images_Enabled,fps_force;
    extern std::atomic<int> snapshot_Type;
    extern std::atomic<bool> in_custom;
    extern std::atomic<int> swapColor_r, swapColor_g, swapColor_b;
    extern cv::Size resolution;
    extern std::atomic<bool> strobe_It;
    extern std::atomic<int> set_color_map;
    extern std::atomic<bool> color_map_set;
    extern int GetFX(cv::Mat &frame, int x, int nw);
    extern int GetFY(cv::Mat &frame, int y, int nh);
    extern bool OpenCL_Enabled();
    extern std::atomic<bool> reset_alpha;
    extern std::unordered_map<std::string, UserFilter> user_filter;
    void invert(cv::Mat &frame, int x, int y);
     /* filter typedef */
    typedef void (*DrawFunction)(cv::Mat &frame);
    typedef std::pair<std::string,DrawFunction> FilterType;
    extern DrawFunction custom_callback;
    extern DrawFunction plugin_func;
    extern std::atomic<bool> release_frames;
    extern std::atomic<bool> syphon_in_enabled;
    extern std::atomic<bool> syphon_in_changed;
    // ror/rol tempaltes
    template<typename T>
    inline T ror(T x, int m){
        return (x >> m) | (x << (sizeof(T)*8 - m));
    }
    template<typename T>
    inline T rol(T x, int m) {
        return (x << m) | (x >> (sizeof(T)*8 -m));
    }
    enum class KeyValueType { KEY_RANGE, KEY_TOLERANCE };
    class Keys {
    public:
        cv::Vec3b low, high;
        KeyValueType key_type;
        bool spill;
        Keys() : key_type(KeyValueType::KEY_RANGE), spill(false) {}
    };
    
    extern std::deque<cv::Mat> frames_;
    
    enum SearchType { SEARCH_NOTFOUND=0, SEARCH_PIXEL, SEARCH_GRAY };
    enum PROC_MODE_TYPE { MOVEINOUT_INC, MOVEINOUT, MOVERESET };
    enum class IntertwineDir { HORIZONTAL, VERTICAL };
    extern std::vector<std::string> solo_filter;
    extern std::vector<std::string> color_filter;
    void init();
    // be sure to call this when the application starts
    void fill_filter_map();
    void setRunning(bool b);
    // resize surface preserve ratio
    cv::Mat resizeRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);
    
    // draw functions
    DrawFunction getRandomFilter();
    void DrawFilter(const std::string &name, const cv::Mat &frame, cv::Mat &outframe);
    void DrawFilter(int index, const cv::Mat &frame, cv::Mat &outframe);
    void DrawFilter(int index, cv::Mat &frame);
    void DrawFilter(const std::string &name, cv::Mat &frame);
    void DrawFilterUnordered(const std::string &name, cv::Mat &frame);
    DrawFunction getFilter(std::string name);
    FilterType filterByIndex(const int &num);
    FilterType filterByString(const std::string &num);
    bool CallFilter(int index, cv::Mat &frame);
    bool CallFilter(const std::string &name, cv::Mat &frame);
    bool CallFilterFile(cv::Mat &frame, std::string filename);
    bool LoadFilterFile(std::string name, std::string file, int &plugin);
    void clearFilterFiles();
    bool getSupportedResolutions(cv::VideoCapture &capture, std::vector<cv::Size> &res);
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
    void SquareVertical_RollReverse(cv::Mat &frame);
    void SquareSwapSort_RollReverse(cv::Mat &frame);
    void Circular(cv::Mat &frame);
    void WhitePixel(cv::Mat &frame);
    void FrameBlend(cv::Mat &frame);
    void FrameBlendRGB(cv::Mat &frame);
    void TrailsFilter(cv::Mat &frame);
    void TrailsFilterIntense(cv::Mat &frame);
    void TrailsFilterSelfAlpha(cv::Mat &frame);
    void TrailsFilterXor(cv::Mat &frame);
    void ColorTrails(cv::Mat &frame);
    void MoveRed(cv::Mat &frame);
    void MoveRGB(cv::Mat &frame);
    void MoveRedGreenBlue(cv::Mat &frame);
    void BlurSim(cv::Mat &frame);
    void Block(cv::Mat &frame);
    void BlockXor(cv::Mat &frame);
    void BlockScale(cv::Mat &frame);
    void BlockStrobe(cv::Mat &frame);
    void PrevFrameBlend(cv::Mat &frame);
    void Wave(cv::Mat &frame);
    void HighWave(cv::Mat &frame);
    void VerticalSort(cv::Mat &frame);
    void VerticalChannelSort(cv::Mat &frame);
    void HorizontalBlend(cv::Mat &frame);
    void VerticalBlend(cv::Mat &frame);
    void OppositeBlend(cv::Mat &frame);
    void DiagonalLines(cv::Mat &frame);
    void HorizontalLines(cv::Mat &frame);
    void InvertedScanlines(cv::Mat &frame);
    void Soft_Mirror(cv::Mat &frame);
    void KanapaTrip(cv::Mat &frame);
    void ColorMorphing(cv::Mat &frame);
    void ScanSwitch(cv::Mat &frame);
    void ScanAlphaSwitch(cv::Mat &frame);
    void NegativeStrobe(cv::Mat &frame);
    void XorAddMul(cv::Mat &frame);
    void ParticleRelease(cv::Mat &frame);
    void BlendSwitch(cv::Mat &frame);
    void AllRed(cv::Mat &frame);
    void AllGreen(cv::Mat &frame);
    void AllBlue(cv::Mat &frame);
    void LineRGB(cv::Mat &frame);
    void PixelRGB(cv::Mat &frame);
    void BoxedRGB(cv::Mat &frame);
    void KruegerSweater(cv::Mat &frame);
    void RGBFlash(cv::Mat &frame);
    void IncreaseBlendHorizontal(cv::Mat &frame);
    void BlendIncrease(cv::Mat &frame);
    void GradientReverse(cv::Mat &frame);
    void GradientReverseVertical(cv::Mat &frame);
    void GradientReverseBox(cv::Mat &frame);
    void GradientNewFilter(cv::Mat &frame);
    void ReinterpretDouble(cv::Mat &frame);
    void ReinterpSelfScale(cv::Mat &frame);
    void AverageLines(cv::Mat &frame);
    void ImageFile(cv::Mat &frame);
    void ImageXor(cv::Mat &frame);
    void ImageAlphaBlend(cv::Mat &frame);
    void ColorRange(cv::Mat &frame);
    void ImageInter(cv::Mat &frame);
    void TrailsInter(cv::Mat &frame);
    void TrailsBlend(cv::Mat &frame);
    void TrailsNegate(cv::Mat &frame);
    void InterReverse(cv::Mat &frame);
    void InterMirror(cv::Mat &frame);
    void InterFullMirror(cv::Mat &frame);
    void MirrorRGB(cv::Mat &frame);
    void RGBStatic1(cv::Mat &frame);
    void RGBStatic2(cv::Mat &frame);
    void VectorIncrease(cv::Mat &frame);
    void LineByLineReverse(cv::Mat &frame);
    void RandomIntertwine(cv::Mat &frame);
    void RandomFour(cv::Mat &frame);
    void BlendThree(cv::Mat &frame);
    void AcidTrails(cv::Mat &frame);
    void RandomTwo(cv::Mat &frame);
    void HorizontalTrailsInter(cv::Mat &frame);
    void Trails(cv::Mat &frame);
    void BlendTrails(cv::Mat &frame);
    void RandomFilteredSquare(cv::Mat &frame);
    void ImageX(cv::Mat &frame);
    void RandomQuads(cv::Mat &frame);
    void QuadCosSinMultiply(cv::Mat &frame);
    void QuadRandomFilter(cv::Mat &frame);
    void RollRandom(cv::Mat &frame);
    void AverageRandom(cv::Mat &frame);
    void HorizontalStripes(cv::Mat &frame);
    void DiamondStrobe(cv::Mat &frame);
    void SmoothTrails(cv::Mat &frame);
    void GridFilter8x(cv::Mat &frame);
    void GridFilter16x(cv::Mat &frame);
    void GridFilter8xBlend(cv::Mat &frame);
    void GridRandom(cv::Mat &frame);
    void GridRandomPixel(cv::Mat &frame);
    void Dual_SelfAlphaRainbow(cv::Mat &frame);
    void Dual_SelfAlphaBlur(cv::Mat &frame);
    void SurroundPixelXor(cv::Mat &frame);
    void Darken(cv::Mat &frame);
    void WeakBlend(cv::Mat &frame);
    void AverageVertical(cv::Mat &frame);
    void RandomCollectionAverage(cv::Mat &frame);
    void RandomCollectionAverageMax(cv::Mat &frame);
    void SmoothTrailsSelfAlphaBlend(cv::Mat &frame);
    void SmoothTrailsRainbowBlend(cv::Mat &frame);
    void MedianBlend(cv::Mat &frame);
    void SmoothRandomImageBlend(cv::Mat &frame);
    void SmoothImageAlphaBlend(cv::Mat &frame);
    void RandomAlphaBlend(cv::Mat &frame);
    void RandomTwoFilterAlphaBlend(cv::Mat &frame);
    void PixelatedSquare(cv::Mat &frame);
    void AlphaBlendPosition(cv::Mat &frame);
    void BlendRowAlpha(cv::Mat &frame);
    void BlendRow(cv::Mat &frame);
    void BlendRowByVar(cv::Mat &frame);
    void BlendRowByDirection(cv::Mat &frame);
    void BlendAlphaXor(cv::Mat &frame);
    void SelfXorScale(cv::Mat &frame);
    void BitwiseXorScale(cv::Mat &frame);
    void XorTrails(cv::Mat &frame);
    void RainbowTrails(cv::Mat &frame);
    void NegativeTrails(cv::Mat &frame);
    void IntenseTrails(cv::Mat &frame);
    void SelfAlphaRGB(cv::Mat &frame);
    void BlendImageOnOff(cv::Mat &frame);
    void XorSelfAlphaImage(cv::Mat &frame);
    void BitwiseXorStrobe(cv::Mat &frame);
    void AlphaBlendRandom(cv::Mat &frame);
    void ChannelSortAlphaBlend(cv::Mat &frame);
    void XorChannelSort(cv::Mat &frame);
    void GradientColors(cv::Mat &frame);
    void GradientColorsVertical(cv::Mat &frame);
    void Bitwise_XOR_Average(cv::Mat &frame);
    void ImageShiftMatrixLeft(cv::Mat &frame);
    void GradientXorSelfScale(cv::Mat &frame);
    void SmoothSourcePixel(cv::Mat &frame);
    void StrobeBlend(cv::Mat &frame);
    void FrameBars(cv::Mat &frame);
    void Sort_Vertical_Horizontal(cv::Mat &frame);
    void Sort_Vertical_Horizontal_Bitwise_XOR(cv::Mat &frame);
    void Scalar_Average_Multiply(cv::Mat &frame);
    void Scalar_Average(cv::Mat &frame);
    void Total_Average(cv::Mat &frame);
    void AlphaBlendImageXor(cv::Mat &frame);
    void FlashWhite(cv::Mat &frame);
    void FlashBlackAndWhite(cv::Mat &frame);
    void GaussianBlend(cv::Mat &frame);
    void RandomXor(cv::Mat &frame);
    void RandomXorFlash(cv::Mat &frame);
    void RandomAmountMedianBlur(cv::Mat &frame);
    void SoftXor(cv::Mat &frame);
    void SelfXorBlend(cv::Mat &frame);
    void SelfXorDoubleFlash(cv::Mat &frame);
    void SelfOrDoubleFlash(cv::Mat &frame);
    void BlendRowCurvedSqrt(cv::Mat &frame);
    void CycleShiftRGB(cv::Mat &frame);
    void CycleShiftRandomRGB(cv::Mat &frame);
    void CycleShiftRandomRGB_XorBlend(cv::Mat &frame);
    void CycleShiftRandomAlphaBlend(cv::Mat &frame);
    void VerticalColorBars(cv::Mat &frame);
    void GradientLeftRight(cv::Mat &frame);
    void GraidentUpDown(cv::Mat &frame);
    void GradientLeftRightInOut(cv::Mat &frame);
    void GradientUpDownInOut(cv::Mat &frame);
    void Lines(cv::Mat &frame);
    void ColorLines(cv::Mat &frame);
    void WhiteLines(cv::Mat &frame);
    void ThickWhiteLines(cv::Mat &frame);
    void UseLineObject(cv::Mat &frame);
    void TanAlphaGrid(cv::Mat &frame);
    void MedianBlendAnimation(cv::Mat &frame);
    void FibFlash(cv::Mat &frame);
    void ScaleFlash(cv::Mat &frame);
    void Curtain(cv::Mat &frame);
    void RandomCurtain(cv::Mat &frame);
    void CurtainVertical(cv::Mat &frame);
    void RandomCurtainVertical(cv::Mat &frame);
    void InOrder(cv::Mat &frame);
    void InOrderBySecond(cv::Mat &frame);
    void DarkenFilter(cv::Mat &frame);
    void RandomFilterBySecond(cv::Mat &frame);
    void ThreeRandom(cv::Mat &frame);
    void InOrderAlpha(cv::Mat &frame);
    void InOrderAlphaXor(cv::Mat &frame);
    void SlideFilterXor(cv::Mat &frame);
    void SlideFilter(cv::Mat &frame);
    void RandomSlideFilter(cv::Mat &frame);
    void SlideUpDown(cv::Mat &frame);
    void SlideUpDownXor(cv::Mat &frame);
    void SlideUpDownRandom(cv::Mat &frame);
    void SlideSubFilter(cv::Mat &frame);
    void SlideSubUpDownFilter(cv::Mat &frame);
    void ParticleBlend(cv::Mat &frame);
    void ParticleFlash(cv::Mat &frame);
    void ExactImage(cv::Mat &frame);
    void ParticleAlpha(cv::Mat &frame);
    void BlendInAndOut(cv::Mat &frame);
    void BlendScaleInAndOut(cv::Mat &frame);
    void AcidGlitch(cv::Mat &frame);
    void XorBackwards(cv::Mat &frame);
    void LiquidFilter(cv::Mat &frame);
    void MatrixXorAnd(cv::Mat &frame);
    void XorAlpha(cv::Mat &frame);
    void AlphaAcidTrails(cv::Mat &frame);
    void SelfXorAverage(cv::Mat &frame);
    void RandomXorBlend(cv::Mat &frame);
    void RGBVerticalXor(cv::Mat &frame);
    void RGBVerticalXorScale(cv::Mat &frame);
    void RGBHorizontalXor(cv::Mat &frame);
    void RGBHorizontalXorScale(cv::Mat &frame);
    void FadeStrobe(cv::Mat &frame);
    void RGBMirror(cv::Mat &frame);
    void MirrorStrobe(cv::Mat &frame);
    void AndStrobe(cv::Mat &frame);
    void AndStrobeScale(cv::Mat &frame);
    void AndPixelStrobe(cv::Mat &frame);
    void AndOrXorStrobe(cv::Mat &frame);
    void AndOrXorStrobeScale(cv::Mat &frame);
    void FadeInAndOut(cv::Mat &frame);
    void BrightStrobe(cv::Mat &frame);
    void DarkStrobe(cv::Mat &frame);
    void ParticleFast(cv::Mat &frame);
    void RandomXorOpposite(cv::Mat &frame);
    void StrobeTransform(cv::Mat &frame);
    void InitBlend(cv::Mat &frame);
    void MoveUpLeft(cv::Mat &frame);
    void RandomStrobe(cv::Mat &frame);
    void RandomBlur(cv::Mat &frame);
    void Stuck(cv::Mat &frame);
    void StuckStrobe(cv::Mat &frame);
    void OrStrobe(cv::Mat &frame);
    void LagBlend(cv::Mat &frame);
    void SubFilter(cv::Mat &frame);
    void BlendImageXor(cv::Mat &frame);
    void BlendImageAround_Median(cv::Mat &frame);
    void ImageBlendTransform(cv::Mat &frame);
    void RGBTrails(cv::Mat &frame);
    void RGBTrailsDark(cv::Mat &frame);
    void RGBTrailsAlpha(cv::Mat &frame);
    void RGBTrailsNegativeAlpha(cv::Mat &frame);
    void MovementRGBTrails(cv::Mat &frame);
    void RGBTrailsXor(cv::Mat &frame);
    void DifferenceStrobe(cv::Mat &frame);
    void BlackAndWhiteDifferenceStrobe(cv::Mat &frame);
    void DifferenceXor(cv::Mat &frame);
    void DifferenceRand(cv::Mat &frame);
    void DifferenceBrightStrobe(cv::Mat &frame);
    void PsycheTrails(cv::Mat &frame);
    void FourSquare(cv::Mat &frame);
    void EightSquare(cv::Mat &frame);
    void DiagonalSquare(cv::Mat &frame);
    void DiagonalSquareRandom(cv::Mat &frame);
    void SquareStretchDown(cv::Mat &frame);
    void SquareStretchRight(cv::Mat &frame);
    void SquareStretchUp(cv::Mat &frame);
    void SquareStretchLeft(cv::Mat &frame);
    void DarkTrails(cv::Mat &frame);
    void SoftFeedback(cv::Mat &frame);
    void SoftFeedbackFrames(cv::Mat &frame);
    void ResizeSoftFeedback(cv::Mat &frame);
    void SoftFeedback8(cv::Mat &frame);
    void SoftFeedbackFrames8(cv::Mat &frame);
    void ResizeSoftFeedback8(cv::Mat &frame);
    void ResizeSoftFeedbackSubFilter(cv::Mat &frame);
    void SoftFeedbackRandFilter(cv::Mat &frame);
    void SoftFeedback32(cv::Mat &frame);
    void SoftFeedbackFrames32(cv::Mat &frame);
    void ResizeSoftFeedback32(cv::Mat &frame);
    void SoftFeedbackRandFilter32(cv::Mat &frame);
    void SoftFeedbackSubFilter(cv::Mat &frame);
    void SoftFeedbackResize64(cv::Mat &frame);
    void SoftFeedbackResizeSubFilter(cv::Mat &frame);
    void SoftFeedbackResizeSubFilter64(cv::Mat &frame);
    void SoftFeedbackReszieSubFilter64_Negate(cv::Mat &frame);
    void SoftFeedbackReszieSubFilter64_Mirror(cv::Mat &frame);
    void HalfNegateStrobe(cv::Mat &frame);
    void MedianBlurXor(cv::Mat &frame);
    void NegateTrails(cv::Mat &frame);
    void RandomGradient(cv::Mat &frame);
    void RandomStrobeFlash(cv::Mat &frame);
    void RandomMirror(cv::Mat &frame);
    void RandomOther(cv::Mat &frame);
    void RandomXorFilter(cv::Mat &frame);
    void RandomMirrorBlend(cv::Mat &frame);
    void RandomMirrorAlphaBlend(cv::Mat &frame);
    void Bitwise_XOR_AlphaSubFilter(cv::Mat &frame);
    void AlphaBlendSubFilter(cv::Mat &frame);
    void GradientSubFilterXor(cv::Mat &frame);
    void XorBlend_SubFilter(cv::Mat &frame);
    void SmoothSubFilterAlphaBlend(cv::Mat &frame);
    void SmoothSubFilterXorBlend(cv::Mat &frame);
    void IntertwineSubFilter(cv::Mat &frame);
    void RandBlend(cv::Mat &frame);
    void EveryOther(cv::Mat &frame);
    void EveryOtherSubFilter(cv::Mat &frame);
    void SmoothRandomFilter(cv::Mat &frame);
    void RandomFilterRandomTimes(cv::Mat &frame);
    void RandomSubFilterRandomTimes(cv::Mat &frame);
    void AddToFrameSubFilter(cv::Mat &frame);
    void MirrorXor(cv::Mat &frame);
    void MirrorXorAll(cv::Mat &frame);
    void MirrorXorScale(cv::Mat &frame);
    void EnergyMirror(cv::Mat &frame);
    void SmoothSubFilter(cv::Mat &frame);
    void SmoothSubFilter16(cv::Mat &frame);
    void EnergizeSubFilter(cv::Mat &frame);
    void EnergizeSubFilter16(cv::Mat &frame);
    void EnergizeSubFilter32(cv::Mat &frame);
    void SmoothSubFilter32(cv::Mat &frame);
    void HalfAddSubFilter(cv::Mat &frame);
    void HalfXorSubFilter(cv::Mat &frame);
    void StaticXorBlend(cv::Mat &frame);
    void PsycheSort(cv::Mat &frame);
    void XorScale(cv::Mat &frame);
    void ChannelMedianSubFilter(cv::Mat &frame);
    void GaussianStrobe(cv::Mat &frame);
    void StrobeSort(cv::Mat &frame);
    void GlitchSortStrobe(cv::Mat &frame);
    void Bitwise_XOR_Blend(cv::Mat &frame);
    void Bitwise_XOR_Sort(cv::Mat &frame);
    void Bitwise_OR_Blend(cv::Mat &frame);
    void Bitwise_AND_Blend(cv::Mat &frame);
    void BitwiseColorMatrix(cv::Mat &frame);
    void PixelReverseXor(cv::Mat &frame);
    void PixelatedSubFilterSort(cv::Mat &frame);
    void SilverBlend(cv::Mat &frame);
    void RandomPixelOrderSort(cv::Mat &frame);
    void ImageXorAlpha(cv::Mat &frame);
    void ImageAverageXor(cv::Mat &frame);
    void PixelXorBlend(cv::Mat &frame);
    void SelfAlphaScale(cv::Mat &frame);
    void SelfScaleAlpha(cv::Mat &frame);
    void RainbowXorBlend(cv::Mat &frame);
    void FrameDifference(cv::Mat &frame);
    void SmallDiffference(cv::Mat &frame);
    void FadeBlend(cv::Mat &frame);
    void FilteredDifferenceSubFilter(cv::Mat &frame);
    void ExpandSquareSubFilter(cv::Mat &frame);
    void ExpandSquareBlendSubFilter(cv::Mat &frame);
    void ExpandSquareVerticalSubFilter(cv::Mat &frame);
    void DarkImageMedianBlend(cv::Mat &frame);
    void GammaDarken5(cv::Mat &frame);
    void GammaDarken10(cv::Mat &frame);
    void SelfAlphaScaleBlend(cv::Mat &frame);
    void FadeBars(cv::Mat &frame);
    void MirrorXorAlpha(cv::Mat &frame);
    void MirrorEnergizeSubFilter(cv::Mat &frame);
    void StrobeXor(cv::Mat &frame);
    void IntertwinedMirror(cv::Mat &frame);
    void BlurredMirror(cv::Mat &frame);
    void ShadeRGB(cv::Mat &frame);
    void InterRGB_SubFilter(cv::Mat &frame);
    void InterSmoothSubFilter(cv::Mat &frame);
    void InterRGB_Bars_XY(cv::Mat &frame);
    void InterRGB_Bars_X(cv::Mat &frame);
    void InterRGB_Bars_Y(cv::Mat &frame);
    void StoredFramesAlphaBlend_SubFilter(cv::Mat &frame);
    void BlendSubFilter(cv::Mat &frame);
    void BlendAlphaSubFilter(cv::Mat &frame);
    void ReverseFrameBlend(cv::Mat &frame);
    void ReverseFrameBlendSwitch(cv::Mat &frame);
    void DoubleRandomMirror(cv::Mat &frame);
    void Blend_AlphaSubFilter(cv::Mat &frame);
    void RandomBlendFilter(cv::Mat &frame);
    void DoubleRandomBlendFilter(cv::Mat &frame);
    void FlipBlendWH(cv::Mat &frame);
    void FlipBlendW(cv::Mat &frame);
    void FlipBlendH(cv::Mat &frame);
    void FlipBlendAll(cv::Mat &frame);
    void FrameMedianBlendSubFilter(cv::Mat &frame);
    void FrameBlurSubFilter(cv::Mat &frame);
    void ImageBlendSubFilter(cv::Mat &frame);
    void ImageBlendXorSubFilter(cv::Mat &frame);
    void ImageCollectionSubFilter(cv::Mat &frame);
    void SelfScaleXorIncrease(cv::Mat &frame);
    void Blend_RedGreenBlue(cv::Mat &frame);
    void XorBlend_RedGreenBlue(cv::Mat &frame);
    void BlendIncrease_RedGreenBlue(cv::Mat &frame);
    void Blend_RedReenBlue_Dark(cv::Mat &frame);
    void DarkModBlend(cv::Mat &frame);
    void PictureBuzz(cv::Mat &frame);
    void IncDifference(cv::Mat &frame);
    void IncDifferenceAlpha(cv::Mat &frame);
    void MirrorMedianBlend(cv::Mat &frame);
    void SubFilterMedianBlend(cv::Mat &frame);
    void DarkenBlend(cv::Mat &frame);
    void DarkCollectionSubFilter(cv::Mat &frame);
    void ChannelSort_NoBlend_Descending(cv::Mat &frame);
    void ChannelSort_NoBlend_Ascending(cv::Mat &frame);
    void Headrush(cv::Mat &frame);
    void DarkSmooth_Filter(cv::Mat &frame);
    void DarkSelfAlpha(cv::Mat &frame);
    void FlipMedian(cv::Mat &frame);
    void FlipMedianSubFilter(cv::Mat &frame);
    void FlipMirror(cv::Mat &frame);
    void FlipMirrorAverage(cv::Mat &frame);
    void FlipMirrorSubFilter(cv::Mat &frame);
    void ShuffleMedian(cv::Mat &frame);
    void ShuffleRGB(cv::Mat &frame);
    void ParticleSnow(cv::Mat &frame);
    void RandomPixels(cv::Mat &frame);
    void DarkRandomPixels(cv::Mat &frame);
    void MedianBlurSubFilter(cv::Mat &frame);
    void Bars(cv::Mat &frame);
    void ShuffleAlpha(cv::Mat &frame);
    void AlphaMorph(cv::Mat &frame);
    void ShuffleSelf(cv::Mat &frame);
    void PixelatedHorizontalLines(cv::Mat &frame);
    void PixelatedVerticalLines(cv::Mat &frame);
    void StrobeShuffle(cv::Mat &frame);
    void BlendBurred(cv::Mat &frame);
    void BlendCombinedValues(cv::Mat &frame);
    void RGBColorTrails(cv::Mat &frame);
    void BlendCombinedValueSubFilter(cv::Mat &frame);
    void BlendSubFilterAlpha(cv::Mat &frame);
    void GradientXorPixels(cv::Mat &frame);
    void PurpleRain(cv::Mat &frame);
    void PixelByPixelXor(cv::Mat &frame);
    void CopyXorAlpha(cv::Mat &frame);
    void AveragePixelsXor(cv::Mat &frame);
    void AveragePixelAlpha(cv::Mat &frame);
    void NegativeByRow(cv::Mat &frame);
    void AveragePixelCollection(cv::Mat &frame);
    void IncorrectLine(cv::Mat &frame);
    void XorShift(cv::Mat &frame);
    void StrobeXorAndOr(cv::Mat &frame);
    void XorWithSource(cv::Mat &frame);
    void AlphaBlendWithSource(cv::Mat &frame);
    void RGBSep1x(cv::Mat &frame);
    void RGBMedianBlend(cv::Mat &frame);
    void RGBMirror1(cv::Mat &frame);
    void RGBMirror1Median(cv::Mat &frame);
    void FlashMirror(cv::Mat &frame);
    void CollectionXorSourceSubFilter(cv::Mat &frame);
    void ReverseMirrorX(cv::Mat &frame);
    void MirrorXorAll_Reverse(cv::Mat &frame);
    void MirrorRGBReverse(cv::Mat &frame);
    void MirrorRGBReverseBlend(cv::Mat &frame);
    void BlendReverseSubFilter(cv::Mat &frame);
    void MirrorBitwiseXor(cv::Mat &frame);
    void SmoothBlendReverseSubFilter(cv::Mat &frame);
    void RandomIncrease(cv::Mat &frame);
    void MedianBlend16(cv::Mat &frame);
    void MedianBlendBufferSubFilter(cv::Mat &frame);
    void BGRBlend(cv::Mat &frame);
    void RGBBlend(cv::Mat &frame);
    void RGBBlendSubFilter(cv::Mat &frame);
    void DivideAndIncH(cv::Mat &frame);
    void DivideAndIncW(cv::Mat &frame);
    void XorOppositeSubFilter(cv::Mat &frame);
    void BlendSmoothSubFilter(cv::Mat &frame);
    void BlurSmooth(cv::Mat &frame);
    void BlurSmoothMedian(cv::Mat &frame);
    void BlurSmoothSubFilter(cv::Mat &frame);
    void BlurFlip(cv::Mat &frame);
    void BlurFlipSubFilter(cv::Mat &frame);
    void BlurMirrorGamma(cv::Mat &frame);
    void MedianBlendDark(cv::Mat &frame);
    void MedianBlendSubFilterEx(cv::Mat &frame);
    void EnergyMirrorDark(cv::Mat &frame);
    void AlphaBlendMirror(cv::Mat &frame);
    void MirrorAlphaBlendedImage(cv::Mat &frame);
    void AlphaBlendXorImage(cv::Mat &frame);
    void ShiftFrameSmoothSubFilter(cv::Mat &frame);
    void ShiftFrameStaticXorSubFilter(cv::Mat &frame);
    void IncreaseDecreaseGamma(cv::Mat &frame);
    void GammaIncDecIncrease(cv::Mat &frame);
    void RandomSubFilter2(cv::Mat &frame);
    void TwistedVision(cv::Mat &frame);
    void TwistedMirror(cv::Mat &frame);
    void SelfScaleSortBlend(cv::Mat &frame);
    void FlashMedianBlend(cv::Mat &frame);
    void BlendWithFrameSubFilter(cv::Mat &frame);
    void AlphaBlendWithFrameSubFilter(cv::Mat &frame);
    void AlphaXorBlendWithFrameSubFilter(cv::Mat &frame);
    void XorBlendSubFilter(cv::Mat &frame);
    void FlipAlphaBlend(cv::Mat &frame);
    void RandomFlipFilter(cv::Mat &frame);
    void MirrorMedian(cv::Mat &frame);
    void FlipMatrixCollection(cv::Mat &frame);
    void MirrorMatrixCollection(cv::Mat &frame);
    void MirrorMatrixSource(cv::Mat &frame);
    void SelfScaleByFrame(cv::Mat &frame);
    void SmoothMedianRotateSubFilter(cv::Mat &frame);
    void SmoothCollectionAlphaBlend(cv::Mat &frame);
    void XorSubFilter(cv::Mat &frame);
    void XorAlphaSubFilter(cv::Mat &frame);
    void BlurXorAlphaSubFilter(cv::Mat &frame);
    void ImageXorFrame(cv::Mat &frame);
    void ImageXorFunction(cv::Mat &frame);
    void ImageXorAlphaBlend(cv::Mat &frame);
    void ImageAlphaXorMedianSubFilter(cv::Mat &frame);
    void ImageSmoothAlphaXorSubFilter(cv::Mat &frame);
    void ImageXorMirrorFilter(cv::Mat &frame);
    void ImageXorSubFilter(cv::Mat &frame);
    void ImageAlphaXorSubFilter(cv::Mat &frame);
    void SmoothTrailsBlend(cv::Mat &frame);
    void MatrixCollectionRGBXor(cv::Mat &frame);
    void RainbowGlitch(cv::Mat &frame);
    void RainbowGlichStrobe(cv::Mat &frame);
    void NegateSwitchStrobe(cv::Mat &frame);
    void StrobeAlphaShuffle(cv::Mat &frame);
    void ShuffleAlphaWithRGB(cv::Mat &frame);
    void ShuffleAlphaSubFilter(cv::Mat &frame);
    void ShuffleColorMap(cv::Mat &frame);
    void BlendWithRainbowSubFilter(cv::Mat &frame);
    void BlendWithJetSubFilter(cv::Mat &frame);
    void ColormapBlendSubFilter(cv::Mat &frame);
    void RandomColorMap(cv::Mat &frame);
    void SmoothMirrorBlurFlip(cv::Mat &frame);
    void RandomColorMapAlphaBlendSubFilter(cv::Mat &frame);
    void RandomOrder(cv::Mat &frame);
    void RandomOrderMedianBlendSubFilter(cv::Mat &frame);
    void MirrorOrder(cv::Mat &frame);
    void MirrorOrderSubFilter(cv::Mat &frame);
    void BlurMirrorOrder(cv::Mat &frame);
    void AveragePixelMirror(cv::Mat &frames);
    void ShuffleAlphaMedianBlend(cv::Mat &frame);
    void MirrorOrderAlpha(cv::Mat &frame);
    void FilterStrobeSubFilter(cv::Mat &frame);
    void ImageSubtractMedianBlend(cv::Mat &frame);
    void ImageDarkBlend(cv::Mat &frame);
    void ImageAverageDark(cv::Mat &frame);
    void ImageRemainderPixel(cv::Mat &frame);
    void AverageLinesBlend(cv::Mat &frame);
    void SoftFeedbackMirror(cv::Mat &frame);
    void AverageVerticalLinesBlend(cv::Mat &frame);
    void LinesMedianBlend(cv::Mat &frame);
    void XorSquare(cv::Mat &frame);
    void PixelValuesPlusOne(cv::Mat &frame);
    void AverageHorizontalFilter(cv::Mat &frame);
    void AverageVerticalFilter(cv::Mat &frame);
    void GradientAlphaXorHorizontal(cv::Mat &frame);
    void GradientAlphaXorVertical(cv::Mat &frame);
    void BlendImageWithSubFilter(cv::Mat &frame);
    void BlendImageWithSubFilterAlpha(cv::Mat &frame);
    void MedianBlendSoft(cv::Mat &frame);
    void AndImageSubFilterXor(cv::Mat &frame);
    void AlphaBlendImageSubFilterXor(cv::Mat &frame);
    void AlphaBlendImageSubFilterXorRev(cv::Mat &frame);
    void ParticleReleaseXor(cv::Mat &frame);
    void ParticleReleaseXorVec(cv::Mat &frame);
    void ParticleReleaseAlphaBlend(cv::Mat &frame);
    void ParticleReleaseWithImage(cv::Mat &frame);
    void ParticleReleaseSubFilter(cv::Mat &frame);
    void ParticleReleaseImageSubFilter(cv::Mat &frame);
    void ImageEnergy(cv::Mat &frame);
    void ImageEnergySubFilter(cv::Mat &frame);
    void ImageDistortion(cv::Mat &frame);
    void ImageDistortionSubFilter(cv::Mat &frame);
    void SmoothExactImageXorAlpha(cv::Mat &frame);
    void FeedbackColormap(cv::Mat &frame);
    void SmoothImageAlphaBlendMedian(cv::Mat &frame);
    void ImageDarkenSmoothMedian(cv::Mat &frame);
    void XorReverseImageSmooth(cv::Mat &frame);
    void ReverseSubFilterBlend(cv::Mat &frame);
    void ReverseSubFilterXor(cv::Mat &frame);
    void ImageReverseSubFilter(cv::Mat &frame);
    void SmoothRainbowMedian(cv::Mat &frame);
    void MirrorAlphaBlend(cv::Mat &frame);
    void ImageSmoothMedianBlend(cv::Mat &frame);
    void ImageSmoothMedianSubFilter(cv::Mat &frame);
    void ImageAlphaXorMedianBlend(cv::Mat &frame);
    void MatrixCollectionBlend(cv::Mat &frame);
    void MatrixCollectionSubFilter(cv::Mat &frame);
    void MatrixCollectionImageSubFilter(cv::Mat &frame);
    void MatrixCollectionBlurAlpha(cv::Mat &frame);
    void MatrixCollectionXor(cv::Mat &frame);
    void MatrixCollectionXor32(cv::Mat &frame);
    void MatrixCollectionRandomColorMap(cv::Mat &frame);
    void MatrixCollectionDarkXor(cv::Mat &frame);
    void MatrixCollectionRGB(cv::Mat &frame);
    void TrailsSubFilter(cv::Mat &frame);
    void TrailsSubFilter32(cv::Mat &frame);
    void CompareWithSubFilter(cv::Mat &frame);
    void MedianTrails(cv::Mat &frame);
    void SmoothMedianBlend(cv::Mat &frame);
    void ColorTransition(cv::Mat &frame);
    void ColorTransitionMedian(cv::Mat &frame);
    void ColorTransitionRandom(cv::Mat &frame);
    void ColorTransitionRandomMedian(cv::Mat &frame);
    void ColorTransitionSubFilter(cv::Mat &frame);
    void ColorTransitionImageSubFilter(cv::Mat &frame);
    void CurtainSubFilter(cv::Mat &frame);
    void RandomTrails(cv::Mat &frame);
    void RandomTrailsSubFilter(cv::Mat &frame);
    void CosSinMedianBlend(cv::Mat &frame);
    void TrailsRGB(cv::Mat &frame);
    void MatrixTrailsXorRandom(cv::Mat &frame);
    void CosSinMultiplyCollectionXor(cv::Mat &frame);
    void Filter8_Blend(cv::Mat &frame);
    void Filter8_SubFilter(cv::Mat &frame);
    void RandomSmoothAlphaMedian(cv::Mat &frame);
    void RandomAlphaBlendFilter(cv::Mat &frame);
    void RandomMirrorBitwiseXor(cv::Mat &frame);
    void SquareDivideSubFilter(cv::Mat &frame);
    void SquareSubFilter(cv::Mat &frame);
    void SquareSubFilter8(cv::Mat &frame);
    void ColorExpand(cv::Mat &frame);
    void ColorExpandSubFilter(cv::Mat &frame);
    void RotateImage(cv::Mat &frame);
    void RotateBlendImage(cv::Mat &frame);
    void RotateImageSubFilter(cv::Mat &frame);
    void RotateAlphaBlendImage(cv::Mat &frame);
    void FlipShuffle(cv::Mat &frame);
    void FlipRandom(cv::Mat &frame);
    void FlipOrder(cv::Mat &frame);
    void FlipStrobeSubFilter(cv::Mat &frame);
    void MirrorBlendFrame(cv::Mat &frame);
    void MirrorBlendVertical(cv::Mat &frame);
    void MirrorVerticalAndHorizontal(cv::Mat &frame);
    void BlendFor360(cv::Mat &frame);
    void MirrorSidesMedian(cv::Mat &frame);
    void MirrorSidesSubFilter(cv::Mat &frame);
    void MedianFrameAlphaBlendSubFilter(cv::Mat &frame);
    void MedianSubFilter(cv::Mat &frame);
    void ColorXorScale(cv::Mat &frame);
    void ColorXorScaleSubFilter(cv::Mat &frame);
    void ImageXorScale(cv::Mat &frame);
    void MatrixCollectionShiftSubFilter(cv::Mat &frame);
    void MatrixCollectionImageShiftSubFilter(cv::Mat &frame);
    void MatrixCollectionXorBlendAlphaBlend(cv::Mat &frame);
    void MatrixCollectionBlurImageXorAlpha(cv::Mat &frame);
    void MatrixCollectionBlurImageSubFilter(cv::Mat &frame);
    void MatrixCollectionBlurImageSubFilter16(cv::Mat &frame);
    void ImageAlphaBlendSubFilter(cv::Mat &frame);
    void MultipleMatrixCollectionSubFilter(cv::Mat &frame);
    void BlurAlphaSubFilter(cv::Mat &frame);
    void BlurImageSubFilter(cv::Mat &frame);
    void MedianBlendSubFilter(cv::Mat &frame);
    void MedianBlendImageSubFilter(cv::Mat &frame);
    void MedianBlendSelfBlend(cv::Mat &frame);
    void BlendHalfSubFilter(cv::Mat &frame);
    void BlurImageAlphaBlend(cv::Mat &frame);
    void BlurImageAlphaBlendSubFilter(cv::Mat &frame);
    void BlurImageAlphaBlendScaleSubFilter(cv::Mat &frame);
    void RandomAmountOfMedianBlur(cv::Mat &frame);
    void Bitwise_XOR_BlendFrame(cv::Mat &frame);
    void AlphaBlendWithSubFilter(cv::Mat &frame);
    void AlphaBlendScaleWithSubFilter(cv::Mat &frame);
    void GaussianBlendEx(cv::Mat &frame);
    void SimpleMatrixBlend(cv::Mat &frame);
    void MatrixBlendSubFilter(cv::Mat &frame);
    void SmoothMatrixBlendSubFilter(cv::Mat &frame);
    void BlurSmoothSubFilterAlphaBlend(cv::Mat &frame);
    void BlurSmoothAlphaXorBlendSubFilter(cv::Mat &frame);
    void BlurTwiceSubFilter(cv::Mat &frame);
    void BlurFrameBlendSubFilter(cv::Mat &frame);
    void BlurFrameSubFilter(cv::Mat &frame);
    void BlurSmoothMatrix(cv::Mat &frame);
    void MedianBlurInc(cv::Mat &frame);
    void GaussianBlurInc(cv::Mat &frame);
    void BlurSmoothMedianInc(cv::Mat &frame);
    void BlurSmoothGaussianInc(cv::Mat &frame);
    void BlurMatrixCollectionXor(cv::Mat &frame);
    void MatrixCollection8XorSubFilter(cv::Mat &frame);
    void BlurSmoothRevFilter(cv::Mat &frame);
    void SurroundingPixels(cv::Mat &frame);
    void SurroundingPixelsAlpha(cv::Mat &frame);
    void MatrixCollectionSurroundingPixels(cv::Mat &frame);
    void MatrixCollectionSurroundingPixelsSubFilter(cv::Mat &frame);
    void MatrixCollectionSurroundingPixelsImage(cv::Mat &frame);
    void MatrixCollectionSurroundingPixelsImageSubFilter(cv::Mat &frame);
    void ImageTransparent(cv::Mat &frame);
    void MatrixImageAlphaBlendSubFilter(cv::Mat &frame);
    void ImageAlphaCollectionSmoothBlend(cv::Mat &frame);
    void ImageRandomColormap(cv::Mat &frame);
    void ImageRandomColormapAlphaBlend(cv::Mat &frame);
    void ImageRandomColormapAlphaScale(cv::Mat &frame);
    void ImageRandomColormapSubFilter(cv::Mat &frame);
    void ImageShuffle(cv::Mat &frame);
    void ImageSubFilter(cv::Mat &frame);
    void ImageAlphaBlendWithFrameSubFilter(cv::Mat &frame);
    void ImageFadeInOut(cv::Mat &frame);
    void ImageFadeBlackInOut(cv::Mat &frame);
    void ImageFadeBlackInOutSubFilter(cv::Mat &frame);
    void ImageFadeFrameInOut(cv::Mat &frame);
    void ImageFadeFrameInOutSubFilter(cv::Mat &frame);
    void ImageFadeDouble(cv::Mat &frame);
    void BlendSubFilterAndImage(cv::Mat &frame);
    void FadeSubFilter(cv::Mat &frame);
    void FadeSubFilterRev(cv::Mat &frame);
    void ImageBlendSubFilterMedianBlend(cv::Mat &frame);
    void FadeSubFilterXor(cv::Mat &frame);
    void BlurXorSubFilter(cv::Mat &frame);
    void ColorFlashIncrease(cv::Mat &frame);
    void ScaleFilter(cv::Mat &frame);
    void NegativeDarkenXor(cv::Mat &frame);
    void ImageXor_SubFilter(cv::Mat &frame);
    void NegateBlendSubFilter(cv::Mat &frame);
    void StrobeNegatePixel(cv::Mat &frame);
    void StrobeNegateInOut(cv::Mat &frame);
    void ImageStrobeOnOff(cv::Mat &frame);
    void AlphaStrobeBlend(cv::Mat &frame);
    void CannyRandomPixels(cv::Mat &frame);
    void FrameImageFadeInOut(cv::Mat &frame);
    void FrameImageFadeInOutDouble(cv::Mat &frame);
    void ChangeEachSecond(cv::Mat &frame);
    void ShuffleImage(cv::Mat &frame);
    void ChangeImageEachSecond(cv::Mat &frame);
    void ChangeImageFilterOnOff(cv::Mat &frame);
    void ChangeXorEachSecond(cv::Mat &frame);
    void MorphXor(cv::Mat &frame);
    void MorphXorWithSubFilter(cv::Mat &frame);
    void MirrorEachSecond(cv::Mat &frame);
    void MirrorReverseSubFilter(cv::Mat &frame);
    void MirrorReverseSubFilterAlphaBlend(cv::Mat &frame);
    void Mirror_Xor_Combined(cv::Mat &frame);
    void MirrorFrameShuffle(cv::Mat &frame);
    void MirrorShuffleSmooth(cv::Mat &frame);
    void Mirror_Xor_Smooth(cv::Mat &frame);
    void XorFrameShuffle(cv::Mat &frame);
    void XorMirrorBlendFrame(cv::Mat &frame);
    void ImageXorSmooth(cv::Mat &frame);
    void SmoothSubFilter64(cv::Mat &frame);
    void SmoothMedian64(cv::Mat &frame);
    void SmoothMedian32_SubFilter(cv::Mat &frame);
    void SmoothAlphaMedian_SubFilter(cv::Mat &frame);
    void SmoothImage_SubFilter(cv::Mat &frame);
    void SmoothImageMedian_SubFilter(cv::Mat &frame);
    void SmoothImageAndSubFilter(cv::Mat &frame);
    void SmoothSubFilter90(cv::Mat &frame);
    void SmoothMedianImageSubFilter16(cv::Mat &frame);
    void ImageNegate(cv::Mat &frame);
    void ImageNegateAlphaBlend(cv::Mat &frame);
    void ImageNegateAlphaBlendSubFilter(cv::Mat &frame);
    void FrameNegateAlphaBlendImage(cv::Mat &frame);
    void DarkTrailsEffect(cv::Mat &frame);
    void DarkNegate(cv::Mat &frame);
    void ChannelSortMedianBlend(cv::Mat &frame);
    void MatrixCollectionMirrorDirection(cv::Mat &frame);
    void StrobeRandomChannel(cv::Mat &frame);
    void SplitFramesSort(cv::Mat &frame);
    void SplitFrameSortSubFilter(cv::Mat &frame);
    void MedianBlend64(cv::Mat &frame);
    void SplitFrameFilter(cv::Mat &frame);
    void SplitFrameBlend(cv::Mat &frame);
    void SplitFrameBlendSubFilter(cv::Mat &frame);
    void SplitFrameCollection(cv::Mat &frame);
    void SplitFrameMirror(cv::Mat &frame);
    void RandomChannels(cv::Mat &frame);
    void SmoothRandomChannels(cv::Mat &frame);
    void SmoothChannelSubFilter(cv::Mat &frame);
    void IncreaseRGB(cv::Mat &frame);
    void IncreaseColor(cv::Mat &frame);
    void SaturateBlend(cv::Mat &frame);
    void SaturateBlendSubFilter(cv::Mat &frame);
    void MaxRGB(cv::Mat &frame);
    void XorDifferenceFilter(cv::Mat &frame);
    void AlphaBlendChannelSort(cv::Mat &frame);
    void ColorTrailsFilter(cv::Mat &frame);
    void ColorTrailsSubFilter(cv::Mat &frame);
    void DarkNegateRainbowMedian(cv::Mat &frame);
    void IncreaseQuick(cv::Mat &frame);
    void IncreaseRandomIndex(cv::Mat &frame);
    void ImageChannelSubFilter(cv::Mat &frame);
    void ImageChannelChangeSubFilter(cv::Mat &frame);
    void ImageChannelRandom(cv::Mat &frame);
    void ImageChannelRandomSubFilter(cv::Mat &frame);
    void PixelateBlur(cv::Mat &frame);
    void PixelateBlock(cv::Mat &frame);
    void PixelateNoResize12(cv::Mat &frame);
    void PixelateNoResize8(cv::Mat &frame);
    void PixelateNoResize32(cv::Mat &frame);
    void PixelateNoResize16(cv::Mat &frame);
    void PixelateNoResize24(cv::Mat &frame);
    void PixelateBlurFilter(cv::Mat &frame);
    void PixelateBlurSubFilter(cv::Mat &frame);
    void MirrorXorImage(cv::Mat &frame);
    void MirrorXorSubFilter(cv::Mat &frame);
    void PixelXorImageSubFilter(cv::Mat &frame);
    void PixelAlphaImageSubFilter(cv::Mat &frame);
    void PixelScaleImageSubFilter(cv::Mat &frame);
    void PixelImageSubFilter(cv::Mat &frame);
    void PixelImageTex(cv::Mat &frame);
    void PixelImageXorSubFilter(cv::Mat &frame);
    void PixelRowMedianBlend(cv::Mat &frame);
    void IntertwineRows32(cv::Mat &frame);
    void IntertwineRows16(cv::Mat &frame);
    void IntertwineRows8(cv::Mat &frame);
    void IntertwineAlpha(cv::Mat &frame);
    void IntertwineRowsSubFilter(cv::Mat &frame);
    void IntertwineRows4(cv::Mat &frame);
    void Intertwine64x4(cv::Mat &frame);
    void Intertwine64X(cv::Mat &frame);
    void Intertwine64XSubFilter(cv::Mat &frame);
    void Intertwine64XPixel(cv::Mat &frame);
    void IntertwinePixels(cv::Mat &frame);
    void IntertwineColsX(cv::Mat &frame);
    void IntertwineCols16(cv::Mat &frame);
    void IntertwineCols8(cv::Mat &frame);
    void IntertwineCols32(cv::Mat &frame);
    void MirrorIntertwine(cv::Mat &frame);
    void MirrorIntertwineImage(cv::Mat &frame);
    void MirrorIntertwineImageSubFilter(cv::Mat &frame);
    void IntertwineImageSubFilter(cv::Mat &frame);
    void BlendWithImage(cv::Mat &frame);
    void IntertwineRowsReverse32(cv::Mat &frame);
    void IntertwineRowsReverse16(cv::Mat &frame);
    void IntertwineRowsReverse8(cv::Mat &frame);
    void IntertwineRowsReverse64X(cv::Mat &frame);
    void IntertwineRowsSwitch32(cv::Mat &frame);
    void IntertwineRowsSwitch16(cv::Mat &frame);
    void IntertwineRowsSwitch8(cv::Mat &frame);
    void IntertwineRows64(cv::Mat &frame);
    void IntertwineRowsReverse64(cv::Mat &frame);
    void IntertwineRowsSwitch64(cv::Mat &frame);
    void IntertwineShuffle(cv::Mat &frame);
    void InterwtineAlphaSubFilter(cv::Mat &frame);
    void IntertwineImageAndSubFilter(cv::Mat &frame);
    void IntertwineRowsAndCols(cv::Mat &frame);
    void IntertwineRowsAndColsSubFilter(cv::Mat &frame);
    void MatrixCollectionVariable(cv::Mat &frame);
    void MatrixCollectionVariableSubFilter(cv::Mat &frame);
    void MatrixCollectionVariableImage(cv::Mat &frame);
    void MatrixCollectionVariableImageSubFilter(cv::Mat &frame);
    void SmoothImageAverageSubFilter(cv::Mat &frame);
    void CallSubFilterBlend50(cv::Mat &frame);
    void CallSubFilterBlend25(cv::Mat &frame);
    void CallSubFilterBlend75(cv::Mat &frame);
    void IntertwineColsX2(cv::Mat &frame);
    void SmoothFrame64(cv::Mat &frame);
    void SmoothFrame32(cv::Mat &frame);
    void MatrixCollectionMedianBlendFilter(cv::Mat &frame);
    void MedianBlendIncreaseFilter(cv::Mat &frame);
    void MedianBlendNegate(cv::Mat &frame);
    void BlendWithFrameAndImageSubFilter(cv::Mat &frame);
    void ImageIntertwine32(cv::Mat &frame);
    void ImageIntertwine16(cv::Mat &Frame);
    void ImageIntertwine8(cv::Mat &frame);
    void ImageIntertwine64X(cv::Mat &frame);
    void IntertwineDoubleSubFilter(cv::Mat &frame);
    void IntertwineDoubleImageSubFilter(cv::Mat &frame);
    void IntertwineEachRowXSubFilter(cv::Mat &frame);
    void IntertwineGhost(cv::Mat &frame);
    void IntertwineGhost32(cv::Mat &frame);
    void IntertwineGhostSubFilter(cv::Mat &frame);
    void BlendWithOldFrame(cv::Mat &frame);
    void BlendWith16thFrame(cv::Mat &frame);
    void BlendTrailsSubFilter(cv::Mat &frame);
    void LineMoveInOut(cv::Mat &frame);
    void MatrixCollectionTrails(cv::Mat &frame);
    void MatrixCollectionRandom(cv::Mat &frame);
    void MatrixCollectionRandom(cv::Mat &frame);
    void MatrixCollectionRandomSmooth(cv::Mat &frame);
    void MatrixCollectionRandomDouble(cv::Mat &frame);
    void MatrixCollectionAddImage(cv::Mat &frame);
    void MatrixCollectionAlphaBlendSubFilter(cv::Mat &frame);
    void MatrixCollectionImageBlendSubFilter(cv::Mat &frame);
    void TwitchinGlitchin(cv::Mat &frame);
    void IntertwineRowsImageAlphaBlend(cv::Mat &frame);
    void ChannelSortSubFilter(cv::Mat &frame);
    void IntertwineRow2(cv::Mat &frame);
    void IntertwineRow720p(cv::Mat &frame);
    void IntertwineRow1080p(cv::Mat &frame);
    void IntertwineRow720pX2(cv::Mat &frame);
    void IntertwineRow1080pX2(cv::Mat &frame);
    void IntertwineCols1280(cv::Mat &frame);
    void IntertwineRowCols1280x720(cv::Mat &frame);
    void IntertwineRowsImage(cv::Mat &frame);
    void MedianBlendSquare(cv::Mat &frame);
    void SmoothIntertwineMedianBlend(cv::Mat &frame);
    void SmoothBlendMedian(cv::Mat &frame);
    void SmoothDoubleSubFilter(cv::Mat &frame);
    void AlphaBlendMedianSubFilter(cv::Mat &frame);
    void ReverseOnOff(cv::Mat &frame);
    void SmoothReverseSubFilter(cv::Mat &frame);
    void IntertwineWithSubFilter(cv::Mat &frame);
    void IntertwineWithImageAndSubFilter(cv::Mat &frame);
    void IntertwineFrameWithImage(cv::Mat &frame);
    void InterlaceVerticalFilter(cv::Mat &frame);
    void InterlaceHorizontalFilter(cv::Mat &frame);
    void IntertwineImageFlip(cv::Mat &frame);
    void IntertwineImageFlipSubFilter(cv::Mat &frame);
    void IntertwineFlipImageAndSubFilter(cv::Mat &frame);
    void IntertwineImageOnOff(cv::Mat &frame);
    void IntertwineVideoShuffle(cv::Mat &frame);
    void ImageFlipFilter(cv::Mat &frame);
    void FlipFrameFilter(cv::Mat &frame);
    void AlphaBlendImageAndSubFilter(cv::Mat &frame);
    void SquareFill(cv::Mat &frame);
    void StretchAlphaBlend(cv::Mat &frame);
    void StretchAlphaBlendWidth(cv::Mat &frame);
    void StretchAlphaBlendHeight(cv::Mat &frame);
    void StretchAlphaBlendTwitch(cv::Mat &frame);
    void StretchAlphaBlednTwitchFast(cv::Mat &frame);
    void StretchRandomValues(cv::Mat &frame);
    void ImageRandomValues(cv::Mat &frame);
    void AlphaBlendTrails(cv::Mat &frame);
    void VideoStretchHorizontal(cv::Mat &frame);
    void VideoStretchVertical(cv::Mat &frame);
    void StrobeTrailsFilter(cv::Mat &frame);
    void AlphaBlendTrailsReverse(cv::Mat &frame);
    void ShadowAlphaTrails16(cv::Mat &frame);
    void ShadowAlphaTrailsReset(cv::Mat &frame);
    void SetColormap(cv::Mat &frame);
    void ShadowAlphaTrails(cv::Mat &frame);
    void ShadowAlphaTrails64(cv::Mat &frame);
    void MirrorLeft(cv::Mat &frame);
    void MirrorRight(cv::Mat &frame);
    void MirrorBackAndForth(cv::Mat &frame);
    void FadeFromColorToColor(cv::Mat &frame);
    void FadeFromColorToColorImage(cv::Mat &frame);
    void Random_Filter(cv::Mat &frame);
    void FlipY_Axis(cv::Mat &frame);
    void FlipX_Axis(cv::Mat &frame);
    void FlipBoth(cv::Mat &frame);
    void FlipMirrorAlphaBlend(cv::Mat &frame);
    void Random_FilterX2(cv::Mat &frame);
    void Random_FilterSubFilter(cv::Mat &frame);
    void IntertwineRow480pX2(cv::Mat &frame);
    void LowFi(cv::Mat &frame);
    void HighToLow(cv::Mat &frame);
    void LowToHigh(cv::Mat &frame);
    void MoveHighAndLow(cv::Mat &frame);
    void StretchSubFilter(cv::Mat &frame);
    void Quality480(cv::Mat &frame);
    void Quality720(cv::Mat &frame);
    void Quality1080(cv::Mat &frame);
    void StretchVerticalMirror(cv::Mat &frame);
    void ImageLoFi(cv::Mat &frame);
    void ImageLofiAlphaBlend(cv::Mat &frame);
    void ImageAlphaBlendScale(cv::Mat &frame);
    void FrameStretchAlphaBlend(cv::Mat &frame);
    void BlurHighToLow(cv::Mat &frame);
    void ImageHighToLowAlpha(cv::Mat &frame);
    void MirrorTopToBottom(cv::Mat &frame);
    void MirrorBottomToTop(cv::Mat &frame);
    void FlashRGB_SubFilter(cv::Mat &frame);
    void MirrorSwitch(cv::Mat &frame);
    void MirrorSwitchSubFilter(cv::Mat &frame);
    void MirrorSwitchFlip(cv::Mat &frame);
    void BlendImageLayer(cv::Mat &frame);
    void StrobeRandomFilter(cv::Mat &frame);
    void AlphaBlendRandom_Filter(cv::Mat &frame);
    void DoubleRandomAlphaImageSubFilter(cv::Mat &frame);
    void MirrorLeftMirrorRightBlend(cv::Mat &frame);
    void MirrorTopMirrorBottomBlend(cv::Mat &frame);
    void MirrorAll(cv::Mat &frame);
    void ImageDiff(cv::Mat &frame);
    void ImageDiffSubFilter(cv::Mat &frame);
    void RestoreBlack(cv::Mat &frame);
    void OrigBlendSubFilter(cv::Mat &frame);
    void OrigAndCurrentRandomX2(cv::Mat &frame);
    void FillPixelsImage(cv::Mat &frame);
    void AverageHorizontalDistortion(cv::Mat &frame);
    void AlphaBlendImageWithOrigSource(cv::Mat &frame);
    void ResizeFrameWidth(cv::Mat &frame);
    void ResizeFrameHeight(cv::Mat &frame);
    void ResizeFrameWidthAndHeight(cv::Mat &frame);
    void ResizeImageHeight(cv::Mat &frame);
    void ResizeImageWidth(cv::Mat &frame);
    void ResizeImageWidthAndHeight(cv::Mat &frame);
    void ResizeImageAndFrameBlend(cv::Mat &frame);
    void ResizeImageWidthAndHeightSubFilter(cv::Mat &frame);
    void ResizeImageFrameWidth(cv::Mat &frame);
    void ResizeImageFrameHeight(cv::Mat &frame) ;
    void ResizeImageFrameWidthAndHeight(cv::Mat &frame);
    void ResizeImageRandom(cv::Mat &frame);
    void ResizeFrameRandom(cv::Mat &frame);
    void ResizeFrameImageFlash(cv::Mat &frame);
    void RandomFlip(cv::Mat &frame);
    void ColorsFilter(cv::Mat &frame);
    void MirrorDoubleVision(cv::Mat &frame);
    void ColorFadeFilter(cv::Mat &frame);
    void ColorChannelMoveUpAndDown(cv::Mat &frame);
    void MedianStrobe(cv::Mat &frame);
    void DifferenceSubFilter(cv::Mat &frame);
    void AlphaBlendFlippedFilter(cv::Mat &frame);
    void ColorVariableBlend(cv::Mat &frame);
    void ColorXorBlend(cv::Mat &frame);
    void ColorAddBlend(cv::Mat &frame);
    void IntertwineRowsShadow720p(cv::Mat &frame);
    void IntertwineRowsAlpha720p(cv::Mat &frame);
    void IntertwineByFrameDown(cv::Mat &frame);
    void IntertwineByFrameUp(cv::Mat &frame);
    void IntertwineFrameFast(cv::Mat &frame);
    void IntertwineFrame360(cv::Mat &frame);
    void IntertwineFrame360X(cv::Mat &frame);
    void IntertwineFrameTwitch(cv::Mat &frame);
    void IntertwineFrameDark(cv::Mat &frame);
    void IntertwineFrame360_Reverse(cv::Mat &frame);
    void IntertwineFrameBlend(cv::Mat &frame);
    void IntertwineXorCollection(cv::Mat &frame);
    void IntertwineFrame720X(cv::Mat &frame);
    void IntertwineFrame1080X(cv::Mat &frame);
    void IntertwineFrameImage1080X(cv::Mat &frame);
    void RandomXorMultiThreadTest(cv::Mat &frame);
    void SelfAlphaBlendMultiThread(cv::Mat &frame);
    void MedianBlendMultiThread(cv::Mat &frame);
    void BytePixelSort(cv::Mat &frame);
    void SortedImageColorVariable(cv::Mat &frame);
    void SmoothColorVariableImageBlend(cv::Mat &frame);
    void BlendChannelXor(cv::Mat &frame);
    void ColorShiftXor(cv::Mat &frame);
    void RandomSquares(cv::Mat &frame);
    void RandomImageSquares(cv::Mat &frame);
    void Random_FilterX2_SubFilter(cv::Mat &frame);
    void FuzzyFilter(cv::Mat &frame);
    void XorMultiply(cv::Mat &frame);
    void ColorShadowBlend(cv::Mat &frame);
    void Grayscale(cv::Mat &frame);
    void FlashMatrixTrails(cv::Mat &frame);
    void GhostTrails(cv::Mat &frame);
    void MedianBlend_Random_Filter(cv::Mat &frame);
    void IntertwineMirrorEnergy(cv::Mat &frame);
    void IntertwineMultipleRows(cv::Mat &Frame);
    void GhostTwitch(cv::Mat &frame);
    void GhostTwitchTrails(cv::Mat &frame);
    void Dyskinesia(cv::Mat &frame);
    void Mirror_Rainbow_Blur(cv::Mat &frame);
    void MirrorMedianBlur(cv::Mat &frame);
    void VideoTwitch(cv::Mat &frame);
    void ErodeFilter(cv::Mat &frame);
    void DilateFilter(cv::Mat &frame);
    void MirrorLeftBottomToTop(cv::Mat &frame);
    void MirrorRightTopToBottom(cv::Mat &frame);
    void BilateralFilter(cv::Mat &frame);
    void BilateralFilterFade(cv::Mat &frame);
    void BilateralBlend(cv::Mat &frame);
    void BilateralBlendFade(cv::Mat &frame);
    void BilateralFilterScale(cv::Mat &frame);
    void ColorRGB_IncDec(cv::Mat &frame);
    void ColorCollection(cv::Mat &frame);
    void ColorCollectionRandom(cv::Mat &frame);
    void ColorCollectionStrobeShift(cv::Mat &frame);
    void ColorCollectionRandom_Filter(cv::Mat &frame);
    void ColorCollectionShift(cv::Mat &frame);
    void CollectionEnergy(cv::Mat &frame);
    void ColorCollectionInterlace(cv::Mat &frame);
    void ColorCollectionStrobeShake(cv::Mat &frame);
    void ColorCollectionSubFilter(cv::Mat &frame);
    void ColorCollectionShiftSubFilter(cv::Mat &frame);
    void ColorCollectionSubtle(cv::Mat &frame);
    void ColorCollection64(cv::Mat &frame);
    void ColorCollectionSubtleStrobe(cv::Mat &frame);
    void CollectionRandom(cv::Mat &frame);
    void CollectionRandomSubFilter(cv::Mat &frame);
    void CollectionImage(cv::Mat &frame);
    void CollectionAlphaXor(cv::Mat &frame);
    void ColorCollection64X(cv::Mat &frame);
    void ColorCollectionSwitch(cv::Mat &frame);
    void ColorCollectionRGB_Index(cv::Mat &frame);
    void ColorCollectionRGBStrobeSubFilter(cv::Mat &frame);
    void ColorCollectionGhostTrails(cv::Mat &frame);
    void ColorCollectionScale(cv::Mat &frame);
    void ColorCollectionReverseStrobe(cv::Mat &frame);
    void CollectionAlphaBlend_SubFilter(cv::Mat &frame);
    void ColorCollectionXorPixel(cv::Mat &frame);
    void BlendWithSource25(cv::Mat &frame);
    void BlendWithSource50(cv::Mat &frame);
    void BlendWithSource75(cv::Mat &frame);
    void BlendWithSource100(cv::Mat &frame);
    void ColorCollectionXorOffsetFlash(cv::Mat &frame);
    void ColorCollectionMatrixGhost(cv::Mat &frame);
    void MildStrobe(cv::Mat &frame);
    void ReduceBy50(cv::Mat &frame);
    void AlphaBlendWithSourceScale(cv::Mat &frame);
    void ColorPositionAverageXor(cv::Mat &frame);
    void ColorPositionXor(cv::Mat &frame);
    void ColorPositionXorMedianBlend(cv::Mat &frame);
    void CannyStrobe(cv::Mat &frame);
    void LaplacianStrobe(cv::Mat &frame);
    void LaplacianStrobeOnOff(cv::Mat &frame);
    void ColorCollectionPixelXor(cv::Mat &frame);
    void GrayStrobe(cv::Mat &frame);
    void ColorStrobeXor(cv::Mat &frame);
    void ColorGhost(cv::Mat &frame);
    void ColorCollectionTwitchSubFilter(cv::Mat &frame);
    void BlurredOutXor(cv::Mat &frame);
    void BoxFilter(cv::Mat &frame);
    void DizzyFilter(cv::Mat &frame);
    void Buzzed(cv::Mat &frame);
    void BlendWithImage25(cv::Mat &frame);
    void BlendWithImage50(cv::Mat &frame);
    void BlendWithImage75(cv::Mat &frame);
    void BuzzedDark(cv::Mat &frame);
    void AlphaBlendImageDownUp(cv::Mat &frame);
    void BlendWithImageAndSource(cv::Mat &frame);
    void PixelSourceFrameBlend256(cv::Mat &frame);
    void SplitMatrixCollection(cv::Mat &frame);
    void RectangleGlitch(cv::Mat &frame);
    void PositionShift(cv::Mat &frame);
    void ColorCollectionMovementIndex(cv::Mat &frame);
    void Shake(cv::Mat &frame);
    void Disoriented(cv::Mat &frame);
    void ColorCollectionPositionStrobe(cv::Mat &frame);
    void ColorCollectionStrobeBlend(cv::Mat &frame);
    void AlphaBlendStoredFrames(cv::Mat &frame);
    void SplitMatrixSortChannel(cv::Mat &frame);
    void SplitMatrixSortChannelArrayPosition(cv::Mat &frame);
    void SplitMatrixSortChannelImage(cv::Mat &frame);
    void ShiftColorLeft(cv::Mat &frame);
    void CycleInAndOutRepeat(cv::Mat &frame);
    void ColorCollectionShuffle(cv::Mat &frame);
    void BlendFromXtoY(cv::Mat &frame);
    void BlendImageXtoY(cv::Mat &frame);
    void BlendColorImageStrobeSubFilter(cv::Mat &frame);
    void BlendByColsSubFilter(cv::Mat &frame);
    void BlendByColsImage(cv::Mat &frame);
    void IntertwineX(cv::Mat &frame);
    void IntertwineY(cv::Mat &frame);
    void IntertwineSubFilterX(cv::Mat &frame);
    void IntertwineSubFilterY(cv::Mat &frame);
    void IntertwineImageX(cv::Mat &frame);
    void IntertwineImageY(cv::Mat &frame);
    void InertwineImageSubFilter(cv::Mat &frame);
    void IntertwineHorizontalImageSubFilter(cv::Mat &frame);
    void InterwtineHorizontalImageSubFilterByIndex(cv::Mat &frame);
    void IntertwineHorizontalImageSubFilterMatrixCollection(cv::Mat &frame);
    void BlendForwardAndBack16(cv::Mat &frame);
    void BlendForwardAndBack32(cv::Mat &frame);
    void BlendForwardAndBack64(cv::Mat &frame);
    void BlendForwardAndBack8_RandomStrobe(cv::Mat &frame);
    void AlphaBlendWithThreeCollections(cv::Mat &frame);
    void AlphaBlendArrayRandom(cv::Mat &frame);
    void AlphaBlendArrayTrailsFilters(cv::Mat &frame);
    void AlphaBlendMirrorChannelSort(cv::Mat &frame);
    void ColorCollectionAlphaBlendArray(cv::Mat &frame);
    void AlphaBlendArrayExpand(cv::Mat &frame);
    void ColorImageMatrixFade(cv::Mat &frame);
    void ColorImageMatrixFadeFast(cv::Mat &frame);
    void ColorImageMatrixFadeDirection(cv::Mat &frame);
    void ColorImageMatrixFadeDirectionBlend(cv::Mat &frame);
    void ColorMatrixCollectionPixelation(cv::Mat &frame);
    void ColorMatrixColllectionImagePixelation(cv::Mat &frame);
    void ColorMatrixCollectionImagePixelationSubFilter(cv::Mat &frame);
    void ColorImageMatrixMedianBlend(cv::Mat &frame);
    void ColorImageBlendPixelMedian(cv::Mat &frame);
    void ColorImageBlendOrigMedian(cv::Mat &frame);
    void ColorImageBlendSource(cv::Mat &frame);
    void ColorImageMatrixFadeOnAndOff(cv::Mat &frame);
    void ColorImageMatrixRect(cv::Mat &frame);
    void ColorImageBackAndForth(cv::Mat &frame);
    void ColorImageFadeReallyFast(cv::Mat &frame);
    void ColorImageSubFilterXor(cv::Mat &frame);
    void ColorImageAllXor(cv::Mat &frame);
    void ColorChannelIteration(cv::Mat &frame);
    void ColorImageMatrixFadeVariable(cv::Mat &frame);
    void ColorXorImageFade(cv::Mat &frame);
    void ColorFreezeBlend(cv::Mat &frame);
    void ColorImageFill(cv::Mat &frame);
    void ColorImageFillMatrix(cv::Mat &frame);
    void ColorImageFillSubFilter(cv::Mat &frame);
    void ColorImagePixelsResetSubFilter(cv::Mat &frame);
    void ColorIncrementReset(cv::Mat &frame);
    void ColorPixelArray2D(cv::Mat &frame);
    void ManualShell(cv::Mat &frame);
    void ColorIncrementRandomReset(cv::Mat &frame);
    void ColorImageIncrementResetCollection(cv::Mat &frame);
    void ColorMoveDown(cv::Mat &frame);
    void ColorMoveDownResetMedianBlend(cv::Mat &frame);
    void ColorMoveDownSmoothMedianBlend(cv::Mat &frame);
    void PixelRandom3(cv::Mat &frame);
    void ShiftMatrixLeft(cv::Mat &frame);
    void ShiftMatrixLeftSubFilter(cv::Mat &frame);
    void ShiftMatrixUpSubFilter(cv::Mat &frame);
    void PixelatePixelValues(cv::Mat &frame);
    void StretchCollection(cv::Mat &frame);
    void PsychedelicSlitScan(cv::Mat &frame);
    void SineValue(cv::Mat &frame);
    void SineTransitionUpLeft(cv::Mat &frame);
    void TemporaryTrails(cv::Mat &frame);
    void StrobingPixelDissolve(cv::Mat &frame);
    void ImagePixelFrameBlend(cv::Mat &frame);
    void PreviewFilters(cv::Mat &frame);
    void EachFilterSubFilter(cv::Mat &frame);
    void EachFilterRandomStartSubFilter(cv::Mat &frame);
    void PixelPsychosis(cv::Mat &frame);
    void PixelCloud(cv::Mat &frame);
    void PiixelXorBlendFrame(cv::Mat &frame);
    void PixelImageBlendFrame(cv::Mat &frame);
    void PixelCollectionRandom(cv::Mat &frame);
    void PixelCollectionSubFilter(cv::Mat &frame);
    void PixelCollectionMatrixSubFilter(cv::Mat &frame);
    void PixelCollectionIncreaseSubFilter(cv::Mat &frame);
    void PixelCollectionImageBlend(cv::Mat &frame);
    void ColorPulseIncrease(cv::Mat &frame);
    void PixelCollectionLongMatrixSubFilter(cv::Mat &frame);
    void ColorPulseImageIncreaseSubFilter(cv::Mat &frame);
    void ColorPulseRandom(cv::Mat &frame);
    void ColorPulseImage(cv::Mat &frame);
    void ColorPulseAlpha(cv::Mat &frame);
    void ColorLower75(cv::Mat &frame);
    void ColorImageMedianBlend(cv::Mat &frame);
    void ColorDullBlur(cv::Mat &frame);
    void ColorImageDull(cv::Mat &frame);
    void ColorImageBlendWithFrame(cv::Mat &frame);
    void ColorImageBlendSubFilter(cv::Mat &frame);
    void ColorMatrixImageFilter(cv::Mat &frame);
    void ColorXorMatrixImage(cv::Mat &frame);
    void MatrixColorBlur(cv::Mat &frame);
    void ColorImageLower(cv::Mat &frame);
    void BlurSubFilter8(cv::Mat &frame);
    void LightBlend(cv::Mat &frame);
    void LightBlendXor(cv::Mat &frame);
    void Source_ImageSubFilter(cv::Mat &frame);
    void XorZeroImage(cv::Mat &frame);
    void SlowDownFilterSubFilter(cv::Mat &frame);
    void VariableFilterSubFilter(cv::Mat &frame);
    void SingleFrameGlitch(cv::Mat &frame);
    void ColorFadeSlow(cv::Mat &frame);
    void FadeBetweenSubFilter(cv::Mat &frame);
    void FadeBetweenFrame(cv::Mat &frame);
    void TrailsTwitch(cv::Mat &frame);
    void Tremors(cv::Mat &frame);
    void XorImageIndexPixel(cv::Mat &frame);
    void ImageStrobeMedianBlend(cv::Mat &frame);
    void StrobeImageArrayPosition(cv::Mat &frame);
    void OppositeImageArray(cv::Mat &frame);
    void NoMedianBlurBlendMultiThread(cv::Mat &frame);
    void NegateNoBlurMedian(cv::Mat &frame);
    void PulseIncreaseVariableSpeed(cv::Mat &frame);
    void Source25_Image75(cv::Mat &frame);
    void LightStrobe(cv::Mat &frame);
    void StrobeRedChannel(cv::Mat &frame);
    void StrobeGreenChannel(cv::Mat &frame);
    void StrobeBlueChannel(cv::Mat &frame);
    void StrobeOnOffRandomChannel(cv::Mat &frame);
    void StrobeLightFlashRandomChannel(cv::Mat &frame);
    void LostSideDeadImageSubFilter(cv::Mat &frame);
    void SwapImageFrameSubFilter(cv::Mat &frame);
    void PulseIncreaseFast(cv::Mat &frame);
    void FibonacciXor(cv::Mat &frame);
    void ColorFibonacci(cv::Mat &frame);
    void ImageFibonacci(cv::Mat &frame);
    void ImageFibonacciInAndOut(cv::Mat &frame);
    void ImageFibonacciStrobe(cv::Mat &frame);
    void ImageFibonacciMedianBlend(cv::Mat &frame);
    void ImageFibonacciInAndOutSubFilter(cv::Mat &frame);
    void ImageKaleidoscopeSubFilter(cv::Mat &frame);
    void ImageMirrorLeftRight(cv::Mat &frame);
    void ImageMirrorShiftUpLeftDownRight(cv::Mat &frame);
    void SelfScaleRefined(cv::Mat &frame);
    void MetalMedianBlend(cv::Mat &frame);
    void SelfScaleRefinedRGB(cv::Mat &frame);
    void ImageFreezeReleaseRepeat(cv::Mat &frame);
    void ImageReplaceColorIntensity(cv::Mat &frame);
    void ImageReplaceColorIntensitySubFilter(cv::Mat &frame);
    void ImageReplaceColorIntensitySubFilterInOut(cv::Mat &frame);
    void ImageFillColor(cv::Mat &frame);
    void MultiRandFilter(cv::Mat &frame);
    void MultiFilter(cv::Mat &frame);
    void GradientRandom(cv::Mat &frame);
    void LineMedianBlend(cv::Mat &frame);
    void PerfectMedianBlend(cv::Mat &frame);
    void ImageRowAlphaSubFilter(cv::Mat &frame);
    void IndexPixelRowSubFilter(cv::Mat &frame);
    void IndexSourceRowSubFilter(cv::Mat &frame);
    void IndexSourceBlendSubFilter(cv::Mat &frame);
    void BlendFilterWithSubFilter(cv::Mat &frame);
    void FadeFilterWithSubFilter(cv::Mat &frame);
    void AlphaBlendByRowSubFilter(cv::Mat &frame);
    void ColorMatrixTrailsSubFilter(cv::Mat &frame);
    void LoFi_320x240_Interlaced(cv::Mat &frame);
    void LoFi_320x240(cv::Mat &frame);
    void LoFi_320x240_SubFilter(cv::Mat &frame);
    void LoFi_320x240_Alpha_SubFilter(cv::Mat &frame);
    void LoFi_160x120_SubFilter(cv::Mat &frame);
    void LoFi_Trails(cv::Mat &frame);
    void LoFi_ImageScaleSubFilter(cv::Mat &frame);
    void MedianShift(cv::Mat &frame);
    void ImageAlienKaleidoscope(cv::Mat &frame);
    void ImageMirror_Left(cv::Mat &frame);
    void ImageMirror_Right(cv::Mat &frame);
    void ImageMirror_Up(cv::Mat &frame);
    void ImageMirror_Down(cv::Mat &frame);
    void ImageMirror_LeftSubFilter(cv::Mat &frame);
    void ImageMirror_RightSubFilter(cv::Mat &frame);
    void ImageMirror_UpSubFilter(cv::Mat &frame);
    void ImageMirror_DownSubFilter(cv::Mat &frame);
    void ImageMirrorKaleidoscope(cv::Mat &frame);
    void ExactImageSubFilter(cv::Mat &frame);
    void AlphaImageSourceScale(cv::Mat &frame);
    void GhostMirrorReversed(cv::Mat &frame);
    void GhostMirrorFade(cv::Mat &frame);
    void MirrorFadeLeftRight(cv::Mat &frame);
    void FadeMirrorSubFilter(cv::Mat &frame);
    void MirrorFadeUpDown(cv::Mat &frame);
    void DarkTrailsKaleidoscope(cv::Mat &frame);
    void MirrorFadeAll(cv::Mat &frame);
    void KaleidoscopeMirrorSubFilter(cv::Mat &frame);
    void ImageKaleidoscopeMirrorAlphaSubFilter(cv::Mat &frame);
    void BlendOppositesSubFilter(cv::Mat &frame);
    void AlphaBlendMirrorFade(cv::Mat &frame);
    void DarkenMedianBlend(cv::Mat &frame);
    void ReduceMedianBlend(cv::Mat &frame);
    void DarkColors(cv::Mat &frame);
    void DarkenChannelXorNoMedian(cv::Mat &frame);
    void DarkenStrobeMedianBlend(cv::Mat &frame);
    void DarkenImageAlphaBlend(cv::Mat &frame);
    void MatrixStrobeTrails(cv::Mat &frame);
    void MatrixStrobeMedianBlend(cv::Mat &frame);
    void ImageEnergyKaleidoscopeSubFilter(cv::Mat &frame);
    void ImageEnergyKaleidoscopeBlendSubFilter(cv::Mat &frame);
    void ImageEnergyKaleidoscopeEvenSubFilter(cv::Mat &frame);
    void ImageEnergyFadeSubFilter(cv::Mat &frame);
    void ImageEnergizeBlendFilter(cv::Mat &frame);
    void ImageEnergizeSubFilter(cv::Mat &frame);
    void MatrixCollectionSmoothedSubFilter(cv::Mat &frame);
    void MonoColorTrails(cv::Mat &frame);
    void TremorMonoColorTrails(cv::Mat &frame);
    void StrobeMonoColorTrails(cv::Mat &frame);
    void StrobeEffectColorTrails(cv::Mat &frame);
    void PixelStrobeColorTrails(cv::Mat &frame);
    void ElectricImageFilter(cv::Mat &frame);
    void ImageEnergyStrobeSubFilter(cv::Mat &frame);
    void Twin(cv::Mat &frame);
    void TwinKaleidoscope(cv::Mat &frame);
    void MedianBlendMultiThread_2160p(cv::Mat &frame);
    void MedianBlendMultiThreadByFour(cv::Mat &frame);
    void MedianBlendMultiThreadByEight(cv::Mat &frame);
    void MedianBlendMultiThreadByTweleve(cv::Mat &frame);
    void MedianBlendMultiThreadByThree(cv::Mat &frame);
    void MedianBlendIncrease(cv::Mat &frame);
    void MedianBlendIncrease16(cv::Mat &frame);
    void StrangeGlitch64(cv::Mat &frame);
    void StrangeGlitch16(cv::Mat &frame);
    void CollectionMatrixOutline(cv::Mat &frame);
    void CollectionMatrixSubFilter(cv::Mat &frame);
    void CollectionMatrixRandomMedianBlend(cv::Mat &frame);
    void ImageCollectionMatrixOutline(cv::Mat &frame);
    void ImageCollectionMatrixOutlineSubFilter(cv::Mat &frame);
    void ImageCollectionMatrixFillSubFilter(cv::Mat &frame);
    void ImageCollectionMatrixFadeInOutSubFilter(cv::Mat &frame);
    void ImageCollectionMatrixIntensitySubFilter(cv::Mat &frame);
    void ImageCollectionMatrixMedianSubFilter(cv::Mat &frame);
    void CollectionMatrxOutlineAlphaMedianBlend(cv::Mat &frame);
    void ImageSourceOrigXor(cv::Mat &frame);
    void MatrixCollectionAuraTrails(cv::Mat &frame);
    void MatrixCollectionAuraTrails32(cv::Mat &frame);
    void PixelIntensityFillSubFilter(cv::Mat &frame);
    void SmoothImageFrameSubFilter(cv::Mat &frame);
    void ImageCycleBlend(cv::Mat &frame);
    void ImageCycleAlphaBlend(cv::Mat &frame);
    void ImageCycleXor(cv::Mat &frame);
    void ImageCycleMedian(cv::Mat &frame);
    void ImageCycleAlphaSubFilter(cv::Mat &frame);
    void ImageCycleSmoothAlphaBlend(cv::Mat &frame);
    void ImageCycleSmoothAlphaBlend32(cv::Mat &frame);
    void BlendWithSourceMedianBlendMultiThreadEight(cv::Mat &frame);
    void BlendWithImageMedianMutliThreadEight(cv::Mat &frame);
    void GradientBlendWithImage(cv::Mat &frame);
    void DarkColorMovement(cv::Mat &frame);
    void DarkColorFibonacci(cv::Mat &frame);
    void UseFilterOnAndOffSubFilter(cv::Mat &frame);
    void UseFilterOnAndOffEachQuarterSecondSubFilter(cv::Mat &frame);
    void UseFilterOnAndOffByEightSubFilter(cv::Mat &frame);
    void UseImageOnAndOffByEight(cv::Mat &frame);
    void UseImageOnAndOff(cv::Mat &frame);
    void SelfAlphaScale_Down(cv::Mat &frame);
    void PsychoticVision(cv::Mat &frame);
    void DarkPsychoticVision(cv::Mat &frame);
    void Mirror_ReverseColor(cv::Mat &frame);
    void ImageColorCycleMorph(cv::Mat &frame);
    void ImageBlendAndSubFilter(cv::Mat &frame);
    void ImageSquareShrink(cv::Mat &frame);
    void ImageSquareShrinkFast(cv::Mat &frame);
    void ImageSquareShrinkAlpha(cv::Mat &frame);
    void ImageSquareExpand(cv::Mat &frame);
    void ImageKaleidoscopeNoBlur(cv::Mat &frame);
    void KaleidoscopeSubFilter(cv::Mat &frame);
    void ImageSquareShrinkSubFilter(cv::Mat &frame);
    void DifferenceReplaceSubFilter(cv::Mat &frame);
    void InvertedDifferenceReplaceSubFilter(cv::Mat &frame);
    void ImageInvertedDifferenceReplaceSubFilter(cv::Mat &frame);
    void ImageDifferenceReplaceSubFilter(cv::Mat &frame);
    void DifferenceReplaceSubFilterAlphaBlend(cv::Mat &frame);
    void GradientGlitch(cv::Mat &frame);
    void ImageGradientBlend(cv::Mat &frame);
    void MedianBlendGradientMultiThread(cv::Mat &frame);
    void MedianBlendGradientDarkMultiThread(cv::Mat &frame);
    void GradientAlphaBlend(cv::Mat &frame);
    void GradientFilter(cv::Mat &frame);
    void GradientFilterAlphaBlend(cv::Mat &frame);
    void MedianBlendGradientFilterMultiThread(cv::Mat &frame);
    void MedianBlendGraidentFilterDarkMultiThread(cv::Mat &frame);
    void ColorOrderSwap(cv::Mat &frame);
    void ColorOrderSwapMap(cv::Mat &frame);
    void MedianBlendSwapMapMultiThread(cv::Mat &frame);
    void RandomGradientColors(cv::Mat &frame);
    void GradientColorMap(cv::Mat &frame);
    void GradientXor(cv::Mat &frame);
    void RandomSubFilter(cv::Mat &frame);
    void ShuffleSubFilter(cv::Mat &frame);
    void Shuffle_Filter(cv::Mat &frame);
    void RandomOrigFrame(cv::Mat &frame);
    void ColorVariableRectangles(cv::Mat &frame);
    void VariableRectangles(cv::Mat &frame);
    void VariableRectanglesSimple(cv::Mat &frame);
    void VariableRectanglesExtra(cv::Mat &frame);
    void VariableRectangleImageAlphaBlend(cv::Mat &frame);
    void MirrorSwitchMode(cv::Mat &frame);
    void MirrorSwitchLeftRight(cv::Mat &frame);
    void VariableRectanglesSmall(cv::Mat &frame);
    void VariableRectanglesLarge(cv::Mat &frame);
    void VariableRectanglesImageCollection(cv::Mat &frame);
    void VariableRectanglesVariableImageSubFilter(cv::Mat &frame);
    void RainbowXorStrobeBlend(cv::Mat &frame);
    void IncreaseRainbowXorBlend(cv::Mat &frame);
    void ColorStrobeIndexSubFilter(cv::Mat &frame);
    void MatrixCollectionRandomFrames(cv::Mat &frame);
    void MatrixCollectionFrames(cv::Mat &frame);
    void MatrixCollectionFramesLeft(cv::Mat &frame);
    void MatrixCollectionFramesMirrorLeft(cv::Mat &frame);
    void MatrixCollectionFramesTop(cv::Mat &frame);
    void FrameSqueeze(cv::Mat &frame);
    void MatrixCollectionBlendLeftToRight(cv::Mat &frame);
    void MedianBlendMatrixCollection(cv::Mat &frame);
    void MedianBlendMatrixCollection_2160p(cv::Mat &frame);
    void MedianBlendMatrixCollection_2160p_X16(cv::Mat &frame);
    void MatrixCollectionAdd(cv::Mat &frame);
    void MatrixCollection_Xor(cv::Mat &frame);
    void ImageMatrixCollectionBlend(cv::Mat &frame);
    void MatrixCollectionNegateAdd(cv::Mat &frame);
    void MatrixCollectionFrameRect(cv::Mat &frame);
    void MatrixCollectionRows8x1_SubFilter(cv::Mat &frame);
    void MatrixCollectionRows8x8_SubFilter(cv::Mat &frame);
    void MatrixCollectionRows2x2_SubFilter(cv::Mat &frame);
    void MatrixCollectionRows4x4_SubFilter(cv::Mat &frame);
    void IntertwineBlock(cv::Mat &frame);
    void IntertwineVertical(cv::Mat &frame);
    void IntertwineImageVerticalSubFilter(cv::Mat &frame);
    void MirrorDiamond(cv::Mat &frame);
    void MirrorDiamondRight(cv::Mat &frame);
    void MirrorDiamondReverse(cv::Mat &frame);
    void MirrorDiamondBlend(cv::Mat &frame);
    void StretchOutward(cv::Mat &frame);
    void MirrorDiamondRandom(cv::Mat &frame);
    void MatrixCollectionAlphaRow(cv::Mat &frame);
    void MedianBlendCollectionAlphaRow(cv::Mat &frame);
    void MedianBlendDoubleVision(cv::Mat &frame);
    void MedianBlendMultiThreadScale(cv::Mat &frame);
    void AcidShuffleMedian(cv::Mat &frame);
    void MedianBlendSmoothAlpha(cv::Mat &frame);
    void MirrorDiamondSubFilter(cv::Mat &frame);
    void ImageFadeTo(cv::Mat &frame);
    void ImageFadeToXor(cv::Mat &frame);
    void MedianBlendImageFadeTo(cv::Mat &frame);
    void MedianMirrorAllBlend(cv::Mat &frame);
    void DigitalHaze(cv::Mat &frame);
    void BlendWithColor(cv::Mat &frame);
    void DigitalHazeBlend(cv::Mat &frame);
    void VideoAlphaBlend25(cv::Mat &frame);
    void VideoAlphaBlend50(cv::Mat &frame);
    void VideoAlphaBlend75(cv::Mat &frame);
    void VideoExactFrame(cv::Mat &frame);
    void VideoAlphaFade(cv::Mat &frame);
    void VideoAlphaBlendSubFilter(cv::Mat &frame);
    void VideoAlphaImage(cv::Mat &frame);
    void VideoAlphaImageFrame(cv::Mat &frame);
    void VideoAlphaImageScale(cv::Mat &frame);
    void VideoAlphaBlendImage25(cv::Mat &frame);
    void VideoAlphaBlendImage50(cv::Mat &frame);
    void VideoAlphaBlendImage75(cv::Mat &frame);
    void VideoFrameFilterSubFilter(cv::Mat &frame);
    void VideoFrameImageSubFilter(cv::Mat &frame);
    void VideoAlphaSubFilter(cv::Mat &frame);
    void VideoRoll(cv::Mat &frame);
    void VideoRollReverse(cv::Mat &frame);
    void VideoRollMedianBlend(cv::Mat &frame);
    void VideoRollScaleMedianBlend(cv::Mat &frame);
    void VideoSubFilter(cv::Mat &frame);
    void MedianBlendWithColor(cv::Mat &frame);
    void VideoImage(cv::Mat &frame);
    void VideoImageFade(cv::Mat &frame);
    void ColorMovement(cv::Mat &frame);
    void MedianBlendColorMovement(cv::Mat &frame);
    void ColorMovementRange(cv::Mat &frame);
    void ColorPixelOrder(cv::Mat &frame);
    void ColorPixelOrder32(cv::Mat &frame);
    void MedianBlendMuliThread_Pixelate(cv::Mat &frame);
    void SelfScaleGlitch(cv::Mat &frame);
    void PixelGlitch(cv::Mat &frame);
    void VideoPixelOnOffSwitch(cv::Mat &frame);
    void VideoBlendImageSubFilter(cv::Mat &frame);
    void VideoAlphaImageSubFilter(cv::Mat &frame);
    void MedianBlendVideo(cv::Mat &frame);
    void VideoFrameBlur8(cv::Mat &frame);
    void VideoFrameBlur16(cv::Mat &frame);
    void VideoFrameBlur32(cv::Mat &frame);
    void VideoSourceFrameBlur8(cv::Mat &frame);
    void VideoSourceFrameBlur16(cv::Mat &frame);
    void VideoSourceFrameBlur32(cv::Mat &frame);
    void VideoImageSourceFrame(cv::Mat &frame);
    void SlowRed(cv::Mat &frame);
    void SlowGreen(cv::Mat &frame);
    void SlowBlue(cv::Mat &frame);
    void VideoMirrorAlpha(cv::Mat &frame);
    void VideoMirrorLeftRight(cv::Mat &frame);
    void VideoFlipFrames(cv::Mat &frame);
    void VideoImageBlend(cv::Mat &frame);
    void VideoCycleReverse(cv::Mat &frame);
    void VideoCycleMedianReverse(cv::Mat &frame);
    void VideoStrobeFrameOnOff(cv::Mat &frame);
    void VideoMirror(cv::Mat &frame);
    void VideoMedianBlendScale(cv::Mat &frame);
    void VideoMedianBlend(cv::Mat &frame);
    void VideoBlendSubFilter(cv::Mat &frame);
    void VideoLoFi(cv::Mat &frame);
    void VideoLoFiBlend(cv::Mat &frame);
    void VideoLoFiSourceNormal(cv::Mat &frame);
    void VideoDarken25(cv::Mat &frame);
    void VideoDarken50(cv::Mat &frame);
    void VideoDarken75(cv::Mat &frame);
    void VideoDarkenBlend25(cv::Mat &frame);
    void VideoDarkenBlend50(cv::Mat &frame);
    void VideoDarkenBlend75(cv::Mat &frame);
    void VideoBlendSmooth8(cv::Mat &frame);
    void VideoBlendSmooth16(cv::Mat &frame);
    void VideoBlendSmooth32(cv::Mat &frame);
    void VideoXorSource(cv::Mat &frame);
    void VideoXorSelfScale(cv::Mat &frame);
    void MedianBlendNoBlurWithColor(cv::Mat &frame);
    void VideoImageSmooth(cv::Mat &frame);
    void ColorMapFilter(cv::Mat &frame);
    void VideoColorMap(cv::Mat &frame);
    void VideoColorMapAlphaBlend(cv::Mat &frame);
    void ReduceColors(cv::Mat &frame);
    void ReduceColorsRandom(cv::Mat &frame);
    void ReduceColorsRed(cv::Mat &frame);
    void ReduceColorsGreen(cv::Mat &frame);
    void ReduceColorsBlue(cv::Mat &frame);
    void BlendRandomPixels(cv::Mat &frame);
    void MedianBlendMultiThread_LoFi(cv::Mat &frame);
    void ExpandFrame(cv::Mat &frame);
    void ExpandImage(cv::Mat &frame);
    void ExpandVideo(cv::Mat &frame);
    void ColorIncreaseFadeRGB(cv::Mat &frame);
    void ColorIncreaseInOutRGB(cv::Mat &frame);
    void AlphaVideoXor(cv::Mat &frame);
    void DarkScaleNoBlur(cv::Mat &frame);
    void LightBlur(cv::Mat &frame);
    void LightMedianBlendMultiThread(cv::Mat &frame);
    void ColorFadeMedianBlendMultiThread(cv::Mat &frame);
    void ColorFadeBlendMedianBlendMultiThread(cv::Mat &frame);
    void MedianBlendMultiThread32(cv::Mat &frame);
    void MedianBlendMultiThread4(cv::Mat &frame);
    void MedianBlendFadeWithColor(cv::Mat &frame);
    void GaussianBlurMild(cv::Mat &frame);
    void MedianBlendWithColorValue(cv::Mat &frame);
    void MedianBlendWithColorInOutRGB(cv::Mat &frame);
    void MedianBlendWithCollection16(cv::Mat &frame);
    void FadeRtoGtoB(cv::Mat &frame);
    void MedianBlendFadeRtoGtoB(cv::Mat &frame);
    void FadeRtoGtoB_Increase(cv::Mat &frame);
    void MedianBlendFadeRtoGtoB_Increase(cv::Mat &frame);
    void FadeRandomChannel(cv::Mat &frame);
    void FadeRandomChannel_Increase(cv::Mat &frame);
    void MedianBlendFadeRandomChannel(cv::Mat &frame);
    void GlitchyTrails(cv::Mat &frame);
    void GlitchyXorTrails(cv::Mat &frame);
    void GlitchedFilter(cv::Mat &frame);
    void DisplaySometimesSubFilter(cv::Mat &frame);
    void GlitchyVideoXorTrails(cv::Mat &frame);
    void GltichedVideoFilter(cv::Mat &frame);
    void DualGitchyVideoXorTrails(cv::Mat &frame);
    void StaticGlitch(cv::Mat &frame);
    void VideoMirrorMultiDirection(cv::Mat &frame);
    void SoftwareGlitch_64(cv::Mat &frame);
    void VideoMatrixBlendDouble(cv::Mat &frame);
    void VideoMatrixFadeDouble(cv::Mat &frame);
    void VideoMatrixColorSmooth(cv::Mat &frame);
    void VideoMedianBlendShared8(cv::Mat &frame);
    void VideoMedianBlendShared16(cv::Mat &frame);
    void GlitchedGrid(cv::Mat &frame);
    void ShuffleGlitch(cv::Mat &frame);
    void ShuffleVideoMatrix(cv::Mat &frame);
    void TruncateColor(cv::Mat &frame);
    void TruncateColor75(cv::Mat &frame);
    void TruncateColor25(cv::Mat &frame);
    void TruncateVariable(cv::Mat &frame);
    void TruncateVariableSlow(cv::Mat &frame);
    void TruncateVariableScale(cv::Mat &frame);
    void MedianBlendTruncate(cv::Mat &frame);
    void MedianBlendTruncateScale(cv::Mat &frame);
    void Variable_ColorScale(cv::Mat &frame);
    void MedianBlendVariable_ColorScale(cv::Mat &frame);
    void InOut_ColorScale(cv::Mat &frame);
    void MedianBlendInOut_ColorScale(cv::Mat &frame);
    void CollectionTruncate(cv::Mat &frame);
    void MedianBlendCollectionTruncate(cv::Mat &frame);
    void VideoBlendTruncate(cv::Mat &frame);
    void RandomTruncateFrame(cv::Mat &frame);
    void RandomStrobeMedianBlend(cv::Mat &frame);
    void XorFade(cv::Mat &frame);
    void ColorIncreaseXorFade(cv::Mat &frame);
    void Pixels_InOut(cv::Mat &frame);
    void PixelDistortionCloud(cv::Mat &frame);
    void XorRow(cv::Mat &frame);
    void UseOldRow(cv::Mat &frame);
    void UseEveryOtherRow(cv::Mat &frame);
    void UseOffRow(cv::Mat &frame);
    void FrameJump(cv::Mat &frame);
    void MedianBlendMultiThreadVariable(cv::Mat &frame);
    void MedianBlendMultiThreadVariable32(cv::Mat &frame);
    void MedianBlendMultiThreadVariable16(cv::Mat &frame);
    void MedianBlendMultiThreadSkip8(cv::Mat &frame);
    void BlockyTrails(cv::Mat &frame);
    void BlockyTrails16(cv::Mat &frame);
    void BlockyTrails32(cv::Mat &frame);
    void BlockyTrails64(cv::Mat &frame);
    void UseOffRowVert(cv::Mat &frame);
    void UseOldRowVert(cv::Mat &frame);
    void UseEveryOtherRowVert(cv::Mat &frame);
    void UseOffRowDir(cv::Mat &frame);
    void UseOldRowDir(cv::Mat &frame);
    void UseEveryOtherRowDir(cv::Mat &frame);
    void SetCurrentFrameStateAsSource(cv::Mat &frame);
    void RegularTrails(cv::Mat &frame);
    void UseOldRow64(cv::Mat &frame);
    void UseOldRowVert64(cv::Mat &frame);
    void UseOldRowDir64(cv::Mat &fame);
    void GradientColorBlend(cv::Mat &frame);
    void MedianBlendMultiThreadGradientGray(cv::Mat &frame);
    void GradientRedBlend(cv::Mat &frame);
    void MedianBlendMultiThreadGradientRed(cv::Mat &frame);
    void GradientGreenBlend(cv::Mat &frame);
    void MedianBlendMultiThreadGradientGreen(cv::Mat &frame);
    void GradientBlueBlend(cv::Mat &frame);
    void MedianBlendMultiThreadGradientBlue(cv::Mat &frame);
    void GradientColorBlendAll(cv::Mat &frame);
    void MedianBlendMultiThreadGradientAll(cv::Mat &frame);
    void GradientXRed(cv::Mat &frame);
    void GradientXGreen(cv::Mat &frame);
    void GradientXBlue(cv::Mat &frame);
    void GradientAll(cv::Mat &frame);
    void MedianBlendMultiThreadAllGradients(cv::Mat &frame);
    void StretchRowMatrix16(cv::Mat &frame);
    void StretchRowMatrix32(cv::Mat &frame);
    void StretchRowMatrix8(cv::Mat &frame);
    void StretchColMatrix8(cv::Mat &frame);
    void StretchColMatrix16(cv::Mat &frame);
    void StretchColMatrix32(cv::Mat &frame);
    void GradientRandomSwitch(cv::Mat &frame);
    void GradientFlashColor(cv::Mat &frame);
    void GradientFlashComponent(cv::Mat &frame);
    void GradientRandomShift(cv::Mat &frame);
    void MedianBlendMultiThreadGradientShift(cv::Mat &frame);
    void MirrorIntertwineRows720(cv::Mat &frame);
    void MirrorIntertwineRowsY(cv::Mat &frame);
    void MirrorIntertwineRowsX_Y_Width_Height(cv::Mat &frame);
    void MirrorTrailsLeft(cv::Mat &frame);
    void MirrorTrailsLeftRightTopBottom(cv::Mat &frame);
    void MirrorTrailsRightLeftBottomTop(cv::Mat &frame);
    void MirrorTrailsFlash(cv::Mat &frame);
    void MirrorLeftTopToBottom(cv::Mat &frame);
    void MirrorRightBottomToTop(cv::Mat &frame);
    void MirrorFlipLeft(cv::Mat &frame);
    void MirrorFlipRight(cv::Mat &frame);
    void MirrorFlipBottomLeft(cv::Mat &frame);
    void MirrorFlipBottomRight(cv::Mat &frame);
    void MirrorFlipXMirrorLeft(cv::Mat &frame);
    void MirrorFlipXMirrorRight(cv::Mat &frame);
    void MirrorFlipYMirrorLeft(cv::Mat &frame);
    void MirrorFlipYMirrorRight(cv::Mat &frame);
    void MirrorFlipXLeftTopToBottom(cv::Mat &frame);
    void MirrorFlipXLeftBottomToTop(cv::Mat &frame);
    void MirrorFlipXRightTopToBottom(cv::Mat &frame);
    void MirrorFlipXRightBottomToTop(cv::Mat &frame);
    void MirrorFlipYLeftTopToBottom(cv::Mat &frame);
    void MirrorFlipYLeftBottomToTop(cv::Mat &frame);
    void MirrorFlipYRightTopToBottom(cv::Mat &frame);
    void MirrorFlipYRightBottomToTop(cv::Mat &frame);
    void HorizontalGlitch(cv::Mat &frame);
    void VerticalGlitch(cv::Mat &frame);
    void HorizontalXor(cv::Mat &frame);
    void VerticalXor(cv::Mat &frame);
    void Vertical_Horizontal_Glitch(cv::Mat &frame);
    void HorizontalSplitGlitch(cv::Mat &frame);
    void VerticalSplitGlitch(cv::Mat &frame);
    void HorizontalRandomLine(cv::Mat &frame);
    void VerticalRandomLine(cv::Mat &frame);
    void PixelInterlace(cv::Mat &frame);
    void PixelInterlaceColSkip(cv::Mat &frame);
    void PixelInterlaceRowSkip(cv::Mat &frame);
    void StartOffsetInterlace(cv::Mat &frame);
    void WaveTrails(cv::Mat &frame);
    void WaveTrailsAura(cv::Mat &frame);
    void ImageInterlaceWave(cv::Mat &frame);
    void ColorWaveTrails(cv::Mat &frame);
    void StrobePixelTrails(cv::Mat &frame);
    void RectangleTrails(cv::Mat &frame);
    void RectangleXY_Trails(cv::Mat &frame);
    void MedianBlendImage(cv::Mat &frame);
    void GhostWaveTrails(cv::Mat &frame);
    void VideoPixelFade(cv::Mat &frame);
    void ImagePixelFade(cv::Mat &frame);
    void CollectionPixelFade(cv::Mat &frame);
    void CollectionPixelFadeSubFilter(cv::Mat &frame);
    void DiamondWave(cv::Mat &frame);
    void RGBWave(cv::Mat &frame);
    void VideoCollectionOutline(cv::Mat &frame);
    void VideoSaturateAdd(cv::Mat &frame);
    void VideoSmoothMedianBlend(cv::Mat &frame);
    void Square_Blocks(cv::Mat &frame);
    void Square_Block_Resize(cv::Mat &frame);
    void Square_Block_Resize_Vertical(cv::Mat &frame);
    void Square_Block_Resize_Reset(cv::Mat &frame);
    void Square_Block_Resize_Vert_Reset(cv::Mat &frame);
    void Square_Block_Resize_Video(cv::Mat &frame);
    void Square_Block_Resize_Vert_Video(cv::Mat &frame);
    void Square_Block_Resize_Dir(cv::Mat &frame);
    void Square_Block_Resize_All(cv::Mat &frame);
    void VideoBlendGradient(cv::Mat &frame);
    void VideoMatrixBlend(cv::Mat &frame);
    void VideoMatrixBlendAlpha(cv::Mat &frame);
    void VideoMatrixBlendAlphaRandom(cv::Mat &frame);
    void VideoMatrixSwitch(cv::Mat &frame);
    void VideoCollectionWave(cv::Mat &frame);
    void CollectionWave(cv::Mat &frame);
    void TremorShake(cv::Mat &frame);
    void PixelateSquares(cv::Mat &frame);
    void ColorPixelDoubleXor(cv::Mat &frame);
    void VideoCollectionOffsetBlend(cv::Mat &frame);
    void VideoTransparent(cv::Mat &frame);
    void VideoStripes(cv::Mat &frame);
    void VideoSmoothGradient(cv::Mat &frame);
    void SmoothSelfBlend(cv::Mat &frame);
    void MedianBlendMultiThread_Half(cv::Mat &frame);
    void MedianBlendMultiThread_Variable(cv::Mat &frame);
    void FiftyPercentSubFilter(cv::Mat &frame);
    void VariablePercentSubFilter(cv::Mat &frame);
    void TwentyFivePercentSubFilter(cv::Mat &frame);
    void SeventyFivePercentSubFilter(cv::Mat &frame);
    void VideoRowGlitch(cv::Mat &frame);
    void VideoXor_Frame(cv::Mat &frame);
    void VideoSlideRGB(cv::Mat &frame);
    void VideoSlide(cv::Mat &frame);
    void VideoSlideOffset(cv::Mat &frame);
    void VideoSlideOffsetX(cv::Mat &frame);
    void VideoSlideOffsetXRev(cv::Mat &frame);
    void VideoSlideOffsetRGB(cv::Mat &frame);
    void VideoSlideOffsetRGBRev(cv::Mat &frame);
    void SelfSlideOffsetX(cv::Mat &frame);
    void SelfSlideOffsetRGB(cv::Mat &frame);
    void SelfSlideOffsetRGB_X(cv::Mat &frame);
    void MedianBlendMultiThreadStrobe(cv::Mat &frame);
    void VideoXor_XY(cv::Mat &frame);
    void VideoRandomWave(cv::Mat &frame);
    void RandomWave(cv::Mat &frame);
    void RandomWaveStartStop(cv::Mat &frame);
    void RandomMedianFrame(cv::Mat &frame);
    void VideoSmoothDoubleAlphaBlend(cv::Mat &frame);
    void VariableLines(cv::Mat &frame);
    void VariableLinesOffset(cv::Mat &frame);
    void VaraibleLinesRectangle(cv::Mat &frame);
    void VariableLinesRectangleSource(cv::Mat &frame);
    void VariableLinesStartRectangle(cv::Mat &frame);
    void VariableLinesStartRectangleBlend(cv::Mat &frame);
    void VariableLinesOnOffBlend(cv::Mat &frame);
    void VariableCollectionLinesOffBlend(cv::Mat &frame);
    void VariableDistortionWave(cv::Mat &frame);
    void VariableDistortionReset(cv::Mat &frame);
    void VariableLinesY(cv::Mat &frame);
    void VariableLinesY_Blend(cv::Mat &frame);
    void VariableLinesY_Wave(cv::Mat &frame);
    void VariableLinesXY_Interlaced(cv::Mat &frame);
    void VariableLinesExpand(cv::Mat &frame);
    void VariableLinesExpandBlend(cv::Mat &frame);
    void CollectionXor4(cv::Mat &frame);
    void PixelateExpandDistort(cv::Mat &frame);
    void PixelateExpandDistortX(cv::Mat &frame);
    void PixelateExpandDistortY(cv::Mat &frame);
    void PixelateExpandDistortExtra(cv::Mat &frame);
    void RectangleSpin(cv::Mat &frame);
    void RectanglePlotXY(cv::Mat &frame);
    void RectangleLines(cv::Mat &frame);
    void DifferenceFillLinesXor(cv::Mat &frame);
    void DistortPixelate(cv::Mat &frame);
    void DistortPixelate64_Slow(cv::Mat &frame);
    void DistortPixelate128_SubFilter(cv::Mat &frame);
    void DifferenceFillLines_SubFilter(cv::Mat &frame);
    void DifferenceFillLinesBlend_SubFilter(cv::Mat &frame);
    void DistortPixelate24_SubFilter(cv::Mat &frame);
    void MovementTrails_SubFilter(cv::Mat &frame);
    void MovementTrailsX_SubFilter(cv::Mat &frame);
    void FadeFromFrameToFrame(cv::Mat &frame);
    void GlitchFadeFromFrameToFrame(cv::Mat &frame);
    void RandomSolo(cv::Mat &frame);
    void PiecesOfFrames(cv::Mat &frame);
    void XorScaleValue(cv::Mat &frame);
    void DiamondCollection(cv::Mat &frame);
    void RandomFadeDelay(cv::Mat &frame);
    void ColorRowShift(cv::Mat &frame);
    void ColorRowShiftUp(cv::Mat &frame);
    void ColorRowShiftLeft(cv::Mat &frame);
    void ColorRowShiftRight(cv::Mat &frame);
    void MirrorFadeDelay(cv::Mat &frame);
    void MirrorRandomNow(cv::Mat &frame);
    void ScanlineBlack(cv::Mat &frame);
    void ScanlineSubFilter(cv::Mat &frame);
    void VerticalYSubFilter(cv::Mat &frame);
    void ScanlineLessSubFilter(cv::Mat &frame);
    void VerticalYLessSubFilter(cv::Mat &frame);
    void HorizontalColorOffset(cv::Mat &frame);
    void HorizontalColorOffsetLargeSizeSubFilter(cv::Mat &frame);
    void ErodeKernelSubFilter(cv::Mat &frame);
    void DilateKernelSubFilter(cv::Mat &frame);
    void ErodeKernelOffSubFilter(cv::Mat &frame);
    void DilateKernelOffSubFilter(cv::Mat &frame);
    void ColorKeySetOnSubFilter(cv::Mat &frame);
    void ColorKeySetOffSubFilter(cv::Mat &frame);
    void Warp(cv::Mat &frame);
    void WarpDir(cv::Mat &frame);
    void WarpTopLeft(cv::Mat &frame);
    void WarpRandom(cv::Mat &frame);
    void WarpStretch(cv::Mat &frame);
    void RandomLineGlitchSubFilter(cv::Mat &frame);
    void VerticalColorOffsetLargeSizeSubFilter(cv::Mat &frame);
    void PreviousFrameXor(cv::Mat &frame);
    void LightScanlineSubFilter(cv::Mat &frame);
    void ScaleUpDown(cv::Mat &frame);
    void VideoTransitionInOut_SubFilter(cv::Mat &frame);
    void VideoDisplayPercent_SubFilter(cv::Mat &frame);
    void MovementToBlackSubFilter(cv::Mat &frame);
    void Black(cv::Mat &frame);
    void VideoFrameRGB(cv::Mat &frame);
    void VideoAlphaBlendFade(cv::Mat &frame);
    void VideoAlphaAddFade(cv::Mat &frame);
    void VideoAlphaAddFadeSubFilter(cv::Mat &frame);
    void DelayOnOffSubFilter(cv::Mat &frame);
    void VideoImageBlendAlpha(cv::Mat &frame);
    void IntertwineCols640(cv::Mat &frame);
    void HorizontalOffsetLess_SubFilter(cv::Mat &frame);
    void VerticalOffsetLess_SubFilter(cv::Mat &frame);
    void SquareOffsetLess_SubFilter(cv::Mat &frame);
    void SquareOffset_SubFilter(cv::Mat &frame);
    void PrevFrameNotEqual(cv::Mat &frame);
    void MirrorDelayLeft(cv::Mat &frame);
    void MirrorDelayRight(cv::Mat &frame);
    void ApplyColorRange(cv::Mat &frame);
    void IntertwineAlphaBlend(cv::Mat &frame);
    void BlackLines(cv::Mat &frame);
    void MedianBlendMultiThread2(cv::Mat &frame);
    void MedianBlendMultiThread2_ColorChange(cv::Mat &frame);
    void ApplyColorRangeInverted(cv::Mat &frame);
    void VariablesExtraHorizontal(cv::Mat &frame);
    void ChannelSortDelay(cv::Mat &frame);
    void DizzyMode(cv::Mat &frame);
    void IntertwineColormap(cv::Mat &frame);
    void RotateFrame(cv::Mat &frame);
    void GhostShift(cv::Mat &frame);
    void RotateSet(cv::Mat &frame);
    void RotateFrameReverse(cv::Mat &frame);
    void RotateSetReverse(cv::Mat &frame);
    void AuraGhostRotate(cv::Mat &frame);
    void AuraGhostRotateReverse(cv::Mat &frame);
    void AuraGhostCollectionEnergy(cv::Mat &frame);
    void BlendSourceImageAndVideo(cv::Mat &frame);
    void SetImageAndVideoBlend(cv::Mat &frame);
    void ThresholdDark(cv::Mat &frame);
    void ThresholdMedianBlend(cv::Mat &frame);
    void Threshold(cv::Mat &frame);
    void RemoveLowRedLevel(cv::Mat &frame);
    void RemoveLowGreenLevel(cv::Mat &frame);
    void RemoveLowBlueLevel(cv::Mat &frame);
    void IncreaseLowRedLevel(cv::Mat &frame);
    void IncreaseLowGreenLevel(cv::Mat &frame);
    void IncreaseLowBlueLevel(cv::Mat &frame);
    void Zoom(cv::Mat &frame);
    void IntertwineVideo640(cv::Mat &frame);
    void IntertwineCols640x8(cv::Mat &frame);
    void ZoomCorner(cv::Mat &frame);
    void ZoomRandom(cv::Mat &frame);
    void MedianBlendByLowValue(cv::Mat &frame);
    void MedianBlendByIncreasingValue(cv::Mat &frame);
    void AuraZoomMedianBlend(cv::Mat &frame);
    void OffTrackX(cv::Mat &frame);
    void OffTrackY(cv::Mat &frame);
    void FrameAlphaInsert(cv::Mat &frame);
    void FrameAlphaInsert_SubFilter(cv::Mat &frame);
    void FrameAlphaBlend_SubFilter(cv::Mat &frame);
    void SlowTrails_SubFilter(cv::Mat &frame);
    void AddCollectionSubFilter(cv::Mat &frame);
    void AddCollectionXor_SubFilter(cv::Mat &frame);
    void ProperTrails(cv::Mat &frame);
    void ProperTrails_SubFilter(cv::Mat &frame);
    void StuckFrame_SubFilter(cv::Mat &frame);
    void XorLag(cv::Mat &frame);
    void PixelateBlend(cv::Mat &frame);
    void PixelateRect(cv::Mat &frame);
    void RGBSplitFilter(cv::Mat &frame);
    void DiagPixel(cv::Mat &frame);
    void DiagPixelY(cv::Mat &frame);
    void DiagPixelY2(cv::Mat &frame);
    void DiagPixelY3(cv::Mat &frame);
    void DiagPixelY4(cv::Mat &frame);
    void ExpandLeftRight(cv::Mat &frame);
    void DiagSquare(cv::Mat &frame);
    void DiagSquareLarge(cv::Mat &frame);
    void DiagonalSquareCollection(cv::Mat &frame);
    void DiagnoalSquareSize(cv::Mat &frame);
    void DiagonalSquareSizeOnOff(cv::Mat &frame);
    void DiagonalSquareSizeOnOffRandom(cv::Mat &frame);
    void SlitScanGUI(cv::Mat &frame);
    void SlitScanRandom(cv::Mat &frame);
    void VideoInterlacedRandom(cv::Mat &frame);
    void VideoSlitScan(cv::Mat &frame);
    void FloatFade(cv::Mat &frame);
    void SquareShift(cv::Mat &frame);
    void SquareShift8(cv::Mat &frame);
    void SquareShift16(cv::Mat &frame);
    void FloatFadeRandomIncrease(cv::Mat &frame);
    void FloatFadeRGB(cv::Mat &frame);
    void SquareShiftDir(cv::Mat &frame);
    void SquareShiftExpand(cv::Mat &frame);
    void FloatFadeVertical(cv::Mat &frame);
    void LineTrails(cv::Mat &frame);
    void SquareShiftDirVertical(cv::Mat &frame);
    void StretchLineRow(cv::Mat &frame);
    void StretchLineCol(cv::Mat &frame);
    void StretchLineRowInc(cv::Mat &frame);
    void StretchLineColInc(cv::Mat &frame);
    void StretchRowSplit(cv::Mat &frame);
    void VideoAddBlend(cv::Mat &frame);
    void VideoFadeInAndOut(cv::Mat &frame);
    void VideoFadeRGB(cv::Mat &frame);
    void VideoSubtract(cv::Mat &frame);
    void StretchLineRowLeftRight(cv::Mat &frame);
    void FadeInOutBlendRGB(cv::Mat &frame);
    void FadeInOutReverseRGB(cv::Mat &frame);
    void PixelateRandom(cv::Mat &frame);
    void PixelateFillRandom(cv::Mat &frame);
    void JaggedLine(cv::Mat &frame);
    void ScratchyTrails(cv::Mat &frame);
    void ExpandPixelate(cv::Mat &frame);
    void VideoRandom(cv::Mat &frame);
    void VideoCollectionTwitch(cv::Mat &frame);
    void DiagSquare8(cv::Mat &frame);
    void DiagSquareRandom(cv::Mat &frame);
    void SlitScanXGUI(cv::Mat &frame);
    void DiagSquareX(cv::Mat &frame);
    void SlitScanDir(cv::Mat &frame);
    void VideoScanline(cv::Mat &frame);
    void VideoSquareRandom(cv::Mat &frame);
    void VideoScanlineBlend(cv::Mat &frame);
    void VideoSplitColumn(cv::Mat &frame);
    void VideoXStatic(cv::Mat &frame);
    void ScanlineCollection(cv::Mat &frame);
    void SquareShiftDirRGB(cv::Mat &frame);
    void StretchLineRowIncRGB(cv::Mat &frame);
    void StretchLineColIncRGB(cv::Mat &frame);
    void StretchLineRowIncSource(cv::Mat &frame);
    void StretchLineColIncSource(cv::Mat &frame);
    void AlternateAlpha(cv::Mat &frame);
    void Square_Block_Resize_Vertical_RGB(cv::Mat &frame);
    void DiagSquareRGB(cv::Mat &frame);
    void ShiftPixelsRGB(cv::Mat &frame);
    void UseOldRowVertRGB(cv::Mat &frame);
    void ColorWaveTrailsRGB(cv::Mat &frame);
    void MirrorDiamondRGB(cv::Mat &frame);
    void Square_Block_Resize_RGB(cv::Mat &frame);
    void VariableLinesY_RGB(cv::Mat &frame);
    void MedianBlendMultiThread_RGB(cv::Mat &frame);
    void SlitScanXGUI_RGB(cv::Mat &frame);
    void SlitScanGUI_RGB(cv::Mat &frame);
    void SlitScanDir_RGB(cv::Mat &frame);
    void SquareShiftDirGradient(cv::Mat &frame);
    void MovementTrailsXRGB_SubFilter(cv::Mat &frame);
    void TwistedVision_RGB(cv::Mat &frame);
    void CollectionWave_RGB(cv::Mat &frame);
    void BlendWithSourcePercent(cv::Mat &frame);
    void SlitScan_Colors(cv::Mat &frame);
    void ReverseRandom(cv::Mat &frame);
    void ReverseRandomCollection(cv::Mat &frame);
    void RevesreRandomChannelCollection(cv::Mat &frame);
    void ReverseRandomCollectionResize(cv::Mat &frame);
    void SquareBlockGlitch(cv::Mat &frame);
    void SquareStretchRows(cv::Mat &frame);
    void SquareStretchRowsDelay(cv::Mat &frame);
    void SquareStretchEven(cv::Mat &frame);
    void SketchFilter(cv::Mat &frame);
    void SquareStretchEvenCollection(cv::Mat &frame);
    void SquareStretchEven32(cv::Mat &frame);
    void RGBLineFuzz(cv::Mat &frame);
    void RGBLineFuzzX(cv::Mat &frame);
    void ChannelSortCollection32(cv::Mat &frame);
    void LinesAcrossX(cv::Mat &frame);
    void XorLineX(cv::Mat &frame);
    void AlphaComponentIncrease(cv::Mat &frame);
    void ExpandContract(cv::Mat &frame);
    void MoveInThenMoveOut(cv::Mat &frame);
    void MoveInThenMoveOutFast(cv::Mat &frame);
    void CurrentDesktop(cv::Mat &frame);
    void DistortionFuzz(cv::Mat &frame);
    void DistortionByRow(cv::Mat &frame);
    void DistortionByRowRev(cv::Mat &frame);
    void DistortionByRowVar(cv::Mat &frame);
    void DistortionByRowRand(cv::Mat &frame);
    void DistortionByCol(cv::Mat &frame);
    void DistortionByColRand(cv::Mat &frame);
    void DistortionByColVar(cv::Mat &frame);
    void LongLines(cv::Mat &frame);
    void TearRight(cv::Mat &frame);
    void CurrentDesktopRect(cv::Mat &frame);
    void TearDown(cv::Mat &frame);
    void TearUp(cv::Mat &frame);
    void TearLeft(cv::Mat &frame);
    void SyphonInputVideo(cv::Mat &frame);
    void DistortStretch(cv::Mat &frame);
    void MultiVideoBlend(cv::Mat &frame);
    void MultiVideoAlphaBlend(cv::Mat &frame);
    void MultiVideoSmooth(cv::Mat &frame);
    void MultiVideoXor(cv::Mat &frame);
    void MultiVideoFilter_SubFilter(cv::Mat &frame);
    void FadeOnOff(cv::Mat &frame);
    void Stereo(cv::Mat &frame);
    void MultiVideoSaturateBlend(cv::Mat &frame);
    void ImageWithKeys(cv::Mat &frame);
    void MultiVideoColorKeyOn(cv::Mat &frame);
    void MultiVideoColorKeyOff(cv::Mat &frame);
    void MultiVideoMedianBlend(cv::Mat &frame);
    void MultiVideoMirror(cv::Mat &frame);
    void MultiVideoSubtract(cv::Mat &frame);
    void MultiVideoAlphaMedianBlend(cv::Mat &frame);
    void MultiVideoInterlace(cv::Mat &frame);
    void MultiVideoChangeLines(cv::Mat &frame);
    void MultiVideoDiagSquare(cv::Mat &frame);
    void ShiftLinesDown(cv::Mat &frame);
    void PictureStretch(cv::Mat &frame);
    void PictureStretchPieces(cv::Mat &frame);
    void VisualSnow(cv::Mat &frame);
    void VisualSnowX2(cv::Mat &frame);
    void LineGlitch(cv::Mat &frame);
    void SlitReverse64(cv::Mat &frame);
    void SlitReverse64_Increase(cv::Mat &frame);
    void SlitStretch(cv::Mat &frame);
    void LineLeftRight(cv::Mat &frame);
    void LineLeftRightResize(cv::Mat &frame);
    void SoloInOrder(cv::Mat &frame);
    void ImageInOrder(cv::Mat &frame);
    void SubInOrder(cv::Mat &frame);
    void RGBLineTrails(cv::Mat &frame);
    void RGBCollectionBlend(cv::Mat &frame);
    void RGBCollectionIncrease(cv::Mat &frame);
    void RGBCollectionEx(cv::Mat &frame);
    void RGBLongTrails(cv::Mat &frame);
    void VideoMatrixOutlineSubFilter(cv::Mat &frame);
    void FadeRGB_Speed(cv::Mat &frame);
    void RGBStrobeTrails(cv::Mat &frame);
    void FadeRGB_Variable(cv::Mat &frame);
    void BoxGlitch(cv::Mat &frame);
    void VerticalPictureDistort(cv::Mat &frame);
    void ShortTrail(cv::Mat &frame);
    void DiagInward(cv::Mat &frame);
    void DiagSquareInward(cv::Mat &frame);
    void DiagSquareInwardResize(cv::Mat &frame);
    void DiagSquareInwardResizeXY(cv::Mat &frame);
    void ParticleSlide(cv::Mat &frame);
    void DiagPixelated(cv::Mat &frame);
    void DiagPixelatedResize(cv::Mat &frame);
    void DiagPixelRGB_Collection(cv::Mat &frame);
    void RGBShiftTrails(cv::Mat &frame);
    void PictureShiftDown(cv::Mat &frame);
    void PictureShiftRight(cv::Mat &frame);
    void PictureShiftDownRight(cv::Mat &frame);
    void FlipPictureShift(cv::Mat &frame);
    void FlipPictureRandomMirror(cv::Mat &frame);
    void PictureShiftVariable(cv::Mat &frame);
    void RGBWideTrails(cv::Mat &frame);
    void StretchR_Right(cv::Mat &frame);
    void StretchG_Right(cv::Mat &frame);
    void StretchB_Right(cv::Mat &frame);
    void StretchR_Down(cv::Mat &frame);
    void StretchG_Down(cv::Mat &frame);
    void StretchB_Down(cv::Mat &frame);
    void Distorted_LinesY(cv::Mat &frame);
    void Distorted_LinesX(cv::Mat &frame);
    void TripHSV(cv::Mat &frame);
    void Diag_Line_InOut(cv::Mat &frame);
    void Histogram(cv::Mat &frame);
    void ImageHistogramLookup(cv::Mat &frame);
    void MovementRange_SubFilter(cv::Mat &frame);
    void XorSumStrobe(cv::Mat &frame);
    void DetectEdges(cv::Mat &frame);
    void SobelNorm(cv::Mat &frame);
    void SobelThreshold(cv::Mat &frame);
    void EdgeFill_SubFilter(cv::Mat &frame);
    void FillWhite_SubFilter(cv::Mat &frame);
    void FillBlack_SubFilter(cv::Mat &frame);
    void FillPink_SubFilter(cv::Mat &frame);
    void FillGreen_SubFilter(cv::Mat &frame);
    void MedianBlurHigherLevel(cv::Mat &frame);
    void FillNonWhite_SubFilter(cv::Mat &frame);
    void FillNonBlack_SubFilter(cv::Mat &frame);
    void FillNonPink_SubFilter(cv::Mat &frame);
    void LineInLineOut(cv::Mat &frame);
    void LineInLineOut_Increase(cv::Mat &frame);
    void LineInLineOut2_Increase(cv::Mat &frame);
    void LineInLineOut3_Increase(cv::Mat &frame);
    void LineInLineOut4_Increase(cv::Mat &frame);
    void LineInLineOut_ReverseIncrease(cv::Mat &frame);
    void LineInLineOut_InvertedY(cv::Mat &frame);
    void LineInLineOut_ReverseInvertedY(cv::Mat &frame);
    void LineInLineOut_Vertical(cv::Mat &frame);
    void LineInLineOut_VerticalIncrease(cv::Mat &frame);
    void LineInLineOut_IncreaseImage(cv::Mat &frame);
    void LineInLineOut_IncreaseVideo(cv::Mat &frame);
    void LineInLineOut_IncreaseVideo2(cv::Mat &frame);
    void LineInLineOut_IncreaseVideo3(cv::Mat &frame);
    void LineInLineOut_IncreaseImage2(cv::Mat &frame);
    void LineInLineOut_IncreaseImage3(cv::Mat &frame);
    void SquareByRow(cv::Mat &frame);
    void SquareByRowRev(cv::Mat &frame);
    void SquareByRow2(cv::Mat &frame);
    void DivideByValue(cv::Mat &frame);
    void FrameSep(cv::Mat &frame);
    void FrameSep2(cv::Mat &frame);
    void DefStutter(cv::Mat &frame);
    void StutterFilter(cv::Mat &frame);
    void FrameOffsetRSize(cv::Mat &frame);
    void FrameOffsetRSize2(cv::Mat &frame);
    void FrameOffsetRSize3(cv::Mat &frame);
    void FrameOffsetRSize4(cv::Mat &frame);
    void SqOnOff(cv::Mat &frame);
    void SqOnOffAlpha(cv::Mat &frame);
    void SqOnOffAlphaClip(cv::Mat &frame);
    void SqOnOffAlphaClipFast(cv::Mat &frame);
    void SqOnOffAlphaPixel(cv::Mat &frame);
    void SqOnOffDiff(cv::Mat &frame);
    void SqOnOffSize(cv::Mat &frame);
    void Placement(cv::Mat &frame);
    // #NoFilter
    void NoFilter(cv::Mat &frame);
    void Empty(cv::Mat &frame);
    // Alpha blend with original image
    void BlendWithSource(cv::Mat &frame);
    // plugin must be implemented in project
    void plugin(cv::Mat &frame);
    // set Custom Filter callback function
    void setNewSyphonImage(cv::Mat &frame);
    void setCustom(DrawFunction f);
    void setPlugin(DrawFunction f);
    void setProcMode(PROC_MODE_TYPE value);
    PROC_MODE_TYPE getProcMode();
    void setSubFilter(int value);
    void clearSubFilter();
    // color maps
    void Negate(cv::Mat &frame);
    void ApplyColorMap(cv::Mat &frame);
    void AddInvert(cv::Mat &frame);
    void setPixelCollection(int value);
    int  getPixelCollection();
    bool VideoFrame(cv::Mat &frame);
    void SetDesktopRect(int x, int y, int w, int h);
    bool setVideoFiles(std::vector<std::string> &v);
    // color correction
    void MedianBlur(cv::Mat &frame, unsigned int value);
    void setBlendPercentage(const double &value);
    void setBrightness(cv::Mat &frame, double alpha, int beta);
    void setGamma(cv::Mat &frame, cv::Mat &outframe, double gamma);
    void setSaturation(cv::Mat &frame, int saturation);
    void AlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    void RealAlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    void AlphaBlendDouble(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha1, double alpha2);
    void AlphaXorBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    void AlphaXorBlendDouble(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha1, double alpha2);
    void StretchAlphaBlendSelf(cv::Mat &frame, const int speed_x, const int speed_y, int &offset_x, int &offset_y);
    void StretchAlphaBlendSelf(cv::Mat &frame, int &dir, const int &speed_x, const int &speed_y, int &offset_x, int &offset_y, const int &size_x, const int &size_y);
    void AlphaMovement(double *alpha, int *dir, double inc);
    void AlphaMovementMaxMin(double &alpha, int &dir, double speed, double max, double min);
    void AlphaMovementMaxMin(std::atomic<double> &alpha, int &dir, double speed, double max, double min);
    void AlphaBlendArray(cv::Mat &src, cv::Mat *frame, int num_frames);
    void PixelScaleAlpha(cv::Mat &frame, double amt);
    void DarkenImage(cv::Mat &frame, unsigned int size);
    void Pixelate(cv::Mat &frame, unsigned int size);
    void Add(cv::Mat &src, cv::Mat &add, bool sat = false);
    void Sub(cv::Mat &src, cv::Mat &sub, bool sat = false);
    void Xor(cv::Mat &dst, const cv::Mat &add);
    void Xor(const cv::Mat &input, const cv::Mat &add, cv::Mat &out);
    void ScalarAverage(const cv::Mat &frame, cv::Scalar &s);
    void TotalAverageOffset(cv::Mat &frame, unsigned long &value);
    void swapColors(cv::Mat &frame, int x, int y);
    void swapColors_(cv::Mat &frame, int x, int y);
    void procPos(int &direction, double &pos, double &pos_max, const double max_size = 15, double iter = 0.05);
    void setColorKeyRange(cv::Vec3b low, cv::Vec3b high);
    void setBlockedColorKeys(std::vector<Keys> &blocked);
    bool colorBounds(const cv::Vec3b &color, const cv::Vec3b &pixel, const cv::Vec3b &range_low, const cv::Vec3b &range_high);
    SearchType searchColors(const cv::Vec3b &color);
    bool compareColor(const cv::Vec3b &color, const cv::Vec3b &low, const cv::Vec3b &high);
    void setGrayColor(const cv::Vec3b &color);
    void setChannelToValue(cv::Mat &frame, unsigned int channel, unsigned char value);
    unsigned char size_cast(long val);
    unsigned char size_reset(long val);
    void setThreadCount(const int &threads);
    int getThreadCount();
    void ac_resize(const cv::Mat &src, cv::Mat &dst, cv::Size scale);
    void ac_resize(const cv::UMat &src, cv::Mat &dst, cv::Size scale);
    // Alpha Blend two filters and set to frame by alpha variable
    void filterFade(cv::Mat &frame, int filter1, int filter2, double alpha, int alpha_mode = 0);
    void filterColorKeyed(const cv::Vec3b &color, const cv::Mat &orig, const cv::Mat &filtered, cv::Mat &output);
    void resetAlpha(int &dir, std::atomic<double> &alpha);
    void resetAlpha(std::atomic<double> &alpha);
    void resetAlpha(int &dir, double &alpha);
    void resetAlpha(double &alpha);
    void SwapColors(cv::Vec3b &v);
    void FillRow(cv::Mat &frame, unsigned int row, unsigned char value);
    void Shuffle(int &index, cv::Mat &frame, std::vector<std::string> &filter_array);
    void AddMatVector(cv::Mat &frame, std::vector<cv::Mat> &v);
    void setColorMap(int map, cv::Mat &frame);
    void blendFilterWithColorMap(int filter, int map, cv::Mat &frame);
    void SwitchOrder(cv::Vec3b &frame, int order);
    void InterlaceFrames(cv::Mat &frame, const cv::Mat &copy1);
    void InterlaceFrames(cv::Mat &frame, cv::Mat *items, const int num_items);
    void InitArrayPosition(int *arr, const int &dir);
    void pushSubFilter(int newsub);
    void popSubFilter();
    void setSourceFrame(const cv::Mat &frame);
    void IntertwineFrames(IntertwineDir type, const cv::Mat &one, const cv::Mat &two, cv::Mat &dest);
    void VariableScale(double *alpha,int *dir, double *start, double *start_init, double *start_max, double *stop, double *stop_init, double *stop_max, double inc);
    void VariableScaleSpeed(double *alpha,int *dir, double *start, double *start_init, double *start_max, double *stop, double *stop_init, double *stop_max, double *inc);
    bool CopyAudioStream(std::string ffmpeg, std::string file1, std::string file2, std::string output);
    bool FFMPEG_Installed(const std::string &ffmpeg);
    void setMaxAllocated(const int &v);
    void setAllocatedFrames(const int &v);
    int getMaxAllocated();
    int getCurrentAllocatedFrames();
    void swapColorState(const bool &b);
    bool getColorState();
    void setFilterColorMap(int color);
    int  getColorMapFilter();
    void User(cv::Mat &frame);
    void setColorRangeLowToHigh(cv::Vec3b low, cv::Vec3b high);
    void applyColorRange(cv::Mat &frame);
    void setColorRangeEnabled(bool e);
    bool getColorRangeEnabled();
    void setVariableWait(int wait);
    int getVariableWait();
    void rotate_image(cv::Mat &src, cv::Mat &dst, double angle);
    void setColorLevel(int color);
    int getColorLevel();
    void slitScanSet(int num, int width, int height, int repeat, int delay, int on);
    typedef void (*Log)(std::string);
    extern Log log_func;
    void setLogCallback(Log log);
    void log_print(std::string s);
    void setMaxStretch(int max);
    int  getMaxStretch();
    void setBlendPercent(int percent);
    void setThreshold(int t);
    // todo: later
    // void ResizeFrame(const cv::Mat &image, cv::Mat &frame, const cv::Size &s);
    // draw functions / strings
    //extern std::string *draw_strings;
    extern std::vector<std::string> draw_strings;
    
    extern DrawFunction plugin_func;
    extern DrawFunction *draw_func;
    extern FilterType *filter_array;
    extern std::atomic<int> draw_max;
    extern std::atomic<bool> snapShot;
    extern std::atomic<bool> reset_filter;
    extern std::atomic<double> alpha_increase;
    extern std::atomic<bool> swapColorOn;
    extern std::unordered_map<std::string, int> filter_map;
    extern std::unordered_map<std::string, FilterType> filter_map_str;
    extern bool frames_released;
    extern void release_all_objects();
    // Matrix Collection template
    class Frames {
    public:
        cv::Mat **frames;
        int Size;
        Frames(int size) : frames(0), Size(size) {
            
        }
        
        ~Frames() {
            if(frames != 0) {
                for(int i = 0; i < Size; ++i) {
                    delete frames[i];
                }
                delete [] frames;
                frames = 0;
            }
        }
        
        void releaseFrames() {
            if(frames != 0) {
                for(int i = 0; i < Size; ++i) {
                    if(frames[i] != 0) frames[i]->release();
                }
            }
        }
        
        void initFrames() {
            if(frames == 0) {
                frames = new cv::Mat*[Size];
                for(int i = 0; i < Size; ++i) {
                    frames[i] = new cv::Mat();
                }
            }
        }
        
        void shiftFrames(cv::Mat &frame) {
            for(int i = Size-1; i > 0; --i) {
                frames[i] = frames[i-1];
            }
            *frames[0] = frame.clone();
        }
        
        unsigned long getSize() {
            
            if(frames == 0) return 0;
            
            unsigned long total = 0;
            for(int i = 0; i < Size; ++i) {
                
                if(frames[i] != 0 && !frames[i]->empty())
                    total += frames[i]->rows * frames[i]->cols;
            }
            return total;
        }
        
        void ResizeFrames(int ns) {
            if(frames != 0) {
                for(int i = 0; i < Size; ++i) {
                    delete frames[i];
                }
                delete [] frames;
                frames = 0;
            }
            frames = new cv::Mat*[ns];
            for(int i = 0; i < ns; ++i) {
                frames[i] = new cv::Mat();
            }
            Size = ns;
        }
        
        void setMat(int index, cv::Mat &frame) {
            *frames[index] = frame;
        }
        
        cv::Mat &operator[](int pos) {
            if(frames == 0) {
                initFrames();
            }
            if(pos >= 0 && pos < Size)
                return *frames[pos];
            else
                return *frames[0];
        }
    };
    
    extern std::vector<Frames *> all_objects;
    extern unsigned long all_allocation;
    template<int Size>
    class MatrixCollection {
    public:
        static constexpr int ArraySize = Size;
        MatrixCollection() : frames(Size), w(0), h(0) {
            all_objects.push_back(&frames);
            completedRows = 0;
        }
        Frames frames;
        int w, h;
        void shiftFrames(cv::Mat &frame) {
            frames.initFrames();
            if(resetFrame(frame)) {
                shiftEachFrame(frame);
                if(completedRows < size()) ++completedRows;
            }
        }
        
        void shiftEachFrame(cv::Mat &frame) {
            for(int i = Size-1; i > 0; --i) {
                frames[i] = frames[i-1];
            }
            frames[0] = frame.clone();
        }
        
        bool resetFrame(cv::Mat &frame) {
            int wx = frame.cols;
            int wh = frame.rows;
            if(getCurrentAllocatedFrames() > getMaxAllocated()) {
                release_frames = true;
            }
            // check if any frames were released.
            bool check_released = false;
            for(int i = 0; i < Size; ++i) {
                if(frames[i].empty()) {
                    check_released = true;
                    setAllocatedFrames(getCurrentAllocatedFrames()+Size);
                    //allocated_frames += Size;
                    frames[i] = frame.clone();
                    break;
                }
            }
            if(check_released == true || (w != wx || h != wh) || reset_filter == true || frames_released == true) {
                col_lock.lock();
                for(int i = 0; i < Size; ++i)
                    frames[i] = frame.clone();
                w = wx;
                h = wh;
                reset_filter = false;
                completedRows = 0;
                col_lock.unlock();
                return false;
            }
            return true;
        }
        
        bool empty() {
            for(int i = 0; i < size(); ++i) {
                if(!frames[i].empty())
                    return false;
            }
            return true;
        }
        
        bool valid() const {
            return (w != 0 && h != 0);
        }
        int width() const {
            return w;
        }
        int height() const { return h; }
        cv::Size bounds() const {
            return cv::Size(w, h);
        }
        int size() const {
            return ArraySize;
            
        }
        int completed() {
            return completedRows;
            
        }
        int completedRows;
    };
    
    class DynamicMatrixCollection {
    public:
        int ArraySize;
        
        DynamicMatrixCollection(int s) : ArraySize(s), frames(s), w(0), h(0) {
            // do not count as part of stored frames (commented out below)
            //all_objects.push_back(&frames);
            completedRows = 0;
        }
        Frames frames;
        int w, h;
        void shiftFrames(cv::Mat &frame) {
            frames.initFrames();
            if(resetFrame(frame)) {
                shiftEachFrame(frame);
                if(completedRows < size()) ++completedRows;
            }
        }
        
        bool set_reset;
        
        void resize(int sz) {
            frames.ResizeFrames(sz);
            ArraySize = sz;
        }
        
        void shiftEachFrame(cv::Mat &frame) {
            for(int i = ArraySize-1; i > 0; --i) {
                frames[i] = frames[i-1];
            }
            frames[0] = frame.clone();
        }
        
        bool resetFrame(cv::Mat &frame) {
            int wx = frame.cols;
            int wh = frame.rows;
            if(getCurrentAllocatedFrames() > getMaxAllocated()) {
                release_frames = true;
            }
            // check if any frames were released.
            bool check_released = false;
            for(int i = 0; i < ArraySize; ++i) {
                if(frames[i].empty()) {
                    check_released = true;
                    //allocaed frames not added to count
                    //setAllocatedFrames(getCurrentAllocatedFrames()+ArraySize);
                    //allocated_frames += Size;
                    frames[i] = frame.clone();
                    break;
                }
            }
            if(check_released == true || (w != wx || h != wh) || reset_filter == true || frames_released == true) {
                col_lock.lock();
                for(int i = 0; i < ArraySize; ++i)
                    frames[i] = frame.clone();
                w = wx;
                h = wh;
                reset_filter = false;
                completedRows = 0;
                col_lock.unlock();
                return false;
            }
            return true;
        }
        
        bool empty() {
            for(int i = 0; i < size(); ++i) {
                if(!frames[i].empty())
                    return false;
            }
            return true;
        }
        
        void releaseFrames() {
            frames.releaseFrames();
        }
        
        bool valid() const {
            return (w != 0 && h != 0);
        }
        int width() const {
            return w;
        }
        int height() const { return h; }
        cv::Size bounds() const {
            return cv::Size(w, h);
        }
        int size() const {
            return ArraySize;
            
        }
        int completed() {
            return completedRows;
            
        }
        int completedRows;
    };
    
    unsigned long calculateMemory();
    extern bool testSize(cv::Mat &frame);
    // pass function pointer, functor or lambda with prototype
    // void filter(cv::Mat *frame, int offset, int cols, int size)
    template<typename F>
    void UseMultipleThreads(cv::Mat &frame, int cores, F func) {
        int size = frame.rows/cores;
        std::vector<std::thread> values;
        for(int i = 0; i < cores; ++i) {
            values.push_back(std::thread(func, &frame, i*size, frame.cols, size));
        }
        col_lock.lock();
        for(unsigned int i = 0; i < values.size(); ++i) {
            values[i].join();
        }
        col_lock.unlock();
    }
    //  MedianBlend
    template<int Size>
    void MedianBlendMultiThread(cv::Mat &frame, MatrixCollection<Size> *collection, int div_value) {
        
        if(div_value <= 0)
            return;
        int r = 3+rand()%7;
        for(int i = 0; i < r; ++i)
            MedianBlur(frame);
        collection->shiftFrames(frame);
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Scalar value;
                    for(int j = 0; j < collection->size(); ++j) {
                        cv::Vec3b pixel = collection->frames[j].template at<cv::Vec3b>(z, i);
                        for(int q = 0; q < 3; ++q) {
                            value[q] += pixel[q];
                        }
                    }
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int val = 1+static_cast<int>(value[j]/div_value);
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
        AddInvert(frame);
    }
    
    template<int Size>
    void MedianBlendMultiThreadNoShift(cv::Mat &frame, MatrixCollection<Size> *collection, int div_value) {
        if(div_value <= 0)
            return;
        int r = 3+rand()%7;
        for(int i = 0; i < r; ++i)
            MedianBlur(frame);
        
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Scalar value;
                    for(int j = 0; j < collection->size(); ++j) {
                        cv::Vec3b pixel = collection->frames[j].template at<cv::Vec3b>(z, i);
                        for(int q = 0; q < 3; ++q) {
                            value[q] += pixel[q];
                        }
                    }
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int val = 1+static_cast<int>(value[j]/div_value);
                        pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
        AddInvert(frame);
    }
    
    // Trails function
    template<int Size>
    void Smooth(cv::Mat &frame, MatrixCollection<Size> *collection, bool addframe = true) {
        
        if(collection->empty())
            collection->shiftFrames(frame);
        
        if(addframe == true)
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
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    test[j] /= (collection->size()-1);
                    pixel[j] = cv::saturate_cast<unsigned char>(test[j]);
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
            }
        }
    }
    
    template<int Size>
    void DarkSmooth(cv::Mat &frame, MatrixCollection<Size> *collection, int dark) {
        collection->shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar test;
                for(int q = 1; q < collection->size(); ++q) {
                    cv::Mat &framev = collection->frames[q];
                    cv::Vec3b pix = framev.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        test[j] += pix[j];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    test[j] /= (collection->size());
                    test[j] /= dark;
                    pixel[j] /= dark;
                    pixel[j] = pixel[j]^static_cast<unsigned char>(test[j]);
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
            }
        }
    }
    // Trails function
    template<int Size>
    void StaticXor(cv::Mat &frame, MatrixCollection<Size> *collection, cv::Vec3b r) {
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
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    test[j] /= (collection->size());
                    pixel[j] = static_cast<unsigned char>(test[j])^r[j];
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
            }
        }
    }
    // Trails function
    template<int Size>
    void SmoothRGB(cv::Mat &frame, MatrixCollection<Size> *collection) {
        collection->shiftFrames(frame);
        static int index = 0;
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
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                test[index] /= (collection->size()-1);
                pixel[index] = cv::saturate_cast<unsigned char>(test[index]);
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
            }
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    
    template<int Size, typename Func>
    void ImageDifference(cv::Mat &frame, MatrixCollection<Size> *collection, Func func_call, int range = 30) {
        collection->shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar values;
                for(int q = 0; q < collection->size(); ++q) {
                    cv::Mat &pix_val = collection->frames[q];
                    cv::Vec3b pix = pix_val.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        values[j] += pix[j];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                bool found = false;
                for(int j = 0; j < 3; ++j) {
                    values[j] /= collection->size();
                    unsigned char val = static_cast<unsigned char>(values[j]);
                    if(pixel[j] > val+range || pixel[j] < val-range) {
                        found = true;
                        break;
                    }
                }
                if(found == true) {
                    func_call(pixel);
                }
            }
        }
    }
    template<int Size>
    void ImageCopyDifference(cv::Mat &frame,cv::Mat &filtered, MatrixCollection<Size> *collection, int range = 5) {
        if(frame.size() != filtered.size())
            return;
        collection->shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar values;
                for(int q = 0; q < collection->size(); ++q) {
                    cv::Mat &pix_val = collection->frames[q];
                    cv::Vec3b pix = pix_val.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        values[j] += pix[j];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b copypix = filtered.at<cv::Vec3b>(z, i);
                bool found = false;
                for(int j = 0; j < 3; ++j) {
                    values[j] /= collection->size();
                    unsigned char val = static_cast<unsigned char>(values[j]);
                    if(pixel[j] > val+range || pixel[j] < val-range) {
                        found = true;
                        break;
                    }
                }
                if(found == true) {
                    pixel = copypix;
                }
            }
        }
    }
    
    template<int Size>
    void MatrixBlend(cv::Mat &frame, MatrixCollection<Size> *collection, int div_val = 1) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection->size(); ++j) {
                    cv::Mat &frame1 = collection->frames[j];
                    cv::Vec3b pixel = frame1.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        value[q] += pixel[q];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    int val = 1+static_cast<int>(value[j]/div_val);
                    pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                }
            }
        }
    }
    
    template<int Size>
    void MatrixBlendDouble(cv::Mat &frame, MatrixCollection<Size> *collection1, MatrixCollection<Size> *collection2, int div_val = 1, double per1 = 0.4, double per2 = 0.2) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar value1, value2;
                for(int j = 0; j < collection1->size(); ++j) {
                    cv::Mat &frame1 = collection1->frames[j];
                    cv::Mat &frame2 = collection2->frames[j];
                    cv::Vec3b pixel1 = frame1.at<cv::Vec3b>(z, i);
                    cv::Vec3b pixel2 = frame2.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        value1[q] += pixel1[q];
                        value2[q] += pixel2[q];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    int val1 = 1+static_cast<int>(value1[j]/div_val);
                    int val2 = 1+static_cast<int>(value2[j]/div_val);
                    unsigned char op1 = static_cast<unsigned char>(0.2 * pixel[j]);
                    unsigned char op2 = static_cast<unsigned char>(val1 * per1);
                    unsigned char op3 = static_cast<unsigned char>(val2 * per2);
                    pixel[j] = (op1 ^ op2 ^ op3);
                }
            }
        }
    }
    
    template<int row_size>
    void IntertwineRows(cv::Mat &frame, MatrixCollection<row_size> *collection, const int height, bool hideRows = false) {
        collection->shiftFrames(frame);
        int index = 0;
        int pos = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Mat &current = collection->frames[index];
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(hideRows == false) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                } else if(z < collection->completedRows) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                }
            }
            ++pos;
            if(pos > height-1) {
                pos = 0;
                ++index;
                if(index > row_size-1)
                    index = 0;
            }
        }
    }
    
    template<int row_size>
    void IntertwineRows(cv::Mat &frame, MatrixCollection<row_size> *collection) {
        IntertwineRows(frame, collection, row_size);
    }
    
    template<int row_size>
    void IntertwineRowsInter(cv::Mat &frame, MatrixCollection<row_size> *collection, const int height, bool hideRows = false) {
        collection->shiftFrames(frame);
        int index = 0;
        int pos = 0;
        int on_off = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Mat &current = (on_off == 0) ? collection->frames[index] : frame;
            on_off = (on_off == 0) ? 1 : 0;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(hideRows == false) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                } else if(z < collection->completedRows) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                }
            }
            ++pos;
            if(pos > height-1) {
                pos = 0;
                ++index;
                if(index > row_size-1)
                    index = 0;
            }
        }
    }
    
    template<int row_size>
    void IntertwineRowsReverse(cv::Mat &frame, MatrixCollection<row_size> *collection, const int height, bool hideRows = false) {
        collection->shiftFrames(frame);
        int index = 0;
        int pos = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Mat &current = collection->frames[row_size-index-1];
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(hideRows == false) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                } else if(z < collection->completedRows) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = value[j];
                    }
                }
            }
            ++pos;
            if(pos > height-1) {
                pos = 0;
                ++index;
                if(index > row_size-1)
                    index = 0;
            }
        }
    }
    
    template<int row_size>
    void IntertwineRowsReverse(cv::Mat &frame, MatrixCollection<row_size> *collection) {
        IntertwineRowsReverse(frame, collection, row_size);
    }
    
    
    template<int row_size, typename F>
    void IntertwineRowsOperation(cv::Mat &frame, MatrixCollection<row_size> *collection, F func) {
        collection->shiftFrames(frame);
        int index = 0;
        int pos = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Mat &current = collection->frames[index];
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(z < collection->completedRows) {
                    cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                    func(pixel, value);
                }
            }
            ++pos;
            if(pos > row_size-1) {
                pos = 0;
                ++index;
                if(index > row_size-1)
                    index = 0;
            }
        }
    }
    
    template<int col_size>
    void IntertwineCols(cv::Mat &frame, MatrixCollection<col_size> *collection, int width, bool add = true) {
        if(add == true) collection->shiftFrames(frame);
        int index = 0;
        int pos = 0;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Mat &current = collection->frames[index];
            for(int z = 0; z < frame.rows; ++z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b value = current.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = value[j];
                }
            }
            ++pos;
            if(pos > width-1) {
                pos = 0;
                ++index;
                if(index > col_size-1)
                    index = 0;
            }
        }
    }
    
    template<int col_size>
    void IntertwineCols(cv::Mat &frame, MatrixCollection<col_size> *collection) {
        IntertwineCols(frame, collection, col_size);
    }
    
    template<int row_size>
    void IntertwineDoubleRows(cv::Mat &frame, MatrixCollection<row_size> *collection1, MatrixCollection<row_size> *collection2, int height) {
        int index_col = 0, index = 0;
        int pos = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Mat &copy1 = collection1->frames[index];
            cv::Mat &copy2 = collection2->frames[index];
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix_val[2];
                pix_val[0] = copy1.at<cv::Vec3b>(z, i);
                pix_val[1] = copy2.at<cv::Vec3b>(z, i);
                pixel = pix_val[index_col];
            }
            index_col = (index_col == 0) ? 1 : 0;
            ++pos;
            if(pos > height-1) {
                pos = 0;
                ++index;
                if(index > row_size-1) {
                    index = 0;
                }
            }
        }
    }
    
    template<int row_size>
    void IntertwineResizeRowX(cv::Mat &frame, MatrixCollection<row_size> *collection, int width, int height, int &dir, int &size_value) {
        cv::Mat copy1;
        ac_resize(frame, copy1, cv::Size(width,height));
        collection->shiftFrames(copy1);
        int index = 1;
        for(int z = 0; z < copy1.rows; ++z) {
            for(int i = 0; i < copy1.cols; ++i) {
                cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection->frames[index].template at<cv::Vec3b>(z, i);
                pixel = pix;
            }
            ++index;
            if(index > size_value) {
                index = 0;
            }
        }
        if(dir == 1) {
            ++size_value;
            if(size_value > collection->size()-1) {
                size_value = collection->size()-1;
                dir = 0;
            }
        } else if(dir == 0) {
            --size_value;
            if(size_value <= 1) {
                dir = 1 ;
            }
        }
        ac_resize(copy1, frame, frame.size());
    }
    
    template<int r_size>
    void MatrixVariable(cv::Mat &frame, MatrixCollection<r_size> *collection, int &depth, int &dir) {
        static constexpr int v_size = r_size;
        collection->shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Scalar values;
                for(int q = 0; q < depth; ++q) {
                    cv::Mat &frame = collection->frames[q];
                    cv::Vec3b pixel = pixelAt(frame,z, i);
                    for(int j = 0; j < 3; ++j) {
                        values[j] += pixel[j];
                    }
                }
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    int val = static_cast<int>(values[j]);
                    pixel[j] = pixel[j]^val;
                }
            }
        }
        if(dir == 1) {
            ++depth;
            if(depth > v_size-1)
                dir = 0;
        } else {
            --depth;
            if(depth <= 2)
                dir = 1;
        }
    }
    
    template<int SIZE>
    void MedianBlendMultiThreadVariable(cv::Mat &frame, MatrixCollection<SIZE> *collection) {
        collection->shiftFrames(frame);
        static int position = 2;
        for(int z = 0; z <  frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Scalar value;
                    for(int j = 0; j < 3; ++j) {
                        if(j >= 0 && j < 3 && i >= 0 && i < collection->frames[j].cols && z >= 0 && z < collection->frames[j].rows) {
                            cv::Vec3b pixel = collection->frames[j].template at<cv::Vec3b>(z, i);
                            for(int q = 0; q < 3; ++q) {
                                value[q] += pixel[q];
                            }
                        }
                    }
                    if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                        for(int j = 0; j < 3; ++j) {
                            int val = 1+static_cast<int>(value[j]);
                            pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                        }
                    }
                }
            }
        }
        static int dir = 1;
        if(dir == 1) {
            ++position;
            if(position > (collection->size()-1)) {
                position = collection->size()-1;
                dir = 0;
            }
        } else {
            --position;
            if(position <= 2) {
                position = 2;
                dir = 1;
            }
        }
    }
    
    template<int size_val>
    void ShadowTrails(cv::Mat &frame, MatrixCollection<size_val> *collection, int &index) {
        collection->shiftFrames(frame);
        cv::Mat &copy1 = collection->frames[index];
        cv::Mat copy2  = frame.clone();
        AlphaBlend(copy2, copy1, frame, 0.5);
        static int dir = 0;
        if(dir == 1) {
            ++index;
            if(index >= size_val-1) {
                dir = 0;
                index = size_val-1;
            }
        } else {
            --index;
            if(index <= 1) {
                dir = 1;
                index = 1;
            }
        }
    }
    
    // bound long values to size of a byte
    template<typename T>
    T size_type_cast(const long &val) {
        if(val >= 255) return 255;
        if(val <= 0) return 0;
        return static_cast<T>(val);
    }
    // point class
    class Point {
    public:
        Point();
        Point(const Point &p);
        Point(int xx, int yy);
        int x, y;
        void setPoint(int xx, int yy);
        Point &operator=(const Point &p);
    };
    // Rectangle class
    class Rect {
    public:
        Rect();
        Rect(const Rect &r);
        Rect(int xx, int yy, int ww, int hh);
        Rect(int xx, int yy);
        Rect(int xx, int yy, cv::Size s);
        Rect(Point pt, int ww, int hh);
        Rect(Point pt, cv::Size s);
        void setRect(int xx, int yy, int ww, int hh);
        Rect &operator=(const Rect &r);
        int x,y,w,h;
    };
    // classes to be used by the filter
    // Square class to hold broken up cv::Mat
    class Square {
    public:
        // constructor init's vars
        Square() : pos(0), width(0), height(0), x(0), y(0) {}
        // change the size of a square
        void setSize(const int &xx, const int &yy, const int &w, const int &h);
        // set position
        void setPos(const int &p);
        // copy image from cv::Mat
        void copyImage(const cv::Mat &f);
        // copy Image to Target Matrix
        void copyImageToTarget(int xx, int yy, cv::Mat &f);
        // get position
        int getPos() const { return pos; }
        // get square width
        int getWidth() const { return width; }
        // get square height
        int getHeight() const { return height; }
    protected:
        // protected vars
        int pos,width,height,x,y;
        cv::Mat image;
    };
    // Particle movement directions
    enum { DIR_UP=0, DIR_UP_LEFT, DIR_UP_RIGHT, DIR_DOWN, DIR_DOWN_LEFT, DIR_DOWN_RIGHT, DIR_LEFT, DIR_RIGHT };
    // contains info for each pixel
    class Particle {
    public:
        Particle() : x(0), y(0), dir(0), m_count(0) {}
        cv::Vec3b pixel;// color
        int x, y, dir; // position/direction
        int m_count; // counter
    };
    // class to process the pixel
    class ParticleEmiter {
    public:
        // initalize to null
        ParticleEmiter();
        // clean up after done
        ~ParticleEmiter();
        // set frame pixel values
        void set(cv::Mat &frame);
        // draw pixel values to frame
        void draw(cv::Mat &frame);
        void draw_blend(cv::Mat &frame);
        void draw_flash(cv::Mat &frame);
        void draw_alpha(cv::Mat &frame);
        void draw_move(cv::Mat &frame);
        void draw_op(cv::Mat &frame);
        
        // move pixel coordinates around
        void movePixels();
        // reset
        void reset();
    private:
        Particle **part; // array of pointers for Particles
        int w, h; // frame width/height
        int speed;
    };
    extern int colors[3];
    // class to use for random growing filtered rects.
    class Box {
    public:
        Box() : x(0), y(0), w(0), h(0), steps(0), index(0), frame_index(0) {}
        void initBox(int w, int h);
        void drawBox(cv::Mat &frame);
        void sizeBox();
        int x,y,w,h,steps,index,frame_index;
        static int frame_width, frame_height; // current resolution
    };
    
    class GridBox {
    public:
        GridBox();
        GridBox(const GridBox &gb);
        GridBox(const Rect &r, const cv::Vec3b &col);
        GridBox(const cv::Vec3b &col);
        Rect location;
        cv::Vec3b color;
        bool on;
        GridBox &operator=(const GridBox &gb);
    };
    
    class Grid {
    public:
        GridBox **boxes;
        Grid();
        ~Grid();
        void createGrid(cv::Mat &frame, int w, int h, int size);
        void updateGrid(int max);
        void Release();
        void cleanBoxes();
        void fillGrid(cv::Mat &frame);
        int g_w, g_h, g_s;
        std::vector<Point> points;
        std::default_random_engine rng;
        int current_offset;
        bool g_random;
    };
    
    class LineObject {
    public:
        ac::Rect line_size;
        cv::Vec3b color;
        bool on;
        int dir;
    };
    
    class HLine {
    public:
        HLine();
        void drawLines(cv::Mat &frame);
        void createLines(int size, int width, int height);
        void clearLines();
        bool empty() const { return lines.empty(); }
    protected:
        int w, h;
        std::vector<LineObject> lines;
    };
    
    bool operator<(const Point &p1, const Point &p2);
    
    class ImageIndex {
    public:
        ImageIndex() : x(0), y(0) {}
        ImageIndex(int xx,int yy) {
            x = xx;
            y = yy;
        }
        ImageIndex(const ImageIndex &i) {
            x = i.x;
            y = i.y;
        }
        int x,y;
        bool operator<(const ImageIndex &i1) {
            if(x < i1.x && y < i1.y)
                return true;
            return false;
        }
    };
    
    class Pixelated {
    public:
        Pixelated();
        void reset(cv::Mat &val);
        void setPixel(int amount);
        bool resetNeeded() const { return reset_needed; }
        void setInit(const bool &b) { is_init = b; }
        bool isInit() const { return is_init; }
        void drawToMatrix(cv::Mat &frame);
        int size() const {  return pix_image.cols * pix_image.rows; }
    private:
        cv::Mat pix_image, copy_val;
        std::vector<ImageIndex> value_x;
        std::default_random_engine rng;
        int x_offset;
        bool reset_needed;
        bool is_init;
        void setPix();
    };
    
    // slow copy functions
    void copyMat(const cv::Mat &src,int src_x, int src_y, cv::Mat &target, const Rect &rc);
    void copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const Rect &rc);
    void copyMat(const cv::Mat &src, int x, int y, cv::Mat &target, int rx, int ry, int rw, int rh);
    void copyMat(const cv::Mat &src, const Rect &srcrc, cv::Mat &target, const Rect &dst);
    void copyMat(cv::Mat &frame, const cv::Mat &cpy, int x, int y);
    void copyMat(cv::Mat &frame, const cv::Mat &cpy);
    void copyMatSize(cv::Mat &frame, const cv::Mat &cpy, int offset_x, int offset_y);
    void fillRect(cv::Mat &m, const Rect &r, cv::Vec3b pixel);
    
    // Transform Template
    template<typename Func>
    void Transform(const cv::Mat &source, cv::Mat &output, Func func) {
        if(output.empty() || output.size() != source.size())
            output.create(source.size(), CV_8UC3);
        
        for(int z = 0; z < source.rows; ++z) {
            for(int i = 0; i < source.cols; ++i) {
                cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
                cv::Vec3b value = source.at<cv::Vec3b>(z, i);
                func(value, i, z);
                pixel = value;
                swapColors(output, z, i);
                if(isNegative) invert(output, z, i);
            }
        }
    }
    
    template<typename F>
    void transformMat(cv::Mat &src, const Rect &rc,F func) {
        for(int z = rc.y; z < rc.y+rc.h && z < src.rows; ++z) {
            for(int i = rc.x; i < rc.x+rc.w && i < src.cols; ++i) {
                cv::Vec3b &pixel = src.at<cv::Vec3b>(z, i);
                func(pixel, i, z);
            }
        }
    }
    extern Pixelated pix;
    struct PixelValues {
        int dir[4];
        int col[4];
        int add[4];
        int speed;
        int position_x, position_y;
        PixelValues() {
            for(int i = 0; i< 3; ++i) {
                dir[i] = rand()%2;
                col[i] = rand()%25;
                add[i] = 0;
            }
            speed = 1+(rand()%10);
            position_x = position_y = 0;
        }
        PixelValues(PixelValues &pv) {
            this->operator=(pv);
        }
        PixelValues& operator=(const PixelValues &pv) {
            for(int i =0; i < 3; ++i) {
                dir[i] = pv.dir[i];
                col[i] = pv.col[i];
                add[i] = pv.add[i];
            }
            speed = pv.speed;
            return *this;
        }
        PixelValues &operator=(const cv::Vec3b &v) {
            for(int j = 0; j < 3; ++j) {
                col[j] = v[j];
            }
            return *this;
        }
        
        void clear() {
            for(int i = 0; i < 3; ++i) {
                dir[i] = 0;
                col[i] = 0;
                add[i] = 0;
            }
            speed = 1;
            position_x = position_y = 0;
        }
    };
    
    class PixelArray2D {
    public:
        PixelArray2D() : pix_values(0), pix_x(0), pix_y(0), rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())) {}
        PixelArray2D(const PixelArray2D &) = delete;
        ~PixelArray2D();
        void create(cv::Mat &frame, int w, int h, int dir, bool addvec = false);
        void create_empty(int w, int h);
        void erase();
        void setAll(const int &value);
        void setAllDirection(const int &value);
        void insert(cv::Mat &image);
        PixelArray2D &operator=(const PixelArray2D &) = delete;
        PixelValues **pix_values;
        int getWidth() const {
            return pix_x;
        }
        int getHeight() const {
            return pix_y;
        }
        void set();
        void shuffle();
        void generateMatrix(cv::Mat &frame);
        std::vector<PixelValues*> pixel_index;
        
        void reset() {
            for(int z = 0; z < pix_y; ++z) {
                for(int i = 0; i < pix_x; ++i) {
                    PixelValues &p = pix_values[i][z];
                    p.clear();
                }
            }
        }
        
        template<typename Func>
        void setColorValues(Func value) {
            for(int z = 0; z < pix_y; ++z) {
                for(int i = 0; i < pix_x; ++i) {
                    PixelValues &p = pix_values[i][z];
                    for(int j = 0; j < 3; ++j)  {
                        p.col[j] = value(i, z);
                        p.dir[j] = rand()%2;
                    }
                }
            }
        }
    protected:
        int pix_x, pix_y;
        std::default_random_engine rng;
    };
    extern bool image_matrix_reset, image_cycle_reset;
    
    template<int Size>
    void MatrixDrawFrames(cv::Mat &frame, MatrixCollection<Size> *collection, int w, int h, int subfilter) {
        cv::Mat resized;
        cv::Size s(frame.cols/w, frame.rows/h);
        ac_resize(frame, resized, s);
        if(subfilter != -1)
            CallFilter(subfilter, resized);
        
        collection->shiftFrames(resized);
        int index = 0;
        for(int x = 0; x < w; ++x) {
            for(int y = 0; y < h; ++y) {
                Rect rc(x*s.width, y*s.height, s.width, s.height);
                cv::Mat &cp = collection->frames[index];
                copyMat(cp, 0, 0, frame, rc);
                ++index;
            }
        }
        AddInvert(frame);
    }
    
    template<int Size>
    void BlurVideoMatrix(cv::Mat &frame, MatrixCollection<Size> *collection) {
        cv::Mat vframe;
        if(VideoFrame(vframe)) {
            cv::Mat reframe;
            ac_resize(vframe, reframe, frame.size());
            collection->shiftFrames(reframe);
            Smooth(frame, collection, false);
        }
    }
    
    template<int Size>
    void GlitchyXorTrails(cv::Mat &frame, MatrixCollection<Size> *collection, bool random = false) {
        collection->shiftFrames(frame);
        static int square_max = (frame.rows / (collection->size()-1));
        static int square_size = 2 + (rand()% (square_max));
        int row = 0;
        int off = 0;
        int off_row = 1+rand()%35;
        int size_past = 0;
        while(row < frame.rows-1) {
            square_size = 2 + (rand()% (square_max));
            for(int z = row; z < row+square_size; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    if(i < frame.cols-1 && z < frame.rows-1) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix;
                        int frame_index = (rand()%(collection->size()-1));
                        if(frame_index < (collection->size()-1) && off < (collection->size()-1)) {
                            if(random == false && z < collection->frames[off].rows && i < collection->frames[off].cols)
                                pix = collection->frames[off].template at<cv::Vec3b>(z, i);
                            else if(z < collection->frames[off].rows && i < collection->frames[off].cols)
                                pix = collection->frames[frame_index].template at<cv::Vec3b>(z, i);;
                            
                            for(int j = 0; j < 3; ++j) {
                                if(off_row <= 5 && (z%off_row)==0)
                                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j])) ^ pix[j];
                                else
                                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
            row += square_size;
            size_past += square_size;
            if(row > frame.rows-1 || size_past > square_max-1) {
                size_past = 0;
                ++off;
                if(off > (collection->size()-1)) {
                    off = 0;
                    break;
                }
            }
        }
    }
    
    template<int Size>
    void GlitchyXorTrailsRandom(cv::Mat &frame, MatrixCollection<Size> *collection) {
        collection->shiftFrames(frame);
        static int square_max = (frame.rows / (collection->size()-1));
        static int square_size =2+ (rand()% (square_max));
        int row = 0;
        int off = 0;
        int off_row = 1+rand()%35;
        int size_past = 0;
        while(row < frame.rows-1) {
            square_size = 2+ (rand()% (square_max));
            int frame_index = (rand()%(collection->size()-1));
            for(int z = row; z < row+square_size; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    if(i < frame.cols-1 && z < frame.rows-1) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        cv::Vec3b pix;
                        if(frame_index < (collection->size()-1) && off < (collection->size()-1) && z < collection->frames[frame_index].rows && i < collection->frames[frame_index].cols) {
                            pix = collection->frames[frame_index].template at<cv::Vec3b>(z, i);
                            
                            for(int j = 0; j < 3; ++j) {
                                if(off_row <= 5 && (z%off_row)==0)
                                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j])) ^ pix[j];
                                else
                                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
            row += square_size;
            size_past += square_size;
            if(size_past > square_max-1) {
                size_past = 0;
                ++off;
                if(row > frame.rows-1 || off > (collection->size()-1)) {
                    off = 0;
                    break;
                }
            }
        }
    }
    
    template<int Size>
    void MatrixCollectionTruncate(cv::Mat &frame, MatrixCollection<Size> *collection, int maxSize) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > maxSize)
                        pixel[j] = maxSize;
                }
            }
        }
        collection->shiftFrames(frame);
    }
    
    class Histogram_Type {
    private:
        int histSize;
    public:
        Histogram_Type() : histSize{256} {}
        cv::Mat b_hist, g_hist, r_hist;
        cv::Mat createHistogram(cv::Mat &source) {
            std::vector<cv::Mat> bgr_planes;
            split(source, bgr_planes);
            float range[] = {0,256};
            const float *histRange = {range};
            bool uniform = true, accumulate = false;
            cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::Mat histImage(source.rows, source.cols, CV_8UC3, cv::Scalar(0,0,0));
            cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            cv::Mat channels[3];
            channels[0] = b_hist;
            channels[1] = r_hist;
            channels[2] = r_hist;
            cv::merge(channels, 3, histImage);
            return histImage;
        }

        cv::Mat createGraph(cv::Mat &source) {
            std::vector<cv::Mat> bgr_planes;
            split(source, bgr_planes);
            float range[] = {0,256};
            const float *histRange = {range};
            cv::Mat b_hist, g_hist, r_hist;
            bool uniform = true, accumulate = false;
            cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::Mat histImage(source.rows, source.cols, CV_8UC3, cv::Scalar(0,0,0));
            cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
            int hist_w = histImage.cols;
            int hist_h = histImage.rows;
            int bin_w = cvRound( (double) hist_w/histSize );

            for( int i = 1; i < histSize; i++ )
            {
                cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ),
                         cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                         cv::Scalar( 255, 0, 0), 2, 8, 0  );
                cv::line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ),
                     cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                     cv::Scalar( 0, 255, 0), 2, 8, 0  );
                line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ),
                     cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                     cv::Scalar( 0, 0, 255), 2, 8, 0  );
            }
            return histImage;
        }
    };
}

extern ac::ParticleEmiter emiter;
void changePixel(cv::Mat &full_buffer, int i, int z, cv::Vec3b &buffer, double pos, double *count);
extern std::vector<ac::Keys> blocked_color_keys;

#endif
