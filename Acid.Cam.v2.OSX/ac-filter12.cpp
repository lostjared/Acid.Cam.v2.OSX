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

void ac::BlendReverseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendReverseSubFitler")
        return;
    cv::Mat copyf = frame.clone();
    Reverse(copyf);
    CallFilter(subfilter, copyf);
    static double alpha = 1.0, alpha_max = 7.0;
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copyf.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    pixel[0] = pixel[0]^pix[0];
                    pixel[1] = pixel[1]^pix[1];
                    pixel[2] = pixel[2]^pix[2];
                    break;
                case 1:
                    pixel[0] = pixel[2]^pix[0];
                    pixel[1] = pixel[0]^pix[1];
                    pixel[2] = pixel[1]^pix[2];
                    break;
                case 2:
                    pixel[0] = pixel[1]^pix[0];
                    pixel[1] = pixel[2]^pix[1];
                    pixel[2] = pixel[0]^pix[2];
                    break;
                case 3:
                    pixel[0] = pixel[0]^pix[2];
                    pixel[1] = pixel[1]^pix[0];
                    pixel[2] = pixel[2]^pix[1];
                    break;
                case 4:
                    pixel[0] = pixel[0]^pix[0];
                    pixel[1] = pixel[1]^pix[2];
                    pixel[2] = pixel[2]^pix[1];
                    break;
                case 5:
                    pixel[0] = pixel[0]^pix[1];
                    pixel[1] = pixel[1]^pix[2];
                    pixel[2] = pixel[2]^pix[0];
                    break;
            }
        }
    }
    ++index;
    if(index > 5)
        index = 0;
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 7.1, 0.005);
    AddInvert(frame);
}

void ac::MirrorBitwiseXor(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat temp = frame.clone();
    FlipBlendAll(temp);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = temp.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha)^static_cast<unsigned char>(pix[j]*alpha);
            }
        }
    }
    MirrorXorAll(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.001);
    DarkenFilter(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothBlendReverseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothBlendReverseSubFilter")
        return;
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 4.1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    BlendReverseSubFilter(copy2);
    collection.shiftFrames(copy1);
    collection.shiftFrames(copy2);
    Smooth(frame, &collection,false);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}


void ac::RandomIncrease(cv::Mat &frame) {
    static cv::Vec3b values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += values[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        values[j] = size_reset(values[j]+rand()%25);
    }
    AddInvert(frame);
}

void ac::MedianBlend16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += pixel[q];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = 1+static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::MedianBlendBufferSubFilter(cv::Mat &frame) {
    
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianBlendBufferSubFilter")
        return;
    
    static MatrixCollection<12> collection;
    
    cv::Mat copy_f = frame.clone();
    CallFilter(subfilter, copy_f);
    
    for(int i = 0; i < 3; ++i) {
        MedianBlur(frame);
        MedianBlur(copy_f);
    }
    
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += pixel[q];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_f.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = 1+static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val ^ pix[j]);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::BGRBlend(cv::Mat &frame) {
    for(int j = 0; j < 3; ++j)
        MedianBlur(frame);
    static MatrixCollection<12> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar values;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    values[j] += pixel[q];
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = pixel;
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(values[j]);
                pixel[j] = pix[3-j-1] ^ val;
            }
        }
    }
    AddInvert(frame);
}

void ac::RGBBlend(cv::Mat &frame) {
    cv::Mat noblur = frame.clone();
    for(int j = 0; j < 3; ++j)
        MedianBlur(frame);
    static MatrixCollection<12> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar values;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += pixel[q];
                    values[j] /= 1.5;
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = noblur.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(values[j]);
                pixel[j] = pixel[j] ^ val;
            }
        }
    }
    AddInvert(frame);
}

void ac::RGBBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RGBBlendSubFilter")
        return;
    for(int j = 0; j < 3; ++j)
        MedianBlur(frame);
    cv::Mat copy_f = frame.clone();
    CallFilter(subfilter, copy_f);
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar values;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += pixel[q];
                    values[j] /= 1.5;
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_f.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(values[j]);
                pixel[j] = pixel[j] ^ val ^ pix[j];
            }
        }
    }
    AddInvert(frame);
}


void ac::DivideAndIncH(cv::Mat &frame) {
    unsigned int x = 0, y = 0;
    unsigned int counter_x = 0, counter_y = 0;
    unsigned int row_x = frame.cols/255;
    unsigned int row_y = frame.rows/255;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] += x;
            pixel[1] += y;
            ++counter_x;
            if(counter_x >= row_x) {
                counter_x = 0;
                ++x;
            }
        }
        ++counter_y;
        if(counter_y >= row_y) {
            counter_y = 0;
            ++y;
        }
    }
    AddInvert(frame);
}

void ac::DivideAndIncW(cv::Mat &frame) {
    unsigned int x = 0, y = 0;
    unsigned int counter_x = 0, counter_y = 0;
    unsigned int row_x = frame.cols/255;
    unsigned int row_y = frame.rows/255;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] += x;
            pixel[1] += y;
            ++counter_x;
            if(counter_x >= row_x) {
                counter_x = 0;
                ++x;
            }
        }
        ++counter_y;
        if(counter_y >= row_y) {
            counter_y = 0;
            ++y;
        }
    }
    AddInvert(frame);
}

void ac::XorOppositeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "XorOppositeSubFilter")
        return;
    
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b rpix = copyf.at<cv::Vec3b>(frame.rows-z-1, i);
            cv::Vec3b rpix1 = copyf.at<cv::Vec3b>(z, frame.cols-i-1);
            cv::Vec3b rpix2 = copyf.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = (pixel[j]^rpix[j])^(rpix1[j]^rpix2[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::BlendSmoothSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendSmoothSubFilter")
        return;
    static MatrixCollection<12> collection;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    collection.shiftFrames(copyf);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurSmooth(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    for(int i = 0; i < 5; ++i)
    	MedianBlur(frame);
    
    AddInvert(frame);
}

void ac::BlurSmoothMedian(cv::Mat &frame) {
    BlurSmooth(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::BlurSmoothSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurSmoothSubFilter")
        return;
    
    static MatrixCollection<16> collection;
    CallFilter(subfilter, frame);
    for(int i = 0; i < 5; ++i)
        MedianBlur(frame);

    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::BlurFlip(cv::Mat &frame) {
    for(int j = 0; j < 3; ++j)
        MedianBlur(frame);
    FlipBlendAll(frame);
    int amt = rand()%3;
    for(int j = 0; j < amt+3; ++j)
        MedianBlur(frame);
    MedianBlend(frame);
    XorAlpha(frame);
    cv::Mat copy = frame.clone();
    setGamma(copy, frame, 5);
    AddInvert(frame);
}

void ac::BlurFlipSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurFlipSubFilter")
        return;
    cv::Mat copyf = frame.clone();
    cv::Mat copyi = frame.clone();
    CallFilter(subfilter, copyf);
    for(int j = 0; j < 3; ++j)
        MedianBlur(copyi);
    FlipBlendAll(copyi);
    int amt = rand()%3;
    for(int j = 0; j < amt+3; ++j)
        MedianBlur(copyi);
    MedianBlend(copyi);
    XorAlpha(copyi);
    cv::Mat cpi = copyi.clone();
    setGamma(cpi, copyi, 5);
    static double alpha = 1.0, alpha_max = 7.0;
    AlphaBlend(copyf, copyi, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::BlurMirrorGamma(cv::Mat &frame) {
    MirrorXorAll(frame);
    cv::Mat copyf = frame.clone();
    cv::Mat copyi = frame.clone();
    cv::Mat copyo = frame.clone();
    setGamma(copyf, copyo, 2);
    setGamma(copyi, copyf, 5);
    int r = rand()%3;
    for(int i = 0; i < 3+r; ++i) {
        MedianBlur(copyo);
        MedianBlur(copyf);
    }
    FlipBlendWH(copyf);
    static double alpha = 1.0, alpha_max = 4.0;
    AlphaBlend(copyo,copyf,frame,alpha);
    MedianBlend(frame);
    cv::Mat copyt = frame.clone();
    setGamma(copyt, frame, 2);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.1);
    AddInvert(frame);
}

void ac::MedianBlendDark(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Scalar value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] -= pixel[q];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = 1+static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    cv::Mat copyf = frame.clone();
    setGamma(copyf, frame, 4);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
    AddInvert(frame);
}

void ac::MedianBlendSubFilterEx(cv::Mat &frame) {
    
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MedianBlendSubFilterEx")
        return;
    
    CallFilter(subfilter, frame);
    cv::Mat copysub = frame.clone();
    static MatrixCollection<12> collection;
    int r = 3;;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += pixel[q];
                }
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int val = 1+static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    cv::Mat output = frame.clone();
    AlphaBlend(copysub, output, frame, 0.5);
    AddInvert(frame);
}

void ac::EnergyMirrorDark(cv::Mat &frame) {
    MirrorXorAll(frame);
    BlendWithSource(frame);
    MedianBlendDark(frame);
    AddInvert(frame);
}

void ac::AlphaBlendMirror(cv::Mat &frame) {
    cv::Mat copyf = frame.clone();
    cv::Mat ucopy;
    cv::flip(frame, ucopy, -1);
    static double alpha = 1.0, alpha_max = 2.0;
    AlphaBlend(copyf, ucopy, frame, alpha);
    cv::flip(frame, ucopy, 1);
    cv::Mat ecopy = frame.clone();
    AlphaXorBlend(ecopy, ucopy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 2.1, 0.01);
    AddInvert(frame);
}


void ac::AlphaBlendXorImage(cv::Mat &frame) {
    if(blend_set == true) {
    	static double alpha = 1.0, alpha_max = 3.0;
        
        cv::Mat copyf,outval;
        cv::resize(blend_image, copyf, frame.size());
        AlphaBlend(frame, copyf, outval, alpha);
        MedianBlur(outval);
        MedianBlur(outval);
        MedianBlur(outval);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copyf.at<cv::Vec3b>(z, i);
                cv::Vec3b pixval = outval.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = (pixel[j]^pix[j]^pixval[j]);
                }
            }
        }
        static int dir = 1;
    	procPos(dir, alpha, alpha_max, 3.1, 0.05);
    }
    AddInvert(frame);
}

void ac::ShiftFrameSmoothSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ShiftFrameSmoothSubFilter")
        return;
    static MatrixCollection<16> collection;
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection, false);
    AddInvert(frame);
}


void ac::ShiftFrameStaticXorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ShiftFrameStaticXorSubFilter")
        return;
    static MatrixCollection<16> collection;
    CallFilter(subfilter, frame);
    cv::Scalar scalar;
    ScalarAverage(frame, scalar);
    cv::Vec3b val;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int j = 0; j < 3; ++j)
        val[j] = static_cast<unsigned char>(scalar[j] * alpha);
    StaticXor(frame, &collection, val);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 3.1, 0.01);
    AddInvert(frame);
}

void ac::IncreaseDecreaseGamma(cv::Mat &frame) {
    static int dir = 1;
    static int light = 1;
    static int min = 1, max = 10;
    cv::Mat copyf = frame.clone();
    setGamma(copyf, frame, light);
    if(dir == 1) {
        ++light;
        if(light > max)
            dir = 0;
    } else {
        --light;
        if(light <= min)
            dir = 1;
        
    }
    AddInvert(frame);
}

void ac::GammaIncDecIncrease(cv::Mat &frame) {
    static int dir = 1;
    static int light = 1;
    static int min = 1, max = 3, total_max = 12;
    cv::Mat copyf = frame.clone();
    setGamma(copyf, frame, light);
    if(dir == 1) {
        ++light;
        if(light > max) {
            ++max;
            if(max > total_max) {
                max = 1;
            }
            dir = 0;
        }
    } else if(dir == 0) {
        --light;
        if(light <= min) {
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::RandomSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomSubFilter")
        return;
    static int index = 0;
    static std::vector<std::string> vSub { "Bitwise_XOR_AlphaSubFilter", "AlphaBlendSubFilter", "GradientSubFilterXor", "XorBlend_SubFilter","EnergizeSubFilter","PixelatedSubFilterSort","FilteredDifferenceSubFilter","ExpandSquareSubFilter","MirrorEnergizeSubFilter", "InterRGB_SubFilter", "InterSmoothSubFilter", "StoredFramesAlphaBlend_SubFilter", "BlendSubFilter", "BlendAlphaSubFilter", "Blend_AlphaSubFilter", "FrameMedianBlendSubFilter", "FrameBlurSubFilter","SubFilterMedianBlend", "DarkCollectionSubFilter", "FlipMedianSubFilter", "FlipMirrorSubFilter", "BlendCombinedValueSubFilter","CollectionXorSourceSubFilter","BlendReverseSubFilter","SmoothBlendReverseSubFilter","MedianBlendBufferSubFilter","RGBBlendSubFilter","XorOppositeSubFilter", "BlendSmoothSubFilter", "BlurSmoothSubFilter", "BlurFlipSubFilter", "MedianBlendSubFilterEx", "ShiftFrameSmoothSubFilter", "ShiftFrameStaticXorSubFilter"};
    
    static auto rng = std::default_random_engine{};
    CallFilter(vSub[index], frame);
    ++index;
    if(index > vSub.size()-1) {
        index = 0;
        std::shuffle(vSub.begin(), vSub.end(),rng);
    }
    AddInvert(frame);
}

void ac::TwistedVision(cv::Mat &frame) {
    static int pos[3] = {2,frame.cols-1,2};
    static int sized_w = frame.size().width;
    if(sized_w != frame.size().width) {
        pos[1] = frame.cols-1;
        sized_w = frame.size().width;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cx = i+pos[0];
            if(cx >= 0 && cx < frame.cols) {
                cv::Vec3b pix = frame.at<cv::Vec3b>(z, cx);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pix[j]^pixel[j];
                }
            } else {
                for(int j = 0; j < 3; ++j)
                	pixel[j] = pixel[j]^pos[0];
            }
            int cx_x = i+pos[1];
            if(cx_x >= 0 && cx_x < frame.cols) {
                cv::Vec3b pix=frame.at<cv::Vec3b>(z, cx_x);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            } else {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = pixel[j]^pos[1];
            }
        }
    }
    ++pos[0];
    if(pos[0] > frame.cols/2) {
        pos[0] = 2;
    }
    --pos[1];
    if(pos[1] <= frame.cols/2)
        pos[1] = frame.cols-1;
    
    AddInvert(frame);
}

void ac::TwistedMirror(cv::Mat &frame) {
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    FlipBlendAll(copyf);
    TwistedVision(copyi);
    MirrorXorAll(copyi);
    AlphaBlend(copyf,copyi,frame, 0.7);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SelfScaleSortBlend(cv::Mat &frame) {
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    StrobeSort(copyf);
    SelfScale(copyi);
    AlphaBlend(copyi, copyf, frame, 0.5);
    AddInvert(frame);
}

void ac::FlashMedianBlend(cv::Mat &frame) {
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    cv::Vec3b color(rand()%255,rand()%255,rand()%255);
    for(int z = 0; z < copyf.rows; ++z) {
        for(int i = 0; i < copyf.cols; ++i) {
            cv::Vec3b &pixel = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^color[j];
            }
        }
    }
    MedianBlend(copyf);
    AlphaBlend(copyf, copyi, frame, 0.5);
}
