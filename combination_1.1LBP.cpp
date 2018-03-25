#include<opencv2\opencv.hpp>
#include <opencv2/imgproc.hpp>
#include<opencv2/face/facerec.hpp>
#include <iostream>
#include<direct.h>
#include<io.h>
#include <vector> 
#include<windows.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string> 
using namespace cv;
using namespace std;
using namespace face;

int shoot();
int preprocessing();
int csv();
int mmodel_training();
int faceRecognition();
void initiallize();
int updata();


int main()
{
	initiallize();
	char inputKey;
	while (1) {
	
	
	    /*
		   ����˵����
		   ���������Ƭ���浽���أ���Ҫѵ������ܽ���ʶ���ʺ���һ����Ҫ¼��϶�������¼���ͳһ����ѵ��
		   ѵ�������ļ���������xml�ļ�����ѵ���ļ����е���Ƭ
		   ¼��һ���µ������Ὣһ���µ��������µ�xml�ļ��У��ʺ��ڲ�¼��������
		*/
		cout << "��ѡ����" << endl;
		cout << "����----------------0" << endl;
		cout << "ѵ�������ļ�--------1" << endl;
		cout << "¼��һ���µ�����----2" << endl;
		cout << "ʶ��----------------3" << endl;
		cout << "�˳�----------------4" << endl;
		cin >> inputKey;
		switch (inputKey) {
		case '0':shoot(); break;
		case '1': preprocessing(); csv();  mmodel_training(); break;
		case '2': updata(); break;
		case '3': faceRecognition(); break;
		case '4':return 0;
		default:break;
		}		
	}
}

string getTag(string s) {
	;             //����·����ȡ�ļ���ǰ��λ��ѧ�ţ�
	int pos;
	pos = s.find_last_of('\\');
	return s.substr(pos + 1, 7);
}

void getFiles(string folderPath, vector<string>&files)
{
	//�ļ����  
	long long hFile = 0;
	//�ļ���Ϣ������һ���洢�ļ���Ϣ�Ľṹ��  
	struct _finddata_t fileinfo;
	string filesFormat;

	if ((hFile = _findfirst(filesFormat.assign(folderPath).append("\\*").c_str(), &fileinfo)) != -1)//�����ҳɹ��������
	{
		do
		{
			//�����Ŀ¼,�ݹ飨���ļ����ڻ����ļ��У�  
			if (fileinfo.attrib& _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(filesFormat.assign(folderPath).append("\\").append(fileinfo.name), files);

				}
			}
			else
			{
				files.push_back(filesFormat.assign(folderPath).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void white_balance(Mat g_srcImage, Mat &dstImage) {

	vector<Mat> g_vChannels;
	split(g_srcImage, g_vChannels);
	Mat imageBlueChannel = g_vChannels.at(0);
	Mat imageGreenChannel = g_vChannels.at(1);
	Mat imageRedChannel = g_vChannels.at(2);

	double imageBlueChannelAvg = 0;
	double imageGreenChannelAvg = 0;
	double imageRedChannelAvg = 0;

	//���ͨ����ƽ��ֵ
	imageBlueChannelAvg = mean(imageBlueChannel)[0];
	imageGreenChannelAvg = mean(imageGreenChannel)[0];
	imageRedChannelAvg = mean(imageRedChannel)[0];

	//�����ͨ����ռ����
	double K = (imageRedChannelAvg + imageGreenChannelAvg + imageRedChannelAvg) / 3;
	double Kb = K / imageBlueChannelAvg;
	double Kg = K / imageGreenChannelAvg;
	double Kr = K / imageRedChannelAvg;

	//���°�ƽ���ĸ�ͨ��BGRֵ
	addWeighted(imageBlueChannel, Kb, 0, 0, 0, imageBlueChannel);
	addWeighted(imageGreenChannel, Kg, 0, 0, 0, imageGreenChannel);
	addWeighted(imageRedChannel, Kr, 0, 0, 0, imageRedChannel);

	merge(g_vChannels, dstImage);//ͼ���ͨ���ϲ�
}

void detectROIandResize(string imagePath, CascadeClassifier cascade, int i)
{
	vector<Rect> faces;
	Mat image_gray, smallImg;
	Mat image = imread(imagePath);
	if (image.empty()) cout << "Couldn't read" << imagePath << endl;
	//white_balance(image, image_gray);
	cvtColor(image, image_gray, COLOR_BGR2GRAY); //ת�Ҷ�ͼ
	equalizeHist(image_gray, image_gray); // ֱ��ͼ���⻯����ǿ�Աȶȣ�
	cascade.detectMultiScale(image_gray, faces, 1.1, 3, 0 | CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //Ŀ�����50*50
	for (auto faces_iterator = faces.begin(); faces_iterator != faces.end(); faces_iterator++)
	{
		//cout << i << endl;
		Mat faceROI = image_gray(*faces_iterator);
		if (faceROI.cols > 0)
		{
			//equalizeHist(faceROI, faceROI); // ֱ��ͼ���⻯����ǿ�Աȶȣ�
			//cvtColor(faceROI, faceROI, COLOR_BGR2GRAY); //ת�Ҷ�ͼ
			resize(faceROI, faceROI, Size(92, 112));
			string filePath = "E:\\pic1\\" + getTag(imagePath) + format("pic%d.jpg", i);
			imwrite(filePath, faceROI);
		}
	}

}

//�ж��ļ��Ƿ�Ϊ��
bool  CheckFolderExist(const string &strPath)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;
}

void initiallize() {
	string path = "D:\\pic";
	if (CheckFolderExist(path) == 0)
		_mkdir(path.c_str());
	path = "D:\\pic1";
	if (CheckFolderExist(path) == 0)
		_mkdir(path.c_str());
	path = "D:\\orl_faces";
	if (CheckFolderExist(path) == 0)
		_mkdir(path.c_str());

}

//�����ɼ�
int shoot() {
	string tag;
	VideoCapture capture; // ���
	Mat frame;
	int cnt = 0;
	cout << "������ѧ��" << endl;
	cin >> tag;
	if (!capture.open(1)) ///�ҵ�0����������������������ʹ��1�ţ����쳣�����Ϊ!capture.open(0)
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}
	if (capture.isOpened())
	{
		cout << "Video capturing has been started ..." << endl;
		int i = 0;
		int j = 0;
		int pic_num = 0;
		////////////////
		string dirPath;
		while (1) {
			dirPath = format("E:\\pic\\people%d", j);
			if (CheckFolderExist(dirPath) == 1) {     ///�ļ��Ѿ�����_access(dirPath.c_str(), 0) == -1
				j++;
			}
			else {
				int status = _mkdir(dirPath.c_str());
				break;
			}

		}
		///////////////////
		cout << "�밴p���գ���ʮ��,��esc�˳�" << endl;
		while (1)
		{
			char key = waitKey(100); // p
			capture >> frame;
			imshow("[��Ƶ]", frame);

			if (key == 'p')
			{
				i++;

				imshow("[��Ƭ]", frame);
				string filePath = dirPath + "\\" + tag + format("pic%d.jpg", i);
				cout << filePath << endl << dirPath << endl;
				imwrite(filePath, frame);
				waitKey(50);
				destroyWindow("[��Ƭ]");
				if (i == 10) { destroyWindow("[��Ƶ]"); return 0; }
			}
			else if (key == 27)
			{
				cout << "Camera has been closed" << endl;
				destroyWindow("[��Ƶ]");
				return 0;
			}

		}
	}

}

int preprocessing() {
	cout << "Ԥ����..." << endl;

	//cout << "��������ҪԤ�����ͼƬ�����ļ��е�·��" << endl;
	string folderPath;
	folderPath = "E:\\pic";
	//cout << "������ʹ�õķ�����·��" << endl;
	string cascadeName;
	cascadeName = "D:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";//"E:\\GitHub\\opencv-practice\\opencv-practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	vector<string> files;
	CascadeClassifier cascade;
	int i;
	if (!cascade.load(cascadeName))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return -1;
	}
	// ��ȡ�ļ���������ͼƬ
	getFiles(folderPath, files);
	/*��ʾ��ȡ���ļ�
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
	cout << *files_iterator << endl;
	*/

	// ��ȡͼƬ��������ⲿ��ROI
	i = 1;
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++, i++)
	{
		detectROIandResize(*files_iterator, cascade, i);
	}

}

int csv() {
	cout << "����csv..." << endl;
	//cout << "������ѵ��������Ŀ¼" << endl;
	string folderPath;
	folderPath = "E:\\pic1";
	ofstream fout("E:\\orl_faces\\a.txt");
	if (!fout.is_open())
	{
		cout << "д��ʧ��" << endl;
		return -1;
	}
	vector<string> files;
	// ��ȡ�ļ���������ͼƬ
	getFiles(folderPath, files);

	/*��ʾ�����ļ���
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
	cout << *files_iterator << endl;
	*/
	int i = 0;
	int j = 0;

	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
	{
		string s = *files_iterator;
		fout << *files_iterator << ";" << getTag(s) << endl;

		i++;
		if (i == 10)
		{
			i = 0;
			j++;
		}
	}
	fout.close();



}

///training

static Mat norm_0_255(InputArray _src) {
	Mat src = _src.getMat();
	// �����ͷ���һ����һ�����ͼ�����:
	Mat dst;
	switch (src.channels()) {
	case1:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case3:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

int mmodel_training()
{
	cout << "ѵ����..." << endl;
	//��ȡ���CSV�ļ�·��.
	//string fn_csv = string(argv[1]);
	string fn_csv = "E:\\orl_faces\\a.txt";
	// 2�����������ͼ�����ݺͶ�Ӧ�ı�ǩ
	vector<Mat> images;
	vector<int> labels;
	// ��ȡ����. ����ļ����Ϸ��ͻ����
	// ������ļ����Ѿ�����.
	try
	{
		read_csv(fn_csv, images, labels);
	}
	catch (cv::Exception& e)
	{
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// �ļ������⣬����ɶҲ�������ˣ��˳���
		exit(1);
	}
	// ���û�ж�ȡ���㹻ͼƬ��Ҳ�˳�.
	if (images.size() <= 1) {
		string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(CV_StsError, error_message);
	}

	// ����ļ��д�������Ǵ�������ݼ����Ƴ����һ��ͼƬ
	//[gm:��Ȼ������Ҫ�����Լ�����Ҫ�޸ģ���������˺ܶ�����]
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();
	// ���漸�д�����һ��������ģ����������ʶ��
	// ͨ��CSV�ļ���ȡ��ͼ��ͱ�ǩѵ������
	// T������һ��������PCA�任
	//�����ֻ�뱣��10�����ɷ֣�ʹ�����´���
	//      cv::createEigenFaceRecognizer(10);
	//
	// ����㻹ϣ��ʹ�����Ŷ���ֵ����ʼ����ʹ��������䣺
	//      cv::createEigenFaceRecognizer(10, 123.0);
	//
	// �����ʹ��������������ʹ��һ����ֵ��ʹ��������䣺
	//      cv::createEigenFaceRecognizer(0, 123.0);

	//Ptr<FaceRecognizer> model = EigenFaceRecognizer::create();
	//model->train(images, labels);
	//model->save("MyFacePCAModel.xml");
	//���ַ���ѡһ�ּ���
	Ptr<FaceRecognizer> model1 = FisherFaceRecognizer::create();
	model1->train(images, labels);
	model1->save("MyFaceFisherModel.xml");
	
	//Ptr<FaceRecognizer> model2 = LBPHFaceRecognizer::create();
	//model2->train(images, labels);
	//model2->save("MyFaceLBPHModel.xml");
	



	// ����Բ���ͼ�����Ԥ�⣬predictedLabel��Ԥ���ǩ���
	/*
	int predictedLabel = model->predict(testSample);

	int predictedLabel1 = model1->predict(testSample);
	int predictedLabel2 = model2->predict(testSample);

	// ����һ�ֵ��÷�ʽ�����Ի�ȡ���ͬʱ�õ���ֵ:
	//      int predictedLabel = -1;
	//      double confidence = 0.0;
	//      model->predict(testSample, predictedLabel, confidence);

	string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
	string result_message1 = format("Predicted class = %d / Actual class = %d.", predictedLabel1, testLabel);
	string result_message2 = format("Predicted class = %d / Actual class = %d.", predictedLabel2, testLabel);
	cout << result_message << endl;
	cout << result_message1 << endl;
	cout << result_message2 << endl;
	*/

	waitKey(0);
	return 0;
}

int faceRecognition()
{
	VideoCapture capture;
	if (!capture.open(1))
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}

	Mat frame, gray;

	CascadeClassifier cascade;
	//cascade.load("E:\\GitHub\\opencv - practice\\opencv - practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
	if (!cascade.load("D:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml"))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return -1;
	}
	//Ptr<FaceRecognizer> modelPCA = EigenFaceRecognizer::create();
	Ptr<FaceRecognizer> modelLBP = LBPHFaceRecognizer::create();
	//Ptr<FaceRecognizer> modelFisher = FisherFaceRecognizer::create();
	//modelPCA->read("MyFacePCAModel.xml");
	modelLBP->read("MyFaceLBPHModel.xml");
	//modelFisher->read("MyFaceFisherModel.xml");
	while (1)
	{
		capture >> frame;
		char key = waitKey(100);
		vector<Rect> faces(0);
		//white_balance(frame, gray);
		cvtColor(frame, gray, CV_BGR2GRAY);
		equalizeHist(gray, gray);
		cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //Ŀ�����50*50
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
		double confidence = 0;
		double confidence1 = 0;
		double confidence2 = 0;
		int predictPCA = 0;
		int predictLBP= 0;
		int predictFisher = 0;

		if (face.rows >= 120)
			resize(face, face_test, Size(92, 112));

		if (!face_test.empty()) {
			//modelPCA->predict(face_test, predictPCA, confidence);
			modelLBP->predict(face_test, predictLBP, confidence1);
			//modelFisher->predict(face_test, predictFisher, confidence2);
		}


		    cout <<"LBP" <<predictLBP << "   ���Ŷ�:" << confidence1 << endl;
			//cout << "LBP" << predictPCA << "   ���Ŷ�:" << confidence << endl;
			//cout << "FSH" << predictPCA << "   ���Ŷ�:" << confidence2 << endl;

		putText(frame, to_string(predictLBP), text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
		imshow("face", frame);
		if (key == 27) {
			destroyWindow("face");
			break;
		}
	}
}


int updata() {
	int tag;
	VideoCapture capture;
	cout << "���������ѧ��" << endl;
	cin >> tag;


	cout << "�밴p���գ���ʮ��,��esc�˳�" << endl;
	if (!capture.open(1))
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}

	////////////////���ڴ浵���ݣ���ɾȥ�˲���
	string dirPath;
	int j = 0;
	while (1) {
		dirPath = format("E:\\pic\\people%d", j);
		if (CheckFolderExist(dirPath) == 1) {     ///�ļ��Ѿ�����_access(dirPath.c_str(), 0) == -1
			j++;
		}
		else {
			int status = _mkdir(dirPath.c_str());
			break;
		}

	}
	///////////////////////////���ڴ浵���ݣ���ɾ��

	Mat frame, gray;
	CascadeClassifier cascade;
	//cascade.load("E:\\GitHub\\opencv - practice\\opencv - practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml");
	if (!cascade.load("D:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml"))
	{
		cerr << "ERROR: Could not load classifier cascade" << endl;
		return -1;
	}

	Ptr<FaceRecognizer> modelLBP = LBPHFaceRecognizer::create();
	modelLBP->read("MyFaceLBPHModel.xml");


	vector<Mat> newImages;
	vector<int> label;
	int cnt = 0;

	while (1)
	{
		capture >> frame;
		char key = waitKey(100);
		vector<Rect> faces(0);
		//white_balance(frame, gray);
		
		cvtColor(frame, gray, CV_BGR2GRAY);
		equalizeHist(gray, gray);
		cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //Ŀ�����50*50
		
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
		double confidence = 0;
		int predictPCA = 0;
		double confidencelbp = 0;
		int predictLBP = 0;
		if (face.rows >= 120)
			resize(face, face_test, Size(92, 112));

		imshow("¼��", frame);  /////////���ݵ����أ���ɾ��
		string filePath;     /////////���ݵ����أ���ɾ��
		if (key == 'p') {
			newImages.push_back(face_test);
			label.push_back(tag);
			filePath = dirPath + "\\" + to_string(tag) + format("pic%d.jpg", cnt++);/////////���ݵ����أ���ɾ��
			imwrite(filePath, frame);//////////���ݵ����أ���ɾ��

			cout << "��" << cnt << "��¼��" << endl;
		}
		else if (key == 27)
		{
			cout << "Camera has been closed" << endl;
			destroyWindow("¼��");
			return 0;
		}
		if (cnt == 10) {
			cout << "¼����ϣ�������...." << endl;
			cvDestroyWindow("¼��");
			break;
		}

	}

	modelLBP->update(newImages, label);
	modelLBP->save("MyFaceLBPHModel.xml");
	return 0;
}