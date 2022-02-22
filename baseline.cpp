/* Task 1 : baseline Matching
    Top 3 images matching given output
    To Run this file : add this file name in exexutable along with calculation.cpp
*/
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>

using namespace cv;
using namespace std;

void calculateRowCol(int inputRows, int inputCols, int *row, int *col) {
    *row = (inputRows/2)-4;
    *col = (inputCols/2)-4;
}

void distanceMetric(vector<int> target_feature, vector<int> input_feature, float &squaredDist ){
    int error = 0;
    for(int i = 0; i < target_feature.size(); i++){
        error += ( (target_feature[i] - input_feature[i]) * (target_feature[i] - input_feature[i]) ); 
    }
    squaredDist = (error/target_feature.size());
}

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
    
    src = cv::imread("/home/ravina/Desktop/OpenCV/CBIR/testOlympus/pic.1016.jpg", cv::IMREAD_COLOR);

    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);
    
    int row, col;
    std::vector<int> target_feature;
    calculateRowCol(src.rows, src.cols, &row, &col);
    for(int c = 0; c < 3; c++){
        for(int y = row;y < row+9; y++){
            for(int x = col;x < col+9; x++){
                target_feature.push_back(src.at<Vec3b>(y,x)[c]);       
            }   
        }
    }

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
    
    std::vector<pair<float, string>> baselineSortVec;

    while( (dp = readdir(dirp)) != NULL ) {
        
        if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {

                //printf("processing image file: %s\n", dp->d_name);
                strcpy(buffer, dirname);
                strcat(buffer, "/");
                strcat(buffer, dp->d_name);
                //printf("full path name: %s\n", buffer);

                int inputRow, inputCol;
                float squaredDist;
                std::vector<int> input_feature;

                input_img = cv::imread(buffer);

                calculateRowCol(input_img.rows, input_img.cols, &inputRow, &inputCol);

                for(int c = 0; c < 3; c++){
                    for(int y = inputRow; y < inputRow+9; y++){
                        for(int x = inputCol; x < inputCol+9; x++){
                            input_feature.push_back(input_img.at<Vec3b>(y,x)[c]); 
                        }   
                    }
                }
            
                distanceMetric(target_feature, input_feature, squaredDist);
                baselineSortVec.push_back(make_pair(squaredDist,buffer));
        }
    }
    //sorted vector to get top 3 results
    sort(baselineSortVec.begin(),baselineSortVec.end());

    //Top 3 results
    for( int i = 1; i < 4; i++ ) {
        cout << "\n"<<baselineSortVec[i].first << " "<<baselineSortVec[i].second<<endl;
        imshow(to_string(i), imread(baselineSortVec[i].second));
    }
    

    char key = cv::waitKey(0);
    while (1)
    {
        if(key == 'q'){
            break;
        }
    }

}


