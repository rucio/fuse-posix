/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Copyright European Organization for Nuclear Research (CERN)
Licensed under the Apache License, Version 2.0 (the "License");
You may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Authors:
 - Gabriele Gaetano Fronzé, <gfronze@cern.ch>, 2019-2020
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

  std::vector<std::string> argvect(argv, argv + argc);

  if(std::find(argvect.begin(),argvect.end(),"-vv") != argvect.end()){
    fastlog::logLevel = DEBUG;
  }else if(std::find(argvect.begin(),argvect.end(),"-v") != argvect.end()){
    fastlog::logLevel = INFO;
  }else{
    fastlog::logLevel = ERROR;
  }

  auto configOpt = std::find(argvect.begin(),argvect.end(),"-c");

  if(configOpt == argvect.end()) {
    configOpt = std::find(argvect.begin(),argvect.end(),"--config");
  }

  if(configOpt != argvect.end()) {
    parse_settings_cfg(*(configOpt+1));
    fastlog(INFO, "Using custom settings location: %s", (configOpt+1)->data());
  } else {
    parse_settings_cfg();

    std::string ruciofs_settings_root = "./rucio-settings";
    if(getenv("RUCIOFS_SETTINGS_FILES_ROOT") != NULL){
      ruciofs_settings_root = getenv("RUCIOFS_SETTINGS_FILES_ROOT");
      fastlog(INFO, "Using env RUCIOFS_SETTINGS_FILES_ROOT settings location: %s", ruciofs_settings_root.data());
    } else {
      fastlog(INFO, "Using default settings location: %s", ruciofs_settings_root.data());
    }
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
