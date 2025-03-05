#
# makefile
#
# Written by Hampus Fridholm
#

.PHONY: korsord apt-packages pip-packages

default: apt-packages pip-packages config-init korsord

# Define required apt packages
APT_PACKAGES := gcc pip

# Define required pip packages
PIP_PACKAGES := cython

CONFIG_DIR := $(HOME)/.korsord

BASE_DIR := ..

SOURCE_DIR := $(BASE_DIR)/source
OBJECT_DIR := $(BASE_DIR)/object
BINARY_DIR := $(BASE_DIR)/binary
ASSETS_DIR := $(BASE_DIR)/assets

words-copy:
	@echo "Copying .words files from $(ASSETS_DIR)/words to $(CONFIG_DIR)/words..."
	@find "$(ASSETS_DIR)/words" -type f -name "*.words" | \
		while read file; do \
			relative_path=$$(echo $$file | sed 's|$(ASSETS_DIR)/||'); \
			dest="$(CONFIG_DIR)/$$relative_path"; \
      dest_dir=$$(dirname "$$dest"); \
			mkdir -p "$$dest_dir"; \
			cp "$$file" "$$dest"; \
		done
	@echo "Files copied to $(CONFIG_DIR)/words."

models-copy:
	@echo "Copying .model files from $(ASSETS_DIR)/models to $(CONFIG_DIR)/models..."
	@find "$(ASSETS_DIR)/models" -type f -name "*.model" | \
		while read file; do \
			relative_path=$$(echo $$file | sed 's|$(ASSETS_DIR)/||'); \
			dest="$(CONFIG_DIR)/$$relative_path"; \
      dest_dir=$$(dirname "$$dest"); \
			mkdir -p "$$dest_dir"; \
			cp "$$file" "$$dest"; \
		done
	@echo "Files copied to $(CONFIG_DIR)/models."

config-copy:
	@echo "Copying files to $(CONFIG_DIR)...";
	@$(MAKE) --no-print-directory words-copy;
	@$(MAKE) --no-print-directory models-copy;
	@echo "Copied files to $(CONFIG_DIR).";

# Target to check if directory exists, create if not, and copy files
config-init:
	@echo "Checking if directory $(CONFIG_DIR) exists..."
	@if [ ! -d "$(CONFIG_DIR)" ]; then \
		echo "Directory $(CONFIG_DIR) does not exist. Creating it..."; \
		mkdir -p "$(CONFIG_DIR)"; \
		mkdir -p "$(CONFIG_DIR)/words"; \
		mkdir -p "$(CONFIG_DIR)/clues"; \
		mkdir -p "$(CONFIG_DIR)/models"; \
		mkdir -p "$(CONFIG_DIR)/grids"; \
		mkdir -p "$(CONFIG_DIR)/images"; \
		$(MAKE) --no-print-directory config-copy; \
	else \
		echo "Directory $(CONFIG_DIR) already exists."; \
	fi

# Define the target for installing packages
apt-packages:
	@echo "Checking and installing missing packages..."
	@for package in $(PAPT_PACKAGES); do \
		if ! dpkg -l | grep -q $$package; then \
			echo "Package $$package not found. Installing..."; \
			sudo apt-get update && sudo apt-get install -y $$package; \
		else \
			echo "Package $$package already installed."; \
		fi \
	done

# Target for checking and installing packages
pip-packages:
	@echo "Checking if required packages are installed..."
	@for package in $(PIP_PACKAGES); do \
		if ! pip show $$package > /dev/null 2>&1; then \
			echo "$$package is not installed. Installing..."; \
			pip install $$package; \
		else \
			echo "$$package is already installed."; \
		fi \
	done

# AddressSanitizer
# -fsanitize=address
# -lasan -static-libgcc -static-libstdc++ -fsanitize=address

DEBUG_COMPILE_FLAGS := -Wall -Wno-unused-function -Wno-unused-variable -g -pg -O0 -oFast -pthread -funroll-loops -ftree-vectorize -floop-strip-mine -march=native

DEBUG_LINKER_FLAGS := -lncurses -lpthread -pg -g -O0


SPEED_COMPILE_FLAGS := -O3 -pthread -oFast -Wno-unused-function

SPEED_LINKER_FLAGS := -lncurses -lpthread

SOURCE_FILES := $(wildcard $(SOURCE_DIR)/*/*.c $(SOURCE_DIR)/*.c)
HEADER_FILES := $(wildcard $(SOURCE_DIR)/*/*.h $(SOURCE_DIR)/*.h)

OBJECT_FILES := $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCE_FILES:.c=.o)))

PYTHON_SCRIPTS := $(wildcard $(BINARY_DIR)/*.py)

korsord: $(PYTHON_SCRIPTS) $(BINARY_DIR)/grid-gen
	cython $(BINARY_DIR)/korsord.py --embed -o $(BINARY_DIR)/korsord.c
	gcc -Os $$(python3-config --includes) $(BINARY_DIR)/korsord.c -o $(BINARY_DIR)/korsord $$(python3-config --ldflags --embed)
	@-rm 2>/dev/null $(BINARY_DIR)/korsord.c

$(BINARY_DIR)/grid-gen: COMPILE_FLAGS := $(SPEED_COMPILE_FLAGS)
$(BINARY_DIR)/grid-gen: $(OBJECT_FILES) $(SOURCE_FILES) $(HEADER_FILES)
	gcc $(OBJECT_FILES) $(SPEED_LINKER_FLAGS) -o $(BINARY_DIR)/grid-gen

debug: COMPILE_FLAGS := $(DEBUG_COMPILE_FLAGS)
debug: $(OBJECT_FILES) $(SOURCE_FILES) $(HEADER_FILES)
	gcc $(OBJECT_FILES) $(DEBUG_LINKER_FLAGS) -o $(BINARY_DIR)/grid-gen

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/*/%.c
	gcc $< -c $(COMPILE_FLAGS) -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	gcc $< -c $(COMPILE_FLAGS) -o $@

DEBUG_FILES := $(addprefix $(BINARY_DIR)/, vgcore* gmon.out profile.txt debug.log)

BINARY_FILES := $(addprefix $(BINARY_DIR)/, grid-gen korsord korsord.c)

.PRECIOUS: $(OBJECT_FILES) $(BINARY_FILES)

clean:
	-truncate -s 0 $(BINARY_DIR)/output.log
	-rm 2>/dev/null $(OBJECT_DIR)/*.o $(BINARY_FILES) $(DEBUG_FILES)

config-remove:
	@echo "Checking if directory $(CONFIG_DIR) exists..."
	@if [ ! -d "$(CONFIG_DIR)" ]; then \
		echo "Directory $(CONFIG_DIR) does not exist. Do nothing..."; \
	else \
		echo "Directory $(CONFIG_DIR) exists. Removing it..."; \
		rm -r $(CONFIG_DIR); \
	fi

remove: config-remove
