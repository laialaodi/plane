/*
* �ļ�����KeyDetection.cpp
* ���ߣ������
* https://blog.csdn.net/everlasting_20141622/article/details/52222006
*/
#include "KeyDetection.h"

class KeyDetection
{
public:
	KeyDetection();
	~KeyDetection();

	void color(int a)  // �ı��������ɫ����system("color x")��ö�
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
	}

	bool check(char c)  // ���ĳ�������Ƿ��£����¾͸ı�һ�±���
	{
		if (!KEY_DOWN(c)) return true;
		else return false;
	}

private:
};

KeyDetection::KeyDetection()
{
}

KeyDetection::~KeyDetection()
{
}
