CXXFLAGS=
LDFLAGS= -ldl

all:
	$(CXX) loadplugin.cpp -o plug-load $(LDFLAGS)
	$(CXX) -shared plugin.cpp -o plugin.dylib

clean:
	rm -f plug-load
