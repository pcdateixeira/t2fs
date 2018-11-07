#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DWORD cluster_size;
DWORD nr_clusters;
DWORD sectors_per_cluster;
DWORD nr_directory_entries;
DWORD nr_FAT_entries;
DWORD first_FAT_sector;
DWORD root_dir_cluster;

int max_opened_files = 10;
int opened_files = -1;

struct t2fs_superbloco superblock;

DWORD todword(DWORD pos, BYTE *buffer) {
    DWORD result =  (DWORD) buffer[pos + 0] << 24 |
                    (DWORD) buffer[pos + 1] << 16 |
                    (DWORD) buffer[pos + 2] << 8  |
                    (DWORD) buffer[pos + 3] << 0;

    return result;
}

WORD toword(DWORD pos, BYTE *buffer) {
    WORD result =   (WORD) buffer[pos + 0] << 8  |
                    (WORD) buffer[pos + 1] << 0;

    return result;
}

void readSuperblock() {
    BYTE buffer[SECTOR_SIZE];

    memset(buffer, 0, SECTOR_SIZE);
    read_sector(0, &buffer[0]);

    strncpy(superblock.id, &buffer[0], 4);
    superblock.version = toword(4, buffer);
    superblock.superblockSize = toword(6, buffer);
    superblock.DiskSize = todword(8, buffer);
    superblock.NofSectors = todword(12, buffer);
    superblock.SectorsPerCluster = todword(16, buffer);
    superblock.pFATSectorStart = todword(20, buffer);
    superblock.RootDirCluster = todword(24, buffer);
    superblock.DataSectorStart = todword(28, buffer);

    cluster_size = superblock.SectorsPerCluster;
    nr_clusters = superblock.DiskSize / cluster_size;
    sectors_per_cluster = cluster_size / SECTOR_SIZE;
    nr_directory_entries = cluster_size / 64;
    nr_FAT_entries = nr_clusters / SECTOR_SIZE;
    first_FAT_sector = superblock.pFATSectorStart;
    root_dir_cluster = superblock.RootDirCluster;

    //nr_clusters_data = (superblock.NofSectors - superblock.DataSectorStart) / cluster_size;
}

void read_cluster(DWORD sector, BYTE *buffer) {
    int i;
    int nr_reads = sectors_per_cluster;

    for (i = 0; i <= nr_reads - 1; i++) {
        read_sector(sector + (i * SECTOR_SIZE), &buffer[(i * SECTOR_SIZE)]);
    }
}

void write_cluster(DWORD sector, BYTE *buffer) {
    int i;
    int nr_writes = sectors_per_cluster;

    for (i = 0; i <= nr_writes - 1; i++) {
        write_sector(sector + (i * SECTOR_SIZE), &buffer[(i * SECTOR_SIZE)]);
    }
}

struct DirectoryTable {
    struct t2fs_record table[nr_directory_entries];
};

struct DirectoryTable root;

struct t2fs_record torecord(BYTE *buffer) {
    struct t2fs_record entry_read;

    entry_read.TypeVal = (BYTE) buffer[0];
    strncpy(entry_read.name, &buffer[1], 51);
    entry_read.bytesFileSize = todword(52, buffer);
    entry_read.clustersFileSize = todword(56, buffer);
    entry_read.firstCluster = todword(60, buffer);

    return entry_read;
}

void readDirectoryRecords(DWORD firstCluster, DirectoryTable *dir) {
    int i;
    int nr_entries = nr_directory_entries;

    BYTE buffer[cluster_size];

    memset(buffer, 0, cluster_size);
    read_cluster(firstCluster * cluster_size, buffer);

    for (i = 0; i <= nr_entries - 1; i++) {
        dir->table[i] = torecord(&buffer[i * 64]);
    }
}

void writeDirectoryRecords(DWORD firstCluster, DirectoryTable *dir) {
    int i;
    int nr_entries = nr_directory_entries;

    BYTE buffer[cluster_size];

    // Fazer o contrário ainda
}

int getFileIndexInDirectory(char *filename, DirectoryTable *dir) {
    int i;
    int nr_entries = nr_directory_entries;

    for (i = 0; i <= nr_entries - 1; i++) {
        if (strcmp(dir->table[i].name, filename) == 0) {
            return i;
        }
    }

    return -1;
}

struct FileAllocationTable {
    DWORD table[nr_FAT_entries];
};

struct FileAllocationTable fat;

void read_FAT(BYTE *buffer) {
    int i;
    int nr_reads = nr_FAT_entries;

    for (i = 0; i <= nr_reads - 1; i++) {
        read_sector(first_FAT_sector + (i * SECTOR_SIZE), &buffer[(i * SECTOR_SIZE)]);
    }
}

void write_FAT(BYTE *buffer) {
    int i;
    int nr_reads = nr_FAT_entries;

    for (i = 0; i <= nr_reads - 1; i++) {
        write_sector(first_FAT_sector + (i * SECTOR_SIZE), &buffer[(i * SECTOR_SIZE)]);
    }
}

void readFATEntries(FileAllocationTable *fat) {
    int i;
    int nr_entries = nr_directory_entries;

    BYTE buffer[nr_clusters];

    memset(buffer, 0, nr_clusters);
    read_FAT(first_FAT_sector, buffer);

    for (i = 0; i <= nr_entries - 1; i++) {
        fat->table[i] = todword(i * 4, buffer);
    }
}

void writeFATEntries(FileAllocationTable *fat) {
    int i;
    int nr_entries = nr_directory_entries;

    BYTE buffer[nr_clusters];

    // Fazer o contrário ainda
}

int getFirstFreeCluster(FileAllocationTable *fat) {
    int i;

    for (i = 2; i <= nr_FAT_entries - 1; i++) {
        if (fat->table[i] == 0x00000000) {
            return i;
		}
    }

    return -1;
}

int getFirstFreeDirectoryEntry(DirectoryTable *dir) {
    int i;
    int nr_entries = nr_directory_entries;

    for (i = 0; i <= nr_entries - 1; i++) {
        if (dir->table[i].TypeVal == 0x00) {
            return i;
        }
    }

    return -1;
}

void createFileEntry(char *filename, struct t2fs_record *record) {
    record->TypeVal = TYPEVAL_REGULAR;
    strncpy(record->name, filename, 51);
    record->bytesFileSize = 0x00000000;
    record->clustersFileSize = 0x00000001;
}

struct FileStatus {
    int currentPtr;
    int referenceCount;
    struct t2fs_record *recordPtr;
};

struct OpenFileTable {
    struct FileStatus table[max_opened_files];
};

struct OpenFileTable oft;

/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */

FILE2 create2 (char *filename) {
    struct t2fs_record new_file_entry;
    int handle, first_free_cluster, first_free_dir_entry = -1;

    // Se arquivo já existe, o mesmo terá seu conteúdo removido e assumirá um tamanho de zero bytes
    // É a função truncate2(handle) precedida por um seek2(handle, 0)
    // Arrumar a função abaixo para procurar em todos diretórios e também fazer o que diz acima
    if (getFileIndexInDirectory(filename, &root) != -1) {
        return -1;
    }

    readFATEntries(&fat);
    first_free_cluster = getFirstFreeCluster(&fat);

    if (first_free_cluster >= 2) {
        handle = opened_files++;

        // Generalizar a função abaixo para ler do diretório atual de trabalho (?)
        readDirectoryRecords(root_dir_cluster, &root);
        first_free_dir_entry = getFirstFreeDirectoryEntry(&root);

        createFileEntry(filename, &new_file_entry);
        new_file_entry.firstCluster = first_free_cluster;
        root.table[first_free_dir_entry] = new_file_entry;

        oft.table[handle].currentPtr = 0;
        oft.table[handle].referenceCount = 1;
        oft.table[handle].recordPtr = &root.table[first_free_dir_entry];
        fat.table[first_free_cluster] = 0xFFFFFFFF;

        writeFATEntries(&fat);
        writeDirectoryRecords(root_dir_cluster, &root);
    }
    else {
        return -1;
    }

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

