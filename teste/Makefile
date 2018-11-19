#
# Makefile para programas de teste
# 

CC=gcc
LIB_DIR=../lib
EXEMPLO_DIR=../exemplo

all: teste main

teste: teste.c $(LIB_DIR)/libt2fs.a
	$(CC) -o teste teste.c -L$(LIB_DIR) -lt2fs -Wall
	mkdir -p $(EXEMPLO_DIR)
	mv teste $(EXEMPLO_DIR)/teste

main: main.c $(LIB_DIR)/libt2fs.a
	$(CC) -o main main.c -L$(LIB_DIR) -lt2fs -Wall
	mkdir -p $(EXEMPLO_DIR)
	mv main $(EXEMPLO_DIR)/main
	
clean:
	rm -rf $(EXEMPLO_DIR)/* *.o *~