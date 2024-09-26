# -*- MakeFile -*-

TARGET=mish

all: $(TARGET)

$(TARGET): src/main.c
	gcc -O2 -o $(TARGET) $^
	@strip --strip-unneeded $(TARGET)

mem.test: src/main.c
	gcc -g -Og $^ -o $(TARGET)
	valgrind -v --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm $(TARGET)

