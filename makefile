TARGET = build/graph

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

# Пути SFML (для macOS через brew)
SFML_INCLUDE = /opt/homebrew/include
SFML_LIB = /opt/homebrew/lib
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = src/main.cpp src/Graph.cpp src/Node.cpp src/Edge.cpp src/utils.cpp
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p build
	$(CXX) $(OBJS) -o $(TARGET) -I$(SFML_INCLUDE) -L$(SFML_LIB) $(SFML_LIBS)

# Компиляция .cpp -> .o
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -I$(SFML_INCLUDE) -c $< -o $@

# Запуск
run: $(TARGET)
	./$(TARGET)

# Очистка
clean:
	rm -rf build

format:
	clang-format -i $(SRCS) include/*.hpp

tidy:
	clang-tidy $(SRCS) -- -Iinclude -I$(SFML_INCLUDE) -std=c++17
