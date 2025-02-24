OUT    := raster
CXX    := g++
FLAGS  := -std=c++20 -Wall
COMMAND = $(CXX) $(FLAGS) $^ -o
objects:= library.o matrix.o quaternion.o vectors.o 

$(OUT): FLAGS += -g3 -DDEBUG
$(OUT): main.cpp $(objects)
	$(COMMAND) $(OUT)

$(OUT)_release: FLAGS += -O3 -DNDEBUG
$(OUT)_release: main.cpp $(objects)
	$(COMMAND) $(OUT)_release

$(objects): %.o: $(addprefix library/, %.cpp)
	$(CC) $(FLAGS) -c $^ -o $@

clean:
	rm -rf *.o $(OUT)*

.PHONY: clean

