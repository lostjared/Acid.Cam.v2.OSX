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

void ac::StretchColMatrix8(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}
void ac::StretchColMatrix16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}
void ac::StretchColMatrix32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int current = 0;
    int counter = 0;
    int rand_size = rand()%50;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[current].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        ++counter;
        if(counter > rand_size) {
            ++current;
            counter = 0;
            if(current > collection.size()-1) {
                current = 0;
                rand_size = rand()%50;
            }
        }
    }
    AddInvert(frame);
}



void ac::GradientRandomSwitch(cv::Mat &frame) {
    static std::vector<std::string> gradient_filter {"GradientColorBlend", "GradientRedBlend", "GradientGreenBlend", "GradientBlueBlend", "GradientXRed", "GradientXGreen", "GradientXBlue"};
    static std::default_random_engine random(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static int index = 0, num_filter = 1+(rand()%3);
    if(index == 0) {
        std::shuffle(gradient_filter.begin(), gradient_filter.end(), random);
        index = 1;
        num_filter = 1+(rand()%4);
    }
    static int frame_counter = 0, frame_rand = 0, c_fps = static_cast<int>(ac::fps);
    frame_rand = c_fps+(rand()%50);
    ++frame_counter;
    if(frame_counter > frame_rand) {
        frame_counter = 0;
        frame_rand = c_fps+(rand()%50);
        index = 0;
    }
    for(int i = 0; i < num_filter; ++i) {
        CallFilter(gradient_filter[i], frame);
    }
    AddInvert(frame);
}

void ac::GradientFlashColor(cv::Mat &frame) {
    double alpha[3] = {0};
    double r_color = rand()%255;
    double alpha_inc = 0.5/frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (alpha[j] * r_color));
            }
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AddInvert(frame);
}

void ac::GradientFlashComponent(cv::Mat &frame) {
    double alpha[3] = {0};
    double r_color = rand()%255;
    double alpha_inc = 0.5/frame.rows;
    int offset = rand()%3;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[offset] = static_cast<unsigned char>((0.5 * pixel[offset]) + (alpha[offset] * r_color));
        }
        for(int j = 0; j < 3; ++j) {
            alpha[j] += alpha_inc;
        }
    }
    AddInvert(frame);
}

void ac::GradientRandomShift(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    GradientRandomSwitch(frame);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadGradientShift(cv::Mat &frame) {
    GradientColorBlend(frame);
    GradientRandomShift(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::MirrorIntertwineRows720(cv::Mat &frame) {
    static constexpr int num_rows = 720;
    static MatrixCollection<num_rows> collection;
    cv::Mat resized;
    ac_resize(frame, resized, cv::Size(1280, 720));
    static int off = 1;
    off = (off == 0) ? 1 : 0;
    cv::Mat copy1 = resized.clone();
    if(off == 1) {
        MirrorLeft(copy1);
    } else {
        MirrorRight(copy1);
    }
    collection.shiftFrames(copy1);
    for(int index = 0, pos = 0; index < copy1.rows; index ++, ++pos) {
        cv::Mat &ref1 = collection.frames[pos];
        for(int i = 0; i < frame.cols; ++i) {
            if(index < frame.rows && i < frame.cols) {
                cv::Vec3b &pixel = resized.at<cv::Vec3b>(index, i);
                cv::Vec3b pix = ref1.at<cv::Vec3b>(index, i);
                pixel = pix;
            }
        }
    }
    ac_resize(resized, frame, frame.size());
    AddInvert(frame);
}

void ac::MirrorIntertwineRowsY(cv::Mat &frame) {
    static constexpr int num_rows = 360;
    static MatrixCollection<num_rows> collection;
    cv::Mat resized;
    ac_resize(frame, resized, cv::Size(640, 360));
    static int off = 1;
    off = (off == 0) ? 1 : 0;
    cv::Mat copy1 = resized.clone();
    if(off == 1) {
        MirrorTopToBottom(copy1);
    } else {
        MirrorBottomToTop(copy1);
    }
    collection.shiftFrames(copy1);
    for(int index = 0, pos = 0; index < copy1.rows; index ++, ++pos) {
        cv::Mat &ref1 = collection.frames[pos];
        for(int i = 0; i < frame.cols; ++i) {
            if(index < frame.rows && i < frame.cols) {
                cv::Vec3b &pixel = resized.at<cv::Vec3b>(index, i);
                cv::Vec3b pix = ref1.at<cv::Vec3b>(index, i);
                pixel = pix;
            }
        }
    }
    ac_resize(resized, frame, frame.size());
    AddInvert(frame);
}

void ac::MirrorIntertwineRowsX_Y_Width_Height(cv::Mat &frame) {
    static constexpr int num_rows = 720;
    static constexpr int width = 1280;
    static constexpr int height = 720;
    static MatrixCollection<num_rows> collection;
    cv::Mat resized;
    ac_resize(frame, resized, cv::Size(width, height));
    static int off = 1;
    off = (off == 0) ? 1 : 0;
    cv::Mat copy1 = resized.clone();
    if(off == 1) {
        MirrorLeftBottomToTop(copy1);
    } else {
        MirrorRightTopToBottom(copy1);
    }
    collection.shiftFrames(copy1);
    for(int index = 0, pos = 0; index < copy1.rows; index ++, ++pos) {
        cv::Mat &ref1 = collection.frames[pos];
        for(int i = 0; i < frame.cols; ++i) {
            if(index < frame.rows && i < frame.cols) {
                cv::Vec3b &pixel = resized.at<cv::Vec3b>(index, i);
                cv::Vec3b pix = ref1.at<cv::Vec3b>(index, i);
                pixel = pix;
            }
        }
    }
    ac_resize(resized, frame, frame.size());
    AddInvert(frame);
}

void ac::MirrorTrailsLeft(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    MirrorLeft(frames[0]);
    MirrorRight(frames[1]);
    MirrorLeft(frames[2]);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MirrorTrailsLeftRightTopBottom(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    MirrorLeftBottomToTop(frames[0]);
    MirrorRightTopToBottom(frames[1]);
    MirrorLeft(frames[2]);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MirrorTrailsRightLeftBottomTop(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    MirrorRight(frames[0]);
    MirrorLeft(frames[1]);
    MirrorBottomToTop(frames[2]);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.05, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MirrorTrailsFlash(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    static int index = 0;
    switch(index) {
    case 0:
        MirrorLeft(frames[0]);
        MirrorRight(frames[1]);
        MirrorTopToBottom(frames[2]);
        break;
    case 1:
        MirrorRight(frames[0]);
        MirrorLeft(frames[1]);
        MirrorBottomToTop(frames[2]);
        break;
    case 2:
        MirrorLeftBottomToTop(frames[0]);
        MirrorRightTopToBottom(frames[1]);
        MirrorLeft(frames[2]);
        break;
    case 3:
        MirrorLeftBottomToTop(frames[1]);
        MirrorRightTopToBottom(frames[0]);
        MirrorRight(frames[2]);
        break;
    }
    ++index;
    if(index > 3)
        index = 0;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.05, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MirrorLeftTopToBottom(cv::Mat &frame) {
    MirrorLeft(frame);
    MirrorTopToBottom(frame);
    AddInvert(frame);

}

void ac::MirrorRightBottomToTop(cv::Mat &frame) {
    MirrorRight(frame);
    MirrorBottomToTop(frame);
    AddInvert(frame);
}

void ac::MirrorFlipLeft(cv::Mat &frame) {
    FlipBoth(frame);
    MirrorLeftTopToBottom(frame);
    AddInvert(frame);
}

void ac::MirrorFlipRight(cv::Mat &frame) {
    FlipBoth(frame);
    MirrorRightTopToBottom(frame);
    AddInvert(frame);
}

void ac::MirrorFlipBottomLeft(cv::Mat &frame) {
    FlipBoth(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);
}
void ac::MirrorFlipBottomRight(cv::Mat &frame) {
    FlipBoth(frame);
    MirrorRightBottomToTop(frame);
    AddInvert(frame);
}

void ac::MirrorFlipXMirrorLeft(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorLeft(frame);
    AddInvert(frame);
}

void ac::MirrorFlipXMirrorRight(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorRight(frame);
    AddInvert(frame);
}

void ac::MirrorFlipYMirrorLeft(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorLeft(frame);
    AddInvert(frame);
}

void ac::MirrorFlipYMirrorRight(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorRight(frame);
    AddInvert(frame);
}

void ac::MirrorFlipXLeftTopToBottom(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorLeftTopToBottom(frame);
    AddInvert(frame);

}
void ac::MirrorFlipXLeftBottomToTop(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);

}
void ac::MirrorFlipXRightTopToBottom(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorRightTopToBottom(frame);
    AddInvert(frame);
}
void ac::MirrorFlipXRightBottomToTop(cv::Mat &frame) {
    FlipX_Axis(frame);
    MirrorRightBottomToTop(frame);
    AddInvert(frame);

}
void ac::MirrorFlipYLeftTopToBottom(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorLeftTopToBottom(frame);
    AddInvert(frame);

}
void ac::MirrorFlipYLeftBottomToTop(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);

}
void ac::MirrorFlipYRightTopToBottom(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorRightTopToBottom(frame);
    AddInvert(frame);

}
void ac::MirrorFlipYRightBottomToTop(cv::Mat &frame) {
    FlipY_Axis(frame);
    MirrorRightBottomToTop(frame);
    AddInvert(frame);

}

void ac::HorizontalGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int pos = 0;
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = pos; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if((z%4) == 0) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            pos = rand()%(frame.cols-1);
        }
    }
    AddInvert(frame);
}

void ac::VerticalGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int pos = 0;
    int index = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = pos; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if((i%4) == 0) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            pos = rand()%(frame.rows-1);
        }
    }
    AddInvert(frame);
}

void ac::HorizontalXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int pos = 0;
    int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = pos; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j])) ^ static_cast<unsigned char>(((1-alpha) * pix[j]));
            }
        }
        if((z%3) == 0) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            pos = rand()%(frame.cols-1);
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
    AddInvert(frame);
}

void ac::VerticalXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int pos = 0;
    int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = pos; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j])) ^ static_cast<unsigned char>(((1-alpha) * pix[j]));
            }
        }
        if((i%4) == 0) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            pos = rand()%(frame.rows-1);
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
    AddInvert(frame);
}

void ac::Vertical_Horizontal_Glitch(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int pos_x = 0, pos_y = 0;
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = pos_x; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(pos_y, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if((z%4) == 0) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            pos_x = rand()%(frame.cols-1);
            pos_y = rand()%(frame.rows-1);
        }
    }
    AddInvert(frame);
}

void ac::HorizontalSplitGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
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
    }
    AddInvert(frame);
}

void ac::VerticalSplitGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
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
    }
    AddInvert(frame);
}

void ac::HorizontalRandomLine(cv::Mat &frame) {
    static MatrixCollection<48> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int dir = 1;
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
                index = collection.size()-1;
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
}

void ac::VerticalRandomLine(cv::Mat &frame) {
    static MatrixCollection<48> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
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
}

void ac::PixelInterlace(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    int index = 0, dir = 1;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
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
    }
}

void ac::PixelInterlaceColSkip(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    int index = 0, dir = 1;
    int skip = 1+(rand()%25);
    int num = 0;
    static double alpha = 1.0;
    static int a_dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
            }
            ++num;
            if(num > skip) {
                num = 0;
                skip = 1+(rand()%25);;
                if(dir == 1) {
                    ++index;
                    if(index > (collection.size()-1)) {
                        index = collection.size()-1;
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
        }
    }
    AlphaMovementMaxMin(alpha, a_dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::PixelInterlaceRowSkip(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    int index = 0, dir = 1;
    int skip = 1+(rand()%25);
    int num = 0;
    static double alpha = 1.0;
    static int a_dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
            }
            ++num;
            if(num > skip) {
                num = 0;
                skip = 1+(rand()%25);;
                if(dir == 1) {
                    ++index;
                    if(index > (collection.size()-1)) {
                        index = collection.size()-1;
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
        }
    }
    AlphaMovementMaxMin(alpha, a_dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::StartOffsetInterlace(cv::Mat &frame) {
    static MatrixCollection<48> collection;
    collection.shiftFrames(frame);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = rand()%frame.cols; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            ++index;
            if(index > (collection.size()-1)) {
                index = 0;
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                
            }
        }
    }
    AddInvert(frame);
}

void ac::WaveTrails(cv::Mat &frame) {
    static MatrixCollection<48> collection1, collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty())
        collection2.shiftFrames(frame);
    static int index1 = 0, index2 = 0, dir1 = 1, dir2 = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = collection1.frames[index1].at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = collection2.frames[index2].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * pix1[j]) + (0.33 * pix2[j]));
            }
            if(dir1 == 1) {
                ++index2;
                if(index2 > (collection2.size()-1)) {
                    index2 = collection2.size()-1;
                    dir1 = 0;
                }
            } else {
                --index2;
                if(index2 <= 0) {
                    index2 = 0;
                    dir1 = 1;
                }
            }
        }
        if(dir2 == 1) {
            ++index1;
            if(index1 > (collection2.size()-1)) {
                index1 = collection2.size()-1;
                dir2 = 0;
            }
        } else {
            --index1;
            if(index1 <= 0) {
                index1 = 0;
                dir2 = 1;
            }
        }
    }
    static int counter = 0;
    if((++counter%3) == 0)
        collection2.shiftFrames(frame);
    
    AddInvert(frame);
}

void ac::WaveTrailsAura(cv::Mat &frame) {
    WaveTrails(frame);
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::ImageInterlaceWave(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<48> collection1;
    collection1.shiftFrames(frame);
    static int index1 = 0, dir1 = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = collection1.frames[index1].at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (0.4 * pix1[j]) + (0.2 * pix2[j]));
            }
        }
        if(dir1 == 1) {
            ++index1;
            if(index1 > (collection1.size()-1)) {
                index1 = collection1.size()-1;
                dir1 = 0;
            }
        } else {
            --index1;
            if(index1 <= 0) {
                index1 = 0;
                dir1 = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorWaveTrails(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    MatrixCollectionAuraTrails(copy1);
    collection.shiftFrames(copy1);
    int index = 0, dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (pix[j] * 0.7));
            }
        }
        if(dir == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
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
    AddInvert(frame);
}

