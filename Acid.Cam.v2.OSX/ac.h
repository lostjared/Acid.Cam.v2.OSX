/*
 * Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
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
#ifndef __AC_H__
#define __AC_H__
#ifdef __APPLE__
#include<opencv2/videoio.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#else
#include<opencv2/opencv.hpp>
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
//#define ASSERT_CHECK
// Macro for assert testing
#ifdef ASSERT_CHECK
#define ASSERT(X) assert(X)
#else
#define ASSERT(X)
#endif
/*
 *
 * Be sure to call fill_filter_map
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
extern bool negate, blend_set, colorkey_set;
extern cv::Mat blend_image,color_image;

int AC_GetFX(int oldw,int x, int nw);
int AC_GetFZ(int oldh, int y, int nh);

// acid cam namespace
namespace ac {
    // get version string
    extern std::string getVersion();
    // version string
    extern const std::string version;
    extern double translation_variable, pass2_alpha;
    extern double alpha, tr;
    extern bool isNegative, noRecord,iRev;
    extern int color_order;
    extern double fps;
    extern int draw_offset;
    extern int subfilter;
    extern std::string fileName;
    extern cv::Mat orig_frame;
    extern cv::Mat blendW_frame;
    extern cv::Mat image_files[4];
    extern bool images_Enabled,fps_force;
    extern int snapshot_Type;
    extern bool in_custom;
    extern int swapColor_r, swapColor_g, swapColor_b;
    extern cv::Size resolution;
    extern bool strobe_It;
    extern int set_color_map;
    extern bool color_map_set;
    extern int GetFX(cv::Mat &frame, int x, int nw);
    extern int GetFY(cv::Mat &frame, int y, int nh);
    extern bool reset_alpha;
    void invert(cv::Mat &frame, int x, int y);
    /* filter typedef */
    typedef void (*DrawFunction)(cv::Mat &frame);
    typedef std::pair<std::string,DrawFunction> FilterType;
    extern DrawFunction custom_callback;
    extern DrawFunction plugin_func;
    // ror/rol tempaltes
    template<typename T>
    inline T ror(T x, int m){
        return (x >> m) | (x << (sizeof(T)*8 - m));
    }
    template<typename T>
    inline T rol(T x, int m) {
        return (x << m) | (x >> (sizeof(T)*8 -m));
    }
    // be sure to call this when the application starts
    void fill_filter_map();
    // draw functions
    DrawFunction getRandomFilter(int &index);
    void DrawFilter(const std::string &name, const cv::Mat &frame, cv::Mat &outframe);
    void DrawFilter(int index, const cv::Mat &frame, cv::Mat &outframe);
    void DrawFilter(int index, cv::Mat &frame);
    void DrawFilter(const std::string &name, cv::Mat &frame);
    DrawFunction getFilter(std::string name);
    FilterType filterByIndex(const int &num);
    FilterType filterByString(const std::string &num);
    bool CallFilter(int index, cv::Mat &frame);
    bool CallFilter(const std::string &name, cv::Mat &frame);
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
    void NotEqual(cv::Mat &frame);
    void ImageShiftUpLeft(cv::Mat &frame);
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
    void LeftLines(cv::Mat &frame);
    void Curtain(cv::Mat &frame);
    void RandomCurtain(cv::Mat &frame);
    void CurtainVertical(cv::Mat &frame);
    void RandomCurtainVertical(cv::Mat &frame);
    void inOrder(cv::Mat &frame);
    void inOrderBySecond(cv::Mat &frame);
    void DarkenFilter(cv::Mat &frame);
    void RandomFilterBySecond(cv::Mat &frame);
    void ThreeRandom(cv::Mat &frame);
    void inOrderAlpha(cv::Mat &frame);
    void inOrderAlphaXor(cv::Mat &frame);
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
    void AddFilter(cv::Mat &frame);
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
    // No filter (do nothing)
    void NoFilter(cv::Mat &frame);
    // Alpha blend with original image
    void BlendWithSource(cv::Mat &frame);
    // plugin must be implemented in project
    void plugin(cv::Mat &frame);
    // set Custom Filter callback function
    void setCustom(DrawFunction f);
    void setPlugin(DrawFunction f);
    void setProcMode(int value);
    void setSubFilter(int value);
    void clearSubFilter();
    // color maps
    void Negate(cv::Mat &frame);
    void ApplyColorMap(cv::Mat &frame);
    void AddInvert(cv::Mat &frame);
    // color correction
    void setBlendPercentage(const double &value);
    void setBrightness(cv::Mat &frame, double alpha, int beta);
    void setGamma(cv::Mat &frame, cv::Mat &outframe, double gamma);
    void setSaturation(cv::Mat &frame, int saturation);
    void AlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    void AlphaXorBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    void DarkenImage(cv::Mat &frame, unsigned int size);
    void Add(cv::Mat &src, cv::Mat &add, bool sat = false);
    void Sub(cv::Mat &src, cv::Mat &sub, bool sat = false);
    void ScalarAverage(const cv::Mat &frame, cv::Scalar &s);
    void TotalAverageOffset(cv::Mat &frame, unsigned long &value);
    void swapColors(cv::Mat &frame, int x, int y);
    void swapColors_(cv::Mat &frame, int x, int y);
    void procPos(int &direction, double &pos, double &pos_max, const double max_size = 15, double iter = 0.05);
    // Alpha Blend two filters and set to frame by alpha variable
    void filterFade(cv::Mat &frame, int filter1, int filter2, double alpha);
    void filterColorKeyed(const cv::Vec3b &color, const cv::Mat &orig, const cv::Mat &filtered, cv::Mat &output);
    void resetAlpha(int &dir, double &alpha);
    void resetAlpha(double &alpha);
    void SwapColors(cv::Vec3b &v);
    void FillRow(cv::Mat &frame, unsigned int row, unsigned char value);
    void Shuffle(int &index, cv::Mat &frame, std::vector<std::string> &filter_array);
    // draw functions / strings
    extern std::string *draw_strings;
    extern DrawFunction plugin_func;
    extern DrawFunction *draw_func;
    extern FilterType *filter_array;
    extern int draw_max;
    extern bool snapShot;
    extern bool reset_filter;
    extern double alpha_increase;
    extern std::unordered_map<std::string, int> filter_map;
    extern bool frames_released;
    extern std::vector<void *> all_objects;
    // Matrix Collection template
    template<int Size>
    class MatrixCollection {
    public:
        static constexpr int ArraySize = Size;
        MatrixCollection() : w(0), h(0) {
            for(int i = 0; i < Size; ++i)
                all_objects.push_back(&frames[i]);
        }
        cv::Mat frames[Size+4];
        int w, h;
        void shiftFrames(cv::Mat &frame) {
            int wx = frame.cols;
            int wh = frame.rows;
            // check if any frames were released.
            bool check_released = false;
            for(int i = 0; i < Size; ++i) {
                if(frames[i].empty()) {
                    check_released = true;
                    break;
                }
            }
            if(check_released == true || (w != wx || h != wh) || reset_filter == true || frames_released == true) {
                for(int i = 0; i < Size; ++i)
                    frames[i] = frame.clone();
                w = wx;
                h = wh;
                reset_filter = false;
                return;
            }
            for(int i = Size-1; i > 0; --i) {
                frames[i] = frames[i-1];
            }
            frames[0] = frame.clone();
        }
        
        int size() const { return ArraySize; }
    };
    extern void release_all_objects();
    extern bool testSize(cv::Mat &frame);
    // Trails function
    template<int Size>
    void Smooth(cv::Mat &frame, MatrixCollection<Size> *collection, bool addframe = true) {
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    
    // slow copy functions
    void copyMat(const cv::Mat &src,int src_x, int src_y, cv::Mat &target, const Rect &rc);
    void copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const Rect &rc);
    void copyMat(const cv::Mat &src, int x, int y, cv::Mat &target, int rx, int ry, int rw, int rh);
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
    
}

extern ac::ParticleEmiter emiter;
void changePixel(cv::Mat &full_buffer, int i, int z, cv::Vec3b &buffer, double pos, double *count);

#endif
