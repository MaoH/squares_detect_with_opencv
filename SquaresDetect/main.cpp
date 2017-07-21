#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>


using namespace cv;
using namespace std;

const int N = 50;  
const char* wndName= "DetectSquares";  //窗口名字

//angle返回一个角的余弦（cos）；
//pt1,pt2,pt0为确定这个角的三个点，pt0为顶点；
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
	medianBlur(img, blurImg, 9);      //中值滤波

	Mat gray0(blurImg.size(), CV_8U), gray;

	vector<vector<Point>> contours;

	for (int c = 0; c < 3; c++)        //每次处理一个通道
	{
		int channels[] = { c,0 };
		mixChannels(&blurImg, 1, &gray0, 1, channels, 1);   //提取一个通道到gray0
		
		for (int l = 0; l < N; l++)            //选取阈值
		{
			if (l == 0)                        //l=0时，利用Canny滤波得到二值化图像
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

			for (size_t i = 0; i < contours.size(); i++)  //从所有的轮廓中找出接近矩形的
			{
				approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);
				
				if (approx.size() == 4 &&
					fabs(contourArea(approx)) >= 1000 &&
					isContourConvex(approx))
				{
					double maxCosine = 0;
				
					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					if (maxCosine <= 0.3)             //直角的矩形应该为0
						squares.push_back(approx);
					
				}

			}

		}

	}


}


//画出检测到的矩形物体
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
	imshow(wndName, img);
}


int main()
{
	Mat image = imread("C:/Users/maohui/Desktop/square.jpg");

	namedWindow(wndName);
	vector<vector<Point>> squares;
	findSquares(image, squares);
	drawSquares(image, squares);
	waitKey();
	return 0;
}