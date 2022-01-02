CXX := g++
TARGET := Lab4
SRCDIR := ./src
OBJDIR := ./obj
CXXFLAGS := -std=c++17 -g -Wall -O3
SRCS := $(notdir $(wildcard ${SRCDIR}/*.cpp))
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
SRCS := $(addprefix $(SRCDIR)/, $(SRCS))

$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

all: $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) -fopenmp $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -fopenmp $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET)
