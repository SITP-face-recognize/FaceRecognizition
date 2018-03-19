#include <opencv2/opencv.hpp>
#include<opencv2/face/facerec.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace cv;
using namespace std;
using namespace face;

int main()
{
	VideoCapture capture;
	if (!capture.open(0))
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}

	Mat frame, gray;

	CascadeClassifier cascade;
	//cascade.load("E:\\GitHub\\opencv - practice\\opencv - practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
	if (!cascade.load("E:\\GitHub\\opencv-practice\\opencv-practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml"))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return -1;
	}
	Ptr<FaceRecognizer> modelPCA = EigenFaceRecognizer::create();
	modelPCA->read("E:\\orl_faces\\MyFacePCAModel.xml");

	while (1)
	{
		capture >> frame;

		vector<Rect> faces(0);

		cvtColor(frame, gray, CV_BGR2GRAY);

		equalizeHist(gray, gray);

		cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //目标大于50*50

		Mat face;
		Point text_lb;

		for (auto faces_iterator = faces.begin(); faces_iterator != faces.end(); faces_iterator++)
		{
			if ((*faces_iterator).height > 0 && (*faces_iterator).width > 0)
			{
				face = gray(*faces_iterator);
				text_lb = Point((*faces_iterator).x, (*faces_iterator).y);
				rectangle(frame, *faces_iterator, Scalar(255, 0, 0), 1, 8, 0);
			}
		}

		Mat face_test;

		int predictPCA = 0;
		if (face.rows >= 120)
			resize(face, face_test, Size(92, 112));

		if (!face_test.empty())
			predictPCA = modelPCA->predict(face_test);

		cout << predictPCA << endl;
		string name = "yi-yezi";
		if (predictPCA == 40)
		{
			putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
		}

		imshow("face", frame);
		waitKey(50);
	}

}