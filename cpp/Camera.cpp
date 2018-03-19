#include<opencv2\opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture capture; // ���
	Mat frame;
	if (!capture.open(0))
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}
	if (capture.isOpened())
	{
		cout << "Video capturing has been started ..." << endl;
		int i = 0;
		while (1)
		{
			char key = waitKey(100); // p
			capture >> frame;
			imshow("[��Ƶ]", frame);

			if (key=='p')
			{
				i++;
				imshow("[��Ƭ]", frame);
				string filePath = format("E:\\pic\\pic%d.jpg", i);
				imwrite(filePath, frame);
				waitKey(50);
				destroyWindow("[��Ƭ]");
			}
			else if (key == 27)
			{
				cout << "Camera has been closed" << endl;
				return 0;
			}
		}
	}
}