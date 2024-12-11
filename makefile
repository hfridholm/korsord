COMPILE_FLAGS := -Wall

LINKER_FLAGS := -lncurses

korsord: korsord.c words.h
	gcc korsord.c $(COMPILE_FLAGS) $(LINKER_FLAGS) -o korsord

clean:
	rm korsord
