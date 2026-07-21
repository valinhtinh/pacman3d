CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude -Iinclude/SDL2 -MMD -MP
LDFLAGS  ?= -lSDL2 -lSDL2_ttf

SRC_DIR  := src
BUILD_DIR:= build
TARGET   := pacman3d

SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS     := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: all
	./$(TARGET)

-include $(DEPS)
