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

void ac::ResizeImageWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
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

void ac::ResizeImageHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(blend_image, copy1, cv::Size(resize_x,resize_y));
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

void ac::ResizeImageWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    ac_resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
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

void ac::ResizeImageAndFrameBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    ResizeImageWidthAndHeight(reimage);
    ResizeFrameWidthAndHeight(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeImageWidthAndHeightSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ResizeImageWidthAndHeightSubFilter" || orig_frame.empty())
        return;
    cv::Mat copy1 = frame.clone(), reimage, copy3;
    ac_resize(blend_image, reimage, frame.size());
    ResizeImageWidthAndHeight(reimage);
    CallFilter(subfilter, reimage);
    ResizeFrameWidthAndHeight(copy1);
    AlphaBlendDouble(copy1,reimage,copy3, 0.3, 0.7);
    AlphaBlend(copy3, orig_frame, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeImageFrameWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    ResizeImageWidth(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeImageFrameHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    ResizeImageHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeImageFrameWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    ResizeImageWidthAndHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeImageRandom(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int index = 0, fcount = 0;
    switch(index) {
        case 0:
            ResizeImageFrameWidth(frame);
            break;
        case 1:
            ResizeImageFrameHeight(frame);
            break;
        case 2:
            ResizeImageFrameWidthAndHeight(frame);
            break;
    }
    AddInvert(frame);
    ++fcount;
    int fps = static_cast<int>(ac::fps);
    if((fcount%fps) == 0) {
        ++index;
        if(index > 2)
            index = 0;
    }
    AddInvert(frame);
}

void ac::ResizeFrameRandom(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static int index = 0, fcount = 0;
    switch(index) {
        case 0:
            ResizeFrameWidth(copy1);
            break;
        case 1:
            ResizeFrameHeight(copy1);
            break;
        case 2:
            ResizeFrameWidthAndHeight(copy1);
            break;
    }
    AddInvert(copy1);
    ++fcount;
    int fps = static_cast<int>(ac::fps);
    if((fcount%fps) == 0) {
        ++index;
        if(index > 2)
            index = 0;
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ResizeFrameImageFlash(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int offset = rand()%9;
    switch(offset) {
        case 0:
            ResizeFrameWidth(copy1);
            break;
        case 1:
            ResizeFrameHeight(copy1);
            break;
        case 2:
            ResizeFrameWidthAndHeight(copy1);
            break;
        case 3:
            ResizeImageFrameWidth(copy1);
            break;
        case 4:
            ResizeImageFrameHeight(copy1);
            break;
        case 5:
            ResizeImageFrameWidthAndHeight(copy1);
            break;
        case 6:
            ResizeImageWidth(copy1);
            break;
        case 7:
            ResizeImageWidth(copy1);
            break;
        case 8:
            ResizeImageWidthAndHeight(copy1);
            break;
            
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::RandomFlip(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int rt_val = -1;
    rt_val += rand()%4;
    if(rt_val >= 2) return;
    cv::flip(copy1, frame, rt_val);
    AddInvert(frame);
}

void ac::ColorsFilter(cv::Mat &frame) {
    static std::vector<std::string> filter_array({"ColorExpand", "ColorRange", "ColorTransition","ColorTransitionRandom","ColorXorScale", "Rainbow Blend", "RainbowXorBlend", "ColorChannelMoveUpAndDown"});
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
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

void ac::MirrorDoubleVision(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ColorFadeFilter(cv::Mat &frame) {
    static int rgb_start[] = {rand()%255,rand()%255,rand()%255};
    static int speed = 5;
    static int dir[3] = {1, 0, 1};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rgb_start[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_start[j] += speed;
            if(rgb_start[j] >= 255) {
                dir[j] = 0;
                rgb_start[j] = rand()%255;
            }
        } else if(dir[j] == 0) {
            rgb_start[j] -= speed;
            if(rgb_start[j] <= 0) {
                dir[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorChannelMoveUpAndDown(cv::Mat &frame) {
    int rgb_values[3] = {rand()%2255, rand()%255, rand()%255};
    int rgb_stop[3] = {rand()%255, rand()%255, rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += rgb_values[j];
            
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(rgb_values[j] > rgb_stop[j]) {
            --rgb_values[j];
            if(rgb_values[j] == rgb_stop[j]) {
                rgb_stop[j] = rgb_values[j];
                rgb_values[j] = rand()%255;
            }
        } else if(rgb_values[j] < rgb_stop[j]) {
            ++rgb_values[j];
            if(rgb_values[j] == rgb_stop[j]) {
                rgb_stop[j] = rgb_values[j];
                rgb_values[j] = rand()%255;
            }
        }
    }
    AddInvert(frame);
}

void ac::MedianStrobe(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        MedianBlend(frame);
        index = 1;
    } else {
        MedianBlendDark(frame);
        index = 0;
    }
    AddInvert(frame);
}

void ac::DifferenceSubFilter(cv::Mat &frame) {
    if(orig_frame.empty() || orig_frame.size() != frame.size() || subfilter == -1 || draw_strings[subfilter] == "DifferenceSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = pixelAt(orig_frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] -= (pix[j]);
        }
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::AlphaBlendFlippedFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static double alpha1 = 0.5, alpha2 = 2.0;
    static int dir1 = 1, dir2 = 0;
    FlipX_Axis(copy1);
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 2.0, 0.5);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 2.0, 0.5);
    AddInvert(frame);
}

void ac::ColorVariableBlend(cv::Mat &frame) {
    static int rgb_values[3] = {rand()%255, rand()%255, rand()%255};
    static int dir[3] = {1,1,1};
    for(int z = 0; z <  frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rgb_values[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_values[j] += 10;
            if(rgb_values[j] > 255) {
                rgb_values[j] = 255;
                dir[j] = 0;
            }
        } else {
            rgb_values[j] -= 10;
            if(rgb_values[j] <= 0) {
                rgb_values[j] = 0;
                dir[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorXorBlend(cv::Mat &frame) {
    static int rgb_values[3] = {0,0,0};
    static int dir[3] = {1,1,1};
    static int speed = 10;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^rgb_values[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_values[j] += rand()%speed;
            if(rgb_values[j] > 255) {
                rgb_values[j] = 255;
                dir[j] = 0;
                ++speed;
                if(speed > 25)
                    speed = 10;
            }
        } else {
            rgb_values[j] -= rand()%speed;
            if(rgb_values[j] <= 0) {
                rgb_values[j] = 0;
                dir[j] = 1;
                ++speed;
                if(speed > 25)
                    speed = 10;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorAddBlend(cv::Mat &frame) {
    static int rgb_values[3] = {rand()%255,rand()%255,rand()%255};
    static int dir[3] = {1,1,1};
    static int speed = 10;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rgb_values[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_values[j] += rand()%speed;
            if(rgb_values[j] > 255) {
                rgb_values[j] = 255;
                dir[j] = 0;
                ++speed;
                if(speed > 25)
                    speed = 10;
            }
        } else {
            rgb_values[j] -= rand()%speed;
            if(rgb_values[j] <= 0) {
                rgb_values[j] = 0;
                dir[j] = 1;
                ++speed;
                if(speed > 25)
                    speed = 10;
            }
        }
    }
    AddInvert(frame);
}

void ac::IntertwineRowsShadow720p(cv::Mat &frame) {
    static constexpr int row_size = 720;
    static MatrixCollection<row_size> collection;
    IntertwineRowsInter(frame,&collection,2);
    AddInvert(frame);
}

void ac::IntertwineRowsAlpha720p(cv::Mat &frame) {
    static constexpr int row_size = 720;
    static MatrixCollection<row_size> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRowsInter(copy1,&collection,2);
    AlphaBlendDouble(copy1, copy2, frame, 0.7, 0.3);
    AddInvert(frame);
}

void ac::IntertwineByFrameDown(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int index = collection.size()-1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^pix[j];
            }
        }
        --index;
        if(index <= 1)
            index = collection.size()-1;
    }
    AddInvert(frame);
}

void ac::IntertwineByFrameUp(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int index = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^pix[j];
            }
        }
        ++index;
        if(index > collection.size()-1)
            index = 1;
    }
    AddInvert(frame);
}

void ac::IntertwineFrameFast(cv::Mat &frame) {
    static constexpr int row_size = 128;
    static MatrixCollection<row_size> collection;
    collection.shiftFrames(frame);
    static int index = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pix[j];
            }
        }
        ++index;
        if(index > collection.size()-1)
            index = 1;
    }
    AddInvert(frame);
}

void ac::IntertwineFrame360(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(640, 360));
    collection.shiftFrames(copy1);
    int index = 0;
    
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            if(pixel == pix) {
                pixel = collection.frames[1].at<cv::Vec3b>(z, i);
            } else {
                pixel = pix;
            }
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineFrame360X(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    static int dir = 1, size_value = 1;
    IntertwineResizeRowX(frame, &collection, 640, 360, dir, size_value);
    AddInvert(frame);
}

void ac::IntertwineFrameTwitch(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(640, 360));
    collection.shiftFrames(copy1);
    int index = 1;
    static int size_value = 1;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
            ++index;
            if(index > size_value) {
                index = 0;
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineFrameDark(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    cv::Mat copy1;
    cv::Mat copy2 = frame.clone();
    ac_resize(frame, copy1, cv::Size(640, 360));
    ColorVariableBlend(copy1);
    MedianBlend(copy1);
    collection.shiftFrames(copy1);
    int index = 1;
    static int size_value = 1;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
            ++index;
            if(index > size_value) {
                index = 0;
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    cv::Mat reframe;
    ac_resize(copy1, reframe, frame.size());
    AlphaBlend(copy2, reframe, frame, 0.5);
    MedianBlendDark(frame);
    AddInvert(frame);
}

void ac::IntertwineFrame360_Reverse(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(640, 360));
    cv::Mat copyf = copy1.clone();
    static int off_x = 0;
    off_x = (off_x == 0) ? 1 : 0;
    if(off_x == 0) {
        FlipBoth(copyf);
    }
    collection.shiftFrames(copyf);
    int index = 1;
    static int size_value = 1;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        ++index;
        if(index > size_value) {
            index = 0;;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    ac_resize(copy1, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineFrameBlend(cv::Mat &frame) {
    static MatrixCollection<360> collection;
    cv::Mat copy1;
    ac_resize(frame, copy1, cv::Size(640, 360));
    collection.shiftFrames(copy1);
    int index = 1;
    static int size_value = 1;
    for(int z = 0; z < copy1.rows; ++z) {
        for(int i = 0; i < copy1.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel += pix;
        }
        ++index;
        if(index > size_value) {
            index = 0;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    ac_resize(copy1,frame,frame.size());
    AddInvert(frame);
}

void ac::IntertwineXorCollection(cv::Mat &frame) {
    static MatrixCollection<96> collection;
    collection.shiftFrames(frame);
    int index = 0;
    static int size_value = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^pix[j];
            }
        }
        ++index;
        if(index > size_value) {
            index = 1;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    AddInvert(frame);
}

void ac::IntertwineFrame720X(cv::Mat &frame) {
    static MatrixCollection<720> collection;
    static int dir = 1, size_value = 1;
    IntertwineResizeRowX(frame, &collection, 1280, 720, dir, size_value);
    AddInvert(frame);
}

void ac::IntertwineFrame1080X(cv::Mat &frame) {
    static MatrixCollection<1080> collection;
    static int dir = 1, size_value = 1;
    IntertwineResizeRowX(frame, &collection, 1920, 1080, dir, size_value);
    AddInvert(frame);
}

void ac::IntertwineFrameImage1080X(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Size orig_size = frame.size();
    static constexpr int row_size_h = 1080, row_size_w = 1920;
    static MatrixCollection<row_size_h> collection;
    cv::Mat reframe, reimage;
    cv::resize(frame, reframe, cv::Size(row_size_w, row_size_h));
    cv::resize(blend_image, reimage, reframe.size());
    collection.shiftFrames(reframe);
    int index = 1;
    static int size_value = 1;
    for(int z = 0; z < reframe.rows; ++z) {
        for(int i = 0; i < reframe.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(reframe,z, i);
            if((z%2) == 0) {
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.5) + (pix[j]*0.5));
                }
            } else {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++index;
        if(index > size_value) {
            index = 0;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++size_value;
        if(size_value > collection.size()-1) {
            size_value = collection.size()-1;
            dir = 0;
            index = 1;
        }
    } else if(dir == 0) {
        --size_value;
        if(size_value <= 1) {
            dir = 1 ;
            index = 1;
        }
    }
    cv::resize(reframe, frame, orig_size);
    BlendWithImage(frame);
    BlendWithSource(frame);
    AddInvert(frame);
}


void ac::RandomXorMultiThreadTest(cv::Mat &frame) {
    cv::Vec3b rand_color(rand()%255, rand()%255, rand()%255);
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = pixel[j]^rand_color[j];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SelfAlphaBlendMultiThread(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
            }
        }
    };
    ac::AlphaMovementMaxMin(alpha,dir, 0.09, 6.0, 1.0);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection.size(); ++j) {
                    cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        value[q] += pixel[q];
                    }
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int val = 1+static_cast<int>(value[j]);
                    pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BytePixelSort(cv::Mat &frame) {
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            std::vector<unsigned char> bytes;
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    bytes.push_back(pixel[j]);
            }
            std::sort(bytes.begin(), bytes.end());
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = bytes[i+j];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}


void ac::SortedImageColorVariable(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha1 = 1.0, alpha2 = 3.0;
    static int dir1 = 1, dir2 = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            std::vector<unsigned char> bytes;
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b pixel1 = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pixel2 = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    bytes.push_back(pixel1[j]);
                    bytes.push_back(pixel2[j]);
                }
            }
            std::sort(bytes.begin(), bytes.end());
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0;j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha1)+static_cast<unsigned char>((alpha2*bytes[i+j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    //ColorVariableBlend(frame);
    AlphaMovementMaxMin(alpha1, dir1, 0.09, 3.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.01, 3.0, 1.0);
    AddInvert(frame);
}

void ac::SmoothColorVariableImageBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    SortedImageColorVariable(frame);
    SmoothFrame32(frame);
    ColorVariableBlend(frame);
    MedianBlendMultiThread(frame);
}

void ac::BlendChannelXor(cv::Mat &frame) {
    static cv::Vec3b color_value(rand()%100, rand()%100, rand()%100);
    int cur_color[3] = {rand()%100, rand()%100, rand()%100};
    static double alpha1 = 1.0;
    static int dir1 = 1;
    static int index = 0;
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha1) ^ (cur_color[j]);
                }
            }
        }
    };
    ++cur_color[index];
    if((cur_color[index] > 255) || (cur_color[index] == color_value[index])) {
        color_value[index] = cur_color[index];
        cur_color[index] = rand()%255;
        ++index;
        if(index > 2)
            index = 0;
    }
    AlphaMovementMaxMin(alpha1, dir1, 0.09, 3.0, 1.0);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

