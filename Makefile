# -*- MakeFile -*-

all: mish

mish: src/main.c
	gcc -O2 -o $@ $^
	@strip --strip-unneeded $@

clean:
	rm mish

