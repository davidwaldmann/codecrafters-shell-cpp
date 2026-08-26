#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;

#ifdef _WIN32
constexpr char PATH_LIST_SEPARATOR = ';';
#else
constexpr char PATH_LIST_SEPARATOR = ':';
#endif

const std::array<std::string, 3> BUILTINS = {"echo", "exit", "type"};


std::string type_cmd(const std::string& args) {
  // cmd in BUILTINS-array?
  if (std::find(BUILTINS.begin(), BUILTINS.end(), args) != BUILTINS.end())
    return "" + args + " is a shell builtin";

  const char* path_env = getenv("PATH");
  if (path_env == nullptr)
    return args + ": not found";

  std::stringstream ss_path(path_env);
  std::string path;

  // iterate over pathes in PATH-variable
  while (std::getline(ss_path, path, PATH_LIST_SEPARATOR)) {
    std::string executable = path + '/' + args;
    if (access(executable.c_str(), X_OK) != -1)
      return args + " is " + executable;
  }
  return args + ": not found";
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while(true) {
    std::cout << "$ ";

    std::string input;
    if (!std::getline(std::cin, input))
      break;

    std::string command;
    std::string args;
    // split command by first space
    const auto space = input.find(' ');
    if (space != std::string::npos) {
      command = input.substr(0, space);
      args = input.substr(space + 1);
    }
    else
      command = input;

    if (command == "exit")
      break;
    
    if (command == "type") {
      std::cout << type_cmd(args) << '\n';
    }

    else if (command == "echo") {
      std::cout << args << std::endl;
    }

    else
      std::cout << command << ": command not found" << std::endl;
  }
  
}
