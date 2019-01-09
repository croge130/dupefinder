//
// Created by Chris on 2019-01-06.
//
#include <sys/types.h>
#ifndef DUPEFINDER_C_MAIN_H
#define DUPEFINDER_C_MAIN_H


#define DEFAULT_PATH "/mnt/c/Users/Chris/Documents/test/dupe_finder"

static int
file_md5(int dirfd, unsigned char *digest, char *md5_buffer, size_t buffsize, const char *filename, MD5_CTX *md5ctx);
static void* hash_folder (void* param);

struct folderhash_param {
    struct hashmap_map *hashmap;
    struct dirent *dir;
};
#endif //DUPEFINDER_C_MAIN_H
