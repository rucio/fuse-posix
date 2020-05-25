/*
Authors:
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <utils.h>
#include <vector>
#include <gtest/gtest.h>

using namespace std;

TEST(Utils_Test, Test_is_root_path){
    EXPECT_EQ(1, (bool)is_root_path("/"));
    EXPECT_EQ(0, (bool)is_root_path("/a"));
    EXPECT_EQ(0, (bool)is_root_path("/a/b"));
}

TEST(Utils_Test, Test_path_depth){
    EXPECT_EQ(0, path_depth("/", '/'));
    EXPECT_EQ(1, path_depth("/a",'/'));
    EXPECT_EQ(1, path_depth("/a/",'/'));
    EXPECT_EQ(2, path_depth("/a/b",'/'));
    EXPECT_EQ(6, path_depth("/a/b/c/d/e/f/",'/'));
    EXPECT_EQ(0, path_depth("a", 'a'));
}

TEST(Utils_Test, Test_is_server_mountpoint){
    EXPECT_TRUE(is_server_mountpoint("/server1"));
    EXPECT_TRUE(is_server_mountpoint("/server1/"));
    EXPECT_FALSE(is_server_mountpoint("/"));
    EXPECT_FALSE(is_server_mountpoint("/server1/scope1"));
    EXPECT_FALSE(is_server_mountpoint("/server1/scope1/"));
    EXPECT_FALSE(is_server_mountpoint("/server1/scope1/name1"));
}

TEST(Utils_Test, Test_is_main_scope){
    EXPECT_FALSE(is_main_scope("/server1"));
    EXPECT_FALSE(is_main_scope("/server1/"));
    EXPECT_FALSE(is_main_scope("/"));
    EXPECT_TRUE(is_main_scope("/server1/scope1"));
    EXPECT_TRUE(is_main_scope("/server1/scope1/"));
    EXPECT_FALSE(is_main_scope("/server1/scope1/name1"));
}

TEST(Utils_Test, Test_string_manipulation){
    string test_string = "/server/scope/container/dataset/filename/";
    string exp_string = "/server/scope/container/dataset/filename";
    remove_trailing_token(test_string);

    EXPECT_EQ(test_string, exp_string);
    EXPECT_EQ("server", extract_server_name(test_string));
    EXPECT_EQ("scope", extract_scope(test_string));
    EXPECT_EQ("filename", extract_name(test_string));
    EXPECT_EQ("scope:filename", get_did(test_string));
}

TEST(Utils_Test, Test_structurize_DID){
    std::string test_did = R"(
        {
            "scope": "user.root",
            "type": "FILE",
            "name": "test-19102019.txt",
            "parent": null,
            "level": 0
        })";
    std::vector<rucio_did> did_v;
    structurize_did(test_did, did_v);
    EXPECT_EQ("user.root", did_v[0].scope);
    EXPECT_EQ(rucio_data_type::rucio_file, did_v[0].type);
    EXPECT_EQ("test-19102019.txt", did_v[0].name);
    EXPECT_EQ("null", did_v[0].parent);
    EXPECT_EQ(0, did_v[0].level);
}

TEST(Utils_Test, Test_tokenize_python_list){
    std::vector<std::string> expected_scopes = {"a","b","c","d"};
    std::vector<std::string> scopes;
    tokenize_python_list(R"(["a","b","c","d"])", scopes);
    EXPECT_EQ("a", scopes[0]);
    EXPECT_EQ("b", scopes[1]);
    EXPECT_EQ("c", scopes[2]);
    EXPECT_EQ("d", scopes[3]);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}