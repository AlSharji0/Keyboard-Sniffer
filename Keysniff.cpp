extern "C" {
#include "ntddk.h"
}
#include <kbdmou.h>
#include "KbdHook.h"
#include "ntddkbd.h"

int numPendingIrps = 0;
