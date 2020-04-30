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


void ac::SmoothImageAverageSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "SmoothBlendSubFilter8")
        return;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    Smooth(frame, &collection);
}

void ac::CallSubFilterBlend50(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CallSubFilterBlend50")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::CallSubFilterBlend25(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CallSubFilterBlend25")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.25);
    AddInvert(frame);
    
}
void ac::CallSubFilterBlend75(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CallSubFilterBlend75")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.75);
    AddInvert(frame);
}

void ac::IntertwineColsX2(cv::Mat &frame) {
    static MatrixCollection<64> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static int dir = 1, index = 4;
    IntertwineCols(copy1, &collection1, index);
    IntertwineRows(copy2, &collection2, index);
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::SmoothFrame64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::SmoothFrame32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::MatrixCollectionMedianBlendFilter(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int increase = 2;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Scalar values;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += pix[j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                values[j] /= (collection.size()-1);
                static int val = static_cast<int>(values[j]);
                pixel[j] = pixel[j]^increase^val;
            }
        }
    }
    if(dir == 1) {
        ++increase;
        if(increase > collection.size()-1) {
            dir = 0;
            increase = collection.size()-1;
        }
    } else {
        --increase;
        if(increase <= 2) {
            increase = 2;
            dir = 1;
        }
    }
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MedianBlendIncreaseFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    rainbowBlend(copy1);
    ColorExpand(copy2);
    static int index = 0;
    switch(index) {
        case 0:
            AlphaBlend(copy1, copy2, frame, 0.25);
            break;
        case 1:
            AlphaBlend(copy1, copy2, frame, 0.50);
            break;
        case 2:
            AlphaBlend(copy1, copy2, frame, 0.75);
            break;
    }
    ++index;
    if(index > 2) index = 0;
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::MedianBlendNegate(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        index = 1;
        Negate(frame);
        MedianBlend(frame);
    } else {
        index = 0;
        DarkNegate(frame);
        MedianBlend(frame);
    }
    AddInvert(frame);
}

void ac::BlendWithFrameAndImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "BlendWithFrameAndImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    BlendWithSource(reimage);
    BlendWithImage(copy1);
    AlphaBlendDouble(copy1, copy2, frame, 0.7, 0.3);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ImageIntertwine32(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1,reimage, frame, 0.5);
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::ImageIntertwine16(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1,reimage, frame, 0.5);
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::ImageIntertwine8(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<32> collection;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1,reimage, frame, 0.5);
    IntertwineRows(frame, &collection);
    AddInvert(frame);
}

void ac::ImageIntertwine64X(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<64> collection;
    static int dir = 1, index = 4;
    cv::Mat reimage, copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1,reimage, frame, 0.5);
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

void ac::IntertwineDoubleSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineDoubleSubFilter")
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter,copy2);
    collection1.shiftFrames(copy1);
    collection2.shiftFrames(copy2);
    IntertwineDoubleRows(frame, &collection1, &collection2, 32);
    AddInvert(frame);
}

void ac::IntertwineDoubleImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineDoubleImageSubFilter")
        return;
    static MatrixCollection<64> collection1, collection2;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, reimage);
    collection1.shiftFrames(copy1);
    collection2.shiftFrames(reimage);
    static int dir = 1, index = 0;
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    IntertwineDoubleRows(frame, &collection1, &collection2, index);
    AddInvert(frame);
}

void ac::IntertwineEachRowXSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineEachRowXSubFilter")
        return;
    static MatrixCollection<32> collection1, collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    collection1.shiftFrames(copy1);
    collection2.shiftFrames(copy2);
    static int dir = 1, index = 0;
    if(dir == 1) {
        ++index;
        if(index > 63-1)
            dir = 0;
    } else {
        --index;
        if(index <= 0)
            dir = 1;
    }
    IntertwineDoubleRows(frame, &collection1, &collection2, index);
    AddInvert(frame);
}

void ac::IntertwineGhost(cv::Mat &frame) {
    Intertwine64X(frame);
    BlendWithSource(frame);
    AddInvert(frame);
}

void ac::IntertwineGhost32(cv::Mat &frame) {
    IntertwineRows32(frame);
    IntertwineRowsReverse32(frame);
    BlendWithSource(frame);
    AddInvert(frame);
}

void ac::IntertwineGhostSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineGhostSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    IntertwineRows32(copy1);
    Intertwine64X(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    BlendWithSource(frame);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::BlendWithOldFrame(cv::Mat &frame) {
    static int index = 63;
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), copy2 = collection.frames[index];
    AlphaBlend(copy1, copy2, frame, 0.5);
    static int dir = 1;
    if(dir == 1) {
        --index;
        if(index <= 1) {
            dir = 0;
        }
    } else {
        ++index;
        if(index >= 62) {
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::BlendWith16thFrame(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), copy2 = collection.frames[15];
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::BlendTrailsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "BlendTrailsSubFilter")
        return;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), output;
    for(int i = 1; i < collection.size(); i += 2) {
        cv::Mat reimage = collection.frames[i].clone();
        CallFilter(subfilter, reimage);
        AlphaBlend(copy1, reimage, output, 0.5);
        copy1 = output.clone();
    }
    frame = copy1.clone();
    AddInvert(frame);
}

void ac::LineMoveInOut(cv::Mat &frame) {
    static int index = 62;
    static int dir = 0;
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    cv::Mat copy1 = collection.frames[index].clone();
    AlphaBlend(copy1, copyf, frame, 0.5);
    if(dir == 0) {
        --index;
        if(index <= 1)
            dir = 1;
    } else {
        ++index;
        if(index >= 62)
            dir = 0;
    }
    AddInvert(frame);
}

void ac::MatrixCollectionTrails(cv::Mat &frame) {
    static constexpr int a_size = 8;
    static MatrixCollection<a_size> collection;
    collection.shiftFrames(frame);
    cv::Mat frame_copy[4];
    for(int j = 0, index = 0; j < 3 && index < a_size-1; ++j, index += 2) {
        frame_copy[j] = collection.frames[index].clone();
    }
    cv::Mat copy1 = frame.clone(), copy2;
    for(int i = 1; i < 3; ++i) {
        MovementRGBTrails(frame_copy[i]);
        AlphaBlend(copy1, frame_copy[i], copy2, 0.5);
        copy1 = copy2.clone();
    }
    frame = copy2.clone();
    AddInvert(frame);
}

void ac::MatrixCollectionRandom(cv::Mat &frame) {
    static constexpr int c_size = 16;
    static MatrixCollection<c_size> collection;
    int index = rand()%c_size;
    collection.shiftFrames(frame);
    frame = collection.frames[index].clone();
    AddInvert(frame);
}

void ac::MatrixCollectionRandomSmooth(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixCollectionRandom(copy1);
    SmoothFrame32(copy1);
    MatrixCollectionRandom(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionRandomDouble(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixCollectionRandom(copy1);
    MatrixCollectionRandom(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::MatrixCollectionAddImage(cv::Mat &frame) {
    
    if(blend_set == false)
        return;
    
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Scalar values;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b &val = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] += val[j];
                }
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                values[j] /= collection.size();
                unsigned int val = static_cast<unsigned int>(values[j]);
                pixel[j] = (static_cast<unsigned char>(pixel[j]*0.5) + static_cast<unsigned char>(pix[j]*0.3))^val;
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionAlphaBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionAlphaBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MatrixCollectionRandom(copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::MatrixCollectionImageBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionImageBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), output, reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    MatrixCollectionRandom(copy2);
    AlphaBlend(copy1, copy2, output, 0.33);
    AlphaBlend(reimage, output, frame, 0.33);
    AddInvert(frame);
}

void ac::TwitchinGlitchin(cv::Mat &frame) {
    MatrixCollectionRandom(frame);
    MatrixCollectionTrails(frame);
    AddInvert(frame);
}

void ac::IntertwineRowsImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha = 1.0, alpha_max = 2.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Intertwine64X(copy1);
    SmoothImageAlphaBlend(copy1);
    IntertwineRows32(copy2);
    AlphaBlend(copy1, copy2, frame, alpha);
    procPos(dir, alpha, alpha_max,2.0, 0.005);
    AddInvert(frame);
}

void ac::ChannelSortSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ChannelSortSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    ChannelSort_NoBlend_Ascending(copy1);
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::IntertwineRow2(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    IntertwineRows(frame, &collection, 2);
    AddInvert(frame);
}

// Both Takes a lot of ram.
void ac::IntertwineRow720p(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(1280, 720));
    static MatrixCollection<720> collection;
    IntertwineRows(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineRow1080p(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(1920, 1080));
    static MatrixCollection<1080> collection;
    IntertwineRows(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineRow720pX2(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(1280, 720));
    static MatrixCollection<720> collection;
    IntertwineRows(sizef, &collection, 2);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}
void ac::IntertwineRow1080pX2(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(1920, 1080));
    static MatrixCollection<1080> collection;
    IntertwineRows(sizef, &collection, 2);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineCols1280(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(1280, 720));
    static MatrixCollection<720> collection;
    IntertwineCols(sizef, &collection, 2);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineRowCols1280x720(cv::Mat &frame) {
    static MatrixCollection<1280> collection1;
    static MatrixCollection<720> collection2;
    cv::Mat sizef1, sizef2;
    ac_resize(frame, sizef1, cv::Size(1280, 720));
    ac_resize(frame, sizef2, cv::Size(1280, 720));
    IntertwineRows(sizef1, &collection1, 2);
    IntertwineCols(sizef2, &collection2, 2);
    cv::Mat output;
    AlphaBlend(sizef1, sizef2, output, 0.5);
    ac_resize(output, frame, frame.size());
    AddInvert(frame);
}

void ac::IntertwineRowsImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage, output;
    ac_resize(blend_image, reimage, frame.size());
    AlphaBlend(copy1, reimage,output,0.5);
    cv::Mat col_output;
    resize(output, col_output, cv::Size(1280, 720));
    static MatrixCollection<720> collection1;
    IntertwineRows(col_output, &collection1, 4);
    resize(col_output, frame, frame.size());
    AddInvert(frame);
}

void ac::MedianBlendSquare(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static int index = 0, dir = 1, speed = 80;
    IntertwineRow720pX2(copy2);
    for(int z = 0; z < copy1.rows; ++z) {
        if(dir == 1) {
            for(int i = 0; i < index; ++i) {
                cv::Vec3b col = copy2.at<cv::Vec3b>(z, i);
                cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = pixel[j]^col[j];
            }
        } else {
            for(int i = index; i >= 0; --i) {
                cv::Vec3b col = copy2.at<cv::Vec3b>(z, i);
                cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = pixel[j]^col[j];
            }
        }
    }
    if(dir == 1) {
        index += speed;
        if(index >= copy1.cols-1)
            dir = 0;
    } else {
        index -= speed;
        if(index <= 0)
            dir = 1;
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothIntertwineMedianBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.01, 2.0, 1.0);
    pushSubFilter(getFilterByName("IntertwineRow720pX2"));
    SmoothSubFilter(copy1);
    popSubFilter();
    AlphaBlend(copy1, copy2, frame, 1.0);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothBlendMedian(cv::Mat &frame) {
    pushSubFilter(getFilterByName("Random Filter"));
    SmoothSubFilter32(frame);
    popSubFilter();
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::SmoothDoubleSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "SmoothDoubleSubFilter")
        return;
    static MatrixCollection<16> collection1;
    static MatrixCollection<8> collection2;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    Smooth(copy1, &collection1);
    Smooth(copy2, &collection2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::AlphaBlendMedianSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "AlphaBlendMedianSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    DarkenFilter(copy1);
    DarkenFilter(copy2);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    DarkenFilter(copy1);
    DarkenFilter(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    MedianBlend(frame);
    AddInvert(frame);
}

void ac::ReverseOnOff(cv::Mat &frame) {
    static int state = 0;
    if(state == 0) {
        Reverse(frame);
        state = 1;
    } else {
        state = 0;
    }
    AddInvert(frame);
}

void ac::SmoothReverseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "SmoothReverseSubFilter")
        return;
    static MatrixCollection<16> collection;
    ReverseOnOff(frame);
    Smooth(frame, &collection);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::IntertwineWithSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineWithSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if((z%2) == 0) {
                cv::Vec3b cp1 = copy1.at<cv::Vec3b>(z, i);
                pixel = cp1;
            }
        }
    }
    AddInvert(frame);
}

void ac::IntertwineWithImageAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineWithImageAndSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            switch(index) {
                case 0:
                    break;
                case 1:
                    pixel = img_pix;
                    break;
                case 2:
                    pixel = pix;
                    break;
            }
        }
        ++index;
        if(index > 2)
            index = 0;
    }
    AddInvert(frame);
}

void ac::IntertwineFrameWithImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    int index = rand()%2;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 0) {
                index = 1;
            } else {
                index = 0;
                pixel = reimage.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::InterlaceVerticalFilter(cv::Mat &frame) {
    static int index = 0, start_index = 0;
    if(start_index == 0) {
        start_index = 1;
        index = 0;
    } else {
        start_index = 0;
        index = 1;
    }
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 0) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
        index = (index == 0) ? 1 : 0;
    }
    AddInvert(frame);
}

void ac::InterlaceHorizontalFilter(cv::Mat &frame) {
    static int index = 0, start_index = 0;
    if(start_index == 0) {
        start_index = 1;
        index = 0;
    } else {
        start_index = 0;
        index = 1;
    }
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 0) {
                pixel = cv::Vec3b(0,0,0);
                index = 1;
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
                index = 0;
            }
        }
    }
    AddInvert(frame);
}

void ac::IntertwineImageFlip(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int val = -1;
    cv::Mat recopy = reimage.clone();
    cv::flip(recopy, reimage, val);
    InterlaceFrames(frame, reimage);
    ++val;
    if(val > 1)
        val = -1;
    AddInvert(frame);
}

void ac::IntertwineImageFlipSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineImageFlipSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int val = -1;
    cv::Mat recopy = reimage.clone();
    cv::flip(recopy, reimage, val);
    CallFilter(subfilter, frame);
    CallFilter(subfilter, reimage);
    InterlaceFrames(frame, reimage);
    ++val;
    if(val > 1)
        val = -1;
    
    AddInvert(frame);
}

void ac::IntertwineFlipImageAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineFlipImageAndSubFilter")
        return;
    int flip_codes[3] = {-1,-1,-1};
    flip_codes[0] += rand()%2;
    flip_codes[1] += rand()%2;
    flip_codes[2] += rand()%2;
    cv::Mat filtered = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, filtered);
    cv::Mat copy_input[3], output[3];
    copy_input[0] = frame.clone();
    copy_input[1] = reimage.clone();
    copy_input[2] = filtered.clone();
    for(int i = 0; i < 3; ++i) {
        cv::flip(copy_input[i], output[i], flip_codes[i]);
    }
    InterlaceFrames(frame, output, 3);
    AddInvert(frame);
}

void ac::IntertwineImageOnOff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int on_val = 0;
    static int start_off_on = 0;
    if(start_off_on == 0) {
        on_val = 0;
        start_off_on = 1;
    } else {
        on_val = 1;
        start_off_on = 0;
    }
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on_val == 0)
                pixel = reimage.at<cv::Vec3b>(z, i);
            else
                continue;
        }
        on_val = (on_val == 0) ? 1 : 0;
    }
    AddInvert(frame);
}

void ac::IntertwineVideoShuffle(cv::Mat &frame) {
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> filter_array({"IntertwineRows32", "IntertwineRows16", "IntertwineRows8","Intertwine64X", "IntertwineRowsReverse32", "IntertwineRowsReverse16", "IntertwineRowsReverse8", "IntertwineRowsReverse64X", "IntertwineRowsSwitch32", "IntertwineRowsSwitch16", "IntertwineRowsSwitch8", "IntertwineRows64"});
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

void ac::ImageFlipFilter(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int val = -1;
    cv::Mat recopy = reimage.clone(), copy1 = frame.clone();
    cv::flip(recopy, reimage, val);
    AlphaBlend(copy1, reimage, frame, 0.5);
    ++val;
    if(val > 1)
        val = -1;
    AddInvert(frame);
}

void ac::FlipFrameFilter(cv::Mat &frame) {
    static int val = -1;
    cv::Mat copy1 = frame.clone();
    cv::flip(copy1, frame, val);
    ++val;
    if(val > 1)
        val = -1;
    AddInvert(frame);
}

void ac::AlphaBlendImageAndSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "AlphaBlendImageAndSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), reimage, output;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, output, 0.5);
    AlphaBlend(output,reimage,frame, 0.5);
    AddInvert(frame);
}

void ac::StretchAlphaBlend(cv::Mat &frame) {
    static int offset_x = 1;
    static int offset_y = 1;
    StretchAlphaBlendSelf(frame, 1, 1, offset_x, offset_y);
    AddInvert(frame);
}

void ac::StretchAlphaBlendWidth(cv::Mat &frame) {
    static int offset_x = 1;
    static int offset_y = 1;
    StretchAlphaBlendSelf(frame, 1, 0, offset_x, offset_y);
    AddInvert(frame);

}
void ac::StretchAlphaBlendHeight(cv::Mat &frame) {
    static int offset_x = 1;
    static int offset_y = 1;
    StretchAlphaBlendSelf(frame, 0, 1, offset_x, offset_y);
    AddInvert(frame);
}

void ac::StretchAlphaBlendTwitch(cv::Mat &frame) {
    static int offset_x = 1;
    static int offset_y = 1;
    static int dir = 1;
    StretchAlphaBlendSelf(frame,dir, 1, 0, offset_x, offset_y, 100, 100);
    AddInvert(frame);
}

void ac::StretchAlphaBlednTwitchFast(cv::Mat &frame) {
    static int offset_x = 1;
    static int offset_y = 1;
    static int dir = 1;
    StretchAlphaBlendSelf(frame,dir, 10, 0, offset_x, offset_y, 100, 100);
    AddInvert(frame);
}
// Use as a subfilter for Smooth
void ac::StretchRandomValues(cv::Mat &frame) {
    cv::Mat reimage;
    int r_x = 0, r_y = 0;
    r_x = rand()%(frame.cols-1);
    r_y = rand()%(frame.rows-1);
    if(r_x < 10)
        r_x = 10;
    if(r_y < 10)
        r_y = 10;    
    cv::Size size_val(r_x, r_y);
    ac_resize(frame, reimage, size_val);
    copyMat(frame, reimage);
    AddInvert(frame);
}
