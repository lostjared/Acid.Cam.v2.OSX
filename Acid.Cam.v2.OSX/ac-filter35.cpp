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


void ac::LightBlur(cv::Mat &frame) {
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::GaussianBlur(blur, out, cv::Size(3, 3), 0, 0);
    out.copyTo(frame);
    AddInvert(frame);
}

void ac::LightMedianBlendMultiThread(cv::Mat &frame) {
    static constexpr int Size = 8;
    int r = rand()%2;
    for(int j = 0; j < r; ++j)
        LightBlur(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::ColorFadeMedianBlendMultiThread(cv::Mat &frame) {
    ColorIncreaseFadeRGB(frame);
    static constexpr int Size = 8;
    MedianBlur(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::ColorFadeBlendMedianBlendMultiThread(cv::Mat &frame) {
    static constexpr int Size = 8;
    ColorIncreaseFadeRGB(frame);
    BlendWithColor(frame);
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread32(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MedianBlur(frame);
    MatrixBlend(frame, &collection, 4);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread4(cv::Mat &frame) {
    static constexpr int Size = 4;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    MedianBlur(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendFadeWithColor(cv::Mat &frame) {
    ColorIncreaseFadeRGB(frame);
    BlendWithColor(frame);
    MedianBlendMultiThread4(frame);
    AddInvert(frame);
}

void ac::GaussianBlurMild(cv::Mat &frame) {
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::GaussianBlur(blur, out, cv::Size(3, 3), 0, 0);
    out.copyTo(frame);
    AddInvert(frame);
}

void ac::MedianBlendWithColorValue(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    ColorIncreaseFadeRGB(frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendWithColorInOutRGB(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    BlendWithColor(frame);
    ColorIncreaseInOutRGB(frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::MedianBlendWithCollection16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    BlendWithColor(copy1);
    ColorIncreaseFadeRGB(copy1);
    collection.shiftFrames(copy1);
    Smooth(copy1, &collection);
    MatrixBlend(copy1, &collection);
    copy1.copyTo(frame);
    AddInvert(frame);
}

void ac::FadeRtoGtoB(cv::Mat &frame) {
    static int rgb = 0;
    static int colors[3] = { 0, 0, 0 };
    static int dir[3] = {1,1,1};
    static constexpr int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.5)+(0.5*colors[j]));
            }
        }
    };
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            colors[rgb] += speed;
            if(colors[rgb] >= 255) {
                colors[rgb] = rand()%255;
                dir[j] = 0;
                ++rgb;
                if(rgb > 2)
                    rgb = 0;
            }
        } else {
            colors[rgb] -= speed;
            if(colors[rgb] <= 0) {
                colors[rgb] = rand()%255;
                dir[j] = 1;
                ++rgb;
                if(rgb > 2)
                    rgb = 0;
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendFadeRtoGtoB(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    FadeRtoGtoB(frame);
    collection.shiftFrames(frame);
    GaussianBlur(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::FadeRtoGtoB_Increase(cv::Mat &frame) {
    static int rgb = 0;
    static int colors[3] = { 0, 0, 0 };
    static int dir[3] = {1,1,1};
    static int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.5)+(0.5*colors[j]));
            }
        }
    };
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            colors[rgb] += speed;
            if(colors[rgb] >= 255) {
                colors[rgb] = rand()%255;
                dir[j] = 0;
                ++rgb;
                if(rgb > 2) {
                    rgb = 0;
                    ++speed;
                    if(speed > 10)
                        speed = 1;
                }
            }
        } else {
            colors[rgb] -= speed;
            if(colors[rgb] <= 0) {
                colors[rgb] = rand()%255;
                dir[j] = 1;
                ++rgb;
                if(rgb > 2) {
                    rgb = 0;
                    ++speed;
                    if(speed > 10)
                        speed = 1;
                }
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

// cool combo
void ac::MedianBlendFadeRtoGtoB_Increase(cv::Mat &frame) {
    FadeRtoGtoB_Increase(frame);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::FadeRandomChannel(cv::Mat &frame) {
    static int rgb = rand()%frame.channels();
    static int colors[3] = { 0, 0, 0 };
    static int dir[3] = {1,1,1};
    static constexpr int speed = 1;
    const int num = frame.channels();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < num; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.5)+(0.5*colors[j]));
            }
        }
    };
    for(int j = 0; j < num; ++j) {
        if(dir[j] == 1) {
            colors[rgb] += speed;
            if(colors[rgb] >= 255) {
                colors[rgb] = rand()%255;
                dir[j] = 0;
                rgb = rand()%frame.channels();
            }
        } else {
            colors[rgb] -= speed;
            if(colors[rgb] <= 0) {
                colors[rgb] = rand()%255;
                dir[j] = 1;
                rgb = rand()%frame.channels();
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::FadeRandomChannel_Increase(cv::Mat &frame) {
    static int rgb = rand()%frame.channels();
    static int colors[3] = { 0, 0, 0 };
    static int dir[3] = {1,1,1};
    static int speed = 1;
    const int num = frame.channels();
    static int speed_dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < num; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.5)+(0.5*colors[j]));
            }
        }
    };
    for(int j = 0; j < num; ++j) {
        if(dir[j] == 1) {
            colors[rgb] += speed;
            if(colors[rgb] >= 255) {
                colors[rgb] = rand()%255;
                dir[j] = 0;
                rgb = rand()%frame.channels();
                if(speed_dir == 1) {
                    speed += 1;
                    if(speed > 25) {
                        speed = 25;
                        speed_dir = 0;
                    }
                } else {
                    speed -= 1;
                    if(speed <= 1) {
                        speed = 1;
                        speed_dir = 1;
                    }
                }
            }
        } else {
            colors[rgb] -= speed;
            if(colors[rgb] <= 0) {
                colors[rgb] = rand()%255;
                dir[j] = 1;
                rgb = rand()%frame.channels();
                if(speed_dir == 1) {
                    speed += 1;
                    if(speed > 25) {
                        speed = 25;
                        speed_dir = 0;
                    }
                } else {
                    speed -= 1;
                    if(speed <= 1) {
                        speed = 1;
                        speed_dir = 1;
                    }
                }
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendFadeRandomChannel(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    FadeRandomChannel_Increase(frame);
    collection.shiftFrames(frame);
    MatrixBlend(frame, &collection);
    AddInvert(frame);
}

void ac::GlitchyTrails(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_max = (frame.rows / (collection.size()-1));
    static int square_size = 25 + (rand()% (square_max - 25));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols-1 && z < frame.rows-1) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(off < (collection.size()-1)) {
                        cv::Vec3b pix = collection.frames[off].at<cv::Vec3b>(z, i);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                }
            }
        }
        row += square_size;
        size_past += square_size;
        if(row > frame.rows-1 || size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection.size()-1)) {
                off = 0;
                break;
            }
        }
    }
    AddInvert(frame);
}

void ac::GlitchyXorTrails(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    GlitchyXorTrails(frame, &collection);
    AddInvert(frame);
}

void ac::GlitchedFilter(cv::Mat &frame) {
    MatrixCollectionAuraTrails(frame);
    GlitchyXorTrails(frame);
    VariableRectanglesExtra(frame);
    AddInvert(frame);
}

void ac::DisplaySometimesSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DisplaySometimesSubFilter")
        return;
    if((rand()%10) > 5) {
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::GlitchyVideoXorTrails(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<32> collection;
        static MatrixCollection<32> vcollection;
        collection.shiftFrames(frame);
        vcollection.shiftFrames(reframe);
        static int square_max = (frame.rows / (collection.size()-1));
        static int square_size = 25 + (rand()% (square_max - 25));
        int row = 0;
        int off = 0;
        int off_row = 1+rand()%25;
        int size_past = 0;
        while(row < frame.rows-1) {
            square_size = 25 + (rand()% (square_max - 25));
            for(int z = row; z < row+square_size; ++z) {
                 for(int i = 0; i < frame.cols; ++i) {
                    if(i < frame.cols && z < frame.rows) {
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        if(off < (collection.size()-1)) {
                            cv::Vec3b pix[2];
                            pix[0] = collection.frames[off].at<cv::Vec3b>(z, i);
                            pix[1] = vcollection.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                if((z%off_row)==0)
                                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j])) ^ (pix[0][j]+pix[1][j]);
                                else
                                    pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[0][j]) + (0.3 * pix[1][j]));
                            }
                        }
                    }
                 }
            }
            row += square_size;
            size_past += square_size;
            if(row > frame.rows-1 || size_past > square_max-1) {
                size_past = 0;
                ++off;
                if(off > (collection.size()-1)) {
                    off = 0;
                    break;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::GltichedVideoFilter(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        GlitchyXorTrails(reframe);
        VariableRectanglesExtra(reframe);
        cv::Mat copy1 = frame.clone();
        GlitchyTrails(copy1);
        VariableRectanglesSmall(copy1);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::DualGitchyVideoXorTrails(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe, copy1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        copy1 = frame.clone();
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<8> collection1, collection2;
        GlitchyXorTrails(copy1, &collection1);
        GlitchyXorTrails(reframe, &collection2);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::StaticGlitch(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    GlitchyXorTrails(frame, &collection, true);
    AddInvert(frame);
}

void ac::VideoMirrorMultiDirection(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        MirrorLeftBottomToTop(copy1);
        MirrorRightTopToBottom(reframe);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::SoftwareGlitch_64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    GlitchyXorTrails(frame, &collection);
    VariableRectanglesLarge(frame);
    AddInvert(frame);
}

void ac::VideoMatrixBlendDouble(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone();
    int r = rand()%3;
    for(int q = 0; q < r; ++q)
        MedianBlur(copy1);
    collection1.shiftFrames(copy1);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        int r = rand()%3;
        for(int q = 0; q < r; ++q)
            MedianBlur(reframe);
        collection2.shiftFrames(reframe);
        MatrixBlendDouble(frame, &collection1, &collection2);
    }
    AddInvert(frame);
}

void ac::VideoMatrixFadeDouble(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<8> collection1, collection2;
    cv::Mat copy1 = frame.clone();
    int r = rand()%3;
    for(int q = 0; q < r; ++q)
        MedianBlur(copy1);
    collection1.shiftFrames(copy1);
    cv::Mat vframe;
    static double alpha1 = 0.4;
    static int dir1 = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        int r = rand()%3;
        for(int q = 0; q < r; ++q)
            MedianBlur(reframe);
        collection2.shiftFrames(reframe);
        MatrixBlendDouble(frame, &collection1, &collection2, 1, alpha1, (0.4-alpha1));
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 0.4, 0.1);
}

void ac::VideoMatrixColorSmooth(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<8> collection;
    ColorIncreaseFadeRGB(frame);
    BlendWithColor(frame);
    VideoMatrixFadeDouble(frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::VideoMedianBlendShared8(cv::Mat &frame) {
    static constexpr int SIZE = 8;
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<SIZE> collection;
    MedianBlur(frame, 3);
    collection.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        MedianBlur(reframe, 3);
        collection.shiftFrames(reframe);
        MatrixBlend(frame, &collection);
    }
    AddInvert(frame);
}

void ac::VideoMedianBlendShared16(cv::Mat &frame) {
    static constexpr int SIZE = 16;
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<SIZE> collection;
    MedianBlur(frame, 3);
    collection.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        MedianBlur(reframe, 3);
        collection.shiftFrames(reframe);
        MatrixBlend(frame, &collection);
    }
    AddInvert(frame);
}

void ac::GlitchedGrid(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static MatrixCollection<8> collection;
    collection.shiftFrames(copy1);
    GlitchyXorTrailsRandom(frame, &collection);
    AddInvert(frame);
}

void ac::ShuffleGlitch(cv::Mat &frame) {
    static std::vector<std::string> filter_array{"RectangleGlitch","SingleFrameGlitch","GlitchyTrails", "GlitchyXorTrails", "GlitchedFilter", "GlitchyVideoXorTrails", "StaticGlitch", "SoftwareGlitch_64", "GlitchedGrid"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::ShuffleVideoMatrix(cv::Mat &frame) {
    static std::vector<std::string> filter_array{"VideoMatrixBlendDouble", "VideoMatrixFadeDouble", "VideoMatrixColorSmooth"};
    static int index = 0;
    Shuffle(index, frame, filter_array);
    AddInvert(frame);
}

void ac::TruncateColor(cv::Mat &frame) {
    static int max = 255/2;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > max)
                        pixel[j] = max;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TruncateColor75(cv::Mat &frame) {
    static int max = 255/4;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > max)
                        pixel[j] = max;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TruncateColor25(cv::Mat &frame) {
    static int max = 255/4;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > max)
                        pixel[j] = max;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TruncateVariable(cv::Mat &frame) {
    static double index = 150;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > static_cast<int>(index))
                        pixel[j] = static_cast<int>(index);
                }
            }
        }
    };
    AlphaMovementMaxMin(index, dir, 1.0, 255, 150);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TruncateVariableSlow(cv::Mat &frame) {
    static double index = 150;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > static_cast<int>(index))
                        pixel[j] = static_cast<int>(index);
                }
            }
        }
    };
    AlphaMovementMaxMin(index, dir, 0.1, 255, 150);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TruncateVariableScale(cv::Mat &frame) {
    static double index = 75;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > static_cast<int>(index))
                        pixel[j] = pixel[j]^static_cast<int>(index);
                }
            }
        }
    };
    AlphaMovementMaxMin(index, dir, 2.0, 255, 75);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendTruncate(cv::Mat &frame) {
    TruncateVariableScale(frame);
    ColorIncreaseFadeRGB(frame);
    MedianBlendMultiThread4(frame);
}

void ac::MedianBlendTruncateScale(cv::Mat &frame) {
    TruncateVariableScale(frame);
    ColorIncreaseInOutRGB(frame);
    Variable_ColorScale(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::Variable_ColorScale(cv::Mat &frame) {
    static double colorz[3] = { -1, -1, -1 };
    static int dirz[3] = {1, 0, 1};
    static int lazy = 0;
    if(lazy == 0) {
        for(int j = 0; j < 3; ++j) {
            if(colorz[j] == -1) {
                colorz[j] = rand()%255;
                dirz[j] = rand()%2;
            }
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * colorz[j]));
                }
            }
        }
    };
    for(int j = 0; j < 3; ++j)
        AlphaMovementMaxMin(colorz[j], dirz[j], 1.0, 255.0, 1.0);
    
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendVariable_ColorScale(cv::Mat &frame) {
    Variable_ColorScale(frame);
    MedianBlendMultiThread(frame);
}

void ac::InOut_ColorScale(cv::Mat &frame) {
    static double color_value[3] = { -1, -1, -1 };
    static int color_dir[3] = { 0, 1, 0 };
    static double alpha = 1.0;
    static int dir = 1;
    static int lazy = 0;
    if(lazy == 0) {
        for(int j = 0; j < 3; ++j) {
            if(color_value[j] == -1) {
                color_value[j] = rand()%255;
                color_dir[j] = rand()%2;
            }
        }
        lazy = 1;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * color_value[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.4);
    for(int j = 0; j < 3; ++j) {
        AlphaMovementMaxMin(color_value[j], color_dir[j], (rand()%25), 255.0, 0.0);
    }
}

void ac::MedianBlendInOut_ColorScale(cv::Mat &frame) {
    InOut_ColorScale(frame);
    MedianBlendMultiThread(frame);
}

void ac::CollectionTruncate(cv::Mat &frame) {
    static double max_val = 100.0;
    static int dir = 1;
    static MatrixCollection<8> collection;
    MatrixCollectionTruncate(frame, &collection, static_cast<int>(max_val));
    AlphaMovementMaxMin(max_val, dir, 5.0, 255.0, 100.0);
    Smooth(frame, &collection);
}

void ac::MedianBlendCollectionTruncate(cv::Mat &frame) {
    CollectionTruncate(frame);
    MedianBlendMultiThread(frame);
}

void ac::VideoBlendTruncate(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    static double max1 = 100+(rand()%50);
    static int max1_dir = 1;
    static double max2 = 100+(rand()%50);
    static int max2_dir = 0;
    static double alpha = 1.0;
    static int alpha_d = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > static_cast<int>(max1))
                        pixel[j] = static_cast<int>(max1);
                    if(repix[j] > static_cast<int>(max2))
                        repix[j] = static_cast<int>(max2);
                    pixel[j] = static_cast<unsigned char> ((0.5 * pixel[j]) + (repix[j] * 0.5));
                }
            }
        }
    }
    AlphaMovementMaxMin(max1, max1_dir, 0.01, 255.0, 100.0);
    AlphaMovementMaxMin(max2, max2_dir, 0.01, 255.0, 100.0);
    AlphaMovementMaxMin(alpha, alpha_d, 0.01, 1.0, 1.0);
    AddInvert(frame);
}

void ac::RandomTruncateFrame(cv::Mat &frame) {
    cv::Vec3b off(rand()%255, rand()%255, rand()%255);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > off[j])
                        pixel[j] = off[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RandomStrobeMedianBlend(cv::Mat &frame) {
    RandomTruncateFrame(frame);
    MedianBlendMultiThread4(frame);
}

void ac::XorFade(cv::Mat &frame) {
    int chan = frame.channels();
    int r = rand()%(chan*2);
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < chan; ++j) {
                    pixel[j] ^= static_cast<unsigned char>(alpha * pixel[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.001, 1.0, 0.1);
}

void ac::ColorIncreaseXorFade(cv::Mat &frame) {
    ColorIncreaseFadeRGB(frame);
    XorFade(frame);
}

void ac::Pixels_InOut(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    int r = 1+(rand()%25);
                    if(p.dir[j] == 0) {
                        p.add[j] += rand()%r;
                        if(p.add[j] > 255) {
                            p.add[j] = 255;
                            p.dir[j] = 1;
                        }
                    } else {
                        p.add[j] -= rand()%r;
                        if(p.add[j] <= 0) {
                            p.add[j] = 0;
                            p.dir[j] = 0;
                        }
                    }
                    unsigned char val = pixel[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * val));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelDistortionCloud(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.add[j] == 0)
                        p.add[j] = p.col[j];
                    unsigned int val1 = pixel[j]/2;
                    unsigned int val2 = p.col[j]^p.add[j];
                    if(p.dir[j] == 1) {
                        p.add[j] += 5;
                        if(p.add[j] >= 255) {
                            p.col[j] = pixel[j];
                            p.add[j] = 255;
                            p.dir[j] = 0;
                        }
                    } else {
                        p.add[j] -= 5;
                        if(p.add[j] <= 1) {
                            p.col[j] = pixel[j];
                            p.add[j] = 1;
                            p.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * val1) + (0.5 * val2));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
