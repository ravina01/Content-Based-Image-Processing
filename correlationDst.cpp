/* Extension Task : HS Histogram 2D with Correlation as distance
    this code i extension of extension 2 task
    To Run this file : add this file name in exexutable along with calculation.cpp 
*/
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <dirent.h>
#include <limits>
#include "calculation.h"

using namespace cv;
using namespace std;

const int Hsize_2D = 16;

int main(int argc, char** argv)
{
    char dirname[256];
    char buffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int i;
  
    cv::Mat input_img;
    cv::Mat src;
    
    src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0030.jpg", cv::IMREAD_COLOR);
    

    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);

    cvtColor(src, src, cv::COLOR_BGR2HSV);
    
    int *hist_2d_target = new int [Hsize_2D * Hsize_2D];

    for(int i=0; i < (Hsize_2D * Hsize_2D); i++)
        hist_2d_target[i] = 0;
    
    calHSHistogram(src, hist_2d_target,0, src.rows, 0, src.cols);

    vector<float> normalized_target;
    int size = Hsize_2D * Hsize_2D;

    normalizeHist(hist_2d_target, size, normalized_target);

    float mean_target;
    mean_target = 1.0/normalized_target.size();

    if( argc < 2) {
        printf("usage: %s <directory path>\n", argv[0]);
        exit(-1);
    }
    strcpy(dirname, argv[1]);
    printf("Processing directory %s\n", dirname );
    
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    

    std::vector<pair<float, string>> corrSortVec;

    while( (dp = readdir(dirp)) != NULL ) {
        
        if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {

                //printf("processing image file: %s\n", dp->d_name);
                strcpy(buffer, dirname);
                strcat(buffer, "/");
                strcat(buffer, dp->d_name);
                //printf("full path name: %s\n", buffer);

                float intersectionDist;
                float chiDist;

                std::vector<int> input_feature;

                input_img = cv::imread(buffer);
                cvtColor(input_img, input_img, cv::COLOR_BGR2HSV);

                int *hist_2d_input = new int [Hsize_2D * Hsize_2D];

                //initialize to all zeros
                for(int i=0; i < (Hsize_2D * Hsize_2D); i++)
                    hist_2d_input[i] = 0;

                calHSHistogram(input_img, hist_2d_input, 0, input_img.rows, 0, input_img.cols);

                vector<float> normalized_input;
                normalizeHist(hist_2d_input, size, normalized_input);

                float mean_input;
                mean_input = 1.0/normalized_input.size();
                
                float corrDist;

                correlationDst(normalized_target, normalized_input, mean_target, mean_input, corrDist);
                

                delete hist_2d_input;

                corrSortVec.push_back(make_pair(corrDist,buffer));
               
        }
    }
    delete hist_2d_target;

    //sorted all distances
    sort(corrSortVec.begin(), corrSortVec.end());
    

    //Top 3 results for Correlation Distance Metric
    for( int i = 1; i < 4; i++ ) {
        cout<<corrSortVec[i].first << " "<<corrSortVec[i].second<<endl;
        imshow(to_string(i), imread(corrSortVec[i].second));
    }
    
    char key = cv::waitKey(0);
    while (1)
    {
        if(key == 'q'){
            break;
        }
    }

}


