/* Plugin Loading test source file 
   written by Jared Bruni
*/



#include<dlfcn.h>
#include<iostream>


typedef void (*pixel)(int x, int y, unsigned char *pixels);
typedef void (*drawn)();

int main(int argc, char **argv) {
	
	void *library;
	library = dlopen(argv[1], RTLD_LAZY);
	
	if(library == NULL) {
		std::cerr << "Error could not open: " << argv[1] << "\n";
		exit(1);
	}
	
	void *addr;

	// load the plugin function to process pixels
	addr = dlsym(library, "pixel");
	pixel pix;
	pix = reinterpret_cast<pixel>(addr);
	const char *error;
	error = dlerror();
	if(error) {
		std::cerr << "Could not load pixel: " << error << "\n";
		exit(1);
	}

	void *dr = dlsym(library,"drawn");
	drawn drawn_;
	drawn_ = reinterpret_cast<drawn>(dr);
	error = dlerror();
	if(error) {
		std::cerr << "Error could not load drawn function: " << error << "\n";
		exit(1);
	}

	// test plugin:	
	for(unsigned int x = 0; x < 640; ++x) {
		for(unsigned int y = 0; y < 480; ++y) {
			unsigned char rgb[3] = { rand()%255, rand()%255, rand()%255 };
			(*pix)(x,y,rgb);
		}
	}

	(*drawn_)();	

	dlclose(library);
        std::cout << "Plugin executed successfully..\n";
	return 0;
	
}
