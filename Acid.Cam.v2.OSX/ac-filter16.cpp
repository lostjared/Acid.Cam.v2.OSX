/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2019, Jared Bruni
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

void ac::BlurXorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurXorSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        MedianBlur(copy1);
        MedianBlur(copy2);
    }
    CallFilter(subfilter, copy1);
    static double alpha1 = 2.0, alpha2 = 6.0;
    static int dir1 = 0, dir2 = 1;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 6.0, 2.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.1, 6.0, 2.0);
    AlphaXorBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}


void ac::ColorFlashIncrease(cv::Mat &frame) {
    static unsigned int max_value = 2;
    cv::Vec3b value(rand()%max_value, rand()%max_value, rand()%max_value);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MedianBlur(copy1);
    MedianBlur(copy1);
    MedianBlur(copy1);
    DarkenFilter(copy1);
    DarkenFilter(copy1);
    for(int z = 0; z < copy2.rows; ++z) {
        for(int i = 0; i < copy2.cols; ++i) {
            cv::Vec3b &pixel = copy2.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^value[j]^pix[j];
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++max_value;
        if(max_value >= 255)
            dir = 0;
    } else {
        --max_value;
        if(max_value <= 1)
            dir = 1;
    }
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AddInvert(frame);
}

void ac::ScaleFilter(cv::Mat &frame) {
    static double amt = 0.1;
    static int dir = 1;
    PixelScaleAlpha(frame, amt);
    AlphaMovementMaxMin(amt, dir, 0.01, 1.0, 0.1);
}

void ac::NegativeDarkenXor(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static double alpha1 = 1.0;
    static int dir = 1;
    MedianBlur(copy1);
    MedianBlur(copy1);
    DarkenFilter(copy1);
    DarkenFilter(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>((-pix[j])*alpha1);
            }
        }
    }
    AlphaMovementMaxMin(alpha1, dir, 0.1, 6.0, 1.0);
    AddInvert(frame);
}

void ac::ImageXor_SubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageXor_SubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    Negate(copy1);
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = reimage.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pix1[j]^pix2[j]^pixel[j];
        }
    }
    AddInvert(frame);
}

void ac::NegateBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "NegateBlendSubFilter")
        return;
    static double alpha1 = 1.0, alpha2 = 3.0;
    static int dir1 = 1, dir2 = 0;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Negate(copy2);
    CallFilter(subfilter, copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.08, 4.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.08, 4.0, 1.0);
    AddInvert(frame);
}

void ac::StrobeNegatePixel(cv::Mat &frame) {
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = ~pixel[index];
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::StrobeNegateInOut(cv::Mat &frame) {
    static int dir = 1;
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = ~pixel[index];
        }
    }
    if(dir == 1) {
        ++index;
        if(index > 2) {
            dir = 0;
            index = 2;
        }
    } else {
        --index;
        if(index < 0) {
            dir = 1;
            index = 0;
        }
    }
    AddInvert(frame);
}

void ac::ImageStrobeOnOff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static int index = 0;
    if(index == 0) {
        Negate(reimage);
        index = 1;
    } else {
        Negate(copy1);
        index = 0;
    }
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::AlphaStrobeBlend(cv::Mat &frame) {
    static double alpha = 1.0;
    static int index = 0;
    static int dir_v = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
            pixel[index] = ~pixel[index];
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index > 2) {
            dir = 0;
            index = 2;
        }
    } else {
        --index;
        if(index < 0) {
            dir = 1;
            index = 0;
        }
    }
    AlphaMovementMaxMin(alpha, dir_v, 0.1, 3.0, 1.0);
    AddInvert(frame);

}

void ac::CannyRandomPixels(cv::Mat &frame) {
    Canny(frame);
    cv::Vec3b r(100+rand()%155, 100+rand()%155, 100+rand()%155);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
                pixel = r;
            }
        }
    }
    AddInvert(frame);
}

void ac::FrameImageFadeInOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha1 = 1.0;
    static int dir1 = 1;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 3.0, 1.0);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, 0.5);
    AddInvert(frame);
}

void ac::FrameImageFadeInOutDouble(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    static double alpha1 = 1.0, alpha2 = 4.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.05, 4.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 4.0, 1.0);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
}

// todo: change every 1 second
void ac::ChangeEachSecond(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend","ScanAlphaSwitch", "Dual_SelfAlphaRainbow","BlendAlphaXor","SmoothTrailsSelfAlphaBlend","SelfAlphaRGB","XorAlpha","SelfAlphaScale", "SelfScaleAlpha","DarkSelfAlpha", "RGB Shift", "RGB Sep", "SlideRGB", "GradientRGB", "FrameBlendRGB", "MoveRGB", "LineRGB", "PixelRGB", "RGBFlash", "MirrorRGB", "RGBStatic1", "RGBStatic2", "SelfAlphaRGB", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "RGBMirror", "RGBTrails", "RGBTrailsDark", "RGBTrailsAlpha", "RGBTrailsNegativeAlpha", "MovementRGBTrails", "RGBTrailsXor", "ShadeRGB", "Self AlphaBlend", "Self Scale", "ReinterpSelfScale", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","SelfXorScale", "SelfAlphaRGB", "GradientXorSelfScale", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "SelfXorAverage", "SelfAlphaScale", "SelfScaleAlpha", "SelfAlphaScaleBlend", "SelfScaleXorIncrease", "DarkSelfAlpha", "ShuffleColorMap"};
    static int index = 0;
    static unsigned int frame_counter = 0;
    if(frame_counter == 0) {
        std::sort(filter_array.begin(), filter_array.end());
    }
    unsigned int frames_per_second = static_cast<unsigned int>(ac::fps);
    ++frame_counter;
    if((frame_counter%frames_per_second)==0) {
        ++index;
        if(index > filter_array.size())
            index = 0;
    }
    CallFilter(filter_array[index], frame);
    AddInvert(frame);
}

void ac::ShuffleImage(cv::Mat &frame) {
	static std::vector<std::string> filter_array { "ImageXor", "ImageAlphaBlend", "ImageInter", "ImageX", "SmoothRandomImageBlend", "SmoothImageAlphaBlend", "BlendImageOnOff", "XorSelfAlphaImage", "ImageShiftUpLeft", "AlphaBlendImageXor", "BlendImageXor","BlendImageAround_Median","ImageBlendTransform","ImageXorAlpha","ImageAverageXor","DarkImageMedianBlend","MirrorAlphaBlendedImage","AlphaBlendXorImage","ImageXorFrame","ImageXorFunction","ImageXorAlphaBlend","ImageXorMirrorFilter","ImageSubtractMedianBlend","ImageDarkBlend","ImageAverageDark","ImageRemainderPixel","ParticleReleaseWithImage","ImageEnergy","ImageDistortion","SmoothExactImageXorAlpha","SmoothImageAlphaBlendMedian","ImageDarkenSmoothMedian","XorReverseImageSmooth","ImageSmoothMedianBlend","ImageAlphaXorMedianBlend","RotateImage","RotateBlendImage","RotateAlphaBlendImage","ImageXorScale","MatrixCollectionBlurImageXorAlpha", "BlurImageAlphaBlend", "MatrixCollectionSurroundingPixelsImage", "ImageTransparent", "ImageAlphaCollectionSmoothBlend", "ImageRandomColormap", "ImageRandomColormapAlphaBlend", "ImageRandomColormapAlphaScale", "ImageShuffle", "ImageFadeInOut", "ImageFadeBlackInOut", "ImageFadeFrameInOut", "ImageFadeDouble", "ImageStrobeOnOff", "FrameImageFadeInOut", "FrameImageFadeInOutDouble"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::ChangeImageEachSecond(cv::Mat &frame) {
    static std::vector<std::string> filter_array { "ImageXor", "ImageAlphaBlend", "ImageInter", "ImageX", "SmoothRandomImageBlend", "SmoothImageAlphaBlend", "BlendImageOnOff", "XorSelfAlphaImage", "ImageShiftUpLeft", "AlphaBlendImageXor", "BlendImageXor","BlendImageAround_Median","ImageBlendTransform","ImageXorAlpha","ImageAverageXor","DarkImageMedianBlend","MirrorAlphaBlendedImage","AlphaBlendXorImage","ImageXorFrame","ImageXorFunction","ImageXorAlphaBlend","ImageXorMirrorFilter","ImageSubtractMedianBlend","ImageDarkBlend","ImageAverageDark","ImageRemainderPixel","ParticleReleaseWithImage","ImageEnergy","ImageDistortion","SmoothExactImageXorAlpha","SmoothImageAlphaBlendMedian","ImageDarkenSmoothMedian","XorReverseImageSmooth","ImageSmoothMedianBlend","ImageAlphaXorMedianBlend","RotateImage","RotateBlendImage","RotateAlphaBlendImage","ImageXorScale","MatrixCollectionBlurImageXorAlpha", "BlurImageAlphaBlend", "MatrixCollectionSurroundingPixelsImage", "ImageTransparent", "ImageAlphaCollectionSmoothBlend", "ImageRandomColormap", "ImageRandomColormapAlphaBlend", "ImageRandomColormapAlphaScale", "ImageShuffle", "ImageFadeInOut", "ImageFadeBlackInOut", "ImageFadeFrameInOut", "ImageFadeDouble", "ImageStrobeOnOff", "FrameImageFadeInOut", "FrameImageFadeInOutDouble"};
    static unsigned int counter = 0;
    int fps_val = static_cast<unsigned int>(ac::fps);
    static int index = 0;
    static auto rng = std::default_random_engine{};
    CallFilter(filter_array[index], frame);
    ++counter;
    if((counter%fps_val) == 0) {
    	++index;
    	if(index > filter_array.size()-1) {
        	index = 0;
        	std::shuffle(filter_array.begin(), filter_array.end(),rng);
    	}
    }
    AddInvert(frame);
}

void ac::ChangeImageFilterOnOff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int index = 0;
    if(index == 0) {
        index = 1;
        ChangeImageEachSecond(frame);
    } else {
        index = 0;
        ChangeEachSecond(frame);
    }
    AddInvert(frame);
}

void ac::ChangeXorEachSecond(cv::Mat &frame) {
    static auto rng = std::default_random_engine{};
    static std::vector<std::string> filter_array {"XorMultiBlend", "XorSine", "TrailsFilterXor","XorAddMul", "SurroundPixelXor", "BlendAlphaXor", "SelfXorScale", "BitwiseXorScale", "XorTrails", "BitwiseXorStrobe", "RandomXorFlash", "SoftXor", "SelfXorBlend", "SelfXorDoubleFlash","XorBackwards", "MatrixXorAnd", "XorAlpha", "SelfXorAverage","AndOrXorStrobe", "AndOrXorStrobeScale","MedianBlurXor", "StaticXorBlend", "XorScale", "PixelReverseXor", "PixelXorBlend", "RainbowXorBlend", "StrobeXor", "SelfScaleXorIncrease","PixelByPixelXor", "CopyXorAlpha", "AveragePixelsXor","StrobeXorAndOr"};
    static unsigned int counter = 0;
    
    if(counter == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    
    int fps_val = static_cast<unsigned int>(ac::fps);
    static int index = 0;
    CallFilter(filter_array[index], frame);
    ++counter;
    if((counter%fps_val) == 0) {
        ++index;
        if(index > filter_array.size()-1) {
            index = 0;
            std::shuffle(filter_array.begin(), filter_array.end(),rng);
        }
    }
    AddInvert(frame);
}

void ac::MorphXor(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    ChangeXorEachSecond(frame);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MorphXorWithSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MorphXorWithSubFilter")
        return;
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    ChangeXorEachSecond(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorEachSecond(cv::Mat &frame) {
    static auto rng = std::default_random_engine{};
    static std::vector<std::string> filter_array {"MirrorBlend", "Sideways Mirror", "Mirror No Blend", "Mirror Average", "Mirror Average Mix", "Soft_Mirror", "InterMirror", "InterFullMirror", "MirrorRGB", "RGBMirror", "MirrorStrobe","MirrorXor", "MirrorXorAll", "MirrorXorScale", "EnergyMirror", "MirrorXorAlpha", "IntertwinedMirror", "BlurredMirror", "MirrorMedianBlend", "FlipMirror", "FlipMirrorAverage","ReverseMirrorX", "MirrorXorAll_Reverse", "MirrorRGBReverse", "MirrorRGBReverseBlend", "MirrorBitwiseXor", "BlurMirrorGamma", "EnergyMirrorDark", "AlphaBlendMirror", "TwistedMirror", "MirrorMedian","SmoothMirrorBlurFlip", "MirrorOrder", "BlurMirrorOrder", "MirrorOrderAlpha", "SoftFeedbackMirror", "MirrorAlphaBlend","MirrorBlendFrame", "MirrorBlendVertical", "MirrorVerticalAndHorizontal", "MirrorSidesMedian"};
    static unsigned int counter = 0;
    if(counter == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    int fps_val = static_cast<unsigned int>(ac::fps);
    static int index = 0;
    CallFilter(filter_array[index], frame);
    ++counter;
    if((counter%fps_val) == 0) {
        ++index;
        if(index > filter_array.size()-1) {
            index = 0;
            std::shuffle(filter_array.begin(), filter_array.end(),rng);
        }
    }
    AddInvert(frame);
}

void ac::MirrorReverseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorReverseSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2;
    cv::flip(frame, copy2, 0);
    CallFilter(subfilter, copy1);
    MirrorEachSecond(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorReverseSubFilterAlphaBlend(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorReverseSubFilter")
        return;
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2;
    cv::flip(frame, copy2, 0);
    CallFilter(subfilter, copy1);
    MirrorEachSecond(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.05, 3.0, 1.0);
    AlphaBlend(copy1, copy2, frame, alpha);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::Mirror_Xor_Combined(cv::Mat &frame) {
    DarkenFilter(frame);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static MatrixCollection<32> collection;
    MirrorEachSecond(copy1);
    ChangeXorEachSecond(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.05, 3.0, 1.0);
    AlphaBlend(copy1, copy2, frame, alpha);
    Smooth(frame, &collection);
    BlendWithSource(frame);
    BlendWithSource(frame);
    BlendWithSource(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorFrameShuffle(cv::Mat &frame) {
    static auto rng = std::default_random_engine{};
    static std::vector<std::string> filter_array {"MirrorBlend", "Sideways Mirror", "Mirror No Blend", "Mirror Average", "Mirror Average Mix", "Soft_Mirror", "InterMirror", "InterFullMirror", "MirrorRGB", "RGBMirror", "MirrorStrobe","MirrorXor", "MirrorXorAll", "MirrorXorScale", "EnergyMirror", "MirrorXorAlpha", "IntertwinedMirror", "BlurredMirror", "MirrorMedianBlend", "FlipMirror", "FlipMirrorAverage","ReverseMirrorX", "MirrorXorAll_Reverse", "MirrorRGBReverse", "MirrorRGBReverseBlend", "MirrorBitwiseXor", "BlurMirrorGamma", "EnergyMirrorDark", "AlphaBlendMirror", "TwistedMirror", "MirrorMedian","SmoothMirrorBlurFlip", "MirrorOrder", "BlurMirrorOrder", "MirrorOrderAlpha", "SoftFeedbackMirror", "MirrorAlphaBlend","MirrorBlendFrame", "MirrorBlendVertical", "MirrorVerticalAndHorizontal", "MirrorSidesMedian"};
    static int init = 0;
    if(init == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(), rng);
        init = 1;
    }    static int index = 0;
    CallFilter(filter_array[index], frame);
    ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    AddInvert(frame);
}

void ac::MirrorShuffleSmooth(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    MirrorFrameShuffle(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::Mirror_Xor_Smooth(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    MirrorFrameShuffle(copy1);
    XorFrameShuffle(copy2);
    collection.shiftFrames(copy1);
    collection.shiftFrames(copy2);
    Smooth(frame, &collection, false);
    AlphaMovementMaxMin(alpha, dir, 0.005, 4.0, 1.0);
    AddInvert(frame);

}

void ac::XorFrameShuffle(cv::Mat &frame) {
    static auto rng = std::default_random_engine{};
    static std::vector<std::string> filter_array {"XorMultiBlend", "XorSine", "TrailsFilterXor","XorAddMul", "SurroundPixelXor", "BlendAlphaXor", "SelfXorScale", "BitwiseXorScale", "XorTrails", "SoftXor", "SelfXorBlend","XorBackwards", "MatrixXorAnd", "XorAlpha", "SelfXorAverage","AndOrXorStrobe", "AndOrXorStrobeScale","MedianBlurXor", "StaticXorBlend", "XorScale", "PixelReverseXor", "PixelXorBlend", "RainbowXorBlend", "StrobeXor", "SelfScaleXorIncrease","CopyXorAlpha","AveragePixelsXor","StrobeXorAndOr"};
    static unsigned int index = 0;
    static int init = 0;
    if(init == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(), rng);
        init = 1;
    }
    CallFilter(filter_array[index], frame);
    ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    AddInvert(frame);
}

void ac::XorMirrorBlendFrame(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    XorFrameShuffle(copy1);
    MirrorFrameShuffle(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageXorSmooth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ShuffleImage(copy1);
    XorFrameShuffle(copy2);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::SmoothSubFilter64(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothSubFilter64")
        return;
    static MatrixCollection<64> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::SmoothMedian64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothMedian32_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothMedian32_SubFilter")
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothAlphaMedian_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothAlphaMedian_SubFilter")
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    static double alpha1 = 0.5, alpha2 = 3.0;
    static int dir1 = 1, dir2 = 0;
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 3.0, 0.5);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 3.0, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothImage_SubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "SmoothImage_SubFilter")
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    Smooth(copy1, &collection1);
    CallFilter(subfilter, reimage);
    Smooth(reimage, &collection2);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::SmoothImageMedian_SubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "SmoothImageMedian_SubFilter")
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(frame, reimage, frame.size());
    CallFilter(subfilter, reimage);
    SmoothRGB(reimage, &collection1);
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection2);
    AlphaBlend(copy1, reimage, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothImageAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "SmoothImageAndSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3, reimage;
    cv::resize(blend_image, reimage, frame.size());
    static MatrixCollection<16> collection1, collection2, collection3;
    CallFilter(subfilter, reimage);
    CallFilter(subfilter, copy1);
    Smooth(reimage, &collection1);
    Smooth(copy1, &collection2);
    Smooth(copy2, &collection3);
    AlphaBlend(copy1, reimage, copy3, 0.5);
    AlphaBlend(copy3, copy2, frame, 0.5);
}

void ac::SmoothSubFilter90(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothSubFilter90")
        return;
    static MatrixCollection<90> collection;
    CallFilter(subfilter, frame);
    Smooth(frame, &collection);
}
