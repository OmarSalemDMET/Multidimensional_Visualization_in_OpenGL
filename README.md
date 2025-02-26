# Multidimensional Data Visualization

This project demonstrates two distinct techniques for visualizing multidimensional data using C++ and OpenGL. It includes implementations for a **Spiral Technique** (for 5-dimensional data) and **Shape Coding Technique** (for 6-dimensional data). The project reads input from text files, processes the data, and displays visual representations using OpenGL.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Code Report](#code-report)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Overview

The project is divided into two visualization techniques:

1. **Spiral Technique:**  
   - Visualizes 5-dimensional data by computing a "relevance" score based on a user-provided query vector.
   - Maps data points onto a rectangular spiral across several sub-windows of an OpenGL display.
   - Uses a logarithmic transformation of the Euclidean distance to compute relevance.

2. **Shape Coding Technique:**  
   - Visualizes 6-dimensional data by mapping each data point to a grayscale image.
   - Each data point is scaled, normalized, and assigned to a specific pixel block in a generated image.
   - The resulting image is saved in PPM format and displayed using OpenGL.

## Features

- **Data Loading:**  
  Reads multidimensional data from input files (`Points5D.txt` for the spiral technique and `points6D.txt` for shape coding).

- **Data Processing:**  
  - **Spiral Technique:**  
    - Prompts for a 5-dimensional query vector.
    - Computes relevance via logarithmic scaling of squared differences.
    - Sorts and displays processed data.
  - **Shape Coding Technique:**  
    - Loads 6-dimensional data and calculates per-dimension minimum and maximum values.
    - Normalizes data and maps attributes to pixels for image creation.

- **Visualization:**  
  - Uses OpenGL and GLUT for rendering visualizations and handling window events.
  - Supports dynamic window resizing and multiple viewports (for the spiral technique).

## Technologies Used

- **Programming Language:** C++
- **Graphics:** OpenGL, GLUT
- **Libraries:**  
  - C++ Standard Library (for I/O, algorithms, etc.)
  - Math Libraries (cmath for mathematical functions)
- **Compiler:** Any C++11 compliant compiler

## Build Instructions

1. **Ensure OpenGL and GLUT are installed on your system.**  
   - Linux: `sudo apt-get install freeglut3-dev`
   - Windows/macOS: Use your preferred development environment setup for OpenGL.

2. **Compile the Spiral Technique Program:**  
   Assuming the source file is `spiral_technique.cpp`:
   ```
   g++ spiral_technique.cpp -o spiral_technique -lGL -lGLU -lglut
   ```

3. **Compile the Shape Coding Program:**  
   Assuming the source file is `shape_coding.cpp`:
   ```
   g++ shape_coding.cpp -o shape_coding -lGL -lGLU -lglut
   ```

## Usage

- **Spiral Technique:**  
  1. Ensure `Points5D.txt` is in the same directory as the executable.
  2. Run the program:
     ```
     ./spiral_technique
     ```
  3. Enter a 5-dimensional query vector when prompted.
  4. The program displays sorted data in the console and opens an OpenGL window showing a spiral visualization of the data.

- **Shape Coding Technique:**  
  1. Ensure `points6D.txt` is available in the working directory.
  2. Run the program:
     ```
     ./shape_coding
     ```
  3. The program loads the data, maps it to a grayscale image, writes the image to `output.ppm`, and displays it using OpenGL.

## File Structure

```
Multidimensional_Visualization/
├── spiral_technique.cpp      # Source code for spiral technique visualization (5D data)
├── shape_coding.cpp          # Source code for shape coding visualization (6D data)
├── Points5D.txt              # Data file for the spiral technique (5D data)
├── points6D.txt              # Data file for shape coding (6D data)
├── output.ppm                # Generated image file from shape coding
├── README.md                 # This file
└── [Additional Files]        # Any required libraries or assets
```

## Code Report

### Spiral Technique (spiral_technique.cpp)
- **Data Loading & Processing:**
  - `load_data()`: Reads integers from `Points5D.txt` in groups of five and stores them as data points.
  - `prompt_query()`: Prompts the user to enter a 5-dimensional query vector.
  - `calculate_distances()`: Computes the squared Euclidean distance between each data point and the query vector, applies logarithmic scaling, and tracks the minimum and maximum relevance.
  - `sort_data()`: Sorts data points by their computed relevance.
  - `display_data()`: Outputs the sorted data to the console.

- **Visualization:**
  - `run_visualization()`: Initializes OpenGL, sets up the display environment, and enters the GLUT main loop.
  - `display()`: Divides the window into six sub-windows and calls `draw_sub_window()` for each.
  - `draw_sub_window()`: Maps data points onto a rectangular spiral. The intensity of each point is determined by its relevance or corresponding attribute value.
  - `reshape()`: Handles window resizing, ensuring the display is updated accordingly.

### Shape Coding Technique (shape_coding.cpp)
- **Data Management:**
  - `dat_mgr` class:  
    - Loads 6-dimensional data from a file.
    - Computes minimum and maximum values for each attribute.
    - Provides a normalization function (`norm_val()`) to map data values to a grayscale range.
  
- **Image Creation:**
  - `img_writer` class:  
    - Manages an RGB image buffer.
    - `set_pixel()`: Sets individual pixel values.
    - `map_data_to_image()`: Maps each data point to a fixed-size cell in the image, with each attribute corresponding to a pixel in a 3x2 block.
    - `write_ppm()`: Writes the image data to a PPM file (`output.ppm`).

- **Application Structure:**
  - `app` class:  
    - Encapsulates the data management and image creation process.
    - Initializes OpenGL-related global variables.
    - Provides functions to run the OpenGL display and write the image file.
  - **OpenGL Functions:**
    - `display()`: Renders the generated image.
    - `reshape()`: Adjusts the viewport and projection for window resizing.

## License

This project is licensed under the MIT License.

## Acknowledgements

- **OpenGL & GLUT:** For the rendering framework.
- **C++ Standard Library:** For data processing and I/O operations.
- Thanks to all contributors and open-source projects that made this visualization project possible.
