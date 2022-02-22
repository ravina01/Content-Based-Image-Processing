/* Extension Task : HS Histogram 2D 
    Distance metric : Histogram Intersection and Chi-sqaure to compare
    correlationDst.cpp calculates correlation Distance on same image. But implemented in other file to experiment of different data set
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
    
    //Histogram Intersection Dist Vector
    std::vector<pair<float, string>> histSortVec;

    //Chi-Square Dist Vector
    std::vector<pair<float, string>> chiSortVec;

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
                                
                dstMetricHistIntersection(normalized_target, normalized_input, intersectionDist);
                chiSquare(normalized_target, normalized_input, chiDist);

                delete hist_2d_input;

                histSortVec.push_back(make_pair(intersectionDist,buffer));
                chiSortVec.push_back(make_pair(chiDist,buffer));
        }
    }
    delete hist_2d_target;

    //sorted all distances
    sort(histSortVec.begin(),histSortVec.end());
    sort(chiSortVec.begin(),chiSortVec.end());


    //Top 3 results for Intersection Distance Metric
    for( int i = 1; i < 4; i++ ) {
        cout<<histSortVec[i].first << " "<<histSortVec[i].second<<endl;
        imshow(to_string(i), imread(histSortVec[i].second));
    }

    //Top 3 results for Chi-Square Distance Metric
    for( int i = 1; i < 4; i++ ) {
        cout<<chiSortVec[i].first << " "<<chiSortVec[i].second<<endl;
        imshow(to_string(i), imread(chiSortVec[i].second));
    }
    
    char key = cv::waitKey(0);
    while (1)
    {
        if(key == 'q'){
            break;
        }
    }

}


