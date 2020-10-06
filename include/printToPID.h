/*
Created by Gabriele Gaetano Fronzé on 2019-10-16.
Authors:
- Gabriele Gaetano Fronzé <gabriele.fronze at to.infn.it>, 2019-2020
*/

#ifndef RUCIO_FUSE_POSIX_PRINTTOPID_H
#define RUCIO_FUSE_POSIX_PRINTTOPID_H

#include <string>

#if __linux__
  void printToPID(int pid, std::string str){
    auto command = std::string("lsof -p "+std::to_string(pid)+" | grep -e /dev/ttys | grep 0u | grep -oE '[^ ]+$'")
    std::array<char, 128> buffer;
    std::string stdout_file;
    FILE* pipe = popen(command.c_str(), "r");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    freopen(stdout_file.data(),"w",stdout);
    printf("%s",str.data());
    fclose(stdout);
  }
#elif __APPLE__
  void printToPID(int pid, std::string str){
    auto stdout_file = std::string("/proc/"+std::to_string(pid)+"/fd/1");
    freopen(stdout_file.data(),"w",stdout);
    printf("%s",str.data());
    fclose(stdout);
  }
#else
  void printToPID(unsigned int /*pid*/, std::string /*str*/){}
#endif



#endif //RUCIO_FUSE_POSIX_PRINTTOPID_H
