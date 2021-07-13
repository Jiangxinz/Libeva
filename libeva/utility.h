#ifndef _LIBEVA_UTILITY_H_
#define _LIBEVA_UTILITY_H_

#include <string>

namespace libeva {

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

class StringArg // copyable
{
 public:
  StringArg(const char* str)
    : str_(str)
  { }

  StringArg(const std::string& str)
    : str_(str.c_str())
  { }

  const char* c_str() const { return str_; }

 private:
  const char* str_;
};

}

#endif /* _LIBEVA_UTILITY_H_ */