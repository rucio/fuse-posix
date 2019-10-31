//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <fuse.h>
#include <fuse-op.h>

static struct fuse_operations operations = {0};

int main( int argc, char *argv[] )
{
  operations.getattr = rucio_getattr;
  operations.readdir = rucio_readdir;
  operations.read	= rucio_read;

  //TODO: parse parameters from json or similar
  
	return fuse_main(argc, argv, &operations, NULL );
}
