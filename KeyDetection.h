/*
* �ļ�����KeyDetection.h
* ���ߣ������
*/

#pragma once

#ifndef PLANE_STD_WINDOWS_H_
#include <Windows.h>
#define PLANE_STD_WINDOWS_H_
#endif // !PLANE_STD_WINDOWS_H_

#define KEY_DOWN(VK_NAME) ((GetAsyncKeyState(VK_NAME) & 0x8000) ? 1 : 0)

class KeyDetection
{
public:
	bool check(char c);

private:

};
