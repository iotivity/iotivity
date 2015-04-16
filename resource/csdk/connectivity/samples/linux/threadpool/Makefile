CC=gcc
ROOT_DIR = ../../..
INC_DIR := -I$(ROOT_DIR)/api -I$(ROOT_DIR)/inc -I$(ROOT_DIR)/common/inc
SRC_DIR := $(ROOT_DIR)/common/src/
CFLAGS = -Wall

OUT = threadpool

all:
	$(CC) $(CFLAGS) $(INC_DIR) $(COMPILEFLAGS) $(SRC_DIR)cathreadpool_pthreads.c $(SRC_DIR)umutex.c $(SRC_DIR)/logger.c $(SRC_DIR)/oic_malloc.c main.c -o $(OUT) $(LDFLAGS)
clean:
	rm -f $(OUT)

