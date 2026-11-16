CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wpedantic -Iinclude -Ivendor/raylib/include
LDLIBS   = -lGL -lm -lpthread -ldl -lrt -lX11

# raylib precompilada: se usa la estatica si existe.
RAYLIB_STATIC := vendor/raylib/lib/libraylib.a
ifeq ($(wildcard $(RAYLIB_STATIC)),)
  RAYLIB_LIB := -Lvendor/raylib/lib -lraylib
else
  RAYLIB_LIB := $(RAYLIB_STATIC)
endif

# Symlinks a libGL/libX11 para maquinas sin los paquetes -dev.
LOCALLIB := -Lvendor/lib

# Tu juego: todo lo que este en src/.
SRC := $(wildcard src/*.cpp)
BIN := build/miniMAX

# Demo de la capa de render: no incluye src/main.cpp.
DEMO_SRC := examples/demo_render.cpp src/tablero.cpp src/render.cpp
DEMO_BIN := build/miniMAX-demo

all: $(BIN)

$(BIN): $(SRC) $(wildcard include/*.hpp)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(RAYLIB_LIB) $(LOCALLIB) $(LDLIBS)

demo: $(DEMO_SRC) $(wildcard include/*.hpp)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(DEMO_SRC) -o $(DEMO_BIN) $(RAYLIB_LIB) $(LOCALLIB) $(LDLIBS)

run: $(BIN)
	./$(BIN)

# Binario aparte para no pisar el de release.
debug: CXXFLAGS := $(filter-out -O2,$(CXXFLAGS)) -g -O0
debug:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o build/miniMAX-debug $(RAYLIB_LIB) $(LOCALLIB) $(LDLIBS)

clean:
	rm -rf build/*

.PHONY: all demo run debug clean
