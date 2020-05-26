#include <REST-API.h>
#include <globals.h>
#include <iostream>
#include <gtest/gtest.h>

using namespace std;


std::string server_name;

TEST(RESTAPI_Test, Test_server_connection){
    parse_settings();
    server_name = "rucio-dev-server";

    EXPECT_NO_THROW(rucio_get_auth_token_userpass(server_name));
    ASSERT_TRUE(rucio_is_token_valid(server_name));
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}