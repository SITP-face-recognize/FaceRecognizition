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
		   功能说明：
		   照相仅将照片保存到本地，需要训练后才能进行识别，适合于一次需要录入较多人脸，录入后统一进行训练
		   训练本地文件会清重置xml文件，并训练文件夹中的照片
		   录入一张新的人脸会将一张新的人脸更新到xml文件中，适合于补录少数人脸
		*/
		cout << "请选择功能" << endl;
		cout << "照相----------------0" << endl;
		cout << "训练本地文件--------1" << endl;
		cout << "录入一张新的人脸----2" << endl;
		cout << "识别----------------3" << endl;
		cout << "退出----------------4" << endl;
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
	;             //给定路径获取文件名前六位（学号）
	int pos;
	pos = s.find_last_of('\\');
	return s.substr(pos + 1, 7);
}

void getFiles(string folderPath, vector<string>&files)
{
	//文件句柄  
	long long hFile = 0;
	//文件信息，声明一个存储文件信息的结构体  
	struct _finddata_t fileinfo;
	string filesFormat;

	if ((hFile = _findfirst(filesFormat.assign(folderPath).append("\\*").c_str(), &fileinfo)) != -1)//若查找成功，则进入
	{
		do
		{
			//如果是目录,递归（即文件夹内还有文件夹）  
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

	//求各通道的平均值
	imageBlueChannelAvg = mean(imageBlueChannel)[0];
	imageGreenChannelAvg = mean(imageGreenChannel)[0];
	imageRedChannelAvg = mean(imageRedChannel)[0];

	//求出个通道所占增益
	double K = (imageRedChannelAvg + imageGreenChannelAvg + imageRedChannelAvg) / 3;
	double Kb = K / imageBlueChannelAvg;
	double Kg = K / imageGreenChannelAvg;
	double Kr = K / imageRedChannelAvg;

	//更新白平衡后的各通道BGR值
	addWeighted(imageBlueChannel, Kb, 0, 0, 0, imageBlueChannel);
	addWeighted(imageGreenChannel, Kg, 0, 0, 0, imageGreenChannel);
	addWeighted(imageRedChannel, Kr, 0, 0, 0, imageRedChannel);

	merge(g_vChannels, dstImage);//图像各通道合并
}

void detectROIandResize(string imagePath, CascadeClassifier cascade, int i)
{
	vector<Rect> faces;
	Mat image_gray, smallImg;
	Mat image = imread(imagePath);
	if (image.empty()) cout << "Couldn't read" << imagePath << endl;
	//white_balance(image, image_gray);
	cvtColor(image, image_gray, COLOR_BGR2GRAY); //转灰度图
	equalizeHist(image_gray, image_gray); // 直方图均衡化（增强对比度）
	cascade.detectMultiScale(image_gray, faces, 1.1, 3, 0 | CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //目标大于50*50
	for (auto faces_iterator = faces.begin(); faces_iterator != faces.end(); faces_iterator++)
	{
		//cout << i << endl;
		Mat faceROI = image_gray(*faces_iterator);
		if (faceROI.cols > 0)
		{
			//equalizeHist(faceROI, faceROI); // 直方图均衡化（增强对比度）
			//cvtColor(faceROI, faceROI, COLOR_BGR2GRAY); //转灰度图
			resize(faceROI, faceROI, Size(92, 112));
			string filePath = "E:\\pic1\\" + getTag(imagePath) + format("pic%d.jpg", i);
			imwrite(filePath, faceROI);
		}
	}

}

//判断文件是否为空
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

//人脸采集
int shoot() {
	string tag;
	VideoCapture capture; // 相机
	Mat frame;
	int cnt = 0;
	cout << "请输入学号" << endl;
	cin >> tag;
	if (!capture.open(1)) ///我的0号相机不是正常相机，所以使用1号，若异常，则改为!capture.open(0)
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
			if (CheckFolderExist(dirPath) == 1) {     ///文件已经存在_access(dirPath.c_str(), 0) == -1
				j++;
			}
			else {
				int status = _mkdir(dirPath.c_str());
				break;
			}

		}
		///////////////////
		cout << "请按p拍照，共十张,或按esc退出" << endl;
		while (1)
		{
			char key = waitKey(100); // p
			capture >> frame;
			imshow("[视频]", frame);

			if (key == 'p')
			{
				i++;

				imshow("[照片]", frame);
				string filePath = dirPath + "\\" + tag + format("pic%d.jpg", i);
				cout << filePath << endl << dirPath << endl;
				imwrite(filePath, frame);
				waitKey(50);
				destroyWindow("[照片]");
				if (i == 10) { destroyWindow("[视频]"); return 0; }
			}
			else if (key == 27)
			{
				cout << "Camera has been closed" << endl;
				destroyWindow("[视频]");
				return 0;
			}

		}
	}

}

int preprocessing() {
	cout << "预处理..." << endl;

	//cout << "请输入需要预处理的图片所在文件夹的路径" << endl;
	string folderPath;
	folderPath = "E:\\pic";
	//cout << "请输入使用的分类器路径" << endl;
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
	// 获取文件夹下所有图片
	getFiles(folderPath, files);
	/*显示读取的文件
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
	cout << *files_iterator << endl;
	*/

	// 获取图片中人脸检测部分ROI
	i = 1;
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++, i++)
	{
		detectROIandResize(*files_iterator, cascade, i);
	}

}

int csv() {
	cout << "生成csv..." << endl;
	//cout << "请输入训练库所在目录" << endl;
	string folderPath;
	folderPath = "E:\\pic1";
	ofstream fout("E:\\orl_faces\\a.txt");
	if (!fout.is_open())
	{
		cout << "写入失败" << endl;
		return -1;
	}
	vector<string> files;
	// 获取文件夹下所有图片
	getFiles(folderPath, files);

	/*显示所有文件名
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
	// 创建和返回一个归一化后的图像矩阵:
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
	cout << "训练中..." << endl;
	//读取你的CSV文件路径.
	//string fn_csv = string(argv[1]);
	string fn_csv = "E:\\orl_faces\\a.txt";
	// 2个容器来存放图像数据和对应的标签
	vector<Mat> images;
	vector<int> labels;
	// 读取数据. 如果文件不合法就会出错
	// 输入的文件名已经有了.
	try
	{
		read_csv(fn_csv, images, labels);
	}
	catch (cv::Exception& e)
	{
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// 文件有问题，我们啥也做不了了，退出了
		exit(1);
	}
	// 如果没有读取到足够图片，也退出.
	if (images.size() <= 1) {
		string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		CV_Error(CV_StsError, error_message);
	}

	// 下面的几行代码仅仅是从你的数据集中移除最后一张图片
	//[gm:自然这里需要根据自己的需要修改，他这里简化了很多问题]
	Mat testSample = images[images.size() - 1];
	int testLabel = labels[labels.size() - 1];
	images.pop_back();
	labels.pop_back();
	// 下面几行创建了一个特征脸模型用于人脸识别，
	// 通过CSV文件读取的图像和标签训练它。
	// T这里是一个完整的PCA变换
	//如果你只想保留10个主成分，使用如下代码
	//      cv::createEigenFaceRecognizer(10);
	//
	// 如果你还希望使用置信度阈值来初始化，使用以下语句：
	//      cv::createEigenFaceRecognizer(10, 123.0);
	//
	// 如果你使用所有特征并且使用一个阈值，使用以下语句：
	//      cv::createEigenFaceRecognizer(0, 123.0);

	//Ptr<FaceRecognizer> model = EigenFaceRecognizer::create();
	//model->train(images, labels);
	//model->save("MyFacePCAModel.xml");
	//三种方法选一种即可
	Ptr<FaceRecognizer> model1 = FisherFaceRecognizer::create();
	model1->train(images, labels);
	model1->save("MyFaceFisherModel.xml");
	
	//Ptr<FaceRecognizer> model2 = LBPHFaceRecognizer::create();
	//model2->train(images, labels);
	//model2->save("MyFaceLBPHModel.xml");
	



	// 下面对测试图像进行预测，predictedLabel是预测标签结果
	/*
	int predictedLabel = model->predict(testSample);

	int predictedLabel1 = model1->predict(testSample);
	int predictedLabel2 = model2->predict(testSample);

	// 还有一种调用方式，可以获取结果同时得到阈值:
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


		    cout <<"LBP" <<predictLBP << "   置信度:" << confidence1 << endl;
			//cout << "LBP" << predictPCA << "   置信度:" << confidence << endl;
			//cout << "FSH" << predictPCA << "   置信度:" << confidence2 << endl;

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
	cout << "请输入你的学号" << endl;
	cin >> tag;


	cout << "请按p拍照，共十张,或按esc退出" << endl;
	if (!capture.open(1))
	{
		cout << "Capture didn't work" << endl;
		return -1;
	}

	////////////////用于存档备份，可删去此部分
	string dirPath;
	int j = 0;
	while (1) {
		dirPath = format("E:\\pic\\people%d", j);
		if (CheckFolderExist(dirPath) == 1) {     ///文件已经存在_access(dirPath.c_str(), 0) == -1
			j++;
		}
		else {
			int status = _mkdir(dirPath.c_str());
			break;
		}

	}
	///////////////////////////用于存档备份，可删除

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
		double confidence = 0;
		int predictPCA = 0;
		double confidencelbp = 0;
		int predictLBP = 0;
		if (face.rows >= 120)
			resize(face, face_test, Size(92, 112));

		imshow("录入", frame);  /////////备份到本地，可删除
		string filePath;     /////////备份到本地，可删除
		if (key == 'p') {
			newImages.push_back(face_test);
			label.push_back(tag);
			filePath = dirPath + "\\" + to_string(tag) + format("pic%d.jpg", cnt++);/////////备份到本地，可删除
			imwrite(filePath, frame);//////////备份到本地，可删除

			cout << "第" << cnt << "张录入" << endl;
		}
		else if (key == 27)
		{
			cout << "Camera has been closed" << endl;
			destroyWindow("录入");
			return 0;
		}
		if (cnt == 10) {
			cout << "录入完毕，处理中...." << endl;
			cvDestroyWindow("录入");
			break;
		}

	}

	modelLBP->update(newImages, label);
	modelLBP->save("MyFaceLBPHModel.xml");
	return 0;
}