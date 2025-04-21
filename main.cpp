#include "Controller.h"
#include "Model.h"
#include "View.h"
#include <glad/glad.h>
#include <iostream>

struct Config {
  string fileInput;
  bool textRender = false;
};

bool parseArguments(int argc, char *argv[], Config &config) {
  if (argc > 2) {
    cout << "Unknown arguments supplied. " << argc << " arguments provided.\n";
    return false;
  }

  vector<string> args(argv + 1, argv + argc);

  // remaining argument should be scenegraph location
  if (args.size() > 1) {
    cout << "Too many arguments provided.\n";
    return false;
  } else if (args.size() == 1) {
    config.fileInput = args[0];
  }

  return true;
}

int main(int argc, char *argv[]) {
  Model model;
  View view;
  Config config;

  if (!parseArguments(argc, argv, config)) {
    cout << "Usage:\n"
         << "  ./assignment7 [\"scenegraph-location\"]\n";
    return 1;
  }

  Controller controller(model, view, config.fileInput, config.textRender);
  controller.run();
}
