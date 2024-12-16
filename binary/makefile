PROGRAM := korsord

CLEAN_TARGET := clean
HELP_TARGET  := help

DELETE_CMD := rm

COMPILER := gcc
COMPILE_FLAGS := -Wall -Wno-unused-variable -g -pg -O0 -oFast -pthread -funroll-loops -ftree-vectorize -floop-strip-mine -march=native

# AddressSanitizer
# -fsanitize=address


# DEBUG_COMPILE_FLAGS := -O0
# SPEED_COMPILE_FLAGS := -O3

LINKER_FLAGS := -lncurses -lpthread -pg -g -O0

# AddressSanitizer
# -lasan -static-libgcc -static-libstdc++ -fsanitize=address

# DEBUG_LINKER_FLAGS :=
# SPEED_LINKER_FLAGS :=

SOURCE_DIR := ../source
OBJECT_DIR := ../object
BINARY_DIR := ../binary

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*/*.c $(SOURCE_DIR)/*.c)
HEADER_FILES := $(wildcard $(SOURCE_DIR)/*/*.h $(SOURCE_DIR)/*.h)

OBJECT_FILES := $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCE_FILES:.c=.o)))

all: $(PROGRAM)

$(PROGRAM): $(OBJECT_FILES) $(SOURCE_FILES) $(HEADER_FILES)
	$(COMPILER) $(OBJECT_FILES) $(LINKER_FLAGS) -o $(BINARY_DIR)/$(PROGRAM)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/*/%.c
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

.PRECIOUS: $(OBJECT_DIR)/%.o $(PROGRAM)

DEBUG_FILES := vgcore* gmon.out profile.txt

BINARY_FILES := $(addprefix $(BINARY_DIR)/, $(DEBUG_FILES) $(PROGRAM)) 

$(CLEAN_TARGET):
	$(DELETE_CMD) $(OBJECT_DIR)/*.o $(BINARY_FILES)

$(HELP_TARGET):
	@echo $(PROGRAM) $(CLEAN_TARGET)
