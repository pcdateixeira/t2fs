#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	
	/*
	int ret0 = 22;
	int ret1 = 11;
	int ret2 = 22;
	int ret3 = 33;
	
	ret0 = create2("file5.txt");
	printf("Retorno da create2: %d\n", ret0);
	
	ret1 = create2("file6.txt");
	printf("Retorno da create2: %d\n", ret1);
	
	ret2 = delete2("file3.txt");
	printf("Retorno da delete2: %d\n", ret2);
	
	ret3 = delete2("file4.txt");
	printf("Retorno da delete2: %d\n", ret3);
	*/
	
	/*
	int ret0 = 35;
	
	ret0 = delete2("file3.txt");
	printf("Retorno da delete2: %d\n", ret0);
	*/
	
	/*
	int ret0 = 35;
	int ret1 = 36;
	int ret2 = 35;
	int ret3 = 40;
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = open2("file3.txt");
	printf("Retorno da open2: %d\n", ret1);
	
	ret2 = close2(ret1);
	printf("Retorno da close2: %d\n", ret2);
	
	ret3 = open2("file6.txt");
	printf("Retorno da open2: %d\n", ret3);
	*/
	
	/*
	int ret0 = 35;
	int ret1 = 36;
	int ret2 = 40;
	char buffer[2048];
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret1);
	
	ret2 = read2(ret1, buffer, 1026);
	printf("Retorno da read2: %d\n", ret2);
	*/
	
	/*
	int ret0 = 35;
	int ret1 = 36;
	int ret2 = 40;
	int ret3 = 40;
	int ret4 = 40;
	char buffer[2048];
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret1);
	
	ret2 = read2(ret1, buffer, 512);
	printf("Retorno da read2: %d\n", ret2);
	
	ret3 = read2(ret1, buffer, 256);
	printf("Retorno da read2: %d\n", ret3);
	
	ret4 = read2(ret1, buffer, 512);
	printf("Retorno da read2: %d\n", ret4);
	*/
	
	/*
	int ret0 = 35;
	int ret1 = 36;
	int ret2 = 40;
	int ret3 = 35;
	char buffer[2048];
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret1);
	
	ret2 = read2(ret0, buffer, 32);
	printf("Retorno da read2: %d\n", ret2);
	
	ret3 = read2(ret0, buffer, 64);
	printf("Retorno da read2: %d\n", ret3);
	*/
	
	/*
	int ret0 = 35;
	int ret1 = 36;
	char buffer[2048];
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret1);
	*/
	
	/*
	int ret0 = 42;
	int ret1 = 44;
	int ret2 = 11;
	char buffer[2048];
	
	ret0 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = seek2(ret0, 1024);
	printf("Retorno da seek2: %d\n", ret1);
	
	ret2 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret2);
	*/
	
	/*
	int ret0 = 42;
	int ret1 = 44;
	int ret2 = 11;
	char buffer[2048];
	
	ret0 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = seek2(ret0, -1);
	printf("Retorno da seek2: %d\n", ret1);
	
	ret2 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret2);
	*/
	
	/*
	int ret0 = 42;
	int ret1 = 44;
	int ret2 = 11;
	int ret3 = 22;
	int ret4 = 33;
	int ret5 = 99;
	int ret6 = 44;
	char buffer0[2048];
	char buffer1[24] = "HELLO WORLD";
	
	ret0 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = read2(ret0, buffer0, 9999);
	printf("Retorno da read2: %d\n", ret1);
	
	ret2 = seek2(ret0, 200);
	printf("Retorno da seek2: %d\n", ret2);
	
	ret3 = truncate2(ret0);
	printf("Retorno da truncate2: %d\n", ret3);
	
	ret4 = read2(ret0, buffer0, 9999);
	printf("Retorno da read2: %d\n", ret4);
	
	ret5 = seek2(ret0, 0);
	printf("Retorno da seek2: %d\n", ret5);
	
	ret6 = write2(ret0, buffer1, 11);
	printf("Retorno da write2: %d\n", ret6);
	*/
	
	/*
	int ret0 = 22;
	int ret1 = 22;
	int ret2 = 33;
	int ret3 = 22;
	int ret4 = 11;
	int ret5 = 44;
	int ret6 = 22;
	char buffer[2048];
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret1);
	
	ret2 = seek2(ret0, 30);
	printf("Retorno da seek2: %d\n", ret2);
	
	ret3 = truncate2(ret0);
	printf("Retorno da truncate2: %d\n", ret3);
	
	ret4 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret4);
	
	ret5 = seek2(ret0, 0);
	printf("Retorno da seek2: %d\n", ret5);
	
	ret6 = read2(ret0, buffer, 9999);
	printf("Retorno da read2: %d\n", ret6);
	*/
	
	/*
	int ret0 = 22;
	int ret1 = 11;
	int ret2 = 22;
	int ret3 = 44;
	int ret4 = 55;
	char buffer0[2048];
	char buffer1[2048] = "HELLO WORLD!";
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = read2(ret0, buffer0, 9999);
	printf("Retorno da read2: %d\n", ret1);
	
	ret2 = write2(ret0, buffer1, 11);
	printf("Retorno da write2: %d\n", ret2);
	
	ret3 = seek2(ret0, 0);
	printf("Retorno da seek2: %d\n", ret3);
	
	ret4 = write2(ret0, buffer1, 11);
	printf("Retorno da write2: %d\n", ret4);
	*/
	
	/*
	int ret0 = 22;
	int ret1 = 11;
	int ret2 = 22;
	int ret3 = 33;
	char buffer0[2048];
	char buffer1[2048] = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            ";
	
	ret0 = open2("file1.txt");
	printf("Retorno da open2: %d\n", ret0);
	
	ret1 = read2(ret0, buffer0, 9999);
	printf("Retorno da read2: %d\n", ret1);
	
	ret2 = write2(ret0, buffer1, 1100);
	printf("Retorno da write2: %d\n", ret2);
	
	ret3 = write2(ret0, buffer1, 1100);
	printf("Retorno da write2: %d\n", ret3);
	*/
	
	/*
	DIRENT2 *dentry = malloc(sizeof(DIRENT2));
	int i;
	int ret0 = 0;
	int ret1 = 11;
	int ret2 = 22;
	
	for (i = 0; ret0 == 0; i++) {
		ret0 = readdir2(0, dentry);
		printf("Retorno da readdir2: %d\n", ret0);
	}
	
	ret1 = closedir2(0);
	printf("Retorno da closedir2: %d\n", ret1);
	
	ret2 = readdir2(0, dentry);
	printf("Retorno da readdir2: %d\n", ret2);
	
	free(dentry);
	*/
	
	/*
	DIRENT2 *dentry = malloc(sizeof(DIRENT2));
	int i;
	int ret0 = 22;
	int ret1 = 33;
	int flag = 0;
	
	ret0 = opendir2("dir1");
	printf("Retorno da opendir2: %d\n", ret0);
	
	for (i = 0; flag == 0; i++) {
		flag = readdir2(ret0, dentry);
		printf("Retorno da readdir2: %d\n", flag);
	}
	
	ret1 = closedir2(ret0);
	printf("Retorno da closedir2: %d\n", ret1);
	
	flag = 0;
	for (i = 0; flag == 0; i++) {
		flag = readdir2(ret0, dentry);
		printf("Retorno da readdir2: %d\n", flag);
	}
	
	free(dentry);
	*/
	
	/*
	int ret0 = 22;
	int ret1 = 33;
	int ret2 = 34;
	int ret3 = 99;
	int ret4 = 22;
	
	ret0 = mkdir2("dir2");
	printf("Retorno da mkdir2: %d\n", ret0);
	
	ret1 = mkdir2("dir3");
	printf("Retorno da mkdir2: %d\n", ret1);
	
	ret2 = rmdir2("dir1");
	printf("Retorno da rmdir2: %d\n", ret2);
	
	ret3 = rmdir2("dir3");
	printf("Retorno da rmdir2: %d\n", ret3);
	
	ret4 = rmdir2("dir4");
	printf("Retorno da rmdir2: %d\n", ret4);
	*/
	
	/*
	DIRENT2 *dentry = malloc(sizeof(DIRENT2));
	int i;
	int ret0 = 22;
	int ret1 = 0;
	
	ret0 = create2("file2.txt");
	printf("Retorno da create2: %d\n", ret0);
	
	for (i = 0; ret1 == 0; i++) {
		ret1 = readdir2(0, dentry);
		printf("Retorno da readdir2: %d\n", ret1);
	}
	*/
	
	/*
	int ret0 = 22;
	int ret1 = 33;
	int ret2 = 34;
	int ret3 = 99;
	
	ret0 = rmdir2("dir1");
	printf("Retorno da rmdir2: %d\n", ret0);
	
	ret1 = rmdir2("dir3");
	printf("Retorno da rmdir2: %d\n", ret1);
	
	ret2 = mkdir2("dir1");
	printf("Retorno da mkdir2: %d\n", ret2);
	
	ret3 = mkdir2("dir3");
	printf("Retorno da mkdir2: %d\n", ret3);
	*/
	
	/*
	int ret0 = 11;
	char buffer[2048];
	
	ret0 = getcwd2 (buffer, 2048);
	printf("Retorno da getcwd2: %d\n", ret0);
	*/
	
	/*
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 22;
	int ret3 = 44;
	int ret4 = 55;
	int ret5 = 22;
	int ret6 = 11;
	char buffer[2048];
	char pathname[] = "dir1";
	
	ret0 = getcwd2 (buffer, 2048);
	printf("Retorno da getcwd2: %d\n", ret0);
	
	ret1 = chdir2(pathname);
	printf("Retorno da chdir2: %d\n", ret1);
	
	ret2 = create2("file3.txt");
	printf("Retorno da create2: %d\n", ret2);
	
	ret3 = create2("file4.txt");
	printf("Retorno da create2: %d\n", ret3);
	
	ret4 = delete2("file3.txt");
	printf("Retorno da delete2: %d\n", ret4);
	*/
	
	/*
	//DIRENT2 *dentry = malloc(sizeof(DIRENT2));
	//int i;
	int ret0 = 11;
	//int ret1 = 22;
	int ret2 = 44;
	int ret3 = 22;
	int ret4 = 55;
	int ret5 = 34;
	int ret6 = 44;
	int ret7 = 24;
	int ret8 = 22;
	int ret9 = 89;
	int ret10 = 11;
	int ret11 = 11;
	char buffer[2048];
	//char pathname0[] = "/dir1";
	//char pathname1[] = "/dir1/dir5";
	char write[] = "HELLO WORLD! HELLO WORLD! HELLO WORLD! HELLO WORLD! HELLO WORLD! HELLO WORLD!";
	
	ret0 = getcwd2(buffer, 2048);
	printf("Retorno da getcwd2: %d\n", ret0);
	
	//ret1 = chdir2(pathname0);
	//printf("Retorno da chdir2: %d\n", ret1);
	
	ret2 = create2("file3.txt");
	printf("Retorno da create2: %d\n", ret2);
	
	ret4 = delete2("file1.txt");
	printf("Retorno da delete2: %d\n", ret4);
	
	ret5 = mkdir2("dir5");
	printf("Retorno da mkdir2: %d\n", ret5);
	
	ret6 = mkdir2("dir6");
	printf("Retorno da mkdir2: %d\n", ret6);
	
	ret10 = open2("file2.txt");
	printf("Retorno da open2: %d\n", ret10);
	
	ret11 = truncate2(ret10);
	printf("Retorno da truncate2: %d\n", ret11);
	
	ret7 = rmdir2("dir5");
	printf("Retorno da rmdir2: %d\n", ret7);
	
	ret3 = create2("file4.txt");
	printf("Retorno da create2: %d\n", ret3);
	
	ret8 = write2(ret2, write, 77);
	printf("Retorno da write2: %d\n", ret8);
	
	ret9 = close2(ret2);
	printf("Retorno da close2: %d\n", ret9);
	*/
	
	/*
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 44;
	int ret3 = 22;
	int ret4 = 55;
	int ret5 = 34;
	int ret6 = 44;
	int ret7 = 24;
	char str0[] = "file10.txt";
	char str1[] = "/file11.txt";
	char str2[] = "./file12.txt";
	char str3[] = "../file13.txt";
	char str4[] = "dir1/file14.txt";
	char str5[] = "/dir1/file15.txt";
	char str6[] = "./dir1/file16.txt";
	char str7[] = "../dir1/file17.txt";
	
	ret0 = create2(str0);
	printf("Retorno da create2: %d\n", ret0);
	
	ret1 = create2(str1);
	printf("Retorno da create2: %d\n", ret1);
	
	ret2 = create2(str2);
	printf("Retorno da create2: %d\n", ret2);
	
	ret3 = create2(str3);
	printf("Retorno da create2: %d\n", ret3);
	
	ret4 = create2(str4);
	printf("Retorno da create2: %d\n", ret4);
	
	ret5 = create2(str5);
	printf("Retorno da create2: %d\n", ret5);
	
	ret6 = create2(str6);
	printf("Retorno da create2: %d\n", ret6);
	
	ret7 = create2(str7);
	printf("Retorno da create2: %d\n", ret7);
	*/
	
	/*
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 44;
	int ret3 = 22;
	int ret4 = 55;
	int ret5 = 44;
	int ret6 = 4;
	int ret7 = 22;
	char str0[] = "file10.txt";
	char str1[] = "../file10.txt";
	char str2[] = "dir1/file11.txt";
	char str3[] = "../dir1/file11.txt";
	char str4[] = "../dir2/file12.txt";
	char str5[] = "file13.txt";
	char str6[] = "../file14.txt";
	char path0[] = "/dir1";
	
	ret0 = create2(str0);
	printf("Retorno da create2: %d\n", ret0);
	
	ret1 = create2(str1);
	printf("Retorno da create2: %d\n", ret1);
	
	ret2 = create2(str2);
	printf("Retorno da create2: %d\n", ret2);
	
	ret3 = create2(str3);
	printf("Retorno da create2: %d\n", ret3);
	
	ret4 = create2(str4);
	printf("Retorno da create2: %d\n", ret4);
	
	ret5 = chdir2(path0);
	printf("Retorno da chdir2: %d\n", ret5);
	
	ret6 = create2(str5);
	printf("Retorno da create2: %d\n", ret6);
	
	ret7 = create2(str6);
	printf("Retorno da create2: %d\n", ret7);
	*/
	
	/*
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 44;
	//int ret3 = 22;
	int ret4 = 55;
	int ret5 = 44;
	int ret6 = 4;
	int ret7 = 22;
	int ret8 = 44;
	int ret9 = 33;
	int ret10 = 11;
	int ret11 = 44;
	int ret12 = 22;
	char str0[] = "dir2";
	char str1[] = "file10.txt";
	char str2[] = "file10.txt";
	char str3[] = "HELLO WORLD!";
	char buffer[2048];
	char path0[] = "dir2";
	char path1[] = "dir3";
	
	ret0 = mkdir2(str0);
	printf("Retorno da mkdir2: %d\n", ret0);
	
	ret1 = chdir2(path0);
	printf("Retorno da chdir2: %d\n", ret1);
	
	ret2 = create2(str1);
	printf("Retorno da create2: %d\n", ret2);
	
	//ret3 = open2(str2);
	//printf("Retorno da open2: %d\n", ret3);
	
	ret4 = write2(ret2, str3, 12);
	printf("Retorno da write2: %d\n", ret4);
	
	ret5 = seek2(ret2, 0);
	printf("Retorno da seek2: %d\n", ret5);
	
	ret6 = read2(ret2, buffer, 12);
	printf("Retorno da read2: %d, %s\n", ret6, buffer);
	
	ret7 = truncate2(ret2);
	printf("Retorno da truncate2: %d\n", ret7);
	
	ret8 = close2(ret2);
	printf("Retorno da close2: %d\n", ret8);
	
	ret9 = getcwd2(buffer, 1024);
	printf("Retorno da getcwd2: %d\n", ret9);
	
	ret10 = delete2(str2);
	printf("Retorno da delete2: %d\n", ret10);
	
	ret12 = mkdir2(path1);
	printf("Retorno da mkdir2: %d\n", ret12);
	
	ret11 = rmdir2(path1);
	printf("Retorno da rmdir2: %d\n", ret11);
	*/
	
	/*
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 44;
	int ret3 = 22;
	int ret4 = 55;
	int ret5 = 44;
	int ret6 = 4;
	int ret7 = 22;
	int ret8 = 44;
	int ret9 = 33;
	int ret10 = 11;
	int ret11 = 44;
	int ret12 = 22;
	int ret13 = 223;
	char str0[] = "dir2";
	char str1[] = "/file10.txt";
	char str2[] = "/file2.txt";
	char str3[] = "HELLO WORLD!";
	char buffer[2048];
	char path0[] = "dir2";
	char path1[] = "dir3";
	
	ret0 = mkdir2(str0);
	printf("Retorno da mkdir2: %d\n", ret0);
	
	ret1 = chdir2(path0);
	printf("Retorno da chdir2: %d\n", ret1);
	
	ret2 = create2(str1);
	printf("Retorno da create2: %d\n", ret2);
	
	ret3 = open2(str2);
	printf("Retorno da open2: %d\n", ret3);
	
	ret4 = write2(ret2, str3, 12);
	printf("Retorno da write2: %d\n", ret4);
	
	ret5 = seek2(ret2, 0);
	printf("Retorno da seek2: %d\n", ret5);
	
	ret6 = read2(ret3, buffer, 1024);
	printf("Retorno da read2: %d, %s\n", ret6, buffer);
	
	ret7 = truncate2(ret2);
	printf("Retorno da truncate2: %d\n", ret7);
	
	ret8 = close2(ret2);
	printf("Retorno da close2: %d\n", ret8);
	
	ret9 = getcwd2(buffer, 1024);
	printf("Retorno da getcwd2: %d\n", ret9);
	
	ret13 = close2(ret3);
	printf("Retorno da close2: %d\n", ret13);
	
	ret10 = delete2(str2);
	printf("Retorno da delete2: %d\n", ret10);
	
	ret12 = mkdir2(path1);
	printf("Retorno da mkdir2: %d\n", ret12);
	
	ret11 = rmdir2(path1);
	printf("Retorno da rmdir2: %d\n", ret11);
	*/
	
	int ret0 = 11;
	int ret1 = 22;
	int ret2 = 44;
	int ret3 = 22;
	int ret4 = 55;
	int ret5 = 44;
	int ret6 = 4;
	int ret7 = 22;
	int ret8 = 44;
	int ret9 = 33;
	int ret10 = 11;
	int ret11 = 44;
	int ret12 = 22;
	int ret13 = 223;
	int ret14 = 11;
	char str0[] = "./dir666";
	char str1[] = "./file10.txt";
	char str2[] = "../../file1.txt";
	char str3[] = "HELLO WORLD!";
	char buffer[2048];
	char path0[] = "/dir1";
	char path1[] = "dir3";
	char path2[] = "/dir1/dir666";
	
	ret1 = chdir2(path0);
	printf("Retorno da chdir2: %d\n", ret1);
	
	ret0 = mkdir2(str0);
	printf("Retorno da mkdir2: %d\n", ret0);
	
	ret14 = chdir2(path2);
	printf("Retorno da chdir2: %d\n", ret14);
	
	ret2 = create2(str1);
	printf("Retorno da create2: %d\n", ret2);
	
	ret3 = open2(str2);
	printf("Retorno da open2: %d\n", ret3);
	
	ret4 = write2(ret2, str3, 12);
	printf("Retorno da write2: %d\n", ret4);
	
	ret5 = seek2(ret2, 0);
	printf("Retorno da seek2: %d\n", ret5);
	
	ret6 = read2(ret3, buffer, 1024);
	printf("Retorno da read2: %d, %s\n", ret6, buffer);
	
	ret7 = truncate2(ret2);
	printf("Retorno da truncate2: %d\n", ret7);
	
	ret8 = close2(ret2);
	printf("Retorno da close2: %d\n", ret8);
	
	ret9 = getcwd2(buffer, 1024);
	printf("Retorno da getcwd2: %d\n", ret9);
	
	ret13 = close2(ret3);
	printf("Retorno da close2: %d\n", ret13);
	
	ret10 = delete2(str2);
	printf("Retorno da delete2: %d\n", ret10);
	
	ret12 = mkdir2(path1);
	printf("Retorno da mkdir2: %d\n", ret12);
	
	ret11 = rmdir2(path1);
	printf("Retorno da rmdir2: %d\n", ret11);
	
    return 0;
}
