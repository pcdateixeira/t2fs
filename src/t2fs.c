#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int identify2 (char *name, int size) {
	strncpy (name, "Humberto Lentz - 242308\nMakoto Ishikawa - 216728\nPedro Teixeira - 228509\0", size);
	return 0;
}

FILE2 create2 (char *filename) {
	return -1;
}

int delete2 (char *filename) {
	return -1;
}

FILE2 open2 (char *filename) {
	return -1;
}

int close2 (FILE2 handle) {
	return -1;
}

int read2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

int write2 (FILE2 handle, char *buffer, int size) {
	return -1;
}

int truncate2 (FILE2 handle) {
	return -1;
}

int seek2 (FILE2 handle, DWORD offset) {
	return -1;
}

int mkdir2 (char *pathname) {
	return -1;
}

int rmdir2 (char *pathname) {
	return -1;
}

int chdir2 (char *pathname) {
	return -1;
}

int getcwd2 (char *pathname, int size) {
	return -1;
}

DIR2 opendir2 (char *pathname) {
	return -1;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
	return -1;
}

int closedir2 (DIR2 handle) {
	return -1;
}