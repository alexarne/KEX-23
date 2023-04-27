
#include <math.h>
#include "meanshift.h"




MeanShift::MeanShift(const cv::Mat& image) : image(image) {
	// defaults
	BANDWIDTH = 3.0;
	COLOR_COMPRESSION = 32;
	INTENSITY_THRESHOLD = 100 / COLOR_COMPRESSION;
}

void MeanShift::setParameters(double bandwidth, double colorcompression, double threshold) {
	BANDWIDTH = bandwidth;
	COLOR_COMPRESSION = colorcompression;
	INTENSITY_THRESHOLD = threshold / colorcompression;
}

void MeanShift::meanShift() {
	//printf("parameters are %f %f %f\n", BANDWIDTH, COLOR_COMPRESSION, INTENSITY_THRESHOLD);

	// Fill black
	output_binary = image.clone();
	output_binary = cv::Scalar(0, 0, 0);
	output_image = image.clone();

	for (int row = 0; row < image.rows; ++row) {
		printf("row %i\n", row);
		for (int col = 0; col < image.cols; ++col) {
			cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
			int B = pixel[0];
			int G = pixel[1];
			int R = pixel[2];
			double gray = (R + G + B) / 3.0;
			Point p(col, row, gray);
			Point shifted = shift(p);

			if (shifted.G <= INTENSITY_THRESHOLD) {
				drawMarker(shifted, p);
			}
		}
	}
}

MeanShift::Point MeanShift::shift(const Point& p) {
	//printf("begin shift\n");
	Point compress = Point(1, 1, COLOR_COMPRESSION);
	Point p_curr = p / compress;
	Point p_prev;

	int shifts = 0;
	while (true) {
		shifts++;
		int fromX = std::max(0, (int)(p_curr.X - BANDWIDTH - EPSILON));
		int toX = std::min(image.cols-1, (int)(p_curr.X + BANDWIDTH + EPSILON));
		int fromY = std::max(0, (int)(p_curr.Y - BANDWIDTH - EPSILON));
		int toY = std::min(image.rows-1, (int)(p_curr.Y + BANDWIDTH + EPSILON));
		//printf("fromx %i tox %i fromy %i toy %i\n", fromX, toX, fromY, toY);
		Point tot_weights;
		double tot_kernel = 0;

		for (int x = fromX; x <= toX; x++) {
			for (int y = fromY; y <= toY; y++) {
				cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
				int B = pixel[0];
				int G = pixel[1];
				int R = pixel[2];
				int gray = (R + G + B) / 3.0;
				Point p2(x,  y, gray);
				p2 = p2 / compress;
				double k = kernel(p_curr, p2);
				tot_weights = tot_weights + p2 * k;
				tot_kernel += k;
			}
		}

		if (tot_kernel == 0) break;
		p_prev = p_curr;
		p_curr = tot_weights / tot_kernel;
		//printf("shift %i = %f %f %f\n", shifts, p_curr.X, p_curr.Y, p_curr.G);
		if (p_curr == p_prev) {
			//printf("convergence.\n");
			break;
		}
	}
	//printf("shifts: %i\n", shifts);
	return p_curr;
}

double MeanShift::kernel(const Point& x, const Point& xi) {
	double dist = distSq(x, xi);
	if (dist >= BANDWIDTH * BANDWIDTH) return 0;
	return 1 - sqrt(dist) / BANDWIDTH;
}

void MeanShift::drawMarker(const Point& peak, const Point& from) {
	output_binary.at<cv::Vec3b>(from.Y, from.X) = cv::Vec3b(255, 255, 255);
	output_image.at<cv::Vec3b>(peak.Y, peak.X) = cv::Vec3b(0, 0, 255);

	cv::Vec3b pixel = output_image.at<cv::Vec3b>(from.Y, from.X);
	if (!(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255)) {
		output_image.at<cv::Vec3b>(from.Y, from.X) = cv::Vec3b(255, 0, 0);
	}
}





















MeanShift::Point::Point() : X(0), Y(0), G(0) { }
MeanShift::Point::Point(double scalar)
	: X(scalar), Y(scalar), G(scalar) { }
MeanShift::Point::Point(double X, double Y)
	: X(X), Y(Y), G(0) { }
MeanShift::Point::Point(double X, double Y, double G)
	: X(X), Y(Y), G(G) { }

bool MeanShift::Point::operator==(const Point& other) const {
	return std::fabs(X - other.X) < EPSILON
		&& std::fabs(Y - other.Y) < EPSILON
		&& std::fabs(G - other.G) < EPSILON;
}

MeanShift::Point MeanShift::Point::operator+(const MeanShift::Point& other) const {
	Point p;
	p.X = this->X + other.X;
	p.Y = this->Y + other.Y;
	p.G = this->G + other.G;
	return p;
}

MeanShift::Point MeanShift::Point::operator-(const Point& other) const {
	Point p;
	p.X = this->X - other.X;
	p.Y = this->Y - other.Y;
	p.G = this->G - other.G;
	return p;
}

MeanShift::Point MeanShift::Point::operator*(const Point& other) const {
	Point p;
	p.X = this->X * other.X;
	p.Y = this->Y * other.Y;
	p.G = this->G * other.G;
	return p;
}

MeanShift::Point MeanShift::Point::operator/(const Point& other) const {
	Point p;
	p.X = this->X / other.X;
	p.Y = this->Y / other.Y;
	p.G = this->G / other.G;
	return p;
}

double MeanShift::distSq(const Point& p1, const Point& p2) {
	Point diff = p1 - p2;
	return diff.X * diff.X
		+ diff.Y * diff.Y
		+ diff.G * diff.G;
}
