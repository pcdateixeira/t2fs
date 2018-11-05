#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NR_CLUSTERS 8192
#define NR_CLUSTERS_DATA 8176
#define CLUSTER_SIZE 512

/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */

struct t2fs_superbloco superblock = {
	"T2FS",
	0x7E22,
	0x0001,     // Super Block Size         = 1
	0x00400000, // Disk Size                = 4 194 304 bytes
	0x00004000, // Nr. of Sectors           = 16 384
	0x00000002, // Sectors per Cluster      = 2
	0x00000001, // First FAT Sector         = 1
	0x00000012, // Root Cluster             = 18    (começa em 0)
	0x00001FF0  // First Data Sector        = 8176  (começa em 0)
};

struct FileAllocationTable {
    DWORD table[NR_CLUSTERS_DATA];
    /* - Lista com C elementos de 4 bytes
       - A quantidade C de elementos válidos nessa lista é igual à quantidade total de clusters da área de dados */
};

struct DirectoryTable {
    struct t2fs_record table[CLUSTER_SIZE];
    /* - Os diretórios organizam as entradas como uma lista linear de registros de tamanho da estrutura t2fs_record
       - Todo diretório ocupa exatamente um cluster */
};

struct OpenFileTable {
    /* Usado na seek, write, etc...
        Para definir com atributos como current pointer, se está aberto para leitura ou escrita, etc... */
};

/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */

int isFormatted = 0; /* Disco será dado formatado ou deve ser criado ou estará "novo" e deve ser formatado ou ... (?)
                    Fazer todos casos se necessário e modificar as funções */
int openedFiles = 0; // Será usado como identificador de arquivos (handles)

FileAllocationTable fat;
DirectoryTable root;

void init_fat(FileAllocationTable *fat) {
    int i;

    for (i = 2; i < NR_CLUSTERS_DATA - 1; i++) {
		fat->table[i] = 0x00000000;
	}
}

void init_root(DirectoryTable *root, FileAllocationTable *fat) {
    int i;

    struct t2fs_record current_dir = {
        TYPEVAL_DIRETORIO,
        "root",
        0x00000200, // File Size Bytes          = 512   (é um diretório)
        0x00000001, // File Size Clusters       = 1
        0x00000012  // First Cluster            = 16 para FAT + 2 reservados    (começa em 0)
    };

    struct t2fs_record parent_dir = {
        TYPEVAL_DIRETORIO,
        "root",
        0x00000200, // File Size Bytes          = 512   (é um diretório)
        0x00000001, // File Size Clusters       = 1
        0x00000012  // First Cluster            = 16 para FAT + 2 reservados    (começa em 0)
    };

    for (i = 0; i < CLUSTER_SIZE - 1; i++) {
        root->table[i] = (struct t2fs_record){0};
    }

    root->table[0] = current_dir;
    root->table[1] = parent_dir;
    fat->table[2] = 0xFFFFFFFF;
}

void format() {
    init_fat(&fat);
    init_root(&root, &fat);

    // Criar funções de escrita e leitura mais específicas para as 3 (?) partes mais gerais do disco
    write_sector(0, &superblock);
    write_sector(1, &fat);
    write_sector(8193, &root);
}

int getFirstFreeCluster(FileAllocationTable *fat) {
    int i;

    for (i = 2; i < NR_CLUSTERS_DATA - 1; i++) {
		if (fat->table[i] == 0x00000000) {
            return i;
		}
	}

	return -1;
}

int getFirstFreeDirectoryEntry(DirectoryTable *dir) {
    int i;

    for (i = 0; i < CLUSTER_SIZE - 1; i++) {
        if (dir->table[i] == (struct t2fs_record){0}) {
            return i;
        }
    }

    return -1;
}

void createFileEntry(char *filename, struct t2fs_record *record) {
    record->TypeVal = TYPEVAL_REGULAR;
    strcpy(record->name, filename);
    record->bytesFileSize = 0x00000000;
    record->clustersFileSize = 0x00000001;
}


int getFileIndexInRootDirectory(char *filename, DirectoryTable *dir) {
    int i = 0;
    int index = -1;

    while (i < CLUSTER_SIZE - 1) {
        if (!strcmp(dir->table[i].name, filename)) {
            index = i;
        }

        i++;
    }

    return index;
}

/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */

FILE2 create2 (char *filename) {

    // O contador de posição do arquivo (current pointer) deve ser colocado na posição zero
    // Adicionar ao implementar a tabela de arquivos abertos

    struct t2fs_record new_file_entry;
    int handle, first_free_cluster, first_free_dir_entry = -1;

    // Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação
    // Arrumar a função para procurar em todos diretórios
    if (getFileIndexInRootDirectory(filename, &root) != -1) {
        return -1;
    }

    handle = openedFiles++;

    first_free_cluster = getFirstFreeCluster(&fat);
    first_free_dir_entry = getFirstFreeDirectoryEntry(&root);

    if (first_free_cluster >= 2) {
        createFileEntry(filename, &new_file_entry);
        // Converter para little endian
        new_file_entry.firstCluster = first_free_cluster;

        root.table[first_free_dir_entry] = new_file_entry;
        fat.table[first_free_cluster] = 0xFFFFFFFF;

        // Criar funções de escrita e leitura mais específicas para as 3 (?) partes mais gerais do disco
        write_sector(1, &fat);
        write_sector(8193, &root);
    }
    else {
        return -1;
    }

    // A função deve retornar o identificador (handle) do arquivo
    // Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado
	return handle;
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

int identify2 (char *name, int size) {
	strncpy (name, "Humberto Lentz - 242308\nMakoto Ishikawa - 216728\nPedro Teixeira - 228509\0", size);
	return 0;
}

