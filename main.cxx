//
// Created by Gabriele Gaetano Fronzé on 2019-10-16.
//

#include <fuse.h>
#include <fuse-op.h>
#include <string.h>
#include <sys/stat.h>
#include <json.hpp>
#include <globals.h>
#include <fstream>

using json = nlohmann::json;

static struct fuse_operations operations = {0};

int main( int argc, char *argv[] )
{
  operations.getattr = rucio_getattr;
  operations.readdir = rucio_readdir;
  operations.read	= rucio_read;

  std::ifstream settings_file;
  settings_file.open ("settings.json", std::ifstream::in);
  auto json_settings = json::parse(settings_file);

  for (auto& server : json_settings["servers"].items()){
    auto values = server.value();
    std::cout << values << std::endl;
    auto srv = rucio_server(values["url"], values["account"],values["username"],values["password"]);
    rucio_server_map.emplace(std::make_pair(values["name"],srv));
  }

  char* fuse_argv[3];
  fuse_argv[0] = argv[0];
  std::string option[2] = {"-f", "/ruciofs"};
  fuse_argv[1] = strdup(option[0].c_str());
  mkdir(option[1].c_str(), 0755);
  fuse_argv[2] = strdup(option[1].c_str());

	return fuse_main(3, fuse_argv, &operations, nullptr);
}
