OUT    := rasterizer
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o
HEAD   := library.o matrix.o

$(OUT): FLAGS += -g3 -DDEBUG
$(OUT): main.cpp $(HEAD)
	$(COMMAND) $(OUT)

$(OUT)_release: FLAGS += -O3 -DNDEBUG
$(OUT)_release: main.cpp $(HEAD)
	$(COMMAND) $(OUT)_release

library.o: library/library.cpp
	$(CXX) $(FLAGS) -c $^ -o $@

matrix.o: library/matrix.cpp
	$(CXX) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

