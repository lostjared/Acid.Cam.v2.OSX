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

/*
 //Basic Multithreaded Filter
 auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
 for(int i = 0; i < cols; ++i) {
 }
 }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
 
 */

#include "ac.h"

void ac::VideoMirrorAlpha(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
        MirrorLeftBottomToTop(frame);
    }
    AddInvert(frame);
}

void ac::VideoMirrorLeftRight(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        static int index = 0;
        if(index == 0) {
            MirrorLeft(reframe);
            MirrorRight(copy1);
        } else {
            MirrorRight(reframe);
            MirrorLeft(copy1);
        }
        index = (index == 0) ? 1 : 0;
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoFlipFrames(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    cv::Mat copy1 = frame.clone();
    static int index = 0;
    if(VideoFrame(vframe)) {
        cv::Mat flipped;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        if(index == 0) {
            cv::flip(reframe, flipped, 1);
        } else {
            cv::flip(copy1, flipped, 1);
        }
        index = (index == 0) ? 1 : 0;
        AlphaBlendDouble(reframe, flipped, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoImageBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false || blend_set == false)
        return;
    cv::Mat vframe,reimage;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        ac_resize(blend_image, reimage, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix[3];
                    pix[0] = reframe.at<cv::Vec3b>(z, i);
                    pix[1] = reimage.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j)  {
                        pixel[j] = static_cast<unsigned char>(((pixel[j]^pix[0][j]^pix[1][j])) * 0.8);
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
}

void ac::VideoCycleReverse(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
        CycleShiftRGB(frame);
        MirrorLeft(frame);
    }
    AddInvert(frame);
}

void ac::VideoCycleMedianReverse(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
        CycleShiftRGB(frame);
        MedianBlendMultiThreadScale(frame);
        MirrorLeft(frame);
    }
    AddInvert(frame);
}

void ac::VideoStrobeFrameOnOff(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int index = 0;
    if(index == 0) {
        cv::Mat copy1 = frame.clone();
        Negate(copy1);
        copy1.copyTo(frame);
    } else {
        cv::Mat vframe;
        if(VideoFrame(vframe)) {
            cv::Mat reframe;
            ac_resize(vframe, reframe, frame.size());
            Negate(reframe);
            reframe.copyTo(frame);
        }
    }
    index = (index == 0) ? 1 : 0;
    AddInvert(frame);
}

void ac::VideoMirror(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static double alpha = 1.0;
        static int dir = 1;
        cv::Mat reframe, copy1 = frame.clone();
        ac_resize(vframe, reframe, frame.size());
        MirrorLeftBottomToTop(reframe);
        MirrorRightTopToBottom(copy1);
        AlphaBlendDouble(reframe, copy1, frame, alpha, (1-alpha));
        AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoMedianBlendScale(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat copy1 = frame.clone();
    cv::Mat vframe;
    static double alpha = 1.0;
    static int dir = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        MedianBlendMultiThreadScale(copy1);
        MedianBlendMultiThreadScale(reframe);
        AlphaBlendDouble(reframe, copy1, frame, alpha, (1-alpha));
        AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
    }
    AddInvert(frame);
}

void ac::VideoMedianBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat copy1 = frame.clone();
    cv::Mat vframe;
    static double alpha = 1.0;
    static int dir = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        MedianBlendMultiThread(copy1);
        MedianBlendMultiThread(reframe);
        AlphaBlendDouble(reframe, copy1, frame, alpha, (1-alpha));
        AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
    }
    AddInvert(frame);
}

void ac::VideoBlendSubFilter(cv::Mat &frame) {
    if(v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoBlendSubFilter")
        return;
    cv::Mat vframe;
    cv::Mat copy1 = frame.clone();
    if(VideoFrame(vframe)) {
        static double alpha = 1.0;
        static int dir = 1;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        CallFilter(subfilter, copy1);
        AlphaBlendDouble(reframe, copy1, frame, alpha, (1-alpha));
        AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoLoFi(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, cv::Size(320, 240));
        ac_resize(reframe, frame, frame.size());
    }
    AddInvert(frame);
}

void ac::VideoLoFiBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, cv::Size(320, 240));
        cv::Mat copy1;
        ac_resize(frame, copy1, cv::Size(320, 240));
        cv::Mat output;
        AlphaBlendDouble(reframe, copy1, output, 0.5, 0.5);
        ac_resize(output, frame, frame.size());
    }
    AddInvert(frame);
}

void ac::VideoLoFiSourceNormal(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe, cpframe;
        ac_resize(vframe, reframe, cv::Size(320, 240));
        ac_resize(reframe, cpframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(cpframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoDarken25(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 4);
        reframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoDarken50(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 2);
        reframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoDarken75(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 6);
        reframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoDarkenBlend25(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 4);
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoDarkenBlend50(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 2);
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoDarkenBlend75(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        DarkenImage(reframe, 6);
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoBlendSmooth8(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone(), outframe;
        AlphaBlendDouble(reframe, copy1, outframe, 0.5, 0.5);
        collection.shiftFrames(outframe);
        Smooth(outframe, &collection, false);
        outframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoBlendSmooth16(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 16;
    static MatrixCollection<Size> collection;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone(), outframe;
        AlphaBlendDouble(reframe, copy1, outframe, 0.5, 0.5);
        collection.shiftFrames(outframe);
        Smooth(outframe, &collection, false);
        outframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoBlendSmooth32(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone(), outframe;
        AlphaBlendDouble(reframe, copy1, outframe, 0.5, 0.5);
        collection.shiftFrames(outframe);
        Smooth(outframe, &collection, false);
        outframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoXorSource(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pixel[j]^pix[j];
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
}

void ac::VideoXorSelfScale(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    static double alpha = 1.0;
    static int dir = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((static_cast<int>(pixel[j] * alpha))^static_cast<int>(pix[j] * (1-alpha)));
                    }
                    
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MedianBlendNoBlurWithColor(cv::Mat &frame) {
    BlendWithColor(frame);
    NoMedianBlurBlendMultiThread(frame);
}

void ac::VideoImageSmooth(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection.shiftFrames(reframe);
        collection.shiftFrames(reimage);
        cv::Mat copy1 = frame.clone();
        Smooth(copy1, &collection, false);
        cv::Mat copy2 = frame.clone();
        AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::ColorMapFilter(cv::Mat &frame) {
    int val = getColorMapFilter();
    if(val == -1 || val == 0)
        return;
    setColorMap(val-1, frame);
    AddInvert(frame);
}

void ac::VideoColorMap(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        ColorMapFilter(reframe);
        reframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoColorMapAlphaBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        ColorMapFilter(reframe);
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::ReduceColors(cv::Mat &frame) {
    static int lazy = 0;
    static unsigned char colors[255];
    if(lazy == 0) {
        unsigned char val = 75;
        for(int z = 1; z < 255; ++z) {
            if((z%75) == 0)
                val += 75;
            colors[z] = val;
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = colors[pixel[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ReduceColorsRandom(cv::Mat &frame) {
    static int lazy = 0;
    static unsigned char colors_r[255], colors_g[255], colors_b[255];
    if(lazy == 0 || reset_alpha == true) {
        unsigned char val_r = rand()%255;
        unsigned char val_g = rand()%255;
        unsigned char val_b = rand()%255;
        for(int z = 1; z < 255; ++z) {
            if((z%75) == 0) {
                val_r = rand()%255;
                val_g = rand()%255;
                val_b = rand()%255;
            }
            colors_r[z] = val_r;
            colors_g[z] = val_g;
            colors_b[z] = val_b;
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[0] = colors_b[pixel[0]];
                pixel[1] = colors_g[pixel[1]];
                pixel[2] = colors_r[pixel[2]];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ReduceColorsRed(cv::Mat &frame) {
    static int lazy = 0;
    static unsigned char colors_r[255] = {0};
    if(lazy == 0 || reset_alpha == true) {
        unsigned char val_r = 75;
        static int index = 0;
        for(int z = 1; z < 255; ++z) {
            ++index;
            if(index >  75) {
                index = 0;
                val_r += 75;
            }
            colors_r[z] = val_r;
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[2] = colors_r[pixel[2]];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ReduceColorsGreen(cv::Mat &frame) {
    static int lazy = 0;
    static unsigned char colors_r[255] = {0};
    if(lazy == 0 || reset_alpha == true) {
        unsigned char val_r = 75;
        static int index = 0;
        for(int z = 1; z < 255; ++z) {
            ++index;
            if(index >  75) {
                index = 0;
                val_r += 75;
            }
            colors_r[z] = val_r;
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[1] = colors_r[pixel[1]];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ReduceColorsBlue(cv::Mat &frame) {
    static int lazy = 0;
    static unsigned char colors_r[255] = {0};
    if(lazy == 0 || reset_alpha == true) {
        unsigned char val_r = 75;
        static int index = 0;
        for(int z = 1; z < 255; ++z) {
            ++index;
            if(index >  75) {
                index = 0;
                val_r += 75;
            }
            colors_r[z] = val_r;
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[0] = colors_r[pixel[0]];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
