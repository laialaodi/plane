/*
* 文件名：KeyDetection.cpp
* 作者：刘作瀚
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

	bool check(char c)  // 检测某个按键是否按下，按下就改变一下变量
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
