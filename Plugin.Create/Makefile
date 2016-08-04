CXXFLAGS= -O3
LDFLAGS= -ldl

all:
	$(CXX) $(CXXFLAGS) loadplugin.cpp -o plug-load $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -shared plugin.cpp -o plugin.dylib
	$(CXX) $(CXXFLAGS) -shared plugin-blend.cpp -o plugin-blend.dylib
	$(CXX) $(CXXFLAGS) -shared plugin-selfalpha.cpp -o plugin-selfalpha.dylib
	$(CXX) $(CXXFLAGS) -shared plugin-randcolor.cpp -o plugin-randcolor.dylib
	$(CXX) $(CXXFLAGS) -shared plugin-selfalpha-reblend.cpp -o plugin-selfalpha-reblend.dylib

clean:
	rm -f plug-load
