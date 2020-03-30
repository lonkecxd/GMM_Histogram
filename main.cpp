#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct MousePassParams
{
	Mat image;
	vector<Mat> frames;
};
//鼠标点击事件callback函数
static void onMouse(int event, int x, int y, int flag, void* param)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	cout << "坐标：" << x << ", " <<y<<"\n";

	MousePassParams* mp = (MousePassParams*)param;
	Mat& image = mp->image;
	vector<Mat> frames = mp->frames;
	circle(image, Point(x, y), 1, Scalar(255), 5); //绘制鼠标点击位置
	imshow("image", image);

	//绘制直方图
	vector<int> per_pixel_blue;
	vector<int> per_pixel_green;
	vector<int> per_pixel_red;
	int total_frames = 150;
	for (int i = 0; i < total_frames; i++)
	{
		Mat frame = frames[i];
		per_pixel_blue.push_back(frame.at<Vec3b>(y, x)[0]);  //取鼠标位置像素,蓝色通道值
		per_pixel_green.push_back(frame.at<Vec3b>(y, x)[1]);  //取鼠标位置像素,绿色通道值
		per_pixel_red.push_back(frame.at<Vec3b>(y, x)[2]);  //取鼠标位置像素,红色通道值
	}

	Mat histogram_blue = Mat::zeros(Size(256, 1), CV_32SC1);
	Mat histogram_green = Mat::zeros(Size(256, 1), CV_32SC1);
	Mat histogram_red = Mat::zeros(Size(256, 1), CV_32SC1);
	for (int r = 0; r < total_frames; r++)
	{
		int blue_scale = int(per_pixel_blue[r]);
		int green_scale = int(per_pixel_green[r]);
		int red_scale = int(per_pixel_red[r]);
		histogram_blue.at<int>(0, blue_scale) += 1;
		histogram_green.at<int>(0, green_scale) += 1;
		histogram_red.at<int>(0, red_scale) += 1;
	}

	Mat dstImageBlue(total_frames, 256, CV_8UC3, Scalar(0));
	Mat dstImageGreen(total_frames, 256, CV_8UC3, Scalar(0));
	Mat dstImageRed(total_frames, 256, CV_8UC3, Scalar(0));
	for (int i = 0; i < 256; i++)
	{
		float blueValue = histogram_blue.at<int>(0, i);
		float greenValue = histogram_green.at<int>(0, i);
		float redValue = histogram_red.at<int>(0, i);
		line(dstImageBlue, Point(i, total_frames), Point(i, total_frames - blueValue), Scalar(255, 0, 0));
		line(dstImageGreen, Point(i, total_frames), Point(i, total_frames - greenValue), Scalar(0, 255, 0));
		line(dstImageRed, Point(i, total_frames), Point(i, total_frames - redValue), Scalar(0, 0, 255));
	}
	imshow("Hist Blue", dstImageBlue);
	imshow("Hist Green", dstImageGreen);
	imshow("Hist Red", dstImageRed);
}

int main()
{
	VideoCapture cap("../../background.mp4");
	if (!cap.isOpened())
		return -1;

	vector<Mat> frames;

	for (;;) {
		Mat frame;
		cap >> frame;
		frames.push_back(frame);
		imshow("Video", frame);
		if(waitKey(30)>=0)
			break;
	}

	namedWindow("image");  //建立一个鼠标可以点击的窗口。
	Mat show_frame;
	cap >> show_frame;
	MousePassParams mp;  //为了传入image和frames，建立一个struct来存储。
	mp.image = show_frame;
	mp.frames = frames;
	setMouseCallback("image", onMouse, (void*)&mp);
	imshow("image", show_frame);

	waitKey();
	return 0;
}