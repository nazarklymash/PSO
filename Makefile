# Makefile dla projektu PSO - Symulator Misji Ratunkowej

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lm

# Nazwy programów wynikowych
TARGET = pso

# Pliki źródłowe głównego programu
SRCS = main.c map.c pso.c logger.c utils.c
OBJS = $(SRCS:.c=.o)

# Reguła domyślna - buduje oba programy
all: $(TARGET) $(GENERATOR)

# Linkowanie programu głównego
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)


# Kompilacja plików .c do .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Zależności
main.o: main.c map.h pso.h
map.o: map.c map.h
pso.o: pso.c pso.h map.h logger.h
logger.o: logger.c logger.h pso.h
utils.o: utils.c utils.h

# Czyszczenie
clean:
	rm -f $(OBJS) $(GEN_OBJS) $(TARGET) *.exe

# Ponowna kompilacja
rebuild: clean all

# Uruchomienie z przykładową mapą
run: $(TARGET)
	./$(TARGET) test_map.txt -p 30 -i 100 -n 5

# Sprawdzenie wycieków pamięci (wymaga valgrind)
memcheck: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) test_map.txt -p 10 -i 50

.PHONY: all clean rebuild run generate memcheck

