#include"ac.h"

// initalize to null
ac::ParticleEmiter::ParticleEmiter() : part(0), w(0), h(0) {}

// clean up after done
ac::ParticleEmiter::~ParticleEmiter() {
    if(part != 0) {
        for(int i = 0; i < w; ++i)
            delete [] part[i];
        delete [] part;
        part = 0;
    }
}

void ac::ParticleEmiter::reset() {
    w = 0;
    h = 0;
}

// set frame pixel values
void ac::ParticleEmiter::set(cv::Mat &frame) {
    if(static_cast<unsigned int>(frame.cols) != w || static_cast<unsigned int>(frame.rows) != h) {
        if(part != 0) {
            for(int i = 0; i < w; ++i)
                delete [] part[i];
            delete [] part;
        }
        w = frame.cols;
        h = frame.rows;
        part = new Particle*[w];
        for(int i = 0; i < w; ++i) {
            part[i] = new Particle[h];
            for(int z = 0; z < h; ++z) {
                part[i][z].x = i;
                part[i][z].y = z;
                part[i][z].dir = rand()%4;
            }
        }
    }
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(z, i);
            part[i][z].pixel = pixel;
        }
    }
}
// draw pixel values to frame
void ac::ParticleEmiter::draw(cv::Mat &frame) {
    movePixels();//move values before drawing
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            int x_pos = part[i][z].x;
            int y_pos = part[i][z].y;
            if(x_pos > 0 && x_pos < frame.cols && y_pos > 0 && y_pos < frame.rows) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(y_pos, x_pos);
                pixel = part[i][z].pixel;
            }
        }
    }
}
// move pixel coordinates around
void ac::ParticleEmiter::movePixels() {
    for(int i = 0; i < w; ++i) {
        for(int z = 0; z < h; ++z) {
            Particle &p = part[i][z];
            p.m_count ++;
            if(p.m_count > 250) {
                p.m_count = 0;
                p.dir = rand()%4;
                continue;
            }
            switch(p.dir) {
                case DIR_UP:
                    if(p.y > 0) {
                        p.y--;
                    } else {
                        p.y = 1+rand()%(h-1);
                        p.dir = rand()%4;
                    }
                    break;
                case DIR_DOWN:
                    if(p.y < h-1) {
                        p.y++;
                    } else {
                        p.dir = rand()%4;
                        p.y = 1+rand()%(h-1);
                    }
                    break;
                case DIR_LEFT:
                    if(p.x > 0) {
                        p.x--;
                    } else {
                        p.dir = rand()%4;
                        p.x = 1+rand()%(w-1);
                    }
                    break;
                case DIR_RIGHT:
                    if(p.x < w-1) {
                        p.x++;
                    } else {
                        p.dir = rand()%4;
                        p.x = rand()%(w-1);
                    }
                    break;
                default:
                    p.dir = rand()%4;
            }
        }
    }
}

ac::ParticleEmiter emiter; // initialize

// Particle Filter
void ac::ParticleRelease(cv::Mat &frame) {
    emiter.set(frame);// set values each frame
    emiter.draw(frame); // draw values each frame
}

