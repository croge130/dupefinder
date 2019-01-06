#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <fcntl.h>
#include <pthread.h>

//#define DEFAULT_PATH "/mnt/c/Users/Chris/Documents/test/dupe_finder"


int file_md5(unsigned char* digest, char md5_buf[64], size_t buffsize, const char* path);

const char* ignore_subdirs[] = {".", "..", "data"}; // files to ignore


int main(int argc, char* argv[])
{
    pthread_t *threads[10];
    char* scan_path;
    char  pathbuf[1024];

    DIR*  maindir;
    DIR*  subdir;
    struct dirent* dirfile;
    struct dirent* subfile;


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

    while ((dirfile = readdir(maindir)) != NULL) {
        // TODO: Rewrite to avoid using chdir(2)
        chdir(scan_path); // would it be faster to use fchdir(2) instead? probably, but not noticeably.
        if (dirfile->d_type == DT_DIR) { // not a dir?
            if (strcmp(dirfile->d_name, ".") == 0 || strcpy(dirfile->d_name, "..") == 0)
                continue; // not the droids I'm looking for

            if ((subdir = opendir(dirfile->d_name)) == NULL) {
                perror("Failed to open subdir");
                /// @Hmmm: How should this handle errors here?
            }

            chdir(dirfile->d_name);
            while ((subfile = readdir(subdir)) != NULL) {
                if (subfile->d_type == DT_REG) {
                    printf("File name :%s\n", subfile->d_name);
                    // how the fuck am I going to store such a massive amount of files.
                    /// @Hmmm: Hashmaps?
                } else {
                    // For this program, it should just ignore anything not a file.
                }
            }
        } else if (dirfile->d_type == DT_REG) {
            // ignore this I guess?
        }

    }




    closedir(maindir);
    return 0;
}
// TODO: Use ptreads to md5 hash the files in separate folders (maybe like 5-10 at a time?)
// TODO: Use some struct that points to a structure for this thread to store the md5 hashes.
// TODO: Use hashmaps (crc32?) to store md5 hash with crc32
void* hash_folder (void* param) // uses void* because pthread.
{
    struct dirent* dirfile = (struct dirent*)param;
    struct dirent* subfile;
    DIR* dir;
    int fd;

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

    while (subfile = readdir())



}


int file_md5 (unsigned char *digest, char *md5_buffer, size_t buffsize, const char* path) // md5 is good enough
{
    int fd;
    MD5_CTX* md5 = 0;
    ssize_t bytes_read;
    if ((fd = open(path, O_RDONLY)) == -1) {
        perror("ERROR: Failed to open file");
        return -1;
    }

    MD5_Init(md5);

    while((bytes_read = read(fd, md5_buffer, buffsize)) > 0)
        MD5_Update(md5, md5_buffer, (size_t)bytes_read);
    MD5_Final(digest,md5);

    close(fd);
    return 0;
}
