//
// Created by Gabriele Gaetano Fronzé on 2019-10-17.
//

#include <globals.h>
#include <fstream>
#include <fastlog.h>
#include <REST-API.h>
#include <dirent.h>
#include <algorithm>
#include <unistd.h>

std::unordered_map<std::string, rucio_server> rucio_server_map = {};
std::vector<std::string> rucio_server_names;

// Returns true if server name found in map
bool server_exists(const std::string &key){
  return rucio_server_map.count(key)>0;
}

// Returns server parameters if found, otherwise nullptr
connection_parameters* get_server_params(const std::string& server_name){
  return (server_exists(server_name)) ? rucio_server_map[server_name].get_params() : nullptr;
}

// Returns path to server config file as string ptr
std::string* get_server_config(const std::string& server_name){
  return (server_exists(server_name)) ? &(rucio_server_map[server_name].config_file_path) : nullptr;
}

// Returns token_info if found
token_info* get_server_token(const std::string& server_name){
  return (server_exists(server_name)) ? rucio_server_map[server_name].get_token() : nullptr;
}

using namespace fastlog;

// Useful method to extract values from rucio.cfg-like files
std::string get_cfg_value(std::string& line){
  line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
  return line.substr(line.find('=') + 1);
}

// Method to parse and validate all the .cfg files found in the RUCIOFS_SETTINGS_FILES_ROOT folder
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
    while((inode = readdir(dp))){
      std::string file_name = inode->d_name;

      if(file_name.length() > 4) {
        if (file_name.substr(file_name.length() - 4) == ".cfg") {
          fastlog(INFO, "Parsing settings file: %s", file_name.data());

          auto srv = rucio_server();
          srv.config_file_path = ruciofs_settings_root;
          srv.config_file_path += "/" + file_name;
          auto srv_name = file_name.substr(0, file_name.find(".cfg"));

          std::ifstream settings_file;
          settings_file.open(srv.config_file_path.data());
          std::string line;
          std::string ca_file_path;
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

            if (line.rfind("ca_cert", 0) == 0) {
              ca_file_path = get_cfg_value(line);
            }
          }

          auto ca_file = fopen(ca_file_path.data(), "rb");
          if (!ca_file) {
            fastlog(ERROR, "CA file not found at %s. Server will be skipped.", ca_file_path.data());
            continue;
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

          rucio_server_map.emplace(std::make_pair(srv_name, srv));

          if (not rucio_validate_server(srv_name)) {
            fastlog(ERROR, "Unable to validate server %s. Dropping.", srv_name.data());
          } else {
            rucio_server_names.emplace_back(srv_name);
            fastlog(INFO, "Server %s validated!", srv_name.data());
          }
        }
      }
    }
  }
}

// Method to check read/write permissions on both mountpoint or cache path
bool check_permissions(const std::string& mountpoint_path){
  int mountpoint_status = access(mountpoint_path.data(), R_OK && W_OK);

  if(mountpoint_status != 0){
    DIR *dp = opendir(mountpoint_path.data());
    if(dp) {
      fastlog(ERROR, "Mountpoint at %s has wrong permissions!", mountpoint_path.data());
      closedir(dp);
    } else {
      fastlog(ERROR, "Mountpoint at %s does not exist!", mountpoint_path.data());
    }

    return false;
  }

  int cache_status = access(rucio_cache_path.data(), R_OK && W_OK);

  if(cache_status != 0){
    DIR *dp = opendir(rucio_cache_path.data());
    if(dp){
      fastlog(ERROR, "Cache root path at %s has wrong permissions!", rucio_cache_path.data());
      closedir(dp);
    } else {
      fastlog(ERROR, "Cache root path at %s does not exist!", rucio_cache_path.data());
    }
    return false;
  }

  fastlog(INFO, "Local mountpoint and cache location validated!");
  return true;
}
