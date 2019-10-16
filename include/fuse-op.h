//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_FUSE_OP_H
#define RUCIO_FUSE_FUSE_OP_H

#include <fuse.h>
#include <stdio.h>

static int rucio_getattr (const char *path, struct stat *st){
  printf("rucio_getattr called");
  return 0;
}
static int rucio_readdir (const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
  printf("rucio_readdir called");
  return 0;
}
static int rucio_read (const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fi){
  printf("rucio_read called");
  return 0;
}

#endif //RUCIO_FUSE_FUSE_OP_H
