/* Extension Task : Implemented Gabor filter from scratch 
    with variation in theta angle : 0, 90, 180, 270.
    You can see perfect texture matching in the end.
*/
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <dirent.h>
#include <limits>
#include "calculation.h"

using namespace cv;
using namespace std;

//target_histogram
const int Hsize_2D = 16;

int main(int argc, char** argv)
{
    char dirname[256];
    char buffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    cv::Mat input_img;

    cv::Mat src = imread("/home/ravina/Desktop/OpenCV/CBIR/sample/olympus/pic.0851.jpg", cv::IMREAD_COLOR);
    if (src.empty()) 
    {
        cout << "Could not load the image" << endl;
        return -1;
    }

    String windowName = "Target Image";
    imshow(windowName, src);

    cv::Mat floatImg, dst;
    src.copyTo(floatImg);
    src.convertTo(floatImg,CV_32F);
    src.copyTo(dst);

    // gabor on target image
    int kernel_size = 32;
    double sig = 1, th = 0, lm = 1.0, gm = 0.02, ps = 0;
    double th1 = M_PI/2;
    cv::Mat res;
    src.convertTo(res,CV_32F);
    
    for(int i = 0; i< 2; i++){
        cv::Mat kernel = cv::getGaborKernel(cv::Size(kernel_size,kernel_size), sig, (i*th1), lm, gm, ps);
        cv::filter2D(floatImg, dst, CV_32F, kernel);
        for(int y = 1; y < dst.rows-1; y++){
            for(int x = 1; x < dst.cols-1; x++){
                for(int c =0; c<3; c++){
                    res.at<Vec3f>(y,x)[c] = max(res.at<Vec3f>(y,x)[c], dst.at<Vec3f>(y,x)[c]);
                }                        
            }
        }
    }
    Mat result;
    res.convertTo(result,CV_8UC3, 1.0/255.0);
    //imshow("res",result);

    int *hist_2d_target = new int [Hsize_2D * Hsize_2D];

    for(int i=0; i < (Hsize_2D * Hsize_2D); i++)
        hist_2d_target[i] = 0;
    
    cal2DHistogram(result, hist_2d_target,0, result.rows, 0, result.cols);
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

    std::vector<pair<float, string>> histSortVec;
    while( (dp = readdir(dirp)) != NULL ) {
        
        if( strstr(dp->d_name, ".jpg") || strstr(dp->d_name, ".png") || strstr(dp->d_name, ".ppm") || strstr(dp->d_name, ".tif") ) {

                //printf("processing image file: %s\n", dp->d_name);
                strcpy(buffer, dirname);
                strcat(buffer, "/");
                strcat(buffer, dp->d_name);
                //printf("full path name: %s\n", buffer);

                float intersectionDist;

                std::vector<int> input_feature;

                // Gabor on dataset images
                input_img = cv::imread(buffer);
                cv::Mat floatImg_input, dst_input,res_input,result_input;
                input_img.copyTo(floatImg_input);
                input_img.convertTo(floatImg_input,CV_32F);
                input_img.copyTo(dst_input);
                input_img.convertTo(res_input,CV_32F);

                for(int i = 0; i< 2; i++){
                    cv::Mat kernel_input = cv::getGaborKernel(cv::Size(kernel_size,kernel_size), sig, (i*th1), lm, gm, ps);
                    cv::filter2D(floatImg_input, dst_input, CV_32F, kernel_input);
                    for(int y = 1; y < dst_input.rows-1; y++){
                        for(int x = 1; x < dst_input.cols-1; x++){
                            for(int c =0; c<3; c++){
                                res_input.at<Vec3f>(y,x)[c] = max(res_input.at<Vec3f>(y,x)[c], dst_input.at<Vec3f>(y,x)[c]);
                            }                        
                        }
                    }
                }
                
                res_input.convertTo(result_input,CV_8UC3, 1.0/255.0);

                int *hist_2d_input = new int [Hsize_2D * Hsize_2D];

                //initialize to all zeros
                for(int i=0; i < (Hsize_2D * Hsize_2D); i++)
                    hist_2d_input[i] = 0;

                cal2DHistogram(result_input, hist_2d_input, 0, result_input.rows, 0, result_input.cols);

                vector<float> normalized_input;
                normalizeHist(hist_2d_input, size, normalized_input);
                
                dstMetricHistIntersection(normalized_target, normalized_input, intersectionDist);
                delete hist_2d_input;
                histSortVec.push_back(make_pair(intersectionDist,buffer));
        }
    }
    delete hist_2d_target;
    sort(histSortVec.begin(),histSortVec.end());

    //Top 6 images distance and path
    for( int i = 0; i < 6; i++ ) {
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
