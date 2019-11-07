//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <fuse.h>
#include <fuse-op.h>
#include <string.h>
#include <sys/stat.h>
#include <globals.h>
#include <fastlog.h>

static struct fuse_operations operations = {0};

using namespace fastlog;

int main( int argc, char *argv[] )
{
  operations.getattr = rucio_getattr;
  operations.readdir = rucio_readdir;
  operations.read	= rucio_read;


  logLevel = INFO;

  parse_settings();

  std::vector<std::string> argvect(argv, argv + argc);
  if(std::find(argvect.begin(),argvect.end(),"-vv") != argvect.end()){
    logLevel = DEBUG;
  }else if(std::find(argvect.begin(),argvect.end(),"-v") != argvect.end()){
    logLevel = INFO;
  }else{
    logLevel = ERROR;
  }

  char* fuse_argv[3];

  fuse_argv[0] = argv[0];

  std::string option[2] = {"-f", "/ruciofs"};

  fuse_argv[1] = strdup(option[0].c_str());

  mkdir(option[1].c_str(), 0755);
  fuse_argv[2] = strdup(option[1].c_str());

	return fuse_main(3, fuse_argv, &operations, nullptr);
}
