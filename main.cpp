/*
* �ļ�
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

hid_device* handle;

/*********************************************************
�������ƣ� OpenUSB(LPCTSTR PscSerialNumber)
�������ܣ� ��USB-HID�豸
���������
	����1��  LPCTSTR PscSerialNumber  - HID�豸�����к�
����ֵ��
	�ɹ� - 0
	ʧ�� - -1
********************************************************/
int OpenUSB(LPCTSTR PscSerialNumber)
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

//�ر�USB-HID�豸
/*********************************************************
�������ƣ� CloseUSB(hid_device*& handle)
�������ܣ� �ر�USB-HID�豸
���������
	����1��  hid_device*& handle  - �������
********************************************************/
bool CloseUSB(hid_device*& handle)
{
	hid_close(handle);
	handle = nullptr;//���ｫָ���ÿգ���ǿ��ȫ��
	hid_exit();
}

/*********************************************************
�������ƣ� SendMsg(hid_device* Usb, char* Send)
�������ܣ� ��USB-HID�豸���ͱ���
���������
	����1��  hid_device* Usb	 - USB-HID�豸�������
	����2��  char* Send	 - Ҫ���͵����ݣ���һλ��Ҫ�����ݣ�Ҫ����������
����ֵ��
	�ɹ� - true
	ʧ�� - false
********************************************************/
bool SendMsg(hid_device* Usb, unsigned char* Send)
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

/*********************************************************
�������ƣ� ReceiveMsg(hid_device* Usb, byte* pReceiveBuf, size_t nLength)
�������ܣ� ��������USB-HID�豸Ӧ��ı���
���������
	����1��  hid_device* Usb    - USB-HID�豸�������
	����2��  size_t nLength     - Ӧ�𻺳�������
���������
	����1��  byte* pReceiveBuf  - Ӧ�����ݻ�����
����ֵ��
	ʵ�ʽ��յ����ݵĳ���
********************************************************/
int ReceiveMsg(hid_device* Usb, byte* pReceiveBuf, size_t nLength)
{
	//���ձ�����뽫���ջ��������ֽڸ�Ϊ���豸һ�²���ͨ�ųɹ�
	pReceiveBuf[0] = 0x01;
	int nRecvLen = hid_get_feature_report(Usb, pReceiveBuf, 64);//����������ý��ճ��ȣ����Ҳ�����û�����ݣ�ִ�гɹ��˷��صľ����������õ�ֵ����һ�ֽڱ���ID��Ҳ����65��ʵ���ϻ���������ȫ��0x00
	if (nRecvLen != 0)
	{
		//HIDͨ�Ż��Զ����룬���û�����ݾ�ȫ������0x00
		if (pReceiveBuf[0] == 0x00)
			nRecvLen = 0;//���ڵ�һ���ֽ�����Ϊ��0x01�����Ե�һ���ֽ���0x00����Ϊû���յ����ݡ�
		else if (pReceiveBuf[1] == 0x1b)//�ж�ʵ�ʽ��յ������ݳ��ȣ�������ձ��泤�ȹ̶�����65
		{
			//���ڹ�˾��Э����֡ͷ��0x1b,֡ͷ������λ����֡���ȣ���˿���ͨ������֡���Ȼ��ʵ�ʽ��յ����ݳ���
			nRecvLen = (pReceiveBuf[2] << 8) | pReceiveBuf[3];
		}
	}
	return nRecvLen;
}

int main()
{
	if (hid_init())// ��ʼ��������ʵ���ϲ�������hid_enumerate�������hid_openҲ���Զ�����
		return -1;
	KeyDetection key_link = KeyDetection();
	hid_device_info* Hids, * HidsCopy;// һ�����ڽ����豸��Ϣ�ĵ�������һ�������������ýṹ��ʹ��unicode���룬�������涼Ҫ��unicode����ʽ
	Hids = hid_enumerate(0x04F2, 0xB5C0);// ��ȡvidΪ0x154F��pidΪ0x4304��HID�豸���������������0���ǻ�ȡ���е�HID�豸
	HidsCopy = Hids;
	LPCWSTR wpSerialNumber = L"\0";// ����ȥ�أ�һ��hid�豸һ���ж���˵㣬��˻�������
	while (HidsCopy)
	{
		if (CompareStringW(LOCALE_INVARIANT, NORM_LINGUISTIC_CASING, wpSerialNumber, -1, HidsCopy->serial_number, -1) != CSTR_EQUAL)
		{
			std::cout << HidsCopy->serial_number;
			wpSerialNumber = HidsCopy->serial_number;// ������ֻ��Ҫ���кţ�ʵ��������ṹ�廹�кܶ����ݣ����Բο��ٷ���ʵ��
		}
		OpenUSB(wpSerialNumber);
		while (true)
		{
			std::system("cls");
			if (GetAsyncKeyState(VK_SPACE))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00001"));
				std::cout << "VK_SPACE ";
			}
			if (key_link.check('1'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00010"));
				std::cout << "1 ";
			}
			if (key_link.check('2'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00011"));
				std::cout << "2 ";
			}
			if (key_link.check('3'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00100"));
				std::cout << "3 ";
			}
			if (key_link.check('4'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00101"));
				std::cout << "4 ";
			}
			if (key_link.check('5'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00110"));
				std::cout << "5 ";
			}
			if (key_link.check('6'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("00111"));
				std::cout << "6 ";
			}
			if (key_link.check('7'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01000"));
				std::cout << "7 ";
			}
			if (key_link.check('8'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01001"));
				std::cout << "8 ";
			}
			if (key_link.check('9'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01010"));
				std::cout << "9 ";
			}
			if (key_link.check('0'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01011"));
				std::cout << "0 ";
			}
			if (key_link.check('T'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01100"));
				std::cout << "T ";
			}
			if (!key_link.check('T'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01101"));
				std::cout << "t ";
			}
			if (key_link.check('Y'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01110"));
				std::cout << "Y ";
			}
			if (!key_link.check('Y'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("01111"));
				std::cout << "y ";
			}
			if (key_link.check('G'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10000"));
				std::cout << "G ";
			}
			if (!key_link.check('G'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10001"));
				std::cout << "g ";
			}
			if (key_link.check('H'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10010"));
				std::cout << "H ";
			}
			if (!key_link.check('H'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10011"));
				std::cout << "h ";
			}
			if (key_link.check('V'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10100"));
				std::cout << "V ";
			}
			if (!key_link.check('V'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10101"));
				std::cout << "v ";
			}
			if (key_link.check('N'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10110"));
				std::cout << "N ";
			}
			if (!key_link.check('N'))
			{
				SendMsg(handle, reinterpret_cast<unsigned char*> ("10111"));
				std::cout << "n ";
			}
		}
		HidsCopy = HidsCopy->next;
	}
	hid_free_enumeration(Hids);
}