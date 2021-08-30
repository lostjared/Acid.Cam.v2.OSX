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


void ac::HalfNegateStrobe(cv::Mat &frame) {
    int f_len = frame.cols/2;
    auto neg = [](cv::Vec3b &pixel) {
        for(int j = 0; j < 3; ++j)
            pixel[j] = ~pixel[j];
    };
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < f_len; ++i) {
            if(off == 0) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                neg(pixel);
            }
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int i = f_len; i < frame.cols; ++i) {
            if(off == 1) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                neg(pixel);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++off;
    if(off >= 2) {
        off = 0;
    }
}

void ac::MedianBlurXor(cv::Mat &frame) {
    cv::Mat copy_f = frame.clone();
    MedianBlend(frame);
    Negate(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b v = copy_f.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ v[j];
            }
        }
    }
    Negate(frame);
    AddInvert(frame);
}

void ac::NegateTrails(cv::Mat &frame) {
    MovementRGBTrails(frame);
    MedianBlur(frame);
    RGBTrailsDark(frame);
    MedianBlur(frame);
    Negate(frame);
    AddInvert(frame);
}

void ac::RandomGradient(cv::Mat &frame) {
    static std::vector<std::string> svGradient { "CosSinMultiply","New Blend","Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8", "Graident Rainbow","Gradient Rainbow Flash","Outward", "Outward Square","GradientLines","GradientSelf","GradientSelfVertical","GradientDown","GraidentHorizontal","GradientRGB","GradientStripes", "GradientReverse", "GradientReverseBox", "GradientReverseVertical", "GradientNewFilter", "AverageLines", "QuadCosSinMultiply", "GradientColors", "GradientColorsVertical", "GradientXorSelfScale", "GradientLeftRight", "GraidentUpDown", "GradientLeftRightInOut", "GradientUpDownInOut"};
    CallFilter(svGradient[rand()%svGradient.size()], frame);
    AddInvert(frame);
}

void ac::RandomStrobeFlash(cv::Mat &frame) {
    static std::vector<std::string> svStrobe{  "StrobeEffect", "Blank", "Type","Random Flash","Strobe Red Then Green Then Blue","StrobeScan", "RGBFlash", "ReinterpretDouble", "BitwiseXorStrobe","StrobeBlend", "FibFlash", "ScaleFlash", "FadeStrobe", "AndStrobe", "AndStrobeScale", "AndPixelStrobe", "AndOrXorStrobe", "AndOrXorStrobeScale", "BrightStrobe", "DarkStrobe", "RandomXorOpposite", "StrobeTransform", "OrStrobe", "DifferenceStrobe", "DifferenceXor", "DifferenceRand"};
    CallFilter(svStrobe[rand()%svStrobe.size()], frame);
    AddInvert(frame);
}

void ac::RandomMirror(cv::Mat &frame) {
    static std::vector<std::string> svMirror { "NewOne", "MirrorBlend", "Sideways Mirror","Mirror No Blend","Mirror Average", "Mirror Average Mix","Reverse","Double Vision","RGB Shift","RGB Sep","Side2Side","Top2Bottom", "Soft_Mirror", "KanapaTrip", "InterReverse", "InterMirror", "InterFullMirror", "MirrorRGB", "LineByLineReverse", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBMirror", "MirrorStrobe"};
    CallFilter(svMirror[rand()%svMirror.size()], frame);
    AddInvert(frame);
}

void ac::RandomOther(cv::Mat &frame) {
    static std::vector<std::string> svOther_Custom { "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND", "Bitwise_OR", "Channel Sort", "Reverse_XOR","Bitwise_Rotate","Bitwise_Rotate Diff", "Equalize","PixelSort", "GlitchSort","HPPD","FuzzyLines","Alpha Flame Filters","Scanlines", "TV Static","FlipTrip", "Canny","Inter","Circular","MoveRed","MoveRGB", "MoveRedGreenBlue", "Wave","HighWave","VerticalSort","VerticalChannelSort","ScanSwitch", "ScanAlphaSwitch","XorAddMul", "RandomIntertwine","RandomFour","RandomTwo","Darken","AverageRandom","RandomCollectionAverage","RandomCollectionAverageMax","BitwiseXorScale","XorChannelSort","Bitwise_XOR_Average","Sort_Vertical_Horizontal","Sort_Vertical_Horizontal_Bitwise_XOR", "Scalar_Average_Multiply","Scalar_Average","Total_Average","VerticalColorBars","inOrder","inOrderBySecond","DarkenFilter","RandomFilterBySecond","ThreeRandom","inOrderAlpha","XorBackwards", "MoveUpLeft", "Stuck", "StuckStrobe", "SoftFeedback", "SoftFeedbackFrames", "ResizeSoftFeedback", "SoftFeedback8","SoftFeedbackFrames8","ResizeSoftFeedback8", "ResizeSoftFeedbackSubFilter", "SoftFeedbackRandFilter", "SoftFeedback32","SoftFeedbackFrames32","ResizeSoftFeedback32", "SoftFeedbackRandFilter32", "SoftFeedbackSubFilter","SoftFeedbackResizeSubFilter", "SoftFeedbackResizeSubFilter64", "SoftFeedbackReszieSubFilter64_Negate", "SoftFeedbackReszieSubFilter64_Mirror"};
    CallFilter(svOther_Custom[rand()%svOther_Custom.size()], frame);
    AddInvert(frame);
}

void ac::RandomXorFilter(cv::Mat &frame) {
    static std::vector<std::string> Xor {"XorMultiBlend", "XorSine", "TrailsFilterXor", "BlockXor", "XorAddMul", "SurroundPixelXor", "BlendAlphaXor", "SelfXorScale","XorTrails","XorChannelSort", "GradientXorSelfScale", "RandomXor", "RandomXorFlash", "SoftXor", "SelfXorBlend", "SelfXorDoubleFlash", "CycleShiftRandomRGB_XorBlend", "inOrderAlphaXor", "SlideFilterXor", "SlideUpDownXor", "XorBackwards", "MatrixXorAnd", "XorAlpha", "SelfXorAverage", "RandomXorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "AndOrXorStrobe", "AndOrXorStrobeScale", "RandomXorOpposite","RGBTrailsXor", "DifferenceXor","MedianBlurXor"};
    CallFilter(Xor[rand()%Xor.size()], frame);
    AddInvert(frame);
}

void ac::RandomMirrorBlend(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    RandomMirror(frame_copy);
    static double alpha = 1.0, alpha_max = 7.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10.0, 0.005);
    AddInvert(frame);
}

void ac::RandomMirrorAlphaBlend(cv::Mat &frame) {
    DarkenImage(frame, 4);
    cv::Mat frame_copy = frame.clone();
    RandomMirror(frame_copy);
    static double alpha = 1.0, alpha_max = 7.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]*(alpha+1))+(pix[j]*alpha));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::Bitwise_XOR_AlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "Bitwise_XOR_AlphaSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    static double alpha = 1.0, alpha_max = 7.0;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.005);
    AddInvert(frame);
}

void ac::AlphaBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "AlphaBlendSubFilter")
        return;
    double alpha = 1.0, alpha_max = 7.0;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    AlphaBlend(frame_copy, output_copy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.005);
    AddInvert(frame);
}

void ac::GradientSubFilterXor(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "GradientSubFilterXor")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]+z)^(pix[j]+i);
            }
        }
    }
    AddInvert(frame);
}

void ac::XorBlend_SubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "XorBlend_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    static double alpha = 1.0, alpha_max = 7.0;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned int alpha_val = static_cast<unsigned int>(1+alpha);
                pixel[j] = (static_cast<unsigned char>((pixel[j]*alpha_val))) ^ (static_cast<unsigned char>((pix[j]*alpha_val)));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.005);
    AddInvert(frame);
}

void ac::SmoothSubFilterAlphaBlend(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "SmoothSubFilterAlphaBlend")
        return;
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    cv::Mat same_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame_copy, &collection);
    static double alpha = 1.0, alpha_max = 3.0;
    AlphaBlend(frame_copy, same_copy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.0, 0.1);
    AddInvert(frame);
}

void ac::SmoothSubFilterXorBlend(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "SmoothSubFilterXorBlend")
        return;
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    cv::Mat same_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame_copy, &collection);
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (static_cast<unsigned char>(pixel[j]*(alpha+1))) ^ (static_cast<unsigned char>(pix[j]*alpha));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.0, 0.1);
    AddInvert(frame);
}

void ac::IntertwineSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "IntertwineSubFilter")
        return;
    
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    bool skip_val = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            if(skip_val == true) {
                pixel = pix;
            }
        }
        skip_val = (skip_val == true) ? false : true;
    }
    AddInvert(frame);
}

void ac::RandBlend(cv::Mat &frame) {
    static std::vector<std::string> filter_names {"Self AlphaBlend", "Blend #3", "RandTriBlend", "Rainbow Blend", "Rand Blend","XorMultiBlend", "BlendedScanLines", "FrameBlend", "FrameBlendRGB", "PrevFrameBlend", "HorizontalBlend", "VerticalBlend", "OppositeBlend", "BlendSwitch", "IncreaseBlendHorizontal", "BlendIncrease","TrailsBlend", "BlendThree", "BlendTrails", "WeakBlend", "SmoothTrailsSelfAlphaBlend", "SmoothTrailsRainbowBlend","RandomAlphaBlend", "RandomTwoFilterAlphaBlend", "AlphaBlendPosition", "BlendAlphaXor", "AlphaBlendRandom", "ChannelSortAlphaBlend", "StrobeBlend", "GaussianBlend", "SelfXorBlend","CycleShiftRandomRGB_XorBlend", "CycleShiftRandomAlphaBlend","ParticleBlend", "BlendInAndOut", "BlendScaleInAndOut", "RandomXorBlend", "InitBlend"};
    
    CallFilter(filter_names[rand()%filter_names.size()], frame);
    AddInvert(frame);
}

void ac::EveryOther(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 6.0;
    bool on_off = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on_off == true) {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
                }
            }
            on_off = (on_off == true) ? false : true;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::EveryOtherSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "EveryOtherSubFilter")
        return;
    
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    bool on_off = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on_off == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
            on_off = (on_off == true) ? false : true;
        }
    }
    AddInvert(frame);
}

void ac::SmoothRandomFilter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    randomFilter(frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::RandomFilterRandomTimes(cv::Mat &frame) {
    randomFilter(frame);
    int num = rand()%4;
    for(int i = 0; i < num; ++i) {
        randomFilter(frame);
    }
    AddInvert(frame);
}

void ac::RandomSubFilterRandomTimes(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "RandomSubFilterRandomTimes")
        return;
    CallFilter(subfilter, frame);
    int num = rand()%4;
    for(int i = 0; i < num; ++i) {
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::AddToFrameSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "AddToFrameSubFilter")
        return;
    
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    Add(frame, frame_copy);
    AddInvert(frame);
}

void ac::MirrorXor(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b val = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j] ^ val[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorXorAll(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j] ^ values[0][j] ^ values[1][j] ^ values[2][j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorXorScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j] ^ values[0][j] ^ values[1][j] ^ values[2][j]);
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 5.0, 0.01);
    AddInvert(frame);
}

void ac::EnergyMirror(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    MirrorXorAll(frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "SmoothSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::EnergizeSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "EnergizeSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothSubFilter16(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "SmoothSubFilter16")
        return;
    static MatrixCollection<16> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::EnergizeSubFilter16(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "EnergizeSubFilter16")
        return;
    static MatrixCollection<16> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::EnergizeSubFilter32(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "EnergizeSubFilter32")
        return;
    
    static MatrixCollection<32> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothSubFilter32(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "SmoothSubFilter32")
        return;
    static MatrixCollection<32> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::HalfAddSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "HalfAddSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] /= 4;
                pixel[j] += pix[j];
            }
        }
    }
    AddInvert(frame);
}
void ac::HalfXorSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "HalfXorSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] /= 4;
                pixel[j] ^= pix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::StaticXorBlend(cv::Mat &frame) {
    static cv::Vec3b value(rand()%255, rand()%255, rand()%255);
    static MatrixCollection<8> collection;
    ChannelSort(frame);
    collection.shiftFrames(frame);
    StaticXor(frame, &collection, value);
    AddInvert(frame);
}

void ac::PsycheSort(cv::Mat &frame) {
    cv::Vec3b value(rand()%255,rand()%255,rand()%255);
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    ChannelSort(frame);
    StaticXor(frame, &collection,value);
    AddInvert(frame);
}

void ac::XorScale(cv::Mat &frame) {
    static cv::Scalar scale(rand()%255, rand()%255, rand()%255);
    static int speed = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^static_cast<unsigned char>(scale[j]);
        }
    }
    static int dir[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            scale[j] += speed;
            
            if(scale[j] >= 255)
                dir[j] = 0;
            
        } else if(dir[j] == 0) {
            scale[j] -= speed;
            
            if(scale[j] <= 1)
                dir[j] = 1;
        }
    }
    AddInvert(frame);
}

void ac::ChannelMedianSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    if(ac::draw_strings[subfilter] == "ChannelMedianSubFilter")
        return;
    cv::Mat frame_copy = frame.clone(), output;
    ChannelSort(frame_copy);
    CallFilter(subfilter, frame_copy);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::GaussianStrobe(cv::Mat &frame) {
    rainbowBlend(frame);
    GaussianBlur(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::StrobeSort(cv::Mat &frame) {
    ChannelSort(frame);
    GaussianStrobe(frame);
}

void ac::GlitchSortStrobe(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static cv::Vec3b color_(rand()%255, rand()%255, rand()%255);
    cv::Mat frame_copy = frame.clone();
    glitchSort(frame_copy);
    collection.shiftFrames(frame_copy);
    StaticXor(frame, &collection, color_);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::Bitwise_XOR_Blend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    GaussianBlur(frame_copy);
    collection.shiftFrames(frame_copy);
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i =0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b inner = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    values[q] += (inner[q]*alpha);
                }
            }
            for(int q = 0; q < 3; ++q) {
                values[q] /= collection.size();
                pixel[q] = static_cast<unsigned char>(values[q]) ^ static_cast<unsigned char>(pixel[q]*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 6.0);
    AddInvert(frame);
}

void ac::Bitwise_XOR_Sort(cv::Mat &frame) {
    ChannelSort(frame);
    Bitwise_XOR_Blend(frame);
}

void ac::Bitwise_OR_Blend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    GaussianBlur(frame_copy);
    collection.shiftFrames(frame_copy);
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i =0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b inner = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    values[q] += (inner[q]*alpha);
                }
            }
            for(int q = 0; q < 3; ++q) {
                values[q] /= collection.size();
                pixel[q] = static_cast<unsigned char>(values[q]) | static_cast<unsigned char>(pixel[3-q-1]*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 6.0);
    AddInvert(frame);
}

void ac::Bitwise_AND_Blend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    GaussianBlur(frame_copy);
    collection.shiftFrames(frame_copy);
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i =0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b inner = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    values[q] += (inner[q]*alpha);
                }
            }
            for(int q = 0; q < 3; ++q) {
                values[q] /= collection.size();
                pixel[q] = static_cast<unsigned char>(values[q]) & static_cast<unsigned char>(pixel[3-q-1]*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 6.0);
    AddInvert(frame);
}

void ac::BitwiseColorMatrix(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha[3] = {1.0,4.0,1.0}, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    GaussianBlur(frame_copy);
    collection.shiftFrames(frame_copy);
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i =0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b inner = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    values[q] += (inner[q]*alpha[q]);
                }
            }
            for(int q = 0; q < 3; ++q) {
                values[q] /= collection.size();
                pixel[q] ^= static_cast<unsigned char>(values[q]) & static_cast<unsigned char>(pixel[3-q-1]*alpha[q]);
            }
        }
    }
    static int dir[3] = {1, 0, 1};;
    
    for(int j = 0; j < 3; ++j)
        procPos(dir[j], alpha[j], alpha_max, 6.0, 0.1);
    
    AddInvert(frame);
}

void ac::PixelReverseXor(cv::Mat &frame) {
    static double alpha[3] = {1.0,4.0,1.0}, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixel;
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pix[3-j-1]*alpha[j])^pixel[j];
            }
        }
    }
    static int dir[3] = {1, 0, 1};;
    for(int j = 0; j < 3; ++j)
        procPos(dir[j], alpha[j], alpha_max, 6.0, 0.1);
    AddInvert(frame);
}

void ac::PixelatedSubFilterSort(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "PixelatedSubFilterSort")
        return;
    
    static MatrixCollection<8> collection;
    Block(frame);
    ChannelSort(frame);
    collection.shiftFrames(frame);
    SmoothRGB(frame, &collection);
    if(subfilter != -1)
        CallFilter(subfilter, frame);
}

void ac::SilverBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            double total = (pixel[0]+pixel[1]+pixel[2]) * alpha;
            unsigned int value = static_cast<unsigned int>(total);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^value;
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.0, 0.1);
}

void ac::RandomPixelOrderSort(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 5.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            SwapColors(pixel);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1));
            }
        }
    }
    StrobeSort(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::ImageXorAlpha(cv::Mat &frame) {
    if(blend_set == true) {
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        static double alpha = 1.0, alpha_max = 3.0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = (static_cast<unsigned char>((pixel[j]*(alpha+1))) ^ static_cast<unsigned char>((pix[j]*alpha)));
                }
            }
        }
        static int dir = 1;
        procPos(dir, alpha, alpha_max, 4.0, 0.01);
        AddInvert(frame);
    }
}

void ac::ImageAverageXor(cv::Mat &frame) {
    if(blend_set == true) {
        static MatrixCollection<8> collection;
        static double alpha = 1.0, alpha_max = 3.0;
        collection.shiftFrames(frame);
        DarkenFilter(frame);
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Scalar values;
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j)
                        values[j] += pix[j];
                }
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] /= collection.size();
                    pixel[j] = (static_cast<unsigned char>(pixel[j]*(alpha+1)) ^ static_cast<unsigned char>((pix[j]*alpha)) ^ static_cast<unsigned char>((values[j]*alpha)));
                }
            }
        }
        static int dir = 1;
        procPos(dir, alpha, alpha_max, 3.1, 0.1);
        AddInvert(frame);
    }
}

void ac::PixelXorBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 2.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixel;
            for(int j = 0; j < 3; ++j) {
                unsigned char v = static_cast<unsigned char>(pix[j]*alpha);
                pixel[j] = (pix[0]^pix[1]^pix[2])^v;
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 5.0, 0.1);
    AddInvert(frame);
}

void ac::SelfAlphaScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    static int index[3] = {0,255/3,255/2};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]^index[j])*alpha);
        }
    }
    
    for(int j = 0; j < 3; ++j) {
        ++index[j];
        if(index[j] >= 255) {
            index[j] = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::SelfScaleAlpha(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    static int index[3] = {0,255/3,255/2};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]*(alpha+1)))^index[j];
        }
    }
    static int direction[3] = {1,1,1};
    for(int j = 0; j < 3; ++j) {
        if(direction[j] == 1) {
            if(index[j] >= 255)
                direction[j] = 0;
            else
                ++index[j];
            
        } else if(direction[j] == 0) {
            if(index[j] <= 1)
                direction[j] = 1;
            else
                --index[j];
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::RainbowXorBlend(cv::Mat &frame) {
    static double alpha = 1.0f, alpha_max = 6.0;
    static int rb = 0, gb = 0, bb = 0;
    if(rb == 0)
        rb = rand()%255;
    else ++rb;
    if(gb == 0)
        gb = rand()%255;
    else ++gb;
    if(bb == 0)
        bb = rand()%255;
    else ++bb;
    static int i = 0, z = 0;
    for(z = 0; z < frame.rows; ++z) {
        for(i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = (pixel[0]^static_cast<unsigned char>(alpha*rb));
            pixel[1] = (pixel[1]^static_cast<unsigned char>(alpha*gb));
            pixel[2] = (pixel[2]^static_cast<unsigned char>(alpha*bb));
        }
    }
    if(rb > 255) rb = 0;
    if(gb > 255) gb = 0;
    if(bb > 255) bb = 0;
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
    resetAlpha(direction, alpha);
    AddInvert(frame);
}

void ac::FrameDifference(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Vec3b r(rand()%255, rand()%255, rand()%255);
    ImageDifference(frame, &collection, [=](cv::Vec3b &val) {
        for(int j = 0; j < 3; ++j) {
            val[j] = val[j]^r[j];
        }
    });
    AddInvert(frame);
}

void ac::SmallDiffference(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Vec3b pix_value(rand()%255, rand()%255, rand()%255);
    ImageDifference(frame, &collection, [=](cv::Vec3b &val) {
        for(int j = 0; j < 3; ++j) {
            val[j] = val[j]^pix_value[j];
        }
    }, 5);
    AddInvert(frame);
}

void ac::FadeBlend(cv::Mat &frame) {
    static cv::Scalar fade(rand()%255, rand()%255, rand()%255);
    static int dir[3] = { 1, 1, 1 };
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned char ch = static_cast<unsigned char>(fade[j]*alpha);
                pixel[j] = pixel[j]^ch;
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            fade[j] += alpha_increase;
            if(fade[j] >= 255) {
                dir[j] = 0;
            }
        } else if(dir[j] == 0) {
            fade[j] -= alpha_increase;
            if(fade[j] <= 1) {
                dir[j] = 1;
                fade[j] = rand()%255;
            }
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
    AddInvert(frame);
}

void ac::FilteredDifferenceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FilteredDifferenceSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy_frame = frame.clone();
    CallFilter(subfilter,copy_frame);
    ImageCopyDifference(frame, copy_frame, &collection);
    AddInvert(frame);
}
