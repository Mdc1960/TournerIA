CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wconversion -g
TARGET = main

SRC = $(wildcard *.cpp)


all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)
