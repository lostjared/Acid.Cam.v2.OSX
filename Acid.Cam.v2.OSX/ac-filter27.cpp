
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

#include"ac.h"

void ac::SingleFrameGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    if(collection.empty() || collection.bounds() != frame.size()) {
        collection.resetFrame(frame);
    }
    static int frame_counter = 0;
    static double seconds = 0;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        ++seconds;
        frame_counter = 0;
    }
    if(seconds >= 1) {
        seconds = 0;
        collection.shiftFrames(frame);
        Smooth(frame, &collection);
    }
    AddInvert(frame);
}

void ac::ColorFadeSlow(cv::Mat &frame) {
    static double val[3] = {-1.0, -1.0, -1.0};
    static int init = -1;
    // lazy
    if(init == -1) {
        val[0] = rand()%255;
        val[1] = rand()%255;
        val[2] = rand()%255;
        init = 0;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>(val[j]);
            }
        }
    }
    static int dir[3] = {1,1,1};
    static double speed[3] = {0.33, 0.33, 0.33};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            val[j] += speed[j];
            if(val[j] >= 255) {
                val[j] = rand()%255;
                dir[j] = 0;
                speed[j] += 0.33;
                if(speed[j] > 5) speed[j] = 1;
            }
        } else if(dir[j] == 0) {
            val[j] -= speed[j];
            if(val[j] <= 1) {
                val[j] = rand()%255;
                dir[j] = 1;
                speed[j] += 0.33;
                if(speed[j] > 5) speed[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::FadeBetweenSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "FadeBetweenSubFilter")
        return;
    static cv::Mat stored_copy = frame.clone();
    cv::Mat copy1 = frame.clone();
    if(stored_copy.empty() || (stored_copy.size() != frame.size())) {
        stored_copy = frame.clone();
        CallFilter(subfilter, stored_copy);
    }
    static double alpha1 = 1.0;
    static int dir = 1;
    if(alpha1 >= 0.9) {
        stored_copy = frame.clone();
        CallFilter(subfilter, stored_copy);
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AlphaBlendDouble(copy1, stored_copy, frame, alpha1, (1-alpha));
    AddInvert(frame);
}

void ac::FadeBetweenFrame(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = collection.frames[15].clone();
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha,dir,0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::TrailsTwitch(cv::Mat &frame) {
    static constexpr int Size = 8;
    static double alpha = 1.0;
    static int dir = 1;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    for(int i = 0; i < 3; ++i) {
        frames[i] = collection.frames[rand()%Size].clone();
    }
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    for(int i = 0; i < 3; ++i) {
        AlphaBlendDouble(copy1, frames[i], copy2, alpha, (1-alpha));
        copy1 = copy2.clone();
    }
    frame = copy1.clone();
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.01);
    AddInvert(frame);
}

void ac::Tremors(cv::Mat &frame) {
    static auto rng = std::default_random_engine{};
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int value_index = -1;
    static std::vector<int> index({0,1,2,3,4,5,6,7});
    if(value_index == -1 || value_index <= 7) {
        std::shuffle(index.begin(), index.end(), rng);
        value_index = 0;
    }
    int cur_val = index[value_index];
    frame = collection.frames[cur_val].clone();
    AddInvert(frame);
}

void ac::XorImageIndexPixel(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int index = 0;
    int arr[3];
    InitArrayPosition(arr, index);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b col = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[arr[j]] = pixel[arr[j]]^col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::ImageStrobeMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    int arr[3] = {0};
    static int index = 0;
    InitArrayPosition(arr, index);
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b col = collection.frames[arr[j]].at<cv::Vec3b>(z, i);
                    cv::Vec3b img_col = reimage.at<cv::Vec3b>(z, i);
                    pixel[arr[j]] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * col[j]) + (0.33 * img_col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++index;
    if(index > 2)
        index = 0;
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::StrobeImageArrayPosition(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    int col[3] = {0};
    static int index = 0;
    InitArrayPosition(col, index);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b img_col = reimage.at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * img_col[col[j]]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::OppositeImageArray(cv::Mat &frame) {
    if(blend_set == false)
        return;
    int arrayPos1[3] = {0}, arrayPos2[3] = {0};
    static int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    InitArrayPosition(arrayPos1, index);
    InitArrayPosition(arrayPos2,3-index-1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b image_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[arrayPos1[j]]) + ((1-alpha) * image_pix[arrayPos2[j]]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
    AlphaMovementMaxMin(alpha,dir,0.01,1.0, 0.1);
}

void ac::NoMedianBlurBlendMultiThread(cv::Mat &frame) {
    static MatrixCollection<4> collection;
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

void ac::NegateNoBlurMedian(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    cv::Mat frame_copy = frame.clone();
    Negate(frame_copy);
    collection.shiftFrames(frame_copy);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = collection.frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    NoMedianBlurBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha,dir,0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::PulseIncreaseVariableSpeed(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc[3];
    inc[0] = 1+((rand()%8) * 0.5);
    inc[1] = 1+((rand()%8) * 0.5);
    inc[2] = 1+((rand()%8) * 0.5);
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.5) + ((alpha[j] * 0.5)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScaleSpeed(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::Source25_Image75(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix_color = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j]*0.25) + (pix_color[j] * 0.75));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
