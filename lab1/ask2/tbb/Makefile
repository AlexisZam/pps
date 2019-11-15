CXX = gcc
CXXFLAGS = -O3 -Wall

HDEPS += %.h

OBJS = util.o

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: $(OBJS) fw.cpp
	$(CXX) $(CXXFLAGS) $(OBJS) fw.cpp -o fw
fw_sr: fw_sr.cpp
	$(CXX) $(CXXFLAGS) $(OBJS) fw_sr.cpp -o fw_sr
fw_tiled: fw_tiled.cpp
	$(CXX) $(CXXFLAGS) $(OBJS) fw_tiled.cpp -o fw_tiled

%.o: %.cpp $(HDEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o fw fw_sr fw_tiled

