#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

constexpr double EPSILON = 0.000001;
constexpr int MARKER_RADIUS = 0;
#define MARKER_COLOR cv::Vec3b(255, 255, 255)

class MeanShift {
public:
	cv::Mat output_binary;
	cv::Mat output_image;

	MeanShift(const cv::Mat& image);
	void setParameters(double bandwidth, double colorcompression, double threshold);
	void meanShift();

	// Print progress
	void setProgressString(std::string s);
	void clearProgress();
private:
	cv::Mat image;

	// Algorithm parameters
	double BANDWIDTH;
	double COLOR_COMPRESSION;
	double INTENSITY_THRESHOLD;

	struct Point {
		Point();
		Point(double scalar);
		Point(double X, double Y);
		Point(double X, double Y, double G);

		double X;
		double Y;
		double G;

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

	Point shift(const Point& p);
	double kernel(const Point& x, const Point& xi);
	void drawMarker(const Point& peak, const Point& from);
	double distSq(const Point& p1, const Point& p2);

	// Print progress
	std::string progressString;
	void printProgress(double perc);
};