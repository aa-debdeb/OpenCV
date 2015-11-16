//
//  main.cpp
//  videoAbstraction
//
//  Created by aa_debdeb on 2015/11/16.
//
//

#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"

cv::Mat getAnimeMat(cv::Mat img){
    cv::Mat canny_img, anime_img;
    cv::Canny(img, canny_img, 50, 200);
    anime_img = cv::Mat(cv::Size(img.cols, img.rows), CV_8UC3);
    int color_step_num = 8;
    float color_step_size = float(255) / color_step_num;
    for(int y = 0; y < img.rows; y++){
        for(int x = 0; x < img.cols; x++){
            if(canny_img.at<uchar>(y, x) == 255){
                anime_img.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
            } else {
                cv::Vec3b img_color = img.at<cv::Vec3b>(y, x);
                cv::Vec3b anime_color = cv::Vec3b(255, 255, 255);
                for(int ci = 0; ci < 3; ci++){
                    for(float level = color_step_size / 2; level < 255; level += color_step_size){
                        if(fabs(img_color[ci] - level) <= color_step_size / 2){
                            anime_color[ci] = level;
                            break;
                        }
                    }
                }
                anime_img.at<cv::Vec3b>(y, x) = cv::Vec3b(anime_color[0], anime_color[1], anime_color[2]);
            }
        }
    }
    return anime_img;
}


int main(int argc, const char * argv[]) {
//    cv::Mat img = cv::imread("lenna.jpg", 1);
//    if(img.empty()) return -1;
//
//    cv::Mat anime_img = getAnimeMat(img);
//    cv::imshow("anime image", anime_img);
//    cv::waitKey(0);

    cv::VideoCapture cap(0);
    cv::Mat img;
    while(cv::waitKey(1) != 'q'){
        cap >> img;
        cv::Mat anime_img = getAnimeMat(img);
        cv::imshow("anime image", anime_img);
    }
    
}

