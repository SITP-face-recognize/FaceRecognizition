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
	cout << "��������ҪԤ�����ͼƬ�����ļ��е�·��" << endl;
	string folderPath;
	folderPath = "E:\\pic";
	cout << "������ʹ�õķ�����·��" << endl;
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
	// ��ȡ�ļ���������ͼƬ
	getFiles(folderPath, files);
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
		cout << *files_iterator << endl;

	// ��ȡͼƬ��������ⲿ��ROI
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

	cvtColor(image, image_gray, COLOR_BGR2GRAY); //ת�Ҷ�ͼ
	equalizeHist(image_gray,image_gray); // ֱ��ͼ���⻯����ǿ�Աȶȣ�
	cascade.detectMultiScale(image_gray, faces, 1.1, 3, 0|CASCADE_DO_ROUGH_SEARCH, Size(50, 50)); //Ŀ�����50*50
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