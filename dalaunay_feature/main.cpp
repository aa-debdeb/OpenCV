
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

int main(int argc, const char * argv[]) {
    cv::Mat img = cv::imread("lena.jpg", 1);
    if(img.empty()) return -1;
    
    cv::Mat gray_img;
    cv::cvtColor(img, gray_img, CV_BGR2GRAY);
    cv::normalize(gray_img, gray_img, 0, 255, cv::NORM_MINMAX);
    
    std::vector<cv::KeyPoint> keypoints;
    std::vector<cv::KeyPoint>::iterator itk;
    cv::GoodFeaturesToTrackDetector detector(1600, 0.01, 5, 3); // 固有値に基づく特徴点検出
//    cv::GoodFeaturesToTrackDetector detector(1600, 0.01, 5, 3, true); // Harris検出器に基づく特徴点検出
//    cv::FastFeatureDetector detector(25, false); // FAST検出器に基づく特徴点検出
//    cv::StarFeatureDetector detector(16, 5); // Star検出器に基づく特徴点検出
//    cv::SiftFeatureDetector detector(0.001, 3.0); // SIFT検出器に基づく特徴点検出
//    cv::SurfFeatureDetector detector(100); // SURF検出器に基づく特徴点検出
//    cv::MserFeatureDetector detector; // MSER検出器に基づく特徴点検出
//    cv::OrbFeatureDetector detector(1500); // ORB検出器に基づく特徴点検出
//    cv::GridAdaptedFeatureDetector detector(new cv::FastFeatureDetector(10), 5000, 5, 5); // FAST検出器+Gridアダプタ に基づく特徴点検出
//    cv::PyramidAdaptedFeatureDetector detector(new cv::FastFeatureDetector(10), 3); // FAST検出器+Pyramidアダプタ に基づく特徴点検出

    detector.detect(img, keypoints);
    cv::Mat feature_img = img.clone();
    
    cv::Subdiv2D subdiv;
    subdiv.initDelaunay(cv::Rect(0, 0, img.rows, img.cols));
    std::vector<cv::Point2f> points;
    
    for(itk = keypoints.begin(); itk!=keypoints.end(); ++itk) {
        cv::circle(feature_img, itk->pt, 5, cv::Scalar(184, 210, 0), 1, CV_AA);
        points.push_back(itk->pt);
    }

    points.push_back(cv::Point2f(0, 0));
    points.push_back(cv::Point2f(img.rows - 1, 0));
    points.push_back(cv::Point2f(0, img.cols - 1));
    points.push_back(cv::Point2f(img.rows - 1, img.cols - 1));
    subdiv.insert(points);
    
    // ボロノイモザイクの作成
    std::vector<int> idx;
    std::vector<std::vector<cv::Point2f>> facetLists;
    std::vector<cv::Point2f> facetCenters;
    subdiv.getVoronoiFacetList(idx, facetLists, facetCenters);
    
    cv::Mat voronoi_img = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
    for(auto facetList = facetLists.begin(); facetList != facetLists.end(); facetList++){
        cv::Point *vertices = new cv::Point[facetList->size()];
        cv::Vec3f polyColor = cv::Vec3f(0, 0, 0);
        int polyColorNum = 0;
        for(int i = 0; i < facetList->size(); i++){
            cv::Point vertex = facetList->at(i);
            vertices[i] = vertex;
            if(0 <= vertex.x && vertex.x < img.rows && 0 <= vertex.y && vertex.y < img.cols){
                polyColor += img.at<cv::Vec3b>(vertex.y, vertex.x);
                polyColorNum++;
            }
        }
        polyColor /= float(polyColorNum);
        cv::fillConvexPoly(voronoi_img, vertices, int(facetList->size()), cv::Scalar(polyColor[0], polyColor[1], polyColor[2]), CV_AA);
        delete[] vertices;
    }
    
    // ドロネーモザイクの作成
    std::vector<cv::Vec6f> triangles;
    subdiv.getTriangleList(triangles);
    
    cv::Mat dalaunay_img = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
    for(auto it = triangles.begin(); it != triangles.end(); it++)
    {
        cv::Vec6f &vec = *it;
        cv::Point vertices[3];
        vertices[0] = cv::Point(vec[0], vec[1]);
        vertices[1] = cv::Point(vec[2], vec[3]);
        vertices[2] = cv::Point(vec[4], vec[5]);
        cv::Vec3f polyColor = cv::Vec3f(0, 0, 0);
        polyColor += img.at<cv::Vec3b>(vertices[0].y, vertices[0].x);
        polyColor += img.at<cv::Vec3b>(vertices[1].y, vertices[1].x);
        polyColor += img.at<cv::Vec3b>(vertices[2].y, vertices[2].x);
        polyColor /= float(3);
        cv::fillConvexPoly(dalaunay_img, vertices, 3, cv::Scalar(polyColor[0], polyColor[1], polyColor[2]), CV_AA);
    }
    
    
    cv::imshow("feature", feature_img);
    cv::imshow("voronoi", voronoi_img);
    cv::imshow("dalaunay", dalaunay_img);

    cv::waitKey(0);
    
    
}
