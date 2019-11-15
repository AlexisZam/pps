CXX = g++
CXXFLAGS = -O3 -Wall
HDEPS += %.h
OBJS = util.o
LIBS = -ltbb

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: $(OBJS) fw.cpp
	$(CXX) $(CXXFLAGS) -o fw $(OBJS) fw.cpp $(LIBS)
fw_sr: fw_sr.cpp
	$(CXX) $(CXXFLAGS) -o fw_sr $(OBJS) fw_sr.cpp $(LIBS)
fw_tiled: fw_tiled.cpp
	$(CXX) $(CXXFLAGS) -o fw_tiled $(OBJS) fw_tiled.cpp $(LIBS)

%.o: %.cpp $(HDEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o fw fw_sr fw_tiled