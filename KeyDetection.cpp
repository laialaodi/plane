/*
* �ļ�����KeyDetection.cpp
* ���ߣ������
* https://blog.csdn.net/everlasting_20141622/article/details/52222006
*/

#ifndef PLANE_STD_WINDOWS_H_
#include <Windows.h>
#define PLANE_STD_WINDOWS_H_
#endif // !PLANE_STD_WINDOWS_H_

#ifndef PLANE_HEADERS_KEYDETECTION_H_
#include "KeyDetection.h"
#define PLANE_HEADERS_KEYDETECTION_H_
#endif // !PLANE_HEADERS_KEYDETECTION_H_

class KeyDetection
{
public:
	KeyDetection();
	~KeyDetection();

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
