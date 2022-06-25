/*
* 文件名：KeyDetection.cpp
* 作者：刘作瀚
*/

#ifndef PLANE_HEADERS_KEYDETECTION_H_
#include "KeyDetection.h"
#define PLANE_HEADERS_KEYDETECTION_H_
#endif // !PLANE_HEADERS_KEYDETECTION_H_

class KeyDetection
{
public:
	/*
	* @brief 检查某个键是否被按下
	* @param {char} c - 要检查的键
	* @return 是否按下
	*/
	bool check(char c)
	{
		if (KEY_DOWN(c))
		{
			size_t _i = 0;
			while (_i < 10000)  // 循环大概在0.5秒左右
			{
				if (!KEY_DOWN(c))
				{
					return true;
				}
				_i++;
			}
			return false;
		}
		else
		{
			return false;
		}
	}

private:

};
