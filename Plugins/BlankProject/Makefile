CXXFLAGS=-std=c++11 -O3 -shared

plugin.dylib: plugin.cpp
	$(CXX) $(CXXFLAGS) plugin.cpp -o plugin.dylib
clean:
	rm -f plugin.dylib
