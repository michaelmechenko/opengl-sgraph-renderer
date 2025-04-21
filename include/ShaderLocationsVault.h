#ifndef _SHADERLOCATIONSVAULT_H_
#define _SHADERLOCATIONSVAULT_H_

#include <map>
#include <string>
using namespace std;

namespace util {

/*
 * This class represents a table of shader variables
 * and their values. A shader program would put
 * them here, and the rendering code can use them
 * whenever it can
 */
class ShaderLocationsVault {

public:
  ShaderLocationsVault() {}
  ~ShaderLocationsVault() {}

  /*
   * Add a new shader variable and location
   * \param var
   * \param location
   */
  void add(string var, int location) { vars[var] = location; }

  /*
   * Return the location of a shader variable, else
   * return -1
   * \param var the shader variable name whose location is being sought
   * \return the location if found, else -1
   */
  int getLocation(const string &var) const {
    if (vars.count(var) != 0)
      return vars.at(var);
    else
      return -1;
  }

private:
  map<string, int> vars;
};
} // namespace util

#endif
