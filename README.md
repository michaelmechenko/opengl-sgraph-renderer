# Ray Tracer Application

This application is a ray tracer that can render scenes defined using a scene graph. It supports basic shading, shadows, reflections, and texture mapping for spheres and boxes. It also includes a text-based scene graph renderer for debugging and visualization.

## Features

- **Scene Graph Structure**  
  The scene is described using a hierarchical scene graph that supports:

  - **Group Nodes:** For logical grouping of child nodes.
  - **Leaf Nodes:** Contain drawable geometry and material properties.
  - **Transform Nodes:** Support scale, translation, and rotation transforms.
  - **Animation:** Global animation transforms can be applied to the whole scene.
  - **Command Language:** Define scene structure and attributes (e.g., "translate", "group", "leaf", etc.).

- **Shading and Lighting**  
  The ray tracer performs simple shading using ambient, diffuse, and specular components. Additional features include:

  - **Shadows:** Cast shadow rays from intersection points to light sources.
  - **Reflections:** Recursive ray casting to compute reflective contributions (controlled via bounce count).
  - **Material Properties:** Supports absorption, reflection, transparency, and refractive index. Note that for each material, absorption + reflection + transparency should equal 1.

- **Texture Mapping**  
  Texture mapping is supported (for spheres and boxes) using texture coordinates that are computed during intersections.

- **Rendering Modes**
  - **Interactive OpenGL Rendering:** Uses OpenGL shaders and a modelview stack to render the scene graph visually.
  - **Offline Ray Tracing:** Generates a PPM image by casting rays from the camera through each pixel, applying shading and reflections recursively.

## File Structure

- **Core Application Files:**

  - `main.cpp`: Entry point that parses command line arguments, initializes the controller, model, and view.
  - `Controller.cpp` / `Controller.h`: Handles user input, window events, and scene graph loading.
  - `Model.cpp` / `Model.h`: Contains the scene graph, mesh data, and texture paths.
  - `View.cpp` / `View.h`: Manages OpenGL context, shader programs, camera control, and rendering (both OpenGL and ray tracing).

- **Scene Graph Components (`sgraph` folder):**

  - Abstract and concrete node classes: `AbstractSGNode.h`, `GroupNode.h`, `LeafNode.h`, `TransformNode.h`.
  - Transformations: `ScaleTransform.h`, `TranslateTransform.h`, `RotateTransform.h`.
  - Visitors: Renderers (`GLScenegraphRenderer.h`, `TextScenegraphRenderer.h`, `RaycastScenegraphRenderer.h`), `AnimationVisitor.h`, and visitor interfaces.
  - Scenegraph management: `Scenegraph.h`, `IScenegraph.h`.

- **Utilities:**

  - Mesh and vertex attribute handling: `PolygonMesh.h`, `VertexAttrib.h`.
  - Material and lighting classes: `Material.h`, `Light.h`.
  - Image loaders: `PPMImageLoader.h`, `ImageLoader.h`.
  - Ray casting pipeline: `Rays.h`.

- **Scene Graph Command Files:**
  - Example scene graph files (e.g., `scenegraph.txt`, `box.txt`, etc.) provide instructions for constructing the scene.

## Compilation and Execution

### Building the Application

1. Clean and compile the code by running:
   ```
   make clean && make
   ```

### Running the Application

2. Run the executable with an optional scene graph file argument:
   ```
   ./main <scenegraph.txt>
   ```
   If no file is provided, a default scene graph (e.g., `scenegraphmodels/spheres_on_opposite_sides_of_wall.txt`) is used.

### Rendering Options

- **Interactive Mode (OpenGL):**  
  The application opens a window where users can view the scene rendered using OpenGL. Camera control is available using mouse movements and keyboard inputs (e.g., 'R' to reset the camera).

- **Ray Traced Output:**  
  Press the designated key (which sets a flag) to output a ray traced image. The rendered image is saved as `output.ppm` in the working directory.

## Scene Graph Command Language

The scene graph is defined using a simple command language where each line represents an instruction. Commands include:

- `instance <name> <meshFile>`: Load a mesh from the specified file.
- `group <varname> <nodeName>`: Create a new group node.
- `leaf <varname> <nodeName> instanceof <instanceName>`: Create a leaf node referencing an object instance.
- `translate <varname> <nodeName> <tx> <ty> <tz>`: Apply translation.
- `scale <varname> <nodeName> <sx> <sy> <sz>`: Apply scaling.
- `rotate <varname> <nodeName> <angleInDegrees> <ax> <ay> <az>`: Apply rotation.
- Additional commands to assign materials, lights, textures, add children, and import external graphs.

Comments (lines beginning with `#`) are ignored during parsing.

## Customization

You can adjust various parameters in the code:

- **Maximum Bounce for Reflections:**  
  Set in the `RaycastScenegraphRenderer` class (`maxBounce` parameter).
- **Camera Settings:**  
  Initial camera angles and positions can be modified in `View.cpp`.
- **Material Properties:**  
  Materials include ambient, diffuse, specular, emission, shininess, absorption, reflection, and transparency. Ensure the sum of absorption, reflection, and transparency equals 1.

## Dependencies

- **GLFW & GLAD:** For windowing and OpenGL context.
- **GLM:** For mathematical operations and transformations.
- **Standard C++ Libraries:** For file I/O, memory management, and STL containers.
