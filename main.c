#include <fuse.h>
#include <rucio-fuse-op.h>

static struct fuse_operations operations = {
    .getattr	= rucio_getattr,
    .readdir	= rucio_readdir,
    .read	= rucio_read,
};

int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &operations, NULL );
}
