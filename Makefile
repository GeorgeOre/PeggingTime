CC = gcc
CFLAGS = -Wall -Wextra -Iinclude $(shell sdl2-config --cflags) -I/usr/include/SDL2
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm

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
	$(CC) $(OBJ_DIR)/test_game.o $(OBJ_DIR)/game.o $(OBJ_DIR)/physics.o $(OBJ_DIR)/controls.o $(OBJ_DIR)/rendering.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/ai.o -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(TARGET)
	rm -rf $(TEST_TARGET)
	rm -rf $(TEST_CONTROLS_TARGET)
	rm -rf $(TEST_RENDERING_TARGET)
	rm -rf $(TEST_GAME_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

test_controls: $(TEST_CONTROLS_TARGET)
	./$(TEST_CONTROLS_TARGET)

test_rendering: $(TEST_RENDERING_TARGET)
	./$(TEST_RENDERING_TARGET)

test_game: $(TEST_GAME_TARGET)
	./$(TEST_GAME_TARGET)
