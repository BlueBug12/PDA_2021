CXX := g++
CXXFLAGS := -std=c++11 -g -Wall -O3
TARGET := main
OBJ := main.o corner_stitching.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: $.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)

