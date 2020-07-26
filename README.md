# JSON parser
![C/C++ CI](https://github.com/FloweyTheFlower420/File-parsers/workflows/C/C++%20CI/badge.svg)  
This is a lightweight parser for [JSON](https://en.wikipedia.org/wiki/JSON) formated files, written in [C++](https://en.wikipedia.org/wiki/C%2B%2B). Source code is hosted on [github](https://github.com/FloweyTheFlower420/JSON-parser)
## Build
To build this project on linux, use `make lib` to complile. The libary will be located in `objects/json.a`
## Examples
Currently, we only support deserialization and basic serialization(potenally buggy).  
A basic example is:
```c++
#include "json/json.h"
#include <string>
#include <iostream>
using namespace json;
using namespace std;
int main()
{
  string json_buffer = "{\"Name\": \"Joe\"}";
  // parsing
  JSONparser parser;
  JSONobj obj; // stores value
  
  if(parser.deserialize(json_buffer, obj) != 0)
  {
    // error handling
    return -1;
  }
  std::cout << obj["Name"]; // get the value of 'Name' and print it
}
```
