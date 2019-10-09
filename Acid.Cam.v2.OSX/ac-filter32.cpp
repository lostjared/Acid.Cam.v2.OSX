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

void ac::IncreaseRainbowXorBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    
    MedianBlendMultiThreadByEight(frame);
    MedianBlendMultiThread_2160p(frames[1]);
    MedianBlendIncrease(frames[2]);
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j <3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorStrobeIndexSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ColorStrobeIndexSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    static int offset = 0;
    if(++offset > 2)
        offset = 0;
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    CallFilter(subfilter, frames[offset]);
    static double alpha = 1.0;
    static int dir = 1;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pix[j] * (1-alpha)));
                }
            }
        }
    }
    MedianBlendMultiThreadByEight(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

//void copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const Rect &rc);

void ac::MatrixCollectionRandomFrames(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        int p_x = rand()%(frame.cols-1);
        int p_y = rand()%(frame.rows-1);
        if(p_x < 25)
            p_x = 25;
        if(p_y < 25)
            p_y = 25;
        int w = rand()%(frame.cols-1);
        int h = rand()%(frame.rows-1);
        Point p(0, 0);
        Rect rc(p_x, p_y, w-p_x, h-p_y);
        copyMat(collection.frames[index], p, frame, rc);
    }
    AddInvert(frame);
}

void ac::MatrixCollectionFrames(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int p_x = 0, p_y = 0;
    for(int index = 0; index < collection.size(); ++index) {
        int p_w = frame.cols-p_x;
        int p_h = frame.rows-p_y;
        Rect rc(p_x, p_y, p_w, p_h);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        p_x += 20;
        p_y += 20;
        if(p_x > frame.cols-20 || p_y > frame.rows-20)
            break;
    }
    VariableRectanglesExtra(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionFramesLeft(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    int s_x = 0;
    int s_w = frame.cols/Size;
    for(int index = 0; index < collection.size(); ++index) {
        Rect rc(s_x, 0, s_w, frame.rows);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        s_x += s_w;
        if(s_x > frame.cols-s_w)
            break;
    }
    AddInvert(frame);
}

void ac::MatrixCollectionFramesMirrorLeft(cv::Mat &frame) {
    MatrixCollectionFramesLeft(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionFramesTop(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    int s_y = 0;
    int s_h = frame.rows/Size;
    for(int index = 0; index < collection.size(); ++index) {
        Rect rc(0, s_y, frame.cols, s_h);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        s_y += s_h;
        if(s_y > frame.rows-s_h)
            break;
    }
    AddInvert(frame);
}

void ac::FrameSqueeze(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int off_z = 0;
    static int index = 3;
    int cnt = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix;
            pix = copy1.at<cv::Vec3b>(off_z, i);
            pixel = pix;
        }
        if(++cnt > index) {
            cnt = 0;
            off_z += index;
        }
    }
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index > 16) {
            dir = 0;
        }
    } else {
        --index;
        if(index <= 3) {
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionBlendLeftToRight(cv::Mat &frame) {
    static constexpr int Size = 64;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    int s_x = 0;
    int s_w = frame.cols/Size;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = s_x; i < frame.cols-s_x; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix[j] * 0.5));
                }
            }
        }
        s_x += s_w;
        if(s_x > frame.cols-s_x)
            break;
    }
    AddInvert(frame);
}

void ac::MedianBlendMatrixCollection(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
            }
        }
    }
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::MedianBlendMatrixCollection_2160p(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
            }
        }
    }
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::MedianBlendMatrixCollection_2160p_X16(cv::Mat &frame) {
    static constexpr int Size = 16;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.7 * pixel[j]) + (0.3 * pix[j]));
                }
            }
        }
    }
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionAdd(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += pix[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollection_Xor(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] ^= pix[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageMatrixCollectionBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * pix[j]) + (0.33 * img[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionNegateAdd(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += ~pix[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MatrixCollectionFrameRect(cv::Mat &frame) {
    static constexpr int width = 4;
    static constexpr int height = 2;
    static MatrixCollection<width * height> collection;
    MatrixDrawFrames(frame, &collection, width, height, -1);
    AddInvert(frame);
}

void ac::MatrixCollectionRows8x1_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionRows8x1_SubFilter")
        return;
    static constexpr int width = 8;
    static constexpr int height = 1;
    static MatrixCollection<width * height> collection;
    MatrixDrawFrames(frame, &collection, width, height, subfilter);
    AddInvert(frame);
}

void ac::MatrixCollectionRows8x8_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionRows8x8_SubFilter")
        return;
    static constexpr int width = 8;
    static constexpr int height = 8;
    static MatrixCollection<width * height> collection;
    MatrixDrawFrames(frame, &collection, width, height, subfilter);
    AddInvert(frame);
}

void ac::MatrixCollectionRows2x2_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionRows2x2_SubFilter")
        return;
    static constexpr int width = 2;
    static constexpr int height = 2;
    static MatrixCollection<width * height> collection;
    MatrixDrawFrames(frame, &collection, width, height, subfilter);
    AddInvert(frame);
}

void ac::MatrixCollectionRows4x4_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "MatrixCollectionRows4x4_SubFilter")
        return;
    static constexpr int width = 4;
    static constexpr int height = 4;
    static MatrixCollection<width * height> collection;
    MatrixDrawFrames(frame, &collection, width, height, subfilter);
    AddInvert(frame);
}

void ac::IntertwineBlock(cv::Mat &frame) {
    static constexpr int Size = 256;
    static int Rows = frame.rows/Size;
    static MatrixCollection<Size+1> collection;
    collection.shiftFrames(frame);
    int z_offset = 0;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = z_offset; z < z_offset+Rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        z_offset += Rows;
    }
    for(int z = z_offset; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[collection.size()-1].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::IntertwineVertical(cv::Mat &frame) {
    static constexpr int Size = 256;
    static MatrixCollection<Size+1> collection;
    collection.shiftFrames(frame);
    static int Col = frame.cols/Size;
    int off_x = 0;
    for(int index = 0; index < collection.size(); ++index) {
        for(int i = off_x; i < off_x+Col; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        off_x += Col;
    }
    for(int i = off_x; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[collection.size()-1].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::IntertwineImageVerticalSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineImageVerticalSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static constexpr int Size = 64;
    static MatrixCollection<Size+1> collection;
    collection.shiftFrames(frame);
    collection.shiftFrames(reimage);
    static int Col = frame.cols/Size;
    int off_x = 0;
    for(int index = 0; index < collection.size(); ++index) {
        for(int i = off_x; i < off_x+Col; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        off_x += Col;
    }
    
    for(int i = off_x; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[collection.size()-1].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}
