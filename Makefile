# corex - build the examples
#
#   make          build all examples into build/
#   make run      build, then run each example
#   make clean    remove build/
#
# Override the compiler with:  make CC=clang

CC      ?= cc
CFLAGS  ?= -std=c99 -Wall -Wextra -I.
LDFLAGS ?= -lm
BUILD   := build

EXAMPLES := pong camera_demo collision_demo
BINS     := $(addprefix $(BUILD)/,$(EXAMPLES))

.PHONY: all run clean

all: $(BINS)

$(BUILD)/%: examples/%.c corex.h | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

run: all
	@for bin in $(BINS); do \
		echo ""; \
		echo "===== $$bin ====="; \
		./$$bin; \
	done

clean:
	rm -rf $(BUILD)
