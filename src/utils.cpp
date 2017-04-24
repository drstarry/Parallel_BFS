// Rui Dai
// Sam Olds
// CS 6230
// 4/8/17
//
// Parallel BFS

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

// from http://stackoverflow.com/a/236803
template<typename Out>
void split(const std::string &s, char delim, Out result) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}


std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}
