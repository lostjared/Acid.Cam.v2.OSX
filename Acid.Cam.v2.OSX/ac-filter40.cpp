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

void ac::VideoSlideOffsetXRev(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            int pos = 0;
            for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, pos);
                for(int bgr = 0; bgr < 3; ++bgr) {
                    pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                }
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, pos);
                for(int bgr = 0; bgr < 3; ++bgr) {
                    pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                }
                ++pos;
            }
        }
        if(x_dir == 1) {
            x_offset += speed;
            if(x_offset > frame.cols-1) {
                x_offset = frame.cols-1;
                x_dir = 0;
            }
        } else {
            x_offset -= speed;
            if(x_offset <= 1) {
                x_offset = 1;
                x_dir = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoSlideOffsetRGB(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    static int bgr = 0;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            int pos = 0;
            for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
        }
        if(x_dir == 1) {
            x_offset += speed;
            if(x_offset > frame.cols-1) {
                x_offset = frame.cols-1;
                x_dir = 0;
            }
        } else {
            x_offset -= speed;
            if(x_offset <= 1) {
                x_offset = 1;
                x_dir = 1;
            }
        }
        
        ++bgr;
        if(bgr > 2)
            bgr = 0;
    }
    AddInvert(frame);
}

void ac::VideoSlideOffsetRGBRev(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    static int bgr = 0;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            int pos = 0;
            for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, pos);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, pos);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
        }
        if(x_dir == 1) {
            x_offset += speed;
            if(x_offset > frame.cols-1) {
                x_offset = frame.cols-1;
                x_dir = 0;
            }
        } else {
            x_offset -= speed;
            if(x_offset <= 1) {
                x_offset = 1;
                x_dir = 1;
            }
        }
        ++bgr;
        if(bgr > 2)
            bgr = 0;
    }
    AddInvert(frame);
}

void ac::SelfSlideOffsetX(cv::Mat &frame) {
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    cv::Mat reframe = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        int pos = 0;
        for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            }
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            }
            ++pos;
        }
    }
    if(x_dir == 1) {
        x_offset += speed;
        if(x_offset > frame.cols-1) {
            x_offset = frame.cols-1;
            x_dir = 0;
        }
    } else {
        x_offset -= speed;
        if(x_offset <= 1) {
            x_offset = 1;
            x_dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::SelfSlideOffsetRGB(cv::Mat &frame) {
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    static int bgr = 0;
    cv::Mat reframe = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        int pos = 0;
        for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            ++pos;
        }
    }
    if(x_dir == 1) {
        x_offset += speed;
        if(x_offset > frame.cols-1) {
            x_offset = frame.cols-1;
            x_dir = 0;
        }
    } else {
        x_offset -= speed;
        if(x_offset <= 1) {
            x_offset = 1;
            x_dir = 1;
        }
    }
    ++bgr;
    if(bgr > 2)
        bgr = 0;
    AddInvert(frame);
}

void ac::SelfSlideOffsetRGB_X(cv::Mat &frame) {
    static int x_offset = 0;
    static int x_dir = 1;
    static int speed = 30;
    cv::Mat reframe = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        int pos = 0;
        for(int i = x_offset; i < frame.cols && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr])) ^ static_cast<unsigned char>((0.5 * pix[bgr]));
            }
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = pixelAt(reframe,z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr])) ^ static_cast<unsigned char>((0.5 * pix[bgr]));
            }
            ++pos;
        }
    }
    if(x_dir == 1) {
        x_offset += speed;
        if(x_offset > frame.cols-1) {
            x_offset = frame.cols-1;
            x_dir = 0;
        }
    } else {
        x_offset -= speed;
        if(x_offset <= 1) {
            x_offset = 1;
            x_dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadStrobe(cv::Mat &frame) {
    static int index = 0;
    switch(index) {
        case 0:
            MedianBlendMultiThread(frame);
            break;
        case 1:
            MedianBlendMultiThread_2160p(frame);
            break;
        case 2:
            MedianBlendMultiThreadByFour(frame);
            break;
        case 3:
            MedianBlendMultiThreadByEight(frame);
            break;
    }
    ++index;
    if(index > 3)
        index = 0;
    AddInvert(frame);
}

void ac::VideoXor_XY(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    int val1 = pixel[j]^i;
                    int val2 = pix[j]^z;
                    double op1 = (0.5 * pixel[j]);
                    double op2 = (0.5 * pix[j]);
                    pixel[j] = (static_cast<unsigned char>(op1)^val1) + (static_cast<unsigned char>(op2)^val2);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoRandomWave(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static int index = rand()%collection.size();
        static int dir = rand()%2;
        collection.shiftFrames(reframe);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    dir = rand()%collection.size();
                    index = collection.size()-1;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = rand()%collection.size();
                    dir = 1;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::RandomWave(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    static int index = rand()%collection.size();
    static int dir = rand()%2;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                dir = rand()%2;
                index = collection.size()-1;
            }
        } else {
            --index;
            if(index <= 0) {
                index = rand()%collection.size();
                dir = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::RandomWaveStartStop(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    static int index = rand()%collection.size();
    static int dir = rand()%2;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                dir = rand()%2;
                index = rand()%collection.size();
            }
        } else {
            --index;
            if(index <= 0) {
                index = rand()%collection.size();
                dir = rand()%2;
            }
        }
    }
    AddInvert(frame);
}

void ac::RandomMedianFrame(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat random_frame = collection.frames[rand()%collection.size()].clone();
    MedianBlendMultiThread(random_frame);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = pixelAt(random_frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::VideoSmoothDoubleAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection1, collection2;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        collection1.shiftFrames(frame);
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection2.shiftFrames(reframe);
        cv::Mat copy1 = frame.clone(),copy2 = reframe.clone();
        Smooth(copy1, &collection1);
        Smooth(copy2, &collection2);
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
                    cv::Vec3b pix2 = copy2.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pix1[j]) + (0.5 * pix2[j]));
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
}

void ac::VariableLines(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        int offset = rand()%frame.cols;
        int pos = 0;
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
            }
            ++pos;
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, pos);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
            }
            ++pos;
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesOffset(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int offset_counter = rand()%150;
    int index = 0;
    for(int z = 0; z < frame.rows; z++) {
        int offset = rand()%frame.cols;
        int pos = 0;
        ++index;
        if(index > offset_counter) {
            index = 0;
            offset_counter = rand()%150;
            z += offset_counter;
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VaraibleLinesRectangle(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int offset_counter = rand()%400;
    int index = 0;
    int offset = rand()%frame.cols;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        ++index;
        if(index > offset_counter) {
            index = 0;
            offset = rand()%frame.cols;
            offset_counter = rand()%400;
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesRectangleSource(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int offset_counter = rand()%400;
    int index = 0;
    int offset = rand()%frame.cols;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        ++index;
        if(index > offset_counter) {
            index = 0;
            offset = rand()%frame.cols;
            offset_counter = rand()%400;
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesStartRectangle(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesStartRectangleBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesOnOffBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        int rand_off =  rand()%15;
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(rand_off <= 5) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
                else
                    pixel = pix;
                
                ++pos;
            }
        }
        
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(rand_off <= 5) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
                else
                    pixel = pix;
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableCollectionLinesOffBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    static int index = 0;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        int rand_off =  rand()%15;
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(rand_off <= 5) {
                    pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
                else
                    pixel = pix;
                
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(rand_off <= 5) {
                    pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
                else
                    pixel = pix;
                ++pos;
            }
            ++index;
            if(index > (collection.size()-1))
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::VariableDistortionWave(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    static int index = 0, in_dir = 1;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                if(index >= 0 && index < collection.size()-1) {
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    ++pos;
                }
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                if(index >= 0 && index < collection.size()-1) {
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    ++pos;
                }
            }
        }
        if(in_dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                in_dir = 0;
            }
        } else {
            --index;
            if(index <= 0) {
                index = 0;
                in_dir = 1;
            }
        }
        
    }
    AddInvert(frame);
}

void ac::VariableDistortionReset(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int i_dir = 1;
    static int offset = rand()%frame.cols;
    static int index = 0;
    for(int z = 0; z < frame.rows; z++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.cols-1) {
                offset = frame.cols-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int i = offset; i < frame.cols && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
        for(int i = 0; i < offset && pos < frame.cols; ++i) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,z, pos);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
        ++index;
        if(index > (collection.size()-1)) {
            index = 0;
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesY(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.rows;
    for(int i = 0; i < frame.cols; i++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.rows-1) {
                offset = frame.rows-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int z = offset; z < frame.rows && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
        for(int z = 0; z < offset && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesY_Blend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int i_dir = 1;
    static int offset = rand()%frame.rows;
    for(int i = 0; i < frame.cols; i++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.rows-1) {
                offset = frame.rows-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int z = offset; z < frame.rows && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
                ++pos;
            }
        }
        for(int z = 0; z < offset && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
                ++pos;
            }
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesY_Wave(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int index = 0, index_dir = 1;
    static int i_dir = 1;
    static int offset = rand()%frame.rows;
    for(int i = 0; i < frame.cols; i++) {
        int pos = 0;
        if(i_dir == 1) {
            ++offset;
            if(offset > frame.rows-1) {
                offset = frame.rows-1;
                i_dir = 0;
            }
        } else {
            --offset;
            if(offset <= 1) {
                offset = 1;
                i_dir = 1;
            }
        }
        for(int z = offset; z < frame.rows && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                if(index >= 0 && index < collection.size()-1) {
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    ++pos;
                }
            }
        }
        for(int z = 0; z < offset && pos < frame.rows; ++z) {
            if(z >= 0 && z < frame.rows && pos >= 0 && pos < frame.cols && i >= 0 && i < frame.cols) {
                
                cv::Vec3b &pixel = pixelAt(frame,pos, i);
                if(index >= 0 && index < collection.size()-1) {
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    ++pos;
                }
            }
        }
        if(index_dir == 1) {
            index++;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                index_dir = 0;
            }
        } else {
            --index;
            if(index <= 0) {
                index = 0;
                index_dir = 1;
            }
        }
        
    }
    AddInvert(frame);
}
