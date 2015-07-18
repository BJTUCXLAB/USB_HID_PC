/**
  ******************************************************************************
  * @file    STM32_HID.c
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   STM32的HID实验上位机测试程序.
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
#define     USB_VID			0x0483	//这个值在单片机程序的设备描述符中定义 
#define     USB_PID			0x5750	//这个值在单片机程序的设备描述符中定义   
#define		REPORT_COUNT	100   	//端点长度//07.18-YQL：不能小于64，我先改STM32了，应该是32描述里的问题
/* Private function prototypes -----------------------------------------------*/
HANDLE OpenMyHIDDevice(int overlapped);
void HIDSampleFunc() ;
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  主函数
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
  * @brief  发送数据后读取数据 
  * @param  None
  * @retval None
  */
void HIDSampleFunc(void)   
{       
	HANDLE       hDev;       
	BYTE         recvDataBuf[1024],reportBuf[1024];;                   
	DWORD        bytes;       
	hDev = OpenMyHIDDevice(0); // 打开设备，不使用重叠（异步）方式 ;      
	if (hDev == INVALID_HANDLE_VALUE){           
		printf("INVALID_HANDLE_VALUE\n");
		return;  
	}
	reportBuf[0] = 0; // 输出报告的报告 ID 是 0      
	for(int i=0;i<REPORT_COUNT;i++){
		reportBuf[i+1]=i+1;//将数据存放在数据缓冲区
	}
	printf("开始写数据到设备...\n");
	// 写入数据到设备，注意，第三个参数值必须为REPORT_COUNT+1，否则会返回1784错误
	if (!WriteFile(hDev, reportBuf, REPORT_COUNT+1, &bytes, NULL)){           
		printf("write data error! %d\n",GetLastError());
		printf("按任意键退出！");getchar();
		//return;    
	}else{
		printf("成功向设备写出%d个数据... \n", REPORT_COUNT);
	}
	printf("开始从设备读取数据...\n");
	// 从设备读取数据，注意，第三个参数值必须大于等于REPORT_COUNT+1，否则会返回1784错误
	if(!ReadFile(hDev, recvDataBuf, REPORT_COUNT+1, &bytes, NULL)){ // 读取设备发给主机的数据  
		printf("read data error! %d\n",GetLastError());
		return;    
	}else{
		printf("成功向设备读出%d个数据... \n", REPORT_COUNT);
	}
	printf("设备返回的数据为：\n");
	//显示读取回来的数据
	for(int i=0;i<REPORT_COUNT;i++){
		printf("0x%02X ",recvDataBuf[i+1]);
	}
	printf("\n\r按任意键结束");
	getchar();
}  
/**
  * @brief  打开HID设备
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

				printf("找到了我想要的设备，哈哈哈....\n");
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

