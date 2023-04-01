
#include <math.h>
#include "meanshift.h"




MeanShift::MeanShift(cv::Mat& image) : image(image), clusters(0) { }

int MeanShift::meanShift() {
	//clusters.push_back({ {100, 50}, 1 });

	for (int row = 0; row < image.rows; ++row) {
		printf("row %i\n", row);
		for (int col = 0; col < image.cols; ++col) {
			cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
			int B = pixel[0];
			int G = pixel[1];
			int R = pixel[2];
			Point p(col, row, R, G, B);
			//printf("pre shift R=%i G=%i B=%i\n", R, G, B);
			Point shifted = shift(p);
			cluster(shifted, p);
			//drawMarker(shifted);
		}
	}

	// Mark clusters
	for (const Cluster& c : clusters) {
		drawMarker(c.mean);
	}

	return clusters.size();
}

MeanShift::Point MeanShift::shift(const Point& p) {
	//printf("shift received %f %f %f %f %f\n", p.X, p.Y, p.R, p.G, p.B);
	
	//Point compress = Point(1, 1, COLOR_COMPRESSION);
	//Point p_curr = p / compress;
	//Point p_prev;

	Point compress = Point(1, 1, COLOR_COMPRESSION);
	double Gray = (p.R + p.G + p.B) / 3.0;
	Point p_curr(p.X, p.Y, Gray);
	p_curr = p_curr / compress;
	Point p_prev;

	int shifts = 0;
	while (true) {
		shifts++;
		int fromX = std::max(0, (int)(p_curr.X - BANDWIDTH - EPSILON));
		int toX = std::min(image.cols, (int)(p_curr.X + BANDWIDTH + EPSILON));
		int fromY = std::max(0, (int)(p_curr.Y - BANDWIDTH - EPSILON));
		int toY = std::min(image.rows, (int)(p_curr.Y + BANDWIDTH + EPSILON));

		Point tot_weights;
		double tot_kernel = 0;

		for (int x = fromX; x < toX; x++) {
			for (int y = fromY; y < toY; y++) {
				cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
				int B = pixel[0];
				int G = pixel[1];
				int R = pixel[2];
				Gray = (R + G + B) / 3.0;
				Point p2(x, y, Gray);
				p2 = p2 / compress;
				double k = kernel(p_curr, p2);
				tot_weights = tot_weights + p2 * k;
				tot_kernel += k;
			}
		}

		if (tot_kernel == 0) break;
		p_prev = p_curr;
		p_curr = tot_weights / tot_kernel;
		//printf("shift %i = %f %f %f %f %f\n", shifts, p_curr.X, p_curr.Y, p_curr.R, p_curr.G, p_curr.B);
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

void MeanShift::cluster(const Point& p, const Point& p2) {
	//printf("cluster %f %f %f %f %f\n", p.X, p.Y, p.R, p.G, p.B);
	if ((p.R + p.G + p.B) / 3 > INTENSITY_THRESHOLD) return;
	//printf("pass\n");

	cv::Vec3b pixel = image.at<cv::Vec3b>(p2.Y, p2.X);
	if (!(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255)) {
		image.at<cv::Vec3b>(p2.Y, p2.X) = cv::Vec3b(255, 0, 0);
	}

	drawMarker(p);
	return;

	for (int i = 0; i < clusters.size(); ++i) {
		if (p == clusters[i].mean) {
			clusters[i].tot_R += p.R;
			clusters[i].tot_G += p.G;
			clusters[i].tot_B += p.B;
			clusters[i].count++;
			return;
		}
	}
	clusters.push_back({ p, 1 });
}

void MeanShift::drawMarker(const Point& p) {
	for (int i = -MARKER_RADIUS; i <= MARKER_RADIUS; ++i) {
		if (p.Y + i < 0 || p.Y + i >= image.rows) continue;
		for (int j = -MARKER_RADIUS; j <= MARKER_RADIUS; ++j) {
			if (p.X + j < 0 || p.X + j >= image.cols) continue;
			image.at<cv::Vec3b>(p.Y + i, p.X + j) = MARKER_COLOR;
		}
	}
}




















MeanShift::Point::Point() : X(0), Y(0), R(0), G(0), B(0) { }
MeanShift::Point::Point(double scalar)
	: X(scalar), Y(scalar), R(scalar), G(scalar), B(scalar) { }
MeanShift::Point::Point(double X, double Y)
	: X(X), Y(Y), R(0), G(0), B(0) { }
MeanShift::Point::Point(double X, double Y, double G)
	: X(X), Y(Y), R(G), G(G), B(G) { }
MeanShift::Point::Point(double X, double Y, double R, double G, double B)
	: X(X), Y(Y), R(R), G(G), B(B) { }

bool MeanShift::Point::operator==(const Point& other) const {
	//printf("-------------------\n");
	//printf("diffX: %f\tEPSILON: %f\n", std::fabs(X - other.X), EPSILON);
	//printf("diffY: %f\tEPSILON: %f\n", std::fabs(Y - other.Y), EPSILON);
	//printf("diffR: %f\tEPSILON: %f\n", std::fabs(R - other.R), EPSILON);
	//printf("diffG: %f\tEPSILON: %f\n", std::fabs(G - other.G), EPSILON);
	//printf("diffB: %f\tEPSILON: %f\n", std::fabs(B - other.B), EPSILON);
	//printf("-------------------\n");
	return std::fabs(X - other.X) < EPSILON
		&& std::fabs(Y - other.Y) < EPSILON
		&& std::fabs(R - other.R) < EPSILON
		&& std::fabs(G - other.G) < EPSILON
		&& std::fabs(B - other.B) < EPSILON;
}

MeanShift::Point MeanShift::Point::operator+(const MeanShift::Point& other) const {
	Point p;
	p.X = this->X + other.X;
	p.Y = this->Y + other.Y;
	p.R = this->R + other.R;
	p.G = this->G + other.G;
	p.B = this->B + other.B;
	return p;
}

MeanShift::Point MeanShift::Point::operator-(const Point& other) const {
	Point p;
	p.X = this->X - other.X;
	p.Y = this->Y - other.Y;
	p.R = this->R - other.R;
	p.G = this->G - other.G;
	p.B = this->B - other.B;
	return p;
}

MeanShift::Point MeanShift::Point::operator*(const Point& other) const {
	Point p;
	p.X = this->X * other.X;
	p.Y = this->Y * other.Y;
	p.R = this->R * other.R;
	p.G = this->G * other.G;
	p.B = this->B * other.B;
	return p;
}

MeanShift::Point MeanShift::Point::operator/(const Point& other) const {
	Point p;
	p.X = this->X / other.X;
	p.Y = this->Y / other.Y;
	p.R = this->R / other.R;
	p.G = this->G / other.G;
	p.B = this->B / other.B;
	return p;
}

double MeanShift::distSq(const Point& p1, const Point& p2) {
	Point diff = p1 - p2;
	return diff.X * diff.X
		+ diff.Y * diff.Y
		+ diff.R * diff.R
		+ diff.G * diff.G
		+ diff.B * diff.B;
}