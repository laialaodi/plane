/*
* 文件名：KeyDetection.cpp
* 作者：刘作瀚
* https://blog.csdn.net/everlasting_20141622/article/details/52222006
*/
#include "KeyDetection.h"

class KeyDetection
{
public:
	KeyDetection();
	~KeyDetection();

	void color(int a)  // 改变输出的颜色，比system("color x")快得多
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a);
	}

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
