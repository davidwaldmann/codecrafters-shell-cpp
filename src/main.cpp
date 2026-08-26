#include <iostream>
#include <string>
#include <algorithm>
#include <array>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;
  const std::array<std::string, 3> BUILTINS = {"echo", "exit", "type"};
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, input);
    if (input.substr(0, 4) == "exit")
      break;
    if (input.substr(0, 5) == "type ") {
      // cmd in BUILTINS-array?
      if (std::find(BUILTINS.begin(), BUILTINS.end(), input.substr(5)) != BUILTINS.end())
        std::cout << input.substr(5) << " is a shell builtin" << std::endl;
      else
        std::cout << input.substr(5) << ": not found" << std::endl;
    }
    else if (input.substr(0, 5) == "echo ") {
      std::cout << input.substr(5) << std::endl;
    }
    else
      std::cout << input << ": command not found" << std::endl;
  }
  
}
