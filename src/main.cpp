#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
namespace fs = std::filesystem;

#ifdef _WIN32
constexpr char PATH_LIST_SEPARATOR = ';';
#else
constexpr char PATH_LIST_SEPARATOR = ':';
#endif

const std::array<std::string, 3> BUILTINS = {"echo", "exit", "type"};

std::string find_exec_in_PATH(const std::string& args) {
  const char* path_env = getenv("PATH");
  if (path_env == nullptr)
    return "";

  std::stringstream ss_path(path_env);
  std::string path;

  // iterate over pathes in PATH-variable
  while (std::getline(ss_path, path, PATH_LIST_SEPARATOR)) {
    std::string executable = path + '/' + args;
    if (access(executable.c_str(), X_OK) != -1)
      return executable;
  }
  return "";
}

std::string type_cmd(const std::string& args) {
  // cmd in BUILTINS-array?
  if (std::find(BUILTINS.begin(), BUILTINS.end(), args) != BUILTINS.end())
    return "" + args + " is a shell builtin";
  std::string executable = find_exec_in_PATH(args);
  return (executable != "") ?
        args + " is " + executable :
        args + ": not found";
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

    else if (std::string executable = find_exec_in_PATH(command); executable != "") {
      pid_t pid = fork();

      if (pid == 0) {
        std::stringstream sstream(args);
        std::string arg;
        std::vector<std::string> args_vec;

        args_vec.push_back(command);
        while (std::getline(sstream, arg, ' ')) {
          args_vec.push_back(arg);
        }
        std::vector<char *> argv;
        for (auto& s : args_vec) {
          argv.push_back(s.data());
        }
        argv.push_back(nullptr);

        execv(executable.c_str(), argv.data());

        // Fehler in exec
        perror("exec");
        _exit(1);
      }
      else if (pid > 0) {
        // Parent process
        waitpid(pid, nullptr, 0);
      }
      else
        perror("fork");
    }

    else
      std::cout << command << ": command not found" << std::endl;
  }
  
}
