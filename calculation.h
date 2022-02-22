#ifndef __CALCULATION_H_
#define __CALCULATION_H_

#include<iostream>
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int cal1DHistogram(cv::Mat &src, int divisor, int *hist_1d_output);
int cal2DHistogram(cv::Mat &src, int *hist_2d_output, int rowStart, int rowEnd, int colStart, int colEnd);
int cal3DHistogram(cv::Mat &src, int *hist_3d_output, int divisor, int rowStart, int rowEnd, int colStart, int colEnd);

int normalizeHist(int *hist_output, int size, vector<float> &normalizedVector);
int dstMetricHistIntersection(vector<float> &normalized_target, vector<float> &normalized_input, float &intersectionDist);
int dstMetric2Vector(vector<float> &normalized_target1, vector<float> &normalized_target2, vector<float> &normalized_input1, vector<float> &normalized_input2, float &intersectionDist, float weight1, float weight2);

int sobelX3x3( cv::Mat &src, cv::Mat3s &dst );
int sobelY3x3( cv::Mat &src, cv::Mat3s &dst );
int sobelMagnitude( cv::Mat &src, cv::Mat &dst );
int lawsL5E5(cv::Mat &src, cv::Mat3s &dst);

int correlationDst(vector<float> &normalized_target, vector<float> &normalized_input, float mean_target, float mean_input,float &corrDist);
int calHSHistogram(cv::Mat &src, int *hist_2d_output, int rowStart, int rowEnd, int colStart, int colEnd);
int chiSquare(vector<float> &normalized_target, vector<float> &normalized_input, float &chiDist);

#endif