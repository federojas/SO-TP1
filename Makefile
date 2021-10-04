CC=gcc
GCCFLAGS = -g -Wall -std=c99 
GCCLIBS = -lrt -lpthread
FILES =  error_handling.c shared_data.c 
MAIN_FILES = app view slave view2


all: $(MAIN_FILES)

$(MAIN_FILES): %: %.c
	@$(CC) $(GCCFLAGS) $(FILES) -o $@ $< $(GCCLIBS)

.PHONY: clean
clean:
	@rm -rf $(MAIN_FILES)
