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
    cv::resize(blend_image, reimage, size_val);
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
    cv::resize(frame, copy1, cv::Size(x, frame.rows));
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
    cv::resize(frame, copy1, cv::Size(frame.cols, y));
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    cv::resize(blend_image, reimage, frame.size());
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>((pix[j]+old_color[j])*0.5);
            }
        }
    }
    AddInvert(frame);
}

void ac::Random_Filter(cv::Mat &frame) {
    std::string value = solo_filter[rand()%solo_filter.size()];
    // uncomment below to see what filter is currently being processed
    // std::cout << "Current Filter: " << value << "\n";
    CallFilter(value, frame);
    AddInvert(frame);
}

void ac::FlipHorizontal(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, 1);
    AddInvert(frame);
}

void ac::FlipVertical(cv::Mat &frame) {
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
    FlipHorizontal(copies[0]);
    FlipVertical(copies[1]);
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
    cv::resize(frame, sizef, cv::Size(640, 480));
    static MatrixCollection<480> collection;
    IntertwineRows(sizef, &collection, 24);
    cv::resize(sizef, frame, frame.size());
    AddInvert(frame);
}


void ac::LowFi(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(copy1, reimage, cv::Size(160, 120));
    cv::resize(reimage, frame, frame.size());
}
