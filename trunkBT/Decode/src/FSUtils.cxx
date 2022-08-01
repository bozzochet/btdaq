//
// Created by Valerio Formato on 12/10/21.
//

#include "FSUtils.hh"
#include <algorithm>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace FSUtils {
int IsRegularFile(const std::string &path) { return IsRegularFile(path.c_str()); }

int IsRegularFile(const char *path) {
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISREG(path_stat.st_mode);
}

std::vector<std::string> ListDir(std::string path) {
  std::vector<std::string> result;

  if(path.back() != '/')
    path.append("/");

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) != NULL) {
    // print all the files within directory, ignore directories
    while ((ent = readdir(dir)) != NULL) {
      if (FSUtils::IsRegularFile(std::string{path} + std::string{ent->d_name}))
        result.emplace_back(ent->d_name);
    }
    closedir(dir);
  } else {
    perror("could not open directory");
  }

  std::sort(begin(result), end(result));

  return result;
}
} // namespace FSUtils
