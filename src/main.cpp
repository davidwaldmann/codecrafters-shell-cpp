#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::string input;
  while(1) {
    std::cout << "$ ";
    std::cin >> input;
    std::cout << input << ": command not found" << std::endl;
    // std::getline(std::cin, input);
  }
  
}
