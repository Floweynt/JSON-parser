# JSON parser
| Branch  | Status |
| ------------- | ------------- |
| Master  | ![build](https://github.com/FloweyTheFlower420/JSON-parser/workflows/Build%20Test/badge.svg?branch=master)    |
| Dev  | ![build](https://github.com/FloweyTheFlower420/JSON-parser/workflows/Build%20Test/badge.svg?branch=dev)    |
## Build
To build this project on linux, use `make lib` to complile. The libary will be located in `objects/json.a`
## Examples 
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
