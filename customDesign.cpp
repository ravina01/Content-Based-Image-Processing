/* Task 5 : Custom Design : L5E5 laws filter + Color information
    0.6 weightage given to color information + 0.4 to laws filter
    Target 1: Green trash container 
    Target 2: Garden with people and bences

    Extension No 2: since we have to collect all blue trash bins I have Top 10 results it.
    I have implemented it using Laws filter only.
    Top 3 images matching given output
    To Run this file : add this file name in exexutable along with calculation.cpp
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
  
    cv::Mat src, input_img;
    cv::Mat result;
    cv::Mat3s result_laws;
    int size = Hsize_2D * Hsize_2D;

    //Task 5, 2 target images - pic.0746.jpg, pic.0492.jpg

    src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0746.jpg", cv::IMREAD_COLOR); //Green Trash container
    //src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0492.jpg", cv::IMREAD_COLOR); //Garden and bences image

    //Extension2 to collect all blue trash bins
    
    //src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0287.jpg", cv::IMREAD_COLOR); //Blue  Trash Bins

    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);

    lawsL5E5(src, result_laws);
    convertScaleAbs(result_laws, result);

    int *hist_rg_target = new int [Hsize_2D * Hsize_2D];
    int *hist_laws_target = new int [Hsize_2D * Hsize_2D];

    for(int i=0; i < (Hsize_2D * Hsize_2D); i++){
        hist_rg_target[i] = 0;
        hist_laws_target[i] = 0;
    }
    
    cal2DHistogram(src, hist_rg_target, 50, 462, 50, 610);
    cal2DHistogram(result, hist_laws_target, 50, 462, 50, 610);
    vector<float> normalized_rg_target;
    vector<float> normalized_laws_target;
    normalizeHist(hist_rg_target, size, normalized_rg_target);
    normalizeHist(hist_laws_target, size, normalized_laws_target);
       

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
                cv::Mat3s laws_input;
                input_img = cv::imread(buffer);

                int *hist_rg_input = new int [Hsize_2D * Hsize_2D];
                int *hist_laws_input = new int [Hsize_2D * Hsize_2D];

                for(int i=0; i < (Hsize_2D * Hsize_2D); i++){
                    hist_rg_input[i] = 0;
                    hist_laws_input[i] = 0;
                }

                lawsL5E5(input_img, laws_input);
                convertScaleAbs(laws_input, result_input);
                
                cal2DHistogram(input_img, hist_rg_input, 50, 462, 50, 610);
                cal2DHistogram(result_input, hist_laws_input, 50, 462, 50, 610);

                vector<float> normalized_rg_input;
                vector<float> normalized_laws_input;
                normalizeHist(hist_rg_input, size, normalized_rg_input);
                normalizeHist(hist_laws_input, size, normalized_laws_input);

                dstMetric2Vector(normalized_rg_target, normalized_laws_target, normalized_rg_input, normalized_laws_input, intersectionDist, 0.6, 0.4);
                delete hist_rg_input;
                delete hist_laws_input;
                histSortVec.push_back(make_pair(intersectionDist,buffer));
        }
    }
    //deleted all created arrays
    delete hist_rg_target;
    delete hist_laws_target;

    sort(histSortVec.begin(),histSortVec.end());
    for( int i = 1; i < 11; i++ ) {
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


