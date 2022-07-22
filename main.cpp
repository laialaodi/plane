/*
* �ļ�����main.cpp
* �ɻ��ͻ���
* ���ߣ������
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
* @brief ��USB-HID�豸
* @param {LPCTSTR} PscSerialNumber - HID�豸�����к�
* @return �ɹ� - 0 ʧ�� - -1
*/
int openUSB(LPCTSTR PscSerialNumber)
{
	hid_device* handle = hid_open(0x154F, 0x4304, PscSerialNumber);// ��ָ��vid��pid�����кŵ��豸
	if (!handle)
	{
		printf("�޷���USB�豸");
		hid_exit();
		return -1;
	}
	// ��hid_read()��������Ϊ��������
	if (hid_set_nonblocking(handle, 1) != 0)// 1���÷�����  0���÷�������
	{
		hid_close(handle);
		hid_exit();
		printf("���÷�����ʧ��,����ԭ��:%ls", hid_error(handle));
		return -1;
	}
	return 0;
}

/*
* @brief �ر�USB-HID�豸
* @param {hid_device*&} handle - �������
*/
bool closeUSB(hid_device*& handle)
{
	hid_close(handle);
	handle = nullptr;//���ｫָ���ÿգ���ǿ��ȫ��
	hid_exit();
}

/*
* @brief ��USB-HID�豸���ͱ���
* @param ����1��  {hid_device*} Usb - USB-HID�豸�������
* @param ����2��  {char*} Send      - Ҫ���͵����ݣ���һλ��Ҫ�����ݣ�Ҫ����������
* @return �ɹ� - true ʧ�� - false
*/
bool sendMsg(hid_device* Usb, unsigned char* Send)
{
	Send[0] = 0x01;//�����ţ���λ����HIDд����ʱ��ÿ��������ĵ�һ��byteΪд����report ID���ϡ���λ������һ�¡�
	//USBͨ�ű��泤�����Ϊ32������һ�ֽڿ�ͷ�����ţ����Ϊ33��
	int nActualLength = hid_send_feature_report(Usb, Send, 33);
	if (nActualLength != 33)
	{
		printf("����ָ��ʧ��, �������: %hx", hid_error(Usb));
		return false;
	}
	return true;
}

/*
* @brief ��������USB-HID�豸Ӧ��ı���
* @param ����1��  {hid_device*} Usb      - USB-HID�豸�������
* @param ����2��  {size_t} nLength       - Ӧ�𻺳�������
* @param �������1��  byte* pReceiveBuf  - Ӧ�����ݻ�����
* @return {int} ʵ�ʽ��յ����ݵĳ���
*/
int receiveMsg(hid_device* Usb, byte* pReceiveBuf, size_t nLength)
{
	// ���ձ�����뽫���ջ��������ֽڸ�Ϊ���豸һ�²���ͨ�ųɹ�
	pReceiveBuf[0] = 0x01;
	int nRecvLen = hid_get_feature_report(Usb, pReceiveBuf, 64);  // ����������ý��ճ��ȣ����Ҳ�����û�����ݣ�ִ�гɹ��˷��صľ����������õ�ֵ����һ�ֽڱ���ID��Ҳ����65��ʵ���ϻ���������ȫ��0x00
	if (nRecvLen != 0)
	{
		// HIDͨ�Ż��Զ����룬���û�����ݾ�ȫ������0x00
		if (pReceiveBuf[0] == 0x00)
			nRecvLen = 0;// ���ڵ�һ���ֽ�����Ϊ��0x01�����Ե�һ���ֽ���0x00����Ϊû���յ����ݡ�
		else if (pReceiveBuf[1] == 0x1b)// �ж�ʵ�ʽ��յ������ݳ��ȣ�������ձ��泤�ȹ̶�����65
		{
			// ���ڹ�˾��Э����֡ͷ��0x1b,֡ͷ������λ����֡���ȣ���˿���ͨ������֡���Ȼ��ʵ�ʽ��յ����ݳ���
			nRecvLen = (pReceiveBuf[2] << 8) | pReceiveBuf[3];
		}
	}
	return nRecvLen;
}

int main()
{
	if (hid_init())  // ��ʼ��������ʵ���ϲ�������hid_enumerate�������hid_openҲ���Զ�����
		return -1;
	int eDown = 0, wDown = 0, dDown = 0, sDown = 0;  // �ĸ������µ�ticks
	KeyDetection keyLink = KeyDetection();
	hid_device_info* Hids, * HidsCopy;  // һ�����ڽ����豸��Ϣ�ĵ�������һ�������������ýṹ��ʹ��unicode���룬�������涼Ҫ��unicode����ʽ
	Hids = hid_enumerate(0x04F2, 0xB5C0);  // ��ȡvidΪ0x154F��pidΪ0x4304��HID�豸���������������0���ǻ�ȡ���е�HID�豸
	HidsCopy = Hids;
	LPCWSTR wpSerialNumber = L"\0";  // ����ȥ�أ�һ��hid�豸һ���ж���˵㣬��˻�������
	while (HidsCopy)
	{
		if (CompareStringW(LOCALE_INVARIANT, NORM_LINGUISTIC_CASING, wpSerialNumber, -1, HidsCopy->serial_number, -1) != CSTR_EQUAL)
		{
			std::cout << HidsCopy->serial_number;
			wpSerialNumber = HidsCopy->serial_number;  // ������ֻ��Ҫ���кţ�ʵ��������ṹ�廹�кܶ����ݣ����Բο��ٷ���ʵ��
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