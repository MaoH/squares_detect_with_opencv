#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>


using namespace cv;
using namespace std;

const int N = 50;

double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;

	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;

	double cosine = (dx1*dx2 + dy1*dy2) / (sqrt(dx1*dx1 + dy1*dy1) * sqrt(dx2*dx2 + dy2*dy2) + 1e-10);

	return cosine;
}


void findSquares(Mat& img,vector<vector<Point>>& squares)
{
	squares.clear();

	Mat blurImg(img);
	medianBlur(img, blurImg, 9);

	Mat gray0(blurImg.size(), CV_8U), gray;

	vector<vector<Point>> contours;

	for (int c = 0; c < 3; c++)
	{
		int channels[] = { c,0 };
		mixChannels(&blurImg, 1, &gray0, 1, channels, 1);
		
		for (int l = 0; l < N; l++)
		{
			if (l == 0)
			{
				Canny(gray0, gray, 5, 50, 5);

				dilate(gray, gray, Mat());
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / N;
			}

			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			
			vector<Point> approx;

			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);
				
				if (approx.size() == 4 &&
					fabs(contourArea(approx)) >= 1000 &&
					isContourConvex(approx))
				{
					double maxCosine = 0;
					cout << approx;
					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					if (maxCosine <= 0.3)
						squares.push_back(approx);
					
				}

			}

		}

	}


}

void drawSquares(Mat& img, vector<vector<Point>>& squares)
{
	polylines(img, squares, true, Scalar(0, 255, 0), 3, LINE_AA);
	/*
	for (size_t i = 0; i < squares.size(); i++)
	{
		Point* pts = &squares[i][0];
		int npt = squares[i].size();
		polylines(img, &pts, &npt, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
		
	}
	*/
	imshow("findSquares", img);
}


int main()
{
	Mat image = imread("C:/Users/maohui/Desktop/square.jpg");

	namedWindow("findSquares");
	//imshow("findSquares", image);
	vector<vector<Point>> squares;
	findSquares(image, squares);
	drawSquares(image, squares);
	waitKey();
}