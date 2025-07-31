# Compiler and directories
CC = gcc
SRCDIR = src
OBJDIR = obj

# Source and object files
SRC = $(wildcard $(shell find $(SRCDIR) -name '*.c'))
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

# Targets
TARGET = programa-principal
TEST_TARGET = programa-testes
INTERACTIVE_TARGET = programa-interativo

# Flags and options
FLAGS = -g -ffunction-sections -fdata-sections
INCLUDE = $(shell pkg-config --cflags glib-2.0 ncurses)
LIBS = $(shell pkg-config --libs glib-2.0 ncurses)
LINKER_FLAGS = -lm -Wl,--gc-sections

# Dependency files
DEP = $(OBJ:.o=.d)

# Default target
all: release

# Debug mode
debug: FLAGS += -g3 -DDEBUG -DDEBUG_FLAG="\"enabled\""
debug: clean $(TARGET) $(TEST_TARGET) $(INTERACTIVE_TARGET)

# Release mode
release: FLAGS += -O3 -DNDEBUG
release: clean $(TARGET) $(TEST_TARGET) $(INTERACTIVE_TARGET)

# Programa Principal
$(TARGET): $(OBJ)
	$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(TARGET) $(LIBS) $(LINKER_FLAGS)

# Programa de Testes
$(TEST_TARGET): $(OBJ)
	$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(TEST_TARGET) $(LIBS) $(LINKER_FLAGS)

# Programa Interativo
$(INTERACTIVE_TARGET): $(OBJ)
	$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(INTERACTIVE_TARGET) $(LIBS) $(LINKER_FLAGS)

# Rules for building .o from .c
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -MMD -MP -c $(INCLUDE) $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET) $(TEST_TARGET) $(INTERACTIVE_TARGET)

# Run valgrind for memory checks
memcheck: debug
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)

# Linting (optional, requires clang-tidy)
lint:
	clang-tidy $(SRC)

# Include dependency files
-include $(DEP)

# Phony targets
.PHONY: all debug release clean memcheck lint
