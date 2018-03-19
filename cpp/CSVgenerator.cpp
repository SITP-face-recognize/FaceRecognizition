#include <io.h>  
#include<iostream>
#include<string>
#include <vector> 
#include<fstream>

using namespace std;

void getFiles(string folderPath, vector<string>&files);


int main()
{
	cout << "������ѵ��������Ŀ¼" << endl;
	string folderPath;
	folderPath = "E:\\orl_faces";
	ofstream fout("E:\\orl_faces\\a.txt");
	if (!fout.is_open())
	{
		cout << "д��ʧ��" << endl;
		return -1;
	}
	vector<string> files;
	// ��ȡ�ļ���������ͼƬ
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
				if (strstr(fileinfo.name, "jpg") != NULL || strstr(fileinfo.name, "pgm") != NULL)
					files.push_back(filesFormat.assign(folderPath).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}