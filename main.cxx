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

  parse_settings_cfg();

  std::vector<std::string> argvect(argv, argv + argc);
  if(std::find(argvect.begin(),argvect.end(),"-vv") != argvect.end()){
    logLevel = DEBUG;
  }else if(std::find(argvect.begin(),argvect.end(),"-v") != argvect.end()){
    logLevel = INFO;
  }else{
    logLevel = ERROR;
  }

  auto fOpt = std::find(argvect.begin(),argvect.end(),"-f");

  const uint fuse_argc = 5;
  char* fuse_argv[fuse_argc];

  fuse_argv[0] = argv[0];

  // Setting up mount path
  std::string path_option[2] = {"-f", (fOpt==argvect.end())?"/ruciofs":*(fOpt+1)};

  fastlog(INFO, "Mount path set to %s", path_option[1].data());

  fuse_argv[1] = strdup(path_option[0].data());
  mkdir(path_option[1].data(), 0755);
  mkdir(rucio_cache_path.data(), 0755);
  fuse_argv[2] = strdup(path_option[1].data());

  if(!check_permissions(fuse_argv[2])){
    fastlog(ERROR, "Fatal error occurred. Aborting!");
    return 1;
  }

  // Setting up NEEDED options to allow users' access
  std::string allow_other[2] = {"-o", "allow_other"};
  fuse_argv[3] = strdup(allow_other[0].data());
  fuse_argv[4] = strdup(allow_other[1].data());

	return fuse_main(fuse_argc, fuse_argv, &operations, nullptr);
}
