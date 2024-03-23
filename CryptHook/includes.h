#pragma once

#include <windows.h>
#include <iostream>
#include "../vendor/MinHook/include/MinHook.h"

#define EXPORT_C extern "C" __declspec(dllexport)

// Original BCryptVerifySignature
int (*originalBCrypt)(void);

__int64 PatchedBCryptVerifySignature()
{
	std::cout << "Called BCryptVerifySignature..." << std::endl;
	return 0;
}