#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "main.h"
#include "hashmap.h"



int main(int argc, char* argv[])
{
    pthread_t *threads[10];
    char* scan_path;
    char  pathbuf[1024];

    DIR*  maindir;
    DIR*  subdir;
    struct dirent* dirfile;
    struct dirent* subfile;
    struct folderhash_param fdp;
    struct hashmap_map *hashmap;

    hashmap = hashmap_new();
    fdp.hashmap = hashmap;


    if (argc == 1) { // if no path was specified, just use the one specified in the #define
        scan_path = malloc(sizeof(DEFAULT_PATH) + 1);
        strncpy(scan_path, DEFAULT_PATH, sizeof(DEFAULT_PATH));
        printf("File not specified, using default path:\n\t%s\n", scan_path);
    } else {
        scan_path = argv[1];
    }

    maindir = opendir(scan_path);
    if (maindir == NULL) {
        perror("ERROR: Failed to open dir");
        return -1;
    }

    errno = 0;
    while ((dirfile = readdir(maindir)) != NULL) {
        errno = 0;
        if (dirfile->d_type == DT_DIR) { // not a dir?
            if (strcmp(dirfile->d_name, ".") == 0 || strcpy(dirfile->d_name, "..") == 0)
                continue; // not the droids I'm looking for

            if ((subdir = opendir(dirfile->d_name)) == NULL) {
                perror("Failed to open subdir");
                /// @Hmmm: How should this handle errors here?
            } else {
                fdp.dir = dirfile;
                hash_folder(&fdp);
            }




            /*chdir(dirfile->d_name);
            while ((subfile = readdir(subdir)) != NULL) {
                if (subfile->d_type == DT_REG) {
                    printf("File name :%s\n", subfile->d_name);

                } else {
                    // For this program, it should just ignore anything not a file.
                }
            }*/
        } else if (dirfile->d_type == DT_REG) {
            // ignore this I guess?
        }

    }
    if (errno != 0) {
        perror("Error occured when reading subfiles of dir");
        return -1;
    }




    closedir(maindir);
    return 0;
}
// TODO: Use ptreads to md5 hash the files in separate folders (maybe like 5-10 at a time?)
// TODO: Use some struct that points to a structure for this thread to store the md5 hashes.
// TODO: Use hashmaps (crc32?) to store md5 hash with crc32
void* hash_folder (void* param) // uses void* because pthread.
{
    struct folderhash_param *fdp = param;
    struct dirent* dirfile = fdp->dir;
    struct dirent* subfile;

    DIR* dir;
    int fd;

    MD5_CTX md5;
    char md5buf[1024];
    char digest[MD5_DIGEST_LENGTH];

    if ((fd = open(dirfile->d_name, O_RDONLY) == -1)) { // need dirfd for openat(2);
        perror("Failed to open directory"); // could use snprintf to format to buffer but whatever.
        // TODO: determine whether perror adds \n
        printf("\t%s", dirfile->d_name);
        pthread_exit(NULL);
    } else {
        if ((dir = fdopendir(fd)) == NULL) {
            perror("failed to fopendir(): ");
            printf("\t%s", dirfile->d_name);
            pthread_exit(NULL);
        }

    }

    while ((subfile = readdir(dir)) != 0) {
        errno = 0;
        if (subfile != NULL) {
            if (subfile->d_type == DT_REG) {
                if (file_md5(fd, (unsigned char *) &digest, md5buf, 1024, subfile->d_name, NULL) > -1) {
                    hashmap_put(fdp->hashmap, subfile->d_name, digest);
                } else {
                    // some error occurred when hashing the file
                }
            } else {
                // shouldn't be here, but ignore it.
            }
        } else {
            perror("ERROR: error occured while reading directory");
            printf("\t%s", dirfile->d_name);
            return 0;
            //pthread_exit(NULL);
        }
    }

    return 0;
}


int file_md5(int dirfd, unsigned char *digest, char *md5_buffer,
        size_t buffsize, const char *filename, MD5_CTX *md5ctx) // md5 is good enough
{
    int fd;
    ssize_t bytes_read;
    errno = 0;
    if ((fd = openat(dirfd, filename, O_RDONLY)) == NULL) {
        perror("ERROR: Failed to open file");
        printf("\t\t%s", filename);
        return -1;
    }

    MD5_Init(md5ctx);

    while((bytes_read = read(fd, md5_buffer, buffsize)) > 0) // 0 bytes returned on EOF, NULL returned on error
        MD5_Update(md5ctx, md5_buffer, (size_t)bytes_read);
    if (errno != 0) {
        perror("ERROR: error occurred when reading from file: ");
        printf("\t\t%s", filename);
        return -1;
    }
    MD5_Final(digest,md5ctx);

    close(fd);
    return 0;
}
