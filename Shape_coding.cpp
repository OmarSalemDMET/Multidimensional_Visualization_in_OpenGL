//omar Abdelhamid Yehia El-Adly
//55-7975

#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits> // Added to use std::numeric_limits
#include <memory>
#include <vector>

// Define M_PI if not defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Implement make_unique for C++11
#if __cplusplus <= 201103L
namespace std {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&...args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace std
#endif

// Struct to represent a data point
struct dat_pt {
  float attrs[6]; // Six attributes representing grayscale values
};

// Class to manage data operations
class dat_mgr {
public:
  std::vector<dat_pt> dpts; // Vector of data points
  float min_vals[6];        // Minimum values for each attribute
  float max_vals[6];        // Maximum values for each attribute

  // Constructor initializes min and max values
  dat_mgr() {
    for (int i = 0; i < 6; ++i) {
      min_vals[i] = std::numeric_limits<float>::max();
      max_vals[i] = std::numeric_limits<float>::lowest();
    }
  }

  // Load data from file and apply scaling
  void load_dat(const char *fname, float scl_fac) {
    std::ifstream file(fname);
    if (!file.is_open()) {
      std::cerr << "Error: Unable to open file " << fname << std::endl;
      std::exit(EXIT_FAILURE);
    }

    dat_pt pt;
    while (file >> pt.attrs[0] >> pt.attrs[1] >> pt.attrs[2] >> pt.attrs[3] >>
           pt.attrs[4] >> pt.attrs[5]) {
      // Scale the attributes
      for (int i = 0; i < 6; ++i) {
        pt.attrs[i] *= scl_fac;
      }
      dpts.emplace_back(pt);
    }
    file.close();
  }

  // Calculate min and max for each attribute
  void calc_min_max() {
    for (const auto &pt : dpts) {
      for (int i = 0; i < 6; ++i) {
        if (pt.attrs[i] < min_vals[i])
          min_vals[i] = pt.attrs[i];
        if (pt.attrs[i] > max_vals[i])
          max_vals[i] = pt.attrs[i];
      }
    }
  }

  // Normalize a value between 0 and 255 based on min and max
  float norm_val(int attr, float val) const {
    if (max_vals[attr] - min_vals[attr] == 0.0f)
      return 0.0f;
    float norm = (val - min_vals[attr]) / (max_vals[attr] - min_vals[attr]);
    // Clamp the value to [0, 255]
    norm = std::min(std::max(norm, 0.0f), 1.0f);
    return norm * 255.0f;
  }
};

// Class to handle image creation and writing
class img_writer {
public:
  const int img_w;                     // Image width in pixels (300)
  const int img_h;                     // Image height in pixels (200)
  std::vector<unsigned char> img_data; // RGB pixel data

  // Constructor initializes image buffer
  img_writer(int width, int height)
      : img_w(width), img_h(height), img_data(width * height * 3, 255) {
  } // Initialize to white

  // Set a pixel's grayscale value
  void set_pixel(int x, int y, unsigned char gray) {
    if (x < 0 || x >= img_w || y < 0 || y >= img_h)
      return;
    int idx = (y * img_w + x) * 3;
    img_data[idx] = gray;     // Red
    img_data[idx + 1] = gray; // Green
    img_data[idx + 2] = gray; // Blue
  }

  // Map data points to image pixels
  void map_data_to_image(const dat_mgr &dm, const int grd_sz,
                         const float cel_sz) {
    for (int i = 0; i < grd_sz; ++i) {
      for (int j = 0; j < grd_sz; ++j) {
        int idx = i * grd_sz + j;
        if (idx >= dm.dpts.size())
          continue;
        const dat_pt &pt = dm.dpts[idx];
        // Calculate the top-left pixel of the 3x2 block
        float x_base = j * cel_sz;
        float y_base = i * cel_sz;
        int x_start = static_cast<int>(x_base);
        int y_start = static_cast<int>(y_base);
        // Assign each attribute to a pixel in the 3x2 block
        // Top row: attrs[0], attrs[1], attrs[2]
        // Bottom row: attrs[3], attrs[4], attrs[5]
        for (int a = 0; a < 6; ++a) {
          int dx = a % 3;
          int dy = a / 3;
          int x = x_start + dx;
          int y = y_start + dy;
          unsigned char gray =
              static_cast<unsigned char>(dm.norm_val(a, pt.attrs[a]));
          set_pixel(x, y, gray);
        }
      }
    }
  }

  // Write the image to a PPM file
  void write_ppm(const char *fname) const {
    std::ofstream ofs(fname, std::ios::binary);
    if (!ofs) {
      std::cerr << "Error: Unable to create file " << fname << std::endl;
      std::exit(EXIT_FAILURE);
    }
    // Write PPM header (binary P6 format)
    ofs << "P6\n" << img_w << " " << img_h << "\n255\n";
    // Write pixel data
    ofs.write(reinterpret_cast<const char *>(img_data.data()), img_data.size());
    ofs.close();
    std::cout << "Image saved to " << fname << std::endl;
  }
};

// Global variables for OpenGL
std::unique_ptr<dat_mgr> g_dat_mgr;
std::unique_ptr<img_writer> g_img_writer;

// OpenGL image dimensions
int g_img_width = 300;
int g_img_height = 200;

// OpenGL display function
void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Set pixel storage mode
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Draw the pixels
  glDrawPixels(g_img_width, g_img_height, GL_RGB, GL_UNSIGNED_BYTE,
               g_img_writer->img_data.data());

  glutSwapBuffers();
}

// OpenGL reshape function to handle window resizing
void reshape(int width, int height) {
  glViewport(0, 0, width, height);

  // Set up orthographic projection to match window size
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, g_img_width, 0, g_img_height);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Class to encapsulate the application
class app {
public:
  dat_mgr dm;
  img_writer iw;

  // Constructor loads data and initializes image writer
  app(const char *fname, float scl_fac = 0.05f, int grd_sz = 100,
      float cel_sz = 3.0f)
      : iw(static_cast<int>(grd_sz * 3.0f),
           static_cast<int>(grd_sz * 2.0f)) { // 100*3=300, 100*2=200
    dm.load_dat(fname, scl_fac);
    dm.calc_min_max();
    iw.map_data_to_image(dm, grd_sz, cel_sz);
  }

  // Initialize OpenGL (assign to global variables)
  void init_opengl() {
    g_dat_mgr = std::make_unique<dat_mgr>(std::move(dm));
    g_img_writer = std::make_unique<img_writer>(std::move(iw));
    g_img_width = g_img_writer->img_w;
    g_img_height = g_img_writer->img_h;
  }

  // Run the application with OpenGL display
  void run_opengl(int argc, char **argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_img_width, g_img_height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("6D Data Visualization");

    // Initialize OpenGL state
    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background

    // Set up callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // Enter the main loop
    glutMainLoop();
  }

  // Run the application and write the image
  void run(const char *out_fname) { iw.write_ppm(out_fname); }
};

// Main function
int main(int argc, char **argv) {
  const char *fname = "./points6D.txt"; // Input data file
  const char *out_fname = "output.ppm"; // Output image file
  app application(fname);

  // Initialize OpenGL-related data
  application.init_opengl();

  // Optionally write the image to a file
  application.run(out_fname);

  // Run OpenGL display (this will block until the window is closed)
  application.run_opengl(argc, argv);

  return 0;
}
