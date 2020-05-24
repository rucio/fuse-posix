/*
Authors:
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <utils.h>
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
    EXPECT_EQ(-1, path_depth("a", 'a'));
}

TEST(Utils_Test, Test_string_manipulation){
    EXPECT_EQ("a", extract_server_name("/a/b/c"));
    EXPECT_EQ("c", extract_scope("/a/b/c/d"));
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

// TEST(Utils_Test, Test_string_manipulation){
//     string test_string = "/server/scope/container/dataset/filename/";
//     EXPECT_NO_THROW(remove_trailing_token(test_string));
// }

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}