CXXFLAGS=
LDFLAGS= -ldl

all:
	$(CXX) loadplugin.cpp -o plug-load $(LDFLAGS)
	$(CXX) -shared plugin.cpp -o plugin.dylib
	$(CXX) -shared plugin-blend.cpp -o plugin-blend.dylib

clean:
	rm -f plug-load
