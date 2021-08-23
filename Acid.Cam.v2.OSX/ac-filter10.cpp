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

#include"ac.h"

void ac::ExpandSquareSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                pixel = frame_copy.at<cv::Vec3b>(z, i);
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::ExpandSquareBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1)) ^ static_cast<unsigned char>(pix[j]*alpha);
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
    AddInvert(frame);
}

void ac::ExpandSquareVerticalSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareVerticalSubFilter")
        return;
    static int start_x = frame.rows/2;
    static int stop_x = frame.rows/2;
    static int speed = frame.rows/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            if(z < frame.cols-1 && i < frame.rows-1 &&  z < frame_copy.cols-1 && i < frame_copy.rows-1)
            {
                cv::Vec3b &pixel = pixelAt(frame,i, z);
                pixel = frame_copy.at<cv::Vec3b>(i, z);
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.rows-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.rows/2)-1  || stop_x <= (frame.rows/2)-1) {
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::DarkImageMedianBlend(cv::Mat &frame) {
    if(blend_set == true) {
        SmoothImageAlphaBlend(frame);
        cv::Mat frame_copy = frame.clone();
        setGamma(frame_copy,frame,5);
        MedianBlend(frame);
    }
}

void ac::GammaDarken5(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    setGamma(frame_copy, frame, 5);
}

void ac::GammaDarken10(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    setGamma(frame_copy, frame, 10);
}

void ac::SelfAlphaScaleBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            unsigned int val = 0;
            for(int j = 0; j < 3; ++j) {
                val += static_cast<unsigned char>(pixel[j]*(alpha+1));
                pixel[j] = pixel[j]^val;
            }
            
        }
    }
    static int dir = 1;
    procPos(dir,alpha,alpha_max);
    AddInvert(frame);
}

void ac::FadeBars(cv::Mat &frame) {
    unsigned char ch[3] = {static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255)};
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^ch[j])*alpha);
                ++ch[j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 5.0, 0.1);
    AddInvert(frame);
}

void ac::MirrorXorAlpha(cv::Mat &frame) {
    static double alpha[3] = {1.0, 3.0, 1.0}, alpha_max = 3.0;
    static cv::Vec3b color_(rand()%255, rand()%255, rand()%255);
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(static_cast<unsigned char>((pixel[j]*alpha[j])) ^ static_cast<unsigned char>((values[0][j]*alpha[j])) ^ static_cast<unsigned char>((values[1][j]*alpha[j])) ^ static_cast<unsigned char>((values[2][j]*alpha[j])));
                pixel[j] = pixel[j]^color_[j];
            }
        }
    }
    AddInvert(frame);
    static int dir[3] = {1, 0, 1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            color_[j] += 5;
        } else if(dir[j] == 0) {
            color_[j] -= 5;
        }
        procPos(dir[j], alpha[j], alpha_max, 4.0, 0.1);
    }
}

void ac::MirrorEnergizeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorEnergizeSubFilter")
        return;
    DarkenImage(frame, 6);
    MirrorXorAlpha(frame);
    EnergizeSubFilter(frame);
    AddInvert(frame);
}

void ac::StrobeXor(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^pix[j];
        }
    }
    AddInvert(frame);
}

void ac::IntertwinedMirror(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    int lines = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^values[lines][j];
            }
        }
        ++lines;
        if(lines > 2)
            lines = 0;
    }
    AddInvert(frame);
}

void ac::BlurredMirror(cv::Mat &frame) {
    cv::Mat frame_copy;
    cv::medianBlur(frame, frame_copy, 3);
    DarkenImage(frame_copy, 2);
    DarkenImage(frame, 2);
    static double alpha = 1.0, alpha_max = 4.0;
    int lines = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j] * (1+alpha))) ^ static_cast<unsigned char>((values[lines][j]*alpha));
        }
        ++lines;
        if(lines > 2)
            lines = 0;
    }
    MedianBlend(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::ShadeRGB(cv::Mat &frame) {
    cv::Mat frame_copy;
    frame_copy = frame.clone();
    DarkenImage(frame_copy, 2);
    DarkenImage(frame, 2);
    static double alpha = 1.0, alpha_max = 4.0;
    int lines = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b values[6];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            if(lines <= 2)
            	pixel[lines] += static_cast<unsigned char>((pixel[lines] * alpha)) ^ static_cast<unsigned char>((values[lines][lines]*alpha));
        }
        ++lines;
        if(lines > 2)
            lines = 0;
        
    }
    MedianBlend(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::InterRGB_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "InterRGB_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    int index = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    pixel[0] = static_cast<unsigned char>(pixel[0]*alpha);
                    pixel[1] = pix[1];
                    pixel[2] = pix[2];
                    break;
                case 1:
                    pixel[0] = pix[0];
                    pixel[1] = static_cast<unsigned char>(pixel[1]*alpha);
                    pixel[2] = pix[2];
                    break;
                case 2:
                    pixel[0] = pix[0];
                    pixel[1] = pix[1];
                    pixel[2] = static_cast<unsigned char>(pixel[2]*alpha);
                    break;
            }
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::InterSmoothSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "InterSmoothSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    int index = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    for(int j = 0; j < 3; ++j)
                        pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1))^pix[j];
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
        }
        ++index;
        if(index > 1)
            index = 0;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::InterRGB_Bars_XY(cv::Mat &frame) {
    unsigned int index = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>((i+z)*alpha);
            }
            pixel[index] = 255;
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::InterRGB_Bars_X(cv::Mat &frame) {
    unsigned int index = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>(i*alpha);
            }
            pixel[index] = 255;
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::InterRGB_Bars_Y(cv::Mat &frame) {
    unsigned int index = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>(z*alpha);
            }
            pixel[index] = 255;
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::StoredFramesAlphaBlend_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "StoredFramesAlphaBlend_SubFilter")
        return;
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    cv::Mat fcopy = frame.clone();
    AlphaBlend(fcopy, frame_copy, frame, alpha);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::BlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::BlendAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendAlphaSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static double alpha = 1.0, alpha_max = 4.0;
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    for(int j = 0; j < 3; ++j)
                        pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1)) ^ static_cast<unsigned char>(pix[j]*alpha);
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

// duplicate for right now
void ac::ReverseFrameBlend(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    Reverse(frame_copy);
    MedianBlend(frame_copy);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::ReverseFrameBlendSwitch(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    Reverse(frame_copy);
    MedianBlend(frame_copy);
    static bool onval = true;
    int index = 0;
    if(onval == false)
        index = 1;
    onval = (onval == true) ? false : true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::DoubleRandomMirror(cv::Mat &frame) {
    cv::Mat frame_copy1 = frame.clone();
    RandomMirror(frame_copy1);
    RandomMirror(frame);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::Blend_AlphaSubFilter(cv::Mat &frame) {
    
    if(subfilter == -1 || ac::draw_strings[subfilter] == "Blend_AlphaSubFilter")
        return;
    
    cv::Mat frame_copy1 = frame.clone();
    RandomBlendFilter(frame);
    CallFilter(subfilter, frame_copy1);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::RandomBlendFilter(cv::Mat &frame) {
    static std::vector<std::string> vzBlend { "Self AlphaBlend", "Self Scale", "Blend #3", "Negative Paradox",  "ThoughtMode", "RandTriBlend", "Filter3","Rainbow Blend","Rand Blend","Pixel Scale","Pulse", "Combine Pixels", "Blend_Angle", "XorMultiBlend", "UpDown","LeftRight", "BlendedScanLines","XorSine", "FrameBlend", "FrameBlendRGB", "PrevFrameBlend", "HorizontalBlend", "VerticalBlend", "OppositeBlend", "DiagonalLines", "HorizontalLines", "BlendSwitch", "IncreaseBlendHorizontal", "BlendIncrease", "ColorRange", "VectorIncrease", "BlendThree", "HorizontalStripes", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur", "SurroundPixelXor", "WeakBlend", "AverageVertical", "RandomAlphaBlend", "RandomTwoFilterAlphaBlend", "AlphaBlendPosition", "BlendRowAlpha", "BlendRow", "BlendRowByVar", "BlendRowByDirection", "BlendAlphaXor", "SelfXorScale", "SelfAlphaRGB", "XorSelfAlphaImage", "AlphaBlendRandom", "ChannelSortAlphaBlend", "RandomXor", "RandomXorFlash", "SoftXor", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "BlendRowCurvedSqrt", "CycleShiftRandomAlphaBlend", "TanAlphaGrid", "BlendInAndOut", "BlendScaleInAndOut", "AcidGlitch", "LiquidFilter", "MatrixXorAnd", "XorAlpha", "SelfXorAverage", "RandomXorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "FadeInAndOut", "InitBlend", "LagBlend",  "RandBlend", "EveryOther","StaticXorBlend", "XorScale", "Bitwise_XOR_Blend", "Bitwise_OR_Blend", "Bitwise_AND_Blend", "PixelReverseXor", "SilverBlend", "PixelXorBlend", "SelfAlphaScale", "SelfScaleAlpha", "RainbowXorBlend", "FadeBlend", "SelfAlphaScaleBlend", "FadeBars", "ShadeRGB", "InterRGB_Bars_XY", "InterRGB_Bars_X", "InterRGB_Bars_Y", "ReverseFrameBlend", "ReverseFrameBlendSwitch"};
    
    CallFilter(vzBlend[rand()%vzBlend.size()], frame);
    AddInvert(frame);
}

void ac::DoubleRandomBlendFilter(cv::Mat &frame) {
    cv::Mat frame_copy1 = frame.clone();
    RandomBlendFilter(frame_copy1);
    RandomBlendFilter(frame);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::FlipBlendWH(cv::Mat &frame) {
    cv::Mat frame_copy1;
    cv::flip(frame, frame_copy1, -1);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::FlipBlendH(cv::Mat &frame) {
    cv::Mat frame_copy1;
    cv::flip(frame, frame_copy1, 0);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::FlipBlendW(cv::Mat &frame) {
    cv::Mat frame_copy1;
    cv::flip(frame, frame_copy1, 1);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::FlipBlendAll(cv::Mat &frame) {
    cv::Mat frame_copy[3];
    cv::flip(frame, frame_copy[0], 0);
    cv::flip(frame, frame_copy[1], 1);
    cv::flip(frame, frame_copy[2], -1);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[3];
            pix[0] = frame_copy[0].at<cv::Vec3b>(z, i);
            pix[1] = frame_copy[1].at<cv::Vec3b>(z, i);
            pix[2] = frame_copy[2].at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix[0];
                    break;
                case 2:
                    pixel = pix[1];
                    break;
                case 3:
                    pixel = pix[2];
                    break;
            }
            ++index;
            if(index > 3)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::FrameMedianBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FrameMedianBlendSubFilter")
        return;
    cv::Mat frame_copy;
    cv::flip(frame, frame_copy, 1);
    CallFilter(subfilter, frame_copy);
    MedianBlend(frame_copy);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = pix;
                    break;
            }
            ++index;
            if(index > 1)
                index = 0;
        }
    }
    MedianBlur(frame);
    RainbowXorBlend(frame);
    AddInvert(frame);
}

void ac::FrameBlurSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FrameBlurSubFilter")
        return;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy1 = frame.clone();
    cv::Mat frame_copy2 = frame.clone();
    MedianBlur(frame_copy1);
    CallFilter(subfilter, frame_copy1);
    MedianBlur(frame_copy1);
    MedianBlur(frame_copy2);
    AlphaBlend(frame_copy1, frame_copy2, frame, alpha);
    MedianBlur(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::ImageBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ImageBlendSubFilter")
        return;
    
    if(blend_set == true) {
        cv::Mat frame_copy1 = frame.clone();
        ExactImage(frame_copy1);
        CallFilter(subfilter, frame_copy1);
        
        int index = 0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = frame_copy1.at<cv::Vec3b>(z, i);
                switch(index) {
                    case 0:
                        break;
                    case 1:
                        pixel = pix1;
                        break;
                }
                ++index;
                if(index > 1)
                    index = 0;
            }
        }
    }
    
}

void ac::ImageBlendXorSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageBlendXorSubFilter")
        return;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy1 = frame.clone();
    cv::Mat frame_copy2 = frame.clone();
    ExactImage(frame_copy1);
    CallFilter(subfilter, frame_copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[3];
            pix[0] = frame_copy1.at<cv::Vec3b>(z, i);
            pix[1] = frame_copy2.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>(pix[0][j]*alpha)^pix[1][j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
}

void ac::ImageCollectionSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageBlendXorSubFilter")
        return;
    static double alpha = 1.0, alpha_max = 4.0;
    static int dir = 1;
    cv::Mat frame_copy1 = frame.clone();
    cv::Mat frame_copy2 = frame.clone();
    ExactImage(frame_copy1);
    AlphaBlend(frame_copy1, frame_copy2, frame, alpha);
    CallFilter(subfilter, frame);
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}

void ac::SelfScaleXorIncrease(cv::Mat &frame) {
    static double alpha = 1.0, increase_val = 0.05, limit_start = 1.0, limit = limit_start,min_start = 4.0, min = min_start, max = 10.0, rev_max = 1.0;
    if(alpha_increase != 0)
        increase_val = alpha_increase;
    else
        increase_val = 0.05;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1))^static_cast<unsigned char>(alpha*15);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        alpha += increase_val;
        if(alpha >= limit) {
            dir = 0;
            limit += 1.0;
            if(limit > max) {
                limit = limit_start;
            }
        }
    } else if(dir == 0) {
        alpha -= increase_val;
        if(alpha <= min) {
            dir = 1;
            min -= 1.0;
            if(min < rev_max) {
                min = min_start;
            }
        }
    }
    AddInvert(frame);
}

void ac::Blend_RedGreenBlue(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    static double speed_val = 5.0;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(values[j]);
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 255) {
            values[j] = 0;
            break;
        } else {
            values[j] += speed_val;
        }
    }
    AddInvert(frame);
}

void ac::XorBlend_RedGreenBlue(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    static double speed_val = 2.5;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<unsigned char>(0.5*values[j]);
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 255) {
            values[j] = 0;
            break;
        } else {
            values[j] += speed_val;
        }
    }
    AddInvert(frame);
    
}

void ac::BlendIncrease_RedGreenBlue(cv::Mat &frame) {
    static bool values_dir[3];
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    static double speed_val = 1.5;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(0.5 * values[j]);
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 255) {
            if(values_dir[j] == true)
                values_dir[j] = false;
            else
                values_dir[j] = true;
            break;
        } else {
            if(values_dir[j] == true)
                values[j] += speed_val;
            else
                values[j] -= speed_val;
        }
    }
    AddInvert(frame);
}

void ac::Blend_RedReenBlue_Dark(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    static double speed_val = 5.0;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned int val = static_cast<unsigned int>(values[j]);
                if(pixel[j] == 0) pixel[j] = 1;
                pixel[j] = val%pixel[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 255) {
            values[j] = 0;
            break;
        } else {
            values[j] += speed_val;
        }
    }
    AddInvert(frame);
}

void ac::DarkModBlend(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    static double speed_val = 5.0;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned int val = static_cast<unsigned int>(values[j]);
                if(pixel[j] == 0) pixel[j] = 1;
                pixel[j] = (val^pixel[j]);
                if(pixel[j] == 0) pixel[j] = 1;
                pixel[j] = val%pixel[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 255) {
            for(int q = 0; q < 3; ++q)
            	values[j] = rand()%255;
            values[j] = 0;
            break;
        } else {
            values[j] += speed_val;
        }
    }
    AddInvert(frame);
}

void ac::PictureBuzz(cv::Mat &frame) {
    static cv::Scalar values(rand()%255, rand()%255, rand()%255);
    cv::Vec3b l_values;
    static double speed_val = 0.01;
    if(reset_alpha) {
        for(int j = 0; j < 3; ++j)
            values[j] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                l_values[j] += pixel[j];
                unsigned int val = static_cast<unsigned int>(values[j]);
                if(l_values[j] == 0) l_values[j] = 1;
                pixel[j] += val%l_values[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(values[j] > 1024) {
            values[j] = 0;
            break;
        } else {
            values[j] += speed_val;
        }
    }
    AddInvert(frame);
}

void ac::IncDifference(cv::Mat &frame) {
    static cv::Mat fcopy = frame.clone();
    static cv::Size size_var;
    if(fcopy.size() != frame.size()) {
        fcopy = frame.clone();
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b back = pixel;
            cv::Vec3b &pix_cp = fcopy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += pix_cp[j];
                pix_cp[j] = 255-back[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::IncDifferenceAlpha(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    static cv::Mat fcopy = frame.clone();
    static cv::Size size_var;
    int index = rand()%3;
    if(fcopy.size() != frame.size()) {
        fcopy = frame.clone();
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b back = pixel;
            cv::Vec3b &pix_cp = fcopy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(pixel[j] == 0) pixel[j] = 1;
                pixel[j] = pix_cp[j]%(1+(pixel[j]));
            }
            pix_cp[index] = static_cast<unsigned char>(255-(back[index]*alpha));
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}
