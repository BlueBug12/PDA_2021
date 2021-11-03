CXX := g++
TARGET := Lab2
SRCDIR := ./src
OBJDIR := ./obj
CXXFLAGS := -std=c++11 -g -Wall -O3
SRCS := $(notdir $(wildcard ${SRCDIR}/*.cpp))
OBJS := $(addprefix $(OBJDIR)/, $(patsubst %.cpp, %.o, $(SRCS)))
SRCS := $(addprefix $(SRCDIR)/, $(SRCS))


$(shell [ -d $(OBJDIR) ] || mkdir -p $(OBJDIR))

all: $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

