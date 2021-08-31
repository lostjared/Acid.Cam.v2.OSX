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
// Acid Cam namespace
namespace ac {
#if defined(__APPLE__)
    const std::string version="2.80.4 (macOS)";
#elif defined(__linux__)
    const std::string version="2.80.4 (Linux)";
#elif defined(_WIN32)
    const std::string version="2.80.4 (Windows)";
#else
    const std::string version="2.80.4 (Generic)";
#endif
    std::mutex col_lock;
    bool swapColorOn = true;
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
    std::vector<std::string> draw_strings = buildDrawStrings();
    // draw strings
    //std::string *draw_strings = draw_strings_value;
    // draw strings
    // filter callback functions
    // number of filters
    int draw_max = static_cast<int>(draw_strings.size())-1;
    int getFilterCount() {
        return draw_max-5;
    }
    // variables
    double translation_variable = 0.001f, pass2_alpha = 0.75f;
    // swap colors inline function
    std::unordered_map<std::string, int> filter_map;
    bool color_map_set = false;
    DrawFunction custom_callback = 0;
    DrawFunction plugin_func = 0;
    int colors[3] = {rand()%255, rand()%255, rand()%255};
    PROC_MODE_TYPE proc_mode = PROC_MODE_TYPE(0);
    bool reset_filter = false;
    double alpha_increase = 0;
    bool syphon_in_enabled = false;
    bool syphon_in_changed = false;

    
    FilterType filters[] = { {"Self AlphaBlend",SelfAlphaBlend}, {"Self Scale", SelfScale}, {"StrobeEffect", StrobeEffect}, {"Blend #3", Blend3}, {"Negative Paradox", NegParadox}, {"ThoughtMode", ThoughtMode}, {"RandTriBlend", RandTriBlend}, {"Blank", Blank}, {"Tri",Tri}, {"Distort",Distort}, {"CDraw",CDraw}, {"Type",Type}, {"NewOne",NewOne}, {"Blend Fractal",blendFractal}, {"Blend Fractal Mood",blendFractalMood}, {"CosSinMultiply",cossinMultiply}, {"Color Accumlate1",colorAccumulate1}, {"Color Accumulate2",colorAccumulate2}, {"Color Accumulate3",colorAccumulate3}, {"Filter8",filter8}, {"Filter3",filter3}, {"Rainbow Blend",rainbowBlend}, {"Rand Blend",randBlend}, {"New Blend",newBlend}, {"Alpha Flame Filters",alphaFlame}, {"Pixel Scale",pixelScale}, {"PixelSort",pixelSort}, {"GlitchSort",glitchSort}, {"Random Filter",randomFilter}, {"Random Flash",randomFlash}, {"Blend with Image",imageBlend}, {"Blend with Image #2",imageBlendTwo}, {"Blend with Image #3",imageBlendThree}, {"Blend with Image #4",imageBlendFour}, {"GaussianBlur",GaussianBlur}, {"Median Blur",MedianBlur}, {"Blur Distortion",BlurDistortion}, {"Diamond Pattern",DiamondPattern}, {"MirrorBlend",MirrorBlend}, {"Pulse",Pulse}, {"Sideways Mirror",SidewaysMirror}, {"Mirror No Blend",MirrorNoBlend}, {"Sort Fuzz",SortFuzz}, {"Fuzz",Fuzz}, {"Double Vision",DoubleVision}, {"RGB Shift",RGBShift}, {"RGB Sep",RGBSep}, {"Graident Rainbow",GradientRainbow}, {"Gradient Rainbow Flash",GradientRainbowFlash}, {"Reverse",Reverse}, {"Scanlines",Scanlines}, {"TV Static",TVStatic}, {"Mirror Average",MirrorAverage}, {"Mirror Average Mix",MirrorAverageMix}, {"Mean",Mean}, {"Laplacian",Laplacian}, {"Bitwise_XOR",Bitwise_XOR}, {"Bitwise_AND",Bitwise_AND}, {"Bitwise_OR",Bitwise_OR}, {"Equalize",Equalize}, {"Channel Sort",ChannelSort}, {"Reverse_XOR",Reverse_XOR}, {"Combine Pixels",CombinePixels}, {"FlipTrip",FlipTrip}, {"Canny",Canny}, {"Boxes",Boxes}, {"Boxes Fade",BoxesFade}, {"Flash Black",FlashBlack}, {"SlideRGB",SlideRGB}, {"Side2Side",Side2Side}, {"Top2Bottom",Top2Bottom}, {"Strobe Red Then Green Then Blue",StrobeRedGreenBlue}, {"Blend_Angle",Blend_Angle}, {"Outward",Outward}, {"Outward Square",OutwardSquare}, {"ShiftPixels",ShiftPixels}, {"ShiftPixelsDown",ShiftPixelsDown}, {"XorMultiBlend",XorMultiBlend}, {"Bitwise_Rotate",BitwiseRotate}, {"Bitwise_Rotate Diff",BitwiseRotateDiff}, {"HPPD",HPPD}, {"FuzzyLines",FuzzyLines}, {"GradientLines",GradientLines}, {"GradientSelf",GradientSelf}, {"GradientSelfVertical",GradientSelfVertical}, {"GradientDown",GradientDown}, {"GraidentHorizontal",GraidentHorizontal}, {"GradientRGB",GradientRGB}, {"Inter",Inter}, {"UpDown",UpDown}, {"LeftRight",LeftRight}, {"StrobeScan",StrobeScan}, {"BlendedScanLines",BlendedScanLines}, {"GradientStripes",GradientStripes}, {"XorSine",XorSine}, {"SquareSwap",SquareSwap}, {"SquareSwap4x2",SquareSwap4x2}, {"SquareSwap8x4",SquareSwap8x4}, {"SquareSwap16x8",SquareSwap16x8}, {"SquareSwap64x32",SquareSwap64x32}, {"SquareBars",SquareBars}, {"SquareBars8",SquareBars8}, {"SquareSwapRand16x8",SquareSwapRand16x8}, {"SquareVertical8",SquareVertical8}, {"SquareVertical16",SquareVertical16}, {"SquareVertical_Roll",SquareVertical_Roll}, {"SquareSwapSort_Roll",SquareSwapSort_Roll}, {"SquareVertical_RollReverse",SquareVertical_RollReverse}, {"SquareSwapSort_RollReverse",SquareSwapSort_RollReverse}, {"Circular",Circular}, {"WhitePixel",WhitePixel}, {"FrameBlend",FrameBlend}, {"FrameBlendRGB",FrameBlendRGB}, {"TrailsFilter",TrailsFilter}, {"TrailsFilterIntense",TrailsFilterIntense}, {"TrailsFilterSelfAlpha",TrailsFilterSelfAlpha}, {"TrailsFilterXor",TrailsFilterXor}, {"ColorTrails",ColorTrails}, {"MoveRed",MoveRed}, {"MoveRGB",MoveRGB}, {"MoveRedGreenBlue",MoveRedGreenBlue}, {"BlurSim",BlurSim}, {"Block",Block}, {"BlockXor",BlockXor}, {"BlockScale",BlockScale}, {"BlockStrobe",BlockStrobe}, {"PrevFrameBlend",PrevFrameBlend}, {"Wave",Wave}, {"HighWave",HighWave}, {"VerticalSort",VerticalSort}, {"VerticalChannelSort",VerticalChannelSort}, {"HorizontalBlend",HorizontalBlend}, {"VerticalBlend",VerticalBlend}, {"OppositeBlend",OppositeBlend}, {"DiagonalLines",DiagonalLines}, {"HorizontalLines",HorizontalLines}, {"InvertedScanlines",InvertedScanlines}, {"Soft_Mirror",Soft_Mirror}, {"KanapaTrip",KanapaTrip}, {"ColorMorphing",ColorMorphing}, {"ScanSwitch",ScanSwitch}, {"ScanAlphaSwitch",ScanAlphaSwitch}, {"NegativeStrobe",NegativeStrobe}, {"XorAddMul",XorAddMul}, {"ParticleRelease",ParticleRelease}, {"BlendSwitch",BlendSwitch}, {"All Red",AllRed}, {"All Green",AllGreen}, {"All Blue",AllBlue}, {"LineRGB",LineRGB}, {"PixelRGB",PixelRGB}, {"BoxedRGB",BoxedRGB}, {"KruegerSweater",KruegerSweater}, {"RGBFlash",RGBFlash}, {"IncreaseBlendHorizontal",IncreaseBlendHorizontal}, {"BlendIncrease",BlendIncrease}, {"GradientReverse",GradientReverse}, {"GradientReverseVertical",GradientReverseVertical}, {"GradientReverseBox",GradientReverseBox}, {"GradientNewFilter",GradientNewFilter}, {"ReinterpretDouble",ReinterpretDouble}, {"ReinterpSelfScale",ReinterpSelfScale}, {"AverageLines",AverageLines}, {"ImageFile",ImageFile}, {"ImageXor",ImageXor}, {"ImageAlphaBlend",ImageAlphaBlend}, {"ColorRange",ColorRange}, {"ImageInter",ImageInter}, {"TrailsInter",TrailsInter}, {"TrailsBlend",TrailsBlend}, {"TrailsNegate",TrailsNegate}, {"InterReverse",InterReverse}, {"InterMirror",InterMirror}, {"InterFullMirror",InterFullMirror}, {"MirrorRGB",MirrorRGB}, {"RGBStatic1",RGBStatic1}, {"RGBStatic2",RGBStatic2}, {"VectorIncrease",VectorIncrease}, {"LineByLineReverse",LineByLineReverse}, {"RandomIntertwine",RandomIntertwine}, {"RandomFour",RandomFour}, {"BlendThree",BlendThree}, {"AcidTrails",AcidTrails}, {"RandomTwo",RandomTwo}, {"HorizontalTrailsInter",HorizontalTrailsInter}, {"Trails",Trails}, {"BlendTrails",BlendTrails}, {"Negate",Negate}, {"RandomFilteredSquare",RandomFilteredSquare}, {"ImageX",ImageX}, {"RandomQuads",RandomQuads}, {"QuadCosSinMultiply",QuadCosSinMultiply}, {"QuadRandomFilter",QuadRandomFilter}, {"RollRandom",RollRandom}, {"AverageRandom",AverageRandom}, {"HorizontalStripes",HorizontalStripes}, {"DiamondStrobe",DiamondStrobe}, {"SmoothTrails",SmoothTrails}, {"GridFilter8x",GridFilter8x}, {"GridFilter16x",GridFilter16x}, {"GridFilter8xBlend",GridFilter8xBlend}, {"GridRandom",GridRandom}, {"GridRandomPixel",GridRandomPixel}, {"Dual_SelfAlphaRainbow",Dual_SelfAlphaRainbow}, {"Dual_SelfAlphaBlur",Dual_SelfAlphaBlur}, {"SurroundPixelXor",SurroundPixelXor}, {"Darken",Darken}, {"WeakBlend",WeakBlend}, {"AverageVertical",AverageVertical}, {"RandomCollectionAverage",RandomCollectionAverage}, {"RandomCollectionAverageMax",RandomCollectionAverageMax}, {"SmoothTrailsSelfAlphaBlend",SmoothTrailsSelfAlphaBlend}, {"SmoothTrailsRainbowBlend",SmoothTrailsRainbowBlend}, {"MedianBlend",MedianBlend}, {"SmoothRandomImageBlend",SmoothRandomImageBlend}, {"SmoothImageAlphaBlend",SmoothImageAlphaBlend}, {"RandomAlphaBlend",RandomAlphaBlend}, {"RandomTwoFilterAlphaBlend",RandomTwoFilterAlphaBlend}, {"PixelatedSquare",PixelatedSquare}, {"AlphaBlendPosition",AlphaBlendPosition}, {"BlendRowAlpha",BlendRowAlpha}, {"BlendRow",BlendRow}, {"BlendRowByVar",BlendRowByVar}, {"BlendRowByDirection",BlendRowByDirection}, {"BlendAlphaXor",BlendAlphaXor}, {"SelfXorScale",SelfXorScale}, {"BitwiseXorScale",BitwiseXorScale}, {"XorTrails",XorTrails}, {"RainbowTrails",RainbowTrails}, {"NegativeTrails",NegativeTrails}, {"IntenseTrails",IntenseTrails}, {"SelfAlphaRGB",SelfAlphaRGB}, {"BlendImageOnOff",BlendImageOnOff}, {"XorSelfAlphaImage",XorSelfAlphaImage}, {"BitwiseXorStrobe",BitwiseXorStrobe}, {"AlphaBlendRandom",AlphaBlendRandom}, {"ChannelSortAlphaBlend",ChannelSortAlphaBlend}, {"XorChannelSort",XorChannelSort}, {"GradientColors",GradientColors}, {"GradientColorsVertical",GradientColorsVertical}, {"Bitwise_XOR_Average",Bitwise_XOR_Average}, {"ImageShiftMatrixLeft",ImageShiftMatrixLeft}, {"GradientXorSelfScale",GradientXorSelfScale}, {"SmoothSourcePixel",SmoothSourcePixel}, {"StrobeBlend",StrobeBlend}, {"FrameBars",FrameBars}, {"Sort_Vertical_Horizontal",Sort_Vertical_Horizontal}, {"Sort_Vertical_Horizontal_Bitwise_XOR",Sort_Vertical_Horizontal_Bitwise_XOR}, {"Scalar_Average_Multiply",Scalar_Average_Multiply}, {"Scalar_Average",Scalar_Average}, {"Total_Average",Total_Average}, {"AlphaBlendImageXor",AlphaBlendImageXor}, {"FlashWhite",FlashWhite}, {"FlashBlackAndWhite",FlashBlackAndWhite}, {"GaussianBlend",GaussianBlend}, {"RandomXor",RandomXor}, {"RandomXorFlash",RandomXorFlash}, {"RandomAmountMedianBlur",RandomAmountMedianBlur}, {"SoftXor",SoftXor}, {"SelfXorBlend",SelfXorBlend}, {"SelfXorDoubleFlash",SelfXorDoubleFlash}, {"SelfOrDoubleFlash",SelfOrDoubleFlash}, {"BlendRowCurvedSqrt",BlendRowCurvedSqrt}, {"CycleShiftRGB",CycleShiftRGB}, {"CycleShiftRandomRGB",CycleShiftRandomRGB}, {"CycleShiftRandomRGB_XorBlend",CycleShiftRandomRGB_XorBlend}, {"CycleShiftRandomAlphaBlend",CycleShiftRandomAlphaBlend}, {"VerticalColorBars",VerticalColorBars}, {"GradientLeftRight",GradientLeftRight}, {"GraidentUpDown",GraidentUpDown}, {"GradientLeftRightInOut",GradientLeftRightInOut}, {"GradientUpDownInOut",GradientUpDownInOut}, {"Lines",Lines}, {"ColorLines",ColorLines}, {"WhiteLines",WhiteLines}, {"ThickWhiteLines",ThickWhiteLines}, {"UseLineObject",UseLineObject}, {"TanAlphaGrid",TanAlphaGrid}, {"MedianBlendAnimation",MedianBlendAnimation}, {"FibFlash",FibFlash}, {"ScaleFlash",ScaleFlash}, {"Curtain",Curtain}, {"RandomCurtain",RandomCurtain}, {"CurtainVertical",CurtainVertical}, {"RandomCurtainVertical",RandomCurtainVertical}, {"inOrder",inOrder}, {"inOrderBySecond",inOrderBySecond}, {"DarkenFilter",DarkenFilter}, {"RandomFilterBySecond",RandomFilterBySecond}, {"ThreeRandom",ThreeRandom}, {"inOrderAlpha",inOrderAlpha}, {"inOrderAlphaXor",inOrderAlphaXor}, {"SlideFilter",SlideFilter}, {"SlideFilterXor",SlideFilterXor}, {"RandomSlideFilter",RandomSlideFilter}, {"SlideUpDown",SlideUpDown}, {"SlideUpDownXor",SlideUpDownXor}, {"SlideUpDownRandom",SlideUpDownRandom}, {"SlideSubFilter",SlideSubFilter}, {"SlideSubUpDownFilter",SlideSubUpDownFilter}, {"ParticleBlend",ParticleBlend}, {"ParticleFlash",ParticleFlash}, {"ExactImage",ExactImage}, {"ParticleAlpha",ParticleAlpha}, {"BlendInAndOut",BlendInAndOut}, {"BlendScaleInAndOut",BlendScaleInAndOut}, {"AcidGlitch",AcidGlitch}, {"XorBackwards",XorBackwards}, {"LiquidFilter",LiquidFilter}, {"MatrixXorAnd",MatrixXorAnd}, {"XorAlpha",XorAlpha}, {"AlphaAcidTrails",AlphaAcidTrails}, {"SelfXorAverage",SelfXorAverage}, {"RandomXorBlend",RandomXorBlend}, {"RGBVerticalXor",RGBVerticalXor}, {"RGBVerticalXorScale",RGBVerticalXorScale}, {"RGBHorizontalXor",RGBHorizontalXor}, {"RGBHorizontalXorScale",RGBHorizontalXorScale}, {"FadeStrobe",FadeStrobe}, {"RGBMirror",RGBMirror}, {"MirrorStrobe",MirrorStrobe}, {"AndStrobe",AndStrobe}, {"AndStrobeScale",AndStrobeScale}, {"AndPixelStrobe",AndPixelStrobe}, {"AndOrXorStrobe",AndOrXorStrobe}, {"AndOrXorStrobeScale",AndOrXorStrobeScale}, {"FadeInAndOut",FadeInAndOut}, {"BrightStrobe",BrightStrobe}, {"DarkStrobe",DarkStrobe}, {"ParticleFast",ParticleFast}, {"RandomXorOpposite",RandomXorOpposite}, {"StrobeTransform",StrobeTransform}, {"InitBlend",InitBlend}, {"MoveUpLeft",MoveUpLeft}, {"RandomStrobe",RandomStrobe}, {"RandomBlur",RandomBlur}, {"Stuck",Stuck}, {"StuckStrobe",StuckStrobe}, {"OrStrobe",OrStrobe}, {"LagBlend",LagBlend}, {"SubFilter",SubFilter}, {"BlendImageXor",BlendImageXor}, {"BlendImageAround_Median",BlendImageAround_Median}, {"ImageBlendTransform",ImageBlendTransform}, {"RGBTrails",RGBTrails}, {"RGBTrailsDark",RGBTrailsDark}, {"RGBTrailsAlpha",RGBTrailsAlpha}, {"RGBTrailsNegativeAlpha",RGBTrailsNegativeAlpha}, {"MovementRGBTrails",MovementRGBTrails}, {"RGBTrailsXor",RGBTrailsXor}, {"DifferenceStrobe",DifferenceStrobe}, {"BlackAndWhiteDifferenceStrobe",BlackAndWhiteDifferenceStrobe}, {"DifferenceXor",DifferenceXor}, {"DifferenceRand",DifferenceRand}, {"DifferenceBrightStrobe",DifferenceBrightStrobe}, {"PsycheTrails",PsycheTrails}, {"FourSquare",FourSquare}, {"EightSquare",EightSquare}, {"DiagonalSquare",DiagonalSquare}, {"DiagonalSquareRandom",DiagonalSquareRandom}, {"SquareStretchDown",SquareStretchDown}, {"SquareStretchRight",SquareStretchRight}, {"SquareStretchUp",SquareStretchUp}, {"SquareStretchLeft",SquareStretchLeft}, {"DarkTrails",DarkTrails}, {"SoftFeedback",SoftFeedback}, {"SoftFeedbackFrames",SoftFeedbackFrames}, {"ResizeSoftFeedback",ResizeSoftFeedback}, {"SoftFeedback8",SoftFeedback8}, {"SoftFeedbackFrames8",SoftFeedbackFrames8}, {"ResizeSoftFeedback8",ResizeSoftFeedback8}, {"ResizeSoftFeedbackSubFilter",ResizeSoftFeedbackSubFilter}, {"SoftFeedbackRandFilter",SoftFeedbackRandFilter}, {"SoftFeedback32",SoftFeedback32}, {"SoftFeedbackFrames32",SoftFeedbackFrames32}, {"ResizeSoftFeedback32",ResizeSoftFeedback32}, {"SoftFeedbackRandFilter32",SoftFeedbackRandFilter32}, {"SoftFeedbackSubFilter",SoftFeedbackSubFilter}, {"SoftFeedbackResizeSubFilter",SoftFeedbackResizeSubFilter},{"SoftFeedbackResize64",SoftFeedbackResize64}, {"SoftFeedbackResizeSubFilter64",SoftFeedbackResizeSubFilter64}, {"SoftFeedbackReszieSubFilter64_Negate",SoftFeedbackReszieSubFilter64_Negate}, {"SoftFeedbackReszieSubFilter64_Mirror",SoftFeedbackReszieSubFilter64_Mirror}, {"HalfNegateStrobe",HalfNegateStrobe},{"MedianBlurXor", MedianBlurXor},{"NegateTrails", NegateTrails},{"RandomGradient", RandomGradient},{"RandomStrobeFlash", RandomStrobeFlash}, {"RandomMirror", RandomMirror},{"RandomOther", RandomOther},{"RandomXorFilter", RandomXorFilter},{"RandomMirrorBlend", RandomMirrorBlend},{"RandomMirrorAlphaBlend", RandomMirrorAlphaBlend}, {"Bitwise_XOR_AlphaSubFilter", Bitwise_XOR_AlphaSubFilter}, {"AlphaBlendSubFilter", AlphaBlendSubFilter},{"GradientSubFilterXor", GradientSubFilterXor},{"XorBlend_SubFilter", XorBlend_SubFilter},{"SmoothSubFilterAlphaBlend", SmoothSubFilterAlphaBlend},{"SmoothSubFilterXorBlend", SmoothSubFilterXorBlend},{"IntertwineSubFilter", IntertwineSubFilter},{"RandBlend", RandBlend},{"EveryOther", EveryOther},{"EveryOtherSubFilter", EveryOtherSubFilter},{"SmoothRandomFilter", SmoothRandomFilter},{"RandomFilterRandomTimes", RandomFilterRandomTimes},{"RandomSubFilterRandomTimes", RandomSubFilterRandomTimes},{"AddToFrameSubFilter",AddToFrameSubFilter},{"MirrorXor", MirrorXor},{"MirrorXorAll", MirrorXorAll}, {"MirrorXorScale", MirrorXorScale},{"EnergyMirror",EnergyMirror},{"SmoothSubFilter", SmoothSubFilter},{"EnergizeSubFilter", EnergizeSubFilter},{"SmoothSubFilter16", SmoothSubFilter16},{"EnergizeSubFilter16", EnergizeSubFilter16},{"EnergizeSubFilter32", EnergizeSubFilter32},{"SmoothSubFilter32", SmoothSubFilter32},{"HalfAddSubFilter", HalfAddSubFilter},{"HalfXorSubFilter", HalfXorSubFilter},{"StaticXorBlend", StaticXorBlend},{"PsycheSort", PsycheSort},{"XorScale", XorScale},{"ChannelMedianSubFilter", ChannelMedianSubFilter},{"GaussianStrobe", GaussianStrobe},{"StrobeSort", StrobeSort},{"GlitchSortStrobe", GlitchSortStrobe},{"Bitwise_XOR_Blend",Bitwise_XOR_Blend},{"Bitwise_XOR_Sort", Bitwise_XOR_Sort},{"Bitwise_OR_Blend", Bitwise_OR_Blend},{"Bitwise_AND_Blend", Bitwise_AND_Blend},{"BitwiseColorMatrix", BitwiseColorMatrix},{"PixelReverseXor", PixelReverseXor},{"PixelatedSubFilterSort", PixelatedSubFilterSort},{"SilverBlend", SilverBlend},{"RandomPixelOrderSort", RandomPixelOrderSort},{"ImageXorAlpha", ImageXorAlpha},{"ImageAverageXor", ImageAverageXor},{"PixelXorBlend", PixelXorBlend},{"SelfAlphaScale", SelfAlphaScale},{"SelfScaleAlpha", SelfScaleAlpha},{"RainbowXorBlend", RainbowXorBlend},{"FrameDifference", FrameDifference},{"SmallDiffference", SmallDiffference},{"FadeBlend", FadeBlend},{"FilteredDifferenceSubFilter", FilteredDifferenceSubFilter},{"ExpandSquareSubFilter", ExpandSquareSubFilter}, {"ExpandSquareBlendSubFilter", ExpandSquareBlendSubFilter},{"ExpandSquareVerticalSubFilter", ExpandSquareVerticalSubFilter},{"DarkImageMedianBlend", DarkImageMedianBlend},{"GammaDarken5", GammaDarken5},{"GammaDarken10", GammaDarken10},{"SelfAlphaScaleBlend", SelfAlphaScaleBlend},{"FadeBars", FadeBars},{"MirrorXorAlpha", MirrorXorAlpha},{"MirrorEnergizeSubFilter", MirrorEnergizeSubFilter},{"StrobeXor", StrobeXor},{"IntertwinedMirror", InterMirror}, {"BlurredMirror", BlurredMirror},{"ShadeRGB", ShadeRGB},{"InterRGB_SubFilter", InterRGB_SubFilter},{"InterSmoothSubFilter", InterSmoothSubFilter},{"InterRGB_Bars_XY", InterRGB_Bars_XY},{"InterRGB_Bars_X",InterRGB_Bars_X},{"InterRGB_Bars_Y", InterRGB_Bars_Y},{"StoredFramesAlphaBlend_SubFilter", StoredFramesAlphaBlend_SubFilter},{"BlendSubFilter", BlendSubFilter},{"BlendAlphaSubFilter", BlendAlphaSubFilter},{"ReverseFrameBlend", ReverseFrameBlend},{"ReverseFrameBlendSwitch", ReverseFrameBlendSwitch},{"DoubleRandomMirror", DoubleRandomMirror},{"Blend_AlphaSubFilter", Blend_AlphaSubFilter},{"RandomBlendFilter", RandomBlendFilter},{"DoubleRandomBlendFilter", DoubleRandomBlendFilter},{"FlipBlendW", FlipBlendW},{"FlipBlendWH", FlipBlendWH}, {"FlipBlendH", FlipBlendH}, {"FlipBlendAll", FlipBlendAll},{"FrameMedianBlendSubFilter", FrameMedianBlendSubFilter}, {"FrameBlurSubFilter", FrameBlurSubFilter},{"ImageBlendSubFilter", ImageBlendSubFilter},{"ImageBlendXorSubFilter", ImageBlendXorSubFilter},{"ImageCollectionSubFilter", ImageCollectionSubFilter},{"SelfScaleXorIncrease", SelfScaleXorIncrease},{"Blend_RedGreenBlue", Blend_RedGreenBlue},{"XorBlend_RedGreenBlue", XorBlend_RedGreenBlue},{"BlendIncrease_RedGreenBlue", BlendIncrease_RedGreenBlue},{"Blend_RedReenBlue_Dark", Blend_RedReenBlue_Dark},{"DarkModBlend", DarkModBlend}, {"PictureBuzz", PictureBuzz},{"IncDifference", IncDifference},{"IncDifferenceAlpha", IncDifferenceAlpha},{"MirrorMedianBlend", MirrorMedianBlend},{"SubFilterMedianBlend", SubFilterMedianBlend},{"DarkenBlend", DarkenBlend},{"DarkCollectionSubFilter", DarkCollectionSubFilter},{"ChannelSort_NoBlend_Descending", ChannelSort_NoBlend_Descending}, {"ChannelSort_NoBlend_Ascending", ChannelSort_NoBlend_Ascending},{"Headrush", Headrush},{"DarkSmooth_Filter", DarkSmooth_Filter},{"DarkSelfAlpha", DarkSelfAlpha},{"FlipMedian", FlipMedian},{"FlipMedianSubFilter", FlipMedianSubFilter},{"FlipMirror", FlipMirror},{"FlipMirrorAverage", FlipMirrorAverage},{"FlipMirrorSubFilter", FlipMirrorSubFilter},{"ShuffleMedian", ShuffleMedian},{"ShuffleRGB", ShuffleRGB},{"ParticleSnow", ParticleSnow},{"RandomPixels", RandomPixels},{"DarkRandomPixels", DarkRandomPixels},{"MedianBlurSubFilter", MedianBlurSubFilter},{"Bars", Bars},{"ShuffleAlpha", ShuffleAlpha},{"AlphaMorph", AlphaMorph},{"ShuffleSelf", ShuffleSelf},{"PixelatedHorizontalLines", PixelatedHorizontalLines},{"PixelatedVerticalLines", PixelatedVerticalLines},{"StrobeShuffle", StrobeShuffle},{"BlendBurred", BlendBurred},{"BlendCombinedValues", BlendCombinedValues},{"RGBColorTrails", RGBColorTrails},{"BlendCombinedValueSubFilter", BlendCombinedValueSubFilter},{"BlendSubFilterAlpha", BlendSubFilterAlpha},{"GradientXorPixels", GradientXorPixels},{"PurpleRain", PurpleRain},{"PixelByPixelXor", PixelByPixelXor},{"CopyXorAlpha", CopyXorAlpha},{"AveragePixelsXor", AveragePixelsXor},{"AveragePixelAlpha", AveragePixelAlpha},{"NegativeByRow", NegativeByRow},{"AveragePixelCollection", AveragePixelCollection},{"IncorrectLine", IncorrectLine},{"XorShift", XorShift},{"StrobeXorAndOr", StrobeXorAndOr},{"XorWithSource", XorWithSource},{"AlphaBlendWithSource", AlphaBlendWithSource},{"RGBSep1x", RGBSep},{"RGBMedianBlend", RGBMedianBlend},{"RGBMirror1", RGBMirror1},{"RGBMirror1Median", RGBMirror1Median}, {"FlashMirror", FlashMirror},{"CollectionXorSourceSubFilter",  CollectionXorSourceSubFilter},{"ReverseMirrorX", ReverseMirrorX},{"MirrorXorAll_Reverse", MirrorXorAll_Reverse},{"MirrorRGBReverse", MirrorRGBReverse},{"MirrorRGBReverseBlend", MirrorRGBReverseBlend},{"BlendReverseSubFilter", BlendReverseSubFilter},{"MirrorBitwiseXor", MirrorBitwiseXor},{"SmoothBlendReverseSubFilter", SmoothBlendReverseSubFilter},{"RandomIncrease", RandomIncrease},{"MedianBlend16", MedianBlend16},{"MedianBlendBufferSubFilter", MedianBlendBufferSubFilter},{"BGRBlend", BGRBlend},{"RGBBlend", RGBBlend},{"RGBBlendSubFilter", RGBBlendSubFilter},{"DivideAndIncH", DivideAndIncH},{"DivideAndIncW", DivideAndIncW},{"XorOppositeSubFilter", XorOppositeSubFilter},{"BlendSmoothSubFilter", BlendSmoothSubFilter},{"BlurSmooth", BlurSmooth},{"BlurSmoothMedian", BlurSmoothMedian},{"BlurSmoothSubFilter", BlurSmoothSubFilter},{"BlurFlip", BlurFlip},{"BlurFlipSubFilter", BlurFlipSubFilter},{"BlurMirrorGamma", BlurMirrorGamma},{"MedianBlendDark", MedianBlendDark},{"MedianBlendSubFilterEx", MedianBlendSubFilterEx},{"EnergyMirrorDark", EnergyMirrorDark},{"AlphaBlendMirror", AlphaBlendMirror},{"MirrorAlphaBlendedImage", MirrorAlphaBlendedImage},{"AlphaBlendXorImage", AlphaBlendXorImage},{"ShiftFrameSmoothSubFilter", ShiftFrameSmoothSubFilter},{"ShiftFrameStaticXorSubFilter", ShiftFrameStaticXorSubFilter},{"IncreaseDecreaseGamma", IncreaseDecreaseGamma},{"GammaIncDecIncrease", GammaIncDecIncrease},{"RandomSubFilter2", RandomSubFilter2},{"TwistedVision", TwistedVision},{"TwistedMirror", TwistedMirror},{"SelfScaleSortBlend", SelfScaleSortBlend},{"FlashMedianBlend", FlashMedianBlend},{"BlendWithFrameSubFilter", BlendWithFrameSubFilter},{"AlphaBlendWithFrameSubFilter", AlphaBlendWithFrameSubFilter},{"AlphaXorBlendWithFrameSubFilter", AlphaXorBlendWithFrameSubFilter},{"XorBlendSubFilter", XorBlendSubFilter},{"FlipAlphaBlend", FlipAlphaBlend},{"RandomFlipFilter", RandomFlipFilter},{"MirrorMedian", MirrorMedian},{"FlipMatrixCollection", FlipMatrixCollection},{"MirrorMatrixCollection", MirrorMatrixCollection},{"MirrorMatrixSource", MirrorMatrixSource},{"SelfScaleByFrame", SelfScaleByFrame},{"SmoothMedianRotateSubFilter", SmoothMedianRotateSubFilter},{"SmoothCollectionAlphaBlend", SmoothCollectionAlphaBlend},{"XorSubFilter", XorSubFilter},{"XorAlphaSubFilter", XorAlphaSubFilter},{"BlurXorAlphaSubFilter", BlurXorAlphaSubFilter},{"ImageXorFrame", ImageXorFrame},{"ImageXorFunction", ImageXorFunction},{"ImageXorAlphaBlend", ImageXorAlphaBlend},{"ImageAlphaXorMedianSubFilter", ImageAlphaXorMedianSubFilter},{"ImageSmoothAlphaXorSubFilter", ImageSmoothAlphaXorSubFilter},{"ImageXorMirrorFilter", ImageXorMirrorFilter},{"ImageXorSubFilter", ImageXorSubFilter},{"ImageAlphaXorSubFilter", ImageAlphaXorSubFilter},{"SmoothTrailsBlend", SmoothTrailsBlend},{"MatrixCollectionRGBXor", MatrixCollectionRGBXor},{"RainbowGlitch", RainbowGlitch},{"RainbowGlichStrobe", RainbowGlichStrobe}, {"NegateSwitchStrobe", NegateSwitchStrobe},{"StrobeAlphaShuffle", StrobeAlphaShuffle},{"ShuffleAlphaWithRGB", ShuffleAlphaWithRGB},{"ShuffleAlphaSubFilter", ShuffleAlphaSubFilter},{"ShuffleColorMap", ShuffleColorMap},{"BlendWithRainbowSubFilter", BlendWithRainbowSubFilter},{"BlendWithJetSubFilter", BlendWithJetSubFilter},{"ColormapBlendSubFilter", ColormapBlendSubFilter},{"RandomColorMap", RandomColorMap},{"SmoothMirrorBlurFlip", SmoothMirrorBlurFlip},{"RandomColorMapAlphaBlendSubFilter", RandomColorMapAlphaBlendSubFilter},{"RandomOrder", RandomOrder},{"RandomOrderMedianBlendSubFilter", RandomOrderMedianBlendSubFilter}, {"MirrorOrder", MirrorOrder},{"MirrorOrderSubFilter", MirrorOrderSubFilter},{"BlurMirrorOrder", BlurMirrorOrder},{"AveragePixelMirror", AveragePixelMirror},{"ShuffleAlphaMedianBlend", ShuffleAlphaMedianBlend},{"MirrorOrderAlpha", MirrorOrderAlpha},{"FilterStrobeSubFilter", FilterStrobeSubFilter},{"ImageSubtractMedianBlend", ImageSubtractMedianBlend},{"ImageDarkBlend", ImageDarkBlend},{"ImageAverageDark", ImageAverageDark},{"ImageRemainderPixel", ImageRemainderPixel},{"AverageLinesBlend", AverageLinesBlend},{"SoftFeedbackMirror", SoftFeedbackMirror},{"AverageVerticalLinesBlend", AverageVerticalLinesBlend},{"LinesMedianBlend", LinesMedianBlend},{"XorSquare", XorSquare},{"PixelValuesPlusOne", PixelValuesPlusOne},{"AverageHorizontalFilter", AverageHorizontalFilter},{"AverageVerticalFilter", AverageVerticalFilter},{"GradientAlphaXorHorizontal", GradientAlphaXorHorizontal},{"GradientAlphaXorVertical", GradientAlphaXorVertical},{"BlendImageWithSubFilter", BlendImageWithSubFilter},{"BlendImageWithSubFilterAlpha", BlendImageWithSubFilterAlpha},{"MedianBlendSoft", MedianBlendSoft},{"AndImageSubFilterXor", AndImageSubFilterXor},{"AlphaBlendImageSubFilterXor", AlphaBlendImageSubFilterXor},{"AlphaBlendImageSubFilterXorRev", AlphaBlendImageSubFilterXorRev},{"ParticleReleaseXor", ParticleReleaseXor},{"ParticleReleaseXorVec", ParticleReleaseXorVec},{"ParticleReleaseAlphaBlend", ParticleReleaseAlphaBlend},{"ParticleReleaseWithImage", ParticleReleaseWithImage},{"ParticleReleaseSubFilter", ParticleReleaseSubFilter},{"ParticleReleaseImageSubFilter", ParticleReleaseImageSubFilter},{"ImageEnergy", ImageEnergy},{"ImageEnergySubFilter", ImageEnergySubFilter},{"ImageDistortion", ImageDistortion},{"ImageDistortionSubFilter", ImageDistortionSubFilter},{"SmoothExactImageXorAlpha", SmoothExactImageXorAlpha},{"FeedbackColormap", FeedbackColormap},{"SmoothImageAlphaBlendMedian", SmoothImageAlphaBlendMedian},{"ImageDarkenSmoothMedian", ImageDarkenSmoothMedian},{"XorReverseImageSmooth", XorReverseImageSmooth},{"ReverseSubFilterBlend", ReverseSubFilterBlend},{"ReverseSubFilterXor", ReverseSubFilterXor},{"ImageReverseSubFilter", ImageReverseSubFilter},{"SmoothRainbowMedian", SmoothRainbowMedian},{"MirrorAlphaBlend", MirrorAlphaBlend},{"ImageSmoothMedianBlend", ImageSmoothMedianBlend},{"ImageSmoothMedianSubFilter", ImageSmoothMedianSubFilter},{"ImageAlphaXorMedianBlend", ImageAlphaXorMedianBlend},{"MatrixCollectionBlend", MatrixCollectionBlend},{"MatrixCollectionSubFilter", MatrixCollectionSubFilter},{"MatrixCollectionImageSubFilter", MatrixCollectionImageSubFilter},{"MatrixCollectionBlurAlpha", MatrixCollectionBlurAlpha},{"MatrixCollectionXor", MatrixCollectionXor},{"MatrixCollectionXor32", MatrixCollectionXor32},{"MatrixCollectionRandomColorMap", MatrixCollectionRandomColorMap},{"MatrixCollectionDarkXor", MatrixCollectionDarkXor},{"MatrixCollectionRGB", MatrixCollectionRGB},{"TrailsSubFilter", TrailsSubFilter},{"TrailsSubFilter32", TrailsSubFilter32},{"CompareWithSubFilter", CompareWithSubFilter},{"MedianTrails", MedianTrails},{"SmoothMedianBlend", SmoothMedianBlend},{"ColorTransition", ColorTransition},{"ColorTransitionMedian", ColorTransitionMedian},{"ColorTransitionRandom", ColorTransitionRandom},{"ColorTransitionRandomMedian", ColorTransitionRandomMedian},{"ColorTransitionSubFilter", ColorTransitionSubFilter},{"ColorTransitionImageSubFilter", ColorTransitionImageSubFilter},{"CurtainSubFilter", CurtainSubFilter},{"RandomTrails", RandomTrails},{"RandomTrailsSubFilter", RandomTrailsSubFilter},{"CosSinMedianBlend", CosSinMedianBlend},{"TrailsRGB", TrailsRGB},{"MatrixTrailsXorRandom", MatrixTrailsXorRandom},{"CosSinMultiplyCollectionXor", CosSinMultiplyCollectionXor},{"Filter8_Blend", Filter8_Blend},{"Filter8_SubFilter", Filter8_SubFilter},{"RandomSmoothAlphaMedian", RandomSmoothAlphaMedian},{"RandomAlphaBlend", RandomAlphaBlendFilter},{"RandomMirrorBitwiseXor", RandomMirrorBitwiseXor},{"SquareDivideSubFilter", SquareDivideSubFilter},{"SquareSubFilter", SquareSubFilter},{"SquareSubFilter8", SquareSubFilter8},{"ColorExpand", ColorExpand},{"ColorExpandSubFilter", ColorExpandSubFilter},{"RotateImage", RotateImage},{"RotateBlendImage", RotateBlendImage},{"RotateImageSubFilter", RotateImageSubFilter},{"RotateAlphaBlendImage", RotateAlphaBlendImage},{"FlipShuffle", FlipShuffle},{"FlipRandom", FlipRandom},{"FlipOrder", FlipOrder},{"FlipStrobeSubFilter", FlipStrobeSubFilter},{"MirrorBlendFrame", MirrorBlendFrame},{"MirrorBlendVertical", MirrorBlendVertical},{"MirrorVerticalAndHorizontal", MirrorVerticalAndHorizontal},{"BlendFor360", BlendFor360},{"MirrorSidesMedian", MirrorSidesMedian},{"MirrorSidesSubFilter", MirrorSidesSubFilter},{"MedianFrameAlphaBlendSubFilter", MedianFrameAlphaBlendSubFilter},{"MedianSubFilter", MedianSubFilter},{"ColorXorScale", ColorXorScale},{"ColorXorScaleSubFilter", ColorXorScaleSubFilter},{"ImageXorScale", ImageXorScale},{"MatrixCollectionShiftSubFilter", MatrixCollectionShiftSubFilter},{"MatrixCollectionImageShiftSubFilter", MatrixCollectionImageShiftSubFilter},{"MatrixCollectionXorBlendAlphaBlend", MatrixCollectionXorBlendAlphaBlend},{"MatrixCollectionBlurImageXorAlpha", MatrixCollectionBlurImageXorAlpha},{"MatrixCollectionBlurImageSubFilter", MatrixCollectionBlurImageSubFilter},{"MatrixCollectionBlurImageSubFilter16", MatrixCollectionBlurImageSubFilter16},{"ImageAlphaBlendSubFilter", ImageAlphaBlendSubFilter},{"MultipleMatrixCollectionSubFilter", MultipleMatrixCollectionSubFilter},{"BlurAlphaSubFilter", BlurAlphaSubFilter},{"BlurImageSubFilter", BlurImageSubFilter},{"MedianBlendSubFilter", MedianBlendSubFilter},{"MedianBlendImageSubFilter", MedianBlendImageSubFilter},{"MedianBlendSelfBlend", MedianBlendSelfBlend},{"BlendHalfSubFilter", BlendHalfSubFilter},{"BlurImageAlphaBlend", BlurImageAlphaBlend},{"BlurImageAlphaBlendSubFilter", BlurImageAlphaBlendSubFilter},{"BlurImageAlphaBlendScaleSubFilter", BlurImageAlphaBlendScaleSubFilter},{"RandomAmountOfMedianBlur", RandomAmountOfMedianBlur},{"Bitwise_XOR_BlendFrame", Bitwise_XOR_BlendFrame},{"AlphaBlendWithSubFilter", AlphaBlendWithSubFilter},{"AlphaBlendScaleWithSubFilter", AlphaBlendScaleWithSubFilter},{"GaussianBlendEx", GaussianBlendEx},{"SimpleMatrixBlend", SimpleMatrixBlend},{"MatrixBlendSubFilter", MatrixBlendSubFilter},{"SmoothMatrixBlendSubFilter", SmoothMatrixBlendSubFilter},{"BlurSmoothSubFilterAlphaBlend", BlurSmoothSubFilterAlphaBlend},{"BlurSmoothAlphaXorBlendSubFilter", BlurSmoothAlphaXorBlendSubFilter},{"BlurTwiceSubFilter", BlurTwiceSubFilter},{"BlurFrameBlendSubFilter", BlurFrameBlendSubFilter},{"BlurFrameSubFilter", BlurFrameSubFilter},{"BlurSmoothMatrix", BlurSmoothMatrix},{"MedianBlurInc", MedianBlurInc},{"GaussianBlurInc", GaussianBlurInc},{"BlurSmoothMedianInc", BlurSmoothMedianInc},{"BlurSmoothGaussianInc", BlurSmoothGaussianInc},{"BlurMatrixCollectionXor", BlurMatrixCollectionXor},{"MatrixCollection8XorSubFilter", MatrixCollection8XorSubFilter},{"BlurSmoothRevFilter", BlurSmoothRevFilter},{"SurroundingPixels", SurroundingPixels},{"SurroundingPixelsAlpha", SurroundingPixelsAlpha},{"MatrixCollectionSurroundingPixels", MatrixCollectionSurroundingPixels},{"MatrixCollectionSurroundingPixelsSubFilter", MatrixCollectionSurroundingPixelsSubFilter},{"MatrixCollectionSurroundingPixelsImage", MatrixCollectionSurroundingPixelsImage},{"MatrixCollectionSurroundingPixelsImageSubFilter", MatrixCollectionSurroundingPixelsImageSubFilter},{"ImageTransparent", ImageTransparent},{"MatrixImageAlphaBlendSubFilter", MatrixImageAlphaBlendSubFilter},{"ImageAlphaCollectionSmoothBlend", ImageAlphaCollectionSmoothBlend},{"ImageRandomColormap", ImageRandomColormap},{"ImageRandomColormapAlphaBlend", ImageRandomColormapAlphaBlend},{"ImageRandomColormapAlphaScale", ImageRandomColormapAlphaScale},{"ImageRandomColormapSubFilter", ImageRandomColormapSubFilter},{"ImageShuffle", ImageShuffle},{"ImageSubFilter", ImageSubFilter},{"ImageAlphaBlendWithFrameSubFilter", ImageAlphaBlendWithFrameSubFilter},{"ImageFadeInOut", ImageFadeInOut},{"ImageFadeBlackInOut", ImageFadeBlackInOut},{"ImageFadeBlackInOutSubFilter", ImageFadeBlackInOutSubFilter},{"ImageFadeFrameInOut", ImageFadeFrameInOut},{"ImageFadeFrameInOutSubFilter",ImageFadeFrameInOutSubFilter},{"ImageFadeDouble", ImageFadeDouble},{"BlendSubFilterAndImage", BlendSubFilterAndImage},{"FadeSubFilter", FadeSubFilter},{"FadeSubFilterRev", FadeSubFilterRev},{"ImageBlendSubFilterMedianBlend", ImageBlendSubFilterMedianBlend},{"FadeSubFilterXor", FadeSubFilterXor},{"BlurXorSubFilter",BlurXorSubFilter},{"ColorFlashIncrease", ColorFlashIncrease},{"ScaleFilter", ScaleFilter},{"NegativeDarkenXor", NegativeDarkenXor},{"ImageXor_SubFilter", ImageXor_SubFilter},{"NegateBlendSubFilter", NegateBlendSubFilter},{"StrobeNegatePixel",StrobeNegatePixel},{"StrobeNegateInOut", StrobeNegateInOut},{"ImageStrobeOnOff", ImageStrobeOnOff},{"AlphaStrobeBlend", AlphaStrobeBlend},{"CannyRandomPixels", CannyRandomPixels},{"FrameImageFadeInOut", FrameImageFadeInOut},{"FrameImageFadeInOutDouble", FrameImageFadeInOutDouble},{"ChangeEachSecond", ChangeEachSecond},{"ShuffleImage", ShuffleImage},{"ChangeImageEachSecond", ChangeImageEachSecond},{"ChangeImageFilterOnOff", ChangeImageFilterOnOff},{"ChangeXorEachSecond", ChangeXorEachSecond},{"MorphXor", MorphXor},{"MorphXorWithSubFilter", MorphXorWithSubFilter},{"MirrorEachSecond", MirrorEachSecond},{"MirrorReverseSubFilter", MirrorReverseSubFilter},{"MirrorReverseSubFilterAlphaBlend", MirrorReverseSubFilterAlphaBlend},{"Mirror_Xor_Combined", Mirror_Xor_Combined},{"MirrorFrameShuffle", MirrorFrameShuffle},{"MirrorShuffleSmooth", MirrorShuffleSmooth},{"Mirror_Xor_Smooth", Mirror_Xor_Smooth},{"XorFrameShuffle", XorFrameShuffle},{"XorMirrorBlendFrame", XorMirrorBlendFrame},{"ImageXorSmooth", ImageXorSmooth},{"SmoothSubFilter64", SmoothSubFilter64},{"SmoothMedian64", SmoothMedian64},{"SmoothMedian32_SubFilter", SmoothMedian32_SubFilter},{"SmoothAlphaMedian_SubFilter", SmoothAlphaMedian_SubFilter},{"SmoothImage_SubFilter", SmoothImage_SubFilter},{"SmoothImageMedian_SubFilter", SmoothImageMedian_SubFilter},{"SmoothImageAndSubFilter", SmoothImageAndSubFilter},{"SmoothSubFilter90", SmoothSubFilter90},{"SmoothMedianImageSubFilter16", SmoothMedianImageSubFilter16},{"ImageNegate", ImageNegate},{"ImageNegateAlphaBlend", ImageNegateAlphaBlend},{"ImageNegateAlphaBlendSubFilter", ImageNegateAlphaBlendSubFilter},{"FrameNegateAlphaBlendImage", FrameNegateAlphaBlendImage},{"DarkTrailsEffect", DarkTrailsEffect},{"DarkNegate", DarkNegate},{"ChannelSortMedianBlend",ChannelSortMedianBlend},{"MatrixCollectionMirrorDirection", MatrixCollectionMirrorDirection},{"StrobeRandomChannel", StrobeRandomChannel},{"SplitFramesSort", SplitFramesSort},{"SplitFrameSortSubFilter", SplitFrameSortSubFilter},{"MedianBlend64", MedianBlend64},{"SplitFrameFilter", SplitFrameFilter},{"SplitFrameBlend", SplitFrameBlend},{"SplitFrameBlendSubFilter", SplitFrameBlendSubFilter},{"SplitFrameCollection", SplitFrameCollection},{"SplitFrameMirror", SplitFrameMirror},{"RandomChannels", RandomChannels},{"SmoothRandomChannels", SmoothRandomChannels},{"SmoothChannelSubFilter", SmoothChannelSubFilter},{"IncreaseRGB", IncreaseRGB},{"IncreaseColor", IncreaseColor},{"SaturateBlend", SaturateBlend},{"SaturateBlendSubFilter", SaturateBlendSubFilter},{"MaxRGB", MaxRGB},{"XorDifferenceFilter", XorDifferenceFilter},{"AlphaBlendChannelSort", AlphaBlendChannelSort},{"ColorTrailsFilter", ColorTrailsFilter},{"ColorTrailsSubFilter", ColorTrailsSubFilter},{"DarkNegateRainbowMedian", DarkNegateRainbowMedian},{"IncreaseQuick", IncreaseQuick},{"IncreaseRandomIndex", IncreaseRandomIndex},{"ImageChannelSubFilter", ImageChannelSubFilter},{"ImageChannelChangeSubFilter", ImageChannelChangeSubFilter},{"ImageChannelRandom", ImageChannelRandom},{"ImageChannelRandomSubFilter", ImageChannelRandomSubFilter},{"PixelateBlur", PixelateBlur},{"PixelateBlock", PixelateBlock},{"PixelateNoResize12", PixelateNoResize12},{"PixelateNoResize8", PixelateNoResize8},{"PixelateNoResize32", PixelateNoResize32},{"PixelateNoResize16", PixelateNoResize16},{"PixelateNoResize24", PixelateNoResize24},{"PixelateBlurFilter",PixelateBlurFilter},{"PixelateBlurSubFilter", PixelateBlurSubFilter},{"MirrorXorImage", MirrorXorImage},{"MirrorXorSubFilter", MirrorXorSubFilter},{"PixelXorImageSubFilter", PixelXorImageSubFilter},{"PixelAlphaImageSubFilter", PixelAlphaImageSubFilter},{"PixelScaleImageSubFilter", PixelScaleImageSubFilter},{"PixelImageSubFilter", PixelImageSubFilter},{"PixelImageTex", PixelImageTex},{"PixelImageXorSubFilter", PixelImageXorSubFilter},{"PixelRowMedianBlend", PixelRowMedianBlend},{"IntertwineRows32", IntertwineRows32},{"IntertwineRows16", IntertwineRows16},{"IntertwineRows8", IntertwineRows8}, {"IntertwineAlpha", IntertwineAlpha},{"IntertwineRowsSubFilter", IntertwineRowsSubFilter}, {"IntertwineRows4", IntertwineRows4},{"Intertwine64x4", Intertwine64x4},{"Intertwine64X", Intertwine64X},{"Intertwine64XSubFilter", Intertwine64XSubFilter},{"Intertwine64XPixel", Intertwine64XPixel},{"IntertwinePixels", IntertwinePixels},{"IntertwineColsX", IntertwineColsX}, {"IntertwineCols16", IntertwineCols16},{"IntertwineCols32", IntertwineCols32}, {"IntertwineCols8", IntertwineCols8},{"MirrorIntertwine", MirrorIntertwine},{"MirrorIntertwineImage", MirrorIntertwineImage},{"MirrorIntertwineImageSubFilter", MirrorIntertwineImageSubFilter},{"IntertwineImageSubFilter", IntertwineImageSubFilter},{"BlendWithImage", BlendWithImage},{"IntertwineRowsReverse32", IntertwineRowsReverse32},{"IntertwineRowsReverse16", IntertwineRowsReverse16}, {"IntertwineRowsReverse8", IntertwineRowsReverse8},{"IntertwineRowsReverse64X", IntertwineRowsReverse64X},{"IntertwineRowsSwitch32", IntertwineRowsSwitch32},{"IntertwineRowsSwitch16", IntertwineRowsSwitch16}, {"IntertwineRowsSwitch8", IntertwineRowsSwitch8},{"IntertwineRows64", IntertwineRows64}, {"IntertwineRowsReverse64", IntertwineRowsReverse64},{"IntertwineRowsSwitch64", IntertwineRowsSwitch64},{"IntertwineShuffle", IntertwineShuffle},{"InterwtineAlphaSubFilter", InterwtineAlphaSubFilter},{"IntertwineImageAndSubFilter", IntertwineImageAndSubFilter},{"IntertwineRowsAndCols", IntertwineRowsAndCols},{"IntertwineRowsAndColsSubFilter", IntertwineRowsAndColsSubFilter},{"MatrixCollectionVariable", MatrixCollectionVariable},{"MatrixCollectionVariableSubFilter", MatrixCollectionVariableSubFilter},{"MatrixCollectionVariableImage",MatrixCollectionVariableImage},{"MatrixCollectionVariableImageSubFilter", MatrixCollectionVariableImageSubFilter},{"SmoothImageAverageSubFilter", SmoothImageAverageSubFilter},{"CallSubFilterBlend50", CallSubFilterBlend50},{"CallSubFilterBlend25", CallSubFilterBlend25},{"CallSubFilterBlend75",CallSubFilterBlend75},{"IntertwineColsX2", IntertwineColsX2},{"SmoothFrame64", SmoothFrame64},{"SmoothFrame32", SmoothFrame32},{"MatrixCollectionMedianBlendFilter", MatrixCollectionMedianBlendFilter},{"MedianBlendIncreaseFilter", MedianBlendIncreaseFilter},{"MedianBlendNegate", MedianBlendNegate},{"BlendWithFrameAndImageSubFilter", BlendWithFrameAndImageSubFilter},{"ImageIntertwine32", ImageIntertwine32},{"ImageIntertwine16", ImageIntertwine16}, {"ImageIntertwine8", ImageIntertwine8},{"ImageIntertwine64X", ImageIntertwine64X},{"IntertwineDoubleSubFilter",IntertwineDoubleSubFilter},{"IntertwineDoubleImageSubFilter", IntertwineDoubleImageSubFilter},{"IntertwineEachRowXSubFilter", IntertwineEachRowXSubFilter},{"IntertwineGhost", IntertwineGhost},{"IntertwineGhost32", IntertwineGhost32},{"IntertwineGhostSubFilter", IntertwineGhostSubFilter},{"BlendWithOldFrame", BlendWithOldFrame},{"BlendWith16thFrame", BlendWith16thFrame},{"BlendTrailsSubFilter",BlendTrailsSubFilter},{"LineMoveInOut", LineMoveInOut},{"MatrixCollectionTrails", MatrixCollectionTrails},{"MatrixCollectionRandom", MatrixCollectionRandom},{"MatrixCollectionRandomSmooth", MatrixCollectionRandomSmooth},{"MatrixCollectionRandomDouble", MatrixCollectionRandomDouble},{"MatrixCollectionAddImage", MatrixCollectionAddImage},{"MatrixCollectionAlphaBlendSubFilter", MatrixCollectionAlphaBlendSubFilter},{"MatrixCollectionImageBlendSubFilter", MatrixCollectionImageBlendSubFilter},{"TwitchinGlitchin", TwitchinGlitchin},{"IntertwineRowsImageAlphaBlend", IntertwineRowsImageAlphaBlend}, {"ChannelSortSubFilter", ChannelSortSubFilter},{"IntertwineRow2", IntertwineRow2},{"IntertwineRow720p", IntertwineRow720p},{"IntertwineRow1080p", IntertwineRow1080p},{"IntertwineRow720pX2", IntertwineRow720pX2}, {"IntertwineRow1080pX2", IntertwineRow1080pX2},{"IntertwineCols1280", IntertwineCols1280},{"IntertwineRowCols1280x720", IntertwineRowCols1280x720},{"IntertwineRowsImage", IntertwineRowsImage},{"MedianBlendSquare", MedianBlendSquare},{"SmoothIntertwineMedianBlend", SmoothIntertwineMedianBlend},{"SmoothBlendMedian", SmoothBlendMedian},{"SmoothDoubleSubFilter", SmoothDoubleSubFilter},{"AlphaBlendMedianSubFilter", AlphaBlendMedianSubFilter},{"ReverseOnOff", ReverseOnOff},{"SmoothReverseSubFilter", SmoothReverseSubFilter},{"IntertwineWithSubFilter", IntertwineWithSubFilter},{"IntertwineWithImageAndSubFilter", IntertwineWithImageAndSubFilter},{"IntertwineFrameWithImage", IntertwineFrameWithImage},{"InterlaceVerticalFilter", InterlaceVerticalFilter},{"InterlaceHorizontalFilter", InterlaceHorizontalFilter},{"IntertwineImageFlip", IntertwineImageFlip},{"IntertwineImageFlipSubFilter", IntertwineImageFlipSubFilter},{"IntertwineFlipImageAndSubFilter", IntertwineFlipImageAndSubFilter},{"IntertwineImageOnOff", IntertwineImageOnOff},{"IntertwineVideoShuffle", IntertwineVideoShuffle},{"ImageFlipFilter", ImageFlipFilter},{"FlipFrameFilter", FlipFrameFilter},{"AlphaBlendImageAndSubFilter", AlphaBlendImageAndSubFilter},{"SquareFill", SquareFill},{"StretchAlphaBlend", StretchAlphaBlend},{"StretchAlphaBlendWidth", StretchAlphaBlendWidth}, {"StretchAlphaBlendHeight", StretchAlphaBlendHeight},{"StretchAlphaBlendTwitch", StretchAlphaBlendTwitch},{"StretchAlphaBlednTwitchFast", StretchAlphaBlednTwitchFast},{"StretchRandomValues", StretchRandomValues},{"ImageRandomValues", ImageRandomValues},{"AlphaBlendTrails", AlphaBlendTrails},{"VideoStretchHorizontal", VideoStretchHorizontal},{"VideoStretchVertical", VideoStretchVertical},{"StrobeTrailsFilter", StrobeTrailsFilter},{"AlphaBlendTrailsReverse", AlphaBlendTrailsReverse},{"ShadowAlphaTrails16",ShadowAlphaTrails},{"ShadowAlphaTrailsReset", ShadowAlphaTrailsReset},{"SetColormap", SetColormap},{"ShadowAlphaTrails", ShadowAlphaTrails},{"ShadowAlphaTrails64", ShadowAlphaTrails64},{"MirrorLeft", MirrorLeft},{"MirrorRight", MirrorRight},{"MirrorBackAndForth", MirrorBackAndForth},{"FadeFromColorToColor", FadeFromColorToColor},{"FadeFromColorToColorImage", FadeFromColorToColorImage},{"Random_Filter", Random_Filter},{"FlipY_Axis", FlipY_Axis}, {"FlipX_Axis", FlipX_Axis}, {"FlipBoth", FlipBoth},{"FlipMirrorAlphaBlend", FlipMirrorAlphaBlend},{"Random_FilterX2", Random_FilterX2},{"Random_FilterSubFilter", Random_FilterSubFilter},{"IntertwineRow480pX2", IntertwineRow480pX2},{"LowFi", LowFi},{"HighToLow", HighToLow},{"LowToHigh", LowToHigh},{"MoveHighAndLow", MoveHighAndLow},{"StretchSubFilter", StretchSubFilter},{"Quality480",Quality480}, {"Quality720",Quality720}, {"Quality1080", Quality1080},{"StretchVerticalMirror", StretchVerticalMirror},{"ImageLoFi", ImageLoFi},{"ImageLofiAlphaBlend", ImageLofiAlphaBlend},{"ImageAlphaBlendScale", ImageAlphaBlendScale},{"FrameStretchAlphaBlend", FrameStretchAlphaBlend},{"BlurHighToLow", BlurHighToLow},{"ImageHighToLowAlpha", ImageHighToLowAlpha},{"MirrorTopToBottom", MirrorTopToBottom},{"MirrorBottomToTop", MirrorBottomToTop},{"FlashRGB_SubFilter", FlashRGB_SubFilter},{"MirrorSwitch", MirrorSwitch},{"MirrorSwitchSubFilter", MirrorSwitchSubFilter},{"MirrorSwitchFlip", MirrorSwitchFlip},{"BlendImageLayer", BlendImageLayer},{"StrobeRandomFilter", StrobeRandomFilter},{"AlphaBlendRandom_Filter", AlphaBlendRandom_Filter},{"DoubleRandomAlphaImageSubFilter", DoubleRandomAlphaImageSubFilter},{"MirrorLeftMirrorRightBlend",MirrorLeftMirrorRightBlend},{"MirrorTopMirrorBottomBlend", MirrorTopMirrorBottomBlend},{"MirrorAll", MirrorAll},{"ImageDiff", ImageDiff},{"ImageDiffSubFilter", ImageDiffSubFilter},{"RestoreBlack", RestoreBlack},{"OrigBlendSubFilter", OrigBlendSubFilter},{"OrigAndCurrentRandomX2", OrigAndCurrentRandomX2},{"FillPixelsImage", FillPixelsImage},{"AverageHorizontalDistortion", AverageHorizontalDistortion},{"AlphaBlendImageWithOrigSource", AlphaBlendImageWithOrigSource},{"resizeFrameWidth", resizeFrameWidth},{"resizeFrameHeight", resizeFrameHeight}, {"resizeFrameWidthAndHeight", resizeFrameWidthAndHeight},{"resizeImageWidth", resizeImageWidth}, {"resizseImageHeight", resizeImageHeight}, {"resizeImageWidthAndHeight", resizeImageWidthAndHeight},{"resizeImageAndFrameBlend", resizeImageAndFrameBlend},{"resizeImageWidthAndHeightSubFilter", resizeImageWidthAndHeightSubFilter},{"resizeImageFrameWidth", resizeImageFrameWidth},{"resizeImageFrameHeight",resizeImageFrameHeight}, {"resizeImageFrameWidthAndHeight", resizeImageFrameWidthAndHeight},{"resizeImageRandom", resizeImageRandom},{"resizeFrameRandom", resizeFrameRandom},{"resizeFrameImageFlash", resizeFrameImageFlash},{"RandomFlip", RandomFlip},{"ColorsFilter", ColorsFilter},{"MirrorDoubleVision", MirrorDoubleVision},{"ColorFadeFilter", ColorFadeFilter},{"ColorChannelMoveUpAndDown", ColorChannelMoveUpAndDown},{"MedianStrobe", MedianStrobe},{"DifferenceSubFilter", DifferenceSubFilter},{"AlphaBlendFlippedFilter", AlphaBlendFlippedFilter},{"ColorVariableBlend", ColorVariableBlend},{"ColorXorBlend", ColorXorBlend},{"ColorAddBlend", ColorAddBlend},{"IntertwineRowsShadow720p", IntertwineRowsShadow720p},{"IntertwineRowsAlpha720p", IntertwineRowsAlpha720p},{"IntertwineByFrameDown", IntertwineByFrameDown},{"IntertwineByFrameUp", IntertwineByFrameUp},{"IntertwineFrameFast", IntertwineFrameFast},{"IntertwineFrame360", IntertwineFrame360},{"IntertwineFrame360X", IntertwineFrame360X},{"IntertwineFrameTwitch", IntertwineFrameTwitch},{"IntertwineFrameDark", IntertwineFrameDark},{"IntertwineFrame360_Reverse", IntertwineFrame360_Reverse},{"IntertwineFrameBlend", IntertwineFrameBlend},{"IntertwineXorCollection", IntertwineXorCollection},{"IntertwineFrame720X", IntertwineFrame720X},{"IntertwineFrame1080X", IntertwineFrame1080X},{"IntertwineFrameImage1080X", IntertwineFrameImage1080X},{"RandomXorMultiThreadTest", RandomXorMultiThreadTest},{"SelfAlphaBlendMultiThread", SelfAlphaBlendMultiThread},{"MedianBlendMultiThread", MedianBlendMultiThread},{"BytePixelSort", BytePixelSort},{"SortedImageColorVariable", SortedImageColorVariable},{"SmoothColorVariableImageBlend", SmoothColorVariableImageBlend},{"BlendChannelXor", BlendChannelXor},{"ColorShiftXor", ColorShiftXor},{"RandomSquares", RandomSquares},{"RandomImageSquares", RandomImageSquares},{"Random_FilterX2_SubFilter", Random_FilterX2_SubFilter},{"FuzzyFilter", FuzzyFilter},{"XorMultiply", XorMultiply},{"Grayscale", Grayscale},{"ColorShadowBlend", ColorShadowBlend},{"FlashMatrixTrails", FlashMatrixTrails},{"GhostTrails", GhostTrails},{"MedianBlend_Random_Filter", MedianBlend_Random_Filter},{"IntertwineMirrorEnergy", IntertwineMirrorEnergy},{"IntertwineMultipleRows", IntertwineMultipleRows},{"GhostTwitch", GhostTwitch},{"GhostTwitchTrails", GhostTwitchTrails},{"Dyskinesia", Dyskinesia},{"Mirror_Rainbow_Blur", Mirror_Rainbow_Blur},{"MirrorMedianBlur", MirrorMedianBlur},{"VideoTwitch", VideoTwitch},{"ErodeFilter", ErodeFilter},{"DilateFilter", DilateFilter},{"MirrorLeftBottomToTop", MirrorLeftBottomToTop},{"MirrorRightTopToBottom", MirrorRightTopToBottom},{"BilateralFilter", BilateralFilter},{"BilateralFilterFade", BilateralFilterFade},{"BilateralBlend", BilateralBlend},{"BilateralBlendFade", BilateralBlendFade},{"BilateralFilterScale", BilateralFilterScale},{"ColorRGB_IncDec", ColorRGB_IncDec},{"ColorCollection", ColorCollection},{"ColorCollectionRandom", ColorCollectionRandom},{"ColorCollectionStrobeShift", ColorCollectionStrobeShift},{"ColorCollectionRandom_Filter", ColorCollectionRandom_Filter},{"ColorCollectionShift", ColorCollectionShift},{"CollectionEnergy", CollectionEnergy},{"ColorCollectionInterlace", ColorCollectionInterlace},{"ColorCollectionStrobeShake", ColorCollectionStrobeShake},{"ColorCollectionSubFilter", ColorCollectionSubFilter},{"ColorCollectionShiftSubFilter", ColorCollectionShiftSubFilter},{"ColorCollectionSubtle", ColorCollectionSubtle},{"ColorCollection64", ColorCollection64},{"ColorCollectionSubtleStrobe", ColorCollectionSubtleStrobe},{"CollectionRandom", CollectionRandom},{"CollectionRandomSubFilter", CollectionRandomSubFilter},{"CollectionImage", CollectionImage},{"CollectionAlphaXor", CollectionAlphaXor},{"ColorCollection64X", ColorCollection64X},{"ColorCollectionSwitch", ColorCollectionSwitch},{"ColorCollectionRGB_Index", ColorCollectionRGB_Index},{"ColorCollectionRGBStrobeSubFilter", ColorCollectionRGBStrobeSubFilter},{"ColorCollectionGhostTrails", ColorCollectionGhostTrails},{"ColorCollectionScale", ColorCollectionScale},{"ColorCollectionReverseStrobe", ColorCollectionReverseStrobe},{"CollectionAlphaBlend_SubFilter", CollectionAlphaBlend_SubFilter},{"ColorCollectionXorPixel", ColorCollectionXorPixel},{"BlendWithSource25",BlendWithSource25}, {"BlendWithSource50",BlendWithSource50}, {"BlendWithSource75",BlendWithSource75}, {"BlendWithSource100",BlendWithSource100},{"ColorCollectionXorOffsetFlash", ColorCollectionXorOffsetFlash},{"ColorCollectionMatrixGhost", ColorCollectionMatrixGhost},{"MildStrobe", MildStrobe},{"ReduceBy50", ReduceBy50},{"AlphaBlendWithSourceScale", AlphaBlendWithSourceScale},{"ColorPositionAverageXor", ColorPositionAverageXor},{"ColorPositionXor", ColorPositionXor},{"ColorPositionXorMedianBlend", ColorPositionXorMedianBlend},{"CannyStrobe", CannyStrobe},{"LaplacianStrobe", LaplacianStrobe},{"LaplacianStrobeOnOff", LaplacianStrobeOnOff},{"ColorCollectionPixelXor", ColorCollectionPixelXor},{"GrayStrobe", GrayStrobe},{"ColorStrobeXor",ColorStrobeXor},{"ColorGhost", ColorGhost},{"ColorCollectionTwitchSubFilter", ColorCollectionTwitchSubFilter},{"BlurredOutXor", BlurredOutXor},{"BoxFilter", BoxFilter},{"DizzyFilter", DizzyFilter},{"Buzzed", Buzzed},{"BlendWithImage25", BlendWithImage25}, {"BlendWithImage50",BlendWithImage50}, {"BlendWithImage75",BlendWithImage75},{"BuzzedDark", BuzzedDark},{"AlphaBlendImageDownUp", AlphaBlendImageDownUp},{"BlendWithImageAndSource", BlendWithImageAndSource},{"PixelSourceFrameBlend256", PixelSourceFrameBlend256},{"SplitMatrixCollection", SplitMatrixCollection},{"RectangleGlitch", RectangleGlitch},{"PositionShift", PositionShift},{"ColorCollectionMovementIndex", ColorCollectionMovementIndex},{"Shake", Shake},{"Disoriented", Disoriented},{"ColorCollectionPositionStrobe", ColorCollectionPositionStrobe},{"ColorCollectionStrobeBlend", ColorCollectionStrobeBlend},{"AlphaBlendStoredFrames", AlphaBlendStoredFrames},{"SplitMatrixSortChannel", SplitMatrixSortChannel},{"SplitMatrixSortChannelArrayPosition", SplitMatrixSortChannelArrayPosition},{"SplitMatrixSortChannelImage", SplitMatrixSortChannelImage},{"ShiftColorLeft", ShiftColorLeft},{"CycleInAndOutRepeat", CycleInAndOutRepeat},{"ColorCollectionShuffle", ColorCollectionShuffle},{"BlendFromXtoY", BlendFromXtoY},{"BlendImageXtoY", BlendImageXtoY},{"BlendColorImageStrobeSubFilter", BlendColorImageStrobeSubFilter},{"BlendByColsSubFilter", BlendByColsSubFilter},{"BlendByColsImage", BlendByColsImage},{"IntertwineX", IntertwineX}, {"IntertwineY", IntertwineY},{"IntertwineSubFilterX", IntertwineSubFilterX}, {"IntertwineSubFilterY", IntertwineSubFilterY},{"IntertwineImageX", IntertwineImageX}, {"IntertwineImageY", IntertwineImageY},{"InertwineImageSubFilter", InertwineImageSubFilter},{"IntertwineHorizontalImageSubFilter", IntertwineHorizontalImageSubFilter},{"InterwtineHorizontalImageSubFilterByIndex", InterwtineHorizontalImageSubFilterByIndex},{"IntertwineHorizontalImageSubFilterMatrixCollection", IntertwineHorizontalImageSubFilterMatrixCollection},{"BlendForwardAndBack16", BlendForwardAndBack16},{"BlendForwardAndBack32", BlendForwardAndBack32},{"BlendForwardAndBack64", BlendForwardAndBack64},{"BlendForwardAndBack8_RandomStrobe", BlendForwardAndBack8_RandomStrobe},{"AlphaBlendWithThreeCollections", AlphaBlendWithThreeCollections},{"AlphaBlendArrayRandom", AlphaBlendArrayRandom},{"AlphaBlendArrayTrailsFilters", AlphaBlendArrayTrailsFilters},{"AlphaBlendMirrorChannelSort", AlphaBlendMirrorChannelSort},{"ColorCollectionAlphaBlendArray", ColorCollectionAlphaBlendArray},{"AlphaBlendArrayExpand", AlphaBlendArrayExpand},{"ColorImageMatrixFade", ColorImageMatrixFade},{"ColorImageMatrixFadeFast", ColorImageMatrixFadeFast},{"ColorImageMatrixFadeDirection", ColorImageMatrixFadeDirection},{"ColorImageMatrixFadeDirectionBlend", ColorImageMatrixFadeDirectionBlend},{"ColorMatrixCollectionPixelation", ColorMatrixCollectionPixelation},{"ColorMatrixColllectionImagePixelation", ColorMatrixColllectionImagePixelation},{"ColorMatrixCollectionImagePixelationSubFilter", ColorMatrixCollectionImagePixelationSubFilter},{"ColorImageMatrixMedianBlend", ColorImageMatrixMedianBlend},{"ColorImageBlendPixelMedian", ColorImageBlendPixelMedian},{"ColorImageBlendOrigMedian", ColorImageBlendOrigMedian},{"ColorImageBlendSource", ColorImageBlendSource},{"ColorImageMatrixFadeOnAndOff", ColorImageMatrixFadeOnAndOff},{"ColorImageMatrixRect", ColorImageMatrixRect},{"ColorImageBackAndForth", ColorImageBackAndForth},{"ColorImageFadeReallyFast", ColorImageFadeReallyFast},{"ColorImageSubFilterXor", ColorImageSubFilterXor},{"ColorImageAllXor", ColorImageAllXor},{"ColorChannelIteration", ColorChannelIteration},{"ColorImageMatrixFadeVariable", ColorImageMatrixFadeVariable},{"ColorXorImageFade", ColorXorImageFade},{"ColorFreezeBlend", ColorFreezeBlend},{"ColorImageFill", ColorImageFill},{"ColorImageFillMatrix", ColorImageFillMatrix},{"ColorImageFillSubFilter", ColorImageFillSubFilter},{"ColorImagePixelsResetSubFilter", ColorImagePixelsResetSubFilter},{"ColorIncrementReset", ColorIncrementReset},{"ColorPixelArray2D", ColorPixelArray2D},{"ManualShell", ManualShell},{"ColorIncrementRandomReset", ColorIncrementRandomReset},{"ColorImageIncrementResetCollection", ColorImageIncrementResetCollection},{"ColorMoveDown", ColorMoveDown},{"ColorMoveDownResetMedianBlend", ColorMoveDownResetMedianBlend},{"ColorMoveDownSmoothMedianBlend", ColorMoveDownSmoothMedianBlend},{"PixelRandom3", PixelRandom3},{"ShiftMatrixLeft", ShiftMatrixLeft},{"ShiftMatrixLeftSubFilter", ShiftMatrixLeftSubFilter},{"ShiftMatrixUpSubFilter", ShiftMatrixUpSubFilter},{"PixelatePixelValues", PixelatePixelValues},{"StretchCollection", StretchCollection},{"PsychedelicSlitScan", PsychedelicSlitScan},{"SineValue", SineValue},{"SineTransitionUpLeft", SineTransitionUpLeft},{"TemporaryTrails", TemporaryTrails},{"StrobingPixelDissolve", StrobingPixelDissolve},{"ImagePixelFrameBlend", ImagePixelFrameBlend},{"PreviewFilters", PreviewFilters},{"EachFilterSubFilter", EachFilterSubFilter},{"EachFilterRandomStartSubFilter", EachFilterRandomStartSubFilter},{"PixelPsychosis", PixelPsychosis},{"PixelCloud", PixelCloud},{"PiixelXorBlendFrame", PiixelXorBlendFrame},{"PixelImageBlendFrame", PixelImageBlendFrame},{"PixelCollectionRandom", PixelCollectionRandom},{"PixelCollectionSubFilter", PixelCollectionSubFilter},{"PixelCollectionMatrixSubFilter", PixelCollectionMatrixSubFilter},{"PixelCollectionIncreaseSubFilter", PixelCollectionIncreaseSubFilter},{"PixelCollectionImageBlend", PixelCollectionImageBlend},{"ColorPulseIncrease", ColorPulseIncrease},{"PixelCollectionLongMatrixSubFilter", PixelCollectionLongMatrixSubFilter},{"ColorPulseImageIncreaseSubFilter", ColorPulseImageIncreaseSubFilter},{"ColorPulseRandom", ColorPulseRandom},{"ColorPulseImage", ColorPulseImage},{"ColorPulseAlpha", ColorPulseAlpha},{"ColorLower75", ColorLower75},{"ColorImageMedianBlend", ColorImageMedianBlend},{"ColorDullBlur", ColorDullBlur},{"ColorImageDull", ColorImageDull},{"ColorImageBlendWithFrame", ColorImageBlendWithFrame},{"ColorImageBlendSubFilter", ColorImageBlendSubFilter},{"ColorMatrixImageFilter", ColorMatrixImageFilter},{"ColorXorMatrixImage", ColorXorMatrixImage},{"MatrixColorBlur", MatrixColorBlur},{"ColorImageLower",ColorImageLower},{"BlurSubFilter8", BlurSubFilter8},{"LightBlend", LightBlend},{"LightBlendXor", LightBlendXor},{"Source_ImageSubFilter", Source_ImageSubFilter},{"XorZeroImage", XorZeroImage},{"SlowDownFilterSubFilter", SlowDownFilterSubFilter},{"VariableFilterSubFilter", VariableFilterSubFilter},{"SingleFrameGlitch", SingleFrameGlitch},{"ColorFadeSlow", ColorFadeSlow},{"FadeBetweenSubFilter", FadeBetweenSubFilter},{"FadeBetweenFrame", FadeBetweenFrame},{"TrailsTwitch", TrailsTwitch},{"Tremors", Tremors},{"XorImageIndexPixel", XorImageIndexPixel},{"ImageStrobeMedianBlend", ImageStrobeMedianBlend},{"StrobeImageArrayPosition", StrobeImageArrayPosition},{"OppositeImageArray", OppositeImageArray},{"NoMedianBlurBlendMultiThread", NoMedianBlurBlendMultiThread},{"NegateNoBlurMedian", NegateNoBlurMedian},{"PulseIncreaseVariableSpeed", PulseIncreaseVariableSpeed},{"Source25_Image75", Source25_Image75},{"LightStrobe", LightStrobe},{"StrobeRedChannel", StrobeRedChannel}, {"StrobeGreenChannel", StrobeGreenChannel},{"StrobeBlueChannel",StrobeBlueChannel},{"StrobeOnOffRandomChannel", StrobeOnOffRandomChannel},{"StrobeLightFlashRandomChannel", StrobeLightFlashRandomChannel},{"LostSideDeadImageSubFilter", LostSideDeadImageSubFilter},{"SwapImageFrameSubFilter", SwapImageFrameSubFilter},{"PulseIncreaseFast", PulseIncreaseFast},{"FibonacciXor", FibonacciXor},{"ColorFibonacci", ColorFibonacci},{"ImageFibonacci", ImageFibonacci},{"ImageFibonacciInAndOut", ImageFibonacciInAndOut},{"ImageFibonacciStrobe", ImageFibonacciStrobe},{"ImageFibonacciMedianBlend", ImageFibonacciMedianBlend},{"ImageFibonacciInAndOutSubFilter", ImageFibonacciInAndOutSubFilter},{"ImageKaleidoscopeSubFilter", ImageKaleidoscopeSubFilter},{"ImageMirrorLeftRight", ImageMirrorLeftRight},{"ImageMirrorShiftUpLeftDownRight", ImageMirrorShiftUpLeftDownRight},{"SelfScaleRefined", SelfScaleRefined},{"MetalMedianBlend", MetalMedianBlend},{"SelfScaleRefinedRGB", SelfScaleRefinedRGB},{"ImageFreezeReleaseRepeat", ImageFreezeReleaseRepeat},{"ImageReplaceColorIntensity", ImageReplaceColorIntensity},{"ImageReplaceColorIntensitySubFilter", ImageReplaceColorIntensitySubFilter},{"ImageReplaceColorIntensitySubFilterInOut", ImageReplaceColorIntensitySubFilterInOut},{"ImageFillColor", ImageFillColor},{"MultiRandFilter", MultiRandFilter},{"MultiFilter", MultiFilter},{"GradientRandom", GradientRandom},{"LineMedianBlend", LineMedianBlend},{"PerfectMedianBlend", PerfectMedianBlend},{"ImageRowAlphaSubFilter", ImageRowAlphaSubFilter},{"IndexPixelRowSubFilter", IndexPixelRowSubFilter},{"IndexSourceRowSubFilter", IndexSourceRowSubFilter},{"IndexSourceBlendSubFilter", IndexSourceBlendSubFilter},{"BlendFilterWithSubFilter", BlendFilterWithSubFilter},{"FadeFilterWithSubFilter", FadeFilterWithSubFilter},{"AlphaBlendByRowSubFilter", AlphaBlendByRowSubFilter},{"ColorMatrixTrailsSubFilter", ColorMatrixTrailsSubFilter},{"LoFi_320x240_Interlaced", LoFi_320x240_Interlaced},{"LoFi_320x240", LoFi_320x240},{"LoFi_320x240_SubFilter", LoFi_320x240_SubFilter},{"LoFi_320x240_Alpha_SubFilter", LoFi_320x240_Alpha_SubFilter},{"LoFi_160x120_SubFilter",LoFi_160x120_SubFilter},{"LoFi_Trails",LoFi_Trails},{"LoFi_ImageScaleSubFilter",LoFi_ImageScaleSubFilter},{"MedianShift",MedianShift},{"ImageAlienKaleidoscope",ImageAlienKaleidoscope},{"ImageMirror_Left",ImageMirror_Left},{"ImageMirror_Right",ImageMirror_Right},{"ImageMirror_Up",ImageMirror_Up},{"ImageMirror_Down",ImageMirror_Down},{"ImageMirror_LeftSubFilter",ImageMirror_LeftSubFilter},{"ImageMirror_RightSubFilter",ImageMirror_RightSubFilter},{"ImageMirror_UpSubFilter",ImageMirror_UpSubFilter}, {"ImageMirror_DownSubFilter",ImageMirror_DownSubFilter},{"ImageMirrorKaleidoscope", ImageMirrorKaleidoscope},{"ExactImageSubFilter", ExactImageSubFilter},{"AlphaImageSourceScale", AlphaImageSourceScale},{"GhostMirrorReversed", GhostMirrorReversed},{"GhostMirrorFade", GhostMirrorFade},{"MirrorFadeLeftRight", MirrorFadeLeftRight},{"FadeMirrorSubFilter", FadeMirrorSubFilter},{"MirrorFadeUpDown", MirrorFadeUpDown},{"DarkTrailsKaleidoscope", DarkTrailsKaleidoscope},{"MirrorFadeAll",MirrorFadeAll},{"KaleidoscopeMirrorSubFilter", KaleidoscopeMirrorSubFilter},{"ImageKaleidoscopeMirrorAlphaSubFilter", ImageKaleidoscopeMirrorAlphaSubFilter},{"BlendOppositesSubFilter", BlendOppositesSubFilter},{"AlphaBlendMirrorFade", AlphaBlendMirrorFade},{"DarkenMedianBlend", DarkenMedianBlend},{"ReduceMedianBlend", ReduceMedianBlend},{"DarkColors", DarkColors},{"DarkenChannelXorNoMedian", DarkenChannelXorNoMedian},{"DarkenStrobeMedianBlend", DarkenStrobeMedianBlend},{"DarkenImageAlphaBlend", DarkenImageAlphaBlend},{"MatrixStrobeTrails", MatrixStrobeTrails},{"MatrixStrobeMedianBlend", MatrixStrobeMedianBlend},{"ImageEnergyKaleidoscopeSubFilter", ImageEnergyKaleidoscopeSubFilter},{"ImageEnergyKaleidoscopeBlendSubFilter", ImageEnergyKaleidoscopeBlendSubFilter},{"ImageEnergyKaleidoscopeEvenSubFilter", ImageEnergyKaleidoscopeEvenSubFilter},{"ImageEnergyFadeSubFilter", ImageEnergyFadeSubFilter},{"ImageEnergizeBlendFilter", ImageEnergizeBlendFilter},{"ImageEnergizeSubFilter", ImageEnergizeSubFilter},{"MatrixCollectionSmoothedSubFilter", MatrixCollectionSmoothedSubFilter},{"MonoColorTrails", MonoColorTrails},{"TremorMonoColorTrails", TremorMonoColorTrails},{"StrobeMonoColorTrails", StrobeMonoColorTrails},{"StrobeEffectColorTrails", StrobeEffectColorTrails},{"PixelStrobeColorTrails", PixelStrobeColorTrails},{"ElectricImageFilter", ElectricImageFilter},{"ImageEnergyStrobeSubFilter", ImageEnergyStrobeSubFilter},{"Twin", Twin},{"TwinKaleidoscope", TwinKaleidoscope},{"MedianBlendMultiThread_2160p", MedianBlendMultiThread_2160p},{"MedianBlendMultiThreadByFour",MedianBlendMultiThreadByFour}, {"MedianBlendMultiThreadByEight", MedianBlendMultiThreadByEight},{"MedianBlendMultiThreadByTweleve", MedianBlendMultiThreadByTweleve},{"MedianBlendMultiThreadByThree", MedianBlendMultiThreadByThree},{"MedianBlendIncrease", MedianBlendIncrease},{"MedianBlendIncrease16", MedianBlendIncrease16},{"StrangeGlitch64", StrangeGlitch64},{"StrangeGlitch16", StrangeGlitch16},{"CollectionMatrixOutline", CollectionMatrixOutline},{"CollectionMatrixSubFilter", CollectionMatrixSubFilter},{"CollectionMatrixRandomMedianBlend", CollectionMatrixRandomMedianBlend},{"ImageCollectionMatrixOutline", ImageCollectionMatrixOutline},{"ImageCollectionMatrixOutlineSubFilter", ImageCollectionMatrixOutlineSubFilter},{"ImageCollectionMatrixFillSubFilter", ImageCollectionMatrixFillSubFilter},{"ImageCollectionMatrixFadeInOutSubFilter", ImageCollectionMatrixFadeInOutSubFilter},{"ImageCollectionMatrixIntensitySubFilter", ImageCollectionMatrixIntensitySubFilter},{"ImageCollectionMatrixMedianSubFilter", ImageCollectionMatrixMedianSubFilter},{"CollectionMatrxOutlineAlphaMedianBlend", CollectionMatrxOutlineAlphaMedianBlend},{"ImageSourceOrigXor", ImageSourceOrigXor},{"MatrixCollectionAuraTrails", MatrixCollectionAuraTrails},{"MatrixCollectionAuraTrails32", MatrixCollectionAuraTrails32},{"PixelIntensityFillSubFilter", PixelIntensityFillSubFilter},{"SmoothImageFrameSubFilter", SmoothImageFrameSubFilter},{"ImageCycleBlend", ImageCycleBlend},{"ImageCycleAlphaBlend", ImageCycleAlphaBlend},{"ImageCycleXor", ImageCycleXor},{"ImageCycleMedian", ImageCycleMedian},{"ImageCycleAlphaSubFilter", ImageCycleAlphaSubFilter},{"ImageCycleSmoothAlphaBlend", ImageCycleSmoothAlphaBlend},{"ImageCycleSmoothAlphaBlend32", ImageCycleSmoothAlphaBlend32},{"BlendWithSourceMedianBlendMultiThreadEight", BlendWithSourceMedianBlendMultiThreadEight},{"BlendWithImageMedianMutliThreadEight", BlendWithImageMedianMutliThreadEight},{"GradientBlendWithImage", GradientBlendWithImage},{"DarkColorMovement", DarkColorMovement},{"DarkColorFibonacci", DarkColorFibonacci},{"UseFilterOnAndOffSubFilter", UseFilterOnAndOffSubFilter},{"UseFilterOnAndOffEachQuarterSecondSubFilter", UseFilterOnAndOffEachQuarterSecondSubFilter},{"UseFilterOnAndOffByEightSubFilter", UseFilterOnAndOffByEightSubFilter},{"UseImageOnAndOffByEight", UseImageOnAndOffByEight},{"UseImageOnAndOff", UseImageOnAndOff},{"SelfAlphaScale_Down",SelfAlphaScale_Down},{"PsychoticVision", PsychoticVision},{"DarkPsychoticVision", DarkPsychoticVision},{"Mirror_ReverseColor", Mirror_ReverseColor},{"ImageColorCycleMorph", ImageColorCycleMorph},{"ImageBlendAndSubFilter", ImageBlendAndSubFilter},{"ImageSquareShrink", ImageSquareShrink},{"ImageSquareShrinkFast", ImageSquareShrinkFast},{"ImageSquareShrinkAlpha", ImageSquareShrinkAlpha},{"ImageSquareExpand", ImageSquareExpand},{"ImageKaleidoscopeNoBlur", ImageKaleidoscopeNoBlur},{"KaleidoscopeSubFilter", KaleidoscopeSubFilter},{"ImageSquareShrinkSubFilter", ImageSquareShrinkSubFilter},{"DifferenceReplaceSubFilter", DifferenceReplaceSubFilter},{"InvertedDifferenceReplaceSubFilter", InvertedDifferenceReplaceSubFilter},{"ImageInvertedDifferenceReplaceSubFilter", ImageInvertedDifferenceReplaceSubFilter},{"ImageDifferenceReplaceSubFilter", ImageDifferenceReplaceSubFilter},{"DifferenceReplaceSubFilterAlphaBlend", DifferenceReplaceSubFilterAlphaBlend},{"GradientGlitch", GradientGlitch},{"ImageGradientBlend", ImageGradientBlend},{"MedianBlendGradientMultiThread", MedianBlendGradientMultiThread},{"MedianBlendGradientDarkMultiThread", MedianBlendGradientDarkMultiThread},{"GradientAlphaBlend", GradientAlphaBlend},{"GradientFilter", GradientFilter},{"GradientFilterAlphaBlend", GradientFilterAlphaBlend},{"MedianBlendGradientFilterMultiThread", MedianBlendGradientFilterMultiThread},{"MedianBlendGraidentFilterDarkMultiThread", MedianBlendGraidentFilterDarkMultiThread},{"ColorOrderSwap", ColorOrderSwap},{"ColorOrderSwapMap", ColorOrderSwapMap},{"MedianBlendSwapMapMultiThread", MedianBlendSwapMapMultiThread},{"RandomGradientColors", RandomGradientColors},{"GradientColorMap", GradientColorMap},{"GradientXor", GradientXor},{"RandomSubFilter", RandomSubFilter},{"ShuffleSubFilter", ShuffleSubFilter},{"Shuffle_Filter", Shuffle_Filter},{"RandomOrigFrame", RandomOrigFrame},{"ColorVariableRectangles", ColorVariableRectangles},{"VariableRectangles", VariableRectangles},{"VariableRectanglesSimple", VariableRectanglesSimple},{"VariableRectanglesExtra", VariableRectanglesExtra},{"VariableRectangleImageAlphaBlend", VariableRectangleImageAlphaBlend},{"MirrorSwitchMode", MirrorSwitchMode},{"MirrorSwitchLeftRight", MirrorSwitchLeftRight},{"VariableRectanglesSmall", VariableRectanglesSmall},{"VariableRectanglesLarge", VariableRectanglesLarge},{"VariableRectanglesImageCollection", VariableRectanglesImageCollection},{"VariableRectanglesVariableImageSubFilter", VariableRectanglesVariableImageSubFilter},{"RainbowXorStrobeBlend", RainbowXorStrobeBlend},{"IncreaseRainbowXorBlend", IncreaseRainbowXorBlend},{"ColorStrobeIndexSubFilter", ColorStrobeIndexSubFilter},{"MatrixCollectionRandomFrames", MatrixCollectionRandomFrames},{"MatrixCollectionFrames", MatrixCollectionFrames},{"MatrixCollectionFramesLeft", MatrixCollectionFramesLeft},{"MatrixCollectionFramesMirrorLeft", MatrixCollectionFramesMirrorLeft},{"MatrixCollectionFramesTop", MatrixCollectionFramesTop},{"FrameSqueeze", FrameSqueeze},{"MatrixCollectionBlendLeftToRight", MatrixCollectionBlendLeftToRight},{"MedianBlendMatrixCollection", MedianBlendMatrixCollection},{"MedianBlendMatrixCollection_2160p", MedianBlendMatrixCollection_2160p},{"MedianBlendMatrixCollection_2160p_X16", MedianBlendMatrixCollection_2160p_X16},{"MatrixCollectionAdd", MatrixCollectionAdd},{"MatrixCollection_Xor", MatrixCollection_Xor},{"ImageMatrixCollectionBlend",ImageMatrixCollectionBlend},{"MatrixCollectionNegateAdd", MatrixCollectionNegateAdd},{"MatrixCollectionFrameRect", MatrixCollectionFrameRect},{"MatrixCollectionRows8x1_SubFilter", MatrixCollectionRows8x1_SubFilter},{"MatrixCollectionRows8x8_SubFilter", MatrixCollectionRows8x8_SubFilter},{"MatrixCollectionRows2x2_SubFilter", MatrixCollectionRows2x2_SubFilter},{"MatrixCollectionRows4x4_SubFilter", MatrixCollectionRows4x4_SubFilter},{"IntertwineBlock", IntertwineBlock},{"IntertwineVertical", IntertwineVertical},{"IntertwineImageVerticalSubFilter", IntertwineImageVerticalSubFilter},{"MirrorDiamond", MirrorDiamond},{"MirrorDiamondRight", MirrorDiamondRight},{"MirrorDiamondReverse", MirrorDiamondReverse},{"MirrorDiamondBlend", MirrorDiamondBlend},{"StretchOutward", StretchOutward},{"MirrorDiamondRandom", MirrorDiamondRandom},{"MatrixCollectionAlphaRow", MatrixCollectionAlphaRow},{"MedianBlendCollectionAlphaRow", MedianBlendCollectionAlphaRow},{"MedianBlendDoubleVision", MedianBlendDoubleVision},{"MedianBlendMultiThreadScale",MedianBlendMultiThreadScale},{"AcidShuffleMedian", AcidShuffleMedian},{"MedianBlendSmoothAlpha", MedianBlendSmoothAlpha},{"MirrorDiamondSubFilter", MirrorDiamondSubFilter},{"ImageFadeTo", ImageFadeTo},{"ImageFadeToXor", ImageFadeToXor},{"MedianBlendImageFadeTo", MedianBlendImageFadeTo},{"MedianMirrorAllBlend", MedianMirrorAllBlend},{"DigitalHaze", DigitalHaze},{"BlendWithColor", BlendWithColor},{"DigitalHazeBlend", DigitalHazeBlend},{"VideoAlphaBlend25", VideoAlphaBlend25},{"VideoAlphaBlend50", VideoAlphaBlend50},{"VideoAlphaBlend75", VideoAlphaBlend75},{"VideoAlphaBlendSubFilter", VideoAlphaBlendSubFilter},{"VideoExactFrame", VideoExactFrame},{"VideoAlphaFade", VideoAlphaFade},{"VideoAlphaImage", VideoAlphaImage},{"VideoAlphaImageFrame", VideoAlphaImageFrame},{"VideoAlphaImageScale", VideoAlphaImageScale},{"VideoAlphaBlendImage25",VideoAlphaBlendImage25},{"VideoAlphaBlendImage50",VideoAlphaBlendImage50},{"VideoAlphaBlendImage75",VideoAlphaBlendImage75},{"VideoFrameFilterSubFilter", VideoFrameFilterSubFilter},{"VideoFrameImageSubFilter", VideoFrameImageSubFilter},{"VideoAlphaSubFilter", VideoAlphaSubFilter},{"VideoRoll", VideoRoll},{"VideoRollReverse", VideoRollReverse},{"VideoRollMedianBlend", VideoRollMedianBlend},{"VideoRollScaleMedianBlend", VideoRollScaleMedianBlend},{"VideoSubFilter", VideoSubFilter},{"MedianBlendWithColor", MedianBlendWithColor},{"VideoImage", VideoImage},{"VideoImageFade", VideoImageFade},{"ColorMovement", ColorMovement},{"MedianBlendColorMovement", MedianBlendColorMovement},{"ColorMovementRange", ColorMovementRange},{"ColorPixelOrder", ColorPixelOrder},{"ColorPixelOrder32", ColorPixelOrder32},{"MedianBlendMuliThread_Pixelate", MedianBlendMuliThread_Pixelate},{"SelfScaleGlitch", SelfScaleGlitch},{"PixelGlitch", PixelGlitch},{"VideoPixelOnOffSwitch", VideoPixelOnOffSwitch},{"VideoBlendImageSubFilter", VideoBlendImageSubFilter},{"VideoAlphaImageSubFilter",VideoAlphaImageSubFilter},{"MedianBlendVideo", MedianBlendVideo},{"VideoFrameBlur8",VideoFrameBlur8},{"VideoFrameBlur16", VideoFrameBlur16}, {"VideoFrameBlur32", VideoFrameBlur32},{"VideoSourceFrameBlur8", VideoSourceFrameBlur8},{"VideoSourceFrameBlur16", VideoSourceFrameBlur16}, {"VideoSourceFrameBlur32", VideoSourceFrameBlur32},{"VideoImageSourceFrame", VideoImageSourceFrame},{"slowRed", slowRed}, {"slowGreen", slowGreen}, {"slowBlue", slowBlue},{"VideoMirrorAlpha", VideoMirrorAlpha},{"VideoMirrorLeftRight", VideoMirrorLeftRight},{"VideoFlipFrames", VideoFlipFrames},{"VideoImageBlend", VideoImageBlend},{"VideoCycleReverse", VideoCycleReverse},{"VideoCycleMedianReverse",VideoCycleMedianReverse},{"VideoStrobeFrameOnOff", VideoStrobeFrameOnOff},{"VideoMirror", VideoMirror},{"VideoMedianBlendScale", VideoMedianBlendScale},{"VideoMedianBlend", VideoMedianBlend},{"VideoBlendSubFilter", VideoBlendSubFilter},{"VideoLoFi", VideoLoFi},{"VideoLoFiBlend", VideoLoFiBlend},{"VideoLoFiSourceNormal", VideoLoFiSourceNormal},{"VideoDarken25", VideoDarken25}, {"VideoDarken50", VideoDarken50}, {"VideoDarken75",VideoDarken75},{"VideoDarkenBlend25",VideoDarkenBlend25}, {"VideoDarkenBlend50", VideoDarkenBlend50},{"VideoDarkenBlend75",VideoDarkenBlend75},{"VideoBlendSmooth8",VideoBlendSmooth8},{"VideoBlendSmooth16",VideoBlendSmooth16},{"VideoBlendSmooth32",VideoBlendSmooth32},{"VideoXorSource", VideoXorSource},{"VideoXorSelfScale", VideoXorSelfScale},{"MedianBlendNoBlurWithColor", MedianBlendNoBlurWithColor},{"VideoImageSmooth", VideoImageSmooth},{"ColorMapFilter", ColorMapFilter},{"VideoColorMap", VideoColorMap},{"VideoColorMapAlphaBlend", VideoColorMapAlphaBlend},{"ReduceColors", ReduceColors},{"ReduceColorsRandom", ReduceColorsRandom},{"ReduceColorsRed", ReduceColorsRed},{"ReduceColorsGreen", ReduceColorsGreen}, {"ReduceColorsBlue", ReduceColorsBlue},{"BlendRandomPixels", BlendRandomPixels}, {"MedianBlendMultiThread_LoFi", MedianBlendMultiThread_LoFi},{"ExpandFrame", ExpandFrame},{"ExpandImage", ExpandImage},{"ExpandVideo", ExpandVideo},{"ColorIncreaseFadeRGB", ColorIncreaseFadeRGB},{"ColorIncreaseInOutRGB", ColorIncreaseInOutRGB},{"AlphaVideoXor", AlphaVideoXor},{"DarkScaleNoBlur", DarkScaleNoBlur},{"LightBlur", LightBlur},{"LightMedianBlendMultiThread", LightMedianBlendMultiThread},{"ColorFadeMedianBlendMultiThread", ColorFadeMedianBlendMultiThread},{"ColorFadeBlendMedianBlendMultiThread", ColorFadeBlendMedianBlendMultiThread},{"MedianBlendMultiThread32", MedianBlendMultiThread32},{"MedianBlendMultiThread4", MedianBlendMultiThread4},{"MedianBlendFadeWithColor", MedianBlendFadeWithColor},{"GaussianBlurMild", GaussianBlurMild},{"MedianBlendWithColorValue", MedianBlendWithColorValue},{"MedianBlendWithColorInOutRGB", MedianBlendWithColorInOutRGB},{"MedianBlendWithCollection16", MedianBlendWithCollection16},{"FadeRtoGtoB", FadeRtoGtoB},{"MedianBlendFadeRtoGtoB", MedianBlendFadeRtoGtoB},{"FadeRtoGtoB_Increase", FadeRtoGtoB_Increase},{"MedianBlendFadeRtoGtoB_Increase", MedianBlendFadeRtoGtoB_Increase},{"FadeRandomChannel", FadeRandomChannel},{"FadeRandomChannel_Increase", FadeRandomChannel_Increase},{"MedianBlendFadeRandomChannel", MedianBlendFadeRandomChannel},{"GlitchyTrails", GlitchyTrails},{"GlitchyXorTrails", GlitchyXorTrails},{"GlitchedFilter", GlitchedFilter},{"DisplaySometimesSubFilter", DisplaySometimesSubFilter},{"GlitchyVideoXorTrails", GlitchyVideoXorTrails},{"GltichedVideoFilter", GltichedVideoFilter},{"DualGitchyVideoXorTrails", DualGitchyVideoXorTrails},{"StaticGlitch", StaticGlitch},{"VideoMirrorMultiDirection", VideoMirrorMultiDirection},{"SoftwareGlitch_64", SoftwareGlitch_64},{"VideoMatrixBlendDouble", VideoMatrixBlendDouble},{"VideoMatrixFadeDouble", VideoMatrixFadeDouble},{"VideoMatrixColorSmooth", VideoMatrixColorSmooth},{"VideoMedianBlendShared8", VideoMedianBlendShared8},{"VideoMedianBlendShared16", VideoMedianBlendShared16},{"GlitchedGrid", GlitchedGrid},{"ShuffleGlitch", ShuffleGlitch},{"ShuffleVideoMatrix", ShuffleVideoMatrix},{"TruncateColor", TruncateColor},{"TruncateColor25", TruncateColor25}, {"TruncateColor75", TruncateColor75},{"TruncateVariable", TruncateVariable},{"TruncateVariableSlow", TruncateVariableSlow},{"TruncateVariableScale", TruncateVariableScale},{"MedianBlendTruncate", MedianBlendTruncate},{"MedianBlendTruncateScale", MedianBlendTruncateScale},{"Variable_ColorScale", Variable_ColorScale},{"MedianBlendVariable_ColorScale", MedianBlendVariable_ColorScale},{"InOut_ColorScale", InOut_ColorScale},{"MedianBlendInOut_ColorScale", MedianBlendInOut_ColorScale},{"CollectionTruncate", CollectionTruncate},{"MedianBlendCollectionTruncate", MedianBlendCollectionTruncate},{"VideoBlendTruncate", VideoBlendTruncate},{"RandomTruncateFrame", RandomTruncateFrame},{"RandomStrobeMedianBlend", RandomStrobeMedianBlend},{"XorFade", XorFade},{"ColorIncreaseXorFade", ColorIncreaseXorFade},{"Pixels_InOut", Pixels_InOut},{"PixelDistortionCloud", PixelDistortionCloud},{"XorRow", XorRow},{"UseOldRow", UseOldRow},{"UseEveryOtherRow", UseEveryOtherRow},{"UseOffRow", UseOffRow},{"FrameJump", FrameJump},{"MedianBlendMultiThreadVariable", MedianBlendMultiThreadVariable},{"MedianBlendMultiThreadVariable32", MedianBlendMultiThreadVariable32},{"MedianBlendMultiThreadVariable16", MedianBlendMultiThreadVariable16},{"MedianBlendMultiThreadSkip8", MedianBlendMultiThreadSkip8},{"BlockyTrails", BlockyTrails},{"BlockyTrails16", BlockyTrails16},{"BlockyTrails32", BlockyTrails32},{"BlockyTrails64", BlockyTrails64},{"UseOffRowVert", UseOffRowVert},{"UseOldRowVert", UseOldRowVert},{"UseEveryOtherRowVert", UseEveryOtherRowVert},{"UseOffRowDir", UseOffRowDir},{"UseOldRowDir",UseOldRowDir}, {"UseEveryOtherRowDir", UseEveryOtherRowDir},{"SetCurrentFrameStateAsSource", SetCurrentFrameStateAsSource},{"RegularTrails", RegularTrails},{"UseOldRow64", UseOldRow64},{"UseOldRowVert64", UseOldRowVert64},{"UseOldRowDir64", UseOldRowDir64},{"GradientColorBlend", GradientColorBlend},{"MedianBlendMultiThreadGradientGray", MedianBlendMultiThreadGradientGray},{"GradientRedBlend", GradientRedBlend},{"GradientGreenBlend",GradientGreenBlend}, {"MedianBlendMultiThreadGradientGreen",MedianBlendMultiThreadGradientGreen}, {"GradientBlueBlend",GradientBlueBlend}, {"MedianBlendMultiThreadGradientBlue", MedianBlendMultiThreadGradientBlue},{"GradientColorBlendAll", GradientColorBlendAll}, {"MedianBlendMultiThreadGradientAll",MedianBlendMultiThreadGradientAll},{"GradientXRed", GradientXRed},{"GradientXGreen", GradientXGreen},{"GradientXBlue", GradientXBlue},{"GradientAll", GradientAll},{"MedianBlendMultiThreadAllGradients", MedianBlendMultiThreadAllGradients},{"StretchRowMatrix16", StretchRowMatrix16},{"StretchRowMatrix32", StretchRowMatrix32},{"StretchRowMatrix8", StretchRowMatrix8},{"StretchColMatrix8",StretchColMatrix8},{"StretchColMatrix16",StretchColMatrix16},{"StretchColMatrix32",StretchColMatrix32},{"GradientRandomSwitch", GradientRandomSwitch},{"GradientFlashColor", GradientFlashColor},{"GradientFlashComponent", GradientFlashComponent},{"GradientRandomShift", GradientRandomShift},{"MedianBlendMultiThreadGradientShift", MedianBlendMultiThreadGradientShift},{"MirrorIntertwineRows720", MirrorIntertwineRows720},{"MirrorIntertwineRowsY", MirrorIntertwineRowsY},{"MirrorIntertwineRowsX_Y_Width_Height", MirrorIntertwineRowsX_Y_Width_Height},{"MirrorTrailsLeft", MirrorTrailsLeft},{"MirrorTrailsLeftRightTopBottom", MirrorTrailsLeftRightTopBottom},{"MirrorTrailsRightLeftBottomTop", MirrorTrailsRightLeftBottomTop},{"MirrorTrailsFlash", MirrorTrailsFlash},{"MirrorLeftTopToBottom",MirrorLeftTopToBottom}, {"MirrorRightBottomToTop", MirrorRightBottomToTop},{"MirrorFlipLeft", MirrorFlipLeft},{"MirrorFlipRight", MirrorFlipRight},{"MirrorFlipBottomLeft", MirrorFlipBottomLeft}, {"MirrorFlipBottomRight", MirrorFlipBottomLeft},{"MirrorFlipXMirrorLeft", MirrorFlipXMirrorLeft}, {"MirrorFlipXMirrorRight", MirrorFlipXMirrorRight},{"MirrorFlipYMirrorLeft", MirrorFlipYMirrorLeft}, {"MirrorFlipYMirrorRight", MirrorFlipYMirrorRight},{"MirrorFlipXLeftTopToBottom",MirrorFlipXLeftTopToBottom}, {"MirrorFlipXLeftBottomToTop",MirrorFlipXLeftBottomToTop}, {"MirrorFlipXRightTopToBottom",MirrorFlipXRightTopToBottom}, {"MirrorFlipXRightBottomToTop",MirrorFlipXRightBottomToTop}, {"MirrorFlipYLeftTopToBottom", MirrorFlipYLeftTopToBottom},{"MirrorFlipYLeftBottomToTop",MirrorFlipYLeftBottomToTop}, {"MirrorFlipYRightTopToBottom",MirrorFlipYRightTopToBottom}, {"MirrorFlipYRightBottomToTop",MirrorFlipYRightBottomToTop},{"HorizontalGlitch", HorizontalGlitch},{"VerticalGlitch", VerticalGlitch},{"HorizontalXor", HorizontalXor},{"VerticalXor", VerticalXor},{"Vertical_Horizontal_Glitch", Vertical_Horizontal_Glitch},{"HorizontalSplitGlitch", HorizontalSplitGlitch},{"VerticalSplitGlitch", VerticalSplitGlitch},{"HorizontalRandomLine", HorizontalRandomLine},{"VerticalRandomLine", VerticalRandomLine},{"PixelInterlace", PixelInterlace},{"PixelInterlaceColSkip", PixelInterlaceColSkip},{"PixelInterlaceRowSkip", PixelInterlaceRowSkip},{"StartOffsetInterlace", StartOffsetInterlace},{"WaveTrails", WaveTrails},{"WaveTrailsAura", WaveTrailsAura},{"ImageInterlaceWave", ImageInterlaceWave},{"ColorWaveTrails", ColorWaveTrails},{"StrobePixelTrails", StrobePixelTrails},{"RectangleTrails", RectangleTrails},{"RectangleXY_Trails", RectangleXY_Trails},{"MedianBlendImage", MedianBlendImage},{"GhostWaveTrails", GhostWaveTrails},{"VideoPixelFade", VideoPixelFade},{"ImagePixelFade", ImagePixelFade},{"CollectionPixelFade", CollectionPixelFade},{"CollectionPixelFadeSubFilter", CollectionPixelFadeSubFilter},{"DiamondWave", DiamondWave},{"RGBWave",RGBWave},{"VideoCollectionOutline", VideoCollectionOutline},{"VideoSaturateAdd", VideoSaturateAdd},{"VideoSmoothMedianBlend", VideoSmoothMedianBlend},{"Square_Blocks", Square_Blocks},{"Square_Block_Resize", Square_Block_Resize},{"Square_Block_Resize_Vertical", Square_Block_Resize_Vertical},{"Square_Block_Resize_Reset", Square_Block_Resize_Reset}, {"Square_Block_Resize_Vert_Reset", Square_Block_Resize_Vert_Reset},{"Square_Block_Resize_Video", Square_Block_Resize_Video},{"Square_Block_Resize_Vert_Video", Square_Block_Resize_Vert_Video},{"Square_Block_Resize_Dir", Square_Block_Resize_Dir},{"Square_Block_Resize_All", Square_Block_Resize_All},{"VideoBlendGradient", VideoBlendGradient},{"VideoMatrixBlend", VideoMatrixBlend},{"VideoMatrixBlendAlpha", VideoMatrixBlendAlpha},{"VideoMatrixBlendAlphaRandom", VideoMatrixBlendAlphaRandom},{"VideoMatrixSwitch", VideoMatrixSwitch},{"VideoCollectionWave", VideoCollectionWave},{"CollectionWave", CollectionWave},{"TremorShake", TremorShake},{"PixelateSquares", PixelateSquares},{"ColorPixelDoubleXor", ColorPixelDoubleXor},{"VideoCollectionOffsetBlend", VideoCollectionOffsetBlend},{"VideoTransparent", VideoTransparent},{"VideoStripes", VideoStripes},{"VideoSmoothGradient", VideoSmoothGradient},{"SmoothSelfBlend", SmoothSelfBlend},{"MedianBlendMultiThread_Half", MedianBlendMultiThread_Half},{"MedianBlendMultiThread_Variable", MedianBlendMultiThread_Variable},{"FiftyPercentSubFilter",FiftyPercentSubFilter}, {"VariablePercentSubFilter",VariablePercentSubFilter},{"TwentyFivePercentSubFilter", TwentyFivePercentSubFilter},{"SeventyFivePercentSubFilter", SeventyFivePercentSubFilter},{"VideoRowGlitch", VideoRowGlitch},{"VideoXor_Frame", VideoXor_Frame},{"VideoSlideRGB", VideoSlideRGB},{"VideoSlide", VideoSlide},{"VideoSlideOffset", VideoSlideOffset},{"VideoSlideOffsetX", VideoSlideOffsetX},{"VideoSlideOffsetXRev", VideoSlideOffsetXRev},{"VideoSlideOffsetRGB", VideoSlideOffsetRGB},{"VideoSlideOffsetRGBRev", VideoSlideOffsetRGBRev},{"SelfSlideOffsetX", SelfSlideOffsetX},{"SelfSlideOffsetRGB", SelfSlideOffsetRGB},{"SelfSlideOffsetRGB_X", SelfSlideOffsetRGB_X},{"MedianBlendMultiThreadStrobe", MedianBlendMultiThreadStrobe},{"VideoXor_XY", VideoXor_XY},{"VideoRandomWave", VideoRandomWave},{"RandomWave", RandomWave},{"RandomWaveStartStop", RandomWaveStartStop},{"RandomMedianFrame", RandomMedianFrame},{"VideoSmoothDoubleAlphaBlend", VideoSmoothDoubleAlphaBlend},{"VariableLines", VariableLines},{"VariableLinesOffset", VariableLinesOffset},{"VaraibleLinesRectangle", VaraibleLinesRectangle},{"VariableLinesRectangleSource", VariableLinesRectangleSource},{"VariableLinesStartRectangle", VariableLinesStartRectangle},{"VariableLinesStartRectangleBlend", VariableLinesStartRectangleBlend},{"VariableLinesOnOffBlend", VariableLinesOnOffBlend},{"VariableCollectionLinesOffBlend", VariableCollectionLinesOffBlend},{"VariableDistortionWave", VariableDistortionWave},{"VariableDistortionReset", VariableDistortionReset},{"VariableLinesY", VariableLinesY}, {"VariableLinesY_Blend", VariableLinesY_Blend},{"VariableLinesY_Wave", VariableLinesY_Wave},{"VariableLinesXY_Interlaced", VariableLinesXY_Interlaced},{"VariableLinesExpand", VariableLinesExpand},{"VariableLinesExpandBlend", VariableLinesExpandBlend},{"CollectionXor4", CollectionXor4},{"PixelateExpandDistort", PixelateExpandDistort},{"PixelateExpandDistortX", PixelateExpandDistortX}, {"PixelateExpandDistortY", PixelateExpandDistortY},{"PixelateExpandDistortExtra", PixelateExpandDistortExtra},{"RectangleSpin", RectangleSpin},{"RectanglePlotXY", RectanglePlotXY},{"RectangleLines", RectangleLines},{"DifferenceFillLinesXor", DifferenceFillLinesXor},{"DistortPixelate", DistortPixelate},{"DistortPixelate64_Slow", DistortPixelate64_Slow},{"DistortPixelate128_SubFilter", DistortPixelate128_SubFilter},{"DifferenceFillLines_SubFilter", DifferenceFillLines_SubFilter},{"DifferenceFillLinesBlend_SubFilter", DifferenceFillLinesBlend_SubFilter},{"DistortPixelate24_SubFilter", DistortPixelate24_SubFilter},{"MovementTrails_SubFilter", MovementTrails_SubFilter},{"MovementTrailsX_SubFilter", MovementTrailsX_SubFilter},{"FadeFromFrameToFrame", FadeFromFrameToFrame},{"GlitchFadeFromFrameToFrame", GlitchFadeFromFrameToFrame},{"RandomSolo", RandomSolo},{"PiecesOfFrames", PiecesOfFrames},{"XorScaleValue", XorScaleValue},{"DiamondCollection", DiamondCollection},{"RandomFadeDelay", RandomFadeDelay},{"ColorRowShift", ColorRowShift},{"ColorRowShiftUp", ColorRowShiftUp},{"ColorRowShiftLeft", ColorRowShiftLeft}, {"ColorRowShiftRight", ColorRowShiftRight},{"MirrorFadeDelay", MirrorFadeDelay},{"MirrorRandomNow", MirrorRandomNow},{"ScanlineBlack", ScanlineBlack},{"ScanlineSubFilter", ScanlineSubFilter},{"VerticalYSubFilter", VerticalYSubFilter},{"ScanlineLessSubFilter",ScanlineLessSubFilter}, {"VerticalYLessSubFilter", VerticalYLessSubFilter},{"HorizontalColorOffset", HorizontalColorOffset},{"HorizontalColorOffsetLargeSizeSubFilter", HorizontalColorOffsetLargeSizeSubFilter},{"ErodeKernelSubFilter", ErodeKernelSubFilter},{"DilateKernelSubFilter", DilateKernelSubFilter},{"ErodeKernelOffSubFilter",ErodeKernelOffSubFilter}, {"DilateKernelOffSubFilter", DilateKernelOffSubFilter},{"ColorKeySetOnSubFilter",ColorKeySetOnSubFilter}, {"ColorKeySetOffSubFilter",ColorKeySetOffSubFilter},{"Warp", Warp},{"WarpDir", WarpDir},{"WarpTopLeft", WarpTopLeft},{"WarpRandom", WarpRandom},{"WarpStretch", WarpStretch},{"RandomLineGlitchSubFilter", RandomLineGlitchSubFilter},{"VerticalColorOffsetLargeSizeSubFilter", VerticalColorOffsetLargeSizeSubFilter},{"PreviousFrameXor", PreviousFrameXor},{"LightScanlineSubFilter", LightScanlineSubFilter},{"ScaleUpDown", ScaleUpDown},{"VideoTransitionInOut_SubFilter", VideoTransitionInOut_SubFilter},{"VideoDisplayPercent_SubFilter", VideoDisplayPercent_SubFilter},{"MovementToBlackSubFilter", MovementToBlackSubFilter},{"Black", Black},{"VideoFrameRGB", VideoFrameRGB},{"VideoAlphaBlendFade", VideoAlphaBlendFade},{"VideoAlphaAddFade", VideoAlphaAddFade},{"VideoAlphaAddFadeSubFilter", VideoAlphaAddFadeSubFilter},{"DelayOnOffSubFilter", DelayOnOffSubFilter},{"VideoImageBlendAlpha", VideoImageBlendAlpha}, {"IntertwineCols640",IntertwineCols640},{"HorizontalOffsetLess_SubFilter", HorizontalOffsetLess_SubFilter}, {"VerticalOffsetLess_SubFilter", VerticalOffsetLess_SubFilter},{"SquareOffsetLess_SubFilter", SquareOffsetLess_SubFilter},{"SquareOffset_SubFilter", SquareOffset_SubFilter},{"PrevFrameNotEqual", PrevFrameNotEqual},{"MirrorDelayLeft", MirrorDelayLeft},{"MirrorDelayRight", MirrorDelayRight},{"ApplyColorRange", ApplyColorRange},{"IntertwineAlphaBlend", IntertwineAlphaBlend},{"BlackLines", BlackLines},{"MedianBlendMultiThread2", MedianBlendMultiThread2},{"MedianBlendMultiThread2_ColorChange", MedianBlendMultiThread2_ColorChange},{"ApplyColorRangeInverted", ApplyColorRangeInverted},{"VariablesExtraHorizontal", VariablesExtraHorizontal},{"ChannelSortDelay", ChannelSortDelay},{"DizzyMode", DizzyMode},{"IntertwineColormap", IntertwineColormap},{"RotateFrame", RotateFrame},{"GhostShift", GhostShift},{"RotateSet", RotateSet},{"RotateFrameReverse", RotateFrameReverse},{"RotateSetReverse", RotateSetReverse},{"AuraGhostRotate", AuraGhostRotate},{"AuraGhostRotateReverse", AuraGhostRotateReverse},{"AuraGhostCollectionEnergy", AuraGhostCollectionEnergy},{"BlendSourceImageAndVideo", BlendSourceImageAndVideo},{"SetImageAndVideoBlend", SetImageAndVideoBlend},{"ThresholdDark", ThresholdDark},{"ThresholdMedianBlend", ThresholdMedianBlend},{"Threshold", Threshold},{"RemoveLowRedLevel", RemoveLowRedLevel},{"RemoveLowGreenLevel", RemoveLowGreenLevel},{"RemoveLowBlueLevel", RemoveLowBlueLevel},{"IncreaseLowRedLevel",IncreaseLowRedLevel}, {"IncreaseLowGreenLevel", IncreaseLowGreenLevel}, {"IncreaseLowBlueLevel", IncreaseLowBlueLevel},{"Zoom", Zoom},{"IntertwineVideo640", IntertwineVideo640},{"IntertwineCols640x8", IntertwineCols640x8},{"ZoomCorner", ZoomCorner},{"ZoomRandom", ZoomRandom},{"MedianBlendByLowValue", MedianBlendByLowValue},{"MedianBlendByIncreasingValue",  MedianBlendByIncreasingValue},{"AuraZoomMedianBlend", AuraZoomMedianBlend},{"OffTrackX", OffTrackX},{"OffTrackY", OffTrackY},{"FrameAlphaInsert", FrameAlphaInsert},{"FrameAlphaInsert_SubFilter", FrameAlphaInsert_SubFilter},{"FrameAlphaBlend_SubFilter", FrameAlphaBlend_SubFilter},{"SlowTrails_SubFilter", SlowTrails_SubFilter},{"AddCollectionSubFilter", AddCollectionSubFilter},{"AddCollectionXor_SubFilter", AddCollectionXor_SubFilter},{"ProperTrails", ProperTrails},{"ProperTrails_SubFilter", ProperTrails_SubFilter},{"StuckFrame_SubFilter",StuckFrame_SubFilter},{"XorLag", XorLag},{"PixelateBlend", PixelateBlend},{"PixelateRect", PixelateRect},{"RGBSplitFilter", RGBSplitFilter},{"DiagPixel", DiagPixel},{"DiagPixelY", DiagPixelY},{"DiagPixelY2", DiagPixelY2},{"DiagPixelY3", DiagPixelY3},{"DiagPixelY4", DiagPixelY4},{"ExpandLeftRight", ExpandLeftRight},{"DiagSquare", DiagSquare},{"DiagSquareLarge", DiagSquareLarge},{"DiagonalSquareCollection", DiagonalSquareCollection},{"DiagnoalSquareSize", DiagnoalSquareSize},{"DiagonalSquareSizeOnOff", DiagonalSquareSizeOnOff},{"DiagonalSquareSizeOnOffRandom", DiagonalSquareSizeOnOffRandom},{"SlitScanGUI", SlitScanGUI},{"SlitScanRandom", SlitScanRandom},{"VideoInterlacedRandom", VideoInterlacedRandom},{"VideoSlitScan", VideoSlitScan},{"FloatFade", FloatFade},{"SquareShift", SquareShift},{"SquareShift8", SquareShift8},{"SquareShift16", SquareShift16},{"FloatFadeRandomIncrease", FloatFadeRandomIncrease},{"FloatFadeRGB",FloatFadeRGB},{"SquareShiftDir", SquareShiftDir},{"SquareShiftExpand", SquareShiftExpand},{"FloatFadeVertical", FloatFadeVertical},{"LineTrails", LineTrails},{"SquareShiftDirVertical", SquareShiftDirVertical},{"StretchLineRow", StretchLineRow},{"StretchLineCol", StretchLineCol},{"StretchLineRowInc", StretchLineRowInc},{"StretchLineColInc", StretchLineColInc},{"StretchRowSplit", StretchRowSplit},{"VideoAddBlend", VideoAddBlend},{"VideoFadeInAndOut", VideoFadeInAndOut},{"VideoFadeRGB", VideoFadeRGB},{"VideoSubtract", VideoSubtract},{"StretchLineRowLeftRight", StretchLineRowLeftRight},{"FadeInOutBlendRGB", FadeInOutBlendRGB},{"FadeInOutReverseRGB", FadeInOutReverseRGB},{"PixelateRandom", PixelateRandom},{"PixelateFillRandom", PixelateFillRandom},{"JaggedLine", JaggedLine},{"ScratchyTrails", ScratchyTrails},{"ExpandPixelate", ExpandPixelate},{"VideoRandom", VideoRandom},{"VideoCollectionTwitch", VideoCollectionTwitch},{"DiagSquare8", DiagSquare8},{"DiagSquareRandom", DiagSquareRandom},{"SlitScanXGUI", SlitScanXGUI},{"DiagSquareX", DiagSquareX},{"SlitScanDir", SlitScanDir},{"VideoScanline", VideoScanline},{"VideoSquareRandom", VideoSquareRandom},{"VideoScanlineBlend", VideoScanlineBlend},{"VideoSplitColumn", VideoSplitColumn},{"VideoXStatic", VideoXStatic},{"ScanlineCollection", ScanlineCollection},{"SquareShiftDirRGB", SquareShiftDirRGB},{"StretchLineRowIncRGB",StretchLineRowIncRGB},{"StretchLineColIncRGB", StretchLineColIncRGB},{"StretchLineRowIncSource", StretchLineRowIncSource},{"StretchLineColIncSource", StretchLineColIncSource},{"AlternateAlpha", AlternateAlpha},{"Square_Block_Resize_Vertical_RGB", Square_Block_Resize_Vertical_RGB},{"DiagSquareRGB", DiagSquareRGB},{"ShiftPixelsRGB", ShiftPixelsRGB},{"UseOldRowVertRGB", UseOldRowVertRGB},{"ColorWaveTrailsRGB", ColorWaveTrailsRGB},{"MirrorDiamondRGB", MirrorDiamondRGB},{"Square_Block_Resize_RGB", Square_Block_Resize_RGB},{"VariableLinesY_RGB", VariableLinesY_RGB},{"MedianBlendMultiThread_RGB", MedianBlendMultiThread_RGB},{"SlitScanXGUI_RGB", SlitScanXGUI_RGB},{"SlitScanGUI_RGB", SlitScanGUI_RGB},{"SlitScanDir_RGB", SlitScanDir_RGB},{"SquareShiftDirGradient", SquareShiftDirGradient},{"MovementTrailsXRGB_SubFilter", MovementTrailsXRGB_SubFilter},{"TwistedVision_RGB", TwistedVision_RGB},{"CollectionWave_RGB", CollectionWave_RGB},{"BlendWithSourcePercent", BlendWithSourcePercent},{"SlitScan_Colors", SlitScan_Colors},{"ReverseRandom", ReverseRandom},{"ReverseRandomCollection", ReverseRandomCollection},{"RevesreRandomChannelCollection", RevesreRandomChannelCollection},{"ReverseRandomCollectionResize", ReverseRandomCollectionResize},{"SquareBlockGlitch", SquareBlockGlitch},{"SquareStretchRows", SquareStretchRows},{"SquareStretchRowsDelay", SquareStretchRowsDelay},{"SquareStretchEven", SquareStretchEven},{"SketchFilter", SketchFilter},{"SquareStretchEvenCollection", SquareStretchEvenCollection},{"SquareStretchEven32", SquareStretchEven32},{"RGBLineFuzz", RGBLineFuzz},{"RGBLineFuzzX", RGBLineFuzzX},{"ChannelSortCollection32", ChannelSortCollection32},{"LinesAcrossX", LinesAcrossX},{"XorLineX", XorLineX},{"AlphaComponentIncrease", AlphaComponentIncrease},{"ExpandContract", ExpandContract},{"MoveInThenMoveOut", MoveInThenMoveOut},{"MoveInThenMoveOutFast", MoveInThenMoveOutFast},{"CurrentDesktop", CurrentDesktop},{"DistortionFuzz", DistortionFuzz},{"DistortionByRow", DistortionByRow},{"DistortionByRowRev", DistortionByRowRev},{"DistortionByRowVar", DistortionByRowVar},{"DistortionByRowRand", DistortionByRowRand},{"DistortionByCol", DistortionByCol},{"DistortionByColRand", DistortionByColRand},{"DistortionByColVar", DistortionByColVar},{"LongLines", LongLines},{"TearRight", TearRight},{"CurrentDesktopRect", CurrentDesktopRect},{"TearDown", TearDown},{"TearUp", TearUp},{"TearLeft", TearLeft},{"SyphonInputVideo", SyphonInputVideo},{"DistortStretch", DistortStretch},{"MultiVideoBlend",MultiVideoBlend},{"MultiVideoAlphaBlend", MultiVideoAlphaBlend},
        {"MultiVideoSmooth", MultiVideoSmooth},{"MultiVideoXor", MultiVideoXor},{"MultiVideoFilter_SubFilter", MultiVideoFilter_SubFilter},{"FadeOnOff", FadeOnOff},{"Stereo", Stereo},{"MultiVideoSaturateBlend", MultiVideoSaturateBlend},{"ImageWithKeys", ImageWithKeys},{"MultiVideoColorKeyOn", MultiVideoColorKeyOn}, {"MultiVideoColorKeyOff", MultiVideoColorKeyOff},{"MultiVideoMedianBlend", MultiVideoMedianBlend},
        {"MultiVideoMirror", MultiVideoMirror},{"MultiVideoSubtract", MultiVideoSubtract},{"MultiVideoAlphaMedianBlend", MultiVideoAlphaMedianBlend},{"MultiVideoInterlace", MultiVideoInterlace},{"MultiVideoChangeLines", MultiVideoChangeLines},{"MultiVideoDiagSquare", MultiVideoDiagSquare},{"ShiftLinesDown", ShiftLinesDown},{"PictureStretch", PictureStretch},{"PictureStretchPieces", PictureStretchPieces},{"VisualSnow", VisualSnow},
        {"VisualSnowX2", VisualSnowX2},{"LineGlitch", LineGlitch},{"SlitReverse64", SlitReverse64},{"SlitReverse64_Increase", SlitReverse64_Increase},{"SlitStretch",SlitStretch},{"LineLeftRight", LineLeftRight},{"LineLeftRightResize", LineLeftRightResize},{"SoloInOrder", SoloInOrder},{"ImageInOrder", ImageInOrder},{"SubInOrder", SubInOrder},{"RGBLineTrails", RGBLineTrails},{"RGBCollectionBlend",RGBCollectionBlend},{"RGBCollectionIncrease", RGBCollectionIncrease},{"RGBCollectionEx", RGBCollectionEx},{"RGBLongTrails", RGBLongTrails},{"VideoMatrixOutlineSubFilter", VideoMatrixOutlineSubFilter},{"FadeRGB_Speed", FadeRGB_Speed},{"RGBStrobeTrails", RGBStrobeTrails},{"FadeRGB_Variable", FadeRGB_Variable},{"BoxGlitch", BoxGlitch},{"VerticalPictureDistort", VerticalPictureDistort},{"ShortTrail", ShortTrail},{"DiagInward", DiagInward},{"DiagSquareInward", DiagSquareInward},{"DiagSquareInwardResize",DiagSquareInwardResize},{"DiagSquareInwardResizeXY", DiagSquareInwardResizeXY},{"ParticleSlide", ParticleSlide},{"DiagPixelated", DiagPixelated},{"DiagPixelatedResize", DiagPixelatedResize},{"DiagPixelRGB_Collection", DiagPixelRGB_Collection},{"RGBShiftTrails", RGBShiftTrails},{"PictureShiftDown", PictureShiftDown},{"PictureShiftRight", PictureShiftRight},{"PictureShiftDownRight", PictureShiftDownRight},{"FlipPictureShift", FlipPictureShift},{"FlipPictureRandomMirror", FlipPictureRandomMirror},{"PictureShiftVariable", PictureShiftVariable},{"RGBWideTrails", RGBWideTrails},{"StretchR_Right", StretchR_Right},{"StretchG_Right", StretchG_Right}, {"StretchB_Right", StretchB_Right},
        {"StretchR_Down", StretchR_Down}, {"StretchG_Down", StretchG_Down}, {"StretchB_Down", StretchB_Down},{"Distorted_LinesY", Distorted_LinesY},{"Distorted_LinesX", Distorted_LinesX},{"TripHSV", TripHSV},{"Diag_Line_InOut", Diag_Line_InOut},{"Histogram", Histogram},{"ImageHistogramLookup", ImageHistogramLookup},{"MovementRange_SubFilter", MovementRange_SubFilter},{"XorSumStrobe", XorSumStrobe},{"DetectEdges", DetectEdges},{"SobelNorm", SobelNorm},{"SobelThreshold", SobelThreshold},{"EdgeFill_SubFilter", EdgeFill_SubFilter}, {"FillWhite_SubFilter", FillWhite_SubFilter}, {"FillBlack_SubFilter", FillBlack_SubFilter},{"FillPink_SubFilter", FillPink_SubFilter},{"FillGreen_SubFilter", FillGreen_SubFilter},{"MedianBlurHigherLevel", MedianBlurHigherLevel},{"FillNonWhite_SubFilter", FillNonWhite_SubFilter}, {"FillNonBlack_SubFilter", FillNonBlack_SubFilter},{"FillNonPink_SubFilter", FillNonPink_SubFilter},{"LineInLineOut", LineInLineOut},{"LineInLineOut_Increase", LineInLineOut_Increase},{"LineInLineOut2_Increase", LineInLineOut2_Increase},{"LineInLineOut3_Increase", LineInLineOut3_Increase},{"LineInLineOut4_Increase", LineInLineOut4_Increase},{"LineInLineOut_ReverseIncrease", LineInLineOut_ReverseIncrease},{"LineInLineOut_InvertedY", LineInLineOut_InvertedY},{"LineInLineOut_ReverseInvertedY", LineInLineOut_ReverseInvertedY},{"LineInLineOut_Vertical", LineInLineOut_Vertical},{"LineInLineOut_VerticalIncrease", LineInLineOut_VerticalIncrease},{"LineInLineOut_IncreaseImage", LineInLineOut_IncreaseImage},{"LineInLineOut_IncreaseVideo", LineInLineOut_IncreaseVideo},{"LineInLineOut_IncreaseVideo2", LineInLineOut_IncreaseVideo2},{"LineInLineOut_IncreaseVideo3", LineInLineOut_IncreaseVideo3},{"LineInLineOut_IncreaseImage2", LineInLineOut_IncreaseImage2},{"LineInLineOut_IncreaseImage3", LineInLineOut_IncreaseImage3},{"SquareByRow", SquareByRow},{"SquareByRowRev", SquareByRowRev},{"SquareByRow2", SquareByRow2},{"DivideByValue", DivideByValue},
        
        {"No Filter",NoFilter},
        {"Blend with Source",BlendWithSource}, {"Plugin",plugin}, {"Custom",custom}, {"DummyFiller",Empty}, {"DummyFiller",Empty}, {"DummyFiller",Empty}};
    FilterType *filter_array = &filters[0];
    
    FilterType filterByIndex(const int &num) {
        return filter_array[num];
    }
    FilterType filterByString(const std::string &num) {
        return filter_array[ac::filter_map[num]];
    }
    std::unordered_map<std::string, FilterType> filter_map_str;
    Pixelated pix;
    unsigned long all_allocation = 0;
}
std::unordered_map<std::string, ac::UserFilter> ac::user_filter;
cv::Mat blend_image, color_image, color_bg_image,color_replace_image,color_filter;
bool blend_set = false;
bool colorkey_set = false;
bool colorkey_bg = false;
bool colorkey_replace = false;
bool colorkey_filter = false;
bool cl_init = false;

bool ac::OpenCL_Enabled() {
    return cl_init;
}


// return version info
std::string ac::getVersion() {
    return version;
}
// be sure to call this on startup
void ac::fill_filter_map() {
    cl_init = cv::ocl::useOpenCL();
    for(int i = 0; i < ac::draw_max; ++i)
        filter_map[draw_strings[i]] = i;
    for(int i = 0; i < ac::draw_max-3; ++i)
        filter_map_str[filters[i].first] = filters[i];
    ac::init_filter_menu_map();
}

void ac::DrawFilter(const std::string &name, const cv::Mat &frame, cv::Mat &outframe) {
    outframe = frame.clone();
    CallFilter(name, outframe);
}

void ac::DrawFilter(int index, const cv::Mat &frame, cv::Mat &outframe) {
    outframe = frame.clone();
    CallFilter(index, outframe);
}
void ac::DrawFilter(int index, cv::Mat &frame) {
    CallFilter(index, frame);
}
void ac::DrawFilter(const std::string &name, cv::Mat &frame) {
    CallFilter(name, frame);
}

bool ac::CallFilter(int index, cv::Mat &frame) {
    if(index >= 0 && index < ac::draw_strings.size()) {
        if(ac::release_frames) {
            ac::release_all_objects();
            ac::release_frames = false;
        }
        DrawFilterUnordered(ac::draw_strings[index], frame);
        return true;
    }
    return false;
}

unsigned long ac::calculateMemory() {
    
    if(all_objects.empty())
        return 0;
    
    unsigned long total = 0;
    for(int i = 0; i < static_cast<int>(all_objects.size()); ++i) {
        if(all_objects[i] != 0) {
            total += all_objects[i]->getSize();
        }
    }
    return total;
}

bool ac::CallFilter(const std::string &name, cv::Mat &frame) {
    int index = ac::filter_map[name];
    if(index >= 0 && index < ac::draw_strings.size()) {
        if(ac::release_frames) {
            ac::release_all_objects();
            ac::release_frames = false;
        }
        DrawFilterUnordered(name, frame);
        return true;
    }
    if(index == subfilter)
        std::cout << "Filter same as subfilter...\n";
    else
        std::cout << "filter: " << name << " not found.\n";
    return false;
}

void ac::DrawFilterUnordered(const std::string &name, cv::Mat &frame) {
    if(user_filter.find(name) != user_filter.end()) {
        std::string fname = name;
        if(!ac::CallFilterFile(frame, fname)) {
            std::cerr << "CallFilterFile failed...\n";
        }
    }
    else
        if(filter_map_str.find(name) != filter_map_str.end()) {
            filter_map_str[name].second(frame);
        }
        else
            std::cerr << "filter: " << name << " not found!\n";
}




void ac::clearFilterFiles() {
    for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
        int offset = i->second.index;
        if(!draw_strings.empty()) {
            auto pos = std::find(draw_strings.begin(), draw_strings.end(), i->first);
            if(pos != draw_strings.end()) {
                std::cout << "cleraed offset: " << offset << "\n";
                draw_strings.erase(pos);
                user_filter.erase(i);
            }
        }
    }
}

bool ac::getSupportedResolutions(cv::VideoCapture &capture, std::vector<cv::Size> &res) {
    static cv::Size size[] = {{320, 240}, {640,480}, {800,600}, {960,720}, {1024,768}, {1280,960}, {1400,1050}, {1440,1080}, {1600,1200}, {1856,1392}, {1920,1440}, {2048,1536}, {1280,800}, {1440,900}, {1680,1050}, {1920,1200}, {2560,1600}, {1024,576}, {1152,648}, {1280,720}, {1366,768}, {1600,900}, {1920,1080}, {2560,1440}, {3840,2160}, {7680,4320}, {0, 0} };
    for(int i = 0; size[i].width != 0; ++i) {
        capture.set(cv::CAP_PROP_FRAME_WIDTH, size[i].width);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, size[i].height);
        if(capture.get(cv::CAP_PROP_FRAME_WIDTH) == size[i].width && capture.get(cv::CAP_PROP_FRAME_HEIGHT) == size[i].height) {
            res.push_back(size[i]);
        }
    }
    if(res.size()>0)
        return true;
    return false;
}

bool ac::CallFilterFile(cv::Mat &frame, std::string filtername) {
    auto pos = user_filter.find(filtername);
    if(pos != user_filter.end()) {
        if(pos->second.func != 0) {
            pos->second.func(frame);
            return true;
        }
        for(int i = 0; i < pos->second.custom_filter.name.size(); ++i) {
            FileT &type = pos->second.custom_filter;
            std::string f = type.name[i];
            std::string s = type.subname[i];
            auto pos1 = ac::filter_map.find(f);
            auto pos2 = ac::filter_map.find(pos->first);
            if(pos2 != ac::filter_map.end()) {
                if(*pos1 == *pos2)
                    continue;
            }
            if(s.length() > 0) {
                //ac::setSubFilter(ac::filter_map[s]);
                ac::pushSubFilter(ac::filter_map[s]);
                ac::CallFilter(f, frame);
                ac::popSubFilter();
            }
            else {
                CallFilter(f, frame);
            }
        }
    }
    return true;
}

std::vector<std::string> ac::buildDrawStrings() {
    std::vector<std::string> v;
    v.push_back("Self AlphaBlend");
    v.push_back("Self Scale");
    v.push_back("StrobeEffect");
    v.push_back("Blend #3");
    v.push_back("Negative Paradox");
    v.push_back("ThoughtMode");
    v.push_back("RandTriBlend");
    v.push_back("Blank");
    v.push_back("Tri");
    v.push_back("Distort");
    v.push_back("CDraw");
    v.push_back("Type");
    v.push_back("NewOne");
    v.push_back("Blend Fractal");
    v.push_back("Blend Fractal Mood");
    v.push_back("CosSinMultiply");
    v.push_back("Color Accumlate1");
    v.push_back("Color Accumulate2");
    v.push_back("Color Accumulate3");
    v.push_back("Filter8");
    v.push_back("Filter3");
    v.push_back("Rainbow Blend");
    v.push_back("Rand Blend");
    v.push_back("New Blend");
    v.push_back("Alpha Flame Filters");
    v.push_back("Pixel Scale");
    v.push_back("PixelSort");
    v.push_back("GlitchSort");
    v.push_back("Random Filter");
    v.push_back("Random Flash");
    v.push_back("Blend with Image");
    v.push_back("Blend with Image #2");
    v.push_back("Blend with Image #3");
    v.push_back("Blend with Image #4");
    v.push_back("GaussianBlur");
    v.push_back("Median Blur");
    v.push_back("Blur Distortion");
    v.push_back("Diamond Pattern");
    v.push_back("MirrorBlend");
    v.push_back("Pulse");
    v.push_back("Sideways Mirror");
    v.push_back("Mirror No Blend");
    v.push_back("Sort Fuzz");
    v.push_back("Fuzz");
    v.push_back("Double Vision");
    v.push_back("RGB Shift");
    v.push_back("RGB Sep");
    v.push_back("Graident Rainbow");
    v.push_back("Gradient Rainbow Flash");
    v.push_back("Reverse");
    v.push_back("Scanlines");
    v.push_back("TV Static");
    v.push_back("Mirror Average");
    v.push_back("Mirror Average Mix");
    v.push_back("Mean");
    v.push_back("Laplacian");
    v.push_back("Bitwise_XOR");
    v.push_back("Bitwise_AND");
    v.push_back("Bitwise_OR");
    v.push_back("Equalize");
    v.push_back("Channel Sort");
    v.push_back("Reverse_XOR");
    v.push_back("Combine Pixels");
    v.push_back("FlipTrip");
    v.push_back("Canny");
    v.push_back("Boxes");
    v.push_back("Boxes Fade");
    v.push_back("Flash Black");
    v.push_back("SlideRGB");
    v.push_back("Side2Side");
    v.push_back("Top2Bottom");
    v.push_back("Strobe Red Then Green Then Blue");
    v.push_back("Blend_Angle");
    v.push_back("Outward");
    v.push_back("Outward Square");
    v.push_back("ShiftPixels");
    v.push_back("ShiftPixelsDown");
    v.push_back("XorMultiBlend");
    v.push_back("Bitwise_Rotate");
    v.push_back("Bitwise_Rotate Diff");
    v.push_back("HPPD");
    v.push_back("FuzzyLines");
    v.push_back("GradientLines");
    v.push_back("GradientSelf");
    v.push_back("GradientSelfVertical");
    v.push_back("GradientDown");
    v.push_back("GraidentHorizontal");
    v.push_back("GradientRGB");
    v.push_back("Inter");
    v.push_back("UpDown");
    v.push_back("LeftRight");
    v.push_back("StrobeScan");
    v.push_back("BlendedScanLines");
    v.push_back("GradientStripes");
    v.push_back("XorSine");
    v.push_back("SquareSwap");
    v.push_back("SquareSwap4x2");
    v.push_back("SquareSwap8x4");
    v.push_back("SquareSwap16x8");
    v.push_back("SquareSwap64x32");
    v.push_back("SquareBars");
    v.push_back("SquareBars8");
    v.push_back("SquareSwapRand16x8");
    v.push_back("SquareVertical8");
    v.push_back("SquareVertical16");
    v.push_back("SquareVertical_Roll");
    v.push_back("SquareSwapSort_Roll");
    v.push_back("SquareVertical_RollReverse");
    v.push_back("SquareSwapSort_RollReverse");
    v.push_back("Circular");
    v.push_back("WhitePixel");
    v.push_back("FrameBlend");
    v.push_back("FrameBlendRGB");
    v.push_back("TrailsFilter");
    v.push_back("TrailsFilterIntense");
    v.push_back("TrailsFilterSelfAlpha");
    v.push_back("TrailsFilterXor");
    v.push_back("ColorTrails");
    v.push_back("MoveRed");
    v.push_back("MoveRGB");
    v.push_back("MoveRedGreenBlue");
    v.push_back("BlurSim");
    v.push_back("Block");
    v.push_back("BlockXor");
    v.push_back("BlockScale");
    v.push_back("BlockStrobe");
    v.push_back("PrevFrameBlend");
    v.push_back("Wave");
    v.push_back("HighWave");
    v.push_back("VerticalSort");
    v.push_back("VerticalChannelSort");
    v.push_back("HorizontalBlend");
    v.push_back("VerticalBlend");
    v.push_back("OppositeBlend");
    v.push_back("DiagonalLines");
    v.push_back("HorizontalLines");
    v.push_back("InvertedScanlines");
    v.push_back("Soft_Mirror");
    v.push_back("KanapaTrip");
    v.push_back("ColorMorphing");
    v.push_back("ScanSwitch");
    v.push_back("ScanAlphaSwitch");
    v.push_back("NegativeStrobe");
    v.push_back("XorAddMul");
    v.push_back("ParticleRelease");
    v.push_back("BlendSwitch");
    v.push_back("All Red");
    v.push_back("All Green");
    v.push_back("All Blue");
    v.push_back("LineRGB");
    v.push_back("PixelRGB");
    v.push_back("BoxedRGB");
    v.push_back("KruegerSweater");
    v.push_back("RGBFlash");
    v.push_back("IncreaseBlendHorizontal");
    v.push_back("BlendIncrease");
    v.push_back("GradientReverse");
    v.push_back("GradientReverseVertical");
    v.push_back("GradientReverseBox");
    v.push_back("GradientNewFilter");
    v.push_back("ReinterpretDouble");
    v.push_back("ReinterpSelfScale");
    v.push_back("AverageLines");
    v.push_back("ImageFile");
    v.push_back("ImageXor");
    v.push_back("ImageAlphaBlend");
    v.push_back("ColorRange");
    v.push_back("ImageInter");
    v.push_back("TrailsInter");
    v.push_back("TrailsBlend");
    v.push_back("TrailsNegate");
    v.push_back("InterReverse");
    v.push_back("InterMirror");
    v.push_back("InterFullMirror");
    v.push_back("MirrorRGB");
    v.push_back("RGBStatic1");
    v.push_back("RGBStatic2");
    v.push_back("VectorIncrease");
    v.push_back("LineByLineReverse");
    v.push_back("RandomIntertwine");
    v.push_back("RandomFour");
    v.push_back("BlendThree");
    v.push_back("AcidTrails");
    v.push_back("RandomTwo");
    v.push_back("HorizontalTrailsInter");
    v.push_back("Trails");
    v.push_back("BlendTrails");
    v.push_back("Negate");
    v.push_back("RandomFilteredSquare");
    v.push_back("ImageX");
    v.push_back("RandomQuads");
    v.push_back("QuadCosSinMultiply");
    v.push_back("QuadRandomFilter");
    v.push_back("RollRandom");
    v.push_back("AverageRandom");
    v.push_back("HorizontalStripes");
    v.push_back("DiamondStrobe");
    v.push_back("SmoothTrails");
    v.push_back("GridFilter8x");
    v.push_back("GridFilter16x");
    v.push_back("GridFilter8xBlend");
    v.push_back("GridRandom");
    v.push_back("GridRandomPixel");
    v.push_back("Dual_SelfAlphaRainbow");
    v.push_back("Dual_SelfAlphaBlur");
    v.push_back("SurroundPixelXor");
    v.push_back("Darken");
    v.push_back("WeakBlend");
    v.push_back("AverageVertical");
    v.push_back("RandomCollectionAverage");
    v.push_back("RandomCollectionAverageMax");
    v.push_back("SmoothTrailsSelfAlphaBlend");
    v.push_back("SmoothTrailsRainbowBlend");
    v.push_back("MedianBlend");
    v.push_back("SmoothRandomImageBlend");
    v.push_back("SmoothImageAlphaBlend");
    v.push_back("RandomAlphaBlend");
    v.push_back("RandomTwoFilterAlphaBlend");
    v.push_back("PixelatedSquare");
    v.push_back("AlphaBlendPosition");
    v.push_back("BlendRowAlpha");
    v.push_back("BlendRow");
    v.push_back("BlendRowByVar");
    v.push_back("BlendRowByDirection");
    v.push_back("BlendAlphaXor");
    v.push_back("SelfXorScale");
    v.push_back("BitwiseXorScale");
    v.push_back("XorTrails");
    v.push_back("RainbowTrails");
    v.push_back("NegativeTrails");
    v.push_back("IntenseTrails");
    v.push_back("SelfAlphaRGB");
    v.push_back("BlendImageOnOff");
    v.push_back("XorSelfAlphaImage");
    v.push_back("BitwiseXorStrobe");
    v.push_back("AlphaBlendRandom");
    v.push_back("ChannelSortAlphaBlend");
    v.push_back("XorChannelSort");
    v.push_back("GradientColors");
    v.push_back("GradientColorsVertical");
    v.push_back("Bitwise_XOR_Average");
    v.push_back("ImageShiftMatrixLeft");
    v.push_back("GradientXorSelfScale");
    v.push_back("SmoothSourcePixel");
    v.push_back("StrobeBlend");
    v.push_back("FrameBars");
    v.push_back("Sort_Vertical_Horizontal");
    v.push_back("Sort_Vertical_Horizontal_Bitwise_XOR");
    v.push_back("Scalar_Average_Multiply");
    v.push_back("Scalar_Average");
    v.push_back("Total_Average");
    v.push_back("AlphaBlendImageXor");
    v.push_back("FlashWhite");
    v.push_back("FlashBlackAndWhite");
    v.push_back("GaussianBlend");
    v.push_back("RandomXor");
    v.push_back("RandomXorFlash");
    v.push_back("RandomAmountMedianBlur");
    v.push_back("SoftXor");
    v.push_back("SelfXorBlend");
    v.push_back("SelfXorDoubleFlash");
    v.push_back("SelfOrDoubleFlash");
    v.push_back("BlendRowCurvedSqrt");
    v.push_back("CycleShiftRGB");
    v.push_back("CycleShiftRandomRGB");
    v.push_back("CycleShiftRandomRGB_XorBlend");
    v.push_back("CycleShiftRandomAlphaBlend");
    v.push_back("VerticalColorBars");
    v.push_back("GradientLeftRight");
    v.push_back("GraidentUpDown");
    v.push_back("GradientLeftRightInOut");
    v.push_back("GradientUpDownInOut");
    v.push_back("Lines");
    v.push_back("ColorLines");
    v.push_back("WhiteLines");
    v.push_back("ThickWhiteLines");
    v.push_back("UseLineObject");
    v.push_back("TanAlphaGrid");
    v.push_back("MedianBlendAnimation");
    v.push_back("FibFlash");
    v.push_back("ScaleFlash");
    v.push_back("Curtain");
    v.push_back("RandomCurtain");
    v.push_back("CurtainVertical");
    v.push_back("RandomCurtainVertical");
    v.push_back("inOrder");
    v.push_back("inOrderBySecond");
    v.push_back("DarkenFilter");
    v.push_back("RandomFilterBySecond");
    v.push_back("ThreeRandom");
    v.push_back("inOrderAlpha");
    v.push_back("inOrderAlphaXor");
    v.push_back("SlideFilter");
    v.push_back("SlideFilterXor");
    v.push_back("RandomSlideFilter");
    v.push_back("SlideUpDown");
    v.push_back("SlideUpDownXor");
    v.push_back("SlideUpDownRandom");
    v.push_back("SlideSubFilter");
    v.push_back("SlideSubUpDownFilter");
    v.push_back("ParticleBlend");
    v.push_back("ParticleFlash");
    v.push_back("ExactImage");
    v.push_back("ParticleAlpha");
    v.push_back("BlendInAndOut");
    v.push_back("BlendScaleInAndOut");
    v.push_back("AcidGlitch");
    v.push_back("XorBackwards");
    v.push_back("LiquidFilter");
    v.push_back("MatrixXorAnd");
    v.push_back("XorAlpha");
    v.push_back("AlphaAcidTrails");
    v.push_back("SelfXorAverage");
    v.push_back("RandomXorBlend");
    v.push_back("RGBVerticalXor");
    v.push_back("RGBVerticalXorScale");
    v.push_back("RGBHorizontalXor");
    v.push_back("RGBHorizontalXorScale");
    v.push_back("FadeStrobe");
    v.push_back("RGBMirror");
    v.push_back("MirrorStrobe");
    v.push_back("AndStrobe");
    v.push_back("AndStrobeScale");
    v.push_back("AndPixelStrobe");
    v.push_back("AndOrXorStrobe");
    v.push_back("AndOrXorStrobeScale");
    v.push_back("FadeInAndOut");
    v.push_back("BrightStrobe");
    v.push_back("DarkStrobe");
    v.push_back("ParticleFast");
    v.push_back("RandomXorOpposite");
    v.push_back("StrobeTransform");
    v.push_back("InitBlend");
    v.push_back("MoveUpLeft");
    v.push_back("RandomStrobe");
    v.push_back("RandomBlur");
    v.push_back("Stuck");
    v.push_back("StuckStrobe");
    v.push_back("OrStrobe");
    v.push_back("LagBlend");
    v.push_back("SubFilter");
    v.push_back("BlendImageXor");
    v.push_back("BlendImageAround_Median");
    v.push_back("ImageBlendTransform");
    v.push_back("RGBTrails");
    v.push_back("RGBTrailsDark");
    v.push_back("RGBTrailsAlpha");
    v.push_back("RGBTrailsNegativeAlpha");
    v.push_back("MovementRGBTrails");
    v.push_back("RGBTrailsXor");
    v.push_back("DifferenceStrobe");
    v.push_back("BlackAndWhiteDifferenceStrobe");
    v.push_back("DifferenceXor");
    v.push_back("DifferenceRand");
    v.push_back("DifferenceBrightStrobe");
    v.push_back("PsycheTrails");
    v.push_back("FourSquare");
    v.push_back("EightSquare");
    v.push_back("DiagonalSquare");
    v.push_back("DiagonalSquareRandom");
    v.push_back("SquareStretchDown");
    v.push_back("SquareStretchRight");
    v.push_back("SquareStretchUp");
    v.push_back("SquareStretchLeft");
    v.push_back("DarkTrails");
    v.push_back("SoftFeedback");
    v.push_back("SoftFeedbackFrames");
    v.push_back("ResizeSoftFeedback");
    v.push_back("SoftFeedback8");
    v.push_back("SoftFeedbackFrames8");
    v.push_back("ResizeSoftFeedback8");
    v.push_back("ResizeSoftFeedbackSubFilter");
    v.push_back("SoftFeedbackRandFilter");
    v.push_back("SoftFeedback32");
    v.push_back("SoftFeedbackFrames32");
    v.push_back("ResizeSoftFeedback32");
    v.push_back("SoftFeedbackRandFilter32");
    v.push_back("SoftFeedbackSubFilter");
    v.push_back("SoftFeedbackResizeSubFilter");
    v.push_back("SoftFeedbackResize64");
    v.push_back("SoftFeedbackResizeSubFilter64");
    v.push_back("SoftFeedbackReszieSubFilter64_Negate");
    v.push_back("SoftFeedbackReszieSubFilter64_Mirror");
    v.push_back("HalfNegateStrobe");
    v.push_back("MedianBlurXor");
    v.push_back("NegateTrails");
    v.push_back("RandomGradient");
    v.push_back("RandomStrobeFlash");
    v.push_back("RandomMirror");
    v.push_back("RandomOther");
    v.push_back("RandomXorFilter");
    v.push_back("RandomMirrorBlend");
    v.push_back("RandomMirrorAlphaBlend");
    v.push_back("Bitwise_XOR_AlphaSubFilter");
    v.push_back("AlphaBlendSubFilter");
    v.push_back("GradientSubFilterXor");
    v.push_back("XorBlend_SubFilter");
    v.push_back("SmoothSubFilterAlphaBlend");
    v.push_back("SmoothSubFilterXorBlend");
    v.push_back("IntertwineSubFilter");
    v.push_back("RandBlend");
    v.push_back("EveryOther");
    v.push_back("EveryOtherSubFilter");
    v.push_back("SmoothRandomFilter");
    v.push_back("RandomFilterRandomTimes");
    v.push_back("RandomSubFilterRandomTimes");
    v.push_back("AddToFrameSubFilter");
    v.push_back("MirrorXor");
    v.push_back("MirrorXorAll");
    v.push_back("MirrorXorScale");
    v.push_back("EnergyMirror");
    v.push_back("SmoothSubFilter");
    v.push_back("EnergizeSubFilter");
    v.push_back("SmoothSubFilter16");
    v.push_back("EnergizeSubFilter16");
    v.push_back("EnergizeSubFilter32");
    v.push_back("SmoothSubFilter32");
    v.push_back("HalfAddSubFilter");
    v.push_back("HalfXorSubFilter");
    v.push_back("StaticXorBlend");
    v.push_back("PsycheSort");
    v.push_back("XorScale");
    v.push_back("ChannelMedianSubFilter");
    v.push_back("GaussianStrobe");
    v.push_back("StrobeSort");
    v.push_back("GlitchSortStrobe");
    v.push_back("Bitwise_XOR_Blend");
    v.push_back("Bitwise_XOR_Sort");
    v.push_back("Bitwise_OR_Blend");
    v.push_back("Bitwise_AND_Blend");
    v.push_back("BitwiseColorMatrix");
    v.push_back("PixelReverseXor");
    v.push_back("PixelatedSubFilterSort");
    v.push_back("SilverBlend");
    v.push_back("RandomPixelOrderSort");
    v.push_back("ImageXorAlpha");
    v.push_back("ImageAverageXor");
    v.push_back("PixelXorBlend");
    v.push_back("SelfAlphaScale");
    v.push_back("SelfScaleAlpha");
    v.push_back("RainbowXorBlend");
    v.push_back("FrameDifference");
    v.push_back("SmallDiffference");
    v.push_back("FadeBlend");
    v.push_back("FilteredDifferenceSubFilter");
    v.push_back("ExpandSquareSubFilter");
    v.push_back("ExpandSquareBlendSubFilter");
    v.push_back("ExpandSquareVerticalSubFilter");
    v.push_back("DarkImageMedianBlend");
    v.push_back("GammaDarken5");
    v.push_back("GammaDarken10");
    v.push_back("SelfAlphaScaleBlend");
    v.push_back("FadeBars");
    v.push_back("MirrorXorAlpha");
    v.push_back("MirrorEnergizeSubFilter");
    v.push_back("StrobeXor");
    v.push_back("IntertwinedMirror");
    v.push_back("BlurredMirror");
    v.push_back("ShadeRGB");
    v.push_back("InterRGB_SubFilter");
    v.push_back("InterSmoothSubFilter");
    v.push_back("InterRGB_Bars_XY");
    v.push_back("InterRGB_Bars_X");
    v.push_back("InterRGB_Bars_Y");
    v.push_back("StoredFramesAlphaBlend_SubFilter");
    v.push_back("BlendSubFilter");
    v.push_back("BlendAlphaSubFilter");
    v.push_back("ReverseFrameBlend");
    v.push_back("ReverseFrameBlendSwitch");
    v.push_back("DoubleRandomMirror");
    v.push_back("Blend_AlphaSubFilter");
    v.push_back("RandomBlendFilter");
    v.push_back("DoubleRandomBlendFilter");
    v.push_back("FlipBlendW");
    v.push_back("FlipBlendWH");
    v.push_back("FlipBlendH");
    v.push_back("FlipBlendAll");
    v.push_back("FrameMedianBlendSubFilter");
    v.push_back("FrameBlurSubFilter");
    v.push_back("ImageBlendSubFilter");
    v.push_back("ImageBlendXorSubFilter");
    v.push_back("ImageCollectionSubFilter");
    v.push_back("SelfScaleXorIncrease");
    v.push_back("Blend_RedGreenBlue");
    v.push_back("XorBlend_RedGreenBlue");
    v.push_back("BlendIncrease_RedGreenBlue");
    v.push_back("Blend_RedReenBlue_Dark");
    v.push_back("DarkModBlend");
    v.push_back("PictureBuzz");
    v.push_back("IncDifference");
    v.push_back("IncDifferenceAlpha");
    v.push_back("MirrorMedianBlend");
    v.push_back("SubFilterMedianBlend");
    v.push_back("DarkenBlend");
    v.push_back("DarkCollectionSubFilter");
    v.push_back("ChannelSort_NoBlend_Descending");
    v.push_back("ChannelSort_NoBlend_Ascending");
    v.push_back("Headrush");
    v.push_back("DarkSmooth_Filter");
    v.push_back("DarkSelfAlpha");
    v.push_back("FlipMedian");
    v.push_back("FlipMedianSubFilter");
    v.push_back("FlipMirror");
    v.push_back("FlipMirrorAverage");
    v.push_back("FlipMirrorSubFilter");
    v.push_back("ShuffleMedian");
    v.push_back("ShuffleRGB");
    v.push_back("ParticleSnow");
    v.push_back("RandomPixels");
    v.push_back("DarkRandomPixels");
    v.push_back("MedianBlurSubFilter");
    v.push_back("Bars");
    v.push_back("ShuffleAlpha");
    v.push_back("AlphaMorph");
    v.push_back("ShuffleSelf");
    v.push_back("PixelatedHorizontalLines");
    v.push_back("PixelatedVerticalLines");
    v.push_back("StrobeShuffle");
    v.push_back("BlendBurred");
    v.push_back("BlendCombinedValues");
    v.push_back("RGBColorTrails");
    v.push_back("BlendCombinedValueSubFilter");
    v.push_back("BlendSubFilterAlpha");
    v.push_back("GradientXorPixels");
    v.push_back("PurpleRain");
    v.push_back("PixelByPixelXor");
    v.push_back("CopyXorAlpha");
    v.push_back("AveragePixelsXor");
    v.push_back("AveragePixelAlpha");
    v.push_back("NegativeByRow");
    v.push_back("AveragePixelCollection");
    v.push_back("IncorrectLine");
    v.push_back("XorShift");
    v.push_back("StrobeXorAndOr");
    v.push_back("XorWithSource");
    v.push_back("AlphaBlendWithSource");
    v.push_back("RGBSep1x");
    v.push_back("RGBMedianBlend");
    v.push_back("RGBMirror1");
    v.push_back("RGBMirror1Median");
    v.push_back("FlashMirror");
    v.push_back("CollectionXorSourceSubFilter");
    v.push_back("ReverseMirrorX");
    v.push_back("MirrorXorAll_Reverse");
    v.push_back("MirrorRGBReverse");
    v.push_back("MirrorRGBReverseBlend");
    v.push_back("BlendReverseSubFilter");
    v.push_back("MirrorBitwiseXor");
    v.push_back("SmoothBlendReverseSubFilter");
    v.push_back("RandomIncrease");
    v.push_back("MedianBlend16");
    v.push_back("MedianBlendBufferSubFilter");
    v.push_back("BGRBlend");
    v.push_back("RGBBlend");
    v.push_back("RGBBlendSubFilter");
    v.push_back("DivideAndIncH");
    v.push_back("DivideAndIncW");
    v.push_back("XorOppositeSubFilter");
    v.push_back("BlendSmoothSubFilter");
    v.push_back("BlurSmooth");
    v.push_back("BlurSmoothMedian");
    v.push_back("BlurSmoothSubFilter");
    v.push_back("BlurFlip");
    v.push_back("BlurFlipSubFilter");
    v.push_back("BlurMirrorGamma");
    v.push_back("MedianBlendDark");
    v.push_back("MedianBlendSubFilterEx");
    v.push_back("EnergyMirrorDark");
    v.push_back("AlphaBlendMirror");
    v.push_back("MirrorAlphaBlendedImage");
    v.push_back("AlphaBlendXorImage");
    v.push_back("ShiftFrameSmoothSubFilter");
    v.push_back("ShiftFrameStaticXorSubFilter");
    v.push_back("IncreaseDecreaseGamma");
    v.push_back("GammaIncDecIncrease");
    v.push_back("RandomSubFilter2");
    v.push_back("TwistedVision");
    v.push_back("TwistedMirror");
    v.push_back("SelfScaleSortBlend");
    v.push_back("FlashMedianBlend");
    v.push_back("BlendWithFrameSubFilter");
    v.push_back("AlphaBlendWithFrameSubFilter");
    v.push_back("AlphaXorBlendWithFrameSubFilter");
    v.push_back("XorBlendSubFilter");
    v.push_back("FlipAlphaBlend");
    v.push_back("RandomFlipFilter");
    v.push_back("MirrorMedian");
    v.push_back("FlipMatrixCollection");
    v.push_back("MirrorMatrixCollection");
    v.push_back("MirrorMatrixSource");
    v.push_back("SelfScaleByFrame");
    v.push_back("SmoothMedianRotateSubFilter");
    v.push_back("SmoothCollectionAlphaBlend");
    v.push_back("XorSubFilter");
    v.push_back("XorAlphaSubFilter");
    v.push_back("BlurXorAlphaSubFilter");
    v.push_back("ImageXorFrame");
    v.push_back("ImageXorFunction");
    v.push_back("ImageXorAlphaBlend");
    v.push_back("ImageAlphaXorMedianSubFilter");
    v.push_back("ImageSmoothAlphaXorSubFilter");
    v.push_back("ImageXorMirrorFilter");
    v.push_back("ImageXorSubFilter");
    v.push_back("ImageAlphaXorSubFilter");
    v.push_back("SmoothTrailsBlend");
    v.push_back("MatrixCollectionRGBXor");
    v.push_back("RainbowGlitch");
    v.push_back("RainbowGlichStrobe");
    v.push_back("NegateSwitchStrobe");
    v.push_back("StrobeAlphaShuffle");
    v.push_back("ShuffleAlphaWithRGB");
    v.push_back("ShuffleAlphaSubFilter");
    v.push_back("ShuffleColorMap");
    v.push_back("BlendWithRainbowSubFilter");
    v.push_back("BlendWithJetSubFilter");
    v.push_back("ColormapBlendSubFilter");
    v.push_back("RandomColorMap");
    v.push_back("SmoothMirrorBlurFlip");
    v.push_back("RandomColorMapAlphaBlendSubFilter");
    v.push_back("RandomOrder");
    v.push_back("RandomOrderMedianBlendSubFilter");
    v.push_back("MirrorOrder");
    v.push_back("MirrorOrderSubFilter");
    v.push_back("BlurMirrorOrder");
    v.push_back("AveragePixelMirror");
    v.push_back("ShuffleAlphaMedianBlend");
    v.push_back("MirrorOrderAlpha");
    v.push_back("FilterStrobeSubFilter");
    v.push_back("ImageSubtractMedianBlend");
    v.push_back("ImageDarkBlend");
    v.push_back("ImageAverageDark");
    v.push_back("ImageRemainderPixel");
    v.push_back("AverageLinesBlend");
    v.push_back("SoftFeedbackMirror");
    v.push_back("AverageVerticalLinesBlend");
    v.push_back("LinesMedianBlend");
    v.push_back("XorSquare");
    v.push_back("PixelValuesPlusOne");
    v.push_back("AverageHorizontalFilter");
    v.push_back("AverageVerticalFilter");
    v.push_back("GradientAlphaXorHorizontal");
    v.push_back("GradientAlphaXorVertical");
    v.push_back("BlendImageWithSubFilter");
    v.push_back("BlendImageWithSubFilterAlpha");
    v.push_back("MedianBlendSoft");
    v.push_back("AndImageSubFilterXor");
    v.push_back("AlphaBlendImageSubFilterXor");
    v.push_back("AlphaBlendImageSubFilterXorRev");
    v.push_back("ParticleReleaseXor");
    v.push_back("ParticleReleaseXorVec");
    v.push_back("ParticleReleaseAlphaBlend");
    v.push_back("ParticleReleaseWithImage");
    v.push_back("ParticleReleaseSubFilter");
    v.push_back("ParticleReleaseImageSubFilter");
    v.push_back("ImageEnergy");
    v.push_back("ImageEnergySubFilter");
    v.push_back("ImageDistortion");
    v.push_back("ImageDistortionSubFilter");
    v.push_back("SmoothExactImageXorAlpha");
    v.push_back("FeedbackColormap");
    v.push_back("SmoothImageAlphaBlendMedian");
    v.push_back("ImageDarkenSmoothMedian");
    v.push_back("XorReverseImageSmooth");
    v.push_back("ReverseSubFilterBlend");
    v.push_back("ReverseSubFilterXor");
    v.push_back("ImageReverseSubFilter");
    v.push_back("SmoothRainbowMedian");
    v.push_back("MirrorAlphaBlend");
    v.push_back("ImageSmoothMedianBlend");
    v.push_back("ImageSmoothMedianSubFilter");
    v.push_back("ImageAlphaXorMedianBlend");
    v.push_back("MatrixCollectionBlend");
    v.push_back("MatrixCollectionSubFilter");
    v.push_back("MatrixCollectionImageSubFilter");
    v.push_back("MatrixCollectionBlurAlpha");
    v.push_back("MatrixCollectionXor");
    v.push_back("MatrixCollectionXor32");
    v.push_back("MatrixCollectionRandomColorMap");
    v.push_back("MatrixCollectionDarkXor");
    v.push_back("MatrixCollectionRGB");
    v.push_back("TrailsSubFilter");
    v.push_back("TrailsSubFilter32");
    v.push_back("CompareWithSubFilter");
    v.push_back("MedianTrails");
    v.push_back("SmoothMedianBlend");
    v.push_back("ColorTransition");
    v.push_back("ColorTransitionMedian");
    v.push_back("ColorTransitionRandom");
    v.push_back("ColorTransitionRandomMedian");
    v.push_back("ColorTransitionSubFilter");
    v.push_back("ColorTransitionImageSubFilter");
    v.push_back("CurtainSubFilter");
    v.push_back("RandomTrails");
    v.push_back("RandomTrailsSubFilter");
    v.push_back("CosSinMedianBlend");
    v.push_back("TrailsRGB");
    v.push_back("MatrixTrailsXorRandom");
    v.push_back("CosSinMultiplyCollectionXor");
    v.push_back("Filter8_Blend");
    v.push_back("Filter8_SubFilter");
    v.push_back("RandomSmoothAlphaMedian");
    v.push_back("RandomAlphaBlendFilter");
    v.push_back("RandomMirrorBitwiseXor");
    v.push_back("SquareDivideSubFilter");
    v.push_back("SquareSubFilter");
    v.push_back("SquareSubFilter8");
    v.push_back("ColorExpand");
    v.push_back("ColorExpandSubFilter");
    v.push_back("RotateImage");
    v.push_back("RotateBlendImage");
    v.push_back("RotateImageSubFilter");
    v.push_back("RotateAlphaBlendImage");
    v.push_back("FlipShuffle");
    v.push_back("FlipRandom");
    v.push_back("FlipOrder");
    v.push_back("FlipStrobeSubFilter");
    v.push_back("MirrorBlendFrame");
    v.push_back("MirrorBlendVertical");
    v.push_back("MirrorVerticalAndHorizontal");
    v.push_back("BlendFor360");
    v.push_back("MirrorSidesMedian");
    v.push_back("MirrorSidesSubFilter");
    v.push_back("MedianFrameAlphaBlendSubFilter");
    v.push_back("MedianSubFilter");
    v.push_back("ColorXorScale");
    v.push_back("ColorXorScaleSubFilter");
    v.push_back("ImageXorScale");
    v.push_back("MatrixCollectionShiftSubFilter");
    v.push_back("MatrixCollectionImageShiftSubFilter");
    v.push_back("MatrixCollectionXorBlendAlphaBlend");
    v.push_back("MatrixCollectionBlurImageXorAlpha");
    v.push_back("MatrixCollectionBlurImageSubFilter");
    v.push_back("MatrixCollectionBlurImageSubFilter16");
    v.push_back("ImageAlphaBlendSubFilter");
    v.push_back("MultipleMatrixCollectionSubFilter");
    v.push_back("BlurAlphaSubFilter");
    v.push_back("BlurImageSubFilter");
    v.push_back("MedianBlendSubFilter");
    v.push_back("MedianBlendImageSubFilter");
    v.push_back("MedianBlendSelfBlend");
    v.push_back("BlendHalfSubFilter");
    v.push_back("BlurImageAlphaBlend");
    v.push_back("BlurImageAlphaBlendSubFilter");
    v.push_back("BlurImageAlphaBlendScaleSubFilter");
    v.push_back("RandomAmountOfMedianBlur");
    v.push_back("Bitwise_XOR_BlendFrame");
    v.push_back("AlphaBlendWithSubFilter");
    v.push_back("AlphaBlendScaleWithSubFilter");
    v.push_back("GaussianBlendEx");
    v.push_back("SimpleMatrixBlend");
    v.push_back("MatrixBlendSubFilter");
    v.push_back("SmoothMatrixBlendSubFilter");
    v.push_back("BlurSmoothSubFilterAlphaBlend");
    v.push_back("BlurSmoothAlphaXorBlendSubFilter");
    v.push_back("BlurTwiceSubFilter");
    v.push_back("BlurFrameBlendSubFilter");
    v.push_back("BlurFrameSubFilter");
    v.push_back("BlurSmoothMatrix");
    v.push_back("MedianBlurInc");
    v.push_back("GaussianBlurInc");
    v.push_back("BlurSmoothMedianInc");
    v.push_back("BlurSmoothGaussianInc");
    v.push_back("BlurMatrixCollectionXor");
    v.push_back("MatrixCollection8XorSubFilter");
    v.push_back("BlurSmoothRevFilter");
    v.push_back("SurroundingPixels");
    v.push_back("SurroundingPixelsAlpha");
    v.push_back("MatrixCollectionSurroundingPixels");
    v.push_back("MatrixCollectionSurroundingPixelsSubFilter");
    v.push_back("MatrixCollectionSurroundingPixelsImage");
    v.push_back("MatrixCollectionSurroundingPixelsImageSubFilter");
    v.push_back("ImageTransparent");
    v.push_back("MatrixImageAlphaBlendSubFilter");
    v.push_back("ImageAlphaCollectionSmoothBlend");
    v.push_back("ImageRandomColormap");
    v.push_back("ImageRandomColormapAlphaBlend");
    v.push_back("ImageRandomColormapAlphaScale");
    v.push_back("ImageRandomColormapSubFilter");
    v.push_back("ImageShuffle");
    v.push_back("ImageSubFilter");
    v.push_back("ImageAlphaBlendWithFrameSubFilter");
    v.push_back("ImageFadeInOut");
    v.push_back("ImageFadeBlackInOut");
    v.push_back("ImageFadeBlackInOutSubFilter");
    v.push_back("ImageFadeFrameInOut");
    v.push_back("ImageFadeFrameInOutSubFilter");
    v.push_back("ImageFadeDouble");
    v.push_back("BlendSubFilterAndImage");
    v.push_back("FadeSubFilter");
    v.push_back("FadeSubFilterRev");
    v.push_back("ImageBlendSubFilterMedianBlend");
    v.push_back("FadeSubFilterXor");
    v.push_back("BlurXorSubFilter");
    v.push_back("ColorFlashIncrease");
    v.push_back("ScaleFilter");
    v.push_back("NegativeDarkenXor");
    v.push_back("ImageXor_SubFilter");
    v.push_back("NegateBlendSubFilter");
    v.push_back("StrobeNegatePixel");
    v.push_back("StrobeNegateInOut");
    v.push_back("ImageStrobeOnOff");
    v.push_back("AlphaStrobeBlend");
    v.push_back("CannyRandomPixels");
    v.push_back("FrameImageFadeInOut");
    v.push_back("FrameImageFadeInOutDouble");
    v.push_back("ChangeEachSecond");
    v.push_back("ShuffleImage");
    v.push_back("ChangeImageEachSecond");
    v.push_back("ChangeImageFilterOnOff");
    v.push_back("ChangeXorEachSecond");
    v.push_back("MorphXor");
    v.push_back("MorphXorWithSubFilter");
    v.push_back("MirrorEachSecond");
    v.push_back("MirrorReverseSubFilter");
    v.push_back("MirrorReverseSubFilterAlphaBlend");
    v.push_back("Mirror_Xor_Combined");
    v.push_back("MirrorFrameShuffle");
    v.push_back("MirrorShuffleSmooth");
    v.push_back("Mirror_Xor_Smooth");
    v.push_back("XorFrameShuffle");
    v.push_back("XorMirrorBlendFrame");
    v.push_back("ImageXorSmooth");
    v.push_back("SmoothSubFilter64");
    v.push_back("SmoothMedian64");
    v.push_back("SmoothMedian32_SubFilter");
    v.push_back("SmoothAlphaMedian_SubFilter");
    v.push_back("SmoothImage_SubFilter");
    v.push_back("SmoothImageMedian_SubFilter");
    v.push_back("SmoothImageAndSubFilter");
    v.push_back("SmoothSubFilter90");
    v.push_back("SmoothMedianImageSubFilter16");
    v.push_back("ImageNegate");
    v.push_back("ImageNegateAlphaBlend");
    v.push_back("ImageNegateAlphaBlendSubFilter");
    v.push_back("FrameNegateAlphaBlendImage");
    v.push_back("DarkTrailsEffect");
    v.push_back("DarkNegate");
    v.push_back("ChannelSortMedianBlend");
    v.push_back("MatrixCollectionMirrorDirection");
    v.push_back("StrobeRandomChannel");
    v.push_back("SplitFramesSort");
    v.push_back("SplitFrameSortSubFilter");
    v.push_back("MedianBlend64");
    v.push_back("SplitFrameFilter");
    v.push_back("SplitFrameBlend");
    v.push_back("SplitFrameBlendSubFilter");
    v.push_back("SplitFrameCollection");
    v.push_back("SplitFrameMirror");
    v.push_back("RandomChannels");
    v.push_back("SmoothRandomChannels");
    v.push_back("SmoothChannelSubFilter");
    v.push_back("IncreaseRGB");
    v.push_back("IncreaseColor");
    v.push_back("SaturateBlend");
    v.push_back("SaturateBlendSubFilter");
    v.push_back("MaxRGB");
    v.push_back("XorDifferenceFilter");
    v.push_back("AlphaBlendChannelSort");
    v.push_back("ColorTrailsFilter");
    v.push_back("ColorTrailsSubFilter");
    v.push_back("DarkNegateRainbowMedian");
    v.push_back("IncreaseQuick");
    v.push_back("IncreaseRandomIndex");
    v.push_back("ImageChannelSubFilter");
    v.push_back("ImageChannelChangeSubFilter");
    v.push_back("ImageChannelRandom");
    v.push_back("ImageChannelRandomSubFilter");
    v.push_back("PixelateBlur");
    v.push_back("PixelateBlock");
    v.push_back("PixelateNoResize12");
    v.push_back("PixelateNoResize8");
    v.push_back("PixelateNoResize32");
    v.push_back("PixelateNoResize16");
    v.push_back("PixelateNoResize24");
    v.push_back("PixelateBlurFilter");
    v.push_back("PixelateBlurSubFilter");
    v.push_back("MirrorXorImage");
    v.push_back("MirrorXorSubFilter");
    v.push_back("PixelXorImageSubFilter");
    v.push_back("PixelAlphaImageSubFilter");
    v.push_back("PixelScaleImageSubFilter");
    v.push_back("PixelImageSubFilter");
    v.push_back("PixelImageTex");
    v.push_back("PixelImageXorSubFilter");
    v.push_back("PixelRowMedianBlend");
    v.push_back("IntertwineRows32");
    v.push_back("IntertwineRows16");
    v.push_back("IntertwineRows8");
    v.push_back("IntertwineAlpha");
    v.push_back("IntertwineRowsSubFilter");
    v.push_back("IntertwineRows4");
    v.push_back("Intertwine64x4");
    v.push_back("Intertwine64X");
    v.push_back("Intertwine64XSubFilter");
    v.push_back("Intertwine64XPixel");
    v.push_back("IntertwinePixels");
    v.push_back("IntertwineColsX");
    v.push_back("IntertwineCols16");
    v.push_back("IntertwineCols32");
    v.push_back("IntertwineCols8");
    v.push_back("MirrorIntertwine");
    v.push_back("MirrorIntertwineImage");
    v.push_back("MirrorIntertwineImageSubFilter");
    v.push_back("IntertwineImageSubFilter");
    v.push_back("BlendWithImage");
    v.push_back("IntertwineRowsReverse32");
    v.push_back("IntertwineRowsReverse16");
    v.push_back("IntertwineRowsReverse8");
    v.push_back("IntertwineRowsReverse64X");
    v.push_back("IntertwineRowsSwitch32");
    v.push_back("IntertwineRowsSwitch16");
    v.push_back("IntertwineRowsSwitch8");
    v.push_back("IntertwineRows64");
    v.push_back("IntertwineRowsReverse64");
    v.push_back("IntertwineRowsSwitch64");
    v.push_back("IntertwineShuffle");
    v.push_back("InterwtineAlphaSubFilter");
    v.push_back("IntertwineImageAndSubFilter");
    v.push_back("IntertwineRowsAndCols");
    v.push_back("IntertwineRowsAndColsSubFilter");
    v.push_back("MatrixCollectionVariable");
    v.push_back("MatrixCollectionVariableSubFilter");
    v.push_back("MatrixCollectionVariableImage");
    v.push_back("MatrixCollectionVariableImageSubFilter");
    v.push_back("SmoothImageAverageSubFilter");
    v.push_back("CallSubFilterBlend50");
    v.push_back("CallSubFilterBlend25");
    v.push_back("CallSubFilterBlend75");
    v.push_back("IntertwineColsX2");
    v.push_back("SmoothFrame64");
    v.push_back("SmoothFrame32");
    v.push_back("MatrixCollectionMedianBlendFilter");
    v.push_back("MedianBlendIncreaseFilter");
    v.push_back("MedianBlendNegate");
    v.push_back("BlendWithFrameAndImageSubFilter");
    v.push_back("ImageIntertwine32");
    v.push_back("ImageIntertwine16");
    v.push_back("ImageIntertwine8");
    v.push_back("ImageIntertwine64X");
    v.push_back("IntertwineDoubleSubFilter");
    v.push_back("IntertwineDoubleImageSubFilter");
    v.push_back("IntertwineEachRowXSubFilter");
    v.push_back("IntertwineGhost");
    v.push_back("IntertwineGhost32");
    v.push_back("IntertwineGhostSubFilter");
    v.push_back("BlendWithOldFrame");
    v.push_back("BlendWith16thFrame");
    v.push_back("BlendTrailsSubFilter");
    v.push_back("LineMoveInOut");
    v.push_back("MatrixCollectionTrails");
    v.push_back("MatrixCollectionRandom");
    v.push_back("MatrixCollectionRandomSmooth");
    v.push_back("MatrixCollectionRandomDouble");
    v.push_back("MatrixCollectionAddImage");
    v.push_back("MatrixCollectionAlphaBlendSubFilter");
    v.push_back("MatrixCollectionImageBlendSubFilter");
    v.push_back("TwitchinGlitchin");
    v.push_back("IntertwineRowsImageAlphaBlend");
    v.push_back("ChannelSortSubFilter");
    v.push_back("IntertwineRow2");
    v.push_back("IntertwineRow720p");
    v.push_back("IntertwineRow1080p");
    v.push_back("IntertwineRow720pX2");
    v.push_back("IntertwineRow1080pX2");
    v.push_back("IntertwineCols1280");
    v.push_back("IntertwineRowCols1280x720");
    v.push_back("IntertwineRowsImage");
    v.push_back("MedianBlendSquare");
    v.push_back("SmoothIntertwineMedianBlend");
    v.push_back("SmoothBlendMedian");
    v.push_back("SmoothDoubleSubFilter");
    v.push_back("AlphaBlendMedianSubFilter");
    v.push_back("ReverseOnOff");
    v.push_back("SmoothReverseSubFilter");
    v.push_back("IntertwineWithSubFilter");
    v.push_back("IntertwineWithImageAndSubFilter");
    v.push_back("IntertwineFrameWithImage");
    v.push_back("InterlaceVerticalFilter");
    v.push_back("InterlaceHorizontalFilter");
    v.push_back("IntertwineImageFlip");
    v.push_back("IntertwineImageFlipSubFilter");
    v.push_back("IntertwineFlipImageAndSubFilter");
    v.push_back("IntertwineImageOnOff");
    v.push_back("IntertwineVideoShuffle");
    v.push_back("ImageFlipFilter");
    v.push_back("FlipFrameFilter");
    v.push_back("AlphaBlendImageAndSubFilter");
    v.push_back("SquareFill");
    v.push_back("StretchAlphaBlend");
    v.push_back("StretchAlphaBlendWidth");
    v.push_back("StretchAlphaBlendHeight");
    v.push_back("StretchAlphaBlendTwitch");
    v.push_back("StretchAlphaBlednTwitchFast");
    v.push_back("StretchRandomValues");
    v.push_back("ImageRandomValues");
    v.push_back("AlphaBlendTrails");
    v.push_back("VideoStretchHorizontal");
    v.push_back("VideoStretchVertical");
    v.push_back("StrobeTrailsFilter");
    v.push_back("AlphaBlendTrailsReverse");
    v.push_back("ShadowAlphaTrails16");
    v.push_back("ShadowAlphaTrailsReset");
    v.push_back("SetColormap");
    v.push_back("ShadowAlphaTrails");
    v.push_back("ShadowAlphaTrails64");
    v.push_back("MirrorLeft");
    v.push_back("MirrorRight");
    v.push_back("MirrorBackAndForth");
    v.push_back("FadeFromColorToColor");
    v.push_back("FadeFromColorToColorImage");
    v.push_back("Random_Filter");
    v.push_back("FlipY_Axis");
    v.push_back("FlipX_Axis");
    v.push_back("FlipBoth");
    v.push_back("FlipMirrorAlphaBlend");
    v.push_back("Random_FilterX2");
    v.push_back("Random_FilterSubFilter");
    v.push_back("IntertwineRow480pX2");
    v.push_back("LowFi");
    v.push_back("HighToLow");
    v.push_back("LowToHigh");
    v.push_back("MoveHighAndLow");
    v.push_back("StretchSubFilter");
    v.push_back("Quality480");
    v.push_back("Quality720");
    v.push_back("Quality1080");
    v.push_back("StretchVerticalMirror");
    v.push_back("ImageLoFi");
    v.push_back("ImageLofiAlphaBlend");
    v.push_back("ImageAlphaBlendScale");
    v.push_back("FrameStretchAlphaBlend");
    v.push_back("BlurHighToLow");
    v.push_back("ImageHighToLowAlpha");
    v.push_back("MirrorTopToBottom");
    v.push_back("MirrorBottomToTop");
    v.push_back("FlashRGB_SubFilter");
    v.push_back("MirrorSwitch");
    v.push_back("MirrorSwitchSubFilter");
    v.push_back("MirrorSwitchFlip");
    v.push_back("BlendImageLayer");
    v.push_back("StrobeRandomFilter");
    v.push_back("AlphaBlendRandom_Filter");
    v.push_back("DoubleRandomAlphaImageSubFilter");
    v.push_back("MirrorLeftMirrorRightBlend");
    v.push_back("MirrorTopMirrorBottomBlend");
    v.push_back("MirrorAll");
    v.push_back("ImageDiff");
    v.push_back("ImageDiffSubFilter");
    v.push_back("RestoreBlack");
    v.push_back("OrigBlendSubFilter");
    v.push_back("OrigAndCurrentRandomX2");
    v.push_back("FillPixelsImage");
    v.push_back("AverageHorizontalDistortion");
    v.push_back("AlphaBlendImageWithOrigSource");
    v.push_back("resizeFrameWidth");
    v.push_back("resizeFrameHeight");
    v.push_back("resizeFrameWidthAndHeight");
    v.push_back("resizeImageWidth");
    v.push_back("resizseImageHeight");
    v.push_back("resizeImageWidthAndHeight");
    v.push_back("resizeImageAndFrameBlend");
    v.push_back("resizeImageWidthAndHeightSubFilter");
    v.push_back("resizeImageFrameWidth");
    v.push_back("resizeImageFrameHeight");
    v.push_back("resizeImageFrameWidthAndHeight");
    v.push_back("resizeImageRandom");
    v.push_back("resizeFrameRandom");
    v.push_back("resizeFrameImageFlash");
    v.push_back("RandomFlip");
    v.push_back("ColorsFilter");
    v.push_back("MirrorDoubleVision");
    v.push_back("ColorFadeFilter");
    v.push_back("ColorChannelMoveUpAndDown");
    v.push_back("MedianStrobe");
    v.push_back("DifferenceSubFilter");
    v.push_back("AlphaBlendFlippedFilter");
    v.push_back("ColorVariableBlend");
    v.push_back("ColorXorBlend");
    v.push_back("ColorAddBlend");
    v.push_back("IntertwineRowsShadow720p");
    v.push_back("IntertwineRowsAlpha720p");
    v.push_back("IntertwineByFrameDown");
    v.push_back("IntertwineByFrameUp");
    v.push_back("IntertwineFrameFast");
    v.push_back("IntertwineFrame360");
    v.push_back("IntertwineFrame360X");
    v.push_back("IntertwineFrameTwitch");
    v.push_back("IntertwineFrameDark");
    v.push_back("IntertwineFrame360_Reverse");
    v.push_back("IntertwineFrameBlend");
    v.push_back("IntertwineXorCollection");
    v.push_back("IntertwineFrame720X");
    v.push_back("IntertwineFrame1080X");
    v.push_back("IntertwineFrameImage1080X");
    v.push_back("RandomXorMultiThreadTest");
    v.push_back("SelfAlphaBlendMultiThread");
    v.push_back("MedianBlendMultiThread");
    v.push_back("BytePixelSort");
    v.push_back("SortedImageColorVariable");
    v.push_back("SmoothColorVariableImageBlend");
    v.push_back("BlendChannelXor");
    v.push_back("ColorShiftXor");
    v.push_back("RandomSquares");
    v.push_back("RandomImageSquares");
    v.push_back("Random_FilterX2_SubFilter");
    v.push_back("FuzzyFilter");
    v.push_back("XorMultiply");
    v.push_back("Grayscale");
    v.push_back("ColorShadowBlend");
    v.push_back("FlashMatrixTrails");
    v.push_back("GhostTrails");
    v.push_back("MedianBlend_Random_Filter");
    v.push_back("IntertwineMirrorEnergy");
    v.push_back("IntertwineMultipleRows");
    v.push_back("GhostTwitch");
    v.push_back("GhostTwitchTrails");
    v.push_back("Dyskinesia");
    v.push_back("Mirror_Rainbow_Blur");
    v.push_back("MirrorMedianBlur");
    v.push_back("VideoTwitch");
    v.push_back("ErodeFilter");
    v.push_back("DilateFilter");
    v.push_back("MirrorLeftBottomToTop");
    v.push_back("MirrorRightTopToBottom");
    v.push_back("BilateralFilter");
    v.push_back("BilateralFilterFade");
    v.push_back("BilateralBlend");
    v.push_back("BilateralBlendFade");
    v.push_back("BilateralFilterScale");
    v.push_back("ColorRGB_IncDec");
    v.push_back("ColorCollection");
    v.push_back("ColorCollectionRandom");
    v.push_back("ColorCollectionStrobeShift");
    v.push_back("ColorCollectionRandom_Filter");
    v.push_back("ColorCollectionShift");
    v.push_back("CollectionEnergy");
    v.push_back("ColorCollectionInterlace");
    v.push_back("ColorCollectionStrobeShake");
    v.push_back("ColorCollectionSubFilter");
    v.push_back("ColorCollectionShiftSubFilter");
    v.push_back("ColorCollectionSubtle");
    v.push_back("ColorCollection64");
    v.push_back("ColorCollectionSubtleStrobe");
    v.push_back("CollectionRandom");
    v.push_back("CollectionRandomSubFilter");
    v.push_back("CollectionImage");
    v.push_back("CollectionAlphaXor");
    v.push_back("ColorCollection64X");
    v.push_back("ColorCollectionSwitch");
    v.push_back("ColorCollectionRGB_Index");
    v.push_back("ColorCollectionRGBStrobeSubFilter");
    v.push_back("ColorCollectionGhostTrails");
    v.push_back("ColorCollectionScale");
    v.push_back("ColorCollectionReverseStrobe");
    v.push_back("CollectionAlphaBlend_SubFilter");
    v.push_back("ColorCollectionXorPixel");
    v.push_back("BlendWithSource25");
    v.push_back("BlendWithSource50");
    v.push_back("BlendWithSource75");
    v.push_back("BlendWithSource100");
    v.push_back("ColorCollectionXorOffsetFlash");
    v.push_back("ColorCollectionMatrixGhost");
    v.push_back("MildStrobe");
    v.push_back("ReduceBy50");
    v.push_back("AlphaBlendWithSourceScale");
    v.push_back("ColorPositionAverageXor");
    v.push_back("ColorPositionXor");
    v.push_back("ColorPositionXorMedianBlend");
    v.push_back("CannyStrobe");
    v.push_back("LaplacianStrobe");
    v.push_back("LaplacianStrobeOnOff");
    v.push_back("ColorCollectionPixelXor");
    v.push_back("GrayStrobe");
    v.push_back("ColorStrobeXor");
    v.push_back("ColorGhost");
    v.push_back("ColorCollectionTwitchSubFilter");
    v.push_back("BlurredOutXor");
    v.push_back("BoxFilter");
    v.push_back("DizzyFilter");
    v.push_back("Buzzed");
    v.push_back("BlendWithImage25");
    v.push_back("BlendWithImage50");
    v.push_back("BlendWithImage75");
    v.push_back("BuzzedDark");
    v.push_back("AlphaBlendImageDownUp");
    v.push_back("BlendWithImageAndSource");
    v.push_back("PixelSourceFrameBlend256");
    v.push_back("SplitMatrixCollection");
    v.push_back("RectangleGlitch");
    v.push_back("PositionShift");
    v.push_back("ColorCollectionMovementIndex");
    v.push_back("Shake");
    v.push_back("Disoriented");
    v.push_back("ColorCollectionPositionStrobe");
    v.push_back("ColorCollectionStrobeBlend");
    v.push_back("AlphaBlendStoredFrames");
    v.push_back("SplitMatrixSortChannel");
    v.push_back("SplitMatrixSortChannelArrayPosition");
    v.push_back("SplitMatrixSortChannelImage");
    v.push_back("ShiftColorLeft");
    v.push_back("CycleInAndOutRepeat");
    v.push_back("ColorCollectionShuffle");
    v.push_back("BlendFromXtoY");
    v.push_back("BlendImageXtoY");
    v.push_back("BlendColorImageStrobeSubFilter");
    v.push_back("BlendByColsSubFilter");
    v.push_back("BlendByColsImage");
    v.push_back("IntertwineX");
    v.push_back("IntertwineY");
    v.push_back("IntertwineSubFilterX");
    v.push_back("IntertwineSubFilterY");
    v.push_back("IntertwineImageX");
    v.push_back("IntertwineImageY");
    v.push_back("InertwineImageSubFilter");
    v.push_back("IntertwineHorizontalImageSubFilter");
    v.push_back("InterwtineHorizontalImageSubFilterByIndex");
    v.push_back("IntertwineHorizontalImageSubFilterMatrixCollection");
    v.push_back("BlendForwardAndBack16");
    v.push_back("BlendForwardAndBack32");
    v.push_back("BlendForwardAndBack64");
    v.push_back("BlendForwardAndBack8_RandomStrobe");
    v.push_back("AlphaBlendWithThreeCollections");
    v.push_back("AlphaBlendArrayRandom");
    v.push_back("AlphaBlendArrayTrailsFilters");
    v.push_back("AlphaBlendMirrorChannelSort");
    v.push_back("ColorCollectionAlphaBlendArray");
    v.push_back("AlphaBlendArrayExpand");
    v.push_back("ColorImageMatrixFade");
    v.push_back("ColorImageMatrixFadeFast");
    v.push_back("ColorImageMatrixFadeDirection");
    v.push_back("ColorImageMatrixFadeDirectionBlend");
    v.push_back("ColorMatrixCollectionPixelation");
    v.push_back("ColorMatrixColllectionImagePixelation");
    v.push_back("ColorMatrixCollectionImagePixelationSubFilter");
    v.push_back("ColorImageMatrixMedianBlend");
    v.push_back("ColorImageBlendPixelMedian");
    v.push_back("ColorImageBlendOrigMedian");
    v.push_back("ColorImageBlendSource");
    v.push_back("ColorImageMatrixFadeOnAndOff");
    v.push_back("ColorImageMatrixRect");
    v.push_back("ColorImageBackAndForth");
    v.push_back("ColorImageFadeReallyFast");
    v.push_back("ColorImageSubFilterXor");
    v.push_back("ColorImageAllXor");
    v.push_back("ColorChannelIteration");
    v.push_back("ColorImageMatrixFadeVariable");
    v.push_back("ColorXorImageFade");
    v.push_back("ColorFreezeBlend");
    v.push_back("ColorImageFill");
    v.push_back("ColorImageFillMatrix");
    v.push_back("ColorImageFillSubFilter");
    v.push_back("ColorImagePixelsResetSubFilter");
    v.push_back("ColorIncrementReset");
    v.push_back("ColorPixelArray2D");
    v.push_back("ManualShell");
    v.push_back("ColorIncrementRandomReset");
    v.push_back("ColorImageIncrementResetCollection");
    v.push_back("ColorMoveDown");
    v.push_back("ColorMoveDownResetMedianBlend");
    v.push_back("ColorMoveDownSmoothMedianBlend");
    v.push_back("PixelRandom3");
    v.push_back("ShiftMatrixLeft");
    v.push_back("ShiftMatrixLeftSubFilter");
    v.push_back("ShiftMatrixUpSubFilter");
    v.push_back("PixelatePixelValues");
    v.push_back("StretchCollection");
    v.push_back("PsychedelicSlitScan");
    v.push_back("SineValue");
    v.push_back("SineTransitionUpLeft");
    v.push_back("TemporaryTrails");
    v.push_back("StrobingPixelDissolve");
    v.push_back("ImagePixelFrameBlend");
    v.push_back("PreviewFilters");
    v.push_back("EachFilterSubFilter");
    v.push_back("EachFilterRandomStartSubFilter");
    v.push_back("PixelPsychosis");
    v.push_back("PixelCloud");
    v.push_back("PiixelXorBlendFrame");
    v.push_back("PixelImageBlendFrame");
    v.push_back("PixelCollectionRandom");
    v.push_back("PixelCollectionSubFilter");
    v.push_back("PixelCollectionMatrixSubFilter");
    v.push_back("PixelCollectionIncreaseSubFilter");
    v.push_back("PixelCollectionImageBlend");
    v.push_back("ColorPulseIncrease");
    v.push_back("PixelCollectionLongMatrixSubFilter");
    v.push_back("ColorPulseImageIncreaseSubFilter");
    v.push_back("ColorPulseRandom");
    v.push_back("ColorPulseImage");
    v.push_back("ColorPulseAlpha");
    v.push_back("ColorLower75");
    v.push_back("ColorImageMedianBlend");
    v.push_back("ColorDullBlur");
    v.push_back("ColorImageDull");
    v.push_back("ColorImageBlendWithFrame");
    v.push_back("ColorImageBlendSubFilter");
    v.push_back("ColorMatrixImageFilter");
    v.push_back("ColorXorMatrixImage");
    v.push_back("MatrixColorBlur");
    v.push_back("ColorImageLower");
    v.push_back("BlurSubFilter8");
    v.push_back("LightBlend");
    v.push_back("LightBlendXor");
    v.push_back("Source_ImageSubFilter");
    v.push_back("XorZeroImage");
    v.push_back("SlowDownFilterSubFilter");
    v.push_back("VariableFilterSubFilter");
    v.push_back("SingleFrameGlitch");
    v.push_back("ColorFadeSlow");
    v.push_back("FadeBetweenSubFilter");
    v.push_back("FadeBetweenFrame");
    v.push_back("TrailsTwitch");
    v.push_back("Tremors");
    v.push_back("XorImageIndexPixel");
    v.push_back("ImageStrobeMedianBlend");
    v.push_back("StrobeImageArrayPosition");
    v.push_back("OppositeImageArray");
    v.push_back("NoMedianBlurBlendMultiThread");
    v.push_back("NegateNoBlurMedian");
    v.push_back("PulseIncreaseVariableSpeed");
    v.push_back("Source25_Image75");
    v.push_back("LightStrobe");
    v.push_back("StrobeRedChannel");
    v.push_back("StrobeGreenChannel");
    v.push_back("StrobeBlueChannel");
    v.push_back("StrobeOnOffRandomChannel");
    v.push_back("StrobeLightFlashRandomChannel");
    v.push_back("LostSideDeadImageSubFilter");
    v.push_back("SwapImageFrameSubFilter");
    v.push_back("PulseIncreaseFast");
    v.push_back("FibonacciXor");
    v.push_back("ColorFibonacci");
    v.push_back("ImageFibonacci");
    v.push_back("ImageFibonacciInAndOut");
    v.push_back("ImageFibonacciStrobe");
    v.push_back("ImageFibonacciMedianBlend");
    v.push_back("ImageFibonacciInAndOutSubFilter");
    v.push_back("ImageKaleidoscopeSubFilter");
    v.push_back("ImageMirrorLeftRight");
    v.push_back("ImageMirrorShiftUpLeftDownRight");
    v.push_back("SelfScaleRefined");
    v.push_back("MetalMedianBlend");
    v.push_back("SelfScaleRefinedRGB");
    v.push_back("ImageFreezeReleaseRepeat");
    v.push_back("ImageReplaceColorIntensity");
    v.push_back("ImageReplaceColorIntensitySubFilter");
    v.push_back("ImageReplaceColorIntensitySubFilterInOut");
    v.push_back("ImageFillColor");
    v.push_back("MultiRandFilter");
    v.push_back("MultiFilter");
    v.push_back("GradientRandom");
    v.push_back("LineMedianBlend");
    v.push_back("PerfectMedianBlend");
    v.push_back("ImageRowAlphaSubFilter");
    v.push_back("IndexPixelRowSubFilter");
    v.push_back("IndexSourceRowSubFilter");
    v.push_back("IndexSourceBlendSubFilter");
    v.push_back("BlendFilterWithSubFilter");
    v.push_back("FadeFilterWithSubFilter");
    v.push_back("AlphaBlendByRowSubFilter");
    v.push_back("ColorMatrixTrailsSubFilter");
    v.push_back("LoFi_320x240_Interlaced");
    v.push_back("LoFi_320x240");
    v.push_back("LoFi_320x240_SubFilter");
    v.push_back("LoFi_320x240_Alpha_SubFilter");
    v.push_back("LoFi_160x120_SubFilter");
    v.push_back("LoFi_Trails");
    v.push_back("LoFi_ImageScaleSubFilter");
    v.push_back("MedianShift");
    v.push_back("ImageAlienKaleidoscope");
    v.push_back("ImageMirror_Left");
    v.push_back("ImageMirror_Right");
    v.push_back("ImageMirror_Up");
    v.push_back("ImageMirror_Down");
    v.push_back("ImageMirror_LeftSubFilter");
    v.push_back("ImageMirror_RightSubFilter");
    v.push_back("ImageMirror_UpSubFilter");
    v.push_back("ImageMirror_DownSubFilter");
    v.push_back("ImageMirrorKaleidoscope");
    v.push_back("ExactImageSubFilter");
    v.push_back("AlphaImageSourceScale");
    v.push_back("GhostMirrorReversed");
    v.push_back("GhostMirrorFade");
    v.push_back("MirrorFadeLeftRight");
    v.push_back("FadeMirrorSubFilter");
    v.push_back("MirrorFadeUpDown");
    v.push_back("DarkTrailsKaleidoscope");
    v.push_back("MirrorFadeAll");
    v.push_back("KaleidoscopeMirrorSubFilter");
    v.push_back("ImageKaleidoscopeMirrorAlphaSubFilter");
    v.push_back("BlendOppositesSubFilter");
    v.push_back("AlphaBlendMirrorFade");
    v.push_back("DarkenMedianBlend");
    v.push_back("ReduceMedianBlend");
    v.push_back("DarkColors");
    v.push_back("DarkenChannelXorNoMedian");
    v.push_back("DarkenStrobeMedianBlend");
    v.push_back("DarkenImageAlphaBlend");
    v.push_back("MatrixStrobeTrails");
    v.push_back("MatrixStrobeMedianBlend");
    v.push_back("ImageEnergyKaleidoscopeSubFilter");
    v.push_back("ImageEnergyKaleidoscopeBlendSubFilter");
    v.push_back("ImageEnergyKaleidoscopeEvenSubFilter");
    v.push_back("ImageEnergyFadeSubFilter");
    v.push_back("ImageEnergizeBlendFilter");
    v.push_back("ImageEnergizeSubFilter");
    v.push_back("MatrixCollectionSmoothedSubFilter");
    v.push_back("MonoColorTrails");
    v.push_back("TremorMonoColorTrails");
    v.push_back("StrobeMonoColorTrails");
    v.push_back("StrobeEffectColorTrails");
    v.push_back("PixelStrobeColorTrails");
    v.push_back("ElectricImageFilter");
    v.push_back("ImageEnergyStrobeSubFilter");
    v.push_back("Twin");
    v.push_back("TwinKaleidoscope");
    v.push_back("MedianBlendMultiThread_2160p");
    v.push_back("MedianBlendMultiThreadByFour");
    v.push_back("MedianBlendMultiThreadByEight");
    v.push_back("MedianBlendMultiThreadByTweleve");
    v.push_back("MedianBlendMultiThreadByThree");
    v.push_back("MedianBlendIncrease");
    v.push_back("MedianBlendIncrease16");
    v.push_back("StrangeGlitch64");
    v.push_back("StrangeGlitch16");
    v.push_back("CollectionMatrixOutline");
    v.push_back("CollectionMatrixSubFilter");
    v.push_back("CollectionMatrixRandomMedianBlend");
    v.push_back("ImageCollectionMatrixOutline");
    v.push_back("ImageCollectionMatrixOutlineSubFilter");
    v.push_back("ImageCollectionMatrixFillSubFilter");
    v.push_back("ImageCollectionMatrixFadeInOutSubFilter");
    v.push_back("ImageCollectionMatrixIntensitySubFilter");
    v.push_back("ImageCollectionMatrixMedianSubFilter");
    v.push_back("CollectionMatrxOutlineAlphaMedianBlend");
    v.push_back("ImageSourceOrigXor");
    v.push_back("MatrixCollectionAuraTrails");
    v.push_back("MatrixCollectionAuraTrails32");
    v.push_back("PixelIntensityFillSubFilter");
    v.push_back("SmoothImageFrameSubFilter");
    v.push_back("ImageCycleBlend");
    v.push_back("ImageCycleAlphaBlend");
    v.push_back("ImageCycleXor");
    v.push_back("ImageCycleMedian");
    v.push_back("ImageCycleAlphaSubFilter");
    v.push_back("ImageCycleSmoothAlphaBlend");
    v.push_back("ImageCycleSmoothAlphaBlend32");
    v.push_back("BlendWithSourceMedianBlendMultiThreadEight");
    v.push_back("BlendWithImageMedianMutliThreadEight");
    v.push_back("GradientBlendWithImage");
    v.push_back("DarkColorMovement");
    v.push_back("DarkColorFibonacci");
    v.push_back("UseFilterOnAndOffSubFilter");
    v.push_back("UseFilterOnAndOffEachQuarterSecondSubFilter");
    v.push_back("UseFilterOnAndOffByEightSubFilter");
    v.push_back("UseImageOnAndOffByEight");
    v.push_back("UseImageOnAndOff");
    v.push_back("SelfAlphaScale_Down");
    v.push_back("PsychoticVision");
    v.push_back("DarkPsychoticVision");
    v.push_back("Mirror_ReverseColor");
    v.push_back("ImageColorCycleMorph");
    v.push_back("ImageBlendAndSubFilter");
    v.push_back("ImageSquareShrink");
    v.push_back("ImageSquareShrinkFast");
    v.push_back("ImageSquareShrinkAlpha");
    v.push_back("ImageSquareExpand");
    v.push_back("ImageKaleidoscopeNoBlur");
    v.push_back("KaleidoscopeSubFilter");
    v.push_back("ImageSquareShrinkSubFilter");
    v.push_back("DifferenceReplaceSubFilter");
    v.push_back("InvertedDifferenceReplaceSubFilter");
    v.push_back("ImageInvertedDifferenceReplaceSubFilter");
    v.push_back("ImageDifferenceReplaceSubFilter");
    v.push_back("DifferenceReplaceSubFilterAlphaBlend");
    v.push_back("GradientGlitch");
    v.push_back("ImageGradientBlend");
    v.push_back("MedianBlendGradientMultiThread");
    v.push_back("MedianBlendGradientDarkMultiThread");
    v.push_back("GradientAlphaBlend");
    v.push_back("GradientFilter");
    v.push_back("GradientFilterAlphaBlend");
    v.push_back("MedianBlendGradientFilterMultiThread");
    v.push_back("MedianBlendGraidentFilterDarkMultiThread");
    v.push_back("ColorOrderSwap");
    v.push_back("ColorOrderSwapMap");
    v.push_back("MedianBlendSwapMapMultiThread");
    v.push_back("RandomGradientColors");
    v.push_back("GradientColorMap");
    v.push_back("GradientXor");
    v.push_back("RandomSubFilter");
    v.push_back("ShuffleSubFilter");
    v.push_back("Shuffle_Filter");
    v.push_back("RandomOrigFrame");
    v.push_back("ColorVariableRectangles");
    v.push_back("VariableRectangles");
    v.push_back("VariableRectanglesSimple");
    v.push_back("VariableRectanglesExtra");
    v.push_back("VariableRectangleImageAlphaBlend");
    v.push_back("MirrorSwitchMode");
    v.push_back("MirrorSwitchLeftRight");
    v.push_back("VariableRectanglesSmall");
    v.push_back("VariableRectanglesLarge");
    v.push_back("VariableRectanglesImageCollection");
    v.push_back("VariableRectanglesVariableImageSubFilter");
    v.push_back("RainbowXorStrobeBlend");
    v.push_back("IncreaseRainbowXorBlend");
    v.push_back("ColorStrobeIndexSubFilter");
    v.push_back("MatrixCollectionRandomFrames");
    v.push_back("MatrixCollectionFrames");
    v.push_back("MatrixCollectionFramesLeft");
    v.push_back("MatrixCollectionFramesMirrorLeft");
    v.push_back("MatrixCollectionFramesTop");
    v.push_back("FrameSqueeze");
    v.push_back("MatrixCollectionBlendLeftToRight");
    v.push_back("MedianBlendMatrixCollection");
    v.push_back("MedianBlendMatrixCollection_2160p");
    v.push_back("MedianBlendMatrixCollection_2160p_X16");
    v.push_back("MatrixCollectionAdd");
    v.push_back("MatrixCollection_Xor");
    v.push_back("ImageMatrixCollectionBlend");
    v.push_back("MatrixCollectionNegateAdd");
    v.push_back("MatrixCollectionFrameRect");
    v.push_back("MatrixCollectionRows8x1_SubFilter");
    v.push_back("MatrixCollectionRows8x8_SubFilter");
    v.push_back("MatrixCollectionRows2x2_SubFilter");
    v.push_back("MatrixCollectionRows4x4_SubFilter");
    v.push_back("IntertwineBlock");
    v.push_back("IntertwineVertical");
    v.push_back("IntertwineImageVerticalSubFilter");
    v.push_back("MirrorDiamond");
    v.push_back("MirrorDiamondRight");
    v.push_back("MirrorDiamondReverse");
    v.push_back("MirrorDiamondBlend");
    v.push_back("StretchOutward");
    v.push_back("MirrorDiamondRandom");
    v.push_back("MatrixCollectionAlphaRow");
    v.push_back("MedianBlendCollectionAlphaRow");
    v.push_back("MedianBlendDoubleVision");
    v.push_back("MedianBlendMultiThreadScale");
    v.push_back("AcidShuffleMedian");
    v.push_back("MedianBlendSmoothAlpha");
    v.push_back("MirrorDiamondSubFilter");
    v.push_back("ImageFadeTo");
    v.push_back("ImageFadeToXor");
    v.push_back("MedianBlendImageFadeTo");
    v.push_back("MedianMirrorAllBlend");
    v.push_back("DigitalHaze");
    v.push_back("BlendWithColor");
    v.push_back("DigitalHazeBlend");
    v.push_back("VideoAlphaBlend25");
    v.push_back("VideoAlphaBlend50");
    v.push_back("VideoAlphaBlend75");
    v.push_back("VideoAlphaBlendSubFilter");
    v.push_back("VideoExactFrame");
    v.push_back("VideoAlphaFade");
    v.push_back("VideoAlphaImage");
    v.push_back("VideoAlphaImageFrame");
    v.push_back("VideoAlphaImageScale");
    v.push_back("VideoAlphaBlendImage25");
    v.push_back("VideoAlphaBlendImage50");
    v.push_back("VideoAlphaBlendImage75");
    v.push_back("VideoFrameFilterSubFilter");
    v.push_back("VideoFrameImageSubFilter");
    v.push_back("VideoAlphaSubFilter");
    v.push_back("VideoRoll");
    v.push_back("VideoRollReverse");
    v.push_back("VideoRollMedianBlend");
    v.push_back("VideoRollScaleMedianBlend");
    v.push_back("VideoSubFilter");
    v.push_back("MedianBlendWithColor");
    v.push_back("VideoImage");
    v.push_back("VideoImageFade");
    v.push_back("ColorMovement");
    v.push_back("MedianBlendColorMovement");
    v.push_back("ColorMovementRange");
    v.push_back("ColorPixelOrder");
    v.push_back("ColorPixelOrder32");
    v.push_back("MedianBlendMuliThread_Pixelate");
    v.push_back("SelfScaleGlitch");
    v.push_back("PixelGlitch");
    v.push_back("VideoPixelOnOffSwitch");
    v.push_back("VideoBlendImageSubFilter");
    v.push_back("VideoAlphaImageSubFilter");
    v.push_back("MedianBlendVideo");
    v.push_back("VideoFrameBlur8");
    v.push_back("VideoFrameBlur16");
    v.push_back("VideoFrameBlur32");
    v.push_back("VideoSourceFrameBlur8");
    v.push_back("VideoSourceFrameBlur16");
    v.push_back("VideoSourceFrameBlur32");
    v.push_back("VideoImageSourceFrame");
    v.push_back("slowRed");
    v.push_back("slowGreen");
    v.push_back("slowBlue");
    v.push_back("VideoMirrorAlpha");
    v.push_back("VideoMirrorLeftRight");
    v.push_back("VideoFlipFrames");
    v.push_back("VideoImageBlend");
    v.push_back("VideoCycleReverse");
    v.push_back("VideoCycleMedianReverse");
    v.push_back("VideoStrobeFrameOnOff");
    v.push_back("VideoMirror");
    v.push_back("VideoMedianBlendScale");
    v.push_back("VideoMedianBlend");
    v.push_back("VideoBlendSubFilter");
    v.push_back("VideoLoFi");
    v.push_back("VideoLoFiBlend");
    v.push_back("VideoLoFiSourceNormal");
    v.push_back("VideoDarken25");
    v.push_back("VideoDarken50");
    v.push_back("VideoDarken75");
    v.push_back("VideoDarkenBlend25");
    v.push_back("VideoDarkenBlend50");
    v.push_back("VideoDarkenBlend75");
    v.push_back("VideoBlendSmooth8");
    v.push_back("VideoBlendSmooth16");
    v.push_back("VideoBlendSmooth32");
    v.push_back("VideoXorSource");
    v.push_back("VideoXorSelfScale");
    v.push_back("MedianBlendNoBlurWithColor");
    v.push_back("VideoImageSmooth");
    v.push_back("ColorMapFilter");
    v.push_back("VideoColorMap");
    v.push_back("VideoColorMapAlphaBlend");
    v.push_back("ReduceColors");
    v.push_back("ReduceColorsRandom");
    v.push_back("ReduceColorsRed");
    v.push_back("ReduceColorsGreen");
    v.push_back("ReduceColorsBlue");
    v.push_back("BlendRandomPixels");
    v.push_back("MedianBlendMultiThread_LoFi");
    v.push_back("ExpandFrame");
    v.push_back("ExpandImage");
    v.push_back("ExpandVideo");
    v.push_back("ColorIncreaseFadeRGB");
    v.push_back("ColorIncreaseInOutRGB");
    v.push_back("AlphaVideoXor");
    v.push_back("DarkScaleNoBlur");
    v.push_back("LightBlur");
    v.push_back("LightMedianBlendMultiThread");
    v.push_back("ColorFadeMedianBlendMultiThread");
    v.push_back("ColorFadeBlendMedianBlendMultiThread");
    v.push_back("MedianBlendMultiThread32");
    v.push_back("MedianBlendMultiThread4");
    v.push_back("MedianBlendFadeWithColor");
    v.push_back("GaussianBlurMild");
    v.push_back("MedianBlendWithColorValue");
    v.push_back("MedianBlendWithColorInOutRGB");
    v.push_back("MedianBlendWithCollection16");
    v.push_back("FadeRtoGtoB");
    v.push_back("MedianBlendFadeRtoGtoB");
    v.push_back("FadeRtoGtoB_Increase");
    v.push_back("MedianBlendFadeRtoGtoB_Increase");
    v.push_back("FadeRandomChannel");
    v.push_back("FadeRandomChannel_Increase");
    v.push_back("MedianBlendFadeRandomChannel");
    v.push_back("GlitchyTrails");
    v.push_back("GlitchyXorTrails");
    v.push_back("GlitchedFilter");
    v.push_back("DisplaySometimesSubFilter");
    v.push_back("GlitchyVideoXorTrails");
    v.push_back("GltichedVideoFilter");
    v.push_back("DualGitchyVideoXorTrails");
    v.push_back("StaticGlitch");
    v.push_back("VideoMirrorMultiDirection");
    v.push_back("SoftwareGlitch_64");
    v.push_back("VideoMatrixBlendDouble");
    v.push_back("VideoMatrixFadeDouble");
    v.push_back("VideoMatrixColorSmooth");
    v.push_back("VideoMedianBlendShared8");
    v.push_back("VideoMedianBlendShared16");
    v.push_back("GlitchedGrid");
    v.push_back("ShuffleGlitch");
    v.push_back("ShuffleVideoMatrix");
    v.push_back("TruncateColor");
    v.push_back("TruncateColor25");
    v.push_back("TruncateColor75");
    v.push_back("TruncateVariable");
    v.push_back("TruncateVariableSlow");
    v.push_back("TruncateVariableScale");
    v.push_back("MedianBlendTruncate");
    v.push_back("MedianBlendTruncateScale");
    v.push_back("Variable_ColorScale");
    v.push_back("MedianBlendVariable_ColorScale");
    v.push_back("InOut_ColorScale");
    v.push_back("MedianBlendInOut_ColorScale");
    v.push_back("CollectionTruncate");
    v.push_back("MedianBlendCollectionTruncate");
    v.push_back("VideoBlendTruncate");
    v.push_back("RandomTruncateFrame");
    v.push_back("RandomStrobeMedianBlend");
    v.push_back("XorFade");
    v.push_back("ColorIncreaseXorFade");
    v.push_back("Pixels_InOut");
    v.push_back("PixelDistortionCloud");
    v.push_back("XorRow");
    v.push_back("UseOldRow");
    v.push_back("UseEveryOtherRow");
    v.push_back("UseOffRow");
    v.push_back("FrameJump");
    v.push_back("MedianBlendMultiThreadVariable");
    v.push_back("MedianBlendMultiThreadVariable32");
    v.push_back("MedianBlendMultiThreadVariable16");
    v.push_back("MedianBlendMultiThreadSkip8");
    v.push_back("BlockyTrails");
    v.push_back("BlockyTrails16");
    v.push_back("BlockyTrails32");
    v.push_back("BlockyTrails64");
    v.push_back("UseOffRowVert");
    v.push_back("UseOldRowVert");
    v.push_back("UseEveryOtherRowVert");
    v.push_back("UseOffRowDir");
    v.push_back("UseOldRowDir");
    v.push_back("UseEveryOtherRowDir");
    v.push_back("SetCurrentFrameStateAsSource");
    v.push_back("RegularTrails");
    v.push_back("UseOldRow64");
    v.push_back("UseOldRowVert64");
    v.push_back("UseOldRowDir64");
    v.push_back("GradientColorBlend");
    v.push_back("MedianBlendMultiThreadGradientGray");
    v.push_back("GradientRedBlend");
    v.push_back("GradientGreenBlend");
    v.push_back("MedianBlendMultiThreadGradientGreen");
    v.push_back("GradientBlueBlend");
    v.push_back("MedianBlendMultiThreadGradientBlue");
    v.push_back("GradientColorBlendAll");
    v.push_back("MedianBlendMultiThreadGradientAll");
    v.push_back("GradientXRed");
    v.push_back("GradientXGreen");
    v.push_back("GradientXBlue");
    v.push_back("GradientAll");
    v.push_back("MedianBlendMultiThreadAllGradients");
    v.push_back("StretchRowMatrix16");
    v.push_back("StretchRowMatrix32");
    v.push_back("StretchRowMatrix8");
    v.push_back("StretchColMatrix8");
    v.push_back("StretchColMatrix16");
    v.push_back("StretchColMatrix32");
    v.push_back("GradientRandomSwitch");
    v.push_back("GradientFlashColor");
    v.push_back("GradientFlashComponent");
    v.push_back("GradientRandomShift");
    v.push_back("MedianBlendMultiThreadGradientShift");
    v.push_back("MirrorIntertwineRows720");
    v.push_back("MirrorIntertwineRowsY");
    v.push_back("MirrorIntertwineRowsX_Y_Width_Height");
    v.push_back("MirrorTrailsLeft");
    v.push_back("MirrorTrailsLeftRightTopBottom");
    v.push_back("MirrorTrailsRightLeftBottomTop");
    v.push_back("MirrorTrailsFlash");
    v.push_back("MirrorLeftTopToBottom");
    v.push_back("MirrorRightBottomToTop");
    v.push_back("MirrorFlipLeft");
    v.push_back("MirrorFlipRight");
    v.push_back("MirrorFlipBottomLeft");
    v.push_back("MirrorFlipBottomRight");
    v.push_back("MirrorFlipXMirrorLeft");
    v.push_back("MirrorFlipXMirrorRight");
    v.push_back("MirrorFlipYMirrorLeft");
    v.push_back("MirrorFlipYMirrorRight");
    v.push_back("MirrorFlipXLeftTopToBottom");
    v.push_back("MirrorFlipXLeftBottomToTop");
    v.push_back("MirrorFlipXRightTopToBottom");
    v.push_back("MirrorFlipXRightBottomToTop");
    v.push_back("MirrorFlipYLeftTopToBottom");
    v.push_back("MirrorFlipYLeftBottomToTop");
    v.push_back("MirrorFlipYRightTopToBottom");
    v.push_back("MirrorFlipYRightBottomToTop");
    v.push_back("HorizontalGlitch");
    v.push_back("VerticalGlitch");
    v.push_back("HorizontalXor");
    v.push_back("VerticalXor");
    v.push_back("Vertical_Horizontal_Glitch");
    v.push_back("HorizontalSplitGlitch");
    v.push_back("VerticalSplitGlitch");
    v.push_back("HorizontalRandomLine");
    v.push_back("VerticalRandomLine");
    v.push_back("PixelInterlace");
    v.push_back("PixelInterlaceColSkip");
    v.push_back("PixelInterlaceRowSkip");
    v.push_back("StartOffsetInterlace");
    v.push_back("WaveTrails");
    v.push_back("WaveTrailsAura");
    v.push_back("ImageInterlaceWave");
    v.push_back("ColorWaveTrails");
    v.push_back("StrobePixelTrails");
    v.push_back("RectangleTrails");
    v.push_back("RectangleXY_Trails");
    v.push_back("MedianBlendImage");
    v.push_back("GhostWaveTrails");
    v.push_back("VideoPixelFade");
    v.push_back("ImagePixelFade");
    v.push_back("CollectionPixelFade");
    v.push_back("CollectionPixelFadeSubFilter");
    v.push_back("DiamondWave");
    v.push_back("RGBWave");
    v.push_back("VideoCollectionOutline");
    v.push_back("VideoSaturateAdd");
    v.push_back("VideoSmoothMedianBlend");
    v.push_back("Square_Blocks");
    v.push_back("Square_Block_Resize");
    v.push_back("Square_Block_Resize_Vertical");
    v.push_back("Square_Block_Resize_Reset");
    v.push_back("Square_Block_Resize_Vert_Reset");
    v.push_back("Square_Block_Resize_Video");
    v.push_back("Square_Block_Resize_Vert_Video");
    v.push_back("Square_Block_Resize_Dir");
    v.push_back("Square_Block_Resize_All");
    v.push_back("VideoBlendGradient");
    v.push_back("VideoMatrixBlend");
    v.push_back("VideoMatrixBlendAlpha");
    v.push_back("VideoMatrixBlendAlphaRandom");
    v.push_back("VideoMatrixSwitch");
    v.push_back("VideoCollectionWave");
    v.push_back("CollectionWave");
    v.push_back("TremorShake");
    v.push_back("PixelateSquares");
    v.push_back("ColorPixelDoubleXor");
    v.push_back("VideoCollectionOffsetBlend");
    v.push_back("VideoTransparent");
    v.push_back("VideoStripes");
    v.push_back("VideoSmoothGradient");
    v.push_back("SmoothSelfBlend");
    v.push_back("MedianBlendMultiThread_Half");
    v.push_back("MedianBlendMultiThread_Variable");
    v.push_back("FiftyPercentSubFilter");
    v.push_back("VariablePercentSubFilter");
    v.push_back("TwentyFivePercentSubFilter");
    v.push_back("SeventyFivePercentSubFilter");
    v.push_back("VideoRowGlitch");
    v.push_back("VideoXor_Frame");
    v.push_back("VideoSlideRGB");
    v.push_back("VideoSlide");
    v.push_back("VideoSlideOffset");
    v.push_back("VideoSlideOffsetX");
    v.push_back("VideoSlideOffsetXRev");
    v.push_back("VideoSlideOffsetRGB");
    v.push_back("VideoSlideOffsetRGBRev");
    v.push_back("SelfSlideOffsetX");
    v.push_back("SelfSlideOffsetRGB");
    v.push_back("SelfSlideOffsetRGB_X");
    v.push_back("MedianBlendMultiThreadStrobe");
    v.push_back("VideoXor_XY");
    v.push_back("VideoRandomWave");
    v.push_back("RandomWave");
    v.push_back("RandomWaveStartStop");
    v.push_back("RandomMedianFrame");
    v.push_back("VideoSmoothDoubleAlphaBlend");
    v.push_back("VariableLines");
    v.push_back("VariableLinesOffset");
    v.push_back("VaraibleLinesRectangle");
    v.push_back("VariableLinesRectangleSource");
    v.push_back("VariableLinesStartRectangle");
    v.push_back("VariableLinesStartRectangleBlend");
    v.push_back("VariableLinesOnOffBlend");
    v.push_back("VariableCollectionLinesOffBlend");
    v.push_back("VariableDistortionWave");
    v.push_back("VariableDistortionReset");
    v.push_back("VariableLinesY");
    v.push_back("VariableLinesY_Blend");
    v.push_back("VariableLinesY_Wave");
    v.push_back("VariableLinesXY_Interlaced");
    v.push_back("VariableLinesExpand");
    v.push_back("VariableLinesExpandBlend");
    v.push_back("CollectionXor4");
    v.push_back("PixelateExpandDistort");
    v.push_back("PixelateExpandDistortX");
    v.push_back("PixelateExpandDistortY");
    v.push_back("PixelateExpandDistortExtra");
    v.push_back("RectangleSpin");
    v.push_back("RectanglePlotXY");
    v.push_back("RectangleLines");
    v.push_back("DifferenceFillLinesXor");
    v.push_back("DistortPixelate");
    v.push_back("DistortPixelate64_Slow");
    v.push_back("DistortPixelate128_SubFilter");
    v.push_back("DifferenceFillLines_SubFilter");
    v.push_back("DifferenceFillLinesBlend_SubFilter");
    v.push_back("DistortPixelate24_SubFilter");
    v.push_back("MovementTrails_SubFilter");
    v.push_back("MovementTrailsX_SubFilter");
    v.push_back("FadeFromFrameToFrame");
    v.push_back("GlitchFadeFromFrameToFrame");
    v.push_back("RandomSolo");
    v.push_back("PiecesOfFrames");
    v.push_back("XorScaleValue");
    v.push_back("DiamondCollection");
    v.push_back("RandomFadeDelay");
    v.push_back("ColorRowShift");
    v.push_back("ColorRowShiftUp");
    v.push_back("ColorRowShiftLeft");
    v.push_back("ColorRowShiftRight");
    v.push_back("MirrorFadeDelay");
    v.push_back("MirrorRandomNow");
    v.push_back("ScanlineBlack");
    v.push_back("ScanlineSubFilter");
    v.push_back("VerticalYSubFilter");
    v.push_back("ScanlineLessSubFilter");
    v.push_back("VerticalYLessSubFilter");
    v.push_back("HorizontalColorOffset");
    v.push_back("HorizontalColorOffsetLargeSizeSubFilter");
    v.push_back("ErodeKernelSubFilter");
    v.push_back("DilateKernelSubFilter");
    v.push_back("ErodeKernelOffSubFilter");
    v.push_back("DilateKernelSubFilter");
    v.push_back("ColorKeySetOnSubFilter");
    v.push_back("ColorKeySetOffSubFilter");
    v.push_back("Warp");
    v.push_back("WarpDir");
    v.push_back("WarpTopLeft");
    v.push_back("WarpRandom");
    v.push_back("WarpStretch");
    v.push_back("RandomLineGlitchSubFilter");
    v.push_back("VerticalColorOffsetLargeSizeSubFilter");
    v.push_back("PreviousFrameXor");
    v.push_back("LightScanlineSubFilter");
    v.push_back("ScaleUpDown");
    v.push_back("VideoTransitionInOut_SubFilter");
    v.push_back("VideoDisplayPercent_SubFilter");
    v.push_back("MovementToBlackSubFilter");
    v.push_back("Black");
    v.push_back("VideoFrameRGB");
    v.push_back("VideoAlphaBlendFade");
    v.push_back("VideoAlphaAddFade");
    v.push_back("VideoAlphaAddFadeSubFilter");
    v.push_back("DelayOnOffSubFilter");
    v.push_back("VideoImageBlendAlpha");
    v.push_back("IntertwineCols640");
    v.push_back("HorizontalOffsetLess_SubFilter");
    v.push_back("VerticalOffsetLess_SubFilter");
    v.push_back("SquareOffsetLess_SubFilter");
    v.push_back("SquareOffset_SubFilter");
    v.push_back("PrevFrameNotEqual");
    v.push_back("MirrorDelayLeft");
    v.push_back("MirrorDelayRight");
    v.push_back("ApplyColorRange");
    v.push_back("IntertwineAlphaBlend");
    v.push_back("BlackLines");
    v.push_back("MedianBlendMultiThread2");
    v.push_back("MedianBlendMultiThread2_ColorChange");
    v.push_back("ApplyColorRangeInverted");
    v.push_back("VariablesExtraHorizontal");
    v.push_back("ChannelSortDelay");
    v.push_back("DizzyMode");
    v.push_back("IntertwineColormap");
    v.push_back("RotateFrame");
    v.push_back("GhostShift");
    v.push_back("RotateSet");
    v.push_back("RotateFrameReverse");
    v.push_back("RotateSetReverse");
    v.push_back("AuraGhostRotate");
    v.push_back("AuraGhostRotateReverse");
    v.push_back("AuraGhostCollectionEnergy");
    v.push_back("BlendSourceImageAndVideo");
    v.push_back("SetImageAndVideoBlend");
    v.push_back("ThresholdDark");
    v.push_back("ThresholdMedianBlend");
    v.push_back("Threshold");
    v.push_back("RemoveLowRedLevel");
    v.push_back("RemoveLowGreenLevel");
    v.push_back("RemoveLowBlueLevel");
    v.push_back("IncreaseLowRedLevel");
    v.push_back("IncreaseLowGreenLevel");
    v.push_back("IncreaseLowBlueLevel");
    v.push_back("Zoom");
    v.push_back("IntertwineVideo640");
    v.push_back("IntertwineCols640x8");
    v.push_back("ZoomCorner");
    v.push_back("ZoomRandom");
    v.push_back("MedianBlendByLowValue");
    v.push_back("MedianBlendByIncreasingValue");
    v.push_back("AuraZoomMedianBlend");
    v.push_back("OffTrackX");
    v.push_back("OffTrackY");
    v.push_back("FrameAlphaInsert");
    v.push_back("FrameAlphaInsert_SubFilter");
    v.push_back("FrameAlphaBlend_SubFilter");
    v.push_back("SlowTrails_SubFilter");
    v.push_back("AddCollectionSubFilter");
    v.push_back("AddCollectionXor_SubFilter");
    v.push_back("ProperTrails");
    v.push_back("ProperTrails_SubFilter");
    v.push_back("StuckFrame_SubFilter");
    v.push_back("XorLag");
    v.push_back("PixelateBlend");
    v.push_back("PixelateRect");
    v.push_back("RGBSplitFilter");
    v.push_back("DiagPixel");
    v.push_back("DiagPixelY");
    v.push_back("DiagPixelY2");
    v.push_back("DiagPixelY3");
    v.push_back("DiagPixelY4");
    v.push_back("ExpandLeftRight");
    v.push_back("DiagSquare");
    v.push_back("DiagSquareLarge");
    v.push_back("DiagonalSquareCollection");
    v.push_back("DiagnoalSquareSize");
    v.push_back("DiagonalSquareSizeOnOff");
    v.push_back("DiagonalSquareSizeOnOffRandom");
    v.push_back("SlitScanGUI");
    v.push_back("SlitScanRandom");
    v.push_back("VideoInterlacedRandom");
    v.push_back("VideoSlitScan");
    v.push_back("FloatFade");
    v.push_back("SquareShift");
    v.push_back("SquareShift8");
    v.push_back("SquareShift16");
    v.push_back("FloatFadeRandomIncrease");
    v.push_back("FloatFadeRGB");
    v.push_back("SquareShiftDir");
    v.push_back("SquareShiftExpand");
    v.push_back("FloatFadeVertical");
    v.push_back("LineTrails");
    v.push_back("SquareShiftDirVertical");
    v.push_back("StretchLineRow");
    v.push_back("StretchLineCol");
    v.push_back("StretchLineRowInc");
    v.push_back("StretchLineColInc");
    v.push_back("StretchRowSplit");
    v.push_back("VideoAddBlend");
    v.push_back("VideoFadeInAndOut");
    v.push_back("VideoFadeRGB");
    v.push_back("VideoSubtract");
    v.push_back("StretchLineRowLeftRight");
    v.push_back("FadeInOutBlendRGB");
    v.push_back("FadeInOutReverseRGB");
    v.push_back("PixelateRandom");
    v.push_back("PixelateFillRandom");
    v.push_back("JaggedLine");
    v.push_back("ScratchyTrails");
    v.push_back("ExpandPixelate");
    v.push_back("VideoRandom");
    v.push_back("VideoCollectionTwitch");
    v.push_back("DiagSquare8");
    v.push_back("DiagSquareRandom");
    v.push_back("SlitScanXGUI");
    v.push_back("DiagSquareX");
    v.push_back("SlitScanDir");
    v.push_back("VideoScanline");
    v.push_back("VideoSquareRandom");
    v.push_back("VideoScanlineBlend");
    v.push_back("VideoSplitColumn");
    v.push_back("VideoXStatic");
    v.push_back("ScanlineCollection");
    v.push_back("SquareShiftDirRGB");
    v.push_back("StretchLineRowIncRGB");
    v.push_back("StretchLineColIncRGB");
    v.push_back("StretchLineRowIncSource");
    v.push_back("StretchLineColIncSource");
    v.push_back("AlternateAlpha");
    v.push_back("Square_Block_Resize_Vertical_RGB");
    v.push_back("DiagSquareRGB");
    v.push_back("ShiftPixelsRGB");
    v.push_back("UseOldRowVertRGB");
    v.push_back("ColorWaveTrailsRGB");
    v.push_back("MirrorDiamondRGB");
    v.push_back("Square_Block_Resize_RGB");
    v.push_back("VariableLinesY_RGB");
    v.push_back("MedianBlendMultiThread_RGB");
    v.push_back("SlitScanXGUI_RGB");
    v.push_back("SlitScanGUI_RGB");
    v.push_back("SlitScanDir_RGB");
    v.push_back("SquareShiftDirGradient");
    v.push_back("MovementTrailsXRGB_SubFilter");
    v.push_back("TwistedVision_RGB");
    v.push_back("CollectionWave_RGB");
    v.push_back("BlendWithSourcePercent");
    v.push_back("SlitScan_Colors");
    v.push_back("ReverseRandom");
    v.push_back("ReverseRandomCollection");
    v.push_back("RevesreRandomChannelCollection");
    v.push_back("ReverseRandomCollectionResize");
    v.push_back("SquareBlockGlitch");
    v.push_back("SquareStretchRows");
    v.push_back("SquareStretchRowsDelay");
    v.push_back("SquareStretchEven");
    v.push_back("SketchFilter");
    v.push_back("SquareStretchEvenCollection");
    v.push_back("SquareStretchEven32");
    v.push_back("RGBLineFuzz");
    v.push_back("RGBLineFuzzX");
    v.push_back("ChannelSortCollection32");
    v.push_back("LinesAcrossX");
    v.push_back("XorLineX");
    v.push_back("AlphaComponentIncrease");
    v.push_back("ExpandContract");
    v.push_back("MoveInThenMoveOut");
    v.push_back("MoveInThenMoveOutFast");
    v.push_back("CurrentDesktop");
    v.push_back("DistortionFuzz");
    v.push_back("DistortionByRow");
    v.push_back("DistortionByRowRev");
    v.push_back("DistortionByRowVar");
    v.push_back("DistortionByRowRand");
    v.push_back("DistortionByCol");
    v.push_back("DistortionByColRand");
    v.push_back("DistortionByColVar");
    v.push_back("LongLines");
    v.push_back("TearRight");
    v.push_back("CurrentDesktopRect");
    v.push_back("TearDown");
    v.push_back("TearUp");
    v.push_back("TearLeft");
    v.push_back("SyphonInputVideo");
    v.push_back("DistortStretch");
    v.push_back("MultiVideoBlend");
    v.push_back("MultiVideoAlphaBlend");
    v.push_back("MultiVideoSmooth");
    v.push_back("MultiVideoXor");
    v.push_back("MultiVideoFilter_SubFilter");
    v.push_back("FadeOnOff");
    v.push_back("Stereo");
    v.push_back("MultiVideoSaturateBlend");
    v.push_back("ImageWithKeys");
    v.push_back("MultiVideoColorKeyOn");
    v.push_back("MultiVideoColorKeyOff");
    v.push_back("MultiVideoMedianBlend");
    v.push_back("MultiVideoMirror");
    v.push_back("MultiVideoSubtract");
    v.push_back("MultiVideoAlphaMedianBlend");
    v.push_back("MultiVideoInterlace");
    v.push_back("MultiVideoChangeLines");
    v.push_back("MultiVideoDiagSquare");
    v.push_back("ShiftLinesDown");
    v.push_back("PictureStretch");
    v.push_back("PictureStretchPieces");
    v.push_back("VisualSnow");
    v.push_back("VisualSnowX2");
    v.push_back("LineGlitch");
    v.push_back("SlitReverse64");
    v.push_back("SlitReverse64_Increase");
    v.push_back("SlitStretch");
    v.push_back("LineLeftRight");
    v.push_back("LineLeftRightResize");
    v.push_back("SoloInOrder");
    v.push_back("ImageInOrder");
    v.push_back("SubInOrder");
    v.push_back("RGBLineTrails");
    v.push_back("RGBCollectionBlend");
    v.push_back("RGBCollectionIncrease");
    v.push_back("RGBCollectionEx");
    v.push_back("RGBLongTrails");
    v.push_back("VideoMatrixOutlineSubFilter");
    v.push_back("FadeRGB_Speed");
    v.push_back("RGBStrobeTrails");
    v.push_back("FadeRGB_Variable");
    v.push_back("BoxGlitch");
    v.push_back("VerticalPictureDistort");
    v.push_back("ShortTrail");
    v.push_back("DiagInward");
    v.push_back("DiagSquareInward");
    v.push_back("DiagSquareInwardResize");
    v.push_back("DiagSquareInwardResizeXY");
    v.push_back("ParticleSlide");
    v.push_back("DiagPixelated");
    v.push_back("DiagPixelatedResize");
    v.push_back("DiagPixelRGB_Collection");
    v.push_back("RGBShiftTrails");
    v.push_back("PictureShiftDown");
    v.push_back("PictureShiftRight");
    v.push_back("PictureShiftDownRight");
    v.push_back("FlipPictureShift");
    v.push_back("FlipPictureRandomMirror");
    v.push_back("PictureShiftVariable");
    v.push_back("RGBWideTrails");
    v.push_back("StretchR_Right");
    v.push_back("StretchG_Right");
    v.push_back("StretchB_Right");
    v.push_back("StretchR_Down");
    v.push_back("StretchG_Down");
    v.push_back("StretchB_Down");
    v.push_back("Distorted_LinesY");
    v.push_back("Distorted_LinesX");
    v.push_back("TripHSV");
    v.push_back("Diag_Line_InOut");
    v.push_back("Histogram");
    v.push_back("ImageHistogramLookup");
    v.push_back("MovementRange_SubFilter");
    v.push_back("XorSumStrobe");
    v.push_back("DetectEdges");
    v.push_back("SobelNorm");
    v.push_back("SobelThreshold");
    v.push_back("EdgeFill_SubFilter");
    v.push_back("FillWhite_SubFilter");
    v.push_back("FillBlack_SubFilter");
    v.push_back("FillPink_SubFilter");
    v.push_back("FillGreen_SubFilter");
    v.push_back("MedianBlurHigherLevel");
    v.push_back("FillNonWhite_SubFilter");
    v.push_back("FillNonBlack_SubFilter");
    v.push_back("FillNonPink_SubFilter");
    v.push_back("LineInLineOut");
    v.push_back("LineInLineOut_Increase");
    v.push_back("LineInLineOut2_Increase");
    v.push_back("LineInLineOut3_Increase");
    v.push_back("LineInLineOut4_Increase");
    v.push_back("LineInLineOut_ReverseIncrease");
    v.push_back("LineInLineOut_InvertedY");
    v.push_back("LineInLineOut_ReverseInvertedY");
    v.push_back("LineInLineOut_Vertical");
    v.push_back("LineInLineOut_VerticalIncrease");
    v.push_back("LineInLineOut_IncreaseImage");
    v.push_back("LineInLineOut_IncreaseVideo");
    v.push_back("LineInLineOut_IncreaseVideo2");
    v.push_back("LineInLineOut_IncreaseVideo3");
    v.push_back("LineInLineOut_IncreaseImage2");
    v.push_back("LineInLineOut_IncreaseImage3");
    v.push_back("SquareByRow");
    v.push_back("SquareByRowRev");
    v.push_back("SquareByRow2");
    v.push_back("DivideByValue");
    v.push_back("No Filter");
    v.push_back("Blend with Source");
    v.push_back("Plugin");
    v.push_back("Custom");
    v.push_back("Dummy_Filler");
    v.push_back("Dummy_Filler");
    v.push_back("Dummy_Filler");
    v.push_back("Dummy_Filler");
    return v;
}


bool ac::checkFilter(std::string name) {
    auto pos = user_filter.find(name);
    if(pos == user_filter.end())
        return true;
    FileT &ft = pos->second.custom_filter;
    for(int i = 0; i < ft.name.size(); i++) {
        std::cout << ft.name[i] << ":" << name << "\n";
        if(ft.name[i] == name || (ft.name[i].find("User_") != std::string::npos && checkFilter(ft.name[i]) == false)) {
            return false;
        }
    }
    return true;
}


bool ac::LoadFilterFile(std::string fname, std::string filen, int &plugin) {
    std::cout << "attempting to load: " << fname << ":" << filen << "\n";
    static int sort_index = 0;
    ac::FileT typev;
    std::fstream file;
    file.open(filen, std::ios::in);
    if(!file.is_open()) {
        std::cout << "LoadFilterFile: Error could not load file: " << filen << "\n";
        return false;
    }
    plugin = 0;
    std::vector<std::string> values;
    while(!file.eof()) {
        std::string item;
        std::getline(file, item);
        if(file)
            values.push_back(item);
    }
  
     // check if data valid
    for(int i = 0; i < values.size(); ++i ){
        std::string item = values[i];
        std::string s_left, s_right;
        
        if(item[0] == '*') {
            plugin = 1;
            continue;
        }
        
        auto pos = item.find(":");
        if(pos == std::string::npos) {
            return false;
        }
        if(item[0] == '=')
            continue;
        
        s_left = item.substr(0,pos);
        s_right = item.substr(pos+1, item.length());
        if(ac::filter_map.find(s_left) == ac::filter_map.end()) {
            return false;
        }
    }
    for(int i = 0; i < values.size(); ++i) {
        std::string item = values[i];
        if(item[0] == '*') {
            plugin = 1;
            continue;
        }
        std::string s_left, s_right;
        s_left = item.substr(0, item.find(":"));
        s_right = item.substr(item.find(":")+1, item.length());
        std::string val1 = s_left;
        std::string val2;
        val2 = s_right;
        if(item[0] == '=') {
            std::string item = values[i];
            std::string s_left, s_right;
            s_left = item.substr(0, item.find(":"));
            s_right = item.substr(item.find(":")+1, item.length());
            typev.options[s_left] = s_right;
        } else {
            typev.name.push_back(val1);
            typev.subname.push_back(val2);
            typev.filter_on.push_back(1);
            std::cout << "added: " << val1 << ":" << val2 << "\n";
        }
    }
    std::cout << "Loaded file: " << filen << "\n";
    user_filter[fname].custom_filter = typev;
    int found = -1;
    for(int i = 0; i < draw_strings.size(); ++i) {
        if(draw_strings[i] == fname) {
            found = i;
            break;
        }
    }
    if(found == -1) {
        draw_strings.push_back(fname);
        user_filter[fname].name = fname;
        user_filter[fname].index = static_cast<int>(draw_strings.size()-1);
        ac::filter_map[fname] = static_cast<int>(ac::draw_strings.size()-1);
    } else {
        user_filter[fname].index = found;
        ac::filter_map[fname] = found;
    }
    user_filter[fname].sort_num = ++sort_index;
    user_filter[fname].other_name = fname;
    std::cout << "Successfully loaded file with: " << typev.name.size() << " filters.\n";
    return true;
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
    cv::Vec3b &cur = pixelAt(frame,y,x);
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
    cv::Vec3b &cur = pixelAt(frame,y,x);// cur pixel
    cur[0] = ~cur[0]; // bit manipulation sets opposite
    cur[1] = ~cur[1];
    cur[2] = ~cur[2];
}

//
//enum PROC_MODE_TYPE { MOVEINOUT_INC, MOVEINOUT, MOVERESET };

// proc position
void ac::procPos(int &direction, double &pos, double &pos_max, const double max_size, double iter) {
    if(alpha_increase != 0) iter = alpha_increase;
    switch(getProcMode()) {
        case MOVEINOUT_INC: { // move in - increase move out movin - increase move out
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
        case MOVEINOUT: { // flat fade in fade out
            if(direction == 1) {
                pos += iter;
                if(pos > max_size) direction = 0;
                
            } else if(direction == 0) {
                pos -= iter;
                if(pos <= 1) direction = 1;
            }
            
        }
            break;
        case MOVERESET: {
            pos += iter;
            if(pos >= pos_max) {
                pos = 1.0;
            }
        }
            break;
    }
    resetAlpha(direction, pos);
}


void ac::setProcMode(ac::PROC_MODE_TYPE value) {
    proc_mode = value;
}

ac::PROC_MODE_TYPE ac::getProcMode() {
    return proc_mode;
}
