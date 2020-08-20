// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#define DIRECTINPUT_VERSION 0x0800 // Target DirectInput version

// Windows Header Files:
#include <Unknwn.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Some hack trick to export functions with unmangled names
#define C_EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

// Our GUIDs
#define XDINPUT_GUID_DATA1 0x33484358L  // '3HCX'
#define XDINPUT_GUID_DATA2 0x304C       // '0L'
#define XDINPUT_GUID_DATA3 0x4000       // GUID Class 4 + controller ID
#define XDINPUT_GUID_DATA4 {'X', 'D', 'I', 'N', 'P', 'U', 'T', 0}

static GUID XDINPUT_CONTROLLER_GUID = {
  XDINPUT_GUID_DATA1,
  XDINPUT_GUID_DATA2,
  XDINPUT_GUID_DATA3,
  XDINPUT_GUID_DATA4
};

static GUID XDINPUT_VIBRATOR_GUID = {
  XDINPUT_GUID_DATA1,
  XDINPUT_GUID_DATA2,
  XDINPUT_GUID_DATA3 | 0x0100,
  XDINPUT_GUID_DATA4
};

static GUID XDINPUT_PRODUCT_GUID = {
  XDINPUT_GUID_DATA1,
  XDINPUT_GUID_DATA2,
  XDINPUT_GUID_DATA3 | 0x0F00,
  XDINPUT_GUID_DATA4
};

// Utility macros
#define XDINPUT_IS_CONTROLLER_GUID(x) (x).Data1 == XDINPUT_GUID_DATA1 && (x).Data2 == XDINPUT_GUID_DATA2 && ((x).Data3 & 0xFFF0) == XDINPUT_GUID_DATA3
#define XDINPUT_GETCONTROLLER(x) ((x).Data3 & 15)
#define XDINPUT_CONTROLLER_GUID(x, y) (x) = XDINPUT_CONTROLLER_GUID; (x).Data3 += (y)

// These are modified by XDinputEffect and read by DeviceStatus
// to display vibration. One for each controller.
extern LONG leftMotor[4];
extern LONG rightMotor[4];

// Linear Interpolate
static FLOAT lerp(int v0, int v1, FLOAT t) {
  return (1 - t) * v0 + t * v1;
}

// Pointer magic!
static int getPointedInt(int* ptr) {
  int x = *(int*) *ptr;
  *ptr += 4;
  return x;
}