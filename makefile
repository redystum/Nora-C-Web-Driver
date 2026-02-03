# Libraries
LIBS=-lcurl -lcjson #-lm -pthread

# Compiler flags
# Added -I./src so you can include headers easily (e.g., #include "core/web_core.h")
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L -Werror=vla -I./src

# Linker flags
LDFLAGS=

# Indentation flags
IFLAGS=-linux -brs -brf -br

## Program Definitions
PROGRAM_NAME=webtest
BUILD_DIR=build
PROGRAM=$(BUILD_DIR)/$(PROGRAM_NAME)
PROGRAM_OPT=args

# --------------------------------------------------------------------------
# AUTOMATIC FILE DISCOVERY
# --------------------------------------------------------------------------

# 1. Find all .c files inside src/ and its subdirectories (core, element, etc.)
MODULE_SRCS := $(wildcard src/*/*.c)

# 2. Define the Main sources (root directory files)
MAIN_SRCS := main.c $(PROGRAM_OPT).c

# 3. Find all .c files inside tests/
TEST_SRCS := $(wildcard tests/*.c)

# 4. Combine them
ALL_SRCS := $(MAIN_SRCS) $(MODULE_SRCS) $(TEST_SRCS)

# 4. Convert .c filenames to .o filenames inside the BUILD_DIR
#    Example: src/core/web_core.c -> build/src/core/web_core.o
PROGRAM_OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(ALL_SRCS))

# Ensure args.h is generated before compiling any object file
$(PROGRAM_OBJS): $(PROGRAM_OPT).h

# --------------------------------------------------------------------------
# TARGETS
# --------------------------------------------------------------------------

.PHONY: clean all docs indent debugon

all: $(PROGRAM)

# Debug build
debugon: CFLAGS += -D DEBUG_ENABLED -g
debugon: $(PROGRAM)

# Optimization
OPTIMIZE_FLAGS=-O2
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(PROGRAM)

# Linking the executable
$(PROGRAM): $(PROGRAM_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(PROGRAM_OBJS) $(LIBS) $(LDFLAGS)
	@echo "Build successful: $(PROGRAM)"

# --------------------------------------------------------------------------
# COMPILATION RULES
# --------------------------------------------------------------------------

# 1. Rule for gengetopt file (specific flags)
$(BUILD_DIR)/$(PROGRAM_OPT).o: $(PROGRAM_OPT).c $(PROGRAM_OPT).h
	@mkdir -p $(dir $@)
	$(CC) -ggdb -std=c11 -pedantic -c $(PROGRAM_OPT).c -o $@

# 2. GENERIC MAGIC RULE
#    This handles main.c, utils.c, AND any file deep inside src/
#    $(dir $@) ensures the folder (e.g., build/src/window/) exists before compiling
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --------------------------------------------------------------------------
# UTILITIES
# --------------------------------------------------------------------------

# Generate gengetopt files
$(PROGRAM_OPT).c $(PROGRAM_OPT).h: $(PROGRAM_OPT).ggo
	gengetopt < $(PROGRAM_OPT).ggo --file-name=$(PROGRAM_OPT)

clean:
	rm -f *.o core.* *~ *.bak $(PROGRAM_OPT).h $(PROGRAM_OPT).c
	rm -rf $(BUILD_DIR)

docs: Doxyfile
	doxygen Doxyfile

depend:
	$(CC) $(CFLAGS) -MM $(ALL_SRCS)

indent:
	indent $(IFLAGS) $(ALL_SRCS) *.h src/*/*.h && rm -f *~ src/*/*~

pmccabe:
	pmccabe -v $(ALL_SRCS)

cppcheck:
	cppcheck --enable=all --verbose --suppress=missingIncludeSystem $(ALL_SRCS) src/*/*.h

run: $(PROGRAM)
	./$(PROGRAM) -g ./geckodriver/geckodriver

all_debugon: debugon all

gecko:
	./geckodriver/geckodriver --port 9515 --binary /opt/firefox/firefox
test_firefox:
	curl -X POST http://127.0.0.1:9515/session \
         -H "Content-Type: application/json" \
         -d '{"capabilities": {"alwaysMatch": {"browserName": "firefox"}}}'