/*

  File:     usb.h
  Author:   Alan Macek <www.alanmacek.com>
  Date:     March 1, 2001

  This file contains the declarations for connecting to USB HID devices.

  You are free to use this code for anything you want but please send me
  (al@alanmacek.com) an email telling me what you are using it for and
  how it works out.  You are NOT ALLOWED to use this code until you send
  me an email.
    
  This code comes with absolutely no warranty at all.

  -----------------------------------------------------------------------------

  Nov 16, 2004, Golden Crater Software, Jim Koornneef
  Write C++ class around code. Create ExploreTODebugWindow, Read and Writing routines. Added Overlapped read support.
  
	Code written by Jim Koornneef is copyright, simply so I don't get sued by someone else copyrighting it on me. I assume
	code written by Alan Macek is copyright by him, and claim no ownership over his work.
	You are free to use this code for anything, and it comes with no warranty. You must still contact Alan Macek (as per above)
	
*/

#pragma once

#include <windows.h>

/* Both of these functions return INVALID_HANDLE_VALUE on an error, if there is no error
    then the HANDLE must be closed using 'CloseHandle' */

class CUSBDevice
{
public:
	CUSBDevice();
	/* Connects to the USB HID described by the combination of vendor id, product id
	   If the attribute is null, it will connect to first device satisfying the remaining
	   attributes. */
	CUSBDevice(const DWORD *vendorID, const DWORD *productID, const DWORD *versionNumber);
	~CUSBDevice();
	bool Connect(const DWORD *vendorID, const DWORD *productID, const DWORD *versionNumber);
		
	bool Write(const BYTE *bytes, int nBuffLen);
	int Read(BYTE *bytes, int nBuffLen);

#ifdef _DEBUG
	void ExploreToDebugWindow();	// just enumerates the devices and displays vid, pid, ver in the debugger window.
#endif
	
protected:
	void Disconnect();
	HANDLE ConnectToIthDevice (DWORD i);	/* Connects to the ith USB HID device connected to the computer */
private:
	HANDLE handle;
	HANDLE EventObject;
	OVERLAPPED HIDOverlapped;

	
};