# -*- MakeFile -*-

all: mish

mish: main.c
	gcc -O2 -o $@ $^
	@strip --strip-unneeded $@

clean:
	rm mish
