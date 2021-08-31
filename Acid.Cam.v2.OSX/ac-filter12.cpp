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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = temp.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1))^static_cast<unsigned char>(pix[j]*alpha);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
                cv::Vec3b pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j)
                    values[j] += pixel[j];
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            for(int q = 0; q < collection.size()-1; ++q) {
                cv::Vec3b pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += pixel[j];
                    values[j] /= 1.5;
                }
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            for(int q = 0; q < collection.size()-1; ++q) {
                cv::Vec3b pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += pixel[j];
                    values[j] /= 1.5;
                }
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
        ac_resize(blend_image, copyf, frame.size());
        AlphaBlend(frame, copyf, outval, alpha);
        MedianBlur(outval);
        MedianBlur(outval);
        MedianBlur(outval);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
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

void ac::RandomSubFilter2(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomSubFilter2")
        return;
    static unsigned int index = 0;
    static std::vector<std::string> vSub { "Bitwise_XOR_AlphaSubFilter", "AlphaBlendSubFilter", "GradientSubFilterXor", "XorBlend_SubFilter","EnergizeSubFilter","PixelatedSubFilterSort","FilteredDifferenceSubFilter","ExpandSquareSubFilter","MirrorEnergizeSubFilter", "InterRGB_SubFilter", "InterSmoothSubFilter", "StoredFramesAlphaBlend_SubFilter", "BlendSubFilter", "BlendAlphaSubFilter", "Blend_AlphaSubFilter", "FrameMedianBlendSubFilter", "FrameBlurSubFilter","SubFilterMedianBlend", "DarkCollectionSubFilter", "FlipMedianSubFilter", "FlipMirrorSubFilter", "BlendCombinedValueSubFilter","CollectionXorSourceSubFilter","BlendReverseSubFilter","SmoothBlendReverseSubFilter","MedianBlendBufferSubFilter","RGBBlendSubFilter","XorOppositeSubFilter", "BlendSmoothSubFilter", "BlurSmoothSubFilter", "BlurFlipSubFilter", "MedianBlendSubFilterEx", "ShiftFrameSmoothSubFilter", "ShiftFrameStaticXorSubFilter"};
    
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int cx = i+pos[0];
            if(cx >= 0 && cx < frame.cols) {
                cv::Vec3b pix = pixelAt(frame,z, cx);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pix[j]^pixel[j];
                }
            } else {
                for(int j = 0; j < 3; ++j)
                	pixel[j] = pixel[j]^pos[0];
            }
            int cx_x = i+pos[1];
            if(cx_x >= 0 && cx_x < frame.cols) {
                cv::Vec3b pix=pixelAt(frame,z, cx_x);
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
    AddInvert(frame);
}

void ac::BlendWithFrameSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendWithFrameSubFilter")
        return;
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    CallFilter(subfilter, copyf);
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::AlphaBlendWithFrameSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AlphaBlendWithFrameSubFilter")
        return;

    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    CallFilter(subfilter, copyf);
    AlphaBlend(copyf, copyi, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::AlphaXorBlendWithFrameSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AlphaXorBlendWithFrameSubFilter")
        return;
    
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    CallFilter(subfilter, copyf);
    AlphaXorBlend(copyf, copyi, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.01);
    AddInvert(frame);
}

void ac::XorBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "XorBlendSubFilter")
        return;
    
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    Xor(frame, copyf);
    AddInvert(frame);
}

void ac::FlipAlphaBlend(cv::Mat &frame) {
    static int flip_ = -1;
    static bool strobe = true;
    cv::Mat copyi = frame.clone(), copyout;
    if(strobe == true) {
        cv::flip(copyi, copyout, flip_);
        strobe = false;
        AlphaBlend(copyi, copyout, frame, 0.5);
        if(++flip_ > 1)
            flip_ = -1;

    } else
        strobe = true;
    
    AddInvert(frame);
}

void ac::RandomFlipFilter(cv::Mat &frame) {
    int value = -1;
    int rnd = rand()%3;
    value += rnd;
    cv::Mat copyf = frame.clone();
    flip(copyf, frame, value);
    AddInvert(frame);
}

void ac::MirrorMedian(cv::Mat &frame) {
    static bool on_off = true;
    if(on_off == true) {
        cv::Mat copyf = frame.clone();
        cv::flip(copyf,frame,0);
        on_off = false;
    } else
        on_off = true;

    cv::Mat copyf = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[5];
            pix[0] = pixel;
            pix[1] = copyf.at<cv::Vec3b>(frame.rows-z-1, i);
            pix[2] = copyf.at<cv::Vec3b>(z, frame.cols-i-1);
            pix[3] = copyf.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pix[0][j] ^ pix[1][j] ^ pix[2][j] ^ pix[3][j];
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::FlipMatrixCollection(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copyf[3], copyi = frame.clone();
    cv::flip(frame, copyf[0], -1);
    cv::flip(frame, copyf[1],0);
    cv::flip(frame, copyf[2], 1);
    collection.shiftFrames(copyf[0]);
    collection.shiftFrames(copyf[1]);
    Smooth(copyf[2], &collection);
    AlphaBlend(copyf[2], copyi, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorMatrixCollection(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    cv::Mat copyf = frame.clone();
    Smooth(frame, &collection);
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[5];
            pix[0] = pixel;
            pix[1] = copyf.at<cv::Vec3b>(frame.rows-z-1, i);
            pix[2] = copyf.at<cv::Vec3b>(z, frame.cols-i-1);
            pix[3] = copyf.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Vec3b pixel2 = frame_copy.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2[5];
            pix2[0] = pixel2;
            pix2[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            pix2[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            pix2[3] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = ((pix[0][j] & pix2[0][j]) ^ (pix[1][j] & pix2[1][j]) ^ (pix[2][j] & pix2[2][j]));
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorMatrixSource(cv::Mat &frame) {
    BlurFlip(frame);
    BlendWithSource(frame);
    MirrorMatrixCollection(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SelfScaleByFrame(cv::Mat &frame) {
    cv::Mat copyf = frame.clone();
    static MatrixCollection<8> collection;
    SelfAlphaScale(copyf);
    collection.shiftFrames(copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i  = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[3].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ pix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::SmoothMedianRotateSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SmoothMedianRotateSubFilter")
        return;
    static MatrixCollection<16> collection;
    cv::Mat copyf = frame.clone(), outf;
    CallFilter(subfilter, copyf);
    int off = -1;
    int random_ = rand()%3;
    off += random_;
    cv::flip(copyf, outf, off);
    collection.shiftFrames(outf);
    Smooth(frame, &collection, false);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothCollectionAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copyf = frame.clone();
    rainbowBlend(copyf);
    Smooth(copyf, &collection);
    BlendWithSource(copyf);
    MedianBlur(frame);
    MedianBlur(frame);
    Xor(frame, copyf);
    AddInvert(frame);
}

void ac::XorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "XorSubFilter")
        return;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    Xor(frame, copyf);
    AddInvert(frame);
}
void ac::XorAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "XorAlphaSubFilter")
        return;
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copyf.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*(alpha+1)) ^ static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    AddInvert(frame);
}


void ac::BlurXorAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlurXorAlphaSubFilter")
        return;

    cv::Mat copyf = frame.clone();
    cv::Mat copyi = frame.clone();
    MedianBlur(copyf);
    MedianBlur(copyf);
    CallFilter(subfilter, copyf);
    MedianBlur(copyf);
    MedianBlur(copyf);
    Xor(copyi,copyf);
    cv::Mat copye = frame.clone();
    AlphaBlend(copyi, copye, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageXorFrame(cv::Mat &frame) {
    if(blend_set == true) {
        cv::Mat copye = frame.clone();
        cv::Mat copyf = frame.clone(), copyi;
        ac_resize(blend_image, copyi, frame.size());
        Xor(copyf, copyi);
        AlphaBlend(copyf, copye, frame, 0.8);
        AddInvert(frame);
    }
}

void ac::ImageXorFunction(cv::Mat &frame) {
    if(blend_set == true) {
        cv::Mat image_resized;
        ac_resize(blend_image, image_resized, frame.size());
        Xor(frame, image_resized);
        AddInvert(frame);
    }
}

void ac::ImageXorAlphaBlend(cv::Mat &frame) {
    if(blend_set == true) {
    	cv::Mat copyf = frame.clone();
    	cv::Mat image_resized;
    	ac_resize(blend_image, image_resized, frame.size());
        Xor(copyf, image_resized);
        static double alpha = 1.0, alpha_max = 4.0;
        cv::Mat copyi = frame.clone();
        AlphaBlend(copyf, copyi, frame, alpha);
        static int dir = 1;
        procPos(dir, alpha, alpha_max, 4.1, 0.05);
        AddInvert(frame);
    }
}

void ac::ImageAlphaXorMedianSubFilter(cv::Mat &frame) {
    if(blend_set == true && subfilter != -1 && ac::draw_strings[subfilter] != "ImageAlphaXorMedianSubFilter") {
        static double alpha = 1.0, alpha_max = 4.0;
        cv::Mat copyf = frame.clone(), copyi = frame.clone();
        cv::Mat resized;
        ac_resize(blend_image, resized, frame.size());
        CallFilter(subfilter, frame);
        AlphaBlend(copyf, resized, frame, alpha);
        Xor(frame, copyi);
        MedianBlend(frame);
        static int dir = 1;
        procPos(dir, alpha, alpha_max, 4.1, 0.05);
        AddInvert(frame);
    }
}

void ac::ImageSmoothAlphaXorSubFilter(cv::Mat &frame) {
    if(blend_set == true && subfilter != -1 && ac::draw_strings[subfilter] != "ImageSmoothAlphaXorSubFilter") {
        cv::Mat copyf = frame.clone(), copyi;
        cv::Mat copye = frame.clone();
        static MatrixCollection<8> collection;
        CallFilter(subfilter, copyf);
        Smooth(copyf, &collection);
        ac_resize(blend_image, copyi, frame.size());
        AlphaBlend(copyf,copyi,frame,0.5);
        Xor(frame, copye);
        AddInvert(frame);
    }
}

void ac::ImageXorMirrorFilter(cv::Mat &frame) {
    if(blend_set == true) {
    	cv::Mat copyf = frame.clone();
        cv::Mat imgf;
        ac_resize(blend_image, imgf, frame.size());
    	for(int z = 0; z < frame.rows; ++z) {
        	for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b frame_values[5], image_values[5];
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                frame_values[0] = pixel;
                frame_values[1] = copyf.at<cv::Vec3b>(copyf.rows-z-1, copyf.cols-i-1);
                frame_values[2] = copyf.at<cv::Vec3b>(z, copyf.cols-i-1);
                frame_values[3] = copyf.at<cv::Vec3b>(copyf.rows-z-1, i);
                image_values[0] = imgf.at<cv::Vec3b>(z, i);
                image_values[1] = imgf.at<cv::Vec3b>(imgf.rows-z-1, imgf.cols-i-1);
                image_values[2] = imgf.at<cv::Vec3b>(z, imgf.cols-i-1);
                image_values[3] = imgf.at<cv::Vec3b>(imgf.rows-z-1, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = frame_values[0][j]^image_values[0][j] ^ frame_values[1][j]^image_values[1][j] ^ frame_values[2][j]^image_values[2][j] ^ frame_values[3][j]^image_values[3][j]^pixel[j];
                }
        	}
    	}
        AddInvert(frame);
    }
}

void ac::ImageXorSubFilter(cv::Mat &frame) {
    if(blend_set == true && subfilter != -1 && ac::draw_strings[subfilter] != "ImageXorSubFilter") {
        cv::Mat copyf = frame.clone();
        CallFilter(subfilter, copyf);
        cv::Mat resized_blend;
        ac_resize(blend_image, resized_blend, frame.size());
        static double scale = 0.5;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = copyf.at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = resized_blend.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*scale) ^ static_cast<unsigned char>(pix1[j]*scale) ^ static_cast<unsigned char>(pix2[j]*scale);
                }
            }
        }
        static int dir = 1;
        if(dir == 1) {
            scale += 0.01;
            if(scale >= 1.0)
                dir = 0;
        } else {
            scale -= 0.01;
            if(scale <= 0.5)
                dir = 1;
        }
    }
}
