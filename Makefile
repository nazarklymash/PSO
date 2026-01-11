# Makefile dla projektu PSO - Symulator Misji Ratunkowej

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

# Nazwy programów wynikowych
TARGET = pso
GENERATOR = map_generator

# Pliki źródłowe głównego programu
SRCS = main.c map.c pso.c logger.c
OBJS = $(SRCS:.c=.o)

# Pliki źródłowe generatora
GEN_SRCS = map_generator.c utils.c
GEN_OBJS = $(GEN_SRCS:.c=.o)

# Reguła domyślna - buduje oba programy
all: $(TARGET) $(GENERATOR)

# Linkowanie programu głównego
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Linkowanie generatora map
$(GENERATOR): $(GEN_OBJS)
	$(CC) $(CFLAGS) -o $(GENERATOR) $(GEN_OBJS) $(LDFLAGS)

# Kompilacja plików .c do .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Zależności
main.o: main.c map.h pso.h
map.o: map.c map.h
pso.o: pso.c pso.h map.h logger.h
logger.o: logger.c logger.h pso.h
utils.o: utils.c utils.h
map_generator.o: map_generator.c utils.h

# Czyszczenie
clean:
	rm -f $(OBJS) $(GEN_OBJS) $(TARGET) $(GENERATOR) *.exe

# Ponowna kompilacja
rebuild: clean all

# Uruchomienie z przykładową mapą
run: $(TARGET)
	./$(TARGET) test_map.txt -p 30 -i 100 -n 5

# Generowanie przykładowej mapy
generate: $(GENERATOR)
	./$(GENERATOR) generated_map.txt -w 25 -h 25 -s 100

# Sprawdzenie wycieków pamięci (wymaga valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) test_map.txt -p 10 -i 50

.PHONY: all clean rebuild run generate memcheck

