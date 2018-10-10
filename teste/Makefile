#
# Makefile para programas de teste
# 

CC=gcc
LIB_DIR=../lib

all: teste

teste: teste.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste teste.c -L$(LIB_DIR) -lt2fs -Wall
	
clean:
	rm -rf teste *.o *~