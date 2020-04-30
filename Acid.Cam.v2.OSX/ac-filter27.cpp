
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
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

void ac::LightStrobe(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::StrobeRedChannel(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[2] = static_cast<unsigned char>((pixel[2] * alpha));
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::StrobeGreenChannel(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[1] = static_cast<unsigned char>((pixel[1] * alpha));
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::StrobeBlueChannel(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[0] = static_cast<unsigned char>((pixel[0] * alpha));
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::StrobeOnOffRandomChannel(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static std::vector<int> random_vec({0, 1, 2});
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static int index = 0;
    int channel = random_vec[index];
    ++index;
    if(index > 2) {
        std::shuffle(random_vec.begin(), random_vec.end(), rng);
        index = 0;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[channel] = static_cast<unsigned char>((pixel[channel] * alpha));
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::StrobeLightFlashRandomChannel(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    pixel[0] = 255;
                    pixel[1] = 255;
                    pixel[2] = 255;
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
        index = 1;
    } else {
        StrobeOnOffRandomChannel(frame);
        index = 0;
    }
    AddInvert(frame);
}

// Math Behind LostSideDead increasing Acronym.
//             1 2 2 3 3  3
// pixel[0] = pixel[0] * scale[0]
// pixel[1] = (pix1[1]+pix2[1] )* scale[1]
// pixel[2] = (pixel[2]+pix1[2]+pix2[2]) * scale[2]
void ac::LostSideDeadImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "LostSideDeadImageSubFilter")
        return;
    cv::Mat reimage;
    cv::Mat copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    static double alpha[3] = {1.0, 0.7, 0.1};
    static int dir[3] = {1, 1, 1};
    static int col[3];
    static int index = 0;
    InitArrayPosition(col, index);
    ++index;
    if(index > 2)
        index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = reimage.at<cv::Vec3b>(z, i);
                pixel[0] = static_cast<unsigned char>((pixel[0]*alpha[col[0]]));
                pixel[1] = static_cast<unsigned char>((pix1[1]+pix2[1])*alpha[col[1]]);
                pixel[2] = static_cast<unsigned char>((pixel[2]+pix1[2]+pix2[2])*alpha[col[2]]);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha[0], dir[0], 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha[1], dir[1], 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha[2], dir[2], 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::SwapImageFrameSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "SwapImageFrameSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    static int index = 0;
    int col[3] = {0};
    InitArrayPosition(col, index);
    ++index;
    if(index > 2)
        index = 0;
    static double alpha[2] = {1.0, 0.1};
    static int dir[2] = {0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b colors[3];
                colors[col[0]] = pixel;
                colors[col[1]] = reimage.at<cv::Vec3b>(z, i);
                colors[col[2]] = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha[0] * pixel[j]) + (alpha[1] * colors[j][j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha[0], dir[0], 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha[1], dir[1], 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::PulseIncreaseFast(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 1.0;
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
    static int direction = 1;
    if(direction == 1) {
        inc += 0.05;
        if(inc > 50) {
            direction = 0;
        }
    } else {
        inc -= 0.05;
        if(inc <= 4) {
            direction = 1;
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::FibonacciXor(cv::Mat &frame) {
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 0};
    static int index = 0;
    static double alpha = 1.0;
    static int dir = 0;
    static int fib_dir = 1;
    if(fib_dir == 1) {
        ++index;
        if(fib_value[index] == 0) {
            --index;
            fib_dir = 0;
        }
    } else {
        --index;
        if(index <= 0) {
            index = 1;
            fib_dir = 1;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]^fib_value[index]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ColorFibonacci(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 0};
    static double alpha = 1.0;
    static int dir = 1;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Scalar values;
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        values[j] += (pix[j]^fib_value[q]);
                    }
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * values[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageFibonacci(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 0};
    static int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorTransition(reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + ((pixel[j]^pix[j]^fib_value[index])*alpha));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(fib_value[index] == 0) {
        index = 0;
    }
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::ImageFibonacciInAndOut(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 0};
    static int index = 0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorPulseIncrease(reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.5) + static_cast<unsigned char>(0.5 * static_cast<unsigned char>((0.5 * (pixel[j]^fib_value[index])) + (0.5 * (pix[j]^fib_value[index]))));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(fib_value[index] == 0) {
            --index;
            dir = 0;
        }
    } else {
        --index;
        if(index <= 0) {
            index = 0;
            dir = 1;
        }
    }
}

void ac::ImageFibonacciStrobe(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 0};
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int index = 0;
    static int dir = 1;
    if(dir == 1) {
        index += 3;
        if(index+2 >= 22) {
            dir = 0;
            index = 21;
        }
    } else {
        index -= 3;
        if(index-2 <= 3) {
            index = 0;
            dir = 1;
        }
    }
    int values[3] = {0};
    if(index+3 <= 21) {
        values[0] = fib_value[index];
        values[1] = fib_value[index+1];
        values[2] = fib_value[index+2];
    }
    static double alpha = 1.0;
    static int dir_v = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (0.5 * ((pixel[j]^values[j])^pix[j])*alpha));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir_v, 0.005, 1.0, 0.1);
}

void ac::ImageFibonacciMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    ColorPulseIncrease(copy1);
    collection.shiftFrames(copy1);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377,0};
    static int index = 0;
    static int frame_counter = 0;
    ++frame_counter;
    if(frame_counter >= static_cast<int>(ac::fps+1)) {
        ++index;
        if(fib_value[index] == 0) {
            index = 0;
        }
        frame_counter = 0;
    }
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * ((pix[j]+img_pix[j])^fib_value[index])));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ImageFibonacciInAndOutSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageFibonacciInAndOutSubFilter")
        return;
    static int fib_value[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 0};
    static int index = 0;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.5) + static_cast<unsigned char>(0.5 * static_cast<unsigned char>((0.5 * (pixel[j]^fib_value[index])) + (0.5 * (pix[j]^fib_value[index]))));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(fib_value[index] == 0) {
            --index;
            dir = 0;
        }
    } else {
        --index;
        if(index <= 0) {
            index = 0;
            dir = 1;
        }
    }
}

void ac::ImageKaleidoscopeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageKaleidoscopeSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    GaussianBlur(reimage);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    GaussianBlur(copy1);
    GaussianBlur(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[5].clone();
    frames[1] = collection.frames[6].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = pixel;
                pix[1] = copy1.at<cv::Vec3b>(z, i);
                pix[2] = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.33 * pix[0][j]) + (0.33 * pix[1][j]) + (0.33 * pix[2][j]));
                    cv::Vec3b pix_v = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) +  (0.5 *pix_v[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);
}

void ac::ImageMirrorLeftRight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorTransition(reimage);
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = reimage.at<cv::Vec3b>(z, i);
                pix[1] = copy1.at<cv::Vec3b>(z, i);
                pix[2] = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.25 * pixel[j]) + (0.25 * pix[0][j]) + (0.25 * pix[1][j]) + (0.25 * pix[2][j]));
                    cv::Vec3b pix_v = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix_v[j]));
                }
            }
        }
    };
    MirrorLeftBottomToTop(frame);
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ImageMirrorShiftUpLeftDownRight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorTransition(reimage);
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    static int index = 0;
    switch(index) {
        case 0:
            MirrorLeft(copy1);
            MirrorBottomToTop(copy2);
            break;
        case 1:
            MirrorRight(copy1);
            MirrorTopToBottom(copy2);
            break;
        case 2:
            MirrorLeft(copy1);
            MirrorTopToBottom(copy2);
            break;
        case 3:
            MirrorRight(copy1);
            MirrorTopToBottom(copy2);
            break;
    }
    ++index;
    if(index > 3)
        index = 0;
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[16].clone();
    frames[2] = collection.frames[31].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = reimage.at<cv::Vec3b>(z, i);
                pix[1] = copy1.at<cv::Vec3b>(z, i);
                pix[2] = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.25 * pixel[j]) + (0.25 * pix[0][j]) + (0.25 * pix[1][j]) + (0.25 * pix[2][j]));
                    cv::Vec3b pix_v = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix_v[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

