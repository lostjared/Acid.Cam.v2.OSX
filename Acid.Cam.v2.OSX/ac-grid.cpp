
#include "ac.h"

ac::GridBox::GridBox() : color(rand()%255, rand()%255, rand()%255), on(true) {}
ac::GridBox::GridBox(const Rect &r, const cv::Vec3b &col) : location(r), color(col) {}
ac::GridBox::GridBox(const cv::Vec3b &col) : color(col), on(true) {}
ac::GridBox::GridBox(const ac::GridBox &gb) : location(gb.location),color(gb.color),on(gb.on) {}
ac::GridBox &ac::GridBox::operator=(const ac::GridBox &gb) {
    color = gb.color;
    location = gb.location;
    on = gb.on;
    return *this;
}
ac::Grid::Grid() :boxes(0), g_random(false) {}
ac::Grid::~Grid() {
    if(boxes != 0) {
        cleanBoxes();
    }
}

bool operator<(const ac::Point &p1, const ac::Point &p2) {
    if(p1.x < p2.x && p1.y < p2.y)
        return true;
    return false;
}

void ac::Grid::cleanBoxes() {
    if(boxes != 0) {
        for(int j = 0; j < g_w; ++j)
            delete [] boxes[j];
        
        delete [] boxes;
        boxes = 0;
    }
}

void ac::Grid::createGrid(cv::Mat &frame, int w, int h, int size) {
    cleanBoxes();
    g_w = w;
    g_h = h;
    g_s = size;
    boxes = new GridBox*[g_w];
    for(int i = 0; i < g_w; ++i) {
        boxes[i] = new GridBox[h];
    }
    if(!points.empty()) {
        points.erase(points.begin(), points.end());
    }
    for(int i = 0; i < g_w; ++i) {
        for(int z = 0; z < g_h; ++z) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(z*size, i*size);
            Point p(i, z);
            points.push_back(p);
            boxes[i][z] = GridBox(pixel);
            if(g_random)
                boxes[i][z].on = ((rand()%4) == 0) ? false : true;
            
        }
    }
    std::shuffle(points.begin(), points.end(), rng);
    current_offset = 0;
}


void ac::Grid::fillGrid(cv::Mat &frame) {
    if(current_offset < points.size())
        return;
    
    for(int i = 0; i < g_w; ++i) {
        for(int z = 0; z < g_h; ++z) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(z*g_s, i*g_s);
            boxes[i][z] = GridBox(pixel);
            if(g_random)
                boxes[i][z].on = ((rand()%4) == 0) ? false : true;
            
        }
    }
    current_offset = 0;
    std::shuffle(points.begin(), points.end(), rng);
}

void ac::Grid::updateGrid(int max) {
    int iter_max = current_offset+max;
    while(current_offset < points.size() && current_offset < iter_max) {
        const Point &p = points[current_offset];
        boxes[p.x][p.y].on = false;
        current_offset++;
    }
}

void ac::GridFilter8x(cv::Mat &frame) {
    static cv::Size s(0, 0);
    static const int box_size = 8;
    static Grid grid;
    if(frame.size() != s) {
        grid.createGrid(frame, frame.cols/box_size, frame.rows/box_size, box_size);
        s = frame.size();
    }
    int num = 0;
    if(frame.rows >= 1080)
        num = 100;
    else if(frame.rows >= 720)
        num = 75;
    else if(frame.rows >= 400)
        num = 50;
    else
        num = 25;
    grid.updateGrid(150+rand()%num);
    grid.fillGrid(frame);
    for(int z = 0; z < grid.g_h; ++z) {
        for(int i = 0; i < grid.g_w; ++i) {
            if(grid.boxes[i][z].on)
                fillRect(frame, ac::Rect(i*box_size, z*box_size, grid.g_s, grid.g_s), grid.boxes[i][z].color);
        }
    }
}

void ac::GridFilter16x(cv::Mat &frame) {
    static cv::Size s(0, 0);
    static const int box_size = 16;
    static Grid grid;
    if(frame.size() != s) {
        grid.createGrid(frame, frame.cols/box_size, frame.rows/box_size, box_size);
        s = frame.size();
    }
    int num = 0;
    if(frame.rows >= 1080)
        num = 50;
    else if(frame.rows >= 720)
        num = 40;
    else if(frame.rows >= 400)
        num = 30;
    else
        num = 20;
    grid.updateGrid(75+rand()%num);
    grid.fillGrid(frame);
    for(int z = 0; z < grid.g_h; ++z) {
        for(int i = 0; i < grid.g_w; ++i) {
            if(grid.boxes[i][z].on)
                fillRect(frame, ac::Rect(i*box_size, z*box_size, grid.g_s, grid.g_s), grid.boxes[i][z].color);
        }
    }
}

void ac::GridFilter8xBlend(cv::Mat &frame) {
    static cv::Size s(0, 0);
    static const int box_size = 8;
    static double alpha = 1.0, alpha_max = 8.0;
    static Grid grid;
    if(frame.size() != s) {
        grid.createGrid(frame, frame.cols/box_size, frame.rows/box_size, box_size);
        s = frame.size();
    }
    int num = 0;
    if(frame.rows >= 1080)
        num = 100;
    else if(frame.rows >= 720)
        num = 75;
    else if(frame.rows >= 400)
        num = 50;
    else
        num = 25;
    grid.updateGrid(150+rand()%num);
    grid.fillGrid(frame);
    for(int z = 0; z < grid.g_h; ++z) {
        for(int i = 0; i < grid.g_w; ++i) {
            if(grid.boxes[i][z].on) {
                cv::Vec3b pixel = frame.at<cv::Vec3b>(z*grid.g_s, i*grid.g_s);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j]+grid.boxes[i][z].color[j])/2);
                    pixel[j] *= static_cast<unsigned char>(alpha);
                }
                fillRect(frame, ac::Rect(i*box_size, z*box_size, grid.g_s, grid.g_s), pixel);
            }
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::GridRandom(cv::Mat &frame) {
    static cv::Size s(0, 0);
    static const int box_size = 16;
    static Grid grid;
    if(frame.size() != s) {
        grid.g_random = true;
        grid.createGrid(frame, frame.cols/box_size, frame.rows/box_size, box_size);
        s = frame.size();
    }
    int num = 0;
    if(frame.rows >= 2000)
        num = 250;
    if(frame.rows >= 1080)
        num = 100;
    else if(frame.rows >= 720)
        num = 40;
    else if(frame.rows >= 400)
        num = 30;
    else
        num = 20;
    grid.updateGrid(75+rand()%num);
    grid.fillGrid(frame);
    for(int z = 0; z < grid.g_h; ++z) {
        for(int i = 0; i < grid.g_w; ++i) {
            if(grid.boxes[i][z].on)
                fillRect(frame, ac::Rect(i*box_size, z*box_size, grid.g_s, grid.g_s), grid.boxes[i][z].color);
        }
    }
}

void ac::GridRandomPixel(cv::Mat &frame) {
    static cv::Size s(0, 0);
    static const int box_size = 8;
    static Grid grid;
    if(frame.size() != s) {
        grid.createGrid(frame, frame.cols/box_size, frame.rows/box_size, box_size);
        s = frame.size();
    }
    int num = 0;
    if(frame.rows >= 2000)
        num = 400;
    if(frame.rows >= 1080)
        num = 200;
    else if(frame.rows >= 720)
        num = 100;
    else if(frame.rows >= 400)
        num = 50;
    else
        num = 20;
    grid.updateGrid(75+rand()%num);
    grid.fillGrid(frame);
    for(int z = 0; z < grid.g_h; ++z) {
        for(int i = 0; i < grid.g_w; ++i) {
            if(grid.boxes[i][z].on) {
                cv::Vec3b rpix(rand()%255,rand()%255,rand()%255);
                for(int j = 0; j < 3; ++j)
                    rpix[j] += grid.boxes[i][z].color[j];
                fillRect(frame, ac::Rect(i*box_size, z*box_size, grid.g_s, grid.g_s),rpix);
            }
        }
    }
}

