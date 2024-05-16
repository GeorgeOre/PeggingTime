CC = gcc
CFLAGS = -Wall -Wextra -Iinclude $(shell sdl2-config --cflags) -I/usr/include/SDL2
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm -lSDL2

SRC_DIR = src
OBJ_DIR = build
INC_DIR = include
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(OBJ_DIR)/game
TEST_TARGET = $(OBJ_DIR)/test_physics
TEST_CONTROLS_TARGET = $(OBJ_DIR)/test_controls
TEST_RENDERING_TARGET = $(OBJ_DIR)/test_rendering
TEST_GAME_TARGET = $(OBJ_DIR)/test_game

.PHONY: all clean test test_controls test_rendering test_game

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(OBJ_DIR)/test_physics.o $(OBJ_DIR)/physics.o
	$(CC) $(OBJ_DIR)/test_physics.o $(OBJ_DIR)/physics.o -o $@ $(LDFLAGS)

$(TEST_CONTROLS_TARGET): $(OBJ_DIR)/test_controls.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/physics.o
	$(CC) $(OBJ_DIR)/test_controls.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/physics.o -o $@ $(LDFLAGS)

$(TEST_RENDERING_TARGET): $(OBJ_DIR)/test_rendering.o $(OBJ_DIR)/rendering.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/physics.o
	$(CC) $(OBJ_DIR)/test_rendering.o $(OBJ_DIR)/rendering.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/physics.o -o $@ $(LDFLAGS)

$(TEST_GAME_TARGET): $(OBJ_DIR)/test_game.o $(OBJ_DIR)/game.o $(OBJ_DIR)/physics.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/rendering.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/ai.o
	$(CC) $(OBJ_DIR)/test_game.o $(OBJ_DIR)/game


# Dependency check (example for SDL2)
check-dependencies:
	@command -v sdl2-config >/dev/null 2>&1 || { echo >&2 "SDL2 is not installed. Aborting."; exit 1; }

# Build target with dependency check
build: check-dependencies all
