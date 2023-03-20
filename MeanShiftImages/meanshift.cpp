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

	Point& operator-=(const Point& other) {
		X -= other.X;
		Y -= other.Y;
		R -= other.R;
		G -= other.G;
		B -= other.B;
		return (*this);
	}

	Point& operator-(const Point& other) {
		Point sP = (*this);
		sP.X -= other.X;
		sP.Y -= other.Y;
		sP.R -= other.R;
		sP.G -= other.G;
		sP.B -= other.B;
		return sP;
	}

	Point& operator+=(const Point& other) {
		X += other.X;
		Y += other.Y;
		R += other.R;
		G += other.G;
		B += other.B;
		return (*this);
	}

	Point& abs() {
		if (X < 0) X = -X;
		if (Y < 0) Y = -Y;
		if (R < 0) R = -R;
		if (G < 0) G = -G;
		if (B < 0) B = -B;

		return (*this);
	}

	Point& operator/(const Point& other) {
		Point dP = *this;
		dP.X /= other.X;
		dP.Y /= other.Y;
		dP.R /= other.R;
		dP.G /= other.G;
		dP.B /= other.B;

		return (*this);
	}

	Point& operator*(const double d) {
		Point mP = *this;
		mP.X *= d;
		mP.Y *= d;
		mP.R *= d;
		mP.G *= d;
		mP.B *= d;

		return mP;
	}

	Point& operator*(const Point& other){
		Point mP = *this;
		mP.X *= other.X;
		mP.Y *= other.Y;
		mP.R *= other.R;
		mP.G *= other.G;
		mP.B *= other.B;

		return mP;
	}

	Point& exp() {
		X = std::exp(X);
		Y = std::exp(Y);
		R = std::exp(R);
		G = std::exp(G);
		B = std::exp(B);
		return (*this);
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
			Point shifted = shift(p, image);
			cluster(shifted, clusters);
		}
	}

	// Mark clusters
	for (const Cluster& c : clusters) {
		drawMarker(image, c.mean);
		printf("debug 2");

	}
	printf("debug 3");
	return clusters.size();
}

Point cylindricalKernelFunction(Point x, Point xi, double c) {
	Point distSqrd = (xi - x) * (xi - x) * -c;
	return distSqrd.exp();
}

Point shift(Point p, const cv::Mat& image) {
	// 1 ) get neighbourhood of pixels


	// Naive implementation iterate through all pixels and check a given condition
	// Naive vector, as we know the size of array necessary for a given bandwidth
	std::vector<Point> neighbourhood;
	Point gradientSumUpper;
	Point gradientSumLower;
	int bandwidth_hs = 3;
	int bandwidth_hr = 2;
	double C = 2;
	int iterations = 3;
	Point e = { 1, 1, 1, 1, 1 };
	Point n;
	cv::Vec3b pixel;
	Point kernelVal;

	for (int i = 0; i < iterations; i++)
	{
		for (int row = 0; row < image.rows; ++row) {
			for (int col = 0; col < image.cols; ++col) {
				int absX = std::abs(col - p.Y);
				int absY = std::abs(row - p.X);

				// if we do cylindrical we only care about hs (not colour space)!
				if (absX * absX + absY * absY <= bandwidth_hs * bandwidth_hs) {
					pixel = image.at<cv::Vec3b>(row, col);
					n = { (double)col, (double)row, (double)pixel[0], (double)pixel[1], (double)pixel[2]};

					kernelVal = n / (e * std::sqrt(absX * absX + absY * absY));
					gradientSumLower += kernelVal;
					gradientSumUpper += kernelVal * n;
				}
			}
		}

		p = gradientSumUpper / gradientSumLower - p;
	}

	// 2 ) Find distance with distance metric to all (usually euclidean)

	//  3) With euclidean distance the following kernel is possible: 
	// kernel K(x) = (C / (hs*hp)) * k (||xs/hs||^2) * k * (||xr/hr||^2)
	// C <- Normalization constant
	// hs, hp <- bandwidth for spatial and spectral feature spaces

	// 4) Calculate gradient

	// 5) return to 1, shift p by gradient a fixed number of iterations

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