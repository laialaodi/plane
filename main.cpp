/*
* 文件名：main.cpp
* 飞机客户端
* 作者：刘作瀚
*/

#ifndef PLANE_STD_IOSTREAM_
#include <iostream>
#define PLANE_STD_IOSTREAM_
#endif // !PLANE_STD_IOSTREAM_

#ifndef PLANE_STD_CSTDLIB_
#include <cstdlib>
#define PLANE_STD_CSTDLIB_
#endif // !PLANE_STD_CSTDLIB_

#ifndef PLANE_STD_WINDOWS_H_
#include <Windows.h>
#define PLANE_STD_WINDOWS_H_
#endif // !PLANE_STD_WINDOWS_H_

#ifndef PLANE_HEADERS_HIDAPI_H_
#include "hidapi.h"
#define PLANE_HEADERS_HIDAPI_H_
#endif // !PLANE_HEADERS_HIDAPI_H_

#ifndef PLANE_HEADERS_KEYDETECTION_H_
#include "KeyDetection.h"
#define PLANE_HEADERS_KEYDETECTION_H_
#endif // !PLANE_HEADERS_KEYDETECTION_H_

#pragma comment (lib, "setupapi.lib")

hid_device* handle = nullptr;

/*
* @brief 打开USB-HID设备
* @param {LPCTSTR} PscSerialNumber - HID设备的序列号
* @return 成功 - 0 失败 - -1
*/
int openUSB(LPCTSTR PscSerialNumber)
{
	hid_device* handle = hid_open(0x154F, 0x4304, PscSerialNumber);// 打开指定vid、pid、序列号的设备
	if (!handle)
	{
		printf("无法打开USB设备");
		hid_exit();
		return -1;
	}
	// 将hid_read()函数设置为非阻塞。
	if (hid_set_nonblocking(handle, 1) != 0)// 1启用非阻塞  0禁用非阻塞。
	{
		hid_close(handle);
		hid_exit();
		printf("设置非阻塞失败,错误原因:%ls", hid_error(handle));
		return -1;
	}
	return 0;
}

/*
* @brief 关闭USB-HID设备
* @param {hid_device*&} handle - 操作句柄
*/
bool closeUSB(hid_device*& handle)
{
	hid_close(handle);
	handle = nullptr;//这里将指针置空，增强安全性
	hid_exit();
}

/*
* @brief 向USB-HID设备发送报告
* @param 参数1：  {hid_device*} Usb - USB-HID设备操作句柄
* @param 参数2：  {char*} Send      - 要发送的数据，第一位不要放数据，要留给报告编号
* @return 成功 - true 失败 - false
*/
bool sendMsg(hid_device* Usb, unsigned char* Send)
{
	Send[0] = 0x01;//报告编号，上位机向HID写数据时，每个包传输的第一个byte为写数据report ID，上、下位机必须一致。
	//USB通信报告长度最短为32，加上一字节开头报告编号，最短为33。
	int nActualLength = hid_send_feature_report(Usb, Send, 33);
	if (nActualLength != 33)
	{
		printf("发送指令失败, 错误代码: %hx", hid_error(Usb));
		return false;
	}
	return true;
}

/*
* @brief 接收来自USB-HID设备应答的报告
* @param 参数1：  {hid_device*} Usb      - USB-HID设备操作句柄
* @param 参数2：  {size_t} nLength       - 应答缓冲区长度
* @param 输出参数1：  byte* pReceiveBuf  - 应答数据缓冲区
* @return {int} 实际接收到数据的长度
*/
int receiveMsg(hid_device* Usb, byte* pReceiveBuf, size_t nLength)
{
	// 接收报告必须将接收缓冲区首字节改为与设备一致才能通信成功
	pReceiveBuf[0] = 0x01;
	int nRecvLen = hid_get_feature_report(Usb, pReceiveBuf, 64);  // 这里必须设置接收长度，并且不管有没有数据，执行成功了返回的就是这里设置的值加上一字节报告ID，也就是65，实际上缓冲区里面全是0x00
	if (nRecvLen != 0)
	{
		// HID通信会自动补齐，如果没有数据就全部都是0x00
		if (pReceiveBuf[0] == 0x00)
			nRecvLen = 0;// 由于第一个字节设置为了0x01，所以第一个字节是0x00就认为没有收到数据。
		else if (pReceiveBuf[1] == 0x1b)// 判断实际接收到的数据长度，否则接收报告长度固定返回65
		{
			// 由于公司的协议里帧头是0x1b,帧头后面两位就是帧长度，因此可以通过解析帧长度获得实际接收的数据长度
			nRecvLen = (pReceiveBuf[2] << 8) | pReceiveBuf[3];
		}
	}
	return nRecvLen;
}

int main()
{
	if (hid_init())  // 初始化函数，实际上不调用它hid_enumerate和下面的hid_open也会自动调用
		return -1;
	int eDown = 0, wDown = 0, dDown = 0, sDown = 0;  // 四个键按下的ticks
	KeyDetection keyLink = KeyDetection();
	hid_device_info* Hids, * HidsCopy;  // 一个用于接收设备信息的单链表，另一个用来遍历，该结构体使用unicode编码，所以下面都要用unicode处理方式
	Hids = hid_enumerate(0x04F2, 0xB5C0);  // 获取vid为0x154F，pid为0x4304的HID设备链表，这里如果都是0就是获取所有的HID设备
	HidsCopy = Hids;
	LPCWSTR wpSerialNumber = L"\0";  // 用来去重，一个hid设备一般有多个端点，因此会读到多个
	while (HidsCopy)
	{
		if (CompareStringW(LOCALE_INVARIANT, NORM_LINGUISTIC_CASING, wpSerialNumber, -1, HidsCopy->serial_number, -1) != CSTR_EQUAL)
		{
			std::cout << HidsCopy->serial_number;
			wpSerialNumber = HidsCopy->serial_number;  // 我这里只需要序列号，实际上这个结构体还有很多数据，可以参考官方的实例
		}
		openUSB(wpSerialNumber);
		while (true)
		{
			std::system("cls");
			if (KEY_DOWN('E') && eDown <= 10000)
			{
				sendMsg(handle, reinterpret_cast<unsigned char*> ("00010"));
				std::cout << "E ";
				++eDown;
			}
			else
			{
				if (eDown <= 10000)
				{

				}
			}
			if (KEY_DOWN('W'))
			{
				sendMsg(handle, reinterpret_cast<unsigned char*> ("00011"));
				std::cout << "W ";
			}
			if (KEY_DOWN('D'))
			{
				sendMsg(handle, reinterpret_cast<unsigned char*> ("00100"));
				std::cout << "D ";
			}
			if (KEY_DOWN('S'))
			{
				sendMsg(handle, reinterpret_cast<unsigned char*> ("00101"));
				std::cout << "S ";
			}
		}
		HidsCopy = HidsCopy->next;
	}
	hid_free_enumeration(Hids);
}