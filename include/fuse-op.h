//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#ifndef RUCIO_FUSE_FUSE_OP_H
#define RUCIO_FUSE_FUSE_OP_H

bool is_root_path(const char *path){
  return true;
}

bool is_main_scope(const char *path){
  return true;
}

static int rucio_getattr (const char *path, struct stat *st);
//static int rucio_readdir (const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
//static int rucio_read (const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fi);

#endif //RUCIO_FUSE_FUSE_OP_H
