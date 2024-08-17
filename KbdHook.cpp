extern "C" {
#include "ntddk.h"
}
#include <kbdmou.h>
#include "ntddkbd.h"
#include "Kbd.h"

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
	RtlFreeUnicodeString(&uKeyboardDeviceName); //Release alloc memory.
	DbgPrint("Filter Device attached successfully.\n");

	return STATUS_SUCCESS;
}
