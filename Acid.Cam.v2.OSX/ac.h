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
//#define ASSERT_CHECK
// Macro for assert testing
#ifdef ASSERT_CHECK
#define ASSERT(X) assert(X)
#else
#define ASSERT(X)
#endif
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
extern bool negate, blend_set, colorkey_set;
extern cv::Mat blend_image, blend_image_scaled, color_image;
// acid cam namespace
namespace ac {
    // version string
    extern const std::string version;
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
    extern cv::Size resolution;
    extern int set_color_map;
    inline int GetFX(cv::Mat &frame, int x, int nw);
    inline int GetFY(cv::Mat &frame, int y, int nh);
    inline void invert(cv::Mat &frame, int x, int y);
    /* filter typedef */
    typedef void (*DrawFunction)(cv::Mat &frame);
    // ror/rol tempaltes
    template<typename T>
    inline T ror(T x, unsigned int m){
        return (x >> m) | (x << (sizeof(T)*8 - m));
    }
    template<typename T>
    inline T rol(T x, unsigned int m) {
        return (x << m) | (x >> (sizeof(T)*8 -m));
    }
    DrawFunction getRandomFilter(unsigned int &index);
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
    void TestFilter108(cv::Mat &frame);
    // No filter (do nothing)
    void NoFilter(cv::Mat &frame);
    // Alpha blend with original image
    void BlendWithSource(cv::Mat &frame);
    // plugin must be implemented in project
    void plugin(cv::Mat &frame);
    // set Custom Filter callback function
    void setCustom(DrawFunction f);
    void setPlugin(DrawFunction f);
    void setProcMode(unsigned int value);
    // color maps
    void Negate(cv::Mat &frame);
    void ApplyColorMap(cv::Mat &frame);
    // color correction
    void setBrightness(cv::Mat &frame, double alpha, int beta);
    void setGamma(cv::Mat &frame, cv::Mat &outframe, double gamma);
    void setSaturation(cv::Mat &frame, int saturation);
    void AlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha);
    inline void swapColors(cv::Mat &frame, int x, int y);
    inline void swapColors_(cv::Mat &frame, int x, int y);
    inline void procPos(int &direction, double &pos, double &pos_max, const double max_size = 15);
    // Alpha Blend two filters and set to frame by alpha variable
    void filterFade(cv::Mat &frame, int filter1, int filter2, double alpha);
    void filterColorKeyed(const cv::Vec3b &color, const cv::Mat &orig, const cv::Mat &filtered, cv::Mat &output);
    // draw functions / strings
    extern std::string *draw_strings;
    extern DrawFunction plugin_func;
    extern DrawFunction *draw_func;
    extern int draw_max;
    extern bool snapShot;
    extern std::unordered_map<std::string, int> filter_map;
    void fill_filter_map();
    // Matrix Collection template
    template<int Size>
    class MatrixCollection {
    public:
        static constexpr unsigned int ArraySize = Size;
        MatrixCollection() : w(0), h(0) {}
        cv::Mat frames[Size+1];
        int w, h;
        void shiftFrames(cv::Mat &frame) {
            int wx = frame.cols;
            int wh = frame.rows;
            if(w != wx || h != wh) {
                for(int i = 0; i < Size; ++i)
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
        unsigned int size() const { return ArraySize; }
    };
    
    // Trails function
    template<int Size>
    void Smooth(cv::Mat &frame, MatrixCollection<Size> *collection);
    
    // point class
    class Point {
    public:
        Point();
        Point(const Point &p);
        Point(unsigned int xx, unsigned int yy);
        unsigned int x, y;
        void setPoint(unsigned int xx, unsigned int yy);
        Point &operator=(const Point &p);
    };
    // Rectangle class
    class Rect {
    public:
        Rect();
        Rect(const Rect &r);
        Rect(unsigned int xx, unsigned int yy, unsigned int ww, unsigned int hh);
        Rect(unsigned int xx, unsigned int yy);
        Rect(unsigned int xx, unsigned int yy, cv::Size s);
        Rect(Point pt, unsigned int ww, unsigned int hh);
        Rect(Point pt, cv::Size s);
        void setRect(unsigned int xx, unsigned int yy, unsigned int ww, unsigned int hh);
        Rect &operator=(const Rect &r);
        unsigned int x,y,w,h;
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
    enum { DIR_UP=0, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
    // contains info for each pixel
    class Particle {
    public:
        Particle() : x(0), y(0), dir(0), m_count(0) {}
        cv::Vec3b pixel;// color
        unsigned int x, y, dir; // position/direction
        unsigned int m_count; // counter
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
        // move pixel coordinates around
        void movePixels();
        // reset
        void reset();
    private:
        Particle **part; // array of pointers for Particles
        unsigned int w, h; // frame width/height
    };
    extern int colors[3];
    // class to use for random growing filtered rects.
    class Box {
    public:
        Box() : x(0), y(0), w(0), h(0), steps(0), index(0), frame_index(0) {}
        void initBox(int w, int h);
        void drawBox(cv::Mat &frame);
        void sizeBox();
        unsigned int x,y,w,h,steps,index,frame_index;
        static unsigned int frame_width, frame_height; // current resolution
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
        void createGrid(cv::Mat &frame, unsigned int w, unsigned int h, unsigned int size);
        void updateGrid(unsigned int max);
        void Release();
        void cleanBoxes();
        void fillGrid(cv::Mat &frame);
        unsigned int g_w, g_h, g_s;
        std::vector<Point> points;
        std::default_random_engine rng;
        unsigned int current_offset;
        bool g_random;
    };
    
    bool operator<(const Point &p1, const Point &p2);
    
    // slow
    void copyMat(const cv::Mat &src,unsigned int src_x, unsigned int src_y, cv::Mat &target, const ac::Rect &rc);
    void copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const ac::Rect &rc);
    void copyMat(const cv::Mat &src, unsigned int x, unsigned int y, cv::Mat &target, unsigned int rx, unsigned int ry, unsigned int rw, unsigned int rh);
    void fillRect(cv::Mat &m, const Rect &r, cv::Vec3b pixel);
    
    template<typename F>
    void transformMat(cv::Mat &src,const Rect &rc,F func);
}

extern ac::ParticleEmiter emiter;

#endif
