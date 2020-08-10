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

void ac::LineInLineOut4_Increase(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 5;
    static MatrixCollection<16> collection;
    static int value_size = 1+(rand()%15);
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
            static int idir = 1;
            if(idir == 1) {
                ++index;
                if(index > collection.size()-2)
                    idir = 0;
            } else {
                --index;
                if(index <= 1)
                    idir = 1;
            }
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            offset += value_size;
            if(offset > max) {
                offset = max;
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 250) {
                        max_dir = 0;
                        value_size = 1+(rand()%15);
                    }
                } else {
                    max -= 1;
                    if(max <= 5)
                        max_dir = 1;
                }
            }
        } else {
            offset -= value_size;
            if(offset <= 1) {
                dir = 1;
                offset = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::LineInLineOut_ReverseIncrease(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 5;
    static MatrixCollection<16> collection;
    static int value_size = 1+(rand()%15);
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                int tpos = frame.cols-pos-1;
                if(i >= 0 && i < frame.cols && tpos >= 0 && tpos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, tpos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
            static int idir = 1;
            if(idir == 1) {
                ++index;
                if(index > collection.size()-2)
                    idir = 0;
            } else {
                --index;
                if(index <= 1)
                    idir = 1;
            }
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            offset += value_size;
            if(offset > max) {
                offset = max;
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 250) {
                        max_dir = 0;
                        value_size = 1+(rand()%15);
                    }
                } else {
                    max -= 1;
                    if(max <= 5)
                        max_dir = 1;
                }
            }
        } else {
            offset -= value_size;
            if(offset <= 1) {
                dir = 1;
                offset = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::LineInLineOut_InvertedY(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                int zpos = frame.rows-z-1;
                if(zpos >= 0 && zpos < frame.rows && i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, zpos, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_ReverseInvertedY(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                int zpos = frame.rows-z-1;
                int rpos = frame.cols-pos-1;
                if(rpos >= 0 && rpos < frame.cols && zpos >= 0 && zpos < frame.rows && i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, zpos, rpos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_Vertical(cv::Mat &frame) {
    static int offset = 0, index = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
             for(int pos=z; pos < z+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.rows && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, pos, i);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > 100)
                dir = 0;
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_VerticalIncrease(cv::Mat &frame) {
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=z; pos < z+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.rows && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, pos, i);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_IncreaseImage(cv::Mat &frame) {
    
    if(blend_set == false)
        return;
    
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    cv::Vec3b rei = pixelAt(reimage,z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
    
}

void ac::LineInLineOut_IncreaseVideo(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reimage;
        ac_resize(vframe, reimage, frame.size());
        static int offset = 0, index = 0, max = 50;
        static MatrixCollection<8> collection;
        collection.shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                for(int pos=i; pos < i+offset; ++pos) {
                    if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b &pixel = pixelAt(frame, z, pos);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                        cv::Vec3b rei = pixelAt(reimage,z, i);
                        pixel = pix;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                        }
                    }
                }
                i += offset;
            }
            static int dir = 1, max_dir = 1;
            if(dir == 1) {
                ++offset;
                if(offset > max) {
                    dir = 0;
                    if(max_dir == 1) {
                        max += 1;
                        if(max > 400)
                            max_dir = 0;
                    } else {
                        max -= 1;
                        if(max <= 50)
                            max_dir = 1;
                    }
                }
            } else {
                --offset;
                if(offset <= 1)
                    dir = 1;
            }
            ++index;
            if(index > collection.size()-1)
                index = 0;
        }
        AddInvert(frame);
    }
}

void ac::LineInLineOut_IncreaseVideo2(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reimage;
        ac_resize(vframe, reimage, frame.size());
        static int offset = 0, index = 0, max = 50;
        static MatrixCollection<8> collection;
        collection.shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                for(int pos=i; pos < i+offset; ++pos) {
                    if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b &pixel = pixelAt(frame, z, pos);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                        cv::Vec3b rei = pixelAt(reimage,z, i);
                        pixel = pix;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                        }
                    }
                }
                i += offset;
                static int dir = 1, max_dir = 1;
                if(dir == 1) {
                    ++offset;
                    if(offset > max) {
                        dir = 0;
                        if(max_dir == 1) {
                            max += 1;
                            if(max > 400)
                                max_dir = 0;
                        } else {
                            max -= 1;
                            if(max <= 50)
                                max_dir = 1;
                        }
                    }
                } else {
                    --offset;
                    if(offset <= 1)
                        dir = 1;
                }
                ++index;
                if(index > collection.size()-1)
                    index = 0;
            }
        }
        AddInvert(frame);
    }
}

void ac::LineInLineOut_IncreaseVideo3(cv::Mat &frame) {
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reimage;
        ac_resize(vframe, reimage, frame.size());
        static int offset = 0, index = 0, max = 50;
        static MatrixCollection<8> collection;
        collection.shiftFrames(frame);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                for(int pos=i; pos < i+offset; ++pos) {
                    if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b &pixel = pixelAt(frame, z, pos);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                        cv::Vec3b rei = pixelAt(reimage,z, pos);
                        pixel = pix;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                        }
                    }
                }
                i += offset;
            }
            static int dir = 1, max_dir = 1;
            if(dir == 1) {
                ++offset;
                if(offset > max) {
                    dir = 0;
                    if(max_dir == 1) {
                        max += 1;
                        if(max > 400)
                            max_dir = 0;
                    } else {
                        max -= 1;
                        if(max <= 50)
                            max_dir = 1;
                    }
                }
            } else {
                --offset;
                if(offset <= 1)
                    dir = 1;
            }
            ++index;
            if(index > collection.size()-1)
                index = 0;
        }
        AddInvert(frame);
    }
}

void ac::LineInLineOut_IncreaseImage3(cv::Mat &frame) {
    if(blend_set == false)
        return;
    
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    cv::Vec3b rei = pixelAt(reimage,z, pos);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                    }
                }
            }
            i += offset;
        }
        static int dir = 1, max_dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = 0;
                if(max_dir == 1) {
                    max += 1;
                    if(max > 400)
                        max_dir = 0;
                } else {
                    max -= 1;
                    if(max <= 50)
                        max_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
        ++index;
        if(index > collection.size()-1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::LineInLineOut_IncreaseImage2(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static int offset = 0, index = 0, max = 50;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int pos=i; pos < i+offset; ++pos) {
                if(i >= 0 && i < frame.cols && pos >= 0 && pos < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame, z, pos);
                    cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                    cv::Vec3b rei = pixelAt(reimage,z, i);
                    pixel = pix;
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.3 * pix[j]) + (0.3 * rei[j]));
                    }
                }
            }
            i += offset;
            static int dir = 1, max_dir = 1;
            if(dir == 1) {
                ++offset;
                if(offset > max) {
                    dir = 0;
                    if(max_dir == 1) {
                        max += 1;
                        if(max > 400)
                            max_dir = 0;
                    } else {
                        max -= 1;
                        if(max <= 50)
                            max_dir = 1;
                    }
                }
            } else {
                --offset;
                if(offset <= 1)
                    dir = 1;
            }
            ++index;
            if(index > collection.size()-1)
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::SquareByRow(cv::Mat &frame) {
    
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    
    static constexpr int SIZE_VALUE=96;
    
    for(int z = 0; z < frame.rows; z += SIZE_VALUE) {
        for(int i = 0; i < frame.cols; i += SIZE_VALUE) {
            for(int x = 0; x < SIZE_VALUE; ++x) {
                int index = rand()%64;
                for(int y = 0; y < SIZE_VALUE; ++y) {
                    if(z+y < frame.rows-1 && i+x < frame.cols-1) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        ++index;
                        if(index > collection.size()-1) {
                            index = 0;
                        }
                        
                        pixel = pix;
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareByRowRev(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static constexpr int SIZE_VALUE=96;
    for(int z = 0; z < frame.rows; z += SIZE_VALUE) {
        for(int i = 0; i < frame.cols; i += SIZE_VALUE) {
            for(int x = 0; x < SIZE_VALUE; ++x) {
                int index = rand()%64;
                int offset = 0;
                for(int y = 0; y < SIZE_VALUE; ++y) {
                    if(z+y < frame.rows-1 && i+x < frame.cols-1 && (frame.cols-(i+x)-1) > 0) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                        cv::Vec3b pix;
                        if((offset%2)==0) {
                            pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        } else {
                            pix = collection.frames[index].at<cv::Vec3b>(z+y, frame.cols-(i+x)-1);
                        }
                        ++index;
                        if(index > collection.size()-1) {
                            index = 0;
                        }
                        
                        pixel = pix;
                        ++offset;
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::SquareByRow2(cv::Mat &frame) {
    static MatrixCollection<48> collection;
    collection.shiftFrames(frame);
    static constexpr int SIZE_VALUE=96;
    for(int z = 0; z < frame.rows; z += SIZE_VALUE) {
        for(int i = 0; i < frame.cols; i += SIZE_VALUE) {
            for(int x = 0; x < SIZE_VALUE; ++x) {
                int index = rand()%32;
                for(int y = 0; y < SIZE_VALUE; ++y) {
                    if(z+y < frame.rows-1 && i+x < frame.cols-1) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                    ++index;
                    if(index > collection.size()-1) {
                        index = 0;
                    }
                }
            }
        }
    }
    RGBStrobeTrails(frame);
    AddInvert(frame);
}

void ac::DivideByValue(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix[8];
            unsigned int total_value[3] = {0};
            for(int j = 0; j < 8; ++j) {
                pix[j] = pixelAt(collection.frames[j], z, i);
                for(int q = 0; q < 3; ++q)
                    total_value[q] += pix[j][q];
            }
            for(int j = 0; j < 3; ++j) {
                total_value[j] /= 5;
                pixel[j] = pixel[j]^total_value[j];
            }
        }
    }
    AddInvert(frame);
}
