/*
 * Software written by Jared Bruni https://github.com/lostjared
 
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

#include "ac.h"
// Acid Cam namespace
namespace ac {
#if defined(__APPLE__)
    const std::string version="2.5.8 (macOS)";
#elif defined(__linux__)
    const std::string version="2.5.8 (Linux)";
#elif defined(_WIN32)
    const std::string version="2.5.8 (Windows)";
#else
    const std::string version="2.5.8 (Generic)";
#endif
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
    std::string draw_strings_value[] = { "Self AlphaBlend", "Self Scale", "StrobeEffect", "Blend #3", "Negative Paradox","ThoughtMode", "RandTriBlend", "Blank", "Tri", "Distort", "CDraw", "Type", "NewOne", "Blend Fractal","Blend Fractal Mood", "CosSinMultiply", "Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8","Filter3","Rainbow Blend","Rand Blend","New Blend", "Alpha Flame Filters", "Pixel Scale", "PixelSort", "GlitchSort","Random Filter", "Random Flash", "Blend with Image", "Blend with Image #2", "Blend with Image #3", "Blend with Image #4", "GaussianBlur", "Median Blur", "Blur Distortion", "Diamond Pattern","MirrorBlend","Pulse","Sideways Mirror","Mirror No Blend","Sort Fuzz","Fuzz","Double Vision","RGB Shift","RGB Sep","Graident Rainbow","Gradient Rainbow Flash", "Reverse", "Scanlines", "TV Static", "Mirror Average", "Mirror Average Mix", "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND","Bitwise_OR","Equalize", "Channel Sort", "Reverse_XOR", "Combine Pixels", "FlipTrip", "Canny","Boxes","Boxes Fade", "Flash Black","SlideRGB","Side2Side","Top2Bottom","Strobe Red Then Green Then Blue","Blend_Angle", "Outward", "Outward Square","ShiftPixels", "ShiftPixelsDown", "XorMultiBlend", "Bitwise_Rotate", "Bitwise_Rotate Diff","HPPD","FuzzyLines","GradientLines","GradientSelf","GradientSelfVertical","GradientDown","GraidentHorizontal","GradientRGB","Inter","UpDown","LeftRight","StrobeScan","BlendedScanLines","GradientStripes","XorSine","SquareSwap","SquareSwap4x2","SquareSwap8x4","SquareSwap16x8","SquareSwap64x32","SquareBars","SquareBars8","SquareSwapRand16x8","SquareVertical8","SquareVertical16","SquareVertical_Roll","SquareSwapSort_Roll","SquareVertical_RollReverse","SquareSwapSort_RollReverse","Circular","WhitePixel","FrameBlend","FrameBlendRGB","TrailsFilter","TrailsFilterIntense","TrailsFilterSelfAlpha","TrailsFilterXor","ColorTrails","MoveRed","MoveRGB","MoveRedGreenBlue","BlurSim","Block","BlockXor","BlockScale","BlockStrobe","PrevFrameBlend","Wave","HighWave","VerticalSort","VerticalChannelSort","HorizontalBlend","VerticalBlend","OppositeBlend","DiagonalLines","HorizontalLines","InvertedScanlines","Soft_Mirror","KanapaTrip","ColorMorphing","ScanSwitch","ScanAlphaSwitch","NegativeStrobe", "XorAddMul","ParticleRelease", "BlendSwitch", "All Red", "All Green", "All Blue","LineRGB","PixelRGB","BoxedRGB","KruegerSweater","RGBFlash","IncreaseBlendHorizontal","BlendIncrease","GradientReverse","GradientReverseVertical","GradientReverseBox","GradientNewFilter","ReinterpretDouble","ReinterpSelfScale","AverageLines","ImageFile","ImageXor","ImageAlphaBlend","ColorRange","ImageInter","TrailsInter","TrailsBlend","TrailsNegate","InterReverse","InterMirror","InterFullMirror","MirrorRGB","RGBStatic1","RGBStatic2","VectorIncrease","LineByLineReverse","RandomIntertwine","RandomFour","BlendThree","AcidTrails","RandomTwo","HorizontalTrailsInter","Trails","BlendTrails","Negate","RandomFilteredSquare","ImageX","RandomQuads","QuadCosSinMultiply","QuadRandomFilter","RollRandom","AverageRandom","HorizontalStripes","DiamondStrobe","SmoothTrails","GridFilter8x","GridFilter16x","GridFilter8xBlend","GridRandom","GridRandomPixel","Dual_SelfAlphaRainbow","Dual_SelfAlphaBlur","SurroundPixelXor","Darken","WeakBlend","AverageVertical","RandomCollectionAverage","RandomCollectionAverageMax","SmoothTrailsSelfAlphaBlend","SmoothTrailsRainbowBlend","MedianBlend","SmoothRandomImageBlend","SmoothImageAlphaBlend","RandomAlphaBlend","RandomTwoFilterAlphaBlend","PixelatedSquare","AlphaBlendPosition","BlendRowAlpha", "BlendRow","BlendRowByVar","BlendRowByDirection","BlendAlphaXor","SelfXorScale","BitwiseXorScale","XorTrails", "RainbowTrails","NegativeTrails","IntenseTrails","SelfAlphaRGB","BlendImageOnOff","XorSelfAlphaImage","BitwiseXorStrobe","AlphaBlendRandom","ChannelSortAlphaBlend","XorChannelSort","GradientColors","GradientColorsVertical","Bitwise_XOR_Average","NotEqual","ImageShiftUpLeft","GradientXorSelfScale","SmoothSourcePixel","StrobeBlend","FrameBars","Sort_Vertical_Horizontal","Sort_Vertical_Horizontal_Bitwise_XOR","Scalar_Average_Multiply","Scalar_Average","Total_Average","AlphaBlendImageXor","FlashWhite","FlashBlackAndWhite","GaussianBlend","RandomXor","RandomXorFlash","RandomAmountMedianBlur","SoftXor","SelfXorBlend","SelfXorDoubleFlash","SelfOrDoubleFlash","BlendRowCurvedSqrt","CycleShiftRGB","CycleShiftRandomRGB","CycleShiftRandomRGB_XorBlend","CycleShiftRandomAlphaBlend","VerticalColorBars","GradientLeftRight","GraidentUpDown","GradientLeftRightInOut","GradientUpDownInOut","Lines","ColorLines","WhiteLines","ThickWhiteLines","UseLineObject","TanAlphaGrid","MedianBlendAnimation","FibFlash", "ScaleFlash","LeftLines","Curtain","RandomCurtain","CurtainVertical","RandomCurtainVertical","inOrder","inOrderBySecond","DarkenFilter","RandomFilterBySecond","ThreeRandom","inOrderAlpha","inOrderAlphaXor","SlideFilter","SlideFilterXor", "RandomSlideFilter","SlideUpDown","SlideUpDownXor", "SlideUpDownRandom","SlideSubFilter", "SlideSubUpDownFilter","ParticleBlend","ParticleFlash","ExactImage","ParticleAlpha","BlendInAndOut","BlendScaleInAndOut","AcidGlitch","XorBackwards","LiquidFilter","MatrixXorAnd","XorAlpha","AlphaAcidTrails","SelfXorAverage","RandomXorBlend","RGBVerticalXor","RGBVerticalXorScale","RGBHorizontalXor","RGBHorizontalXorScale","FadeStrobe","RGBMirror","MirrorStrobe","AndStrobe","AndStrobeScale", "AndPixelStrobe","AndOrXorStrobe","AndOrXorStrobeScale","FadeInAndOut","BrightStrobe","DarkStrobe","ParticleFast","RandomXorOpposite","StrobeTransform","InitBlend","MoveUpLeft","RandomStrobe","RandomBlur","Stuck","StuckStrobe","OrStrobe","LagBlend","SubFilter","AddFilter","BlendImageXor","BlendImageAround_Median","ImageBlendTransform","RGBTrails","RGBTrailsDark","RGBTrailsAlpha","RGBTrailsNegativeAlpha","MovementRGBTrails","RGBTrailsXor","DifferenceStrobe","BlackAndWhiteDifferenceStrobe","DifferenceXor","DifferenceRand", "DifferenceBrightStrobe","PsycheTrails","FourSquare","EightSquare","DiagonalSquare","DiagonalSquareRandom","SquareStretchDown","SquareStretchRight","SquareStretchUp","SquareStretchLeft","DarkTrails","SoftFeedback","SoftFeedbackFrames","ResizeSoftFeedback","SoftFeedback8","SoftFeedbackFrames8","ResizeSoftFeedback8","ResizeSoftFeedbackSubFilter","SoftFeedbackRandFilter","SoftFeedback32","SoftFeedbackFrames32","ResizeSoftFeedback32","SoftFeedbackRandFilter32","SoftFeedbackSubFilter","SoftFeedbackResizeSubFilter","SoftFeedbackResize64","SoftFeedbackResizeSubFilter64","SoftFeedbackReszieSubFilter64_Negate","SoftFeedbackReszieSubFilter64_Mirror","HalfNegateStrobe","MedianBlurXor","NegateTrails","RandomGradient","RandomStrobeFlash","RandomMirror","RandomOther","RandomXorFilter","RandomMirrorBlend","RandomMirrorAlphaBlend","Bitwise_XOR_AlphaSubFilter","AlphaBlendSubFilter","GradientSubFilterXor","XorBlend_SubFilter","SmoothSubFilterAlphaBlend","SmoothSubFilterXorBlend","IntertwineSubFilter","RandBlend","EveryOther","EveryOtherSubFilter","SmoothRandomFilter","RandomFilterRandomTimes","RandomSubFilterRandomTimes","AddToFrameSubFilter","MirrorXor","MirrorXorAll","MirrorXorScale","EnergyMirror","SmoothSubFilter","EnergizeSubFilter","SmoothSubFilter16","EnergizeSubFilter16","EnergizeSubFilter32","SmoothSubFilter32","HalfAddSubFilter","HalfXorSubFilter","StaticXorBlend","PsycheSort","XorScale","ChannelMedianSubFilter","GaussianStrobe","StrobeSort","GlitchSortStrobe","Bitwise_XOR_Blend","Bitwise_XOR_Sort","Bitwise_OR_Blend","Bitwise_AND_Blend","BitwiseColorMatrix","PixelReverseXor","PixelatedSubFilterSort","SilverBlend","RandomPixelOrderSort","ImageXorAlpha","ImageAverageXor","PixelXorBlend","SelfAlphaScale","SelfScaleAlpha","RainbowXorBlend","FrameDifference","SmallDiffference","FadeBlend","FilteredDifferenceSubFilter","ExpandSquareSubFilter","ExpandSquareBlendSubFilter","ExpandSquareVerticalSubFilter","DarkImageMedianBlend","GammaDarken5","GammaDarken10","SelfAlphaScaleBlend","FadeBars","MirrorXorAlpha","MirrorEnergizeSubFilter","StrobeXor","IntertwinedMirror","BlurredMirror","ShadeRGB","InterRGB_SubFilter","InterSmoothSubFilter","InterRGB_Bars_XY","InterRGB_Bars_X","InterRGB_Bars_Y","StoredFramesAlphaBlend_SubFilter","BlendSubFilter","BlendAlphaSubFilter","ReverseFrameBlend","ReverseFrameBlendSwitch","DoubleRandomMirror","Blend_AlphaSubFilter","RandomBlendFilter","DoubleRandomBlendFilter","FlipBlendW","FlipBlendWH", "FlipBlendH", "FlipBlendAll","FrameMedianBlendSubFilter","FrameBlurSubFilter","ImageBlendSubFilter","ImageBlendXorSubFilter","ImageCollectionSubFilter","SelfScaleXorIncrease","Blend_RedGreenBlue","XorBlend_RedGreenBlue","BlendIncrease_RedGreenBlue","Blend_RedReenBlue_Dark","DarkModBlend","PictureBuzz","IncDifference","IncDifferenceAlpha","MirrorMedianBlend","SubFilterMedianBlend","DarkenBlend","DarkCollectionSubFilter","ChannelSort_NoBlend_Descending", "ChannelSort_NoBlend_Ascending","Headrush","DarkSmooth_Filter","DarkSelfAlpha","FlipMedian","FlipMedianSubFilter","FlipMirror","FlipMirrorAverage","FlipMirrorSubFilter","ShuffleMedian","ShuffleRGB","ParticleSnow","RandomPixels","DarkRandomPixels","MedianBlurSubFilter","Bars","ShuffleAlpha","AlphaMorph","ShuffleSelf",
        "No Filter",
        "Blend with Source", "Plugin", "Custom","Blend With Image #1","TriBlend with Image", "Image Strobe", "Image distraction" };
    // draw strings
    std::string *draw_strings = draw_strings_value;
    
    
    // filter callback functions
    DrawFunction draw_func_value[] = { SelfAlphaBlend, SelfScale, StrobeEffect, Blend3, NegParadox, ThoughtMode, RandTriBlend, Blank,Tri,Distort,CDraw,Type,NewOne,blendFractal,blendFractalMood,cossinMultiply,colorAccumulate1,colorAccumulate2,colorAccumulate3,filter8,filter3,rainbowBlend,randBlend,newBlend,alphaFlame,pixelScale,pixelSort,glitchSort,randomFilter,randomFlash,imageBlend,imageBlendTwo,imageBlendThree,imageBlendFour,GaussianBlur,MedianBlur,BlurDistortion,DiamondPattern,MirrorBlend,Pulse,SidewaysMirror,MirrorNoBlend,SortFuzz,Fuzz,DoubleVision,RGBShift,RGBSep,GradientRainbow,GradientRainbowFlash,Reverse,Scanlines,TVStatic,MirrorAverage,MirrorAverageMix,Mean,Laplacian,Bitwise_XOR,Bitwise_AND,Bitwise_OR,Equalize,ChannelSort,Reverse_XOR,CombinePixels,FlipTrip,Canny,Boxes,BoxesFade,FlashBlack,SlideRGB,Side2Side,Top2Bottom,StrobeRedGreenBlue,Blend_Angle,Outward,OutwardSquare,ShiftPixels,ShiftPixelsDown,XorMultiBlend,BitwiseRotate,BitwiseRotateDiff,HPPD,FuzzyLines,GradientLines,GradientSelf,GradientSelfVertical,GradientDown,GraidentHorizontal,GradientRGB,Inter,UpDown,LeftRight,StrobeScan,BlendedScanLines,GradientStripes,XorSine,SquareSwap,SquareSwap4x2,SquareSwap8x4,SquareSwap16x8,SquareSwap64x32,SquareBars,SquareBars8,SquareSwapRand16x8,SquareVertical8,SquareVertical16,SquareVertical_Roll,SquareSwapSort_Roll,SquareVertical_RollReverse,SquareSwapSort_RollReverse,Circular,WhitePixel,FrameBlend,FrameBlendRGB,TrailsFilter,TrailsFilterIntense,TrailsFilterSelfAlpha,TrailsFilterXor,ColorTrails,MoveRed,MoveRGB,MoveRedGreenBlue,BlurSim,Block,BlockXor,BlockScale,BlockStrobe,PrevFrameBlend,Wave,HighWave,VerticalSort,VerticalChannelSort,HorizontalBlend,VerticalBlend,OppositeBlend,DiagonalLines,HorizontalLines,InvertedScanlines,Soft_Mirror,KanapaTrip,ColorMorphing,ScanSwitch,ScanAlphaSwitch,NegativeStrobe,XorAddMul,ParticleRelease,BlendSwitch,AllRed,AllGreen,AllBlue,LineRGB,PixelRGB,BoxedRGB,KruegerSweater,RGBFlash,IncreaseBlendHorizontal,BlendIncrease,GradientReverse,GradientReverseVertical,GradientReverseBox,GradientNewFilter,ReinterpretDouble,ReinterpSelfScale,AverageLines,ImageFile,ImageXor,ImageAlphaBlend,ColorRange,ImageInter,TrailsInter,TrailsBlend,TrailsNegate,InterReverse,InterMirror,InterFullMirror,MirrorRGB,RGBStatic1,RGBStatic2,VectorIncrease,LineByLineReverse,RandomIntertwine,RandomFour,BlendThree,AcidTrails,RandomTwo,HorizontalTrailsInter,Trails,BlendTrails,Negate,RandomFilteredSquare,ImageX,RandomQuads,QuadCosSinMultiply,QuadRandomFilter,RollRandom,AverageRandom,HorizontalStripes,DiamondStrobe,SmoothTrails,GridFilter8x,GridFilter16x,GridFilter8xBlend,GridRandom,GridRandomPixel,Dual_SelfAlphaRainbow,Dual_SelfAlphaBlur,SurroundPixelXor,Darken,WeakBlend,AverageVertical,RandomCollectionAverage,RandomCollectionAverageMax,SmoothTrailsSelfAlphaBlend,SmoothTrailsRainbowBlend,MedianBlend,SmoothRandomImageBlend,SmoothImageAlphaBlend,RandomAlphaBlend,RandomTwoFilterAlphaBlend,PixelatedSquare,AlphaBlendPosition,BlendRowAlpha,BlendRow,BlendRowByVar,BlendRowByDirection,BlendAlphaXor,SelfXorScale,BitwiseXorScale,XorTrails,RainbowTrails,NegativeTrails,IntenseTrails,SelfAlphaRGB,BlendImageOnOff,XorSelfAlphaImage,BitwiseXorStrobe,AlphaBlendRandom,ChannelSortAlphaBlend,XorChannelSort,GradientColors,GradientColorsVertical,Bitwise_XOR_Average,NotEqual,ImageShiftUpLeft,GradientXorSelfScale,SmoothSourcePixel,StrobeBlend,FrameBars,Sort_Vertical_Horizontal,Sort_Vertical_Horizontal_Bitwise_XOR,Scalar_Average_Multiply,Scalar_Average,Total_Average,AlphaBlendImageXor,FlashWhite,FlashBlackAndWhite,GaussianBlend,RandomXor,RandomXorFlash,RandomAmountMedianBlur,SoftXor,SelfXorBlend,SelfXorDoubleFlash,SelfOrDoubleFlash,BlendRowCurvedSqrt,CycleShiftRGB,CycleShiftRandomRGB,CycleShiftRandomRGB_XorBlend,CycleShiftRandomAlphaBlend,VerticalColorBars,GradientLeftRight,GraidentUpDown,GradientLeftRightInOut,GradientUpDownInOut,Lines,ColorLines,WhiteLines,ThickWhiteLines,UseLineObject,TanAlphaGrid,MedianBlendAnimation,FibFlash,ScaleFlash,LeftLines,Curtain,RandomCurtain,CurtainVertical,RandomCurtainVertical,inOrder,inOrderBySecond,DarkenFilter,RandomFilterBySecond,ThreeRandom,inOrderAlpha,inOrderAlphaXor,SlideFilter,SlideFilterXor,RandomSlideFilter,SlideUpDown,SlideUpDownXor,SlideUpDownRandom,SlideSubFilter,SlideSubUpDownFilter,ParticleBlend,ParticleFlash,ExactImage,ParticleAlpha,BlendInAndOut,BlendScaleInAndOut,AcidGlitch,XorBackwards,LiquidFilter,MatrixXorAnd,XorAlpha,AlphaAcidTrails,SelfXorAverage,RandomXorBlend,RGBVerticalXor,RGBVerticalXorScale,RGBHorizontalXor,RGBHorizontalXorScale,FadeStrobe,RGBMirror,MirrorStrobe,AndStrobe,AndStrobeScale,AndPixelStrobe,AndOrXorStrobe,AndOrXorStrobeScale,FadeInAndOut,BrightStrobe,DarkStrobe,ParticleFast,RandomXorOpposite,StrobeTransform,InitBlend,MoveUpLeft,RandomStrobe,RandomBlur,Stuck,StuckStrobe,OrStrobe,LagBlend,SubFilter,AddFilter,BlendImageXor,BlendImageAround_Median,ImageBlendTransform,RGBTrails,RGBTrailsDark,RGBTrailsAlpha,RGBTrailsNegativeAlpha,MovementRGBTrails,RGBTrailsXor,DifferenceStrobe,BlackAndWhiteDifferenceStrobe,DifferenceXor,DifferenceRand,DifferenceBrightStrobe,PsycheTrails,FourSquare,EightSquare,DiagonalSquare,DiagonalSquareRandom,SquareStretchDown,SquareStretchRight,SquareStretchUp,SquareStretchLeft,DarkTrails,SoftFeedback,SoftFeedbackFrames,ResizeSoftFeedback,SoftFeedback8,SoftFeedbackFrames8,ResizeSoftFeedback8,ResizeSoftFeedbackSubFilter,SoftFeedbackRandFilter,SoftFeedback32,SoftFeedbackFrames32,ResizeSoftFeedback32,SoftFeedbackRandFilter32,SoftFeedbackSubFilter,SoftFeedbackResizeSubFilter,SoftFeedbackResize64,SoftFeedbackResizeSubFilter64,SoftFeedbackReszieSubFilter64_Negate,SoftFeedbackReszieSubFilter64_Mirror,HalfNegateStrobe,MedianBlurXor,NegateTrails,RandomGradient,RandomStrobeFlash,RandomMirror,RandomOther,RandomXorFilter,RandomMirrorBlend,RandomMirrorAlphaBlend,Bitwise_XOR_AlphaSubFilter,AlphaBlendSubFilter,GradientSubFilterXor,XorBlend_SubFilter,SmoothSubFilterAlphaBlend,SmoothSubFilterXorBlend,IntertwineSubFilter,RandBlend,EveryOther,EveryOtherSubFilter,SmoothRandomFilter,RandomFilterRandomTimes,RandomSubFilterRandomTimes,AddToFrameSubFilter,MirrorXor,MirrorXorAll,MirrorXorScale,EnergyMirror,SmoothSubFilter,EnergizeSubFilter,SmoothSubFilter16,EnergizeSubFilter16,EnergizeSubFilter32,SmoothSubFilter32,HalfAddSubFilter,HalfXorSubFilter,StaticXorBlend,PsycheSort,XorScale,ChannelMedianSubFilter,GaussianStrobe,StrobeSort,GlitchSortStrobe,Bitwise_XOR_Blend,Bitwise_XOR_Sort,Bitwise_OR_Blend,Bitwise_AND_Blend,BitwiseColorMatrix,PixelReverseXor,PixelatedSubFilterSort,SilverBlend,RandomPixelOrderSort,ImageXorAlpha,ImageAverageXor,PixelXorBlend,SelfAlphaScale,SelfScaleAlpha,RainbowXorBlend,FrameDifference,SmallDiffference,FadeBlend,FilteredDifferenceSubFilter,ExpandSquareSubFilter,ExpandSquareBlendSubFilter,ExpandSquareVerticalSubFilter,DarkImageMedianBlend,GammaDarken5,GammaDarken10,SelfAlphaScaleBlend,FadeBars,MirrorXorAlpha,MirrorEnergizeSubFilter,StrobeXor,IntertwinedMirror,BlurredMirror,ShadeRGB,InterRGB_SubFilter,InterSmoothSubFilter,InterRGB_Bars_XY,InterRGB_Bars_X,InterRGB_Bars_Y,StoredFramesAlphaBlend_SubFilter,BlendSubFilter,BlendAlphaSubFilter,ReverseFrameBlend,ReverseFrameBlendSwitch,DoubleRandomMirror,Blend_AlphaSubFilter,RandomBlendFilter,DoubleRandomBlendFilter,FlipBlendW,FlipBlendWH,FlipBlendH,FlipBlendAll,FrameMedianBlendSubFilter,FrameBlurSubFilter,ImageBlendSubFilter,ImageBlendXorSubFilter,ImageCollectionSubFilter,SelfScaleXorIncrease,Blend_RedGreenBlue,XorBlend_RedGreenBlue,BlendIncrease_RedGreenBlue,Blend_RedReenBlue_Dark,DarkModBlend,PictureBuzz,IncDifference,IncDifferenceAlpha,MirrorMedianBlend,SubFilterMedianBlend,DarkenBlend,DarkCollectionSubFilter,ChannelSort_NoBlend_Descending,ChannelSort_NoBlend_Ascending,Headrush,DarkSmooth_Filter,DarkSelfAlpha,FlipMedian,FlipMedianSubFilter,FlipMirror,FlipMirrorAverage,FlipMirrorSubFilter,ShuffleMedian,ShuffleRGB,ParticleSnow,RandomPixels,DarkRandomPixels,MedianBlurSubFilter,Bars,ShuffleAlpha,AlphaMorph,ShuffleSelf,
        NoFilter,
        BlendWithSource,plugin,custom,blendWithImage, triBlendWithImage,imageStrobe, imageDistraction,0};
    // draw functions
    DrawFunction *draw_func = draw_func_value;
    // number of filters
    int draw_max = 525;
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
    
    FilterType filters[] = {
        {"Self AlphaBlend",SelfAlphaBlend}, {"Self Scale", SelfScale}, {"StrobeEffect", StrobeEffect}, {"Blend #3", Blend3}, {"Negative Paradox", NegParadox}, {"ThoughtMode", ThoughtMode}, {"RandTriBlend", RandTriBlend}, {"Blank", Blank}, {"Tri",Tri}, {"Distort",Distort}, {"CDraw",CDraw}, {"Type",Type}, {"NewOne",NewOne}, {"Blend Fractal",blendFractal}, {"Blend Fractal Mood",blendFractalMood}, {"CosSinMultiply",cossinMultiply}, {"Color Accumlate1",colorAccumulate1}, {"Color Accumulate2",colorAccumulate2}, {"Color Accumulate3",colorAccumulate3}, {"Filter8",filter8}, {"Filter3",filter3}, {"Rainbow Blend",rainbowBlend}, {"Rand Blend",randBlend}, {"New Blend",newBlend}, {"Alpha Flame Filters",alphaFlame}, {"Pixel Scale",pixelScale}, {"PixelSort",pixelSort}, {"GlitchSort",glitchSort}, {"Random Filter",randomFilter}, {"Random Flash",randomFlash}, {"Blend with Image",imageBlend}, {"Blend with Image #2",imageBlendTwo}, {"Blend with Image #3",imageBlendThree}, {"Blend with Image #4",imageBlendFour}, {"GaussianBlur",GaussianBlur}, {"Median Blur",MedianBlur}, {"Blur Distortion",BlurDistortion}, {"Diamond Pattern",DiamondPattern}, {"MirrorBlend",MirrorBlend}, {"Pulse",Pulse}, {"Sideways Mirror",SidewaysMirror}, {"Mirror No Blend",MirrorNoBlend}, {"Sort Fuzz",SortFuzz}, {"Fuzz",Fuzz}, {"Double Vision",DoubleVision}, {"RGB Shift",RGBShift}, {"RGB Sep",RGBSep}, {"Graident Rainbow",GradientRainbow}, {"Gradient Rainbow Flash",GradientRainbowFlash}, {"Reverse",Reverse}, {"Scanlines",Scanlines}, {"TV Static",TVStatic}, {"Mirror Average",MirrorAverage}, {"Mirror Average Mix",MirrorAverageMix}, {"Mean",Mean}, {"Laplacian",Laplacian}, {"Bitwise_XOR",Bitwise_XOR}, {"Bitwise_AND",Bitwise_AND}, {"Bitwise_OR",Bitwise_OR}, {"Equalize",Equalize}, {"Channel Sort",ChannelSort}, {"Reverse_XOR",Reverse_XOR}, {"Combine Pixels",CombinePixels}, {"FlipTrip",FlipTrip}, {"Canny",Canny}, {"Boxes",Boxes}, {"Boxes Fade",BoxesFade}, {"Flash Black",FlashBlack}, {"SlideRGB",SlideRGB}, {"Side2Side",Side2Side}, {"Top2Bottom",Top2Bottom}, {"Strobe Red Then Green Then Blue",StrobeRedGreenBlue}, {"Blend_Angle",Blend_Angle}, {"Outward",Outward}, {"Outward Square",OutwardSquare}, {"ShiftPixels",ShiftPixels}, {"ShiftPixelsDown",ShiftPixelsDown}, {"XorMultiBlend",XorMultiBlend}, {"Bitwise_Rotate",BitwiseRotate}, {"Bitwise_Rotate Diff",BitwiseRotateDiff}, {"HPPD",HPPD}, {"FuzzyLines",FuzzyLines}, {"GradientLines",GradientLines}, {"GradientSelf",GradientSelf}, {"GradientSelfVertical",GradientSelfVertical}, {"GradientDown",GradientDown}, {"GraidentHorizontal",GraidentHorizontal}, {"GradientRGB",GradientRGB}, {"Inter",Inter}, {"UpDown",UpDown}, {"LeftRight",LeftRight}, {"StrobeScan",StrobeScan}, {"BlendedScanLines",BlendedScanLines}, {"GradientStripes",GradientStripes}, {"XorSine",XorSine}, {"SquareSwap",SquareSwap}, {"SquareSwap4x2",SquareSwap4x2}, {"SquareSwap8x4",SquareSwap8x4}, {"SquareSwap16x8",SquareSwap16x8}, {"SquareSwap64x32",SquareSwap64x32}, {"SquareBars",SquareBars}, {"SquareBars8",SquareBars8}, {"SquareSwapRand16x8",SquareSwapRand16x8}, {"SquareVertical8",SquareVertical8}, {"SquareVertical16",SquareVertical16}, {"SquareVertical_Roll",SquareVertical_Roll}, {"SquareSwapSort_Roll",SquareSwapSort_Roll}, {"SquareVertical_RollReverse",SquareVertical_RollReverse}, {"SquareSwapSort_RollReverse",SquareSwapSort_RollReverse}, {"Circular",Circular}, {"WhitePixel",WhitePixel}, {"FrameBlend",FrameBlend}, {"FrameBlendRGB",FrameBlendRGB}, {"TrailsFilter",TrailsFilter}, {"TrailsFilterIntense",TrailsFilterIntense}, {"TrailsFilterSelfAlpha",TrailsFilterSelfAlpha}, {"TrailsFilterXor",TrailsFilterXor}, {"ColorTrails",ColorTrails}, {"MoveRed",MoveRed}, {"MoveRGB",MoveRGB}, {"MoveRedGreenBlue",MoveRedGreenBlue}, {"BlurSim",BlurSim}, {"Block",Block}, {"BlockXor",BlockXor}, {"BlockScale",BlockScale}, {"BlockStrobe",BlockStrobe}, {"PrevFrameBlend",PrevFrameBlend}, {"Wave",Wave}, {"HighWave",HighWave}, {"VerticalSort",VerticalSort}, {"VerticalChannelSort",VerticalChannelSort}, {"HorizontalBlend",HorizontalBlend}, {"VerticalBlend",VerticalBlend}, {"OppositeBlend",OppositeBlend}, {"DiagonalLines",DiagonalLines}, {"HorizontalLines",HorizontalLines}, {"InvertedScanlines",InvertedScanlines}, {"Soft_Mirror",Soft_Mirror}, {"KanapaTrip",KanapaTrip}, {"ColorMorphing",ColorMorphing}, {"ScanSwitch",ScanSwitch}, {"ScanAlphaSwitch",ScanAlphaSwitch}, {"NegativeStrobe",NegativeStrobe}, {"XorAddMul",XorAddMul}, {"ParticleRelease",ParticleRelease}, {"BlendSwitch",BlendSwitch}, {"All Red",AllRed}, {"All Green",AllGreen}, {"All Blue",AllBlue}, {"LineRGB",LineRGB}, {"PixelRGB",PixelRGB}, {"BoxedRGB",BoxedRGB}, {"KruegerSweater",KruegerSweater}, {"RGBFlash",RGBFlash}, {"IncreaseBlendHorizontal",IncreaseBlendHorizontal}, {"BlendIncrease",BlendIncrease}, {"GradientReverse",GradientReverse}, {"GradientReverseVertical",GradientReverseVertical}, {"GradientReverseBox",GradientReverseBox}, {"GradientNewFilter",GradientNewFilter}, {"ReinterpretDouble",ReinterpretDouble}, {"ReinterpSelfScale",ReinterpSelfScale}, {"AverageLines",AverageLines}, {"ImageFile",ImageFile}, {"ImageXor",ImageXor}, {"ImageAlphaBlend",ImageAlphaBlend}, {"ColorRange",ColorRange}, {"ImageInter",ImageInter}, {"TrailsInter",TrailsInter}, {"TrailsBlend",TrailsBlend}, {"TrailsNegate",TrailsNegate}, {"InterReverse",InterReverse}, {"InterMirror",InterMirror}, {"InterFullMirror",InterFullMirror}, {"MirrorRGB",MirrorRGB}, {"RGBStatic1",RGBStatic1}, {"RGBStatic2",RGBStatic2}, {"VectorIncrease",VectorIncrease}, {"LineByLineReverse",LineByLineReverse}, {"RandomIntertwine",RandomIntertwine}, {"RandomFour",RandomFour}, {"BlendThree",BlendThree}, {"AcidTrails",AcidTrails}, {"RandomTwo",RandomTwo}, {"HorizontalTrailsInter",HorizontalTrailsInter}, {"Trails",Trails}, {"BlendTrails",BlendTrails}, {"Negate",Negate}, {"RandomFilteredSquare",RandomFilteredSquare}, {"ImageX",ImageX}, {"RandomQuads",RandomQuads}, {"QuadCosSinMultiply",QuadCosSinMultiply}, {"QuadRandomFilter",QuadRandomFilter}, {"RollRandom",RollRandom}, {"AverageRandom",AverageRandom}, {"HorizontalStripes",HorizontalStripes}, {"DiamondStrobe",DiamondStrobe}, {"SmoothTrails",SmoothTrails}, {"GridFilter8x",GridFilter8x}, {"GridFilter16x",GridFilter16x}, {"GridFilter8xBlend",GridFilter8xBlend}, {"GridRandom",GridRandom}, {"GridRandomPixel",GridRandomPixel}, {"Dual_SelfAlphaRainbow",Dual_SelfAlphaRainbow}, {"Dual_SelfAlphaBlur",Dual_SelfAlphaBlur}, {"SurroundPixelXor",SurroundPixelXor}, {"Darken",Darken}, {"WeakBlend",WeakBlend}, {"AverageVertical",AverageVertical}, {"RandomCollectionAverage",RandomCollectionAverage}, {"RandomCollectionAverageMax",RandomCollectionAverageMax}, {"SmoothTrailsSelfAlphaBlend",SmoothTrailsSelfAlphaBlend}, {"SmoothTrailsRainbowBlend",SmoothTrailsRainbowBlend}, {"MedianBlend",MedianBlend}, {"SmoothRandomImageBlend",SmoothRandomImageBlend}, {"SmoothImageAlphaBlend",SmoothImageAlphaBlend}, {"RandomAlphaBlend",RandomAlphaBlend}, {"RandomTwoFilterAlphaBlend",RandomTwoFilterAlphaBlend}, {"PixelatedSquare",PixelatedSquare}, {"AlphaBlendPosition",AlphaBlendPosition}, {"BlendRowAlpha",BlendRowAlpha}, {"BlendRow",BlendRow}, {"BlendRowByVar",BlendRowByVar}, {"BlendRowByDirection",BlendRowByDirection}, {"BlendAlphaXor",BlendAlphaXor}, {"SelfXorScale",SelfXorScale}, {"BitwiseXorScale",BitwiseXorScale}, {"XorTrails",XorTrails}, {"RainbowTrails",RainbowTrails}, {"NegativeTrails",NegativeTrails}, {"IntenseTrails",IntenseTrails}, {"SelfAlphaRGB",SelfAlphaRGB}, {"BlendImageOnOff",BlendImageOnOff}, {"XorSelfAlphaImage",XorSelfAlphaImage}, {"BitwiseXorStrobe",BitwiseXorStrobe}, {"AlphaBlendRandom",AlphaBlendRandom}, {"ChannelSortAlphaBlend",ChannelSortAlphaBlend}, {"XorChannelSort",XorChannelSort}, {"GradientColors",GradientColors}, {"GradientColorsVertical",GradientColorsVertical}, {"Bitwise_XOR_Average",Bitwise_XOR_Average}, {"NotEqual",NotEqual}, {"ImageShiftUpLeft",ImageShiftUpLeft}, {"GradientXorSelfScale",GradientXorSelfScale}, {"SmoothSourcePixel",SmoothSourcePixel}, {"StrobeBlend",StrobeBlend}, {"FrameBars",FrameBars}, {"Sort_Vertical_Horizontal",Sort_Vertical_Horizontal}, {"Sort_Vertical_Horizontal_Bitwise_XOR",Sort_Vertical_Horizontal_Bitwise_XOR}, {"Scalar_Average_Multiply",Scalar_Average_Multiply}, {"Scalar_Average",Scalar_Average}, {"Total_Average",Total_Average}, {"AlphaBlendImageXor",AlphaBlendImageXor}, {"FlashWhite",FlashWhite}, {"FlashBlackAndWhite",FlashBlackAndWhite}, {"GaussianBlend",GaussianBlend}, {"RandomXor",RandomXor}, {"RandomXorFlash",RandomXorFlash}, {"RandomAmountMedianBlur",RandomAmountMedianBlur}, {"SoftXor",SoftXor}, {"SelfXorBlend",SelfXorBlend}, {"SelfXorDoubleFlash",SelfXorDoubleFlash}, {"SelfOrDoubleFlash",SelfOrDoubleFlash}, {"BlendRowCurvedSqrt",BlendRowCurvedSqrt}, {"CycleShiftRGB",CycleShiftRGB}, {"CycleShiftRandomRGB",CycleShiftRandomRGB}, {"CycleShiftRandomRGB_XorBlend",CycleShiftRandomRGB_XorBlend}, {"CycleShiftRandomAlphaBlend",CycleShiftRandomAlphaBlend}, {"VerticalColorBars",VerticalColorBars}, {"GradientLeftRight",GradientLeftRight}, {"GraidentUpDown",GraidentUpDown}, {"GradientLeftRightInOut",GradientLeftRightInOut}, {"GradientUpDownInOut",GradientUpDownInOut}, {"Lines",Lines}, {"ColorLines",ColorLines}, {"WhiteLines",WhiteLines}, {"ThickWhiteLines",ThickWhiteLines}, {"UseLineObject",UseLineObject}, {"TanAlphaGrid",TanAlphaGrid}, {"MedianBlendAnimation",MedianBlendAnimation}, {"FibFlash",FibFlash}, {"ScaleFlash",ScaleFlash}, {"LeftLines",LeftLines}, {"Curtain",Curtain}, {"RandomCurtain",RandomCurtain}, {"CurtainVertical",CurtainVertical}, {"RandomCurtainVertical",RandomCurtainVertical}, {"inOrder",inOrder}, {"inOrderBySecond",inOrderBySecond}, {"DarkenFilter",DarkenFilter}, {"RandomFilterBySecond",RandomFilterBySecond}, {"ThreeRandom",ThreeRandom}, {"inOrderAlpha",inOrderAlpha}, {"inOrderAlphaXor",inOrderAlphaXor}, {"SlideFilter",SlideFilter}, {"SlideFilterXor",SlideFilterXor}, {"RandomSlideFilter",RandomSlideFilter}, {"SlideUpDown",SlideUpDown}, {"SlideUpDownXor",SlideUpDownXor}, {"SlideUpDownRandom",SlideUpDownRandom}, {"SlideSubFilter",SlideSubFilter}, {"SlideSubUpDownFilter",SlideSubUpDownFilter}, {"ParticleBlend",ParticleBlend}, {"ParticleFlash",ParticleFlash}, {"ExactImage",ExactImage}, {"ParticleAlpha",ParticleAlpha}, {"BlendInAndOut",BlendInAndOut}, {"BlendScaleInAndOut",BlendScaleInAndOut}, {"AcidGlitch",AcidGlitch}, {"XorBackwards",XorBackwards}, {"LiquidFilter",LiquidFilter}, {"MatrixXorAnd",MatrixXorAnd}, {"XorAlpha",XorAlpha}, {"AlphaAcidTrails",AlphaAcidTrails}, {"SelfXorAverage",SelfXorAverage}, {"RandomXorBlend",RandomXorBlend}, {"RGBVerticalXor",RGBVerticalXor}, {"RGBVerticalXorScale",RGBVerticalXorScale}, {"RGBHorizontalXor",RGBHorizontalXor}, {"RGBHorizontalXorScale",RGBHorizontalXorScale}, {"FadeStrobe",FadeStrobe}, {"RGBMirror",RGBMirror}, {"MirrorStrobe",MirrorStrobe}, {"AndStrobe",AndStrobe}, {"AndStrobeScale",AndStrobeScale}, {"AndPixelStrobe",AndPixelStrobe}, {"AndOrXorStrobe",AndOrXorStrobe}, {"AndOrXorStrobeScale",AndOrXorStrobeScale}, {"FadeInAndOut",FadeInAndOut}, {"BrightStrobe",BrightStrobe}, {"DarkStrobe",DarkStrobe}, {"ParticleFast",ParticleFast}, {"RandomXorOpposite",RandomXorOpposite}, {"StrobeTransform",StrobeTransform}, {"InitBlend",InitBlend}, {"MoveUpLeft",MoveUpLeft}, {"RandomStrobe",RandomStrobe}, {"RandomBlur",RandomBlur}, {"Stuck",Stuck}, {"StuckStrobe",StuckStrobe}, {"OrStrobe",OrStrobe}, {"LagBlend",LagBlend}, {"SubFilter",SubFilter}, {"AddFilter",AddFilter}, {"BlendImageXor",BlendImageXor}, {"BlendImageAround_Median",BlendImageAround_Median}, {"ImageBlendTransform",ImageBlendTransform}, {"RGBTrails",RGBTrails}, {"RGBTrailsDark",RGBTrailsDark}, {"RGBTrailsAlpha",RGBTrailsAlpha}, {"RGBTrailsNegativeAlpha",RGBTrailsNegativeAlpha}, {"MovementRGBTrails",MovementRGBTrails}, {"RGBTrailsXor",RGBTrailsXor}, {"DifferenceStrobe",DifferenceStrobe}, {"BlackAndWhiteDifferenceStrobe",BlackAndWhiteDifferenceStrobe}, {"DifferenceXor",DifferenceXor}, {"DifferenceRand",DifferenceRand}, {"DifferenceBrightStrobe",DifferenceBrightStrobe}, {"PsycheTrails",PsycheTrails}, {"FourSquare",FourSquare}, {"EightSquare",EightSquare}, {"DiagonalSquare",DiagonalSquare}, {"DiagonalSquareRandom",DiagonalSquareRandom}, {"SquareStretchDown",SquareStretchDown}, {"SquareStretchRight",SquareStretchRight}, {"SquareStretchUp",SquareStretchUp}, {"SquareStretchLeft",SquareStretchLeft}, {"DarkTrails",DarkTrails}, {"SoftFeedback",SoftFeedback}, {"SoftFeedbackFrames",SoftFeedbackFrames}, {"ResizeSoftFeedback",ResizeSoftFeedback}, {"SoftFeedback8",SoftFeedback8}, {"SoftFeedbackFrames8",SoftFeedbackFrames8}, {"ResizeSoftFeedback8",ResizeSoftFeedback8}, {"ResizeSoftFeedbackSubFilter",ResizeSoftFeedbackSubFilter}, {"SoftFeedbackRandFilter",SoftFeedbackRandFilter}, {"SoftFeedback32",SoftFeedback32}, {"SoftFeedbackFrames32",SoftFeedbackFrames32}, {"ResizeSoftFeedback32",ResizeSoftFeedback32}, {"SoftFeedbackRandFilter32",SoftFeedbackRandFilter32}, {"SoftFeedbackSubFilter",SoftFeedbackSubFilter}, {"SoftFeedbackResizeSubFilter",SoftFeedbackResizeSubFilter},{"SoftFeedbackResize64",SoftFeedbackResize64}, {"SoftFeedbackResizeSubFilter64",SoftFeedbackResizeSubFilter64}, {"SoftFeedbackReszieSubFilter64_Negate",SoftFeedbackReszieSubFilter64_Negate}, {"SoftFeedbackReszieSubFilter64_Mirror",SoftFeedbackReszieSubFilter64_Mirror}, {"HalfNegateStrobe",HalfNegateStrobe},{"MedianBlurXor", MedianBlurXor},{"NegateTrails", NegateTrails},{"RandomGradient", RandomGradient},{"RandomStrobeFlash", RandomStrobeFlash}, {"RandomMirror", RandomMirror},{"RandomOther", RandomOther},{"RandomXorFilter", RandomXorFilter},{"RandomMirrorBlend", RandomMirrorBlend},{"RandomMirrorAlphaBlend", RandomMirrorAlphaBlend}, {"Bitwise_XOR_AlphaSubFilter", Bitwise_XOR_AlphaSubFilter}, {"AlphaBlendSubFilter", AlphaBlendSubFilter},{"GradientSubFilterXor", GradientSubFilterXor},{"XorBlend_SubFilter", XorBlend_SubFilter},{"SmoothSubFilterAlphaBlend", SmoothSubFilterAlphaBlend},{"SmoothSubFilterXorBlend", SmoothSubFilterXorBlend},{"IntertwineSubFilter", IntertwineSubFilter},{"RandBlend", RandBlend},{"EveryOther", EveryOther},{"EveryOtherSubFilter", EveryOtherSubFilter},{"SmoothRandomFilter", SmoothRandomFilter},{"RandomFilterRandomTimes", RandomFilterRandomTimes},{"RandomSubFilterRandomTimes", RandomSubFilterRandomTimes},{"AddToFrameSubFilter",AddToFrameSubFilter},{"MirrorXor", MirrorXor},{"MirrorXorAll", MirrorXorAll}, {"MirrorXorScale", MirrorXorScale},{"EnergyMirror",EnergyMirror},{"SmoothSubFilter", SmoothSubFilter},{"EnergizeSubFilter", EnergizeSubFilter},{"SmoothSubFilter16", SmoothSubFilter16},{"EnergizeSubFilter16", EnergizeSubFilter16},{"EnergizeSubFilter32", EnergizeSubFilter32},{"SmoothSubFilter32", SmoothSubFilter32},{"HalfAddSubFilter", HalfAddSubFilter},{"HalfXorSubFilter", HalfXorSubFilter},{"StaticXorBlend", StaticXorBlend},{"PsycheSort", PsycheSort},{"XorScale", XorScale},{"ChannelMedianSubFilter", ChannelMedianSubFilter},{"GaussianStrobe", GaussianStrobe},{"StrobeSort", StrobeSort},{"GlitchSortStrobe", GlitchSortStrobe},{"Bitwise_XOR_Blend",Bitwise_XOR_Blend},{"Bitwise_XOR_Sort", Bitwise_XOR_Sort},{"Bitwise_OR_Blend", Bitwise_OR_Blend},{"Bitwise_AND_Blend", Bitwise_AND_Blend},{"BitwiseColorMatrix", BitwiseColorMatrix},{"PixelReverseXor", PixelReverseXor},{"PixelatedSubFilterSort", PixelatedSubFilterSort},{"SilverBlend", SilverBlend},{"RandomPixelOrderSort", RandomPixelOrderSort},{"ImageXorAlpha", ImageXorAlpha},{"ImageAverageXor", ImageAverageXor},{"PixelXorBlend", PixelXorBlend},{"SelfAlphaScale", SelfAlphaScale},{"SelfScaleAlpha", SelfScaleAlpha},{"RainbowXorBlend", RainbowXorBlend},{"FrameDifference", FrameDifference},{"SmallDiffference", SmallDiffference},{"FadeBlend", FadeBlend},{"FilteredDifferenceSubFilter", FilteredDifferenceSubFilter},{"ExpandSquareSubFilter", ExpandSquareSubFilter}, {"ExpandSquareBlendSubFilter", ExpandSquareBlendSubFilter},{"ExpandSquareVerticalSubFilter", ExpandSquareVerticalSubFilter},{"DarkImageMedianBlend", DarkImageMedianBlend},{"GammaDarken5", GammaDarken5},{"GammaDarken10", GammaDarken10},{"SelfAlphaScaleBlend", SelfAlphaScaleBlend},{"FadeBars", FadeBars},{"MirrorXorAlpha", MirrorXorAlpha},{"MirrorEnergizeSubFilter", MirrorEnergizeSubFilter},{"StrobeXor", StrobeXor},{"IntertwinedMirror", InterMirror}, {"BlurredMirror", BlurredMirror},{"ShadeRGB", ShadeRGB},{"InterRGB_SubFilter", InterRGB_SubFilter},{"InterSmoothSubFilter", InterSmoothSubFilter},{"InterRGB_Bars_XY", InterRGB_Bars_XY},{"InterRGB_Bars_X",InterRGB_Bars_X},{"InterRGB_Bars_Y", InterRGB_Bars_Y},{"StoredFramesAlphaBlend_SubFilter", StoredFramesAlphaBlend_SubFilter},{"BlendSubFilter", BlendSubFilter},{"BlendAlphaSubFilter", BlendAlphaSubFilter},{"ReverseFrameBlend", ReverseFrameBlend},{"ReverseFrameBlendSwitch", ReverseFrameBlendSwitch},{"DoubleRandomMirror", DoubleRandomMirror},{"Blend_AlphaSubFilter", Blend_AlphaSubFilter},{"RandomBlendFilter", RandomBlendFilter},{"DoubleRandomBlendFilter", DoubleRandomBlendFilter},{"FlipBlendW", FlipBlendW},{"FlipBlendWH", FlipBlendWH}, {"FlipBlendH", FlipBlendH}, {"FlipBlendAll", FlipBlendAll},{"FrameMedianBlendSubFilter", FrameMedianBlendSubFilter}, {"FrameBlurSubFilter", FrameBlurSubFilter},{"ImageBlendSubFilter", ImageBlendSubFilter},{"ImageBlendXorSubFilter", ImageBlendXorSubFilter},{"ImageCollectionSubFilter", ImageCollectionSubFilter},{"SelfScaleXorIncrease", SelfScaleXorIncrease},{"Blend_RedGreenBlue", Blend_RedGreenBlue},{"XorBlend_RedGreenBlue", XorBlend_RedGreenBlue},{"BlendIncrease_RedGreenBlue", BlendIncrease_RedGreenBlue},{"Blend_RedReenBlue_Dark", Blend_RedReenBlue_Dark},{"DarkModBlend", DarkModBlend}, {"PictureBuzz", PictureBuzz},{"IncDifference", IncDifference},{"IncDifferenceAlpha", IncDifferenceAlpha},{"MirrorMedianBlend", MirrorMedianBlend},{"SubFilterMedianBlend", SubFilterMedianBlend},{"DarkenBlend", DarkenBlend},{"DarkCollectionSubFilter", DarkCollectionSubFilter},{"ChannelSort_NoBlend_Descending", ChannelSort_NoBlend_Descending}, {"ChannelSort_NoBlend_Ascending", ChannelSort_NoBlend_Ascending},{"Headrush", Headrush},{"DarkSmooth_Filter", DarkSmooth_Filter},{"DarkSelfAlpha", DarkSelfAlpha},{"FlipMedian", FlipMedian},{"FlipMedianSubFilter", FlipMedianSubFilter},{"FlipMirror", FlipMirror},{"FlipMirrorAverage", FlipMirrorAverage},{"FlipMirrorSubFilter", FlipMirrorSubFilter},{"ShuffleMedian", ShuffleMedian},{"ShuffleRGB", ShuffleRGB},{"ParticleSnow", ParticleSnow},{"RandomPixels", RandomPixels},{"DarkRandomPixels", DarkRandomPixels},{"MedianBlurSubFilter", MedianBlurSubFilter},{"Bars", Bars},{"ShuffleAlpha", ShuffleAlpha},{"AlphaMorph", AlphaMorph},{"ShuffleSelf", ShuffleSelf},
        	{"No Filter",NoFilter},
        	{"Blend with Source",BlendWithSource}, {"Plugin",plugin}, {"Custom",custom}, {"Blend With Image #1",blendWithImage}, {"TriBlend with Image", triBlendWithImage}, {"Image Strobe",imageStrobe}};
      
    FilterType *filter_array = filters;
    FilterType filterByIndex(const int &num) {
        return filter_array[num];
    }
    FilterType filterByString(const std::string &num) {
        return filter_array[ac::filter_map[num]];
    }
}

cv::Mat blend_image, color_image;
bool blend_set = false;
bool colorkey_set = false;

// return version info
std::string ac::getVersion() {
    return version;
}
// be sure to call this on startup
void ac::fill_filter_map() {
    for(int i = 0; i < ac::draw_max; ++i)
        filter_map[draw_strings[i]] = i;
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

bool ac::CallFilter(int index, cv::Mat &frame) {
    if(index >= 0 && index < ac::draw_max) {
        filterByIndex(index).second(frame);
        return true;
    }
    return false;
}

bool ac::CallFilter(const std::string &name, cv::Mat &frame) {
    int index = ac::filter_map[name];
    if(index >= 0 && index < ac::draw_max) {
        filterByString(name).second(frame);
        return true;
    }
    return false;
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

