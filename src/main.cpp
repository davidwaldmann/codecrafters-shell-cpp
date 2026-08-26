#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;
  while(1) {
    std::cout << "$ ";
    std::getline(std::cin, input);
    if (input.substr(0, 4) == "exit")
      break;
    if (input.substr(0, 5) == "echo ") {
      std::cout << input.substr(5) << std::endl;
    }
    else
      std::cout << input << ": command not found" << std::endl;
  }
  
}
