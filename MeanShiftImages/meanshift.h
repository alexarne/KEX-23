#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

// Algorithm parameters
// double BANDWIDTH = 3.0;
// double COLOR_COMPRESSION = 32;
// double INTENSITY_THRESHOLD = 100 / COLOR_COMPRESSION;


constexpr double EPSILON = 0.000001;
constexpr int MARKER_RADIUS = 0;
#define MARKER_COLOR cv::Vec3b(255, 255, 255)
//#define MARKER_COLOR cv::Vec3b(0, 0, 255)
constexpr bool BINARIZED_IMAGE = true;

class MeanShift {
public:
	cv::Mat output_binary;
	cv::Mat output_image;

	MeanShift(const cv::Mat& image, double Bandwidth, double ColorCompression, double intensityThreshold);
	int meanShift();
private:
	cv::Mat image;
	
	double BANDWIDTH;
	double COLOR_COMPRESSION;
	double INTENSITY_THRESHOLD;

	struct Point {
		Point();
		Point(double scalar);
		Point(double X, double Y);
		Point(double X, double Y, double G);
		//Point(double X, double Y, double R, double G, double B);
		double X;
		double Y;
		//double R;
		double G;
		//double B;
		bool operator==(const Point& other) const;
		Point operator+(const Point& other) const;
		Point operator-(const Point& other) const;
		Point operator*(const Point& other) const;
		Point operator/(const Point& other) const;
	};

	struct Cluster {
		Point mean;
		int count;
		int tot_R;
		int tot_G;
		int tot_B;
	};

	std::vector<Cluster> clusters;
	Point shift(const Point& p);
	double kernel(const Point& x, const Point& xi);
	void cluster(const Point& p, const Point& p2);
	void drawMarker(const Point& peak, const Point& from);
	double distSq(const Point& p1, const Point& p2);
};