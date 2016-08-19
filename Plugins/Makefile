CXXFLAGS=-O3

all: plug-debug plugin-blend.dylib plugin-selfalpha.dylib plugin-randcolor.dylib plugin-selfalpha-reblend.dylib temp-plugin.dylib

plug-debug: loadplugin.cpp
	$(CXX) $(CXXFLAGS) loadplugin.cpp -o plug-debug

temp-plugin.dylib: temp-plugin.cpp
	$(CXX) $(CXXFLAGS) -shared temp-plugin.cpp -o temp-plugin.dylib
plugin.dylib: plugin.cpp
	$(CXX) $(CXXFLAGS) -shared plugin.cpp -o plugin.dylib
plugin-blend.dylib: plugin-blend.cpp
	$(CXX) $(CXXFLAGS) -shared plugin-blend.cpp -o plugin-blend.dylib
plugin-selfalpha.dylib: plugin-selfalpha.cpp
	$(CXX) $(CXXFLAGS) -shared plugin-selfalpha.cpp -o plugin-selfalpha.dylib
plugin-randcolor.dylib: plugin-randcolor.cpp
	$(CXX) $(CXXFLAGS) -shared plugin-randcolor.cpp -o plugin-randcolor.dylib
plugin-selfalpha-reblend.dylib: plugin-selfalpha-reblend.cpp
	$(CXX) $(CXXFLAGS) -shared plugin-selfalpha-reblend.cpp -o plugin-selfalpha-reblend.dylib

install: plug-debug
	cp plug-debug /usr/local/bin/plug-debug

clean:
	rm -f plug-debug *.dylib
