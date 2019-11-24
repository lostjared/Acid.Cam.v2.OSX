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
    static int square_max = (frame.rows / collection.size());
    static int square_size = 25 + (rand()% (square_max - 25));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
        if(size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection.size()-1))
                break;
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
        static int square_max = (frame.rows / collection.size());
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
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            if(size_past > square_max-1) {
                size_past = 0;
                ++off;
                if(off > (collection.size()-1))
                    break;
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
