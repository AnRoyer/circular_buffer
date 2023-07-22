#include "circular_buffer.hpp"

#include <iostream>
#include <vector>

class A
{
public:
  A() {}
  A(const A&) = default;
  A(A&&) = default;
  
  A &operator=(const A&) { return *this; }
};

template class anr::circular_buffer<A>;
template class std::vector<A>;


int main(int argc, char **argv)
{
  anr::circular_buffer<int> buffer;
    
  buffer.reserve(5);

  for(auto i = 0; i < 4; ++i) {
    buffer.push_back(i);
  }
  
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  std::cout << "Buffer capacity: " <<  buffer.capacity() << std::endl;

  std::cout << "Buffer values: ";
  for(auto it : buffer) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
  
  buffer.clear();
  
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  std::cout << "Buffer capacity: " <<  buffer.capacity() << std::endl;
  
  buffer.resize(10, 2);
  
  std::cout << "Buffer values: ";
  for(auto it : buffer) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
  
  return 0;
}
