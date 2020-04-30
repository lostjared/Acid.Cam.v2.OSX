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

void ac::DarkNegateRainbowMedian(cv::Mat &frame) {
    static MatrixCollection<8> collection1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    DarkNegate(copy1);
    SmoothRGB(copy1, &collection1);
    rainbowBlend(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::IncreaseQuick(cv::Mat &frame) {
    static double speed = 1.0;
    static double pixel_color[] = {0.1*(rand()%10), (0.1*(rand()%10)), (0.1*(rand()%10))};
    static int dir = 1, speed_dir = 1;
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(pixel[j]*pixel_color[j]);
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir == 1) {
            pixel_color[j] += speed;
            if(pixel_color[j] >= 5) {
                dir = 0;
            }
        } else {
            pixel_color[j] -= speed;
            if(pixel_color[j] <= 1.0) {
                dir = 1;
            }
        }
    }
    if(speed_dir == 1) {
        speed += 0.1;
        if(speed > 2.0) {
            speed_dir = 0;
            for(int j = 0; j < 3; ++j) {
                pixel_color[j] = 3.0;
            }
        }
    } else {
        speed -= 0.1;
        if(speed <= 1.0) {
            speed_dir = 1;
            for(int j = 0; j < 3; ++j) {
                pixel_color[j] = 1.0;
            }
        }
    }
}


void ac::IncreaseRandomIndex(cv::Mat &frame) {
    static int index = rand()%2, speed = 10;
    static int colors[3] = {0,75,150};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] += colors[index];
        }
    }
    for(int j = 0; j < 3; ++j ){
        colors[j] += speed;
        if(colors[j] >= 255) {
            colors[j] = 0;
            ++index;
            if(index > 2)
                index = rand()%2;
        }
    }
    AddInvert(frame);
}

void ac::ImageChannelSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageChannelSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    CallFilter(subfilter, copy1);
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    cv::Mat chan[3], output;
    static int index = 0;
    static int values[3] = {0,1,2};
    switch(index) {
        case 0:
            values[0] = 0;
            values[1] = 1;
            values[2] = 2;
            break;
        case 1:
            values[0] = 1;
            values[1] = 2;
            values[2] = 0;
            break;
        case 2:
            values[0] = 2;
            values[1] = 0;
            values[2] = 1;
            break;
    }
    ++index;
    if(index > 2) index = 0;
    cv::extractChannel(copy1, chan[values[0]], 0);
    cv::extractChannel(reimage, chan[values[1]],1);
    cv::extractChannel(copy2, chan[values[2]], 2);
    cv::merge(chan, 3, output);
    AlphaBlend(output, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ImageChannelChangeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageChannelChangeSubFilter")
        return;
    static int index = 0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, frame);
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b repixel = reimage.at<cv::Vec3b>(z, i);
            pixel[index] = repixel[index];
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::ImageChannelRandom(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int index = 0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    cv::Mat chan[3], output;
    static int values[3] = {0,1,2};
    switch(index) {
        case 0:
            values[0] = 0;
            values[1] = 1;
            values[2] = 2;
            break;
        case 1:
            values[0] = 1;
            values[1] = 2;
            values[2] = 0;
            break;
        case 2:
            values[0] = 2;
            values[1] = 0;
            values[2] = 1;
            break;
    }
    cv::extractChannel(reimage, chan[0], values[0]);
    cv::extractChannel(reimage, chan[1], values[1]);
    cv::extractChannel(reimage, chan[2], values[2]);
    cv::merge(chan, 3, output);
    AlphaBlend(copy1, reimage, frame, 0.5);
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index > 2) {
            index = 2;
            dir = 0;
        }
    } else {
        --index;
        if(index < 0) {
            index = 0;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::ImageChannelRandomSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageChannelRandomSubFilter")
        return;
    
    static int index = 0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    cv::Mat chan[3], output;
    static int values[3] = {0,1,2};
    switch(index) {
        case 0:
            values[0] = 0;
            values[1] = 1;
            values[2] = 2;
            break;
        case 1:
            values[0] = 1;
            values[1] = 2;
            values[2] = 0;
            break;
        case 2:
            values[0] = 2;
            values[1] = 0;
            values[2] = 1;
            break;
    }
    CallFilter(subfilter, copy1);
    cv::extractChannel(reimage, chan[0], values[0]);
    cv::extractChannel(reimage, chan[1], values[1]);
    cv::extractChannel(reimage, chan[2], values[2]);
    cv::merge(chan, 3, output);
    AlphaBlend(copy1, reimage, frame, 0.5);
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index > 2) {
            index = 2;
            dir = 0;
        }
    } else {
        --index;
        if(index < 0) {
            index = 0;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::PixelateBlur(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MedianBlur(copy1);
    Block(copy1);
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    MedianBlur(frame);
    AlphaMovementMaxMin(alpha1,dir1,0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2,dir2,0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::PixelateBlock(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ParticleRelease(copy1);
    Block(copy1);
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    AlphaBlendDouble(copy1, copy2, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1,dir1,0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2,dir2,0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::PixelateNoResize12(cv::Mat &frame) {
    Pixelate(frame, 12);
    AddInvert(frame);
}

void ac::PixelateNoResize8(cv::Mat &frame) {
    Pixelate(frame, 8);
    AddInvert(frame);
}

void ac::PixelateNoResize32(cv::Mat &frame) {
    Pixelate(frame, 32);
    AddInvert(frame);
}

void ac::PixelateNoResize16(cv::Mat &frame) {
    Pixelate(frame, 16);
    AddInvert(frame);
}

void ac::PixelateNoResize24(cv::Mat &frame) {
    Pixelate(frame, 24);
    AddInvert(frame);
}

void ac::PixelateBlurFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int r = 3+(rand()%7);
    for(int i = 0; i < r; ++i) {
        MedianBlur(copy1);
    }
    Pixelate(copy1,16);
    AlphaBlend(copy1, copy2, frame, 0.5);
}

void ac::PixelateBlurSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "PixelateBlurSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    int r = 3+(rand()%7);
    for(int i = 0; i < r; ++i)
        MedianBlur(copy1);
    Pixelate(copy1, 12);
    AlphaBlend(copy1, copy2, frame, 0.5);
}


void ac::MirrorXorImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage, copy1;
    copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values[8];
            values[0] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values[1] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values[2] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[3] = reimage.at<cv::Vec3b>(z, i);
            values[4] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values[5] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values[6] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 7; ++q)
                    pixel[j] ^= values[q][j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MirrorXorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MirrorXorSubFilter")
        return;
    cv::Mat reimage, copy1;
    copy1 = frame.clone();
    reimage = frame.clone();
    CallFilter(subfilter, reimage);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values[8];
            values[0] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values[1] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values[2] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[3] = reimage.at<cv::Vec3b>(z, i);
            values[4] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values[5] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values[6] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 7; ++q)
                    pixel[j] ^= values[q][j];
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelXorImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "PixelXorImageSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone(), copy2 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, frame);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values[8];
            values[0] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values[1] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values[2] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[3] = reimage.at<cv::Vec3b>(z, i);
            values[4] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values[5] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values[6] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 4; ++q)
                    pixel[j] ^= values[q][j];
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelAlphaImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "PixelAlphaImageSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone(), copy2 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values1[4], values2[4],values3[4];
            values1[0] = copy1.at<cv::Vec3b>(z, i);
            values1[1] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values1[2] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values1[3] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values2[0] = reimage.at<cv::Vec3b>(z, i);
            values2[1] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values2[2] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values2[3] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            values3[0] = copy2.at<cv::Vec3b>(z, i);
            values3[1] = copy2.at<cv::Vec3b>(frame.rows-z-1, i);
            values3[2] = copy2.at<cv::Vec3b>(z, frame.cols-i-1);
            values3[3] = copy2.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 4; ++q) {
                    pixel[j] += static_cast<unsigned char>((values1[q][j] + values2[q][j] + values3[q][j]) * 0.1);
                }
            }
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::PixelScaleImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "PixelScaleImageSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone(), copy2 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy2);
    static double alpha = 0.1;
    static int dir = 1;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values1[4], values2[4],values3[4];
            values1[0] = copy1.at<cv::Vec3b>(z, i);
            values1[1] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values1[2] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values1[3] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values2[0] = reimage.at<cv::Vec3b>(z, i);
            values2[1] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values2[2] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values2[3] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            values3[0] = copy2.at<cv::Vec3b>(z, i);
            values3[1] = copy2.at<cv::Vec3b>(frame.rows-z-1, i);
            values3[2] = copy2.at<cv::Vec3b>(z, frame.cols-i-1);
            values3[3] = copy2.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 4; ++q) {
                    pixel[j] += static_cast<unsigned char>((values1[q][j] + values2[q][j] + values3[q][j]) * alpha);
                }
            }
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha,dir,0.005, 0.5, 0.1);
}

void ac::PixelImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "PixelImageSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone(), copy2 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values1[4], values2[4],values3[4];
            values1[0] = copy1.at<cv::Vec3b>(z, i);
            values1[1] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values1[2] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values1[3] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values2[0] = reimage.at<cv::Vec3b>(z, i);
            values2[1] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values2[2] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values2[3] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            values3[0] = copy2.at<cv::Vec3b>(z, i);
            values3[1] = copy2.at<cv::Vec3b>(frame.rows-z-1, i);
            values3[2] = copy2.at<cv::Vec3b>(z, frame.cols-i-1);
            values3[3] = copy2.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 4; ++q) {
                    pixel[j] += static_cast<unsigned char>((values1[q][j] + values2[q][j] + values3[q][j]) * 0.1);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelImageTex(cv::Mat &frame) {
    if(blend_set == false)
        return;
    pushSubFilter(getFilterByName("ColorExpand"));
    PixelImageSubFilter(frame);
    popSubFilter();
    BlendWithSource(frame);
    MedianBlend(frame);
}

void ac::PixelImageXorSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "PixelImageXorSubFilter")
        return;
    cv::Mat reimage, copy1 = frame.clone(), copy2 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy2);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b values1[4], values2[4],values3[4];
            values1[0] = copy1.at<cv::Vec3b>(z, i);
            values1[1] = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            values1[2] = copy1.at<cv::Vec3b>(z, frame.cols-i-1);
            values1[3] = copy1.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values2[0] = reimage.at<cv::Vec3b>(z, i);
            values2[1] = reimage.at<cv::Vec3b>(reimage.rows-z-1, i);
            values2[2] = reimage.at<cv::Vec3b>(z, reimage.cols-i-1);
            values2[3] = reimage.at<cv::Vec3b>(reimage.rows-z-1, reimage.cols-i-1);
            values3[0] = copy2.at<cv::Vec3b>(z, i);
            values3[1] = copy2.at<cv::Vec3b>(frame.rows-z-1, i);
            values3[2] = copy2.at<cv::Vec3b>(z, frame.cols-i-1);
            values3[3] = copy2.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                for(int q = 0; q < 4; ++q) {
                    pixel[j] ^= static_cast<unsigned char>((values1[q][j] + values2[q][j] + values3[q][j]) * 0.1);
                }
            }
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::PixelRowMedianBlend(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        cv::Scalar scale;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                scale[j] += pixel[j] * 0.1;
            }
        }
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                int value = static_cast<int>(scale[j]);
                pixel[j] += static_cast<unsigned char>((pixel[j]^value) * 0.5);
            }
        }
    }
    BlendWithSource(frame);
    BlendWithSource(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::IntertwineRows32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::IntertwineRows16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::IntertwineRows8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::IntertwineAlpha(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    static double alpha = 1.0;
    static int dir = 1;
    IntertwineRowsOperation(frame, &collection, [&](cv::Vec3b &pixel, cv::Vec3b value) {
        for(int j = 0; j < 3; ++j) {
            pixel[j] = static_cast<unsigned char>((pixel[j]+value[j])*alpha);
        }
    });
    AlphaMovementMaxMin(alpha, dir, 0.01, 2.0, 1.0);
    AddInvert(frame);

}



void ac::IntertwineRowsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineSubFilter")
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    IntertwineRows(copy1, &collection1);
    CallFilter(subfilter, copy2);
    IntertwineRows(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}


void ac::IntertwineRows4(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}


void ac::Intertwine64x4(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    IntertwineRows(frame, &collection, 4);
    AddInvert(frame);
}

void ac::Intertwine64X(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int dir = 1, index = 4;
    IntertwineRows(frame, &collection, index);
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::Intertwine64XSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "Intertwine64XSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Intertwine64X(copy1);
    pushSubFilter(subfilter);
    SmoothSubFilter32(copy1);
    popSubFilter();
    Intertwine64X(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
    
}

void ac::Intertwine64XPixel(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int dir = 0, index = 62;
    if(dir == 1) {
        ++index;
        if(index > 62) {
            dir = 0;
        }
    } else {
        --index;
        if(index <= 1)
            dir = 1;
    }
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Intertwine64X(copy1);
    collection.shiftFrames(copy1);
    PixelateNoResize16(copy2);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwinePixels(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    PixelateNoResize16(copy1);
    IntertwineRows(copy1, &collection, 32);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwineColsX(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int dir = 1, index = 4;
    IntertwineCols(frame, &collection, index);
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::IntertwineCols16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    IntertwineCols(frame, &collection);
    AddInvert(frame);
}

void ac::IntertwineCols8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    IntertwineCols(frame, &collection);
    AddInvert(frame);
}
void ac::IntertwineCols32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    IntertwineCols(frame, &collection);
    AddInvert(frame);
}

void ac::MirrorIntertwine(cv::Mat &frame) {
    IntertwineRows32(frame);
    DarkenFilter(frame);
    MirrorXorAll(frame);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MirrorIntertwineImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage, frame, 0.5);
    MirrorIntertwine(frame);
    AddInvert(frame);
}

void ac::MirrorIntertwineImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "MirrorIntertwineImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage, frame, 0.5);
    CallFilter(subfilter, frame);
    MirrorIntertwine(frame);
    AddInvert(frame);
}

void ac::IntertwineImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage, cp1, cp2;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage, cp1, 0.5);
    CallFilter(subfilter, cp1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, cp2, 0.5);
    AlphaBlend(cp1, cp2, frame, 0.5);
    MirrorIntertwine(frame);
    AddInvert(frame);
}

void ac::BlendWithImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwineRowsReverse32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    IntertwineRowsReverse(frame, &collection, 32);
    AddInvert(frame);
}

void ac::IntertwineRowsReverse16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    IntertwineRowsReverse(frame, &collection, 16);
    AddInvert(frame);

}
void ac::IntertwineRowsReverse8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    IntertwineRowsReverse(frame, &collection, 8);
    AddInvert(frame);
}

void ac::IntertwineRowsReverse64X(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int dir = 1, index = 4;
    IntertwineRowsReverse(frame, &collection, index);
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::IntertwineRowsSwitch32(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        index = 1;
        IntertwineRows32(frame);
    } else {
        index = 0;
        IntertwineRowsReverse32(frame);
    }
    AddInvert(frame);
}

void ac::IntertwineRowsSwitch16(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        index = 1;
        IntertwineRows16(frame);
    } else {
        index = 0;
        IntertwineRowsReverse16(frame);
    }
    AddInvert(frame);
}
void ac::IntertwineRowsSwitch8(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        index = 1;
        IntertwineRows8(frame);
    } else {
        index = 0;
        IntertwineRowsReverse8(frame);
    }
    AddInvert(frame);
}

void ac::IntertwineRows64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    IntertwineRows(frame, &collection, 64);
    AddInvert(frame);
}

void ac::IntertwineRowsReverse64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    IntertwineRowsReverse(frame, &collection, 64);
    AddInvert(frame);
}
void ac::IntertwineRowsSwitch64(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        index = 1;
        IntertwineRows64(frame);
    } else {
        index = 0;
        IntertwineRowsReverse64(frame);
    }
    AddInvert(frame);
}

void ac::IntertwineShuffle(cv::Mat &frame) {
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array({"IntertwineRows32","IntertwineRows16","IntertwineRows8","IntertwineRows4","Intertwine64x4","Intertwine64X","IntertwineColsX","IntertwineCols16","IntertwineCols32","IntertwineCols8","IntertwineRowsReverse32","IntertwineRowsReverse16","IntertwineRowsReverse8","IntertwineRowsReverse64X","IntertwineRowsSwitch32","IntertwineRowsSwitch16","IntertwineRowsSwitch8","IntertwineRows64","IntertwineRowsReverse64","IntertwineRowsSwitch64"});
    static int init = 0;
    if(init == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(), rng);
        init = 1;
    }
    static unsigned int index = 0;
    CallFilter(filter_array[index], frame);
    ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    AddInvert(frame);
}

void ac::InterwtineAlphaSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "InterwtineAlphaSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    CallFilter(subfilter, copy1);
    AlphaMovementMaxMin(alpha,dir,0.005, 3.0, 1.0);
    AlphaBlend(copy1,copy2,frame,alpha);
    pushSubFilter(getFilterByName("IntertwineShuffle"));
    SmoothSubFilter32(frame);
    popSubFilter();
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::IntertwineImageAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineImageAndSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    IntertwineRows32(reimage);
    IntertwineRowsReverse32(copy1);
    AlphaBlendDouble(copy1, reimage, frame, 1.0, 0.3);
    CallFilter(subfilter, copy1);
    AddInvert(frame);
}

void ac::IntertwineRowsAndCols(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRows32(copy1);
    IntertwineCols32(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwineRowsAndColsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineRowsAndColsSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRows32(copy1);
    Intertwine64X(copy2);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    static MatrixCollection<16> collection;
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::MatrixCollectionVariable(cv::Mat &frame) {
    static int depth = 2;
    static int dir = 1;
    static constexpr int v_size = 8;
    static MatrixCollection<v_size> collection;
    MatrixVariable(frame, &collection, depth, dir);
    AddInvert(frame);
}

void ac::MatrixCollectionVariableSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionVariableSubFilter")
        return;
    static int depth = 2;
    static int dir = 1;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    MatrixVariable(frame, &collection, depth, dir);
    AddInvert(frame);
}

void ac::MatrixCollectionVariableImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    MatrixCollectionVariable(copy1);
    MatrixCollectionVariable(reimage);
    AlphaBlendDouble(copy1, reimage, frame, 0.7, 0.3);
    AddInvert(frame);
}

void ac::MatrixCollectionVariableImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionVariableImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    MatrixCollectionVariable(reimage);
    AlphaBlendDouble(copy1, reimage, frame, 0.7, 0.3);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

