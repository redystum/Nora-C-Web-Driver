# Libraries to include (if any)
LIBS=-lcurl #-lm -pthread

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L -Werror=vla #-pg

# Linker flags
LDFLAGS=#-pg

# Indentation flags
# IFLAGS=-br -brs -brf -npsl -ce -cli4 -bli4 -nut
IFLAGS=-linux -brs -brf -br

## Name of the executable
PROGRAM=webtest

# Prefix for the gengetopt file (if gengetopt is used)
PROGRAM_OPT=args

## Object files required to build the executable
PROGRAM_OBJS=main.o utils.o web.o $(PROGRAM_OPT).o

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: $(PROGRAM)

# activate DEBUG, defining the SHOW_DEBUG macro
debugon: CFLAGS += -D DEBUG_ENABLED -g
debugon: $(PROGRAM)

# activate optimization (-O...)
OPTIMIZE_FLAGS=-O2 # possible values (for gcc): -O2 -O3 -Os -Ofast
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(PROGRAM)

$(PROGRAM): $(PROGRAM_OBJS)
	$(CC) -o $@ $(PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

## Dependencies
main.o: main.c utils.h $(PROGRAM_OPT).h
$(PROGRAM_OPT).o: $(PROGRAM_OPT).c $(PROGRAM_OPT).h

utils.o: utils.c utils.h
web.o: web.c web.h utils.h

# disable warnings from gengetopt generated files
$(PROGRAM_OPT).o: $(PROGRAM_OPT).c $(PROGRAM_OPT).h
	$(CC) -ggdb -std=c11 -pedantic -c $<

#how to create an object file (.o) from C file (.c)
.c.o:
	$(CC) $(CFLAGS) -c $<

# Generates command line arguments code from gengetopt configuration file
$(PROGRAM_OPT).c $(PROGRAM_OPT).h: $(PROGRAM_OPT).ggo
	gengetopt < $(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT)

clean:
	rm -f *.o core.* *~ $(PROGRAM) *.bak $(PROGRAM_OPT).h $(PROGRAM_OPT).c

docs: Doxyfile
	doxygen Doxyfile

Doxyfile:
	doxygen -g Doxyfile

# entry to create the list of dependencies
depend:
	$(CC) -MM *.c

indent:
	indent $(IFLAGS) *.c *.h && rm -f *~

# entry to run the pmccabe utility (computes the "complexity" of the code)
pmccabe:
	pmccabe -v *.c

# entry to run the cppcheck tool
cppcheck:
	cppcheck --enable=all --verbose --suppress=missingIncludeSystem *.c *.h


run: $(PROGRAM)
	./$(PROGRAM) -g ./geckodriver/geckodriver -f /opt/firefox/firefox
	
# verbose: $(PROGRAM)
# 	./$(PROGRAM) --verbose

# build: $(PROGRAM)
# 	./$(PROGRAM) -i main.lance -o generated -a -f -r

# debug: $(PROGRAM)
# 	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --suppressions=valgrind.supp ./$(PROGRAM) -i main.lance -o generated