OUT    := rasterizer
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o
objects:= library.o vectors.o quaternion.o matrix.o mesh.o light.o

$(OUT): FLAGS += -g3 -DDEBUG
$(OUT): main.cpp $(objects)
	$(COMMAND) $(OUT)

$(OUT)_release: FLAGS += -O3 -DNDEBUG
$(OUT)_release: main.cpp $(objects)
	$(COMMAND) $(OUT)_release

$(objects): %.o: $(addprefix library/, %.cpp)
	$(CXX) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

