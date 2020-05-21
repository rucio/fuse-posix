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

  const uint fuse_argc = 5;
  char* fuse_argv[fuse_argc];

  fuse_argv[0] = argv[0];

  // Setting up mount path
  std::string path_option[2] = {"-f", "/ruciofs"};
  fuse_argv[1] = strdup(path_option[0].c_str());
  mkdir(path_option[1].c_str(), 0755);
  fuse_argv[2] = strdup(path_option[1].c_str());

  // Setting up NEEDED options to allow users' access
  std::string allow_other[2] = {"-o", "allow_other"};
  fuse_argv[3] = strdup(allow_other[0].c_str());
  fuse_argv[4] = strdup(allow_other[1].c_str());

	return fuse_main(fuse_argc, fuse_argv, &operations, nullptr);
}
