CXX = g++
CXXFLAGS = -Wall -std=c++17
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = main
PARAMS = test_programs/prog3.asm

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(EXEC) $(PARAMS)
	rm -f $(EXEC) $(OBJECTS)

clean:
	rm -f $(EXEC) $(OBJECTS)
