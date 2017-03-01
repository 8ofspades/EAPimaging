#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "Image.h"				 

using namespace cv;
using namespace std;

//    Constructor
Image::Image(string imgPath, string Title){
  Mat file = imread(imgPath,0);
  img = file;
  bin = file;
  title = Title;
}

//Displays selected image
void Image::display(string type){
  //  namedWindow(title, WINDOW_AUTOSIZE);
  if (type.compare("original") == 0){
    imshow("title",img);
    cvWaitKey(30); //wait for user input
  }
  if (type.compare("binary") == 0){
    imshow("binary",bin);
    cvWaitKey(30);
  }
  if (type.compare("edges") == 0){
    imshow("edges",color_bin);
    cvWaitKey(30);
  }
  if (type.compare("contours") == 0){
    imshow("contours",edges);
    cvWaitKey(30);
  }
}

//Sets a threshold of v (between 0 and 255) and creates a binary image
//Also creates a binary image with colour channels for colored markers
void Image::thresh(int v){
  threshold(img, bin, v, 255, CV_8UC1);
  cvtColor(bin,color_bin,CV_GRAY2BGR);
}

//Crops an image to a squarecentered at pnt with sides of length size
void Image::crop(Point pnt, int size){
  float x = pnt.x - size/2;
  float y = pnt.y - size/2;
  Mat roi = img( Rect(x,y,size, size) );
  img = roi;
}

//Implements a blob detector to identify the eap from shadows in the image, filters out random noise and extra dark bits that are not part of the EAP
//Returns the center of the EAP
vector<Point> Image::blob(float min, float max){
  SimpleBlobDetector::Params params;
  //set blob detection parameters, focuses on the high contrast between the black EAP and the white background
  params.minDistBetweenBlobs = 100.0;
  params.filterByInertia = false;
  params.filterByConvexity = false;
  params.filterByColor = true;
  params.filterByCircularity = false;
  params.filterByArea = true;
  params.minArea = min;
  params.maxArea = max;
  params.blobColor = 0;

  //run block detection algorithm
  SimpleBlobDetector blob_detector(params);

  //add xy points found by the blob detection algorithm in a sorted list coords
  vector<KeyPoint> key;
  vector<Point> coords;
  blob_detector.detect(img, key);
  
  for (int i = 0; i<key.size(); i++){
    coords.push_back(key[i].pt);
  }

  return coords;
}

//Draw a circle on the image for a selected point and radius
void Image::drawCircle(Point pnt,int r){
  circle(img, pnt, r, Scalar(120, 130, 20), 2, 8);
}

//Implements canny algorithm to find edge points
void Image::cannyDetect(int thr1, int thr2, int aptSz){
  Canny(bin,edges,thr1,thr2,aptSz); //find edges
  Mat map = bin.clone();
  map = Scalar::all(255);
  bin.copyTo(map, edges);
  imshow("map",map); //display black edges on a white background
  cvWaitKey(30);
}

//hough detection algorthim for lines in an image, simpler approximation to find the rectangular bounds of the EAP, used for troubleshooting
void Image::houghDetect(double rho, double theta, int thr, int minLength, int maxGap){
  Canny(bin,bin,0,1000,3,true);
  HoughLinesP(bin,lines,rho,theta,thr,minLength,maxGap); 
  for (size_t i = 0; i < lines.size(); i++){
    line(color_bin, Point(lines[i][0], lines[i][1]), Point(lines[i][2],lines[i][3]), Scalar(0,0,255),1,8); //draw lines on the image
  }
}

//Locates extreme edge contours of the EAP
//Level is a threshold which removes small holes in the center of the image
void Image::edgeFind(int level){
  vector<vector<Point>> test(1);
  Canny(bin,bin,5000,17500,5);
  findContours(bin, contours, order, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //use simple chain approximation to find contours in the image
  edges = color_bin.clone();
  drawContours(edges,contours,-1, Scalar(0,0,255),2,8);
  for (int i = 0; i < contours.size(); i++){
    int contSize = contours[i].size();
    //filter out all contours that fall below the input level
    if (contSize >= level){
      contour.insert(contour.end(), contours[i].begin(), contours[i].end());
      drawContours(color_bin, contours, i, Scalar(0, 0, 255), 4, 8);
    }
  }
}

//draws a box from the most extreme corners of the EAP and turns it into 4 OpenCV contours
void Image::cornerFind(){
  //  RotatedRect bounds = minAreaRect(contour);
  Rect bounds = boundingRect(contour);
  Point2f boxCorners[4];
  Point2f eapCorners[4];
  vector<Point> side;
  vector<double> bAvg = {0, 0, 1000, 1000};
  bounds.points(boxCorners); //find corners of the bounding rectangle
  for( int i = 0; i < 4; i++ ){
    eapCorners[i] = closestPoint(boxCorners[i]); //find closest point of the EAP to the 4 corners
  }

  for( int i = 0; i < 4; i++ ){
    vector<vector<Point>>::iterator it = sides.end();
    vector<vector<Point>>::iterator it2 = realSides.end();
    line(color_bin, boxCorners[i], boxCorners[(i + 1) % 4], Scalar(0, 255, 0)); //Draw lines of bounding box
    line(color_bin, eapCorners[i], eapCorners[(i + 1) % 4], Scalar(0, 0, 255)); //draw lines of the actual EAP corners
    side = createContour(eapCorners[i], eapCorners[(i + 1) % 4]);
    sides.insert(it, side);
    realSides.insert(it2, side);
    realSides[i].clear();
  }

  /*for (int i = 0; i < sides.size(); i++){
    int r1 = rand() % 255;
    int r2 = rand() % 255;
    int r3 = rand() % 255;
    drawContours(color_bin,sides,i, Scalar(r1,r2,r3),3,8);
    }*/
}

//Finds the closest point on the EAP contour to point corner
Point Image::closestPoint(Point corner){
  double min = 100;
  int index;
  for ( int i = 0; i < contour.size(); i++ ){
    double dif = norm(corner-contour[i]); //norm of the contour and EAP points
    if ( dif < min ){
      min = dif; //store smallest difference
      index = i; //index of smallest difference location
    }
  }
  return contour[index];
}

//Creates a contour from 2 points (vector of two points)
vector<Point> Image::createContour(Point start, Point end){
  vector<Point> cnt;
  cnt.push_back(start);
  cnt.push_back(end);
  return cnt;
}

//Splits the sinlge EAP contour into distinct contours for each side
void Image::splitContour(){
  map<double,int> order;
  // vector<vector<Point>> realSides(4);
  for (int i = 0; i<contour.size(); i++){
    // cut off one side of the contour and place it in its own sorted map
    for (int j = 0; j<4; j++){
      order[abs(pointPolygonTest(sides[j], contour[i], true))] = j;
    }
    realSides[order.begin()->second].push_back(contour[i]); //re-order new contour
    order.clear();
  }

  //Draw contours with 4 disctinct colors to show contour seperation
  for (int i = 0; i < realSides.size(); i++){
    int r1 = rand() % 255;
    int r2 = rand() % 255;
    int r3 = rand() % 255;
    drawContours(color_bin, realSides, i, Scalar(r1, r2, r3), 2, 8);
  }
}

//Compute strains by taking the difference of changing side contours over time
vector<double> Image::findStrains(){
  map<double,int> x;
  map<double,int> y;
  double xDist;
  double yDist;
  double Avg[4];
  double avg;
  //compute avg x & y coordinates for each side
  for (int i = 0; i < 4; i++){
    x[findAvg(&realSides[i], 'x')] = i;
    y[findAvg(&realSides[i], 'y')] = i;
  }
  //  cout << "xmin = " << x.rbegin()->first << ", xmax = " << x.begin()->first << endl;
  //  cout << "ymin = " << y.rbegin()->first << ", ymax = " << y.begin()->first << endl;
  //compute difference between average sides accross two images
  xDist = abs(x.begin()->first - x.rbegin()->first);
  yDist = abs(y.begin()->first - y.rbegin()->first);
  vector<double> v {xDist, yDist};
  return v;
}

//Find the average x or y location for a contour
double Image::findAvg(vector<Point>* pnts, char dim){
  double avg = 0;
  //  cout << "# of pnts = " << pnts->size() << endl;
  if (dim == 'x'){
    for ( int i = 0; i < pnts->size(); i++){
      avg = avg + (*pnts)[i].x;
    }
  }
  if (dim == 'y'){
    for ( int i = 0; i < pnts->size(); i++){
      avg = avg + (*pnts)[i].y;
    }
  }
  return avg/pnts->size();
}
  
