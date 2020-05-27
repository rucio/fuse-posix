/*
Authors:
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <REST-API.h>
#include <globals.h>
#include <iostream>
#include <vector>
#include <gtest/gtest.h>

using namespace std;
using namespace fastlog;

std::string server_name="rucio-dev-server";

TEST(RESTAPI_Test, Test_rucio_ping){
    ASSERT_TRUE(rucio_ping("https://localhost/"));
}

TEST(RESTAPI_Test, Test_server_connection){
    fastlog(INFO, "Servers Found:");
    auto servers = rucio_list_servers();
    if (servers.size()>0){
        for (auto i = servers.begin(); i != servers.end(); ++i)
            cout << *i << endl;
    } else{
        fastlog(ERROR, "No servers found!");
        exit(-1); 
    }  
    EXPECT_NO_THROW(rucio_get_auth_token_userpass(server_name));
    ASSERT_TRUE(rucio_is_token_valid(server_name));
}

TEST(RESTAPI_Test, Test_server_scopes){
    auto scopes = rucio_list_scopes(server_name);
    fastlog(INFO, "Scopes Found: ");
    for (auto i = scopes.begin(); i != scopes.end(); ++i)
        cout << *i << endl; 
    ASSERT_TRUE(scopes.size()>=0);
}

void get_scope_dids(std::string server_short_name, std::string scope_name){
    fastlog(INFO, "Using server: %s | Scope: %s", server_short_name.c_str(), scope_name.c_str());
    std::cout<<std::flush;
    {
        auto ret = rucio_list_dids(scope_name, server_short_name);
        if (not ret.empty()){
            fastlog(INFO, "Detected DIDs");
            for (const auto &did : ret) {
                printf("%s/%s\n", did.scope.c_str(), did.name.c_str());
            }
        } else {
            fastlog(INFO, "No DIDs Detected!");
        }
    }
}

TEST(RESTAPI_Test, Test_scope_dids){
    auto scopes = rucio_list_scopes(server_name);
    if(not scopes.empty()){
        for(const auto& scope : scopes){
            ASSERT_NO_THROW(get_scope_dids(server_name, scope));
        }
    } else {
        fastlog(ERROR, "No Scopes Found!");
    }
}

TEST(RESTAPI_Test, Test__is_container){
    EXPECT_TRUE(rucio_is_container("/"+server_name+"/test/container"));
    //EXPECT_FALSE(rucio_is_container("/"+server_name+"/test/container/dataset1"));
    //EXPECT_FALSE(rucio_is_container("/"+server_name+"/test/dataset3"));
}

bool get_replica_metalinks(const std::string& path){
    try{
        auto replicas = rucio_get_replicas_metalinks(path);
        if (not replicas.empty()){
            fastlog(INFO, "%s Located! Replica metalinks found:", extract_name(path).c_str());
            for (const auto& i : replicas){
                cout << i << endl;
            }
            return true;
        } else {
            fastlog (ERROR, "No Replicas Found!");
            return false;
        }
    }catch (...){
        fastlog(INFO, "%s NOT FOUND at location!", extract_name(path).c_str());
        return false;
    }
}

TEST(RESTAPI_Test, Test_rucio_get_replicas_metalinks){
    ASSERT_TRUE(get_replica_metalinks("/"+server_name+"/test/container/dataset1/file1"));
    ASSERT_TRUE(get_replica_metalinks("/"+server_name+"/test/container/dataset1/file2"));
    ASSERT_FALSE(get_replica_metalinks("/"+server_name+"/test/container/dataset1/file6"));
}

bool get_list_container_dids(const string& server, const string& scope, const string& did_name){
    auto ret = rucio_list_container_dids(scope, did_name, server);
    if (not ret.empty()){
        fastlog(INFO, "DIDs Found at Location /%s/%s/%s", server.c_str(), scope.c_str(), did_name.c_str());
        for(const auto& did : ret){
            cout << did.name << endl;
        }
        return true;
    } else {
        fastlog(INFO, "No DIDs Found at Location /%s/%s/%s", server.c_str(), scope.c_str(), did_name.c_str());
        return false;
    }   
}

TEST(RESTAPI_Test, Test_rucio_list_container_dids){
    ASSERT_TRUE(get_list_container_dids(server_name, "test", "container"));
    ASSERT_FALSE(get_list_container_dids(server_name, "test", "container1"));  
}

int main(int argc, char **argv) {
    parse_settings();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}