//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>
#include <json.hpp>
#include <fstream>
#include <fastlog.h>
#include <REST-API.h>
#include <dirent.h>
#include <algorithm>

std::unordered_map<std::string, rucio_server> rucio_server_map = {};
std::vector<std::string> rucio_server_names;

bool key_exists(const std::string& key){
  return rucio_server_map.count(key)>0;
}

connection_parameters* get_server_params(const std::string& server_name){
  return (key_exists(server_name)) ? rucio_server_map[server_name].get_params() : nullptr;
}

std::string* get_server_config(const std::string& server_name){
  return (key_exists(server_name)) ? &(rucio_server_map[server_name].config_file_path) : nullptr;
}

token_info* get_server_token(const std::string& server_name){
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

std::string get_cfg_value(std::string& line){
  line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
  return line.substr(line.find('=') + 1);
}

void parse_settings_cfg(){
  std::string ruciofs_settings_root;

  if(getenv("RUCIOFS_SETTINGS_FILES_ROOT") != NULL){
    ruciofs_settings_root = getenv("RUCIOFS_SETTINGS_FILES_ROOT");
  } else ruciofs_settings_root = "./rucio-settings";

  fastlog(INFO,"Settings files contained at: %s", ruciofs_settings_root.data());

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
        srv.config_file_path = ruciofs_settings_root + "/" + file_name;
        auto srv_name = file_name.substr(0, file_name.find(".cfg"));

        std::ifstream settings_file;
        settings_file.open(srv.config_file_path.data());
        std::string line;
        while (getline(settings_file, line)) {
          if (line.rfind("rucio_host", 0) == 0) {
            srv.rucio_conn_params.server_url = get_cfg_value(line);
          }

          if (line.rfind("username", 0) == 0) {
            srv.rucio_conn_params.user_name = get_cfg_value(line);
          }

          if (line.rfind("password", 0) == 0) {
            srv.rucio_conn_params.password = get_cfg_value(line);
          }

          if (line.rfind("account", 0) == 0) {
            srv.rucio_conn_params.account_name = get_cfg_value(line);
          }
        }

        fastlog(INFO, "\tServer %d -> %s:\n"
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
      inode = readdir(dp);
    }
  }
}
