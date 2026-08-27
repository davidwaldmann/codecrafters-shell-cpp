#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <map>
#include <functional>
namespace fs = std::filesystem;

#ifdef _WIN32
constexpr char PATH_LIST_SEPARATOR = ';';
#else
constexpr char PATH_LIST_SEPARATOR = ':';
#endif

std::string type_cmd(const std::string& args);
std::string cd_cmd(const std::string& args);

using Builtin = std::function<std::string(const std::string&)>;
std::map<std::string, Builtin> builtins {
        {"echo", [](const std::string& args){ return args; }},
        {"exit", [](const std::string&){ return ""; }},
        {"type", type_cmd},
        {"pwd", [](const std::string&){ return fs::current_path().string(); }},
        {"cd", cd_cmd}
};

std::string find_exec_in_PATH(const std::string& command) {
  const char* path_env = getenv("PATH");
  if (path_env == nullptr)
    return "";

  std::stringstream ss_path(path_env);
  std::string path;

  // iterate over pathes in PATH-variable
  while (std::getline(ss_path, path, PATH_LIST_SEPARATOR)) {
    std::string executable = path + '/' + command;
    if (access(executable.c_str(), X_OK) != -1)
      return executable;
  }
  return "";
}

std::string type_cmd(const std::string& args) {
  // cmd in BUILTINS-array?
  if (auto search = builtins.find(args); search != builtins.end())
    return "" + args + " is a shell builtin";
  std::string executable = find_exec_in_PATH(args);
  return (executable != "") ?
        args + " is " + executable :
        args + ": not found";
}

void exec_cmd(const std::string& args, const std::string& command, const std::string& executable) {
  pid_t pid = fork();

  if (pid == 0) {
    std::stringstream sstream(args);
    std::string arg;
    std::vector<std::string> args_vec;

    args_vec.push_back(command);
    while (sstream >> arg) {
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

std::string cd_cmd(const std::string& args) {
  if (fs::exists(args)) {
    fs::current_path(args);
    return "";
  } else
    return "cd: " + args + ": No such file or directory";
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
    // Builtin Commands
    if (auto search = builtins.find(command); search != builtins.end()) {
      std::string out = search->second(args);
      if (out != "")
        std::cout << out << '\n';
    }

    // Executable in PATH
    else if (std::string executable = find_exec_in_PATH(command); executable != "") {
      exec_cmd(args, command, executable);
    }

    else
      std::cout << command << ": command not found\n";
  }
  
}
