CXX = g++
CXXFLAGS = -Wall -std=c++17
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXEC = main
# PARAMS = test_programs/prog_jmp_consts.asm test_programs/prog_jmp.asm
# PARAMS = test_programs/prog_erro_word.asm test_programs/prog_erro_word2.asm
# PARAMS = test_programs/prog_erro_word2.asm test_programs/prog_erro_word3.asm
PARAMS = test_programs/test_mult_prog.asm test_programs/test_mult_prog1.asm test_programs/test_mult_prog2.asm test_programs/test_mult_prog3.asm test_programs/test_mult_prog3.asm

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
