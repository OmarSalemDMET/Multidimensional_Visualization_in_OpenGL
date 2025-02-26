

#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>
#include <vector>

// Type aliases for brevity
using VecInt = std::vector<int>;
using VecFloat = std::vector<float>;
using DataVec = std::vector<std::pair<float, VecInt>>;

// Global constants
const int WIN_W = 800; // Window width
const int WIN_H = 600; // Window height
const int PNT_SZ = 15; // Point size
const int STP_SZ = 20; // Step size for spiral

// Global variables (minimized usage)
DataVec pts; // Data points (immutable after initialization)
VecInt qry;  // Query vector (immutable after initialization)

int win_w = WIN_W; // Current window width
int win_h = WIN_H; // Current window height

float min_rel = std::numeric_limits<float>::max();
float max_rel = std::numeric_limits<float>::lowest();
int min_val = std::numeric_limits<int>::max();
int max_val = std::numeric_limits<int>::lowest();

// Function prototypes
bool load_data(const std::string &filename, DataVec &data, int &minV,
               int &maxV);
VecInt prompt_query();
DataVec calculate_distances(const DataVec &data, const VecInt &query,
                            float &minR, float &maxR);
DataVec sort_data(const DataVec &data);
void display_data(const DataVec &data);
void run_visualization(int argc, char **argv, const DataVec &data);
void display();
void reshape(int w, int h);
void draw_sub_window(int index, int subWinW, int subWinH, const DataVec &data);

int main(int argc, char **argv) {
  // Load data
  if (!load_data("Points5D.txt", pts, min_val, max_val)) {
    return -1;
  }

  // Prompt for query vector
  qry = prompt_query();

  // Calculate distances (relevance)
  pts = calculate_distances(pts, qry, min_rel, max_rel);

  // Sort data
  pts = sort_data(pts);

  // Display data
  display_data(pts);

  // Run visualization
  run_visualization(argc, argv, pts);

  return 0;
}

bool load_data(const std::string &filename, DataVec &data, int &minV,
               int &maxV) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    std::cerr << "Error: Invalid or empty input file.\n";
    return false;
  }

  // Read all integers from the file
  std::vector<int> all_values((std::istream_iterator<int>(infile)),
                              std::istream_iterator<int>());

  if (all_values.empty() || all_values.size() % 5 != 0) {
    std::cerr << "Error: Input file is empty or improperly formatted.\n";
    return false;
  }

  // Process the integers in groups of five
  for (size_t i = 0; i < all_values.size(); i += 5) {
    VecInt vals(all_values.begin() + i, all_values.begin() + i + 5);

    // Update min and max values
    auto [minElem, maxElem] = std::minmax_element(vals.begin(), vals.end());
    minV = std::min(minV, *minElem);
    maxV = std::max(maxV, *maxElem);

    data.emplace_back(0.0f, vals);
  }

  return true;
}

VecInt prompt_query() {
  VecInt query(5);
  std::cout
      << "Enter a 5-dimensional query vector (space-separated integers): ";
  std::cin >> query[0] >> query[1] >> query[2] >> query[3] >> query[4];
  return query;
}

DataVec calculate_distances(const DataVec &data, const VecInt &query,
                            float &minR, float &maxR) {
  // Map data to calculate distances
  DataVec result;
  result.reserve(data.size());

  for (const auto &item : data) {
    const VecInt &vals = item.second;
    // Compute squared differences
    float sum = std::inner_product(
        vals.begin(), vals.end(), query.begin(), 0.0f, std::plus<>(),
        [](int a, int b) { return (a - b) * (a - b); });
    // Apply logarithmic scaling
    float relevance = std::log(sum + 1.0f);

    // Update min and max relevance
    minR = std::min(minR, relevance);
    maxR = std::max(maxR, relevance);

    result.emplace_back(relevance, vals);
  }

  return result;
}

DataVec sort_data(const DataVec &data) {
  // Create a copy to sort
  DataVec sortedData = data;
  std::sort(sortedData.begin(), sortedData.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
  return sortedData;
}

void display_data(const DataVec &data) {
  std::cout << "\nSorted Data with Relevance Points (Approximate Distances):\n";
  for (const auto &item : data) {
    std::cout << item.first << " ";
    for (const auto &val : item.second) {
      std::cout << val << " ";
    }
    std::cout << "\n";
  }
}

void run_visualization(int argc, char **argv, const DataVec &data) {
  // Initialize OpenGL and GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(win_w, win_h);
  glutCreateWindow(
      "5D Data Visualization using Spiral Technique (Functional Version)");

  // Set up display and reshape functions
  glutDisplayFunc([]() { display(); });
  glutReshapeFunc([](int w, int h) { reshape(w, h); });

  // Set up clear color (white background)
  glClearColor(1.0, 1.0, 1.0, 1.0);

  // Enter the GLUT event loop
  glutMainLoop();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Divide the screen into 6 sub-windows
  for (int i = 0; i < 6; ++i) {
    // Calculate viewport dimensions
    int subWinW = win_w / 3;
    int subWinH = win_h / 2;
    int viewportX = (i % 3) * subWinW;
    int viewportY = (i / 3) * subWinH;

    glViewport(viewportX, viewportY, subWinW, subWinH);
    draw_sub_window(i, subWinW, subWinH, pts);
  }

  glutSwapBuffers();
}

void reshape(int w, int h) {
  win_w = w;
  win_h = h;
  glViewport(0, 0, w, h);
  glutPostRedisplay(); // Redraw the scene with the new window size
}

void draw_sub_window(int index, int subWinW, int subWinH, const DataVec &data) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, subWinW, 0, subWinH);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Starting point for spiral (center of the sub-window)
  float x = subWinW / 2.0f;
  float y = subWinH / 2.0f;

  // Adjust step sizes to create a rectangular spiral
  float x_step = STP_SZ * (subWinW / static_cast<float>(WIN_W));
  float y_step = STP_SZ * (subWinH / static_cast<float>(WIN_H));

  int dir = 0; // Direction: 0:right, 1:up, 2:left, 3:down
  int steps_in_leg = 1;
  int leg_count = 0; // Counts how many steps we've taken in the current leg

  glPointSize(PNT_SZ);

  for (const auto &item : data) {
    float intensity;
    if (index == 0) {
      // Relevance Point
      intensity = (item.first - min_rel) / (max_rel - min_rel);
      intensity =
          1.0f -
          intensity; // Invert intensity so that closer points are brighter
    } else {
      // Other dimensions
      int value = item.second[index - 1];
      intensity = (value - min_val) / static_cast<float>(max_val - min_val);
    }
    // Clamp intensity
    intensity = std::clamp(intensity, 0.0f, 1.0f);

    glColor3f(intensity, intensity, intensity);

    // Draw point
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();

    // Move to next position in rectangular spiral
    switch (dir) {
    case 0:
      x += x_step;
      break; // Right
    case 1:
      y += y_step;
      break; // Up
    case 2:
      x -= x_step;
      break; // Left
    case 3:
      y -= y_step;
      break; // Down
    }

    leg_count++;
    if (leg_count == steps_in_leg) {
      leg_count = 0;
      dir = (dir + 1) % 4;
      // Increase steps_in_leg after moving left or down
      if (dir == 0 || dir == 2) {
        steps_in_leg++;
      }
    }

    // Ensure x and y stay within the sub-window boundaries
    x = std::clamp(x, 0.0f, static_cast<float>(subWinW));
    y = std::clamp(y, 0.0f, static_cast<float>(subWinH));
  }
}
