#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"

int main(int argc, const char * argv[]) {
    cv::Mat img = cv::imread("input.jpg", 1);
    if(img.empty()) return -1;
    

    cv::HOGDescriptor hog;
    cv::Mat gray_img;
    std::vector<cv::Rect> found;
    
    cv::cvtColor(img, gray_img, CV_BGR2GRAY);
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    hog.detectMultiScale(gray_img, found);



    for(int i = 0; i < found.size(); i++){
        cv::Rect r = found[i];
        rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 2);
    }
    cv::imshow("hog", img);
    cv::imwrite("output.jpg", img);
    cv::waitKey(0);
}
