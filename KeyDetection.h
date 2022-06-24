/*
* 文件名：KeyDetection.h
* 作者：刘作瀚
* https://blog.csdn.net/everlasting_20141622/article/details/52222006
*/

#pragma once

#ifndef PLANE_STD_BITSET_
#include <bitset>
#define PLANE_STD_BITSET_
#endif // !PLANE_STD_BITSET_

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

class KeyDetection
{
public:
	bool check(char c);
};
