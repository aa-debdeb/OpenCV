
#include <iostream>
#include "opencv2/opencv.hpp"

int main(int argc, const char * argv[]) {
    cv::Mat img = cv::imread("lena.jpg", 1);
    if(img.empty()) return -1;
    
    cv::Subdiv2D subdiv;
    subdiv.initDelaunay(cv::Rect(0, 0, img.rows, img.cols));
    
    std::vector<cv::Point2f> points;
    for(int i = 0; i < 10000; i++){
        cv::Point2f point = cv::Point2f(rand() % img.rows, rand() % img.cols);
        points.push_back(point);
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
    
    cv::Mat voronoi = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
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
        cv::fillConvexPoly(voronoi, vertices, int(facetList->size()), cv::Scalar(polyColor[0], polyColor[1], polyColor[2]), CV_AA);
        delete[] vertices;
    }
    
    // ドロネーモザイクの作成
    std::vector<cv::Vec6f> triangles;
    subdiv.getTriangleList(triangles);
    
    cv::Mat dalaunay = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
    
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
        cv::fillConvexPoly(dalaunay, vertices, 3, cv::Scalar(polyColor[0], polyColor[1], polyColor[2]), CV_AA);
    }
    
    cv::imshow("voronoi", voronoi);
    cv::imshow("dalaunay", dalaunay);
    cv::waitKey(0);

}
