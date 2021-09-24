.DEFAULT_GOAL := all

CC = gcc

CFLAGS = -std=gnu99 -Wall -Wextra -Werror -ggdb -MD -fsanitize=undefined -Isrc/ -DVERBOSE
LDFLAGS = -fsanitize=undefined

DIRECTORY_GUARD=@mkdir -p $(@D)

TARGET   =  eos
SRCS     =  $(shell find src/ -name '*.c')
OBJS     =  $(patsubst %.c, $(BUILD_DIRECTORY)/%.c.o, $(SRCS))


BUILD_DIRECTORY ?= build

$(BUILD_DIRECTORY)/%.c.o: %.c
	$(DIRECTORY_GUARD)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(DIRECTORY_GUARD)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: all
all: $(TARGET)


.PHONY: clean
clean:
	-rm -r build
	-rm $(TARGET)
