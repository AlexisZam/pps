CXX = g++
CXXFLAGS = -O3 -Wall
LIBS = -ltbb

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: $(OBJS) fw.cpp
	$(CXX) $(CXXFLAGS) -o fw fw.cpp util.cpp $(LIBS)

fw_sr: fw_sr.cpp
	$(CXX) $(CXXFLAGS) -o fw_sr fw_sr.cpp util.cpp $(LIBS)
	
fw_tiled: fw_tiled.cpp
	$(CXX) $(CXXFLAGS) -o fw_tiled fw_tiled.cpp util.cpp $(LIBS)

clean:
	$(RM) *.o fw fw_sr fw_tiled