/* Plugin Loading test source file
 written by Jared Bruni
 */



#include<dlfcn.h>
#include<iostream>


typedef void (*pixel)(int x, int y, unsigned char *pixels);
typedef void (*drawn)();

pixel pix;
drawn drawn_;

void testplugin(int iterations) {
    // loop
    for(unsigned int i = 0; i < iterations; ++i) {
        for(unsigned int x = 0; x < 640; ++x) {
            for(unsigned int y = 0; y < 480; ++y) {
                unsigned char rgb[3] = { rand()%255, rand()%255, rand()%255 };
                (*pix)(x,y,rgb); // test random pixel
            }
        }
        (*drawn_)(); // test drawn command
        std::cout << "Tested iteration number: " << i << "\n";
    }
}

int main(int argc, char **argv) {
    
    if(argc != 3) {
        std::cerr << "Use:\n" << argv[0] << " plugin iterations\n";
        exit(1);
    }
    
    int iter = atoi(argv[2]);
    if(iter <= 0) {
        std::cerr << "Error iterations must be a positive integer.\n";
        exit(1);
    }
    
    void *library;
    library = dlopen(argv[1], RTLD_LAZY);
    
    if(library == NULL) {
        std::cerr << "Error could not open: " << argv[1] << "\n";
        exit(1);
    }
    
    void *addr;
    
    // load the plugin function to process pixels
    addr = dlsym(library, "pixel");
    pix = reinterpret_cast<pixel>(addr);
    const char *error;
    error = dlerror();
    if(error) {
        std::cerr << "Could not load pixel: " << error << "\n";
        exit(1);
    }
    
    void *dr = dlsym(library,"drawn");
    drawn_ = reinterpret_cast<drawn>(dr);
    error = dlerror();
    if(error) {
        std::cerr << "Error could not load drawn function: " << error << "\n";
        exit(1);
    }
    
    // test plugin:
    testplugin(iter);
    
    dlclose(library);
    std::cout << "Plugin executed successfully..\n";
    return 0;
    
}
