# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

# Include and library paths
INCLUDE = -I /opt/homebrew/include/eigen3 -I/opt/homebrew/Cellar/sdl2/2.30.8/include
LIB = -L/opt/homebrew/Cellar/sdl2/2.30.8/lib -lSDL2

# Target executable
TARGET = main

# Source files
SRC = input.cpp validity.cpp geometry.cpp projections.cpp transformations.cpp main.cpp

# Object files (replace .cpp with .o)
OBJ = $(SRC:.cpp=.o)

# Build target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(INCLUDE) $(LIB)

# Rule for each .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

# Clean rule to remove compiled files
clean:
	rm -f $(OBJ) $(TARGET)
