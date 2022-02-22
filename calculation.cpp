#include<iostream>
#include<cmath>
#include "calculation.h"
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const int Hsize_2D = 16;
const int Hsize_3D = 8;

//Calculation of distance metric on 2 feature vectors
int dstMetricHistIntersection(vector<float> &normalized_target, vector<float> &normalized_input, float &intersectionDist){

    intersectionDist = 0.0;
    for(int i = 0; i < normalized_target.size(); i++){
        intersectionDist += min(normalized_target[i],normalized_input[i]);
    }
    intersectionDist = 1 - intersectionDist;
    return 0;
}
//Calculation of distance metric on 4 feature vectors with weighted average
int dstMetric2Vector(vector<float> &normalized_target1, vector<float> &normalized_target2, vector<float> &normalized_input1, vector<float> &normalized_input2, float &intersectionDist, float weight1, float weight2){

    intersectionDist = 0.0;
    for(int i = 0; i < normalized_target1.size(); i++){
        intersectionDist += weight1 * min(normalized_target1[i],normalized_input1[i]) + weight2 * min(normalized_target2[i],normalized_input2[i]);
    }
    intersectionDist = 1 - intersectionDist;
    return 0;
}

//Function to calculate 1D histogram
int cal1DHistogram(cv::Mat &src, int divisor, int *hist_1d_output){

    for(int y = 0;y < src.rows; y++){
        for(int x = 0;x < src.cols; x++){
            int ix = (src.at<uchar>(y,x))/divisor;
            hist_1d_output[ix]++;
        }   
    }
    return 0;
}

//Function to calculate 2D rg histogram with selection of rows and cols initial and final position
int cal2DHistogram(cv::Mat &src, int *hist_2d_output, int rowStart, int rowEnd, int colStart, int colEnd){

    for(int y = rowStart;y < rowEnd; y++){
        for(int x = colStart;x < colEnd; x++){
            int rix = (Hsize_2D * src.at<Vec3b>(y,x)[2])/(src.at<Vec3b>(y,x)[0] + src.at<Vec3b>(y,x)[1] + src.at<Vec3b>(y,x)[2] + 1);
            int gix = (Hsize_2D * src.at<Vec3b>(y,x)[1])/(src.at<Vec3b>(y,x)[0] + src.at<Vec3b>(y,x)[1] + src.at<Vec3b>(y,x)[2] + 1);

            hist_2d_output[rix * Hsize_2D + gix]++;
        }   
    }
    return 0;
}

//Function to calculate 3D RGB histogram with selection of rows and cols initial and final position
int cal3DHistogram(cv::Mat &src, int *hist_3d_output, int divisor, int rowStart, int rowEnd, int colStart, int colEnd){

    for(int y = rowStart;y < rowEnd; y++){
        for(int x = colStart;x < colEnd; x++){
            int rix = (src.at<Vec3b>(y,x)[2])/divisor;
            int gix = (src.at<Vec3b>(y,x)[1])/divisor;
            int bix = (src.at<Vec3b>(y,x)[0])/divisor;

            hist_3d_output[rix * Hsize_3D * Hsize_3D + gix * Hsize_3D + bix]++;
        }   
    }
    return 0;
}

//Function to normalize 1D, 2D, and 3D histograms
int normalizeHist(int *hist_output, int size, vector<float> &normalizedVector){

    int sum = 0;
    for(int i = 0; i < size; i++){
        sum = sum + hist_output[i];
    }

    for(int i = 0; i < size; i++){
        float val = ((float) hist_output[i])/sum;
        normalizedVector.push_back(val);
    }
    return 0;
}

//Function for sobel X
int sobelX3x3(cv::Mat &src, cv::Mat3s &dst){
    
    cv::Mat temp;
    src.copyTo(temp);

    for(int c = 0; c < 3; c++){
        for(int y = 1; y < src.rows-1; y++){
            for(int x = 0; x < src.cols; x++){
                temp.at<Vec3b>(y,x)[c] = (0.25*src.at<Vec3b>(y-1, x)[c] + 0.5*src.at<Vec3b>(y, x)[c] + 0.25*src.at<Vec3b>(y+1, x)[c]);
            }
        }
    }
    for(int c = 0; c < 3; c++){
        for(int y = 0; y < src.rows; y++){
            for(int x = 1; x < src.cols-1; x++){
                dst.at<Vec3s>(y,x)[c] = (-1*temp.at<Vec3b>(y, x-1)[c] + 0*temp.at<Vec3b>(y, x)[c] + temp.at<Vec3b>(y, x+1)[c]);
            }
        }
    }
    return 0;

}

//Function for sobel Y
int sobelY3x3(cv::Mat &src, cv::Mat3s &dst)
{
    cv::Mat3s temp;
    src.copyTo(temp);

    for(int c = 0; c < 3; c++){
        for(int y = 1; y < src.rows-1; y++){
            for(int x = 0; x < src.cols; x++){
                temp.at<Vec3s>(y,x)[c] = (src.at<Vec3b>(y-1, x)[c] + 0*src.at<Vec3b>(y, x)[c] - src.at<Vec3b>(y+1, x)[c]);
            }
        }
    }
    for(int c = 0; c < 3; c++){
        for(int y = 0; y < src.rows; y++){
            for(int x = 1; x < src.cols-1; x++){
                dst.at<Vec3s>(y,x)[c] = (0.25*temp.at<Vec3s>(y, x-1)[c] + 0.5*temp.at<Vec3s>(y, x)[c] + 0.25*temp.at<Vec3s>(y, x+1)[c]);
            }
        }
    }
    return 0;    
}

//Function for sobel Magnitude
int sobelMagnitude( cv::Mat &src, cv::Mat &dst ){

    cv::Mat sx, sy;
    cv:: Mat3s temp_sx, temp_sy;

    src.copyTo(dst);
    src.copyTo(temp_sx);
    src.copyTo(temp_sy);                  

    sobelX3x3(src, temp_sx);
    convertScaleAbs(temp_sx, sx);

    sobelY3x3(src, temp_sy);
    convertScaleAbs(temp_sy, sy);
    
    for(int y = 1; y < sx.rows - 1; y++){
        for(int x = 1; x < sx.cols - 1; x++){
            for(int c =0; c<3; c++){
                dst.at<Vec3b>(y,x)[c] = sqrt((sx.at<Vec3b>(y,x)[c] * sx.at<Vec3b>(y,x)[c]) + (sy.at<Vec3b>(y,x)[c] * sy.at<Vec3b>(y,x)[c]));
            }                        
        }
    }
    return 0;
}

//L5E5 Laws Filter
int lawsL5E5(cv::Mat &src, cv::Mat3s &dst){
    
    cv::Mat temp;
    src.copyTo(temp);
    src.copyTo(dst);

    for(int c = 0; c < 3; c++){
        for(int y = 2; y < src.rows-2; y++){
            for(int x = 0; x < src.cols; x++){
                temp.at<Vec3b>(y,x)[c] = (0.0625*src.at<Vec3b>(y-2, x)[c] + 0.25*src.at<Vec3b>(y-1, x)[c] + 0.375*src.at<Vec3b>(y, x)[c] + 0.25*src.at<Vec3b>(y+1, x)[c] + 0.0625*src.at<Vec3b>(y+2, x)[c]);
            }
        }
    }
    for(int c = 0; c < 3; c++){
        for(int y = 0; y < src.rows; y++){
            for(int x = 2; x < src.cols-2; x++){
                dst.at<Vec3s>(y,x)[c] = (-1*temp.at<Vec3b>(y, x-2)[c] + -2*temp.at<Vec3b>(y, x-1)[c] + 0*temp.at<Vec3b>(y, x)[c] + 2*temp.at<Vec3b>(y, x+1)[c] + 1*temp.at<Vec3b>(y, x+2)[c]);
            }
        }
    }
    return 0;

}

//Function to calculate Correlation as distance Metric
int correlationDst(vector<float> &normalized_target, vector<float> &normalized_input, float mean_target, float mean_input,float &corrDist){

    float dist1, dist2, dist3 = 0.0;
    for(int i = 0; i < normalized_target.size(); i++){
        dist1 += (normalized_target[i]- mean_target) * (normalized_input[i] - mean_input);
        dist2 += (normalized_target[i]- mean_target) * (normalized_target[i]- mean_target);
        dist3 +=  (normalized_input[i]- mean_input) * (normalized_input[i]- mean_input);

    }
    corrDist = dist1 / sqrt(dist2 * dist3);
    corrDist = 1 - corrDist;
    return 0;
}

// function to calculte HS histogram
int calHSHistogram(cv::Mat &src, int *hist_2d_output, int rowStart, int rowEnd, int colStart, int colEnd){

    for(int y = rowStart;y < rowEnd; y++){
        for(int x = colStart;x < colEnd; x++){
            int hix = (Hsize_2D * src.at<Vec3b>(y,x)[1])/(src.at<Vec3b>(y,x)[0] + src.at<Vec3b>(y,x)[1] + src.at<Vec3b>(y,x)[2] + 1);
            int six = (Hsize_2D * src.at<Vec3b>(y,x)[2])/(src.at<Vec3b>(y,x)[0] + src.at<Vec3b>(y,x)[1] + src.at<Vec3b>(y,x)[2] + 1);

            hist_2d_output[hix * Hsize_2D + six]++;
        }   
    }
    return 0;
}
//Function to calculate chi-Square Distance Metric
int chiSquare(vector<float> &normalized_target, vector<float> &normalized_input, float &chiDist){

    float dist1, dist2 = 0.0;
    for(int i = 0; i < normalized_target.size(); i++){
        dist1 += (normalized_target[i] - normalized_input[i]) * (normalized_target[i] - normalized_input[i]);
        dist2 += (normalized_target[i]);
    }
    chiDist = dist1/dist2;
    //chiDist = 1 - chiDist;
    return 0;
}