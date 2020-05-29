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
    auto servers = rucio_list_servers();
    if (servers.empty()){
        fastlog(ERROR, "No servers found!");
        exit(-1); 
    }
    ASSERT_EQ(server_name, servers[0]);
    ASSERT_NO_THROW(rucio_get_auth_token_userpass(server_name));
    ASSERT_TRUE(rucio_is_token_valid(server_name));
}

TEST(RESTAPI_Test, Test_server_scopes){
    auto scopes = rucio_list_scopes(server_name);
    ASSERT_TRUE(scopes.size()==5);
    ASSERT_EQ("data13_hip", scopes[0]);
    ASSERT_EQ("test", scopes[4]);
}

TEST(RESTAPI_Test, Test_scope_dids){
    auto ret = rucio_list_dids("mock", server_name);
    ASSERT_TRUE(ret.empty());
   
    ret = rucio_list_dids("archive", server_name);
    ASSERT_TRUE(ret.empty());

    ret = rucio_list_dids("test", server_name);
    ASSERT_TRUE(ret.size()>=2);
    ASSERT_EQ("container", ret[0].name);
    ASSERT_EQ("dataset3", ret[1].name);
    ASSERT_NE("dataset1", ret[1].name);
}

TEST(RESTAPI_Test, Test_is_container){
    EXPECT_TRUE(rucio_is_container("/"+server_name+"/test/container"));
    //EXPECT_FALSE(rucio_is_container("/"+server_name+"/test/container/dataset1"));
    //EXPECT_FALSE(rucio_is_container("/"+server_name+"/test/dataset3"));
}

TEST(RESTAPI_Test, Test_rucio_get_replicas_metalinks){
    ASSERT_TRUE(not rucio_get_replicas_metalinks("/"+server_name+"/test/container/dataset1/file1").empty());
    ASSERT_FALSE(rucio_get_replicas_metalinks("/"+server_name+"/test/container/dataset1/file1").empty());
    ASSERT_TRUE(not rucio_get_replicas_metalinks("/"+server_name+"/test/container/dataset1/file2").empty());
}

TEST(RESTAPI_Test, Test_rucio_list_container_dids){
    auto ret = rucio_list_container_dids("test", "container", server_name);
    ASSERT_TRUE(ret.size()==2);
    ASSERT_EQ("dataset1", ret[0].name);
    ASSERT_EQ("dataset2", ret[1].name);
    ASSERT_NE("dataser3", ret[1].name);

    ret = rucio_list_container_dids("test", "dataset3", server_name);
    ASSERT_TRUE(ret.size()==1);
    ASSERT_EQ("file4", ret[0].name);

    ret = rucio_list_container_dids("test", "dataset2", server_name);
    ASSERT_TRUE(ret.size()==2);
    ASSERT_EQ("file3", ret[0].name);
    ASSERT_EQ("file4", ret[1].name);
}

int main(int argc, char **argv) {
    parse_settings();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}