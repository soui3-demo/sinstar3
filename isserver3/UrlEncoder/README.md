# UrlEncoder
vc++ c++ cpp UrlEncoder class( UrlEncode / UrlDecode / UTF8UrlEncode / UTF8UrlDecode)

参考了网上一个strcoding的代码,原出处已不可查

----------------

包含头文件
```c++
#include "Encoder.h"
```
代码片段
```c++
Encoder encoder;
std::string str = encoder.UTF8UrlEncode("C++中实现类似php的UTF8和UrlEncode函数");
printf("%s\r\n", str.c_str());

str = encoder.UTF8UrlDecode("C%2B%2B%E4%B8%AD%E5%AE%9E%E7%8E%B0%E7%B1%BB%E4%BC%BCphp%E7%9A%84UTF8%E5%92%8CUrlEncode%E5%87%BD%E6%95%B0");
printf("%s\r\n", str.c_str());

str = encoder.UrlEncode("C++中实现类似php的UTF8和UrlEncode函数");
printf("%s\r\n", str.c_str());

str = encoder.UrlDecode("C%2B%2B%D6%D0%CA%B5%CF%D6%C0%E0%CB%C6php%B5%C4UTF8%BA%CDUrlEncode%BA%AF%CA%FD");
printf("%s\r\n", str.c_str());
```
