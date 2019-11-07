//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>
#include <json.hpp>
#include <fstream>
#include <fastlog.h>
#include <REST-API.h>

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

void parse_settings(){
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
