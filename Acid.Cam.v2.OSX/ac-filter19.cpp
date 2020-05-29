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

// Use as a subfilter for Smooth
void ac::ImageRandomValues(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    int r_x = rand()%(frame.cols-1);
    int r_y = rand()%(frame.rows-1);
    if(r_x < 10)
        r_x = 10;
    if(r_y < 10)
        r_y = 10;
    cv::Size size_val(r_x, r_y);
    ac_resize(blend_image, reimage, size_val);
    copyMat(frame, reimage);
    AddInvert(frame);
}

void ac::AlphaBlendTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    
    for(int i = 15; i >= 0;  i -= 2) {
        cv::Mat &cp = collection.frames[i];
        AlphaBlendDouble(copy1, cp, copy2, 0.6, 0.4);
        copy1 = copy2.clone();
    }
    AlphaBlend(frame, copy1, copy2,0.5);
    frame = copy2.clone();
    AddInvert(frame);
}

void ac::AlphaBlendTrailsReverse(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 16;  i += 2) {
        cv::Mat &cp = collection.frames[i];
        AlphaBlendDouble(copy1, cp, copy2, 0.6, 0.4);
        copy1 = copy2.clone();
    }
    AlphaBlend(frame, copy1, copy2,0.5);
    frame = copy2.clone();
    AddInvert(frame);
}


void ac::VideoStretchHorizontal(cv::Mat &frame) {
    static int x = 0, dir = 2;
    static const int speed = 20;
    if(x < frame.cols-1)
        x = frame.cols;
    if(dir == 1) {
        x += speed;
        if(x > (frame.cols*3))
            dir = 0;
    } else if(dir == 0) {
        x -= speed;
        if(x < frame.cols-1)
            dir = 1;
    } else if(dir == 2) {
        x = frame.cols;
        dir = 1;
    }
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(x, frame.rows));
    copyMatSize(frame, copy1, 0, 0);
    AddInvert(frame);
}

void ac::VideoStretchVertical(cv::Mat &frame) {
    static int y = 0, dir = 2;
    static const int speed = 20;
    if(y < frame.rows-1)
        y = frame.rows;
    if(dir == 1) {
        y += speed;
        if(y > (frame.rows*3))
            dir = 0;
    } else if(dir == 0) {
        y -= speed;
        if(y < frame.rows-1)
            dir = 1;
    } else if(dir == 2) {
        y = frame.rows;
        dir = 1;
    }
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(frame.cols, y));
    copyMatSize(frame, copy1, 0, 0);
    AddInvert(frame);
}

void ac::StrobeTrailsFilter(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static int index = 0;
    for(int i = 0; i < 16;  i += 2) {
        cv::Mat &cp = collection.frames[i];
        cv::Mat copy_c = cp.clone();
        setChannelToValue(copy_c, index, 255);
        ++index;
        if(index > 2)
            index = 0;
        AlphaBlendDouble(copy1, copy_c, copy2, 0.6, 0.4);
        copy1 = copy2.clone();
    }
    AlphaBlend(frame, copy1, copy2,0.5);
    frame = copy2.clone();
    AddInvert(frame);
}

void ac::ShadowAlphaTrails16(cv::Mat &frame) {
    static int index = 1;
    static MatrixCollection<16> collection;
    ShadowTrails(frame, &collection, index);
    AddInvert(frame);
}

void ac::ShadowAlphaTrailsReset(cv::Mat &frame) {
    static int index = 1;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat &copy1 = collection.frames[index];
    cv::Mat copy2 = frame.clone();
    AlphaBlend(copy2, copy1, frame, 0.5);
    ++index;
    if(index >= 15) {
        index = 1;
    }
    AddInvert(frame);
}

void ac::SetColormap(cv::Mat &frame) {
    static int index = 2;
    static int counter = 0;
    static MatrixCollection<16> collection;
    int fps = static_cast<int>(ac::fps);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ++counter;
    setColorMap(index, copy2);
    Smooth(copy2, &collection);
    if((counter%fps) == 0) {
        ++index;
        if(index > 10)
            index = 2;
    }
    AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    AddInvert(frame);
}

void ac::ShadowAlphaTrails(cv::Mat &frame) {
    static constexpr int size_val = 32;
    static int index = 1;
    static MatrixCollection<size_val> collection;
    ShadowTrails(frame, &collection, index);
    AddInvert(frame);
}

void ac::ShadowAlphaTrails64(cv::Mat &frame) {
    static constexpr int size_val = 64;
    static int index = 1;
    static MatrixCollection<size_val> collection;
    ShadowTrails(frame, &collection, index);
    AddInvert(frame);
}

void ac::MirrorLeft(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int halfway = (frame.cols/2);
    for(int z = 0; z < frame.rows; ++z) {
        int start = 0;
        for(int i = halfway; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z,((halfway)-start));
            ASSERT(halfway-start > 0);
            pixel = pix;
            ++start;
        }
    }
    AddInvert(frame);
}

void ac::MirrorRight(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int halfway = (frame.cols/2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < halfway; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            ASSERT(frame.cols-i-1 > 0);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::MirrorBackAndForth(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        MirrorLeft(frame);
        index = 1;
    } else {
        MirrorRight(frame);
        index = 0;
    }
    AddInvert(frame);
}

void ac::FadeFromColorToColor(cv::Mat &frame) {
    static cv::Vec3b new_value(rand()%255, rand()%255, rand()%255), old_color(rand()%255, rand()%255, rand()%255);
    static int speed = 10;
    for(int j = 0; j < 3; ++j) {
        if(new_value[j]+15 >= old_color[j] && new_value[j]-15 <= old_color[j])
        {
            old_color[j] = new_value[j];
            new_value[j] = rand()%255;
            continue;
        }
        if(new_value[j] > old_color[j]) {
            old_color[j] += speed;
        } else if(new_value[j] < old_color[j]) {
            old_color[j] -= speed;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += old_color[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::FadeFromColorToColorImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static cv::Vec3b new_value(rand()%255, rand()%255, rand()%255), old_color(rand()%255, rand()%255, rand()%255);
    static int speed = 1;
    for(int j = 0; j < 3; ++j) {
        if(new_value[j]+15 >= old_color[j] && new_value[j]-15 <= old_color[j])
        {
            old_color[j] = new_value[j];
            new_value[j] = rand()%255;
            ++speed;
            if(speed > 10)
                speed = 1;
            continue;
        }
        if(new_value[j] > old_color[j]) {
            old_color[j] += speed;
        } else if(new_value[j] < old_color[j]) {
            old_color[j] -= speed;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>((pix[j]+old_color[j])*0.5);
            }
        }
    }
    AddInvert(frame);
}

void ac::Random_Filter(cv::Mat &frame) {
    randomFilter(frame);
    AddInvert(frame);
}

void ac::FlipY_Axis(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, 1);
    AddInvert(frame);
}

void ac::FlipX_Axis(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, 0);
    AddInvert(frame);
}

void ac::FlipBoth(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, -1);
    AddInvert(frame);
}

void ac::FlipMirrorAlphaBlend(cv::Mat &frame) {
    cv::Mat copies[2];
    copies[0] = frame.clone();
    copies[1] = frame.clone();
    FlipY_Axis(copies[0]);
    FlipX_Axis(copies[1]);
    cv::Mat output[2];
    AlphaBlendDouble(copies[0], frame, output[0], 0.5, 0.5);
    AlphaBlendDouble(copies[1], frame, output[1], 0.5, 0.5);
    AlphaBlend(output[0], output[1], frame, 0.5);
    AddInvert(frame);
}

void ac::Random_FilterX2(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Random_Filter(copy1);
    Random_Filter(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::Random_FilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "Random_FilterSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Random_Filter(copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwineRow480pX2(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(640, 480));
    static MatrixCollection<480> collection;
    IntertwineRows(sizef, &collection, 2);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}


void ac::LowFi(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(copy1, reimage, cv::Size(160, 120));
    ac_resize(reimage, frame, frame.size());
    AddInvert(frame);
}

void ac::HighToLow(cv::Mat &frame) {
    static int div_by = 2;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(copy1, reimage, cv::Size(frame.cols/div_by, frame.rows/div_by));
    ac_resize(reimage, frame, frame.size());
    static int dir = 1;
    if(dir == 1) {
        ++div_by;
        if(div_by > 128) {
            div_by = 128;
            dir = 0;
        }
    } else {
        --div_by;
        if(div_by <= 2) {
            div_by = 2;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::LowToHigh(cv::Mat &frame) {
    static int div_by = 128;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(copy1, reimage, cv::Size(frame.cols/div_by, frame.rows/div_by));
    ac_resize(reimage, frame, frame.size());
    static int dir = 0;
    if(dir == 1) {
        ++div_by;
        if(div_by > 128) {
            div_by = 128;
            dir = 0;
        }
    } else {
        --div_by;
        if(div_by <= 2) {
            div_by = 2;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MoveHighAndLow(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    LowToHigh(copy1);
    HighToLow(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::StretchSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "StretchSubFilter")
        return;
    static int w = 2, h = 2, speed = 5, dir = 100;
    if(dir == 1) {
        w += speed;
        h += speed;
        if(w > (frame.cols-1) || (h > frame.rows-1)) {
            dir = 0;
            w = frame.cols-2;
            h = frame.rows-2;
        }
    } else {
        w -= speed;
        h -= speed;
        if(w <= 64 || h <= 64) {
            dir = 1;
            w = 64;
            h = 64;
        }
    }
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(frame, copy1, cv::Size(w, h));
    CallFilter(subfilter, copy1);
    ac_resize(copy1, reimage, frame.size());
    AlphaBlend(copy2, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::Quality480(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    ac_resize(frame, copy1, cv::Size(640, 480));
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}

void ac::Quality720(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    ac_resize(frame, copy1, cv::Size(1280, 720));
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}
void ac::Quality1080(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    ac_resize(frame, copy1, cv::Size(1920, 1080));
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}

void ac::StretchVerticalMirror(cv::Mat &frame) {
    static int  h = 4, speed = 75, dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(copy1, reimage, cv::Size(copy1.cols, h));
    MirrorXorAll(reimage);
    ac_resize(reimage, copy2, frame.size());
    AlphaBlendDouble(copy1, copy2, frame, 0.7, 0.3);
    AddInvert(frame);
    if(dir == 1) {
        h += speed;
        if(h > frame.rows-2) {
            dir = 0;
        }
    } else if(dir == 0) {
        h -= speed;
        if(h <= 64) {
            dir = 1;
            h = 64;
        }
    }
}

void ac::ImageLoFi(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1, reimage;
    ac_resize(blend_image, reimage, cv::Size(640, 360));
    ac_resize(reimage, frame, frame.size());
    AddInvert(frame);
}

void ac::ImageLofiAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ImageLoFi(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageAlphaBlendScale(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    HighToLow(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::FrameStretchAlphaBlend(cv::Mat &frame) {
    static int w = 2, h = 2, speed = 25, dir = 25;
    if(dir == 1) {
        if(w < frame.cols) w += speed;
        if(h < frame.rows) h += speed;
        if(w > (frame.cols-64) && (h > frame.rows-64)) {
            dir = 0;
            w = frame.cols-64;
            h = frame.rows-64;
        }
    } else {
        if(w > 64) w -= speed;
        if(h > 64) h -= speed;
        if(w < 64 &&  h < 64) {
            dir = 1;
            w = 64;
            h = 64;
        }
    }
    static double alpha = 1.0, alpha_max =3.0;
    static int dir1 = 1;
    procPos(dir1, alpha, alpha_max);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(frame, copy1, cv::Size(w, h));
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows && i < copy1.cols && z < copy1.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j] * alpha) + static_cast<unsigned char>(pix[j] * alpha);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::BlurHighToLow(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    HighToLow(frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::ImageHighToLowAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    HighToLow(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorTopToBottom(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int halfway = (frame.rows/2);
    for(int i = 0; i < frame.cols; ++i) {
        int start = 0;
        for(int z = halfway; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(((halfway)-start), i);
            ASSERT(halfway-start > 0);
            pixel = pix;
            ++start;
        }
    }
    AddInvert(frame);
    
}

void ac::MirrorBottomToTop(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int halfway = (frame.rows/2);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < halfway; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            ASSERT(frame.rows-z-1 > 0);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::FlashRGB_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "FlashRGB_SubFilter")
        return;
    CallFilter(subfilter, frame);
    static int index = 0;
    switch(index) {
        case 0:
            AllRed(frame);
            break;
        case 1:
            AllGreen(frame);
            break;
        case 2:
            AllBlue(frame);
            break;
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::MirrorSwitch(cv::Mat &frame) {
    static int index = 0;
    switch(index) {
        case 0:
            MirrorLeft(frame);
            MirrorTopToBottom(frame);
            break;
        case 2:
            MirrorRight(frame);
            MirrorTopToBottom(frame);
            break;
        case 1:
            MirrorLeft(frame);
            MirrorBottomToTop(frame);
            break;
        case 3:
            MirrorRight(frame);
            MirrorBottomToTop(frame);
            break;
    }
    ++index;
    if(index > 3)
        index = 0;
    AddInvert(frame);
}

void ac::MirrorSwitchSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MirrorSwitchSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorSwitch(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::MirrorSwitchFlip(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int rndval = -1;
    rndval += rand()%3;
    if(rndval >= 2)
        return;
    MirrorSwitch(copy1);
    cv::flip(copy1, frame, rndval);
    AddInvert(frame);
}

void ac::BlendImageLayer(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += pix[j];
            }
        }
    }
    AddInvert(frame);
}
void ac::StrobeRandomFilter(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        Random_Filter(frame);
        index = 1;
    } else {
        switch(rand()%2) {
            case 0:
                AllRed(frame);
                break;
            case 1:
                AllGreen(frame);
                break;
            case 2:
                AllBlue(frame);
                break;
        }
        index = 0;
    }
    AddInvert(frame);
}
// Light
void ac::AlphaBlendRandom_Filter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Random_Filter(copy1);
    AlphaBlendDouble(copy1, copy2, frame, 0.3, 0.7);
    AddInvert(frame);
}

void ac::DoubleRandomAlphaImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "DoubleRandomAlphaImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    Random_Filter(copy1);
    Random_Filter(reimage);
    AlphaBlendDouble(copy1, reimage, frame, 0.3, 0.3);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::MirrorLeftMirrorRightBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorTopMirrorBottomBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorTopToBottom(copy1);
    MirrorBottomToTop(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MirrorAll(cv::Mat &frame) {
    cv::Mat copies[4];
    for(int i = 0; i < 4; ++i)
        copies[i] = frame.clone();
    MirrorLeft(copies[0]);
    MirrorRight(copies[1]);
    MirrorTopToBottom(copies[2]);
    MirrorBottomToTop(copies[3]);
    cv::Mat out[2];
    AlphaBlend(copies[0], copies[1], out[0], 0.5);
    AlphaBlend(copies[2], copies[3], out[1], 0.5);
    AlphaBlend(out[0], out[1], frame, 0.5);
    AddInvert(frame);
}

void ac::ImageDiff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            pixel = pixel-pix;
        }
    }
    AddInvert(frame);
}

void ac::ImageDiffSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageDiffSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    ImageDiff(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
    
}

void ac::RestoreBlack(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(orig_frame,z, i);
                if(pix[0] >= 0 && pix[0] <= 2 && pix[1] >= 0 && pix[1] <= 2 && pix[2] >= 0 && pix[2] <= 2)
                    pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::OrigBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "OrigBlendSubFilter" || orig_frame.empty() ||  orig_frame.size() != frame.size())
        return;
    cv::Mat copy1 = frame.clone(), copy2 = orig_frame.clone();
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::OrigAndCurrentRandomX2(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = orig_frame.clone();
    Random_Filter(copy1);
    Random_Filter(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::FillPixelsImage(cv::Mat &frame) {
     if(blend_set == false)
        return;
    if(pix.isInit() == false || pix.resetNeeded()) {
        cv::Mat copy1;
        ac_resize(blend_image, copy1, frame.size());
        pix.reset(copy1);
    }
    pix.setPixel(pix.size()/120);
    pix.drawToMatrix(frame);
    AddInvert(frame);
}

void ac::AverageHorizontalDistortion(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        unsigned int values[3] = {0,0,0};
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] += pixel[j];
            }
        }
        values[0] /= frame.cols;
        values[1] /= frame.cols;
        values[2] /= frame.cols;
        double value = 0.5;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(values[j] * value);
            }
            value += 0.09;
        }
    }
    AddInvert(frame);
}

void ac::AlphaBlendImageWithOrigSource(cv::Mat &frame) {
    if(blend_set == false || orig_frame.empty() || orig_frame.size() != frame.size())
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha1 = 1.0, alpha2 = 2.0;
    static int dir1 = 1, dir2 = 0;
    cv::Mat copy1 = orig_frame.clone(), copy2 = frame.clone(), copy3;
    AlphaBlendDouble(copy1, reimage, copy3, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 2.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 2.0, 1.0);
    AlphaBlend(copy2, copy3, frame, 0.5);
    AddInvert(frame);
}



void ac::resizeFrameWidth(cv::Mat &frame) {
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        if(resize_x > (frame.cols*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        if(resize_x <= frame.cols) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeFrameHeight(cv::Mat &frame) {
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_y += 50;
        if(resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_y -= 50;
        if(resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeFrameWidthAndHeight(cv::Mat &frame) {
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        resize_y += 50;
        if(resize_x >  (frame.cols*6) || resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        resize_y -= 50;
        if(resize_x <= frame.cols || resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

