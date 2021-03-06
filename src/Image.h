#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

class Image {
 public:
  /***********Variables and Objects************/
  Mat img;
  Mat bin;
  Mat color_bin;
  Mat edges;
  // left, top, right, bottom
  vector<double> boxAvg;
  vector<Vec4i> lines;
  vector<Vec4i> order;
  vector<vector<Point>> contours;
  vector<vector<Point>> sides;
  vector<vector<Point>> realSides;
  vector<Point> contour;
  string title;

  //Constructor
  Image(string imgPath, string Title);

  vector<int> measure(vector<int>);
  vector<Point> blob(float min, float max);
  vector<Point> createContour(Point start, Point end);
  vector<double> findStrains();
  Point closestPoint(Point corner);
  double findAvg(vector<Point>* pnts, char dim);
  int checkSides(double avg);

  void thresh(int v);
  void crop(Point pnt, int size);
  void display(string type);
  void drawCircle(Point pnt, int r);
  void cannyDetect(int threshold1, int threshold2, int aptSz);
  void houghDetect(double rho, double theta, int thr, int minLength, int maxGap);
  void edgeFind(int level);
  void cornerFind();
  void splitContour();

};


