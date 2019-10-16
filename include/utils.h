//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_UTILS_H
#define RUCIO_FUSE_UTILS_H

// Returns true if and only if path is root path
bool is_root_path(const char *path);

// This function returns -1 if path contains no token
// 0 if path is root
// POSIX format depth in other cases
int path_depth(const char *path, char token = '/');

// This function returns true is the depth is 1 (e.g. /scope1 or /scope1/)
bool is_main_scope(const char *path);

#endif //RUCIO_FUSE_UTILS_H
