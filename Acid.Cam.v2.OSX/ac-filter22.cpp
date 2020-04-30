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


void ac::ColorCollectionSubtleStrobe(cv::Mat &frame) {
    static bool image_on = true;
    if(image_on == true) {
        image_on = false;
        return;
    } else {
        image_on = true;
    }
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat values[3];
    values[0] = collection.frames[1].clone();
    values[1] = collection.frames[3].clone();
    values[2] = collection.frames[6].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = values[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::CollectionRandom(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    unsigned int value = rand()%(collection.size()-1);
    frame = collection.frames[value].clone();
    AddInvert(frame);
}

void ac::CollectionRandomSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CollectionRandomSubFilter")
        return;
    static MatrixCollection<8> collection;
    static int index1 = 0, dir1= 1;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = collection.frames[index1].clone().clone();
    static double alpha = 1.0;
    static int dir = 1;
    AlphaMovementMaxMin(alpha, dir, 0.005, 2.0, 1.0);
    CallFilter(subfilter, copy1);
    AlphaBlend(copy1, copy2, frame, alpha);
    AddInvert(frame);
    if(dir1 == 1) {
        ++index1;
        if(index1 > 6) {
            dir = 0;
            index1 = 6;
        }
    } else {
        --index1;
        if(index1 <= 1) {
            dir = 1;
            index1 = 1;
        }
    }
}

void ac::CollectionImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    static double alpha = 1.0;
    static int dir = 1;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = img[j]+pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 2.0, 1.0);
}

void ac::CollectionAlphaXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorCollection64X(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int index = 0, dir = 1;
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[(collection.size()-1)/2].clone();
    frames[2] = collection.frames[index].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > 60) {
            index = 60;
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

void ac::ColorCollectionSwitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0, dir = 1;
    cv::Mat frames[4];
    switch(index) {
        case 0:
            frames[0] = collection.frames[1].clone();
            frames[1] = collection.frames[collection.size()/2].clone();
            frames[2] = collection.frames[collection.size()-1].clone();
            break;
        case 1:
            frames[0] = collection.frames[collection.size()-1].clone();
            frames[1] = collection.frames[1].clone();
            frames[2] = collection.frames[collection.size()/2].clone();
            break;
        case 2:
            frames[0] = collection.frames[collection.size()/2].clone();
            frames[1] = collection.frames[collection.size()-1].clone();
            frames[2] = collection.frames[1].clone();
            break;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b color = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = color[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > 2) {
            index = 2;
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

void ac::ColorCollectionRGB_Index(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[2].clone();
    frames[2] = collection.frames[3].clone();
    static int index = 0, dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = frames[index].at<cv::Vec3b>(z, i);
                pixel[index] = pix[index];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > 2) {
            index = 2;
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

void ac::ColorCollectionRGBStrobeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ColorCollectionRGBStrobeSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    static int index = 0, dir = 1;
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    CallFilter(subfilter, frames[index]);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = frames[index].at<cv::Vec3b>(z, i);
                pixel[index] = pix[index];
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > 2) {
            index = 2;
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

void ac::ColorCollectionGhostTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[8].clone();
    frames[2] = collection.frames[15].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::ColorCollectionScale(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int dir = 1;
    static double alpha = 1.0;
    static int index = 0;
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[8].clone();
    frames[2] = collection.frames[15].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
                pixel[index] = static_cast<unsigned char>(pixel[index]*alpha);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 2.0, 1.0);
    AddInvert(frame);
    ++index;
    if(index > 2) {
        index = 0;
    }

}

void ac::ColorCollectionReverseStrobe(cv::Mat &frame) {
    static int index_on = 1;
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
                    cv::Vec3b value;
                    if(index_on == 0)
                        value = frames[3-j-1].at<cv::Vec3b>(z, i);
                    else
                        value = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = value[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(index_on == 1) {
        index_on = 0;
    } else {
        index_on = 1;
    }
}

void ac::CollectionAlphaBlend_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ColorCollectionXorOffsetFlash")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
    
}

void ac::ColorCollectionXorPixel(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    static int index_on = 0, dir = 1;
    static double alpha = 1.0;
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b value;
                    if((index_on%2) == 0)
                        value = frames[3-j-1].at<cv::Vec3b>(z, i);
                    else
                        value = frames[j].at<cv::Vec3b>(z, i);
                    
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) ^ static_cast<unsigned char>(value[j]*alpha);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index_on;
    AlphaMovementMaxMin(alpha,dir,0.005,2.0,1.0);
}

void ac::BlendWithSource25(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, orig_frame, frame, 0.75, .25);
        AddInvert(frame);
    }
}

void ac::BlendWithSource50(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, orig_frame, frame, 0.50, 0.50);
        AddInvert(frame);
    }
}

void ac::BlendWithSource75(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, orig_frame, frame, 0.25, 0.75);
        AddInvert(frame);
    }
}

void ac::BlendWithSource100(cv::Mat &frame) {
    if(!orig_frame.empty() && orig_frame.size() == frame.size()) {
        cv::Mat copy1 = frame.clone();
        AlphaBlend(copy1, orig_frame, frame, 1.0);
        AddInvert(frame);
    }
}

void ac::ColorCollectionXorOffsetFlash(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int offset_value = 0;
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[14].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    if(offset_value == j)
                        pixel[j] = pixel[j]^pix[offset_value];
                    else
                        pixel[j] = pix[j];
                }
            
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    static int counter = 0;
    ++counter;
    if((counter%30)==0) {
        ++offset_value;
        if(offset_value > 2)
            offset_value = 0;
    }
}

void ac::ColorCollectionMatrixGhost(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    GhostTrails(copy1);
    collection.shiftFrames(copy1);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[16].clone();
    frames[2] = collection.frames[31].clone();
    static int index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
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

void ac::MildStrobe(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[0].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[6].clone();
    static int index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                int values[3] = {0,0,0};
                switch(index) {
                    case 0:
                        values[0] = 0;
                        values[1] = 1;
                        values[2] = 2;
                        break;
                    case 1:
                        values[0] = 1;
                        values[1] = 2;
                        values[2] = 0;
                        break;
                    case 2:
                        values[0] = 0;
                        values[1] = 2;
                        values[2] = 1;
                        break;
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[values[j]];
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

void ac::ReduceBy50(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    AlphaBlend(copy1, copy2, frame, 0.25);
    AddInvert(frame);
}

void ac::AlphaBlendWithSourceScale(cv::Mat &frame) {
    if(orig_frame.empty()) return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = orig_frame.clone();
    AlphaBlendDouble(copy1, copy2, frame, alpha, 1.0-alpha);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha,dir, 0.01, 0.5, 0.1);
}

void ac::ColorPositionAverageXor(cv::Mat &frame) {
    std::vector<cv::Vec3b> pixels;
    int value = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(i == value) {
                pixels.push_back(pixelAt(frame,z, i));
                ++value;
                break;
            }
        }
    }
    cv::Scalar combined;
    int values[3] = {0,0,0};
    for(unsigned int q = 0; q < pixels.size(); ++q) {
        for(int j = 0; j < 3; ++j) {
            combined[j] += pixels[q][j];
        }
    }
    values[0] = static_cast<int>(combined[0]/pixels.size());
    values[1] = static_cast<int>(combined[1]/pixels.size());
    values[2] = static_cast<int>(combined[2]/pixels.size());
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorPositionXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    std::vector<cv::Vec3b> pixels;
    int value = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(i == value) {
                pixels.push_back(pixelAt(frame,z, i));
                ++value;
                break;
            }
        }
    }
    cv::Scalar combined;
    int values[3] = {0,0,0};
    for(unsigned int q = 0; q < pixels.size(); ++q) {
        for(int j = 0; j < 3; ++j) {
            combined[j] += pixels[q][j];
        }
    }
    values[0] = static_cast<int>(combined[0]/pixels.size());
    values[1] = static_cast<int>(combined[1]/pixels.size());
    values[2] = static_cast<int>(combined[2]/pixels.size());
    
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b &pix = collection.frames[7].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
               pixel[j] = cv::saturate_cast<unsigned char>((pixel[j]^pix[j])^values[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorPositionXorMedianBlend(cv::Mat &frame) {
    ColorPositionXor(frame);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
}

void ac::CannyStrobe(cv::Mat &frame) {
    static double x = 250, y = 250;
    static int dir1 = 1, dir2 = 1;
    AlphaMovementMaxMin(x,dir1,1.0, 250.0, 200.0);
    AlphaMovementMaxMin(y,dir2,1.0, 250.0, 200.0);
    cv::UMat copy1 = frame.getUMat(cv::ACCESS_FAST);
    cv::UMat out;
    cv::Canny(copy1, out, x, y);
    cv::Mat converted;
    cv::cvtColor(out, frame, cv::COLOR_GRAY2BGR);
    AddInvert(frame);
    ColorCollectionReverseStrobe(frame);
}

void ac::LaplacianStrobe(cv::Mat &frame) {
    ac::Laplacian(frame);
    ac::ColorCollectionReverseStrobe(frame);
    AddInvert(frame);
}

void ac::LaplacianStrobeOnOff(cv::Mat &frame) {
    static int flash = 0;
    if(flash == 0) {
        flash = 1;
        LaplacianStrobe(frame);
    } else {
        ac::Laplacian(frame);
        flash = 0;
    }
    AddInvert(frame);
}

void ac::ColorCollectionPixelXor(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    int max_blur = 3+(rand()%5);
    for(int m = 0; m < max_blur; ++m)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    value[j] += pix[j];
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                value[j] /= collection.size();
                int ival = static_cast<int>(value[j]);
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^ival);
            }
        }
    }
    AddInvert(frame);
}

void ac::GrayStrobe(cv::Mat &frame) {
    static bool on_off = true;
    if(on_off == true) {
        Grayscale(frame);
        on_off = false;
    } else
        on_off = true;
    
    AddInvert(frame);
}

void ac::ColorStrobeXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int max_blur = 5+(rand()%5);
    for(int m = 0; m < max_blur; ++m)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    cv::Vec3b value(rand()%256, rand()%256, rand()%256);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar sval;
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    sval[j] += pix[j];
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int bgr[3];
            for(int j = 0; j < 3; ++j) {
                bgr[j] = static_cast<int>(sval[j]/collection.size());
                pixel[j] = pixel[j] ^ bgr[j] ^ value[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorGhost(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int index = 0, dir = 1;
    cv::Mat frames[3];
    frames[0] = collection.frames[index].clone();
    frames[1] = collection.frames[collection.size()/2].clone();
    frames[2] = collection.frames[collection.size()-2].clone();
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > collection.size()-1) {
            index = collection.size()-1;
            dir = 0;
        }
    } else if(dir == 0) {
        --index;
        if(index <= 1) {
            index = 1;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::ColorCollectionTwitchSubFilter(cv::Mat &frame) {
    if(subfilter == -1)
        return;
    static MatrixCollection<8> collection;
    int max_blur = 4;
    for(int m = 0; m < max_blur; ++m)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    cv::Mat copy_frame = collection.frames[rand()%(collection.size()-1)].clone();
    cv::Mat temp = frame.clone();
    AlphaBlend(temp, copy_frame, frame, 0.5);
    CallFilter(subfilter, frame);
    AddInvert(frame);
}

void ac::BlurredOutXor(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static int index = 1, dir = 1;
    cv::Mat copyf = frame.clone();
    MedianBlur(copyf, index);
    collection.shiftFrames(copyf);
    cv::Mat frames[3];
    frames[0] = collection.frames[1];
    frames[1] = collection.frames[collection.size()/2];
    frames[2] = collection.frames[collection.size()-1];
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir == 1) {
        index += 2;
        if(index > 31) {
            index = 31;
            dir = 0;
        }
    } else {
        index -= 2;
        if(index <= 3) {
            index = 3;
            dir = 1;
        }
    }
}

void ac::BoxFilter(cv::Mat &frame) {
    static int index = 1, dir = 1;
    cv::UMat src, dst;
    src = frame.getUMat(cv::ACCESS_FAST);
    cv::boxFilter(src, dst, -1, cv::Size(index,index));
    dst.copyTo(frame);
    AddInvert(frame);
    if(dir == 1) {
        index += 2;
        if(index > 13) {
            index = 13;
            dir = 0;
        }
    } else {
        index -= 2;
        if(index <= 3) {
            index = 3;
            dir = 1;
        }
    }
}

void ac::DizzyFilter(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b pix[3];
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pix[0] = collection.frames[1].at<cv::Vec3b>(z, i);
                pix[1] = collection.frames[collection.size()/2].at<cv::Vec3b>(z, i);
                pix[2] = collection.frames[collection.size()-1].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    double value = (pix[0][q]*0.33)+(pix[1][q]*0.33)+(pix[2][q]*0.33);
                    pixel[q] = static_cast<unsigned char>(value);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::Buzzed(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            cv::Vec3b pix[8];
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < collection.size(); ++j) {
                    pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
                }
                const double sep_value = 1.0/collection.size();
                double value[3] = {0,0,0};
                for(int j = 0; j < 3; ++j) {
                     for(int q = 0; q < collection.size(); ++q) {
                        value[j] += (pix[q][j] * sep_value);
                    }
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(value[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BlendWithImage25(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat img;
    ac_resize(blend_image, img, frame.size());
    cv::Mat copy1 = frame.clone();
    AlphaBlendDouble(copy1, img, frame, 0.75, 0.25);
}

void ac::BlendWithImage50(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat img;
    ac_resize(blend_image, img, frame.size());
    cv::Mat copy1 = frame.clone();
    AlphaBlendDouble(copy1, img, frame, 0.5, 0.5);
}

void ac::BlendWithImage75(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat img;
    ac_resize(blend_image, img, frame.size());
    cv::Mat copy1 = frame.clone();
    AlphaBlendDouble(copy1, img, frame, 0.25, 0.75);
}

void ac::BuzzedDark(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            cv::Vec3b pix[16];
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < collection.size(); ++j) {
                    pix[j] = collection.frames[j].at<cv::Vec3b>(z, i);
                }
                const double sep_value = 1.0/(collection.size()-1);
                double value[3] = {0,0,0};
                for(int j = 0; j < 3; ++j) {
                    for(int q = 0; q < collection.size(); ++q) {
                        value[j] += (pix[q][j] * sep_value);
                    }
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(value[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendDark(frame);
    AddInvert(frame);
}

