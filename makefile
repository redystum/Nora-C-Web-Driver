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
PROGRAM_NAME=webtest
BUILD_DIR=build
PROGRAM=$(BUILD_DIR)/$(PROGRAM_NAME)

# Prefix for the gengetopt file (if gengetopt is used)
PROGRAM_OPT=args

## Object files required to build the executable (now in build/)
PROGRAM_OBJS=$(addprefix $(BUILD_DIR)/, main.o utils.o web.o gecko.o getters.o requests.o web_utils.o $(PROGRAM_OPT).o)

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: $(PROGRAM)

# activate DEBUG, defining the SHOW_DEBUG macro
debugon: CFLAGS += -D DEBUG_ENABLED -g
debugon: $(PROGRAM)

all_debugon: debugon all

# activate optimization (-O...)
OPTIMIZE_FLAGS=-O2 # possible values (for gcc): -O2 -O3 -Os -Ofast
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(PROGRAM)

$(PROGRAM): $(PROGRAM_OBJS)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

## Dependencies
# object files now live in $(BUILD_DIR)
$(BUILD_DIR)/main.o: main.c utils.h web/web.h $(PROGRAM_OPT).h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $@

$(BUILD_DIR)/utils.o: utils.c utils.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c utils.c -o $@

# Rule for files in web/ directory
$(BUILD_DIR)/%.o: web/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# disable warnings from gengetopt generated files (object also in build/)
$(BUILD_DIR)/$(PROGRAM_OPT).o: $(PROGRAM_OPT).c $(PROGRAM_OPT).h
	mkdir -p $(BUILD_DIR)
	$(CC) -ggdb -std=c11 -pedantic -c $(PROGRAM_OPT).c -o $@

# generic rule: any other .c → build/%.o
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

#how to create an object file (.o) from C file (.c)
.c.o:
	$(CC) $(CFLAGS) -c $<

# Generates command line arguments code from gengetopt configuration file
$(PROGRAM_OPT).c $(PROGRAM_OPT).h: $(PROGRAM_OPT).ggo
	gengetopt < $(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT)

clean:
	rm -f *.o core.* *~ *.bak $(PROGRAM_OPT).h $(PROGRAM_OPT).c
	rm -rf $(BUILD_DIR)

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