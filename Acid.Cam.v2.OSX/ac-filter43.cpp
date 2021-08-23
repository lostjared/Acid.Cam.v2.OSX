
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

void ac::VideoImageBlendAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static double alpha[3] = {0.1, 0.2, 0.3};
        static int dir[3] = {1,1,1};
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = pixelAt(reframe,z, i);
                cv::Vec3b pix2 = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha[0] * pixel[j]) + (alpha[1] * pix1[j]) + (alpha[2] * pix2[j]));
                }
            }
        }
        for(int j = 0; j < 3; ++j)
            AlphaMovementMaxMin(alpha[j], dir[j], 0.01, 0.4, 0.1);
    }
    
    AddInvert(frame);
}

void ac::IntertwineCols640(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection;
    IntertwineCols(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::HorizontalOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "HorizontalOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::VerticalOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VerticalOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::SquareOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    int start_y = rand()%frame.rows;
    int stop_y = rand()%frame.rows;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = start_y; z < stop_y; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(on == true) {
                    cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                    pixel = pix;
                }
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::SquareOffset_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareOffset_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = true;
    static int counter = 0;
    int start_x = rand()%frame.cols;
    int stop_x = rand()%frame.cols;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                if(on == true) {
                    cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                    pixel = pix;
                }
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            if(on == false && (rand()%250)==0) {
                on = !on;
            } else {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::PrevFrameNotEqual(cv::Mat &frame) {
    static cv::Mat prev = frame.clone();
    if((prev.size() != frame.size()) || (reset_alpha == true)) {
        prev = frame.clone();
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b p = prev.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(pixel[j] >= p[j] && pixel[j] <= p[j]+getPixelCollection())
                    pixel[j] = p[j];
            }
        }
    }
    prev = frame.clone();
    AddInvert(frame);
}

void ac::MirrorDelayLeft(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat &copy = collection.frames[7];
    MirrorLeft(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}
void ac::MirrorDelayRight(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat &copy = collection.frames[7];
    MirrorRight(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::ApplyColorRange(cv::Mat &frame) {
    if(getColorRangeEnabled()==true) {
        applyColorRange(frame);
    }
}
void ac::IntertwineAlphaBlend(cv::Mat &frame) {
    cv::Mat sizef;
    cv::Mat copy1 = frame.clone();
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<360> collection;
    IntertwineRows(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    ac_resize(copy1, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection2;
    IntertwineCols(sizef, &collection2, 1);
    ac_resize(sizef, copy1, copy1.size());
    cv::Mat out;
    AlphaBlend(frame, copy1, out, 0.5);
    frame = out.clone();
    AddInvert(frame);
}

void ac::BlackLines(cv::Mat &frame) {
    static int depth = rand()%50;
    cv::Mat copy1 = frame.clone();
    static int counter = 0;
    static int counter_max = rand()%70;
    static int frame_counter = 0;
    static bool on = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b d;
            if(i > depth && i+depth < frame.cols)
                d = copy1.at<cv::Vec3b>(z, i+depth);
            else
                d = cv::Vec3b(0, 0, 0);
            if(on == true) {
                if(i+depth < frame.cols) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * d[j]));
                    }
                } else {
                    pixel = cv::Vec3b(0, 0, 0);
                }
            }
            if(i < depth) {
                d = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * d[j]));
                }
            }
        }
        if(frame_counter == 0) {
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                depth = rand()%50;
                counter_max = rand()%70;
                on = !on;
            }
        }
    }
    ++frame_counter;
    if(frame_counter > 1) {
        frame_counter = 0;
    }
    AddInvert(frame);
}

void ac::MedianBlendMultiThread2(cv::Mat &frame) {
    static MatrixCollection<2> collection;
    int r = 1+rand()%2;
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

void ac::MedianBlendMultiThread2_ColorChange(cv::Mat &frame) {
    ColorAddBlend(frame);
    ColorTransition(frame);
    MedianBlendMultiThread2(frame);
    AddInvert(frame);
}

void ac::VariablesExtraHorizontal(cv::Mat &frame) {
    int total_lines = frame.cols-2;
    int current_line = 0;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        wait = 0;
    } else {
        return;
    }
    
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int i = current_line; i < current_line+rand_height; ++i) {
            if(current_line > total_lines)
                break;
            for(int z = 0; z < frame.rows-1; ++z) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::ChannelSortDelay(cv::Mat &frame) {
    static cv::Mat copy1 = frame.clone();
    if(copy1.size() != frame.size() || reset_alpha == true)
        copy1 = frame.clone();
    cv::Mat cp = frame.clone();
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        ChannelSort_NoBlend_Descending(cp);
        MedianBlendMultiThread(cp);
        copy1 = cp.clone();
        wait = 0;
    }
    cv::Mat out;
    AlphaBlendDouble(copy1, frame, out, 0.3, 0.7);
    frame = out.clone();
    AddInvert(frame);
}

void ac::DizzyMode(cv::Mat &frame) {
    static cv::Mat copy1 = frame.clone();
    if(copy1.size() != frame.size() || reset_alpha == true)
        copy1 = frame.clone();
    cv::Mat cp = frame.clone();
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        copy1 = cp.clone();
        wait = 0;
    }
    cv::Mat out;
    AlphaBlendDouble(copy1, frame, out, 0.5, 0.5);
    frame = out.clone();
    AddInvert(frame);
}


void ac::rotate_image(cv::Mat &src, cv::Mat &dst, double angle)
{
    cv::Point2f pt(src.cols/2., src.rows/2.);
    cv::Mat r = getRotationMatrix2D(pt, angle, 1.0);
    cv::warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}


void ac::IntertwineColormap(cv::Mat &frame) {
    if(getColorRangeEnabled() == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    applyColorRange(copy1);
    ApplyColorRangeInverted(copy1);
    AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    AddInvert(frame);
}

void ac::RotateFrame(cv::Mat &frame) {
    static double angle = 1.0;
    cv::Mat out;
    ac::rotate_image(frame, out, angle);
    frame = out.clone();
    AddInvert(frame);
    angle += 10.0;
    if(angle > 360)
        angle = 1;
    AddInvert(frame);
}

void ac::GhostShift(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int i = 0; i < collection.size(); ++i) {
        cv::Mat copy1 = collection.frames[i].clone();
        VariablesExtraHorizontal(copy1);
        VariableRectanglesExtra(copy1);
        cv::Mat out;
        AlphaBlendDouble(frame, copy1, out, 0.6, 0.4);
        frame = out.clone();
    }
    AddInvert(frame);
}

void ac::RotateSet(cv::Mat &frame) {
    cv::Mat out = frame.clone();
    RotateFrame(out);
    cv::Mat copy1;
    cv::resize(out, copy1, cv::Size(static_cast<int>(frame.cols*3.5), static_cast<int>(frame.rows*3.5)));
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(z+(copy1.rows/3) < copy1.rows-1 && i+(copy1.cols/3) < copy1.cols-1) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z+(copy1.rows/3), i+(copy1.cols/3));
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::RotateFrameReverse(cv::Mat &frame) {
    static double angle = 360.0;
    cv::Mat out;
    ac::rotate_image(frame, out, angle);
    frame = out.clone();
    AddInvert(frame);
    angle -= 10.0;
    if(angle <= 1)
        angle = 360;
    AddInvert(frame);
}

void ac::RotateSetReverse(cv::Mat &frame) {
    cv::Mat out = frame.clone();
    RotateFrameReverse(out);
    cv::Mat copy1;
    cv::resize(out, copy1, cv::Size(static_cast<int>(frame.cols*3.5), static_cast<int>(frame.rows*3.5)));
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(z+(copy1.rows/3) < copy1.rows-1 && i +(copy1.cols/3) < copy1.cols-1) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z+(copy1.rows/3), i+(copy1.cols/3));
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::AuraGhostRotate(cv::Mat &frame) {
    RotateSet(frame);
    MatrixCollectionAuraTrails(frame);
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::AuraGhostRotateReverse(cv::Mat &frame) {
    RotateSetReverse(frame);
    MatrixCollectionAuraTrails(frame);
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::AuraGhostCollectionEnergy(cv::Mat &frame) {
    static int wait = 0;
    static int dir = 1;
    ++wait;
    if(wait > getVariableWait() * static_cast<int>(ac::fps)) {
        dir = (dir == 0) ? 1 : 0;
        wait = 0;
    }
    if(dir == 1)
        AuraGhostRotate(frame);
    else
        AuraGhostRotateReverse(frame);
    CollectionEnergy(frame);
}

void ac::BlendSourceImageAndVideo(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        static double alpha = 1.0;
        static int dir = 1;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.5) + ((pix1[j] * alpha) + (pix2[j] * (0.5-alpha)));
                }
            }
        }
        AlphaMovementMaxMin(alpha, dir, 0.01, 0.5, 0.1);
    }
    AddInvert(frame);
}

void ac::SetImageAndVideoBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        static double alpha = 1.0;
        static int dir = 1;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix1 = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pix1[j] * alpha) + (pix2[j] * (1.0-alpha)));
                }
            }
        }
        AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    }
    AddInvert(frame);
}

void ac::ThresholdDark(cv::Mat &frame) {
    int color_counter = getColorLevel();
    cv::Mat out,dst;
    cv::cvtColor(frame, out, cv::COLOR_BGR2GRAY);
    cv::threshold(out, dst, color_counter, 255, 1);
    cv::cvtColor(dst, frame, cv::COLOR_GRAY2BGR);
    AddInvert(frame);
}

void ac::ThresholdMedianBlend(cv::Mat &frame) {
    int color_counter = getColorLevel();
    cv::Mat out,dst;
    cv::cvtColor(frame, out, cv::COLOR_BGR2GRAY);
    cv::threshold(out, dst, color_counter, 255, 1);
    cv::cvtColor(dst, frame, cv::COLOR_GRAY2BGR);
    BlendWithSource50(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::Threshold(cv::Mat &frame) {
    int color_counter = getColorLevel();
    cv::Mat out,dst;
    cv::cvtColor(frame, out, cv::COLOR_BGR2GRAY);
    cv::threshold(out, dst, color_counter, 255, 3);
    cv::cvtColor(dst, frame, cv::COLOR_GRAY2BGR);
    AddInvert(frame);
}

void ac::RemoveLowRedLevel(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[2] <= getColorLevel())
                pixel[2] = 0;
        }
    }
    AddInvert(frame);
}

void ac::RemoveLowGreenLevel(cv::Mat &frame)  {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[1] <= getColorLevel())
                pixel[1] = 0;
        }
    }
    AddInvert(frame);

}
void ac::RemoveLowBlueLevel(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[0] <= getColorLevel())
                pixel[0] = 0;
        }
    }
    AddInvert(frame);
}

void ac::IncreaseLowRedLevel(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[2] <= getColorLevel())
                pixel[2] *= 2;
        }
    }
    AddInvert(frame);
}

void ac::IncreaseLowGreenLevel(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[1] <= getColorLevel())
                pixel[1] *= 2;
        }
    }
    AddInvert(frame);
}

void ac::IncreaseLowBlueLevel(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(pixel[0] <= getColorLevel())
                pixel[0] *= 2;
        }
    }
    AddInvert(frame);
}

void ac::Zoom(cv::Mat &frame) {
   static double zoom_x = 2, zoom_y = 2;
    cv::Mat copy1;
    cv::resize(frame, copy1, cv::Size(static_cast<int>(frame.cols*zoom_x), static_cast<int>(frame.rows*zoom_y)));
    int y = 0;
    for(int z = 0; z < frame.rows; ++z) {
        int x = 0;
        for(int i = 0; i < frame.cols; ++i) {
            if(x < copy1.cols && y < copy1.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
                pixel = pix;
            }
            ++x;
        }
        ++y;
    }
    static int dir1 = 1, dir2 = 1;
    AlphaMovementMaxMin(zoom_x, dir1, 0.05, 5, 1.0);
    AlphaMovementMaxMin(zoom_y, dir2, 0.05, 5, 1.0);
    AddInvert(frame);
}

void ac::IntertwineVideo640(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, sizef.size());
        collection.shiftFrames(reframe);
    }
    IntertwineCols(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineCols640x8(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection;
    for(int i = 0; i < 7; ++i)
        collection.shiftFrames(sizef);
    IntertwineCols(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::ZoomCorner(cv::Mat &frame) {
    static double zoom_x = 2, zoom_y = 2;
    cv::Mat copy1;
    cv::resize(frame, copy1, cv::Size(static_cast<int>(frame.cols*zoom_x), static_cast<int>(frame.rows*zoom_y)));
    int y = copy1.rows-frame.rows-2;
    for(int z = 0; z < frame.rows; ++z) {
        int x = copy1.cols-frame.cols-2;
        for(int i = 0; i < frame.cols; ++i) {
            if(x < copy1.cols && y < copy1.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
                pixel = pix;
            }
            ++x;
        }
        ++y;
    }
    static int dir1 = 1, dir2 = 1;
    AlphaMovementMaxMin(zoom_x, dir1, 0.05, 5, 1.0);
    AlphaMovementMaxMin(zoom_y, dir2, 0.05, 5, 1.0);
    AddInvert(frame);
}

void ac::ZoomRandom(cv::Mat &frame) {
    static double zoom_x = 2, zoom_y = 2;
    cv::Mat copy1;
    cv::resize(frame, copy1, cv::Size(static_cast<int>(frame.cols*zoom_x), static_cast<int>(frame.rows*zoom_y)));
    int start_x = rand()%(1+(copy1.cols-frame.cols)), start_y = rand()%(1+(copy1.rows-frame.rows));
    int y = start_y;//copy1.rows-frame.rows-2;
    for(int z = 0; z < frame.rows; ++z) {
        int x = start_x;//copy1.cols-frame.cols-2;
        for(int i = 0; i < frame.cols; ++i) {
            if(x < copy1.cols && y < copy1.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
                pixel = pix;
            }
            ++x;
        }
        ++y;
    }
    static int dir1 = 1, dir2 = 1;
    AlphaMovementMaxMin(zoom_x, dir1, 0.05, 5, 1.0);
    AlphaMovementMaxMin(zoom_y, dir2, 0.05, 5, 1.0);
    AddInvert(frame);
    static int dir_x = 1;
    if(dir_x == 1) {
        ++start_x;
        ++start_y;
    } else {
        --start_x;
        --start_y;
    }
    if(start_x < 0 || start_y < 0) {
        dir_x = 1;
    }
}

void ac::MedianBlendByLowValue(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 2;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection.size()/3; ++j) {
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

void ac::MedianBlendByIncreasingValue(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    static int index = 1;
    for(int i = 0; i < index/4; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < index; ++j) {
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
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index >= 15) {
            index = 15;
            dir = 0;
        }
    } else {
        --index;
        if(index <= 1) {
            index = 1;
            dir = 1;
        }
    }
}

void ac::AuraZoomMedianBlend(cv::Mat &frame) {
    AuraGhostRotate(frame);
    ZoomRandom(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::OffTrackX(cv::Mat &frame) {
    int offset = rand()%frame.cols;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, offset);
            pixel = pix;
            ++offset;
            if(offset > frame.cols-1) offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::OffTrackY(cv::Mat &frame) {
    int offset = rand()%frame.rows;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(offset, i);
            pixel = pix;
        }
        ++offset;
        if(offset > frame.rows-1) offset = 0;
    }
    AddInvert(frame);
}

void ac::FrameAlphaInsert(cv::Mat &frame) {
    static cv::Mat stored_frame = frame.clone();
    if(stored_frame.size() != frame.size())
        stored_frame = frame.clone();
    cv::Mat copy1 = frame.clone();
    StretchRowMatrix8(frame);
    StretchColMatrix8(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(stored_frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (0.6 * pix[j]));
            }
        }
    }
    static int frame_counter = 0;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps) * getVariableWait()) {
        stored_frame = copy1.clone();
        frame_counter = 0;
    }
    AddInvert(frame);
}

void ac::FrameAlphaInsert_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FrameAlphaInsert_SubFilter")
        return;
    static cv::Mat stored_frame = frame.clone();
    if(stored_frame.size() != frame.size())
        stored_frame = frame.clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(stored_frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (0.6 * pix[j]));
            }
        }
    }
    static int frame_counter = 0;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps) * getVariableWait()) {
        stored_frame = copy1.clone();
        frame_counter = 0;
    }
    AddInvert(frame);
}

void ac::FrameAlphaBlend_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "FrameAlphaBlend_SubFilter")
        return;
    static cv::Mat stored_frame = frame.clone();
    if(stored_frame.size() != frame.size())
        stored_frame = frame.clone();
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(stored_frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (0.6 * pix[j]));
            }
        }
    }
    CallFilter(subfilter, frame);
    static int frame_counter = 0;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps) * getVariableWait()) {
        stored_frame = copy1.clone();
        frame_counter = 0;
    }
    AddInvert(frame);
}

void ac::SlowTrails_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlowTrails_SubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    cv::Mat out;
    for(int i = 0; i < collection.size(); ++i) {
        cv::Mat copy2 = collection.frames[i].clone();
        CallFilter(subfilter, copy2);
        AlphaBlendDouble(copy1, copy2, out, 0.3, 0.7);
        copy1 = out.clone();
    }
    frame = copy1.clone();
    AddInvert(frame);
}


