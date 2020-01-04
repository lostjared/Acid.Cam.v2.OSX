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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, pos);
                for(int bgr = 0; bgr < 3; ++bgr) {
                    pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                }
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, pos);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
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
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, pos);
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
                ++pos;
            }
            for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reframe.at<cv::Vec3b>(z, pos);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            }
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
            pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr]) + (0.5 * pix[bgr]));
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
            for(int bgr = 0; bgr < 3; ++bgr) {
                pixel[bgr] = static_cast<unsigned char>((0.5 * pixel[bgr])) ^ static_cast<unsigned char>((0.5 * pix[bgr]));
            }
            ++pos;
        }
        for(int i = 0; i < x_offset && pos < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, pos);
            cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
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
