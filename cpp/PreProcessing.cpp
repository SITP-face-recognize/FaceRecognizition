#include<opencv2\opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <io.h>  
#include<iostream>
#include <vector> 

using namespace std;
using namespace cv;

void getFiles(string folderPath, vector<string>&files);
void detectROIandResize(string imagePath, CascadeClassifier cascade, int i);

int main()
{
	cout << "请输入需要预处理的图片所在文件夹的路径" << endl;
	string folderPath;
	folderPath = "E:\\pic";
	cout << "请输入使用的分类器路径" << endl;
	string cascadeName;
	cascadeName = "E:\\GitHub\\opencv-practice\\opencv-practice1\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
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
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
		cout << *files_iterator << endl;

	// 获取图片中人脸检测部分ROI
	i = 1;
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++,i++)
	{
		detectROIandResize(*files_iterator, cascade, i);
	}


}

void detectROIandResize(string imagePath,CascadeClassifier cascade,int i)
{
	vector<Rect> faces;
	Mat image_gray, smallImg;
	Mat image = imread(imagePath);
	if (image.empty()) cout << "Couldn't read" << imagePath << endl;

	cvtColor(image, image_gray, COLOR_BGR2GRAY); //转灰度图
	equalizeHist(image_gray,image_gray); // 直方图均衡化（增强对比度）
	cascade.detectMultiScale(image_gray, faces, 1.1, 3, 0|CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //目标大于50*50
	for (auto faces_iterator = faces.begin(); faces_iterator != faces.end(); faces_iterator++)
	{
		cout << i << endl;
		Mat faceROI = image(*faces_iterator);
		if (faceROI.cols > 100)
		{
			resize(faceROI, faceROI, Size(92, 112));
			string filePath = format("E:\\pic1\\pic%d.jpg", i);
			imwrite(filePath, faceROI);
		}
	}

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