NUME = tema1
CC = gcc
CFLAGS = -Wall
LIBS = -lm

build: $(NUME)

$(NUME): main.o utils.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $<

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(NUME)