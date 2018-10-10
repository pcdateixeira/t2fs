CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: objeto biblioteca

objeto: $(SRC_DIR)/t2fs.c
	$(CC) -c $(SRC_DIR)/t2fs.c -Wall
	mkdir -p $(BIN_DIR)
	mv t2fs.o $(BIN_DIR)/t2fs.o

biblioteca: $(BIN_DIR)/t2fs.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o
	ar crs $(LIB_DIR)/libt2fs.a $(BIN_DIR)/t2fs.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/t2fs.o