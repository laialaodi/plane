/*
* �ļ�����KeyDetection.cpp
* ���ߣ������
*/

#ifndef PLANE_HEADERS_KEYDETECTION_H_
#include "KeyDetection.h"
#define PLANE_HEADERS_KEYDETECTION_H_
#endif // !PLANE_HEADERS_KEYDETECTION_H_

class KeyDetection
{
public:
	/*
	* @brief ���ĳ�����Ƿ񱻰���
	* @param {char} c - Ҫ���ļ�
	* @return �Ƿ���
	*/
	bool check(char c)
	{
		if (KEY_DOWN(c))
		{
			size_t _i = 0;
			while (_i < 10000)  // ѭ�������0.5������
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
