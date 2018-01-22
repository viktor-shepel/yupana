CC = gcc
CFLAGS += -g -Wall -std=c99 -MMD -MP

LINK = gcc
LINKFLAGS = -g

SOURCE_FOLDER = .
SOURCES = $(shell find $(SOURCE_FOLDER) -name '*.c')
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
REPL_OBJECTS = $(filter-out ./examples/calc.o, $(OBJECTS))
CALC_OBJECTS = $(filter-out ./examples/repl.o, $(OBJECTS))
DEPENDENCIES = $(patsubst %.c, %.d, $(SOURCES))

.PHONY: all

all: repl calc
	@echo -------------------------------------
	@echo run tests here
	@echo -------------------------------------

repl: $(REPL_OBJECTS)
	$(LINK) $(LINKFLAGS) $^ -o $@

calc: $(CALC_OBJECTS)
	$(LINK) $(LINKFLAGS) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm calc
	rm repl
	rm $(OBJECTS)
	rm $(DEPENDENCIES)

-include $(DEPENDENCIES)
