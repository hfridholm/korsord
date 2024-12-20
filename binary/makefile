PROGRAM := korsord

CLEAN_TARGET := clean
HELP_TARGET  := help

DELETE_CMD := rm

COMPILER := gcc

# AddressSanitizer
# -fsanitize=address
# -lasan -static-libgcc -static-libstdc++ -fsanitize=address

DEBUG_COMPILE_FLAGS := -Wall -Wno-unused-function -Wno-unused-variable -g -pg -O0 -oFast -pthread -funroll-loops -ftree-vectorize -floop-strip-mine -march=native

DEBUG_LINKER_FLAGS := -lncurses -lpthread -pg -g -O0


SPEED_COMPILE_FLAGS := -O3 -pthread -oFast -Wno-unused-function

SPEED_LINKER_FLAGS := -lncurses -lpthread


SOURCE_DIR := ../source
OBJECT_DIR := ../object
BINARY_DIR := ../binary

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*/*.c $(SOURCE_DIR)/*.c)
HEADER_FILES := $(wildcard $(SOURCE_DIR)/*/*.h $(SOURCE_DIR)/*.h)

OBJECT_FILES := $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCE_FILES:.c=.o)))

all: debug

speed: COMPILE_FLAGS := $(SPEED_COMPILE_FLAGS)
speed: $(OBJECT_FILES) $(SOURCE_FILES) $(HEADER_FILES)
	$(COMPILER) $(OBJECT_FILES) $(SPEED_LINKER_FLAGS) -o $(BINARY_DIR)/$(PROGRAM)

debug: COMPILE_FLAGS := $(DEBUG_COMPILE_FLAGS)
debug: $(OBJECT_FILES) $(SOURCE_FILES) $(HEADER_FILES)
	$(COMPILER) $(OBJECT_FILES) $(DEBUG_LINKER_FLAGS) -o $(BINARY_DIR)/$(PROGRAM)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/*/%.c
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(COMPILER) $< -c $(COMPILE_FLAGS) -o $@

.PRECIOUS: $(OBJECT_DIR)/%.o $(PROGRAM)

DEBUG_FILES := vgcore* gmon.out profile.txt debug.log

BINARY_FILES := $(addprefix $(BINARY_DIR)/, $(DEBUG_FILES) $(PROGRAM)) 

$(CLEAN_TARGET):
	truncate -s 0 $(BINARY_DIR)/output.txt
	$(DELETE_CMD) $(OBJECT_DIR)/*.o $(BINARY_FILES)

$(HELP_TARGET):
	@echo $(PROGRAM) $(CLEAN_TARGET)
