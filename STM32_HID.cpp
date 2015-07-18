/**
  ******************************************************************************
  * @file    STM32_HID.c
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   STM32��HIDʵ����λ�����Գ���.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stdafx.h"
extern "C" {  
#include "setupapi.h"
#include "hiddll.h"
}
/* Private define ------------------------------------------------------------*/
#define     USB_VID			0x0483	//���ֵ�ڵ�Ƭ��������豸�������ж��� 
#define     USB_PID			0x5750	//���ֵ�ڵ�Ƭ��������豸�������ж���   
#define		REPORT_COUNT	100   	//�˵㳤��//07.18-YQL������С��64�����ȸ�STM32�ˣ�Ӧ����32�����������
/* Private function prototypes -----------------------------------------------*/
HANDLE OpenMyHIDDevice(int overlapped);
void HIDSampleFunc() ;
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ������
  * @param  argc
  * @param  argv
  * @retval None
  */
int _tmain(int argc, _TCHAR* argv[])
{
	HIDSampleFunc();
	return 0;
}


/**
  * @brief  �������ݺ��ȡ���� 
  * @param  None
  * @retval None
  */
void HIDSampleFunc(void)   
{       
	HANDLE       hDev;       
	BYTE         recvDataBuf[1024],reportBuf[1024];;                   
	DWORD        bytes;       
	hDev = OpenMyHIDDevice(0); // ���豸����ʹ���ص����첽����ʽ ;      
	if (hDev == INVALID_HANDLE_VALUE){           
		printf("INVALID_HANDLE_VALUE\n");
		return;  
	}
	reportBuf[0] = 0; // �������ı��� ID �� 0      
	for(int i=0;i<REPORT_COUNT;i++){
		reportBuf[i+1]=i+1;//�����ݴ�������ݻ�����
	}
	printf("��ʼд���ݵ��豸...\n");
	// д�����ݵ��豸��ע�⣬����������ֵ����ΪREPORT_COUNT+1������᷵��1784����
	if (!WriteFile(hDev, reportBuf, REPORT_COUNT+1, &bytes, NULL)){           
		printf("write data error! %d\n",GetLastError());
		printf("��������˳���");getchar();
		//return;    
	}else{
		printf("�ɹ����豸д��%d������... \n", REPORT_COUNT);
	}
	printf("��ʼ���豸��ȡ����...\n");
	// ���豸��ȡ���ݣ�ע�⣬����������ֵ������ڵ���REPORT_COUNT+1������᷵��1784����
	if(!ReadFile(hDev, recvDataBuf, REPORT_COUNT+1, &bytes, NULL)){ // ��ȡ�豸��������������  
		printf("read data error! %d\n",GetLastError());
		return;    
	}else{
		printf("�ɹ����豸����%d������... \n", REPORT_COUNT);
	}
	printf("�豸���ص�����Ϊ��\n");
	//��ʾ��ȡ����������
	for(int i=0;i<REPORT_COUNT;i++){
		printf("0x%02X ",recvDataBuf[i+1]);
	}
	printf("\n\r�����������");
	getchar();
}  
/**
  * @brief  ��HID�豸
  * @param  None
  * @retval None
  */
HANDLE OpenMyHIDDevice(int overlapped)   
{       
	HANDLE hidHandle;       
	GUID hidGuid;       
	HidD_GetHidGuid(&hidGuid);       
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&hidGuid,NULL,NULL,(DIGCF_PRESENT | DIGCF_DEVICEINTERFACE)); 
	if (hDevInfo == INVALID_HANDLE_VALUE)       
	{           
		return INVALID_HANDLE_VALUE;       
	}       
	SP_DEVICE_INTERFACE_DATA devInfoData;       
	devInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);       
	int deviceNo = 0;       
	SetLastError(NO_ERROR);       
	while (GetLastError() != ERROR_NO_MORE_ITEMS)       
	{           
		if (SetupDiEnumInterfaceDevice (hDevInfo,0,&hidGuid,deviceNo,&devInfoData))
		{               
			ULONG  requiredLength = 0;               
			SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData,
				NULL,
				0,
				&requiredLength,
				NULL); 
			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail = (SP_INTERFACE_DEVICE_DETAIL_DATA*)malloc(requiredLength);
			devDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			if(!SetupDiGetInterfaceDeviceDetail(hDevInfo,
				&devInfoData,
				devDetail,
				requiredLength,
				NULL,
				NULL))
			{                   
				free(devDetail);                   
				SetupDiDestroyDeviceInfoList(hDevInfo);                   
				return INVALID_HANDLE_VALUE;               
			}               
			if (overlapped)               
			{                   
				hidHandle = CreateFile(devDetail->DevicePath,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,
					NULL);
			}               
			else             
			{                   
				hidHandle = CreateFile(devDetail->DevicePath,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					0,
					NULL); 
				if(hidHandle==INVALID_HANDLE_VALUE)
					hidHandle = CreateFile(devDetail->DevicePath,
					0,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					0,
					NULL); 
			}               
			free(devDetail);               
			if (hidHandle==INVALID_HANDLE_VALUE)
			{                   
				SetupDiDestroyDeviceInfoList(hDevInfo);
				free(devDetail);                   
				return INVALID_HANDLE_VALUE;               
			}               
			_HIDD_ATTRIBUTES hidAttributes;               
			if(!HidD_GetAttributes(hidHandle, &hidAttributes))               
			{                   
				CloseHandle(hidHandle);                   
				SetupDiDestroyDeviceInfoList(hDevInfo);                   
				return INVALID_HANDLE_VALUE;               
			}               
			if (USB_VID == hidAttributes.VendorID                   
				&& USB_PID  == hidAttributes.ProductID)               
			{          

				printf("�ҵ�������Ҫ���豸��������....\n");
				break;               
			}               
			else             
			{                   
				CloseHandle(hidHandle);                   
				++deviceNo;               
			}           
		}       
	}       
	SetupDiDestroyDeviceInfoList(hDevInfo);       
	return hidHandle;   
}  

