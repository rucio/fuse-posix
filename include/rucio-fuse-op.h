#include <fuse.h>

#ifndef RUCIO_FUSE_OP
#define RUCIO_FUSE_OP

static int rucio_getattr (const char *path, struct stat *st){
  return 0;
}
static int rucio_readdir (const char *path, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
  return 0;
}
static int rucio_read (const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fi){
  return 0;
}

#endif //RUCIO_FUSE_OP
