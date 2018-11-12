#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DWORD cluster_size;
DWORD nr_clusters;
DWORD sectors_per_cluster;
DWORD nr_directory_entries;
DWORD nr_FAT_sectors;
DWORD first_FAT_sector;
DWORD root_dir_cluster;
DWORD first_data_sector;

int max_opened_files = 10;
int max_opened_directories = 10;
int opened_files = -1;
int opened_directories = -1;
int max_path_length = 4096;
int cwd_first_cluster = -1;
int first_time = 1;

char cwd[4096];

struct FileStatus {
    int offset;
    int directoryCluster;
    struct t2fs_record info;
};

struct OpenFileTable {
    struct FileStatus table[10];
};

struct OpenFileTable oft;
struct OpenFileTable odt;
struct t2fs_superbloco superblock;

DWORD swapendiandword(DWORD value) {
    DWORD b0, b1, b2, b3;

    b0 = (value & 0x000000ff) << 24u;
    b1 = (value & 0x0000ff00) << 8u;
    b2 = (value & 0x00ff0000) >> 8u;
    b3 = (value & 0xff000000) >> 24u;

    return (b0 | b1 | b2 | b3);
}

WORD swapendianword(WORD value) {
    WORD b0, b1;

    b0 = (value & 0x00ff) << 8u;
    b1 = (value & 0xff00) >> 8u;

    return (b0 | b1);
}

DWORD todword(DWORD pos, BYTE *buffer) {
    DWORD result =  (DWORD) buffer[pos + 0] << 24 |
                    (DWORD) buffer[pos + 1] << 16 |
                    (DWORD) buffer[pos + 2] << 8  |
                    (DWORD) buffer[pos + 3] << 0;

    return result;
}

void fromdword(DWORD value, DWORD pos, BYTE *buffer) {
    buffer[pos + 0] = (BYTE) ((value >> 24) & 0xFFu);
    buffer[pos + 1] = (BYTE) ((value >> 16) & 0xFFu);
    buffer[pos + 2] = (BYTE) ((value >>  8) & 0xFFu);
    buffer[pos + 3] = (BYTE) ((value >>  0) & 0xFFu);
}

WORD toword(DWORD pos, BYTE *buffer) {
    WORD result =   (WORD) buffer[pos + 0] << 8  |
                    (WORD) buffer[pos + 1] << 0;

    return result;
}

void fromword(WORD value, DWORD pos, BYTE *buffer) {
    buffer[pos + 0] = (BYTE) ((value >>  8) & 0xFFu);
    buffer[pos + 1] = (BYTE) ((value >>  0) & 0xFFu);
}

void readSuperblock() {
    struct t2fs_record new_record = {0};
    BYTE buffer[SECTOR_SIZE];
    BYTE filename[2] = "/";

    memset(buffer, 0, sizeof(buffer));
    read_sector(0, buffer);

    strncpy(superblock.id, buffer, 4);
    superblock.version = swapendianword(toword(4, buffer));
    superblock.superblockSize = swapendianword(toword(6, buffer));
    superblock.DiskSize = swapendiandword(todword(8, buffer));
    superblock.NofSectors = swapendiandword(todword(12, buffer));
    superblock.SectorsPerCluster = swapendiandword(todword(16, buffer));
    superblock.pFATSectorStart = swapendiandword(todword(20, buffer));
    superblock.RootDirCluster = swapendiandword(todword(24, buffer));
    superblock.DataSectorStart = swapendiandword(todword(28, buffer));
    /*
    printf("Versão: %x %d\n", superblock.version, superblock.version);
    printf("Tam. Superbloco: %x %d\n", superblock.superblockSize, superblock.superblockSize);
    printf("Tam. Disco: %x %d\n", superblock.DiskSize, superblock.DiskSize);
    printf("Nr. of Sectors: %x %d\n", superblock.NofSectors, superblock.NofSectors);
    printf("Sectors per Cluster: %x %d\n", superblock.SectorsPerCluster, superblock.SectorsPerCluster);
    printf("First FAT Sector: %x %d\n", superblock.pFATSectorStart, superblock.pFATSectorStart);
    printf("Root Directory Cluster: %x %d\n", superblock.RootDirCluster, superblock.RootDirCluster);
    printf("First Data Sector: %x %d\n\n", superblock.DataSectorStart, superblock.DataSectorStart);
    */
    cluster_size = superblock.SectorsPerCluster * SECTOR_SIZE;
    nr_clusters = (superblock.NofSectors - superblock.DataSectorStart) / superblock.SectorsPerCluster;
    sectors_per_cluster = superblock.SectorsPerCluster;
    nr_directory_entries = cluster_size / sizeof(struct t2fs_record);
    nr_FAT_sectors = nr_clusters / SECTOR_SIZE;
    first_FAT_sector = superblock.pFATSectorStart;
    root_dir_cluster = superblock.RootDirCluster;
    first_data_sector = superblock.DataSectorStart;
    /*
    printf("Tam. Cluster: %x %d\n", cluster_size, cluster_size);
    printf("Nr. of Clusters: %x %d\n", nr_clusters, nr_clusters);
    printf("Sectors per Cluster: %x %d\n", sectors_per_cluster, sectors_per_cluster);
    printf("Nr. of Directory Entries: %x %d\n", nr_directory_entries, nr_directory_entries);
    printf("Nr. of FAT Sectors: %x %d\n", nr_FAT_sectors, nr_FAT_sectors);
    printf("First FAT Sector: %x %d\n", first_FAT_sector, first_FAT_sector);
    printf("Root Directory Cluster: %x %d\n", root_dir_cluster, root_dir_cluster);
    printf("First Data Sector: %x %d\n\n", first_data_sector, first_data_sector);
    */
    new_record.TypeVal = TYPEVAL_DIRETORIO;
    strncpy(new_record.name, filename, 2);
    new_record.bytesFileSize = cluster_size;
    new_record.clustersFileSize = 0x00000001;
    new_record.firstCluster = root_dir_cluster;

    odt.table[0].offset = 0;
    odt.table[0].info = new_record;

    opened_directories++;
    cwd_first_cluster = root_dir_cluster;

    strncpy(cwd, "/", 2);

    first_time = -1;
}

void read_cluster(DWORD sector, BYTE *buffer) {
    int i;
    int nr_reads = sectors_per_cluster;

    for (i = 0; i < nr_reads; i++) {
        read_sector(sector + i, &buffer[(i * SECTOR_SIZE)]);
    }
}

void write_cluster(DWORD sector, BYTE *buffer) {
    int i;
    int nr_writes = sectors_per_cluster;

    for (i = 0; i < nr_writes; i++) {
        write_sector(sector + i, &buffer[(i * SECTOR_SIZE)]);
    }
}

struct t2fs_record torecord(BYTE *buffer) {
    struct t2fs_record entry_read;

    entry_read.TypeVal = (BYTE) buffer[0];
    strncpy(entry_read.name, &buffer[1], 52);
    entry_read.bytesFileSize = swapendiandword(todword(52, buffer));
    entry_read.clustersFileSize = swapendiandword(todword(56, buffer));
    entry_read.firstCluster = swapendiandword(todword(60, buffer));

    return entry_read;
}

void fromrecord(struct t2fs_record record, DWORD pos, BYTE *buffer) {
    buffer[pos + 0] = (BYTE) record.TypeVal;
    strncpy(&buffer[pos + 1], record.name, 51);
    fromdword(swapendiandword(record.bytesFileSize), pos + 52, buffer);
    fromdword(swapendiandword(record.clustersFileSize), pos + 56, buffer);
    fromdword(swapendiandword(record.firstCluster), pos + 60, buffer);
}

void readDirectoryRecords(DWORD firstCluster, struct t2fs_record *dir) {
    int i;
    int nr_entries = nr_directory_entries;
    DWORD initial_sector = first_data_sector + (firstCluster * sectors_per_cluster);

    BYTE buffer[cluster_size]; // Todo diretório ocupa um cluster
    memset(buffer, 0, sizeof(buffer));

    read_cluster(initial_sector, buffer);
    /*
    for (i = 0; i < cluster_size; i++) {
        printf("%c ", buffer[i]);
    }
    */
    for (i = 0; i < nr_entries; i++) {
        dir[i] = torecord(&buffer[i * sizeof(struct t2fs_record)]);
        //printf("TypeVal: %x, name: %s, bytesFileSize: %x, clustersFileSize: %x, firstCluster: %x\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
}

void writeDirectoryRecords(DWORD firstCluster, struct t2fs_record *dir) {
    int i;
    int nr_entries = nr_directory_entries;
    DWORD initial_sector = first_data_sector + (firstCluster * sectors_per_cluster);

    BYTE buffer[cluster_size]; // Todo diretório ocupa um cluster
    memset(buffer, 0, sizeof(buffer));

    for (i = 0; i < nr_entries; i++) {
        fromrecord(dir[i], i * sizeof(struct t2fs_record), buffer);
    }

    write_cluster(initial_sector, buffer);
}

int getFileIndexInDirectory(char *filename, struct t2fs_record *dir) {
    int i = 0;
    int nr_entries = nr_directory_entries;

    for (i = 0; i < nr_entries; i++) {
        if (strcmp(dir[i].name, filename) == 0) {
            return i;
        }
    }

    return -1;
}

void read_FAT(BYTE *buffer) {
    int i;
    int nr_reads = nr_FAT_sectors;

    for (i = 0; i < nr_reads; i++) {
        read_sector(first_FAT_sector + i, &buffer[(i * SECTOR_SIZE)]);
    }
}

void write_FAT(BYTE *buffer) {
    int i;
    int nr_writes = nr_FAT_sectors;

    for (i = 0; i < nr_writes; i++) {
        write_sector(first_FAT_sector + i, &buffer[(i * SECTOR_SIZE)]);
    }
}

void readFATEntries(DWORD *fat) {
    int i;
    int nr_entries = nr_clusters;

    BYTE buffer[nr_clusters * sizeof(DWORD)]; // C entradas (para cada cluster) de 4 bytes
    memset(buffer, 0, sizeof(buffer));

    read_FAT(buffer);
    /*
    for (i = 0; i < nr_clusters * sizeof(DWORD); i++) {
        printf("%c ", buffer[i]);
    }
    */
    for (i = 0; i < nr_entries; i++) {
        fat[i] = swapendiandword(todword(i * sizeof(DWORD), buffer));
        //printf("%x ", fat[i]);
    }
}

void writeFATEntries(DWORD *fat) {
    int i;
    int nr_entries = nr_clusters;

    BYTE buffer[nr_clusters * sizeof(DWORD)]; // C entradas (para cada cluster) de 4 bytes
    memset(buffer, 0, sizeof(buffer));

    for (i = 0; i < nr_entries; i++) {
        fromdword(swapendiandword(fat[i]), i * sizeof(DWORD), buffer);
    }

    write_FAT(buffer);
}

int getFirstFreeCluster(DWORD *fat) {
    int i;
    int nr_entries = nr_clusters;

    for (i = 2; i < nr_entries; i++) {
        if (fat[i] == 0x00000000) {
            //printf("First Free Cluster: %d\n", i);
            return i;
		}
    }

    return -1;
}

int getFirstFreeDirectoryEntry(struct t2fs_record *dir) {
    int i;
    int nr_entries = nr_directory_entries;

    for (i = 0; i < nr_entries; i++) {
        if (dir[i].TypeVal == TYPEVAL_INVALIDO) {
            //printf("First Free Directory Entry: %d\n", i);
            return i;
        }
    }

    return -1;
}

int getFileIndexInOpenFileTable(char *filename) {
    int i;

    for (i = 0; i < max_opened_files; i++) {
        if (strcmp(oft.table[i].info.name, filename) == 0) {
            return i;
        }
        //printf("Index in Open File Table: %d\n", i);
    }

    return -1;
}

int getFirstFreeOpenFileTableEntry() {
    int i;

    for (i = 0; i < max_opened_files; i++) {
        if (oft.table[i].info.TypeVal == TYPEVAL_INVALIDO) {
            //printf("First Free Index in Open File Table: %d\n", i);
            return i;
        }
    }

    return -1;
}

int getFirstFreeOpenDirectoryTableEntry() {
    int i;

    for (i = 0; i < max_opened_directories; i++) {
        if (odt.table[i].info.TypeVal == TYPEVAL_INVALIDO) {
            //printf("First Free Index in Open Directory Table: %d\n", i);
            return i;
        }
    }

    return -1;
}

int getFirstClusterFromPathname(char *pathname, char *buffer, int last) {
    int indexInDirectory = -1;
    int nextCluster = -1;
    int lastCluster = -1;

    char del[2] = "/";
    char *token;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));

    if (pathname[0] == '/') {
        lastCluster = root_dir_cluster;
        nextCluster = root_dir_cluster;

        token = strtok(&pathname[1], del);
    }
    else {
        lastCluster = cwd_first_cluster;
        nextCluster = cwd_first_cluster;

        token = strtok(pathname, del);
    }

    while(token != NULL) {
        lastCluster = nextCluster;

        strcat(buffer, "/");
        strcat(buffer, token);
        //printf("Token: %s\n", token);

        readDirectoryRecords(nextCluster, dir);
        indexInDirectory = getFileIndexInDirectory(token, dir);

        if (indexInDirectory >= 0) {
            nextCluster = dir[indexInDirectory].firstCluster;
        }

        token = strtok(NULL, del);
    }

    free(dir);

    if (last == 0) {
        return lastCluster;
    }

    if (last == 1) {
        return nextCluster;
    }

    return -1;
}

/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */
/* ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ ~~ */

FILE2 create2 (char *filename) {
    int i;
    int handle = -1;
    int indexInDirectory = -1;

    int firstFreeCluster = -1;
    int firstFreeDirEntry = -1;
    struct t2fs_record new_record = {0};

    if (opened_files >= max_opened_files) {
        return -1;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(filename, path, 0);

    if (firstClusterFromName < 0) {
        return -2;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    readDirectoryRecords(firstClusterFromName, dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(lastToken, dir);

    if (indexInDirectory >= 0) {
        handle = open2(lastToken);
        truncate2(handle);

        free(dir);
        free(fat);

        return handle;
    }

    firstFreeCluster = getFirstFreeCluster(fat);
    firstFreeDirEntry = getFirstFreeDirectoryEntry(dir);

    if (firstFreeCluster == -1) {
        free(dir);
        free(fat);

        return -3;
    }

    if (firstFreeDirEntry == -1) {
        free(dir);
        free(fat);

        return -4;
    }

    opened_files++;
    handle = getFirstFreeOpenFileTableEntry();

    new_record.TypeVal = TYPEVAL_REGULAR;
    strcpy(new_record.name, lastToken);
    new_record.bytesFileSize = 0x00000000;
    new_record.clustersFileSize = 0x00000001;
    new_record.firstCluster = firstFreeCluster;

    dir[firstFreeDirEntry] = new_record;
    oft.table[handle].offset = 0;
    oft.table[handle].directoryCluster = firstClusterFromName;
    oft.table[handle].info = new_record;
    fat[firstFreeCluster] = 0xFFFFFFFF;
    /*
    for (i = 0; i < nr_directory_entries; i++) {
        printf("TypeVal: %x, name: %s, bytesFileSize: %x, clustersFileSize: %x, firstCluster: %x\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
    */
    writeFATEntries(fat);
    writeDirectoryRecords(firstClusterFromName, dir);

    free(dir);
    free(fat);

	return handle;
}

int delete2 (char *filename) {
    int i;
    int indexInDirectory = -1;
    int isFileOpen = -1;

    int firstCluster = -1;
    int currentCluster = -1;
    int nextCluster = -1;

    int clustersFileSize = -1;
    struct t2fs_record new_record = {0};
    DWORD initial_sector = 0;

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(filename, path, 0);

    if (firstClusterFromName < 0) {
        return -1;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    isFileOpen = getFileIndexInOpenFileTable(lastToken);

    if (isFileOpen >= 0) {
        return -2;
    }

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    BYTE buffer[cluster_size]; // Todo diretório ocupa um cluster
    memset(buffer, 0, sizeof(buffer));

    readDirectoryRecords(firstClusterFromName, dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(lastToken, dir);

    if (indexInDirectory == -1) {
        free(dir);
        free(fat);

        return -3;
    }

    if (dir[indexInDirectory].TypeVal == TYPEVAL_REGULAR || dir[indexInDirectory].TypeVal == TYPEVAL_LINK) {
        firstCluster = dir[indexInDirectory].firstCluster;
        clustersFileSize = dir[indexInDirectory].clustersFileSize;
        currentCluster = firstCluster;

        for (i = 0; i < clustersFileSize; i++) {
            initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
            //printf("Initial Cluster: %x\n", currentCluster);
            write_cluster(initial_sector, buffer);

            nextCluster = fat[currentCluster];
            fat[currentCluster] = 0x00000000;
            currentCluster = nextCluster;
        }

        dir[indexInDirectory] = new_record;
    }
    else {
        free(dir);
        free(fat);

        return -4;
    }
    /*
    for (i = 0; i < nr_directory_entries; i++) {
        printf("TypeVal: %x, name: %s, bytesFileSize: %x, clustersFileSize: %x, firstCluster: %x\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
    */
    writeFATEntries(fat);
    writeDirectoryRecords(firstClusterFromName, dir);

    free(dir);
    free(fat);

    return 0;
}

FILE2 open2 (char *filename) {
    int handle = -1;
    int indexInDirectory = -1;

    if (opened_files >= max_opened_files) {
        return -1;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(filename, path, 0);

    if (firstClusterFromName < 0) {
        return -2;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));

    readDirectoryRecords(firstClusterFromName, dir);

    indexInDirectory = getFileIndexInDirectory(lastToken, dir);

    if (indexInDirectory == -1) {
        free(dir);

        return -3;
    }

    opened_files++;
    handle = getFirstFreeOpenFileTableEntry();
    oft.table[handle].offset = 0;
    oft.table[handle].directoryCluster = firstClusterFromName;
    oft.table[handle].info = dir[indexInDirectory];
    /*
    printf("TypeVal: %x, name: %s, bytesFileSize: %x, ", oft.table[handle].info.TypeVal, oft.table[handle].info.name, oft.table[handle].info.bytesFileSize);
    printf("clustersFileSize: %x, firstCluster: %x\n", oft.table[handle].info.clustersFileSize, oft.table[handle].info.firstCluster);
    */
    free(dir);

	return handle;
}

int close2 (FILE2 handle) {
    struct t2fs_record new_record = {0};

    if (handle < 0 || handle >= max_opened_files) {
        return -1;
    }

    if (oft.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_files < 0) {
        return -3;
    }

    opened_files--;
    oft.table[handle].offset = 0;
    oft.table[handle].directoryCluster = 0;
    oft.table[handle].info = new_record;

	return 0;
}

int read2 (FILE2 handle, char *buffer, int size) {
    int i;
    int bytes_read = 0;
    int currentOffset = -1;

    int firstCluster = -1;
    int currentCluster = -1;
    int nextCluster = -1;

    int clustersFileSize = -1;
    DWORD initial_sector = 0;

    if (handle < 0 || handle >= max_opened_files) {
        return -1;
    }

    if (oft.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_files < 0) {
        return -3;
    }

    if (size < 0) {
        return -4;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    firstCluster = oft.table[handle].info.firstCluster;
    clustersFileSize = oft.table[handle].info.clustersFileSize;
    currentCluster = firstCluster;
    currentOffset = oft.table[handle].offset;

    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    readFATEntries(fat);

    BYTE tmp[clustersFileSize * cluster_size];
    memset(tmp, 0, sizeof(tmp));

    for (i = 0; i < clustersFileSize; i++) {
        initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
        read_cluster(initial_sector, &tmp[i * cluster_size]);
        /*
        for (i = 0; i < cluster_size; i++) {
            printf("%c ", tmp[i]);
        }
        */
        nextCluster = fat[currentCluster];
        currentCluster = nextCluster;
    }

    i = 0;
    while (tmp[i + currentOffset] != '\0' && bytes_read < size) {
        buffer[i] = tmp[i + currentOffset];
        //printf("%c", buffer[i]);
        i++;
        bytes_read++;
    }

    buffer[i + currentOffset] = '\0';
    oft.table[handle].offset = currentOffset + bytes_read;

    free(fat);

	return bytes_read;
}

int write2 (FILE2 handle, char *buffer, int size) {
    int i;
    int bytes_written = 0;
    int currentOffset = -1;
    int clusterOffset = -1;
    int indexInDirectory = -1;

    int firstFreeCluster = -1;
    int firstCluster = -1;
    int currentCluster = -1;
    int nextCluster = -1;
    int directoryCluster = -1;

    int bytesFileSize = -1;
    int clustersFileSize = -1;
    DWORD initial_sector = 0;

    if (handle < 0 || handle >= max_opened_files) {
        return -1;
    }

    if (oft.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_files < 0) {
        return -3;
    }

    if (size < 0) {
        return -4;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    firstCluster = oft.table[handle].info.firstCluster;
    clustersFileSize = oft.table[handle].info.clustersFileSize;
    bytesFileSize = oft.table[handle].info.bytesFileSize;
    currentCluster = firstCluster;
    currentOffset = oft.table[handle].offset;
    directoryCluster = oft.table[handle].directoryCluster;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    readDirectoryRecords(directoryCluster, dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(oft.table[handle].info.name, dir);
    //printf("Name: %s\n", oft.table[handle].info.name);

    if (indexInDirectory == -1) {
        free(dir);
        free(fat);

        return -5;
    }

    BYTE tmp[clustersFileSize * cluster_size];
    memset(tmp, 0, sizeof(tmp));

    for (i = 0; i < clustersFileSize; i++) {
        initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
        read_cluster(initial_sector, &tmp[i * cluster_size]);
        /*
        for (i = 0; i < cluster_size; i++) {
            printf("%c ", tmp[i]);
        }
        */
        nextCluster = fat[currentCluster];
        currentCluster = nextCluster;
    }

    i = 0;
    while (buffer[i] != '\0' && bytes_written < size) {
        tmp[i + currentOffset] = buffer[i];
        //printf("%c", buffer[i]);
        i++;
        bytes_written++;
    }
    /*
    for (i = 0; tmp[i] != '\0'; i++) {
        printf("%c", tmp[i]);
    }
    */
    oft.table[handle].offset = currentOffset + bytes_written;
    clusterOffset = (bytesFileSize + bytes_written) / cluster_size;

    currentCluster = firstCluster;
    for (i = 0; i < clustersFileSize; i++) {
        initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
        write_cluster(initial_sector, &tmp[i * cluster_size]);

        nextCluster = fat[currentCluster];
        if ((i + 1) >= clusterOffset) {
            firstFreeCluster = getFirstFreeCluster(fat);

            if (firstFreeCluster == -1) {
                free(dir);
                free(fat);

                return -6;
            }

            fat[currentCluster] = firstFreeCluster;
            fat[firstFreeCluster] = 0xFFFFFFFF;

            nextCluster = firstFreeCluster;
        }

        currentCluster = nextCluster;
    }

    if ((oft.table[handle].offset + bytes_written) > bytesFileSize) {
        oft.table[handle].info.bytesFileSize = oft.table[handle].offset;
        dir[indexInDirectory].bytesFileSize = oft.table[handle].offset;
    }

    oft.table[handle].info.clustersFileSize = clusterOffset + 1;
    dir[indexInDirectory].clustersFileSize = clusterOffset + 1;
    /*
    for (i = 0; i < nr_directory_entries; i++) {
        printf("TypeVal: %x, name: %s, bytesFileSize: %x, clustersFileSize: %x, firstCluster: %x\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
    */
    writeFATEntries(fat);
    writeDirectoryRecords(directoryCluster, dir);

    free(dir);
    free(fat);

	return bytes_written;
}

int truncate2 (FILE2 handle) {
    int i;
    int currentOffset = -1;
    int clusterOffset = -1;
    int indexInDirectory = -1;

    int firstCluster = -1;
    int currentCluster = -1;
    int nextCluster = -1;
    int directoryCluster = -1;

    int clustersFileSize = -1;
    int bytesFileSize = -1;
    DWORD initial_sector = 0;

    if (handle < 0 || handle >= max_opened_files) {
        return -1;
    }

    if (oft.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_files < 0) {
        return -3;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    firstCluster = oft.table[handle].info.firstCluster;
    clustersFileSize = oft.table[handle].info.clustersFileSize;
    bytesFileSize = oft.table[handle].info.bytesFileSize;
    currentCluster = firstCluster;
    currentOffset = oft.table[handle].offset;
    clusterOffset = bytesFileSize / cluster_size;
    directoryCluster = oft.table[handle].directoryCluster;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    readDirectoryRecords(directoryCluster, dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(oft.table[handle].info.name, dir);

    if (indexInDirectory == -1) {
        free(dir);
        free(fat);

        return -4;
    }

    BYTE buffer[clustersFileSize * cluster_size];
    memset(buffer, 0, sizeof(buffer));

    for (i = 0; i < clustersFileSize; i++) {
        initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
        read_cluster(initial_sector, &buffer[i * cluster_size]);
        /*
        for (i = 0; i < cluster_size; i++) {
            printf("%c ", buffer[i]);
        }
        */
        nextCluster = fat[currentCluster];
        currentCluster = nextCluster;
    }

    for (i = 0; (i + currentOffset) < bytesFileSize; i++) {
        buffer[i + currentOffset] = 0;
    }

    currentCluster = firstCluster;
    for (i = 0; i < clustersFileSize; i++) {
        initial_sector = first_data_sector + (currentCluster * sectors_per_cluster);
        write_cluster(initial_sector, &buffer[i * cluster_size]);

        nextCluster = fat[currentCluster];

        if ((i + 1) == clusterOffset) {
            fat[currentCluster] = 0xFFFFFFFF;
        }

        if ((i + 1) > clusterOffset) {
            fat[currentCluster] = 0x00000000;
        }

        currentCluster = nextCluster;
    }

    oft.table[handle].info.bytesFileSize = currentOffset;
    oft.table[handle].info.clustersFileSize = clustersFileSize - clusterOffset;
    dir[indexInDirectory].bytesFileSize = currentOffset;
    dir[indexInDirectory].clustersFileSize = clustersFileSize - clusterOffset;
    /*
    for (i = 0; i < nr_directory_entries; i++) {
        printf("TypeVal: %x, name: %s, bytesFileSize: %x, clustersFileSize: %x, firstCluster: %x\n", dir[i].TypeVal, dir[i].name, dir[i].bytesFileSize, dir[i].clustersFileSize, dir[i].firstCluster);
    }
    */
    writeFATEntries(fat);
    writeDirectoryRecords(directoryCluster, dir);

    free(dir);
    free(fat);

	return 0;
}

int seek2 (FILE2 handle, DWORD offset) {
    if (handle < 0 || handle >= max_opened_files) {
        return -1;
    }

    if (oft.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_files < 0) {
        return -3;
    }

    if (offset == -1) {
        oft.table[handle].offset = oft.table[handle].info.bytesFileSize;
    }
    else {
        oft.table[handle].offset = offset;
    }

	return 0;
}

int mkdir2 (char *pathname) {
    int indexInDirectory = -1;

    int firstFreeCluster = -1;
    int firstFreeDirEntry = -1;
    struct t2fs_record new_record = {0};
    struct t2fs_record current_dir = {0};
    struct t2fs_record parent_dir = {0};

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(pathname, path, 1);

    if (firstClusterFromName < 0) {
        return -1;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));

    readDirectoryRecords(firstClusterFromName, dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(lastToken, dir);

    if (indexInDirectory >= 0) {
        free(dir);
        free(fat);

        return -2;
    }

    firstFreeCluster = getFirstFreeCluster(fat);
    firstFreeDirEntry = getFirstFreeDirectoryEntry(dir);

    if (firstFreeCluster == -1) {
        free(dir);
        free(fat);

        return -3;
    }

    if (firstFreeDirEntry == -1) {
        free(dir);
        free(fat);

        return -4;
    }

    new_record.TypeVal = TYPEVAL_DIRETORIO;
    strcpy(new_record.name, lastToken++);
    new_record.bytesFileSize = cluster_size;
    new_record.clustersFileSize = 0x00000001;
    new_record.firstCluster = firstFreeCluster;

    dir[firstFreeDirEntry] = new_record;
    fat[firstFreeCluster] = 0xFFFFFFFF;

    writeDirectoryRecords(firstClusterFromName, dir);

    readDirectoryRecords(new_record.firstCluster, dir);

    current_dir.TypeVal = TYPEVAL_DIRETORIO;
    strncpy(current_dir.name, ".", 2);
    current_dir.bytesFileSize = cluster_size;
    current_dir.clustersFileSize = 0x00000001;
    current_dir.firstCluster = firstFreeCluster;

    parent_dir.TypeVal = TYPEVAL_DIRETORIO;
    strncpy(parent_dir.name, "..", 3);
    parent_dir.bytesFileSize = cluster_size;
    parent_dir.clustersFileSize = 0x00000001;
    parent_dir.firstCluster = firstClusterFromName;

    dir[0] = current_dir;
    dir[1] = parent_dir;

    writeFATEntries(fat);
    writeDirectoryRecords(new_record.firstCluster, dir);

    free(dir);
    free(fat);

	return 0;
}

int rmdir2 (char *pathname) {
    int i;
    int indexInDirectory = -1;

    int currentfirstCluster = -1;
    struct t2fs_record new_record = {0};

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(pathname, path, 0);

    if (firstClusterFromName < 0) {
        return -1;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    struct t2fs_record *parent_dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));
    DWORD *fat = calloc(nr_clusters, sizeof(DWORD));
    /*
    BYTE buffer[cluster_size]; // Todo diretório ocupa um cluster
    memset(buffer, 0, sizeof(buffer));
    */
    readDirectoryRecords(firstClusterFromName, parent_dir);
    readFATEntries(fat);

    indexInDirectory = getFileIndexInDirectory(lastToken, parent_dir);

    if (indexInDirectory == -1) {
        free(parent_dir);
        free(fat);

        return -2;
    }

    if (parent_dir[indexInDirectory].TypeVal == TYPEVAL_DIRETORIO) {
        readDirectoryRecords(parent_dir[indexInDirectory].firstCluster, dir);

        for (i = 2; i < nr_directory_entries; i++) {
            if (dir[i].TypeVal != TYPEVAL_INVALIDO) {
                free(parent_dir);
                free(dir);
                free(fat);

                return -3;
            }
        }
    }
    else {
        free(parent_dir);
        free(dir);
        free(fat);

        return -4;
    }

    currentfirstCluster = parent_dir[indexInDirectory].firstCluster;
    fat[currentfirstCluster] = 0x00000000;

    dir[0] = new_record;
    dir[1] = new_record;

    parent_dir[indexInDirectory] = new_record;

    writeFATEntries(fat);
    writeDirectoryRecords(firstClusterFromName, parent_dir);
    writeDirectoryRecords(dir[indexInDirectory].firstCluster, dir);

    free(parent_dir);
    free(dir);
    free(fat);

	return 0;
}

int chdir2 (char *pathname) {
    int isPathnameValid = -1;

    char buffer[max_path_length];
    memset(buffer, 0, sizeof(buffer));

    if (first_time == 1) {
        readSuperblock();
    }

    isPathnameValid = getFirstClusterFromPathname(pathname, buffer, 1);

    if (isPathnameValid < 0) {
        return -1;
    }

    cwd_first_cluster = isPathnameValid;
    strncpy(cwd, buffer, strlen(buffer));
    //printf("New Current Working Directory: %s\n", cwd);

	return 0;
}

int getcwd2 (char *pathname, int size) {
    if (size < strlen(cwd)) {
        return -1;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    strncpy(pathname, cwd, size);
    printf("Current Working Directory: %s\n", pathname);

	return 0;
}

DIR2 opendir2 (char *pathname) {
    int handle = -1;
    int indexInDirectory = -1;

    if (opened_directories >= max_opened_directories) {
        return -1;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    int firstClusterFromName = cwd_first_cluster;
    char path[max_path_length];
    memset(path, 0, sizeof(path));

    firstClusterFromName = getFirstClusterFromPathname(pathname, path, 0);

    if (firstClusterFromName < 0) {
        return -2;
    }

    char *lastToken = strrchr(path, '/');
    lastToken++;

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));

    readDirectoryRecords(firstClusterFromName, dir);

    indexInDirectory = getFileIndexInDirectory(lastToken, dir);

    if (indexInDirectory == -1) {
        free(dir);

        return -3;
    }

    opened_directories++;
    handle = getFirstFreeOpenDirectoryTableEntry();
    odt.table[handle].offset = 0;
    odt.table[handle].info = dir[indexInDirectory];
    /*
    printf("TypeVal: %x, name: %s, bytesFileSize: %x, ", odt.table[handle].info.TypeVal, odt.table[handle].info.name, odt.table[handle].info.bytesFileSize);
    printf("clustersFileSize: %x, firstCluster: %x\n", odt.table[handle].info.clustersFileSize, odt.table[handle].info.firstCluster);
    //readDirectoryRecords(odt.table[handle].info.firstCluster, dir);
    */
    free(dir);

	return handle;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {
    int currentOffset = -1;

    if (handle < 0 || handle >= max_opened_directories) {
        dentry = NULL;

        return -1;
    }

    if (odt.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (first_time == 1) {
        readSuperblock();
    }

    if (opened_directories < 0) {
        dentry = NULL;

        return -3;
    }

    struct t2fs_record *dir = calloc(nr_directory_entries, sizeof(struct t2fs_record));

    readDirectoryRecords(odt.table[handle].info.firstCluster, dir);
    currentOffset = odt.table[handle].offset;

    if (currentOffset == nr_directory_entries) {
        odt.table[handle].offset = 0;
        dentry = NULL;

        return -4;
    }

    strncpy(dentry->name, dir[currentOffset].name, 51);
    dentry->fileType = dir[currentOffset].TypeVal;
    dentry->fileSize = dir[currentOffset].bytesFileSize;
    /*
    //printf("Current Offset: %d\n", odt.table[handle].offset);
    printf("name: %s, fileType: %x, fileSize: %x\n", dentry->name, dentry->fileType, dentry->fileSize);
    */
    odt.table[handle].offset++;

    free(dir);

	return 0;
}

int closedir2 (DIR2 handle) {
	struct t2fs_record new_record = {0};

    if (handle < 1 || handle >= max_opened_directories) {
        return -1;
    }

    if (odt.table[handle].info.TypeVal == TYPEVAL_INVALIDO) {
        return -2;
    }

    if (opened_directories < 0) {
        return -3;
    }

    opened_directories--;
    odt.table[handle].offset = 0;
    odt.table[handle].info = new_record;

	return 0;
}

int ln2 (char *linkname, char *filename) {
    return -1;
}

int identify2 (char *name, int size) {
	strncpy (name, "Humberto Lentz - 242308\nMakoto Ishikawa - 216728\nPedro Teixeira - 228509\0", size);

	return 0;
}

