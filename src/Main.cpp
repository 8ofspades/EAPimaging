#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include "Image.h"

//using namespace std;
using namespace std;
using namespace cv;

namespace fs = boost::filesystem;

char* window_name = "Binary Threshold Selection";
char* trackbar_value = "Value";

Mat src, dst;
int blobMin = 20000;
int blobMax = 50000;
int binThresh = 100;
int cannyMin = 5000;
int cannyMax = 17500;
int contourThresh = 150;

void threshUI( int, void* );

//Display options
int main() {
  cout << "EAP Strain Image Processing\n\n";
  cout << "Enter path to folder you wish to process\n";

  bool flag = false;
  std::string Path;
  
  while (flag == false){
    getline(std::cin, Path);
    fs::path p (Path);
    if (fs::exists(p)){
      if (fs::is_directory(p)){
	flag = true;
      }
      else{
	cout << "Path is not a directory, please tr yagain\n";
      }
    }
    else{
      cout<< "Path does not exist, please try again\n";
    }
  }
  
  //  Path = "/home/mani/School/Research/Soft Robot/Code/ImageProcessing/Video/";
  fs::path p (Path);

  fs::directory_iterator end_itr;

  int counter = 0;
  vector<Point> pnts;
  Point center;
  vector<double> data;

  int choice;

  Image *img;

  while (choice != 0 ){
    cout << "Options\n\n";
    cout << "1. Set thresholding values\n";
    cout << "2. Run image processing algorithms\n";
    cout << "3. View an Image frame\n";
    cout << "0. quit\n";
    cin >> choice;

    if(choice == 1){
      for (fs::directory_iterator itr(p); itr != end_itr; ++itr){
	counter++;
	string path = fs::canonical(itr->path()).string();
	string title = itr->path().filename().string();

	if(counter == 2){
	  img = new Image(path, title);
	  src = img->img;
	  namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	  createTrackbar( trackbar_value, window_name, &binThresh,255, threshUI);

	  threshUI( 0, 0 );
	  cvWaitKey(0);
	  destroyWindow(window_name);
	}
      }
    }

    counter = 0;

    if(choice == 2){
      for (fs::directory_iterator itr(p); itr != end_itr; ++itr){
	string path = fs::canonical(itr->path()).string();
	string title = itr->path().filename().string();
	vector<double> pixDist;

	img = new Image(path, title);

	counter++;

	if (counter == 2){
	  cout << itr->path().string() << endl;
	  pnts = img->blob(20000,50000);
	  /*  center = pnts[0];
	  cout << pnts.size() << " blobs found\n";
	  for (int i = 0; i<pnts.size(); i++){
	    cout << "Blob " << i << " (X,Y)\n";
	    cout << pnts[i].x << ", " << pnts[i].y<< endl;
	    img->drawCircle(pnts[i],300);
	    }*/
	  }

	if (counter >= 2){// && counter%10 == 0){
	  //	  img->crop(center,300);
	  img->display("original");
	  img->thresh(binThresh);
	  img->display("binary");
	  img->cannyDetect(5000,17500,5);
	  //img->houghDetect(1, CV_PI/180,1,val,5);
	  img->edgeFind(220);
	  cout << "t7\n";
	  img->display("contours");
	  img->cornerFind();
	  cout << "t5\n";
	  img->splitContour();
	  cout << "t6\n";
	  img->display("edges");
	  pixDist = img->findStrains();
	  img->display("contours");
	  data.push_back(pixDist[0]);
	  data.push_back(pixDist[1]);
	  cout << "Frame " << counter << ", Xdist = " << pixDist[0] << ", Ydist = " << pixDist[1] << endl;
	}
	delete img;
      }
  
      ofstream output;
      output.open (Path+"strainData.txt");
      output << "xStrain\tyStrain\n";

      for (int i = 0; i < data.size(); i++){ 
	double xStrain = (data[i]-data[0])/data[0];
	double yStrain = (data[i+1]-data[1])/data[1];
	//    cout << xStrain << " " << yStrain << endl;
	output << xStrain << "\t" << yStrain << endl;
	i++;
      }
      output.close(); 
  
    }
  }
}

void threshUI (int, void* ){
  threshold(src, dst, binThresh, 255, CV_8UC1);
  imshow( window_name, dst );
}



      

