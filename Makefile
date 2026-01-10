OUT    := rasterizer
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o
objects:= library.o vectors.o quaternion.o matrix.o mesh.o light.o scene.o

$(OUT): FLAGS += -g3 -DDEBUG
$(OUT): main.cpp $(objects)
	$(COMMAND) $(OUT)

checkpoint: FLAGS += -g3 -DDEBUG
checkpoint: main_checkpoint.cpp $(objects)
	$(COMMAND) $(OUT)_checkpoint

release: FLAGS += -O3 -DNDEBUG
release: main_release.cpp $(objects)
	$(COMMAND) $(OUT)_release

$(objects): %.o: $(addprefix library/, %.cpp)
	$(CXX) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

