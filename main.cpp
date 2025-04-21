#include "Controller.h"
#include "Model.h"
#include "View.h"
#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// @TODO
// fix text renderer
// adjust opengl renderer (mouse movement, default scenes)
// fix ray tracing reflections

/// Represents configuration settings for the application.
struct Config {
  string fileInput;        ///< File input path for the scenegraph location.
  bool textRender = false; ///< Flag to toggle text rendering mode.
};

/// Parses command-line arguments and populates the configuration.
/// @param argc Number of command-line arguments.
/// @param argv Array of command-line arguments.
/// @param config Reference to the Config object that will be updated.
/// @return true if the arguments are valid, false otherwise.
bool parseArguments(int argc, char *argv[], Config &config) {
  if (argc > 2) {
    cout << "Unknown arguments supplied. " << argc << " arguments provided.\n";
    return false;
  }

  vector<string> args(argv + 1, argv + argc);

  // The remaining argument should be the scenegraph location.
  if (args.size() > 1) {
    cout << "Too many arguments provided.\n";
    return false;
  } else if (args.size() == 1) {
    config.fileInput = args[0];
  }

  return true;
}

/// Entry point for the application.
/// @param argc Number of command-line arguments.
/// @param argv Array of command-line arguments.
/// @return Exit status, where 0 indicates success.
int main(int argc, char *argv[]) {
  // Create model and view instances (definitions assumed to exist elsewhere).
  Model model;
  View view;

  // Initialize configuration.
  Config config;

  // Parse command-line arguments.
  if (!parseArguments(argc, argv, config)) {
    cout << "Usage:\n"
         << "  ./assignment7 [\"scenegraph-location\"]\n";
    return 1;
  }

  // Instantiate the controller with model, view, file input, and text rendering
  // settings.
  Controller controller(model, view, config.fileInput, config.textRender);

  // Run the main application loop.
  controller.run();

  return 0;
}
