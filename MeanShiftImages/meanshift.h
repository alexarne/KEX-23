#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

struct Point;
struct Cluster;

int meanShift(cv::Mat& image);
Point shift(Point p);
void cluster(Point p, std::vector<Cluster>& clusters);
void drawMarker(cv::Mat& image, const Point& p);
