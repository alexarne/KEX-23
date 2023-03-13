#include "meanshift.h"

constexpr int EPSILON = 0.000001;
constexpr int MARKER_RADIUS = 1;
#define MARKER_COLOR cv::Vec3b(0, 0, 255)

struct Point {
	double X;
	double Y;
	double R;
	double G;
	double B;
	bool operator==(const Point& other) {
		return std::abs(X - other.X) < EPSILON
			&& std::abs(Y - other.Y) < EPSILON
			&& std::abs(R - other.R) < EPSILON
			&& std::abs(G - other.G) < EPSILON
			&& std::abs(B - other.B) < EPSILON;
	}
};

struct Cluster {
	Point mean;
	int count;
	int tot_R;
	int tot_G;
	int tot_B;
};

int meanShift(cv::Mat& image) {
	std::vector<Cluster> clusters;
	clusters.push_back({ {100, 50}, 1 });

	for (int row = 0; row < image.rows; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			cv::Vec3b pixel = image.at<cv::Vec3b>(row, col);
			int B = pixel[0];
			int G = pixel[1];
			int R = pixel[2];
			Point p = { col, row, R, G, B };
			Point shifted = shift(p);
			//cluster(shifted, clusters);
		}
	}

	// Mark clusters
	for (const Cluster& c : clusters) {
		drawMarker(image, c.mean);
	}

	return clusters.size();
}

Point shift(Point p) {
	return p;
}

void cluster(Point p, std::vector<Cluster>& clusters) {
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

void drawMarker(cv::Mat& image, const Point& p) {
	for (int i = -MARKER_RADIUS; i <= MARKER_RADIUS; ++i) {
		if (p.Y + i < 0 || p.Y + i >= image.rows) continue;
		for (int j = -MARKER_RADIUS; j <= MARKER_RADIUS; ++j) {
			if (p.X + j < 0 || p.X + j >= image.cols) continue;
			image.at<cv::Vec3b>(p.Y + i, p.X + j) = MARKER_COLOR;
		}
	}
}