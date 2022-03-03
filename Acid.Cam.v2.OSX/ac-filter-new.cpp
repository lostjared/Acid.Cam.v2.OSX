
#include "ac.h"

unsigned char ac::wrap_cast(double d) {
    unsigned long x = static_cast<unsigned long>(d);
    if(x > 255)
        x = x%255;
    return static_cast<unsigned char>(x);
}

void ac::FrameSep(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%240)==0) && ++off > MAX-1)
            off = 0;
    }
}

void ac::FrameSep2(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}

void ac::StutterFilter(cv::Mat  &frame) {
    static cv::Mat stored;
    static cv::Size stored_size;
    
    if(stored_size != frame.size()) {
        srand(static_cast<int>(time(0)));
        stored = frame.clone();
        stored_size = frame.size();
    } else {
        if(stored.empty())
            stored = frame.clone();
        
        static bool on = true;
        if(on == true) {
            if((rand()%15)==0) {
                stored = frame.clone();
                on = !on;
            }
            frame = stored.clone();
        } else {
            if((rand()%10) == 0)
                on = !on;
        }
    }
}

void ac::DefStutter(cv::Mat  &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty())
        srand(static_cast<unsigned int>(time(0)));
    collection.shiftFrames(frame);
    cv::Mat frame_copy = frame.clone();
    StutterFilter(frame_copy);
    int num_pixels = rand()%((frame.rows * frame.cols)/320);
    for(int q = 0; q < num_pixels; ++q) {
        int pixel_size = 16+(rand()%64);
        
        int off_x = rand()%(frame.cols-pixel_size-1);
        int off_y = rand()%(frame.rows-pixel_size-1);
        int offset = rand()%(collection.size()-1);
        
        for(int y = off_y; y < off_y + pixel_size; ++y) {
            for(int x = off_x; x < off_x + pixel_size; ++x) {
                cv::Mat &m = collection.frames[offset];
                cv::Vec3b &pix = m.at<cv::Vec3b>(y, x);
                cv::Vec3b &pix_copy = frame_copy.at<cv::Vec3b>(y, x);
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] != pix[j]) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix_copy[j]));
                    }
                }
            }
        }
    }
}

void ac::FrameOffsetRSize(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
            }
            
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
        }
    }
}

void ac::FrameOffsetRSize2(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Mat &m2 = collection.frames[7];
            
            cv::Vec3b &pixel_diff = m2.at<cv::Vec3b>(z, i);
            
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                if(abs(pixel[j]-pixel_diff[j]) > 15) {
                    pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
                }
            }
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
        }
    }
}


void ac::FrameOffsetRSize3(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    int rgb = rand()%2;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Mat &m2 = collection.frames[4];
            cv::Vec3b &pixel_diff = m2.at<cv::Vec3b>(z, i);
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            bool add = false;
            for(int j = 0; j < 3; ++j) {
                if(abs(pixel[j]-pixel_diff[j]) > 15) {
                    add = true;
                    break;
                }
            }
            if(add == true)
                pixel[rgb] = wrap_cast((0.5 * pixel[rgb]) + (0.5 * pixel_off[rgb]));
            
            offset_x++;
        }
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
            rgb++;
            if(rgb > 2)
                rgb = 0;
        }
    }
}

void ac::FrameOffsetRSize4(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    bool on = true;
    if( rand()%10 > 5 )
        on = false;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            if(on == false) continue;
            
            cv::Mat &m = collection.frames[offset];
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
            }
            
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 150+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 125+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
            on = (on == true) ? false : true;
        }
    }
}

void ac::SqOnOff(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static constexpr int PIXEL_SIZE=32;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    
    static int off = 0;
    
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        pixel = pix_off;
                        
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    ac::AddInvert(frame);
}

void ac::SqOnOffAlpha(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = static_cast<unsigned char>((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 1.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaClip(cv::Mat &frame) {
    
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 4.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaClipFast(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%36);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.07;
            if(alpha >= 8.0)
                adir = 0;
        } else {
            alpha -= 0.07;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaPixel(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%36);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                
                static int adir = 1;
                if(adir == 1) {
                    alpha += 0.07;
                    if(alpha >= 8.0)
                        adir = 0;
                } else {
                    alpha -= 0.07;
                    if(alpha <= 0.1)
                        adir = 1;
                }
                
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffDiff(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            if(abs(pixel[q]-pix_off[q]) > 100) {
                                pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                            }
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 4.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffSize(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static int PIXEL_SIZE=8;
    static int dir = 1;
    
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    
    static int off = 0;
    
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        pixel = pix_off;
                        
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    if(dir == 1) {
        PIXEL_SIZE ++;
        if(PIXEL_SIZE >= 32)
            dir = 0;
    } else {
        PIXEL_SIZE --;
        if(PIXEL_SIZE <= 8)
            dir = 1;
    }
    ac::AddInvert(frame);
}

void ac::Placement(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    
    static int size_x = 24, size_y = 24;
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        static int findex = 0;
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                if((i%size_x) == 0 && (z%size_y) == 0) {
                    findex++;
                    if(findex > collection.size()-1)
                        findex = 0;
                }
                
                cv::Mat &m = collection.frames[findex];
                cv::Vec3b &pix = m.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        size_x += 2;
        size_y += 2;
        
        if(size_x > frame.cols/2 || size_y > frame.rows/2) {
            dir = 0;
        }
    } else {
        size_x -= 2;
        size_y -= 2;
        if(size_x <= 2 || size_y <= 2) {
            dir = 1;
        }
    }
}
