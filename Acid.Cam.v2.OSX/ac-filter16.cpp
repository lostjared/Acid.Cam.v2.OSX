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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    ac_resize(blend_image, reimage, frame.size());
    Negate(copy1);
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    ac_resize(blend_image, reimage, frame.size());
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    ac_resize(blend_image, reimage, frame.size());
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 3.0, 1.0);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, 0.5);
    AddInvert(frame);
}

void ac::FrameImageFadeInOutDouble(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha1 = 1.0, alpha2 = 4.0;
    static int dir1 = 1, dir2 = 0;
    AlphaMovementMaxMin(alpha1, dir1, 0.05, 4.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 4.0, 1.0);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
}

// todo: change every 1 second
void ac::ChangeEachSecond(cv::Mat &frame) {
    static std::vector<std::string> filter_array {"Self AlphaBlend","ScanAlphaSwitch", "Dual_SelfAlphaRainbow","BlendAlphaXor","SmoothTrailsSelfAlphaBlend","SelfAlphaRGB","XorAlpha","SelfAlphaScale", "SelfScaleAlpha","DarkSelfAlpha", "RGB Shift", "RGB Sep", "SlideRGB", "GradientRGB", "FrameBlendRGB", "MoveRGB", "LineRGB", "PixelRGB", "RGBFlash", "MirrorRGB", "RGBStatic1", "RGBStatic2", "SelfAlphaRGB", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "RGBMirror", "RGBTrails", "RGBTrailsDark", "RGBTrailsAlpha", "RGBTrailsNegativeAlpha", "MovementRGBTrails", "RGBTrailsXor", "ShadeRGB", "Self AlphaBlend", "Self Scale", "ReinterpSelfScale", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur","SelfXorScale", "SelfAlphaRGB", "GradientXorSelfScale", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "SelfXorAverage", "SelfAlphaScale", "SelfScaleAlpha", "SelfAlphaScaleBlend", "SelfScaleXorIncrease", "DarkSelfAlpha", "ShuffleColorMap"};
    static unsigned int index = 0;
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
	static std::vector<std::string> filter_array { "ImageXor", "ImageAlphaBlend", "ImageInter", "ImageX", "SmoothRandomImageBlend", "SmoothImageAlphaBlend", "BlendImageOnOff", "XorSelfAlphaImage", "ImageShiftMatrixLeft", "AlphaBlendImageXor", "BlendImageXor","BlendImageAround_Median","ImageBlendTransform","ImageXorAlpha","ImageAverageXor","DarkImageMedianBlend","MirrorAlphaBlendedImage","AlphaBlendXorImage","ImageXorFrame","ImageXorFunction","ImageXorAlphaBlend","ImageXorMirrorFilter","ImageSubtractMedianBlend","ImageDarkBlend","ImageAverageDark","ImageRemainderPixel","ParticleReleaseWithImage","ImageEnergy","ImageDistortion","SmoothExactImageXorAlpha","SmoothImageAlphaBlendMedian","ImageDarkenSmoothMedian","XorReverseImageSmooth","ImageSmoothMedianBlend","ImageAlphaXorMedianBlend","RotateImage","RotateBlendImage","RotateAlphaBlendImage","ImageXorScale","MatrixCollectionBlurImageXorAlpha", "BlurImageAlphaBlend", "MatrixCollectionSurroundingPixelsImage", "ImageTransparent", "ImageAlphaCollectionSmoothBlend", "ImageRandomColormap", "ImageRandomColormapAlphaBlend", "ImageRandomColormapAlphaScale", "ImageShuffle", "ImageFadeInOut", "ImageFadeBlackInOut", "ImageFadeFrameInOut", "ImageFadeDouble", "ImageStrobeOnOff", "FrameImageFadeInOut", "FrameImageFadeInOutDouble"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::ChangeImageEachSecond(cv::Mat &frame) {
    static std::vector<std::string> filter_array { "ImageXor", "ImageAlphaBlend", "ImageInter", "ImageX", "SmoothRandomImageBlend", "SmoothImageAlphaBlend", "BlendImageOnOff", "XorSelfAlphaImage", "ImageShiftMatrixLeft", "AlphaBlendImageXor", "BlendImageXor","BlendImageAround_Median","ImageBlendTransform","ImageXorAlpha","ImageAverageXor","DarkImageMedianBlend","MirrorAlphaBlendedImage","AlphaBlendXorImage","ImageXorFrame","ImageXorFunction","ImageXorAlphaBlend","ImageXorMirrorFilter","ImageSubtractMedianBlend","ImageDarkBlend","ImageAverageDark","ImageRemainderPixel","ParticleReleaseWithImage","ImageEnergy","ImageDistortion","SmoothExactImageXorAlpha","SmoothImageAlphaBlendMedian","ImageDarkenSmoothMedian","XorReverseImageSmooth","ImageSmoothMedianBlend","ImageAlphaXorMedianBlend","RotateImage","RotateBlendImage","RotateAlphaBlendImage","ImageXorScale","MatrixCollectionBlurImageXorAlpha", "BlurImageAlphaBlend", "MatrixCollectionSurroundingPixelsImage", "ImageTransparent", "ImageAlphaCollectionSmoothBlend", "ImageRandomColormap", "ImageRandomColormapAlphaBlend", "ImageRandomColormapAlphaScale", "ImageShuffle", "ImageFadeInOut", "ImageFadeBlackInOut", "ImageFadeFrameInOut", "ImageFadeDouble", "ImageStrobeOnOff", "FrameImageFadeInOut", "FrameImageFadeInOutDouble"};
    static unsigned int counter = 0;
    int fps_val = static_cast<unsigned int>(ac::fps);
    static int unsigned index = 0;
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
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
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array {"XorMultiBlend", "XorSine", "TrailsFilterXor","XorAddMul", "SurroundPixelXor", "BlendAlphaXor", "SelfXorScale", "BitwiseXorScale", "XorTrails", "BitwiseXorStrobe", "RandomXorFlash", "SoftXor", "SelfXorBlend", "SelfXorDoubleFlash","XorBackwards", "MatrixXorAnd", "XorAlpha", "SelfXorAverage","AndOrXorStrobe", "AndOrXorStrobeScale","MedianBlurXor", "StaticXorBlend", "XorScale", "PixelReverseXor", "PixelXorBlend", "RainbowXorBlend", "StrobeXor", "SelfScaleXorIncrease","PixelByPixelXor", "CopyXorAlpha", "AveragePixelsXor","StrobeXorAndOr"};
    static unsigned int counter = 0;
    
    if(counter == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    
    int fps_val = static_cast<unsigned int>(ac::fps);
    static unsigned int index = 0;
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
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array {"MirrorBlend", "Sideways Mirror", "Mirror No Blend", "Mirror Average", "Mirror Average Mix", "Soft_Mirror", "InterMirror", "InterFullMirror", "MirrorRGB", "RGBMirror", "MirrorStrobe","MirrorXor", "MirrorXorAll", "MirrorXorScale", "EnergyMirror", "MirrorXorAlpha", "IntertwinedMirror", "BlurredMirror", "MirrorMedianBlend", "FlipMirror", "FlipMirrorAverage","ReverseMirrorX", "MirrorXorAll_Reverse", "MirrorRGBReverse", "MirrorRGBReverseBlend", "MirrorBitwiseXor", "BlurMirrorGamma", "EnergyMirrorDark", "AlphaBlendMirror", "TwistedMirror", "MirrorMedian","SmoothMirrorBlurFlip", "MirrorOrder", "BlurMirrorOrder", "MirrorOrderAlpha", "SoftFeedbackMirror", "MirrorAlphaBlend","MirrorBlendFrame", "MirrorBlendVertical", "MirrorVerticalAndHorizontal", "MirrorSidesMedian"};
    static unsigned int counter = 0;
    if(counter == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    int fps_val = static_cast<unsigned int>(ac::fps);
    static unsigned int index = 0;
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
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array {"MirrorBlend", "Sideways Mirror", "Mirror No Blend", "Mirror Average", "Mirror Average Mix", "Soft_Mirror", "InterMirror", "InterFullMirror", "MirrorRGB", "RGBMirror", "MirrorStrobe","MirrorXor", "MirrorXorAll", "MirrorXorScale", "EnergyMirror", "MirrorXorAlpha", "IntertwinedMirror", "BlurredMirror", "MirrorMedianBlend", "FlipMirror", "FlipMirrorAverage","ReverseMirrorX", "MirrorXorAll_Reverse", "MirrorRGBReverse", "MirrorRGBReverseBlend", "MirrorBitwiseXor", "BlurMirrorGamma", "EnergyMirrorDark", "AlphaBlendMirror", "TwistedMirror", "MirrorMedian","SmoothMirrorBlurFlip", "MirrorOrder", "BlurMirrorOrder", "MirrorOrderAlpha", "SoftFeedbackMirror", "MirrorAlphaBlend","MirrorBlendFrame", "MirrorBlendVertical", "MirrorVerticalAndHorizontal", "MirrorSidesMedian"};
    static int init = 0;
    if(init == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(), rng);
        init = 1;
    }
    static unsigned int index = 0;
     ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    CallFilter(filter_array[index], frame);
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
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array {"XorMultiBlend", "XorSine", "TrailsFilterXor","XorAddMul", "SurroundPixelXor", "BlendAlphaXor", "SelfXorScale", "BitwiseXorScale", "XorTrails", "SoftXor", "SelfXorBlend","XorBackwards", "MatrixXorAnd", "XorAlpha", "SelfXorAverage","AndOrXorStrobe", "AndOrXorStrobeScale","MedianBlurXor", "StaticXorBlend", "XorScale", "PixelReverseXor", "PixelXorBlend", "RainbowXorBlend", "StrobeXor", "SelfScaleXorIncrease","CopyXorAlpha","AveragePixelsXor","StrobeXorAndOr"};
    static unsigned int index = 0;
    static int init = 0;
    if(init == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(), rng);
        init = 1;
    }
    ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    CallFilter(filter_array[index], frame);
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
}

void ac::SmoothSubFilter64(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothSubFilter64")
        return;
    static MatrixCollection<64> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection, false);
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
    ac_resize(blend_image, reimage, frame.size());
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
    ac_resize(frame, reimage, frame.size());
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
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<16> collection1, collection2, collection3;
    CallFilter(subfilter, reimage);
    CallFilter(subfilter, copy1);
    Smooth(reimage, &collection1);
    Smooth(copy1, &collection2);
    Smooth(copy2, &collection3);
    AlphaBlend(copy1, reimage, copy3, 0.5);
    AlphaBlend(copy3, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::SmoothSubFilter90(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothSubFilter90")
        return;
    static MatrixCollection<90> collection;
    collection.shiftFrames(frame);
    CallFilter(subfilter, frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::SmoothMedianImageSubFilter16(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "SmoothMedianImageSubFilter16")
        return;
    static MatrixCollection<16> collection1, collection2, collection3;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage, copy3;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, reimage);
    Smooth(copy1, &collection1);
    Smooth(reimage, &collection2);
    AlphaBlend(copy1, reimage, copy3, 0.5);
    CallFilter(subfilter, copy3);
    Smooth(copy3, &collection3);
    AlphaBlend(copy3,copy2,frame, 0.5);
    AddInvert(frame);
}

void ac::ImageNegate(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    Negate(reimage);
    frame = reimage.clone();
    AddInvert(frame);
}

void ac::ImageNegateAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    Negate(reimage);
    static double alpha1 = 3.0, alpha2 = 0.1;
    static int dir1 = 0, dir2 = 1;
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 3.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 3.0, 0.1);
    AddInvert(frame);
}

void ac::ImageNegateAlphaBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageNegateAlphaBlendSubFilter")
        return;
    static MatrixCollection<16> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    Negate(reimage);
    Smooth(reimage, &collection1);
    Smooth(copy1, &collection2);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::FrameNegateAlphaBlendImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    Negate(copy1);
    static double alpha1 = 3.0, alpha2 = 0.1;
    static int dir1 = 0, dir2 = 1;
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 3.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 3.0, 0.1);
    AddInvert(frame);
}

void ac::DarkTrailsEffect(cv::Mat &frame) {
    pushSubFilter(getFilterByName("Rainbow Blend"));
    TrailsSubFilter(frame);
    popSubFilter();
    Negate(frame);
    GammaDarken5(frame);
    AddInvert(frame);
}

void ac::DarkNegate(cv::Mat &frame) {
    Negate(frame);
    GammaDarken5(frame);
    AddInvert(frame);
}

void ac::ChannelSortMedianBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
	ChannelSort(copy1);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionMirrorDirection(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3;
    MirrorFrameShuffle(copy1);
    int r_dir = -1+rand()%2;
    cv::flip(copy1, copy3, r_dir);
    static MatrixCollection<64> collection;
    collection.shiftFrames(copy2);
    Smooth(copy3, &collection);
    AlphaBlend(copy2, copy3, frame, 0.3);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::StrobeRandomChannel(cv::Mat &frame) {
    int res = rand()%3;
    static double alpha = 1.0;
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.1, 6.0, 1.0);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[res] = static_cast<unsigned char>(pixel[res]*alpha);
        }
    }
    AddInvert(frame);
}

void ac::SplitFramesSort(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    std::vector<cv::Mat> v1, v2, v3;
    MirrorXorAll(frame);
    rainbowBlend(collection.frames[2]);
    cv::split(frame, v1);
    cv::split(collection.frames[1], v2);
    cv::split(collection.frames[2], v3);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v1[0], channels[0],cv::SORT_ASCENDING);
    cv::sort(v2[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v3[2], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, output);
    frame = output.clone();
    AddInvert(frame);
}

void ac::SplitFrameSortSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SplitFrameSortSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    std::vector<cv::Mat> v1, v2, v3;
    CallFilter(subfilter, frame);
    cv::split(frame, v1);
    cv::split(collection.frames[1], v2);
    cv::split(collection.frames[2], v3);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v1[0], channels[0],cv::SORT_ASCENDING);
    cv::sort(v2[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v3[2], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, output);
    frame = output.clone();
    AddInvert(frame);
}

void ac::MedianBlend64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::SplitFrameFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    std::vector<cv::Mat> v1, v2, v3;
    ColorExpand(frame);
    rainbowBlend(collection.frames[1]);
    RainbowXorBlend(collection.frames[2]);
    cv::split(frame, v1);
    cv::split(collection.frames[1], v2);
    cv::split(collection.frames[2], v3);
    cv::Mat channels[3];
    cv::Mat output;
    cv::sort(v1[0], channels[0],cv::SORT_ASCENDING);
    cv::sort(v2[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v3[2], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, output);
    frame = output.clone();
    AddInvert(frame);
}

void ac::SplitFrameBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    std::vector<cv::Mat> v1, v2, v3;
    ColorXorScale(frame);
    ColorExpand(copy1);
    rainbowBlend(copy2);
    cv::Mat channels[3];
    cv::extractChannel(frame, channels[0], 0);
    cv::extractChannel(copy1, channels[1], 1);
    cv::extractChannel(copy2, channels[2], 2);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::SplitFrameBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SplitFrameBlendSubFilter")
        return;
    cv::Mat frames[3];
    int r = rand()%3;
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    frames[2] = frame.clone();
    CallFilter(subfilter, frames[r]);
    cv::Mat channels[3];
    cv::Mat output;
    cv::extractChannel(frames[0], channels[0], 0);
    cv::extractChannel(frames[1], channels[1], 1);
    cv::extractChannel(frames[2], channels[2], 2);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::SplitFrameCollection(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = frame.clone();
    frames[1] = collection.frames[6].clone();
    frames[2] = collection.frames[7].clone();
    static int dir = 1;
    if(dir == 1) {
    	RainbowXorBlend(frames[0]);
    	MedianBlend(frames[1]);
    	ColorXorScale(frames[2]);
        dir = 0;
    } else {
        dir = 1;
        ColorXorScale(frames[0]);
        MedianBlend(frames[1]);
        RainbowXorBlend(frames[2]);
    }
    cv::Mat channels[3];
    cv::Mat output;
    cv::extractChannel(frames[0], channels[0], 0);
    cv::extractChannel(frames[1], channels[1], 1);
    cv::extractChannel(frames[2], channels[2], 2);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::SplitFrameMirror(cv::Mat &frame) {
    cv::Mat frames[3];
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    frames[2] = frame.clone();
    MirrorXorAll(frames[0]);
    FlipBlendWH(frames[1]);
    rainbowBlend(frames[2]);
    cv::Mat channels[3];
    cv::Mat output;
    static int dir = 1;
    if(dir == 1) {
        dir = 0;
        cv::extractChannel(frames[0], channels[0], 0);
    	cv::extractChannel(frames[1], channels[1], 1);
        cv::extractChannel(frames[2], channels[2], 2);
    } else {
        dir = 1;
        cv::extractChannel(frames[0], channels[2], 0);
        cv::extractChannel(frames[1], channels[1], 1);
        cv::extractChannel(frames[2], channels[0], 2);
    }
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::RandomChannels(cv::Mat &frame) {
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> selection ({"Self AlphaBlend", "Blend #3", "RandTriBlend", "Rainbow Blend", "Rand Blend", "New Blend", "MirrorBlend", "Mirror No Blend", "Blend_Angle", "XorMultiBlend", "BlendedScanLines", "FrameBlend", "FrameBlendRGB", "PrevFrameBlend", "HorizontalBlend", "VerticalBlend", "OppositeBlend", "BlendSwitch", "IncreaseBlendHorizontal", "BlendIncrease", "TrailsBlend", "BlendThree", "BlendTrails", "GridFilter8xBlend", "WeakBlend", "SmoothTrailsSelfAlphaBlend", "SmoothTrailsRainbowBlend", "MedianBlend", "RandomAlphaBlend", "RandomTwoFilterAlphaBlend", "AlphaBlendPosition", "BlendRowAlpha", "BlendRow", "BlendRowByVar", "BlendRowByDirection", "BlendAlphaXor", "AlphaBlendRandom", "ChannelSortAlphaBlend", "StrobeBlend", "GaussianBlend", "SelfXorBlend", "BlendRowCurvedSqrt", "CycleShiftRandomRGB_XorBlend", "CycleShiftRandomAlphaBlend", "MedianBlendAnimation", "ParticleBlend", "BlendInAndOut", "BlendScaleInAndOut", "RandomXorBlend", "InitBlend", "LagBlend", "RandomMirrorBlend", "RandomMirrorAlphaBlend", "RandBlend", "StaticXorBlend", "Bitwise_XOR_Blend", "Bitwise_OR_Blend", "Bitwise_AND_Blend", "SilverBlend", "PixelXorBlend", "RainbowXorBlend", "FadeBlend", "SelfAlphaScaleBlend", "ReverseFrameBlend", "ReverseFrameBlendSwitch", "RandomBlendFilter", "DoubleRandomBlendFilter", "FlipBlendW", "FlipBlendWH", "FlipBlendH", "FlipBlendAll", "Blend_RedGreenBlue", "XorBlend_RedGreenBlue", "BlendIncrease_RedGreenBlue", "Blend_RedReenBlue_Dark", "DarkModBlend", "MirrorMedianBlend", "DarkenBlend", "ChannelSort_NoBlend_Descending", "ChannelSort_NoBlend_Ascending", "BlendBurred", "BlendCombinedValues", "AlphaBlendWithSource", "RGBMedianBlend", "MirrorRGBReverseBlend", "MedianBlend16", "BGRBlend", "RGBBlend", "MedianBlendDark", "AlphaBlendMirror", "SelfScaleSortBlend", "FlashMedianBlend", "FlipAlphaBlend", "SmoothCollectionAlphaBlend", "SmoothTrailsBlend", "ShuffleAlphaMedianBlend", "AverageLinesBlend", "AverageVerticalLinesBlend", "LinesMedianBlend", "MedianBlendSoft", "ParticleReleaseAlphaBlend", "MirrorAlphaBlend", "MatrixCollectionBlend", "SmoothMedianBlend", "CosSinMedianBlend", "Filter8_Blend", "RandomAlphaBlendFilter", "MirrorBlendFrame", "MirrorBlendVertical", "BlendFor360", "MatrixCollectionXorBlendAlphaBlend", "MedianBlendSelfBlend", "Bitwise_XOR_BlendFrame", "GaussianBlendEx", "SimpleMatrixBlend", "AlphaStrobeBlend", "XorMirrorBlendFrame", "ChannelSortMedianBlend", "MedianBlend64", "SplitFrameBlend"});
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone();
    static unsigned int index = 0;
    if(index == 0) {
        std::shuffle(selection.begin(), selection.end(), rng);
    }
    if(index+3 < selection.size()) {
        CallFilter(selection[index], copy1);
    	CallFilter(selection[index+1], copy2);
    	CallFilter(selection[index+2], copy3);
    }
    index += 3;
    if(index > selection.size())
        index = 0;
    cv::Mat channels[3];
    cv::Mat output;
    cv::extractChannel(copy1, channels[0], 0);
    cv::extractChannel(copy2, channels[1], 1);
    cv::extractChannel(copy3, channels[2], 2);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::SmoothRandomChannels(cv::Mat &frame) {
    static MatrixCollection<16> collection1, collection2, collection3;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone();
    randomFilter(copy1);
    randomFilter(copy2);
    randomFilter(copy3);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    Smooth(copy3, &collection3);
    cv::Mat channels[3];
    cv::Mat output;
    cv::extractChannel(copy1, channels[0], 0);
    cv::extractChannel(copy2, channels[1], 1);
    cv::extractChannel(copy3, channels[2], 2);
    cv::merge(channels, 3, frame);
    AddInvert(frame);
}

void ac::SmoothChannelSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothChannelSubFilter")
        return;
    static MatrixCollection<8> collection;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    cv::Mat copies[3];
    copies[0] = collection.frames[1].clone();
    copies[1] = collection.frames[3].clone();
    copies[2] = collection.frames[6].clone();
    copies[index] = copy1.clone();
    ++index;
    if(index > 2)
        index = 0;
    cv::Mat chan[3];
    cv::Mat output;
    cv::extractChannel(copies[0], chan[0], 0);
    cv::extractChannel(copies[1], chan[1], 1);
    cv::extractChannel(copies[2], chan[2], 2);
    cv::merge(chan, 3, frame);
    AddInvert(frame);
}

void ac::IncreaseRGB(cv::Mat &frame) {
    static int index = 0;
    static int max = 0;
    static int speed = 5;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] += max;
        }
    }
    if(max >= 255) {
        ++index;
        if(index > 2)
            index = 0;
        max = 0;
    } else {
        max += speed;
    }
    AddInvert(frame);
}

void ac::IncreaseColor(cv::Mat &frame) {
    static int index = 0;
    static int max[3] = {0,50,100};
    static int speed = 5;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixel;
            for(int j = 0; j < 3; ++j) {
                pixel[j] += max[j];
            }
            pixel[index] = pix[index];
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    for(int j = 0; j < 3; ++j) {
        if(max[j] >= 255) {
            max[j] = 0;
        } else {
            max[j] += speed;
        }
    }
    AddInvert(frame);
}

void ac::SaturateBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    IncreaseColor(copy1);
    rainbowBlend(copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[2];
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy2.at<cv::Vec3b>(z, i);
            for(int j = 1; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]+pix[0][j]+pix[1][j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::SaturateBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SaturateBlendSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    IncreaseColor(copy1);
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[2];
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy2.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= cv::saturate_cast<unsigned char>((pixel[j]*alpha)+(pix[0][j]*alpha)+(pix[1][j]*alpha));
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 2.5, 1.0);
    AddInvert(frame);
}

// average

void ac::MaxRGB(cv::Mat &frame) {
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = 255;
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::XorDifferenceFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b copy_pix[8];
            for(int q = 0; q < collection.size(); ++q) {
                copy_pix[q] = collection.frames[q].at<cv::Vec3b>(z, i);
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b copy_pixel = pixel;
            for(int j = 0; j < 3; ++j) {
                pixel[j] = copy_pix[0][j] ^ copy_pix[1][j] ^ copy_pix[2][j] ^ copy_pix[3][j] ^ copy_pix[4][j] ^ copy_pix[5][j] ^ copy_pix[6][j] ^ copy_pix[7][j] ^ pixel[j];
            }
            cv::Vec3b lw(100, 100, 100);
            if(colorBounds(pixel,copy_pixel,lw, lw)) {
                //pixel = cv::Vec3b(0,0,0);
            } else {
                switch(index) {
                    case 0:
                        pixel = cv::Vec3b(0,0,255);
                        break;
                    case 1:
                        pixel = cv::Vec3b(0,255,0);
                        break;
                    case 2:
                        pixel = cv::Vec3b(255,0,0);
                        break;
                }
            }
        }
    }
    ++index;
    if(index > 2) index = 0;
    AddInvert(frame);
}

void ac::AlphaBlendChannelSort(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    SplitFrameFilter(copy2);
    MedianBlend(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}


void ac::ColorTrailsFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    SmoothRGB(frame, &collection);
    AddInvert(frame);
}


void ac::ColorTrailsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ColorTrailsSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    SmoothRGB(copy1, &collection);
    MedianBlend(copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}
