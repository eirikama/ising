# Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -O2

# Target name for the executable
TARGET = ising_simulation

# Source files
SOURCES = test_some_Ts.cpp ising_2d_simulation.cpp

# Object files (automatically generated from the source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Rule for building object files from source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
