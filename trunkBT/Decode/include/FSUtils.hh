//
// Created by Valerio Formato on 12/10/21.
//

#ifndef DECODE_FSUTILS_HH
#define DECODE_FSUTILS_HH

#include <string>
#include <vector>

namespace FSUtils {
int IsRegularFile(const char *path);
int IsRegularFile(const std::string &path);
std::vector<std::string> ListDir(std::string path);
}; // namespace FSUtils

#endif // DECODE_FSUTILS_HH
