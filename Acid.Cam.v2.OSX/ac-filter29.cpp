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

void ac::ImageMirror_DownSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageMirror_DownSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    cv::Mat copy1 = reimage.clone();
    int halfway = (frame.rows/2);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < halfway; ++z) {
            
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
                if(frame.rows-z-1 > 0) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
        }
    }
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = halfway; z < frame.rows; ++z) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(frame.rows-z-1 > 0) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageMirrorKaleidoscope(cv::Mat &frame) {
    if(blend_set == false)
        return;
    pushSubFilter(getFilterByName("ColorPulseIncrease"));
    ImageMirror_LeftSubFilter(frame);
    popSubFilter();
    pushSubFilter(getFilterByName("ColorTransition"));
    ImageMirror_UpSubFilter(frame);
    popSubFilter();
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
}

void ac::ExactImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ExactImageSubFilter")
        return;
    ExactImage(frame);
    CallFilter(subfilter, frame);
}

void ac::AlphaImageSourceScale(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b opix = pixelAt(orig_frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GhostMirrorReversed(cv::Mat &frame) {
    cv::Mat temp = frame.clone();
    cv::Mat reimage;
    cv::flip(temp, reimage, 1);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                if(i >= 0 && i < frame->cols && z >= 0 && z < frame->rows) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    if(i >= 0 && i < reimage.cols && z >= 0 && z < reimage.rows && i >= 0 && i < orig_frame.cols && z >= 0 && z < orig_frame.rows) {
                        cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                        cv::Vec3b opix = pixelAt(orig_frame,z, i);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    GhostTrails(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GhostMirrorFade(cv::Mat &frame) {
    cv::Mat temp = frame.clone();
    cv::Mat reimage;
    cv::flip(temp, reimage, 1);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                if(i >= 0 && i < frame->cols && z >= 0 && z < frame->rows) {
                    if(i >= 0 && i < reimage.cols && z >= 0 && z < reimage.rows && i >= 0 && i < orig_frame.cols && z >= 0 && z < orig_frame.rows) {
                        cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                        cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                        cv::Vec3b opix = pixelAt(orig_frame,z, i);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    BlendForwardAndBack64(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MirrorFadeLeftRight(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::FadeMirrorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "FadeMirrorSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    CallFilter(subfilter, copy1);
    MirrorRight(copy2);
    CallFilter(subfilter, frame);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MirrorFadeUpDown(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorTopToBottom(copy1);
    MirrorBottomToTop(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::DarkTrailsKaleidoscope(cv::Mat &frame) {
    GhostMirrorFade(frame);
    MirrorFadeLeftRight(frame);
    MirrorFadeUpDown(frame);
    MedianBlendDark(frame);
    AddInvert(frame);
}

void ac::MirrorFadeAll(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    MirrorFadeLeftRight(copy1);
    MirrorFadeUpDown(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::KaleidoscopeMirrorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "KaleidoscopeMirrorSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorRightTopToBottom(copy2);
    MirrorLeftBottomToTop(copy1);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageKaleidoscopeMirrorAlphaSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageKaleidoscopeMirrorAlphaSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3;
    pushSubFilter(subfilter);
    ImageMirror_LeftSubFilter(copy1);
    popSubFilter();
    pushSubFilter(subfilter);
    ImageMirror_RightSubFilter(copy2);
    popSubFilter();
    AlphaBlendDouble(copy1, copy2, copy3, alpha,(1-alpha));
    AlphaBlendDouble(ac::orig_frame, copy3, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::BlendOppositesSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "BlendOppositesSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    cv::Mat copy3, copy4;
    MirrorLeft(copy1);
    MirrorRight(copy2);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copy1, copy2, copy3, alpha, (1-alpha));
    AlphaBlendDouble(copy1, copy2, copy4, (1-alpha), alpha);
    AlphaBlendDouble(copy3, copy4, frame, 0.5, 0.5);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::AlphaBlendMirrorFade(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    AlphaBlendImageWithOrigSource(copy1);
    MirrorLeft(copy1);
    AlphaBlendWithSource(copy2);
    AlphaBlendImageWithOrigSource(copy2);
    MirrorRight(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::DarkenMedianBlend(cv::Mat &frame) {
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ReduceMedianBlend(cv::Mat &frame) {
    ReduceBy50(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::DarkColors(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Vec3b value(rand()%255, rand()%255, rand()%255);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ( (value[j]^pixel[j]) ));
                    pixel[j] /= 3;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::DarkenChannelXorNoMedian(cv::Mat &frame) {
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b mulpix = pixel;
                for(int j = 0; j < 3; ++j) {
                    mulpix[j] /= 2;
                    pixel[j] = pixel[j] ^ mulpix[j];
                }
            }
        }
    };
    NoMedianBlurBlendMultiThread(frame);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::DarkenStrobeMedianBlend(cv::Mat &frame) {
    static int offset = 0;
    int colors[3] = {0};
    InitArrayPosition(colors, offset);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[colors[0]] /= 2;
                pixel[colors[1]] /= 3;
                pixel[colors[2]] /= 4;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++offset;
    if(offset > 2)
        offset = 0;
    NoMedianBlurBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::DarkenImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix[j] /= 3;
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pix[j] * (1-alpha)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MatrixStrobeTrails(cv::Mat &frame) {
    static int col[3] = {0};
    static int index = 0;
    InitArrayPosition(col, index);
    ++index;
    if(index > 2)
        index = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[col[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MatrixStrobeMedianBlend(cv::Mat &frame) {
    MatrixStrobeTrails(frame);
    NoMedianBlurBlendMultiThread(frame);
}

void ac::ImageEnergyKaleidoscopeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergyKaleidoscopeSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = reimage.clone();
    frames[2] = copy1.clone();
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageEnergyKaleidoscopeBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergyKaleidoscopeBlendSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b cp_pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    double alpha_val = (alpha/4);
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha_val) + (pix[j] * alpha_val) + (img_pix[j] * (alpha_val)) + (cp_pix[j] * (alpha_val)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::ImageEnergyKaleidoscopeEvenSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergyKaleidoscopeBlendSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[15].clone();
    frames[2] = collection.frames[31].clone();
    CallFilter(subfilter, reimage);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b cp_pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    double alpha_val = (alpha/2);
                    pixel[j] = static_cast<unsigned char>(((pixel[j] * alpha_val) + (pix[j] * alpha_val)) + ((img_pix[j] * (1-alpha_val)) + (cp_pix[j] * (1-alpha_val))));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.05, 1.0, 0.1);
}

void ac::ImageEnergyFadeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergyFadeSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    CallFilter(subfilter, reimage);
    collection.shiftFrames(reimage);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] =  static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageEnergizeBlendFilter(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    AlphaBlendDouble(copy1, reimage, copy2, alpha, (1-alpha));
    pushSubFilter(getFilterByName("ColorTransition"));
    EnergizeSubFilter(copy2);
    popSubFilter();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy2.at<cv::Vec3b>(z, i);
                cv::Vec3b repix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    double s_alpha = (alpha/3);
                    pixel[j] = static_cast<unsigned char>((s_alpha * pixel[j]) + (repix[j] * s_alpha) - (pix[j] * s_alpha));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.4);
}

void ac::ImageEnergizeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergizeSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    AlphaBlendDouble(copy1, reimage, copy2, alpha, (1-alpha));
    pushSubFilter(subfilter);
    EnergizeSubFilter(copy2);
    popSubFilter();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy2.at<cv::Vec3b>(z, i);
                cv::Vec3b repix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    double s_alpha = (alpha/3);
                    pixel[j] = static_cast<unsigned char>((s_alpha * pixel[j]) + (repix[j] * s_alpha) - (pix[j] * s_alpha));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.4);
}

void ac::MatrixCollectionSmoothedSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionSmoothedSubFilter")
        return;
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    cv::Mat frames[3];
    frames[2] = collection.frames[1];
    frames[1] = collection.frames[4];
    frames[0] = collection.frames[7];
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix_s = frames[j].at<cv::Vec3b>(z, i);
                    int ival = static_cast<int>((pixel[j] * 0.5));
                    int sval = static_cast<int>((pix_s[j] * 0.5));
                    pixel[j] = static_cast<unsigned char>(ival ^ sval);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}


void ac::MonoColorTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[0];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::TremorMonoColorTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static int index = 0;
    int col[3] = {0};
    InitArrayPosition(col, index);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[col[j]] = pix[0];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2)
        index = 0;
}

void ac::StrobeMonoColorTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static int index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[index];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2)
        index = 0;
}

void ac::StrobeEffectColorTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static int index = 0;
    int col[3] = {0};
    InitArrayPosition(col, index);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[col[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2)
        index = 0;
}

void ac::PixelStrobeColorTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static int index = 0;
    int col1[3] = {0}, col2[3] = {0};
    InitArrayPosition(col1, index);
    InitArrayPosition(col2, 3-index-1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[col1[j]] = pix[col2[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2)
        index = 0;
}

void ac::ElectricImageFilter(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    double alpha_s = alpha/3;
                    pixel[j] = static_cast<unsigned char>(((alpha_s * pix[j])) - (alpha_s * pixel[j]) + ((1-alpha_s) * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    ColorTransition(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.4);
}

void ac::ImageEnergyStrobeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || ac::draw_strings[subfilter] == "ImageEnergyStrobeSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat iframe[3];
    iframe[0] = reimage.clone();
    iframe[1] = frame.clone();
    iframe[2] = copy1.clone();
    static int index = 0;
    int col[3] = {0};
    InitArrayPosition(col, index);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = iframe[col[j]].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    ++index;
    if(index > 2)
        index = 0;
}

void ac::Twin(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int index = 63;
    static double alpha = 1.0;
    static int dir = 1, dir_x = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b col_pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * col_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir_x == 0) {
        --index;
        if(index < 1) {
            index = 1;
            dir_x = 1;
        }
    } else {
        ++index;
        if(index > 62) {
            index = 62;
            dir_x = 0;
        }
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.4);
    AddInvert(frame);
}

void ac::TwinKaleidoscope(cv::Mat &frame) {
    Twin(frame);
    MirrorRightTopToBottom(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread_2160p(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlendMultiThread(frame,&collection, 2);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadByFour(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlendMultiThread(frame, &collection, 4);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadByEight(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlendMultiThread(frame, &collection, 8);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadByTweleve(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlendMultiThread(frame, &collection, 12);
    AddInvert(frame);
}

void ac::MedianBlendMultiThreadByThree(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlendMultiThread(frame, &collection, 3);
    AddInvert(frame);
}

void ac::MedianBlendIncrease(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static int index = 2, dir = 1;
    MedianBlendMultiThread(frame, &collection, index);
    if(dir == 1) {
        ++index;
        if(index > 12) {
            index = 12;
            dir = 0;
        }
    } else {
        --index;
        if(index < 2) {
            index = 2;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MedianBlendIncrease16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    static int index = 2, dir = 1;
    MedianBlendMultiThread(frame, &collection, index);
    if(dir == 1) {
        ++index;
        if(index > 16) {
            index = 16;
            dir = 0;
        }
    } else {
        --index;
        if(index < 2) {
            index = 2;
            dir = 1;
        }
    }
    AddInvert(frame);
}

