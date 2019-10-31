//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <fuse.h>
#include <fuse-op.h>
#include <string.h>

static struct fuse_operations operations = {0};

int main( int argc, char *argv[] )
{
  operations.getattr = rucio_getattr;
  operations.readdir = rucio_readdir;
  operations.read	= rucio_read;

  //TODO: parse parameters from json or similar

  char* fuse_argv[3];
  fuse_argv[0] = argv[0];
  std::string option[2] = {"-f", "/ruciofs"};
  fuse_argv[1] = strdup(option[0].c_str());
  fuse_argv[2] = strdup(option[1].c_str());

	return fuse_main(3, fuse_argv, &operations, nullptr);
}
