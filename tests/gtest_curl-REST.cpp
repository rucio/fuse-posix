/*
Authors:
- Vivek Nigam <viveknigam.nigam3@gmail.com>, 2020
*/

#include <curl-REST.h>
#include <gtest/gtest.h>
#include <iostream>

TEST(curl_Test, Test){
    auto res = GET("www.example.com");
    ASSERT_EQ(1, res.payload.size());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}