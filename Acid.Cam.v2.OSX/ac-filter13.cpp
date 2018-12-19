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


void ac::ImageAlphaXorSubFilter(cv::Mat &frame) {
    if(blend_set == true && subfilter != -1 && ac::draw_strings[subfilter] != "ImageAlphaXorSubFilter") {
        cv::Mat copyf = frame.clone(), copyi = frame.clone();;
        cv::Mat resized;
        cv::resize(blend_image, resized, frame.size());
        cv::Mat out;
        CallFilter(subfilter, copyi);
        AlphaBlend(resized, copyi, out, 0.5);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = out.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SmoothTrailsBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copyf = frame.clone();
    cv::Mat copyi = frame.clone();
    Smooth(copyf, &collection);
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionRGBXor(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    cv::Mat copyf = frame.clone();
    MovementRGBTrails(copyf);
    collection.shiftFrames(copyf);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Scalar sc;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    sc[j] += pix[j];
            }
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(sc[j]);
                pixel[j] = pixel[j]^val;
            }
        }
    }
    BlendWithSource(frame);
    AddInvert(frame);
}

void ac::RainbowGlitch(cv::Mat &frame) {
    SmoothTrailsBlend(frame);
    MovementRGBTrails(frame);
    AddInvert(frame);
}

void ac::RainbowGlichStrobe(cv::Mat &frame) {
    SmoothTrailsBlend(frame);
    MovementRGBTrails(frame);
    static bool negate = true;
    if(negate == true) {
        Negate(frame);
        negate = false;
    } else {
        negate = true;
    }
    AddInvert(frame);
}

void ac::NegateSwitchStrobe(cv::Mat &frame) {
    static bool strobe_value = true;
    if(strobe_value == true) {
        Negate(frame);
        strobe_value = false;
    } else
        strobe_value = true;
    
    AddInvert(frame);
}

void ac::StrobeAlphaShuffle(cv::Mat &frame) {
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    ShuffleAlpha(copyf);
    static bool neg_ = true;
    if(neg_ == true) {
        Negate(copyf);
        neg_ = false;
    } else {
        neg_ = true;
    }
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::ShuffleAlphaWithRGB(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ShuffleAlpha(copy1);
    ShuffleRGB(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ShuffleAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ShuffleAlphaSubFilter")
        return;
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    ShuffleAlpha(copyf);
    CallFilter(subfilter, copyi);
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::ShuffleColorMap(cv::Mat &frame) {
    static int index = 0;
    static std::vector<int> v{0,1,2,3,4,5,6,7,8,9,10,11};
    static auto rng = std::default_random_engine{};
    ac::setColorMap(v[index], frame);
    ++index;
    if(index > v.size()-1) {
        index = 0;
        std::shuffle(v.begin(), v.end(),rng);
    }
    AddInvert(frame);
}

void ac::BlendWithRainbowSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendWithRainbowSubFilter")
        return;
    blendFilterWithColorMap(subfilter, 4, frame);
    AddInvert(frame);
}

void ac::BlendWithJetSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "BlendWithJetSubFilter")
        return;
    blendFilterWithColorMap(subfilter, 2, frame);
    AddInvert(frame);
}

void ac::ColormapBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ColormapBlendSubFilter")
        return;
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    setColorMap(rand()%11, copyf);
    setColorMap(rand()%11, copyi);
    RGBColorTrails(copyi);
    CallFilter(subfilter, copyf);
    AlphaBlend(copyf, copyi, frame, 0.5);
    AddInvert(frame);
}

void ac::RandomColorMap(cv::Mat &frame) {
    setColorMap(rand()%11, frame);
    AddInvert(frame);
}

void ac::SmoothMirrorBlurFlip(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    SmoothTrailsBlend(copy1);
    MirrorBitwiseXor(copy1);
    SmoothTrailsBlend(copy2);
    BlurFlip(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::RandomColorMapAlphaBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomColorMapAlphaBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    RandomColorMap(copy1);
    CallFilter(subfilter, copy1);
    static double alpha = 1.0, alpha_max = 4.0;
    AlphaBlend(copy1, copy2, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.005);
    AddInvert(frame);
}

void ac::RandomOrder(cv::Mat &frame) {
    int color_order = 0;
    static std::vector<int> colors { 1,2,3,4 };
    static auto rng = std::default_random_engine{};
    static int index = static_cast<int>(colors.size()+1);
    if(index > colors.size()) {
        std::shuffle(colors.begin(), colors.end(),rng);
        index = 0;
    }
    color_order = colors[index++];
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &cur = frame.at<cv::Vec3b>(z, i);
            SwitchOrder(cur, color_order);
        }
    }
    AddInvert(frame);
}

void ac::RandomOrderMedianBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomOrderMedianBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    RandomOrder(frame);
    AddInvert(frame);
}


void ac::MirrorOrder(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int index = 0;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[6];
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy1.at<cv::Vec3b>(copy1.rows-z-1, i);
            pix[2] = copy1.at<cv::Vec3b>(copy1.rows-z-1, copy1.cols-i-1);
            pix[3] = copy1.at<cv::Vec3b>(z, copy1.cols-i-1);
            pix[4] = copy1.at<cv::Vec3b>(z+1, i+1);
            for(int j = 0; j < 3; ++j)
                SwitchOrder(pix[j], index);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pix[0][j] ^ pix[1][j] ^ pix[2][j] ^ pix[3][j] ^ pix[4][j] ^ pixel[j];
            }
        }
    }
    ++index;
    if(index > 4)
        index = 1;
    
    AddInvert(frame);
}

void ac::MirrorOrderSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorOrderSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    MirrorOrder(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    SmoothTrailsBlend(frame);
    DarkenImage(frame, 4);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::BlurMirrorOrder(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    MedianBlur(copy1);
    MedianBlur(copy1);
    MedianBlur(copy1);
    static double alpha = 1.0, alpha_max = 4.0;
    static int index = 0;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[6];
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy1.at<cv::Vec3b>(copy1.rows-z-1, i);
            pix[2] = copy1.at<cv::Vec3b>(copy1.rows-z-1, copy1.cols-i-1);
            pix[3] = copy1.at<cv::Vec3b>(z, copy1.cols-i-1);
            pix[4] = copy1.at<cv::Vec3b>(z+1, i+1);
            for(int j = 0; j < 3; ++j)
                SwitchOrder(pix[j], index);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(((pix[0][j] ^ pix[1][j] ^ pix[2][j] ^ pix[3][j] ^ pix[4][j] ^ pixel[j])/8) * alpha);
            }
        }
    }
    ++index;
    if(index > 4)
        index = 1;
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 4.1, 0.05);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::AveragePixelMirror(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    MedianBlur(copy1);
    MedianBlur(copy1);
    static int index = 0;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[6];
            pix[0] = copy1.at<cv::Vec3b>(z, i);
            pix[1] = copy1.at<cv::Vec3b>(copy1.rows-z-1, i);
            pix[2] = copy1.at<cv::Vec3b>(copy1.rows-z-1, copy1.cols-i-1);
            pix[3] = copy1.at<cv::Vec3b>(z, copy1.cols-i-1);
            cv::Scalar s;
            for(int i = 0; i < 5; ++i) {
                for(int j = 0; j < 3; ++j) {
                    s[j] += pix[i][j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                s[j] /= 4;
                unsigned int value1 = static_cast<unsigned int>(s[j]);
                unsigned int value2 = pixel[j]%value1;
                pixel[j] = pixel[j]^value2;
            }
        }
    }
    ++index;
    if(index > 4)
        index = 1;
    AddInvert(frame);
}

void ac::ShuffleAlphaMedianBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ShuffleAlpha(copy1);
    ShuffleAlpha(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorOrderAlpha(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorOrder(copy1);
    MirrorXorAll(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
}

void ac::FilterStrobeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FilterStrobeSubFilter")
        return;
    static bool flash = true;
    if(flash == true) {
        CallFilter(subfilter, frame);
        Negate(frame);
        flash = false;
    } else {
        flash = true;
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::ImageSubtractMedianBlend(cv::Mat &frame) {
    if(blend_set == true) {
        cv::Mat image1;
        cv::resize(blend_image, image1, frame.size());
        for(int z  = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = image1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] -= (pixel[j] ^ pix[j])/6;
                }
            }
        }
        MedianBlend(frame);
    }
    AddInvert(frame);
}

void ac::ImageDarkBlend(cv::Mat &frame) {
    if(blend_set == true) {
        cv::Mat reimage;
        cv::resize(blend_image, reimage, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            cv::Scalar value;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    unsigned char val = pixel[j]%(1+pix[j]);
                    pixel[j] = pixel[j]^val;
                }
            }
        }
    }
}
