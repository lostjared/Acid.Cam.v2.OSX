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

void ac::AlphaBlendImageDownUp(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat img, copy1 = frame.clone();
    ac_resize(blend_image, img, frame.size());
    BlendWithSource25(copy1);
    double alpha = 1.0;
    int dir = 1;
    AlphaBlend(copy1, img, frame, alpha);
    for(int i = 0; i < 2; ++i) {
        AlphaMovementMaxMin(alpha, dir, 0.005, 2.0, 1.0);
    }
    AddInvert(frame);
}

void ac::BlendWithImageAndSource(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static double alpha1 = 2.0, alpha2 = 1.0;
    static int dir1 = 0, dir2 = 1;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    BlendWithImage25(copy1);
    BlendWithSource25(reimage);
    AlphaBlendDouble(copy1, reimage, frame, alpha1, alpha2);
    AlphaMovementMaxMin(alpha1, dir1, 0.05, 2.0, 1.0);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 2.0, 1.0);
    AddInvert(frame);
}

void ac::PixelSourceFrameBlend256(cv::Mat &frame) {
    static MatrixCollection<256> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                unsigned int rgb[3] = {pixel[0], pixel[1], pixel[2]};
                cv::Vec3b pix[3];
                for(int q = 0; q < 3; ++q) {
                    pix[q] = collection.frames[rgb[q]].at<cv::Vec3b>(z, i);
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pix[j][j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SplitMatrixCollection(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    static int index = 0;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
                ++index;
                if(index > collection.size()-1)
                    index = 0;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RectangleGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0, dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                if(dir == 1) {
                    ++index;
                    if(index > collection.size()-1) {
                        index = collection.size()-1;
                        dir = 0;
                    } else {
                        --index;
                        if(index <= 1) {
                            index = 1;
                            dir = 1;
                        }
                    }
                }
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    
}

void ac::PositionShift(cv::Mat &frame) {
    if(frame.empty())
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = collection.frames[7].clone();
    int val_offset = rand()%(frame.cols-1);
    for(int z = 0; z < frame.rows; ++z) {
        int index = val_offset;
        for(int i = 0; i < frame.cols; ++i) {
            if(index < frame.cols-1) {
                cv::Vec3b &pixel = pixelAt(frame,z, index);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
                ++index;
            } else
                break;
        }
    }
    AddInvert(frame);
}

void ac::ColorCollectionMovementIndex(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[4];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    static int index = 0, dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                if(z >= 0 && z < frame->rows && i >= 0 && i < frame->cols) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    int values[6];
                    InitArrayPosition(values, index);
                    for(int j = 0; j < 3; ++j) {
                        cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                        if(values[j] >= 0 && values[j] < 3)
                            pixel[j] = pix[values[j]];
                    }
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
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::Shake(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[collection.size()/2].clone();
    frames[2] = collection.frames[collection.size()-1].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        int counter = rand()%3;
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                int values[3];
                InitArrayPosition(values, index);
                cv::Vec3b cpix = pixel;
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[values[j]].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
                pixel[values[counter]] = cpix[values[counter]];
                ++counter;
                if(counter > 2)
                    counter = 0;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ++index;
    if(index > 2) {
        index = 0;
    }
}

void ac::Disoriented(cv::Mat &frame) {
     static MatrixCollection<11> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1];
    frames[1] = collection.frames[5];
    frames[2] = collection.frames[10];
    static int val_offset = 0;
    double alpha = 0.33;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                unsigned int values[3] = {0,0,0};
                for(int q = 1; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        values[j] += pix[j];
                    }
                }
                int pix_values[3] = {0,0,0};
                InitArrayPosition(pix_values, val_offset);
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    values[j] /= collection.size();
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) + static_cast<unsigned char>(values[j]*alpha) + static_cast<unsigned char>(pix[pix_values[j]]*alpha);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlur(frame, 5);
    AddInvert(frame);
    ++val_offset;
    if(val_offset > 2)
        val_offset = 0;
}

void ac::ColorCollectionPositionStrobe(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = collection.frames[1].at<cv::Vec3b>(z, i);
                pix[1] = collection.frames[7].at<cv::Vec3b>(z, i);
                pix[2] = collection.frames[15].at<cv::Vec3b>(z, i);
                int value[3];
                InitArrayPosition(value, index);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pix[j][value[j]];
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

void ac::ColorCollectionStrobeBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    double alpha = 0.33;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = collection.frames[1].at<cv::Vec3b>(z, i);
                pix[1] = collection.frames[7].at<cv::Vec3b>(z, i);
                pix[2] = collection.frames[15].at<cv::Vec3b>(z, i);
                int value[3];
                InitArrayPosition(value, index);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pix[0][value[j]] * alpha) + (pix[1][value[j]] * alpha) + (pix[2][value[j]] * alpha));
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

void ac::AlphaBlendStoredFrames(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    double alpha = 0.33;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = collection.frames[1].at<cv::Vec3b>(z, i);
                pix[1] = collection.frames[7].at<cv::Vec3b>(z, i);
                pix[2] = collection.frames[15].at<cv::Vec3b>(z, i);
                int value[3];
                InitArrayPosition(value, index);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pix[0][j] * alpha) + (pix[1][j] * alpha) + (pix[2][j] * alpha));
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

void ac::SplitMatrixSortChannel(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int value[3];
    InitArrayPosition(value, offset);
    std::vector<cv::Mat> v1, v2, v3;
    cv::split(collection.frames[1], v1);
    cv::split(collection.frames[4], v2);
    cv::split(collection.frames[7], v3);
    cv::Mat channels[3];
    cv::sort(v1[0], channels[value[0]],cv::SORT_ASCENDING);
    cv::sort(v2[1], channels[value[1]],cv::SORT_ASCENDING);
    cv::sort(v3[2], channels[value[2]],cv::SORT_ASCENDING);
    cv::merge(channels, 3, frame);
    ++offset;
    if(offset > 2)
        offset = 0;
    AddInvert(frame);
    MedianBlendMultiThread(frame);
}

void ac::SplitMatrixSortChannelArrayPosition(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int value[3];
    InitArrayPosition(value, offset);
    std::vector<cv::Mat> v1, v2, v3;
    cv::split(collection.frames[1], v1);
    cv::split(collection.frames[4], v2);
    cv::split(collection.frames[7], v3);
    cv::Mat channels[3];
    cv::sort(v1[value[0]], channels[0],cv::SORT_ASCENDING);
    cv::sort(v2[value[1]], channels[1],cv::SORT_ASCENDING);
    cv::sort(v3[value[2]], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, frame);
    ++offset;
    if(offset > 2)
        offset = 0;
    AddInvert(frame);
    BlendWithSource25(frame);
    MedianBlendMultiThread(frame);
}

void ac::SplitMatrixSortChannelImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int value[3];
    InitArrayPosition(value, offset);
    std::vector<cv::Mat> v1, v2, v3;
    cv::split(collection.frames[1], v1);
    cv::split(collection.frames[4], v2);
    cv::split(collection.frames[7], v3);
    cv::Mat channels[3];
    cv::sort(v1[value[0]], channels[0],cv::SORT_ASCENDING);
    cv::sort(v2[value[1]], channels[1],cv::SORT_ASCENDING);
    cv::sort(v3[value[2]], channels[2],cv::SORT_ASCENDING);
    cv::merge(channels, 3, frame);
    ++offset;
    if(offset > 2)
        offset = 0;
    AddInvert(frame);
    BlendWithImage75(frame);
    MedianBlendMultiThread(frame);
}

void ac::ShiftColorLeft(cv::Mat &frame) {
    //static MatrixCollection<8> collection;
    //collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    //cv::Mat copy1 = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols-3; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = copy1.at<cv::Vec3b>(z, (i+1));
                pix[1] = copy1.at<cv::Vec3b>(z, (i+2));
                pix[2] = copy1.at<cv::Vec3b>(z, (i+3));
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j][j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::CycleInAndOutRepeat(cv::Mat &frame) {
    static int index = rand()%128;;
    static MatrixCollection<128> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    cv::Mat cur_frame = collection.frames[index].clone();
    if(index > collection.size()-2) {
        index = 0;
    }
    AlphaBlend(copy1, cur_frame, frame, 0.5);
    AddInvert(frame);
}

void ac::ColorCollectionShuffle(cv::Mat &frame) {
    static int init = 0;
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> index_values;
    if(init == 0) {
        init = 1;
        for(auto &i : color_filter) {
            index_values.push_back(i);
        }
        std::shuffle(index_values.begin(), index_values.end(), rng);
    }
    static unsigned int index = 0;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(index_values[index], copy1);
    Smooth(copy1, &collection);
    AlphaBlend(copy1, copy2, frame, 0.5);
    ++index;
    if(index > index_values.size()-1) {
        index = 0;
        std::shuffle(index_values.begin(), index_values.end(), rng);
    }
}

void ac::BlendFromXtoY(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int x = 0, y = 15, dir1 = 1, dir2 = 0;
    cv::Mat outframe;
    AlphaBlend(collection.frames[x], collection.frames[y], outframe, 0.5);
    AlphaBlend(collection.frames[collection.size()/2], outframe, frame, 0.5);
    if(dir1 == 1) {
        x += 2;
        if(x > collection.size()-1) {
            x = 15;
            dir1 = 0;
        }
    } else if(dir1 == 0) {
        x -= 2;
        if(x <= 1) {
            x = 0;
            dir1 = 1;
        }
    }
    if(dir2 == 1) {
        y += 2;
        if(y > collection.size()-1) {
            y = 15;
            dir2 = 0;
        }
    } else if(dir2 == 0) {
        y -= 2;
        if(y <= 1) {
            y = 0;
            dir2 = 1;
        }
    }
    AddInvert(frame);
}

void ac::BlendImageXtoY(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    static int x = 0, y = 0, dir1 = 1, dir2 = 0;
    static double alpha = 1.0/4;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[2];
                pix[0] = collection.frames[x].at<cv::Vec3b>(z, i);
                pix[1] = collection.frames[y].at<cv::Vec3b>(z, i);
                cv::Vec3b image_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pix[0][j] * alpha) + (pix[1][j] * alpha) + (image_pix[j] * alpha) + (pixel[j] * alpha));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    if(dir1 == 1) {
        x += 2;
        if(x > collection.size()-1) {
            x = 15;
            dir1 = 0;
        }
    } else if(dir1 == 0) {
        x -= 2;
        if(x <= 1) {
            x = 0;
            dir1 = 1;
        }
    }
    if(dir2 == 1) {
        y += 2;
        if(y > collection.size()-1) {
            y = 15;
            dir2 = 0;
        }
    } else if(dir2 == 0) {
        y -= 2;
        if(y <= 1) {
            y = 0;
            dir2 = 1;
        }
    }
}

void ac::BlendColorImageStrobeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "BlendColorImageStrobeSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static int index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b  &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = pixel[0];
                pix[0] = reimage.at<cv::Vec3b>(z, i);
                pix[1] = copy1.at<cv::Vec3b>(z, i);
                int arr[3];
                InitArrayPosition(arr, index);
                for(int j = 0; j < 3; ++j) {
                    pixel[j]= pix[arr[j]][j];
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

void ac::BlendByColsSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "BlendByColsSubFilter")
        return;
    int index = 0;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 1) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        index = (index == 0) ? 1 : 0;
    }
    AddInvert(frame);
}

void ac::BlendByColsImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    int index = 0;
    cv::Mat copy1;
    ac_resize(blend_image, copy1, frame.size());
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 1) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        index = (index == 0) ? 1 : 0;
    }
    AddInvert(frame);
}

void ac::IntertwineX(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    IntertwineFrames(IntertwineDir::HORIZONTAL, copy1, copy1, frame);
    AddInvert(frame);
}

void ac::IntertwineY(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    IntertwineFrames(IntertwineDir::VERTICAL, copy1, copy1, frame);
    AddInvert(frame);
}

void ac::IntertwineSubFilterX(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineSubFilterX")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    IntertwineFrames(IntertwineDir::HORIZONTAL, copy1, copy2, frame);
    AddInvert(frame);
}

void ac::IntertwineSubFilterY(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IntertwineSubFilterX")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    IntertwineFrames(IntertwineDir::VERTICAL, copy1, copy2, frame);
    AddInvert(frame);
}

void ac::IntertwineImageX(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    IntertwineFrames(IntertwineDir::HORIZONTAL, copy1, reimage, frame);
    AddInvert(frame);
}
void ac::IntertwineImageY(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    IntertwineFrames(IntertwineDir::VERTICAL, copy1, reimage, frame);
    AddInvert(frame);
}

void ac::InertwineImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "InertwineImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    IntertwineFrames(IntertwineDir::HORIZONTAL, copy1, reimage, frame);
    AddInvert(frame);
}

void ac::IntertwineHorizontalImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineHorizontalImageSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    cv::Mat frames[3];
    frames[0] = frame.clone();
    frames[1] = reimage.clone();
    frames[2] = copy1.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                pixel[j] = pix[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::InterwtineHorizontalImageSubFilterByIndex(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "InterwtineHorizontalImageSubFilterByIndex")
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    cv::Mat frames[3];
    frames[0] = frame.clone();
    frames[1] = reimage.clone();
    frames[2] = copy1.clone();
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pix[j];
            }
            ++index;
            if(index > 2)
                index = 0;
        }
    }
}

void ac::IntertwineHorizontalImageSubFilterMatrixCollection(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "IntertwineHorizontalImageSubFilterMatrixCollection")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat frames[10];
    for(int i = 0; i < collection.size(); ++i){
        frames[i] = collection.frames[i].clone();
    }
    CallFilter(subfilter, copy1);
    frames[8] = copy1.clone();
    frames[9] = reimage.clone();
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = frames[index].at<cv::Vec3b>(z, i)[j];
                ++index;
                if(index > 9)
                    index = 0;
            }
        }
    }
}

void ac::BlendForwardAndBack16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[8].clone();
    frames[2] = collection.frames[15].clone();
    double alpha = 1.0/3;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b colors[3];
                colors[0] = frames[0].at<cv::Vec3b>(z, i);
                colors[1] = frames[1].at<cv::Vec3b>(z, i);
                colors[2] = frames[2].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha*colors[0][j]) + (alpha*colors[1][j]) + (alpha*colors[2][j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BlendForwardAndBack32(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[4];
    frames[0] = collection.frames[8].clone();
    frames[1] = collection.frames[16].clone();
    frames[2] = collection.frames[24].clone();
    frames[3] = collection.frames[31].clone();
    double alpha = 1.0/4;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b colors[4];
                colors[0] = frames[0].at<cv::Vec3b>(z, i);
                colors[1] = frames[1].at<cv::Vec3b>(z, i);
                colors[2] = frames[2].at<cv::Vec3b>(z, i);
                colors[3] = frames[3].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha*colors[0][j]) + (alpha*colors[1][j]) + (alpha*colors[2][j]) + (alpha*colors[3][j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BlendForwardAndBack64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[6];
    frames[0] = collection.frames[8].clone();
    frames[1] = collection.frames[16].clone();
    frames[2] = collection.frames[24].clone();
    frames[3] = collection.frames[31].clone();
    frames[4] = collection.frames[47].clone();;
    frames[5] = collection.frames[63].clone();
    double alpha = 1.0/6;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b colors[6];
                colors[0] = frames[0].at<cv::Vec3b>(z, i);
                colors[1] = frames[1].at<cv::Vec3b>(z, i);
                colors[2] = frames[2].at<cv::Vec3b>(z, i);
                colors[3] = frames[3].at<cv::Vec3b>(z, i);
                colors[4] = frames[4].at<cv::Vec3b>(z, i);
                colors[5] = frames[5].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha*colors[0][j]) + (alpha*colors[1][j]) + (alpha*colors[2][j]) + (alpha*colors[3][j]) + (alpha *colors[4][j]) + (alpha*colors[5][j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BlendForwardAndBack8_RandomStrobe(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[rand()%7].clone();
    frames[1] = collection.frames[rand()%7].clone();
    frames[2] = collection.frames[rand()%7].clone();
    double alpha = 1.0/3;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b colors[3];
                colors[0] = frames[0].at<cv::Vec3b>(z, i);
                colors[1] = frames[1].at<cv::Vec3b>(z, i);
                colors[2] = frames[2].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha*colors[0][j]) + (alpha*colors[1][j]) + (alpha*colors[2][j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ColorCollectionReverseStrobe(frame);
}

void ac::AlphaBlendWithThreeCollections(cv::Mat &frame) {
    static MatrixCollection<8>  collection1;
    static MatrixCollection<16> collection2;
    static MatrixCollection<32> collection3;
    collection1.shiftFrames(frame);
    collection2.shiftFrames(frame);
    collection3.shiftFrames(frame);
    static int index = 0;
    cv::Mat frames[3];
    switch(index) {
        case 0:
            frames[0] = collection1.frames[1].clone();
            frames[1] = collection1.frames[4].clone();
            frames[2] = collection1.frames[7].clone();
            break;
        case 1:
            frames[0] = collection2.frames[1].clone();
            frames[1] = collection2.frames[7].clone();
            frames[2] = collection2.frames[15].clone();
            break;
        case 2:
            frames[0] = collection3.frames[1].clone();
            frames[1] = collection3.frames[15].clone();
            frames[2] = collection3.frames[31].clone();
            break;
    }
    ++index;
    if(index > 2)
        index = 0;
    AlphaBlendArray(frame, frames, 3);
    AddInvert(frame);
}

void ac::AlphaBlendArrayRandom(cv::Mat &frame) {
    cv::Mat frames[6];
    for(int i = 0; i < 4; ++i) {
        frames[i] = frame.clone();
        Random_Filter(frames[i]);
        BlendWithSource25(frames[i]);
    }
    AlphaBlendArray(frame, frames, 3);
    AddInvert(frame);
}

void ac::AlphaBlendArrayTrailsFilters(cv::Mat &frame) {
    cv::Mat frames[4];
    for(int i = 0; i < 4; ++i)
        frames[i] = frame.clone();
    ColorCollectionReverseStrobe(frames[0]);
    GhostTrails(frames[1]);
    BlendForwardAndBack16(frames[2]);
    RGBTrailsDark(frames[3]);
    AlphaBlendArray(frame, frames, 4);
    AddInvert(frame);
}

void ac::AlphaBlendMirrorChannelSort(cv::Mat &frame) {
    cv::Mat frames[6];
    for(int i = 0; i < 6; ++i)
        frames[i] = frame.clone();
    MirrorLeft(frames[0]);
    MirrorRight(frames[1]);
    GhostTrails(frames[2]);
    ChannelSort(frames[3]);
    SplitMatrixSortChannel(frames[4]);
    SelfScale(frames[5]);
    AlphaBlendArray(frame, frames, 6);
    AddInvert(frame);
}

void ac::ColorCollectionAlphaBlendArray(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[7].clone();
    frames[2] = collection.frames[15].clone();
    static double alpha = 0.5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) + static_cast<unsigned char>(pix[j]*alpha);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::AlphaBlendArrayExpand(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static int num = 1, dir = 1;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat *frames;
    frames = new cv::Mat[num];
    
    for(int i = 0; i < num; ++i) {
        frames[i] = collection.frames[i].clone();
    }
    AlphaBlendArray(frame, frames, num);
    if(dir == 1) {
        ++num;
        if(num >= MAX-1) {
            dir = 0;
            num = MAX-1;
        }
    } else {
        --num;
        if(num <= 1) {
            dir = 1;
            num = 1;
        }
    }
    delete [] frames;
    AddInvert(frame);
}
