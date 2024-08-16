extern "C" {
#include "ntddk.h"
}
#include <kbdmou.h>
#include "ntddkbd.h"
#include "Kbd.h"

NTSTATUS HookKeyboard(IN PDRIVER_OBJECT pDriverObject) {
	DbgPrint("Entering Hook Routine..\n");

	PDEVICE_OBJECT pKeyboardDeviceObject;
	NTSTATUS status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, true, &pKeyboardDeviceObject);

	if (!NT_SUCCESS(status)) return status;
	DbgPrint("Created keyboard device sucessfully.\n");

	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags | (DO_BUFFERED_IO | DO_POWER_PAGABLE);
	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags & ~DO_DEVICE_INITIALIZING;
	DbgPrint("Flags are set successfully.\n");
}
