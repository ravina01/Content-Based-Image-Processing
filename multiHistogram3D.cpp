/* Task 3 : Multi Histogram 3D
    cal3DHistogram(),normalizeHist(),dstMetric2Vector() methods written in calculation header
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

//target_histogram
const int Hsize_3D = 8;

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
    
    src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0274.jpg", cv::IMREAD_COLOR);

    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);
    
    int *hist_3d_target1 = new int [Hsize_3D * Hsize_3D * Hsize_3D];
    int *hist_3d_target2 = new int [Hsize_3D * Hsize_3D * Hsize_3D];

    //initialize to all zeros
    for(int i=0; i < (Hsize_3D * Hsize_3D * Hsize_3D); i++){
        hist_3d_target1[i] = 0;
        hist_3d_target2[i] = 0;
    }

    int divisor = 256/Hsize_3D;
    int size = Hsize_3D * Hsize_3D * Hsize_3D;
    cal3DHistogram(src, hist_3d_target1, divisor, 0, (src.rows/2), 0, src.cols);
    cal3DHistogram(src, hist_3d_target2, divisor, (src.rows/2), src.rows, 0, src.cols);

    vector<float> normalized_target1;
    vector<float> normalized_target2;
    normalizeHist(hist_3d_target1, size, normalized_target1);
    normalizeHist(hist_3d_target2, size, normalized_target2);

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
                std::vector<int> input_feature;

                input_img = cv::imread(buffer);

                int *hist_3d_input1 = new int [Hsize_3D * Hsize_3D * Hsize_3D];
                int *hist_3d_input2 = new int [Hsize_3D * Hsize_3D * Hsize_3D];

                //initialize to all zeros
                for(int i=0; i < (Hsize_3D * Hsize_3D * Hsize_3D); i++){
                    hist_3d_input1[i] = 0;
                    hist_3d_input2[i] = 0;
                }
                    
                cal3DHistogram(input_img, hist_3d_input1, divisor, 0, (input_img.rows/2), 0, input_img.cols);
                cal3DHistogram(input_img, hist_3d_input2, divisor, (input_img.rows/2), input_img.rows, 0, input_img.cols);

                vector<float> normalized_input1;
                vector<float> normalized_input2;
                normalizeHist(hist_3d_input1, size, normalized_input1);
                normalizeHist(hist_3d_input2, size, normalized_input2);

                dstMetric2Vector(normalized_target1, normalized_target2, normalized_input1, normalized_input2, intersectionDist, 0.5, 0.5);
                delete hist_3d_input1;
                delete hist_3d_input2;
                histSortVec.push_back(make_pair(intersectionDist,buffer));
        }
    }
    delete hist_3d_target1;
    delete hist_3d_target2;

    //sorted all distances
    sort(histSortVec.begin(),histSortVec.end());

    //top 3 results
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


