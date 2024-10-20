# -*- MakeFile -*-

SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

OPT := -O3
CFLAGS += -c -Wall -I$(INC_DIR) $(OPT)
TARGET := mish


.PHONY: all clean remake

all: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@
	@strip --strip-unneeded $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ_DIR)

remake: clean all
