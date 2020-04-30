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

void ac::StrobePixelTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    pushSubFilter(getFilterByName("RandomStrobeFlash"));
    TrailsSubFilter(copy1);
    popSubFilter();
    collection.shiftFrames(copy1);
    int index = 0, dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.6));
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
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::RectangleTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int index = 0, dir = 1;
    int y = 0;
    int height = 25+(rand()%75);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.6));
            }
        }
        ++y;
        if(y > height) {
            y = 0;
            height = 25+(rand()%75);
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
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::RectangleXY_Trails(cv::Mat &frame) {
    static MatrixCollection<16> collection, collection2;
    collection.shiftFrames(frame);
    
    if(collection2.empty())
        collection2.shiftFrames(frame);
    
    int index = 0, dir = 1, index2 = 0, dir2 = 1;
    int x = 0, y = 0;
    int height = 25+(rand()%75);
    int width =  25+(rand()%75);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = collection2.frames[index2].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.4 * pixel[j]) + (pix[j] * 0.3)+(pix2[j] * 0.3));
            }
            ++x;
            if(x > width) {
                x = 0;
                width = 25+(rand()%75);
                if(dir2 == 1) {
                    ++index2;
                    if(index2 > (collection2.size()-1)) {
                        index2 = collection2.size()-1;
                        dir2 = 0;
                    }
                } else {
                    --index2;
                    if(index2 <= 0) {
                        index2 = 0;
                        dir2 = 1;
                    }
                }
            }
        }
        ++y;
        if(y > height) {
            y = 0;
            height = 25+(rand()%75);
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
    static int counter = 0;
    ++counter;
    int fps_ = static_cast<int>(ac::fps);
    
    if ((counter%fps_) == 0)
        collection2.shiftFrames(frame);
    
    collection2.shiftFrames(frame);
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::MedianBlendImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static constexpr int SIZE=8;
    static MatrixCollection<SIZE> collection;
    collection.shiftFrames(frame);
    int r = 3+rand()%4;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Scalar total;
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = collection.frames[q].at<cv::Vec3b>(z, i);
                cv::Vec3b ipix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    total[j] += static_cast<unsigned char>((pixel[j] * 0.5) + (ipix[j] * 0.5));
                }
            }
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int t = 1+static_cast<int>(total[j]);
                    pixel[j] = static_cast<unsigned char>((pixel[j]^t)^img[j]);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::GhostWaveTrails(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    GhostTrails(copy1);
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
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}


struct FadeType {
    int x, y, add;
    FadeType() = default;
    FadeType(int xx, int yy, int aa) : x{xx}, y{yy}, add{aa} {}
};

void ac::VideoPixelFade(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        static PixelArray2D pix_container;
        static int pix_x = 0, pix_y = 0;
        static std::vector<FadeType> pos;
        if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
            pix_container.create(frame, frame.cols, frame.rows, -1);
            pix_x = frame.cols;
            pix_y = frame.rows;
            if(!pos.empty()) {
                pos.erase(pos.begin(), pos.end());
            }
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    pos.push_back(FadeType(i, z, 1));
                    for(int j = 0; j < 3; ++j) {
                        pix_container.pix_values[i][z].add[j] = 0;
                    }
                }
            }
            std::shuffle(pos.begin(), pos.end(), rng);
        }
        int total = (frame.cols * frame.rows)/240;
        static int dir = 1;
        for(int i = 0; i < total; ++i) {
            if(pos.size() > 0) {
                FadeType &p = pos[pos.size()-1];
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[p.x][p.y].add[j] = p.add;
                }
                pos.pop_back();
            } else {
                if(!pos.empty()) {
                    pos.erase(pos.begin(), pos.end());
                }
                for(int z = 0; z < frame.rows; ++z) {
                    for(int i = 0; i < frame.cols; ++i) {
                        pos.push_back(FadeType(i, z, (dir == 0) ? 1 : 0));
                        for(int j = 0; j < 3; ++j) {
                            pix_container.pix_values[i][z].add[j] = dir;
                            pix_container.pix_values[i][z].col[j] = rand()%2;
                        }

                    }
                }
                std::shuffle(pos.begin(), pos.end(), rng);
                dir = (dir == 0) ? 1 : 0;
            }
        }
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    PixelValues &p = pix_container.pix_values[i][z];
                    cv::Vec3b pix = pixelAt(reframe,z, i);
                    for(int j = 0; j < 3; ++j) {
                        if(p.add[j] == 1 && p.col[j] == 0) {
                            pixel[j] = static_cast<unsigned char>(pixel[j]^pix[j]);
                        }
                        p.col[j] = rand()%2;
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
}

void ac::ImagePixelFade(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reframe;
    ac_resize(blend_image, reframe, frame.size());
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static std::vector<FadeType> pos;
    if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
        if(!pos.empty()) {
            pos.erase(pos.begin(), pos.end());
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                pos.push_back(FadeType(i, z, 1));
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[i][z].add[j] = 0;
                }
            }
        }
        std::shuffle(pos.begin(), pos.end(), rng);
    }
    int total = (frame.cols * frame.rows)/240;
    static int dir = 1;
    for(int i = 0; i < total; ++i) {
        if(pos.size() > 0) {
            FadeType &p = pos[pos.size()-1];
            for(int j = 0; j < 3; ++j) {
                pix_container.pix_values[p.x][p.y].add[j] = p.add;
            }
            pos.pop_back();
        } else {
            if(!pos.empty()) {
                pos.erase(pos.begin(), pos.end());
            }
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    pos.push_back(FadeType(i, z, (dir == 0) ? 1 : 0));
                    for(int j = 0; j < 3; ++j) {
                        pix_container.pix_values[i][z].add[j] = dir;
                        pix_container.pix_values[i][z].col[j] = rand()%2;
                    }
                    
                }
            }
            std::shuffle(pos.begin(), pos.end(), rng);
            dir = (dir == 0) ? 1 : 0;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.add[j] == 1 && p.col[j] == 0) {
                        pixel[j] = static_cast<unsigned char>(pixel[j]^pix[j]);
                    }
                    p.col[j] = rand()%2;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::CollectionPixelFade(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    GradientColorBlendAll(copy1);
    MedianBlendMultiThread(copy1);
    collection.shiftFrames(copy1);
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int index = 0, dir1 = 1;
    static std::vector<FadeType> pos;
    if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
        if(!pos.empty()) {
            pos.erase(pos.begin(), pos.end());
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                pos.push_back(FadeType(i, z, 1));
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[i][z].add[j] = 0;
                }
            }
        }
        std::shuffle(pos.begin(), pos.end(), rng);
    }
    int total = (frame.cols * frame.rows)/90;
    static int dir = 1;
    for(int i = 0; i < total; ++i) {
        if(pos.size() > 0) {
            FadeType &p = pos[pos.size()-1];
            for(int j = 0; j < 3; ++j) {
                pix_container.pix_values[p.x][p.y].add[j] = p.add;
            }
            pos.pop_back();
        } else {
            if(!pos.empty()) {
                pos.erase(pos.begin(), pos.end());
            }
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    pos.push_back(FadeType(i, z, (dir == 0) ? 1 : 0));
                    for(int j = 0; j < 3; ++j) {
                        pix_container.pix_values[i][z].add[j] = dir;
                        pix_container.pix_values[i][z].col[j] = rand()%2;
                    }
                    
                }
            }
            std::shuffle(pos.begin(), pos.end(), rng);
            dir = (dir == 0) ? 1 : 0;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        cv::Mat &reframe = collection.frames[index];
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.add[j] == 1 && p.col[j] == 0) {
                        pixel[j] = pixel[j]^pix[j];
                    }
                    p.col[j] = rand()%2;
                }
            }
            if(dir1 == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
                    dir1 = 0;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = 0;
                    dir1 = 1;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::CollectionPixelFadeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CollectionPixelFadeSubFilter")
        return;
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int index = 0, dir1 = 1;
    static std::vector<FadeType> pos;
    if(reset_alpha == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
        if(!pos.empty()) {
            pos.erase(pos.begin(), pos.end());
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                pos.push_back(FadeType(i, z, 1));
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[i][z].add[j] = 0;
                }
            }
        }
        std::shuffle(pos.begin(), pos.end(), rng);
    }
    int total = (frame.cols * frame.rows)/90;
    static int dir = 1;
    for(int i = 0; i < total; ++i) {
        if(pos.size() > 0) {
            FadeType &p = pos[pos.size()-1];
            for(int j = 0; j < 3; ++j) {
                pix_container.pix_values[p.x][p.y].add[j] = p.add;
            }
            pos.pop_back();
        } else {
            if(!pos.empty()) {
                pos.erase(pos.begin(), pos.end());
            }
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    pos.push_back(FadeType(i, z, (dir == 0) ? 1 : 0));
                    for(int j = 0; j < 3; ++j) {
                        pix_container.pix_values[i][z].add[j] = dir;
                        pix_container.pix_values[i][z].col[j] = rand()%2;
                    }
                    
                }
            }
            std::shuffle(pos.begin(), pos.end(), rng);
            dir = (dir == 0) ? 1 : 0;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        cv::Mat &reframe = collection.frames[index];
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b pix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.add[j] == 1 && p.col[j] == 0) {
                        pixel[j] = pixel[j]^pix[j];
                    }
                    p.col[j] = rand()%2;
                }
            }
            if(dir1 == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
                    dir1 = 0;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = 0;
                    dir1 = 1;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::DiamondWave(cv::Mat &frame) {
    for(int j = 0; j <3; ++j)
        MedianBlur(frame);
    cv::Mat copy1 = frame.clone();
    DiamondPattern(copy1);
    MirrorLeftBottomToTop(copy1);
    static MatrixCollection<32> collection;
    collection.shiftFrames(copy1);
    static int index = 0, dir1 = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if(dir1 == 1) {
            ++index;
            if(index > (collection.size()-1)) {
                index = collection.size()-1;
                dir1 = 0;
            }
        } else {
            --index;
            if(index <= 0) {
                index = 0;
                dir1 = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::RGBWave(cv::Mat &frame) {
    static constexpr int SIZE=24;
    static MatrixCollection<SIZE> collection;
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    cv::Mat copy1 = frame.clone();
    MatrixCollectionAuraTrails(copy1);
    collection.shiftFrames(copy1);
    int row = 0;
    int size = frame.rows/SIZE;
    std::vector<int> pos;
    for(int j = 0; j < SIZE; ++j)
        pos.push_back(j);
    std::shuffle(pos.begin(), pos.end(), rng);
    static int offset = 0, dir1 = 1;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = row; z < (row+size) && (z < frame.rows); ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b pix = collection.frames[pos[offset]].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) + (0.8 * pix[j]));
                    }
                }
            }
            if(dir1 == 1) {
                ++offset;
                if(offset > (collection.size()-1)) {
                    offset = collection.size()-1;
                    dir1 = 0;
                }
            } else {
                --offset;
                if(offset <= 0) {
                    offset = 0;
                    dir1 = 1;
                }
            }
        }
        row += size;
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::VideoCollectionOutline(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat copy1;
        ac_resize(vframe, copy1, frame.size());
        static MatrixCollection<8> collection;
        ColorPulseIncrease(frame);
        collection.shiftFrames(frame);
        MedianBlendIncrease(copy1);
        static constexpr int val = 4;
        cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
        cv::Mat &copy_frame = collection.frames[val];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy_frame,z, i);
                if(colorBounds(pixel, pix, intensity, intensity)) {
                    pixel = cv::Vec3b(0,0,0);
                } else {
                    pixel = copy1.at<cv::Vec3b>(z, i);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoSaturateAdd(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = pixelAt(reframe,z, i);
                    pixel += pix;
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
}

void ac::VideoSmoothMedianBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static MatrixCollection<8> collection;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection.shiftFrames(frame);
        collection.shiftFrames(reframe);
        Smooth(frame, &collection);
        MedianBlendMultiThread(frame);
    }
    AddInvert(frame);
}

void ac::Square_Blocks(cv::Mat &frame) {
    static MatrixCollection<24> collection;
    collection.shiftFrames(frame);
    int square_size = 16;
    int index = 0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
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
                if(index >= 0) {
                    index = 0;
                    dir = 1;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::Square_Block_Resize(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
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
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
    
}

void ac::Square_Block_Resize_Vertical(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                }
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
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::Square_Block_Resize_Reset(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                }
            }
            ++index;
            if(index > (collection.size()-1)) {
                index = 0;
            }
        }
    }
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::Square_Block_Resize_Vert_Reset(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int square_size = 4, square_dir = 1;
    static int index = 0;
    for(int z = 0; z < frame.rows; z += square_size) {
        for(int i = 0; i < frame.cols; i += square_size) {
            for(int y = 0; y < square_size; ++y) {
                for(int x = 0; x < square_size; ++x) {
                    if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                        cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                        cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                        }
                    }
                }
            }
        }
        ++index;
        if(index > (collection.size()-1)) {
            index = 0;
        }
    }
    if(square_dir == 1) {
        square_size += 2;
        if(square_size >= 64) {
            square_size = 64;
            square_dir = 0;
        }
    } else {
        square_size -= 2;
        if(square_size <= 2) {
            square_size = 2;
            square_dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::Square_Block_Resize_Video(cv::Mat &frame) {
    
    if(v_cap.isOpened() == false)
        return;
    
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<32> collection;
        collection.shiftFrames(frame);
        collection.shiftFrames(reframe);
        static int square_size = 4, square_dir = 1;
        static int index = 0;
        static int dir = 1;
        for(int z = 0; z < frame.rows; z += square_size) {
            for(int i = 0; i < frame.cols; i += square_size) {
                for(int y = 0; y < square_size; ++y) {
                    for(int x = 0; x < square_size; ++x) {
                        if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                            cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
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
        if(square_dir == 1) {
            square_size += 2;
            if(square_size >= 64) {
                square_size = 64;
                square_dir = 0;
            }
        } else {
            square_size -= 2;
            if(square_size <= 2) {
                square_size = 2;
                square_dir = 1;
            }
        }
    }
    AddInvert(frame);
}

