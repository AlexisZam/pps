CXX = g++
CXXFLAGS = -O3 -Wall
LIBS = -ltbb

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: fw.cpp util.cpp
	$(CXX) $(CXXFLAGS) -o fw fw.cpp util.cpp $(LIBS)

fw_sr: fw_sr.cpp util.cpp
	$(CXX) $(CXXFLAGS) -o fw_sr fw_sr.cpp util.cpp $(LIBS)

fw_tiled: fw_tiled.cpp util.cpp
	$(CXX) $(CXXFLAGS) -o fw_tiled fw_tiled.cpp util.cpp $(LIBS)

clean:
	$(RM) fw fw_sr fw_tiled
