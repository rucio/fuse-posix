//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>
#include <json.hpp>
#include <fstream>
#include <fastlog.h>
#include <REST-API.h>
#include <dirent.h>

std::unordered_map<std::string, rucio_server> rucio_server_map = {};
std::vector<std::string> rucio_server_names;

bool key_exists(std::string key){
  return rucio_server_map.count(key)>0;
}

connection_parameters* get_server_params(std::string server_name){
  return (key_exists(server_name)) ? rucio_server_map[server_name].get_params() : nullptr;
}

token_info* get_server_token(std::string server_name){
  return (key_exists(server_name)) ? rucio_server_map[server_name].get_token() : nullptr;
}

using json = nlohmann::json;
using namespace fastlog;

void parse_settings_json(){
  std::ifstream settings_file;

  std::string ruciofs_settings_filename;

  if(getenv("RUCIOFS_SETTINGS_FILE") != NULL){
    ruciofs_settings_filename = getenv("RUCIOFS_SETTINGS_FILE");
  } else ruciofs_settings_filename = "./settings.json";

  fastlog(INFO,"\nSettings file at: %s", ruciofs_settings_filename.data());

  settings_file.open (ruciofs_settings_filename, std::ifstream::in);
  auto json_settings = json::parse(settings_file);

  fastlog(INFO,"Parsing settings file:");
  size_t i_srv = 0;
  for (auto& server : json_settings["servers"].items()){
    auto values = server.value();
    auto srv = rucio_server(values["url"], values["account"],values["username"],values["password"]);

    if(not rucio_ping(srv.rucio_conn_params.server_url)){
      fastlog(ERROR, "Server %s not reachable, skipping connection.", srv.rucio_conn_params.server_url.data());
      continue;
    }

    std::string srv_name = values["name"];

    fastlog(INFO,"\n"
                 "\tServer %d -> %s:\n"
                 "\t\turl = %s\n"
                 "\t\taccount = %s\n"
                 "\t\tusername = %s\n"
                 "\t\tpassword = %s",
            i_srv++,
            srv_name.data(),
            srv.rucio_conn_params.server_url.data(),
            srv.rucio_conn_params.account_name.data(),
            srv.rucio_conn_params.user_name.data(),
            srv.rucio_conn_params.password.data());

    rucio_server_names.emplace_back(srv_name);
    rucio_server_map.emplace(std::make_pair(srv_name,srv));
  }
}

void parse_settings_cfg(){
  std::string ruciofs_settings_root;

  if(getenv("RUCIOFS_SETTINGS_FILES_ROOT") != NULL){
    ruciofs_settings_root = getenv("RUCIOFS_SETTINGS_FILES_ROOT");
  } else ruciofs_settings_root = "./rucio-settings";

  fastlog(INFO,"\nSettings files contained at: %s", ruciofs_settings_root.data());

  struct dirent *inode = nullptr;
  DIR *dp = opendir(ruciofs_settings_root.data());

  size_t i_srv = 0;

  if (dp) {
    inode = readdir(dp);

    while(inode){
      std::string file_name = inode->d_name;

      if(file_name.find(".cfg") != std::string::npos) {
        fastlog(INFO, "Parsing settings file: %s", file_name.data());

        auto srv = rucio_server();

        std::ifstream settings_file;
        settings_file.open((ruciofs_settings_root+"/"+file_name).data());
        std::string line;
        while (getline(settings_file, line)) {
          if (line.rfind("rucio_host", 0) == 0) {
            srv.rucio_conn_params.server_url = line.substr(line.find("rucio_host = "));
          }

          if (line.rfind("username", 0) == 0) {
            srv.rucio_conn_params.user_name = line.substr(line.find("username = "));
          }

          if (line.rfind("password", 0) == 0) {
            srv.rucio_conn_params.password = line.substr(line.find("password = "));
          }

          if (line.rfind("account", 0) == 0) {
            srv.rucio_conn_params.account_name = line.substr(line.find("account = "));
          }
        }

        fastlog(INFO, "\n"
                      "\tServer %d -> %s:\n"
                      "\t\turl = %s\n"
                      "\t\taccount = %s\n"
                      "\t\tusername = %s\n"
                      "\t\tpassword = %s",
                i_srv++,
                file_name.data(),
                srv.rucio_conn_params.server_url.data(),
                srv.rucio_conn_params.account_name.data(),
                srv.rucio_conn_params.user_name.data(),
                srv.rucio_conn_params.password.data());

//        rucio_server_names.emplace_back(srv_name);
//        rucio_server_map.emplace(std::make_pair(srv_name,srv));
      }
      inode = readdir(dp);
    }
  }
}
