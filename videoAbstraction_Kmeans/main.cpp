//
//  main.cpp
//  videoAbstraction_Kmeans
//
//  Created by aa_debdeb on 2015/11/17.
//  Copyright (c) 2015年 aa_debdeb. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"

cv::Mat getAnimeMat(cv::Mat img){
    cv::Mat canny_img;
    cv::Canny(img, canny_img, 50, 200);
    
    const int cluster_count = 16;
    cv::Mat points;
    img.convertTo(points, CV_32FC3);
    points = points.reshape(3, img.rows * img.cols);
    cv::Mat_<int> clusters(points.size(), CV_32SC1);
    cv::Mat centers;
    cv::kmeans(points, cluster_count, clusters,
               cvTermCriteria(CV_TERMCRIT_EPS|CV_TERMCRIT_ITER, 10, 1.0), 1, cv::KMEANS_PP_CENTERS, centers);
    cv::Mat anime_img(img.size(), img.type());
    cv::MatIterator_<cv::Vec3b> itd = anime_img.begin<cv::Vec3b>(),
        itd_end = anime_img.end<cv::Vec3b>();
    cv::MatIterator_<uchar> canny_itd = canny_img.begin<uchar>();
    for(int i = 0; itd != itd_end; ++itd, ++canny_itd, ++i){
        if(*canny_itd == 255){
            (*itd)[0] = 0;
            (*itd)[1] = 0;
            (*itd)[2] = 0;
        } else {
            cv::Vec3f &color = centers.at<cv::Vec3f>(clusters(i), 0);
            (*itd)[0] = cv::saturate_cast<uchar>(color[0]);
            (*itd)[1] = cv::saturate_cast<uchar>(color[1]);
            (*itd)[2] = cv::saturate_cast<uchar>(color[2]);
        }
    }

    return anime_img;
}


int main(int argc, const char * argv[]) {
//        cv::Mat img = cv::imread("lenna.jpg", 1);
//        if(img.empty()) return -1;
//    
//        cv::Mat anime_img = getAnimeMat(img);
//        cv::imshow("anime image", anime_img);
//        cv::waitKey(0);
    
    cv::VideoCapture cap(0);
    cv::Mat img;
    while(cv::waitKey(1) != 'q'){
        cap >> img;
        cv::Mat anime_img = getAnimeMat(img);
        cv::imshow("anime image", anime_img);
    }
    
}