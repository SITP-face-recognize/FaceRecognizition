#include <io.h>  
#include<iostream>
#include<string>
#include <vector> 
#include<fstream>

using namespace std;

void getFiles(string folderPath, vector<string>&files);


int main()
{
	cout << "请输入训练库所在目录" << endl;
	string folderPath;
	folderPath = "E:\\orl_faces";
	ofstream fout("E:\\orl_faces\\a.txt");
	if (!fout.is_open())
	{
		cout << "写入失败" << endl;
		return -1;
	}
	vector<string> files;
	// 获取文件夹下所有图片
	getFiles(folderPath, files);
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
		cout << *files_iterator << endl;
	int i = 0;
	int j = 0;
	for (auto files_iterator = files.begin(); files_iterator != files.end(); files_iterator++)
	{
		fout << *files_iterator << ";" << j << endl;
		i++;
		if (i == 10)
		{
			i = 0;
			j++;
		}
	}
	fout.close();
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
				if (strstr(fileinfo.name, "jpg") != NULL || strstr(fileinfo.name, "pgm") != NULL)
					files.push_back(filesFormat.assign(folderPath).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}