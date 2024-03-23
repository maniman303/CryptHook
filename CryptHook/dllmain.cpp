#include "includes.h"
#include <string>
#include <fstream>

bool logging = false;

void init_log(bool enable)
{
	logging = enable;

	std::ofstream logFile("crypthooks.log", std::ios::out);

	logFile << "CryptHook Bypass" << std::endl;

	logFile.close();
}

void log(std::string message)
{
	if (!logging)
	{
		return;
	}

	std::cout << message << std::endl;

	std::ofstream logFile("crypthooks.log", std::ios::out | std::ios::app);

	logFile << message << std::endl;

	logFile.close();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	static HINSTANCE originalCryptBaseHandle;
	static HMODULE originalBCryptHandle;

	char* fullPath = nullptr;
	std::string message = "";
	FARPROC originalBCryptVerifySignature = nullptr;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

#if _DEBUG
		AllocConsole();
		freopen("CON", "w", stdout);
#endif
		init_log(true);

		if (MH_Initialize() != MH_OK)
		{
			log("MiniHook could not be initialized!");
			return -1;
		}

		fullPath = (char*)malloc(MAX_PATH);

		log("Trying to load original BCrypt.dll");

		if (!ExpandEnvironmentStringsA("%WinDir%\\System32\\bcrypt.dll", fullPath, MAX_PATH))
		{
			log("Could not expand path (BCrypt)...");
			return -1;
		}

		message = "Original BCrypt.dll located at: ";
		message += fullPath;
		log(message);

		originalBCryptHandle = LoadLibraryA(fullPath);
		free(fullPath);

		if (!originalBCryptHandle)
		{
			log("Could not load original BCrypt.dll!");
			return -1;
		}

		log("Successfully loaded original BCrypt.dll");

		originalBCryptVerifySignature = GetProcAddress(originalBCryptHandle, "BCryptVerifySignature");

		SYSTEM_INFO _info;
		GetSystemInfo(&_info);

		message = "Page size: ";
		message += std::to_string(_info.dwPageSize);
		log(message);

		DWORD old;

		if (VirtualProtect(originalBCryptVerifySignature, _info.dwPageSize, PAGE_EXECUTE_READWRITE, &old))
		{
			log("Memory unprotected...");

			if (!MH_CreateHook(originalBCryptVerifySignature, PatchedBCryptVerifySignature, (LPVOID*)originalBCrypt) == MH_OK)
			{
				log("Could not create hook for BCryptVerifySignature!");
				return -1;
			}

			message = "Hooked BCryptVerifySignature to: ";
			message += std::to_string((int)&PatchedBCryptVerifySignature);
			message += " original: ";
			message += std::to_string((int)&originalBCrypt);

			log(message);

			if (MH_EnableHook(originalBCryptVerifySignature) != MH_OK)
			{
				log("Could not enable BCryptVerifySignature hook!");
				return -1;
			}

			log("Enabled hooks...");
			return TRUE;
		}

		return TRUE;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		FreeLibrary(originalCryptBaseHandle);
		FreeLibrary(originalBCryptHandle);
		return TRUE;
	}

	return TRUE;
}