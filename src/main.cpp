#include <iostream>
#include <string>
#include <algorithm>
#include <array>

const std::array<std::string, 3> BUILTINS = {"echo", "exit", "type"};


std::string type_cmd(std::string args) {
  // cmd in BUILTINS-array?
  if (std::find(BUILTINS.begin(), BUILTINS.end(), args) != BUILTINS.end())
    return "" + args + " is a shell builtin";
  else
    return "" + args + ": not found";
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
