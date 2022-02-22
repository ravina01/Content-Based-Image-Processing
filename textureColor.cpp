/* Task 4 : Texture Color Matching
    To Run this file : add this file name in exexutable along with calculation.cpp
    function comments written in calculation.cpp
*/
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <dirent.h>
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
  
    cv::Mat src, target_img, input_img;
    cv::Mat result;
    int size = Hsize_2D * Hsize_2D;

    src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0535.jpg", cv::IMREAD_COLOR);

    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);

    //sobel magnitude on target image
    sobelMagnitude(src, result);

    int *hist_rg_target = new int [Hsize_2D * Hsize_2D];
    int *hist_mag_target = new int [Hsize_2D * Hsize_2D];

    for(int i=0; i < (Hsize_2D * Hsize_2D); i++){
        hist_rg_target[i] = 0;
        hist_mag_target[i] = 0;
    }
    
    cal2DHistogram(src, hist_rg_target, 0, src.rows, 0, src.cols);
    cal2DHistogram(result, hist_mag_target, 0, src.rows, 0, src.cols);
    vector<float> normalized_rg_target;
    vector<float> normalized_mag_target;
    normalizeHist(hist_rg_target, size, normalized_rg_target);
    normalizeHist(hist_mag_target, size, normalized_mag_target);
       

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
    
    std::vector<pair<float, string>> histSortVec;

    while( (dp = readdir(dirp)) != NULL ) {
        
        if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {

                strcpy(buffer, dirname);
                strcat(buffer, "/");
                strcat(buffer, dp->d_name);
                //printf("full path name: %s\n", buffer);

                float intersectionDist;
                cv::Mat result_input;
                input_img = cv::imread(buffer);

                int *hist_rg_input = new int [Hsize_2D * Hsize_2D];
                int *hist_mag_input = new int [Hsize_2D * Hsize_2D];

                for(int i=0; i < (Hsize_2D * Hsize_2D); i++){
                    hist_rg_input[i] = 0;
                    hist_mag_input[i] = 0;
                }
                //sobel magnitude on input image
                sobelMagnitude(input_img, result_input);
                cal2DHistogram(input_img, hist_rg_input, 0, src.rows, 0, src.cols);
                cal2DHistogram(result_input, hist_mag_input, 0, src.rows, 0, src.cols);
                vector<float> normalized_rg_input;
                vector<float> normalized_mag_input;
                normalizeHist(hist_rg_input, size, normalized_rg_input);
                normalizeHist(hist_mag_input, size, normalized_mag_input);

                dstMetric2Vector(normalized_rg_target, normalized_mag_target, normalized_rg_input, normalized_mag_input, intersectionDist, 0.5, 0.5);
                delete hist_rg_input;
                delete hist_mag_input;
                histSortVec.push_back(make_pair(intersectionDist,buffer));
        }
    }
    delete hist_rg_target;
    delete hist_mag_target;
    sort(histSortVec.begin(),histSortVec.end());
    //Top 3 results
    for( int i = 1; i < 4; i++ ) {
        cout<<histSortVec[i].first << " "<<histSortVec[i].second<<endl;
        imshow(to_string(i), imread(histSortVec[i].second));
    }        

    char key = cv::waitKey(0);
    while (1)
    {
        if(key == 'q'){
            break;
        }
    }

}


