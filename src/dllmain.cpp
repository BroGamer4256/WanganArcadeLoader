#include "dllmain.h"
#include "ffb.h"
#include "helpers.h"
#include "poll.h"
#include <dxgi.h>

bool isTerminal   = false;
HWND windowHandle = 0;

typedef void (*callbackTouch) (i32, i32, u8[168], u64);

bool waitingForTouch;
callbackTouch touchCallback;
u64 touchData;
char accessCode[21] = "00000000000000000001";
char chipId[33]     = "00000000000000000000000000000001";
u8 cardData[168]    = {0x01, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x2E, 0x58, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x7F, 0x5C, 0x97, 0x44, 0xF0, 0x88, 0x04, 0x00, 0x43, 0x26, 0x2C, 0x33, 0x00, 0x04, 0x06, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
                       0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
                       0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4E, 0x42, 0x47, 0x49, 0x43, 0x36, 0x00, 0x00, 0xFA, 0xE9, 0x69, 0x00, 0xF6, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#pragma pack(8)
struct string {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;

	char *c_str () {
		if (this->capacity > 15) return this->ptr;
		else return this->data;
	}
};

struct wstring {
	union {
		wchar_t data[8];
		wchar_t *ptr;
	};
	u64 length;
	u64 capacity;

	wchar_t *c_str () {
		if (this->capacity > 7) return this->ptr;
		else return this->data;
	}
};

extern "C" {
u64
BngRwDevReset () {
	return 1;
}
u64
BngRwReadMifare () {
	return 0xFFFFFF9C;
}
void
BngRwFin () {
	return;
}
u64
BngRwGetFwVersion () {
	return 0;
}
u64
BngRwGetStationID () {
	return 0;
}
u64
BngRwGetRetryCount () {
	return 0;
}
i32
BngRwIsCmdExec () {
	return -1;
}
u64
BngRwReqAction () {
	return 1;
}
u64
BngRwReqAiccAuth () {
	return 1;
}
u64
BngRwReqBeep () {
	return 1;
}
u64
BngRwReqFwCleanup () {
	return 1;
}
u64
BngRwReqFwVersionUp () {
	return 1;
}
i32
BngRwReqLatchID () {
	return 1;
}
u64
BngRwReqLed () {
	return 1;
}
i32
BngRwReqSendMail () {
	return 1;
}
i32
BngRwReqSendUrl () {
	return 1;
}
i32
BngRwReqSendUrlTo () {
	return 1;
}
u64
BngRwReqSetLedPower () {
	return 0;
}
i32
BngRwReqCancel () {
	return 1;
}
const char *
BngRwGetVersion () {
	return "WAL v0.0.1";
}
u64
BngRwAttach (i32 a1, char *a2, i32 a3, i32 a4, void *callback, i32 *a6) {
	return 1;
}
i32
BngRwReqWaitTouch (u32 a1, i32 a2, u32 a3, callbackTouch callback, u64 a5) {
	waitingForTouch = true;
	touchCallback   = callback;
	touchData       = a5;
	return 1;
}
i32
BngRwInit () {
	return 0;
}
}

HOOK (u64, IoOpen, ASLR (0x140AAACB0), u64 a1, string *filePath, i32 a3, i32 a4) {
	if (strstr (filePath->c_str (), "F:") || strstr (filePath->c_str (), "G:") || strstr (filePath->c_str (), "J:")) {
		strcpy (filePath->c_str (), filePath->c_str () + 1);
		filePath->c_str ()[0] = '.';
		filePath->length      = filePath->length - 1;
	}
	return originalIoOpen (a1, filePath, a3, a4);
}

HOOK (void *, DbCreate, ASLR (0x1409B6580), u64 a1) {
	string *filePath = (string *)(a1 + 0x10);
	if (strstr (filePath->c_str (), "F:") || strstr (filePath->c_str (), "G:") || strstr (filePath->c_str (), "J:")) {
		strcpy (filePath->c_str (), filePath->c_str () + 1);
		filePath->c_str ()[0] = '.';
		filePath->length      = filePath->length - 1;
	}
	return originalDbCreate (a1);
}

HOOK (void *, LoadCrown, ASLR (0x1409B39C0), u64 a1, i32 a2, u64 a3) {
	string *filePath = (string *)(a1 + 0x10);
	if (strstr (filePath->c_str (), "F:") || strstr (filePath->c_str (), "G:") || strstr (filePath->c_str (), "J:")) {
		strcpy (filePath->c_str (), filePath->c_str () + 1);
		filePath->c_str ()[0] = '.';
		filePath->length      = filePath->length - 1;
	}
	return originalLoadCrown (a1, a2, a3);
}

HOOK (void, DebugPrint, ASLR (0x1409BCD80), u64 a1, i32 a2, wstring *str) {
	if (a2 == 1) return;
	wchar_t prefix[32] = L"";
	if (a2 == 1) wcscpy (prefix, L"[Info]");
	else if (a2 == 2) wcscpy (prefix, L"[Warning]");
	else if (a2 == 3) wcscpy (prefix, L"[Check]");
	else if (a2 == 4) wcscpy (prefix, L"[Error]");
	wprintf (L"%ls%ls", prefix, str->c_str ());
	// originalDebugPrint (a1, a2, str);
}

HOOK (HRESULT, CreateDeviceAndSwapChain, PROC_ADDRESS ("D3D11.dll", "D3D11CreateDeviceAndSwapChain"), void *pAdapter, i32 DriverType, HMODULE Software, u32 Flags, i32 *pFeatureLevels,
      u32 FeatureLevels, u32 SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, void **ppSwapChain, void **ppDevice, i32 *pFeatureLevel, void **ppImmediateContext) {
	if (pSwapChainDesc) pSwapChainDesc->Windowed = true;
	HRESULT res =
	    originalCreateDeviceAndSwapChain (pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	return res;
}

u8 *haspBuffer = (u8 *)malloc (0xD40);
HOOK (i32, HaspDecrypt, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_decrypt"), i32 id, void *a2, i32 a3) { return 0; }
HOOK (i32, HaspEncrypt, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_encrypt"), i32 id, void *a2, i32 a3) { return 0; }
HOOK (i32, HaspLogin, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_login"), i32 a1, char *a2, i32 *id) { return 0; }
HOOK (i32, HaspLogout, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_logout"), i32 id) { return 0; }
HOOK (i32, HaspWrite, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_write"), i32 id) { return 0; }
HOOK (i32, HaspGetSize, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_get_size"), i32 id, i32 a2, i32 *a3) {
	*a3 = 0xD40;
	return 0;
}
HOOK (i32, HaspRead, PROC_ADDRESS ("hasp_windows_x64_30382.dll", "hasp_read"), i32 id, i32 a2, i32 offset, i32 length, void *buffer) {
	memcpy (buffer, haspBuffer + offset, length);
	return 0;
}

InputData *inputData = new InputData;

Keybindings TestBinding{.keycodes = {VK_F1}};
Keybindings ServiceBinding{.keycodes = {VK_F2}};
Keybindings StartBinding{.buttons = {SDL_CONTROLLER_BUTTON_START}};
Keybindings TestUpBinding{.keycodes = {VK_UP}};
Keybindings TestDownBinding{.keycodes = {VK_DOWN}};
Keybindings TestEnterBinding{.keycodes = {VK_RETURN}};

Keybindings CoinBinding{.buttons = {SDL_CONTROLLER_BUTTON_BACK}};

Keybindings GearNextBinding{.buttons = {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER}};
Keybindings GearPreviousBinding{.buttons = {SDL_CONTROLLER_BUTTON_LEFTSHOULDER}};
Keybindings GearBindings[7]{};

Keybindings PerspectiveBinding{.buttons = {SDL_CONTROLLER_BUTTON_Y}};
Keybindings GasBinding{.buttons = {SDL_CONTROLLER_BUTTON_A}, .axis = {SDL_AXIS_RTRIGGER_DOWN}};
Keybindings BrakeBinding{.buttons = {SDL_CONTROLLER_BUTTON_B}, .axis = {SDL_AXIS_LTRIGGER_DOWN}};
Keybindings WheelLeftBinding{.axis = {SDL_AXIS_LEFT_LEFT}};
Keybindings WheelRightBinding{.axis = {SDL_AXIS_LEFT_RIGHT}};

Keybindings CardInsertBinding{.keycodes = {'P'}};

HOOK (bool, WAJVOpen, ASLR (0x1400169C0), const char *jvsComPath) { return 1; }
HOOK (InputData *, WAJVGetInput, ASLR (0x140017890), u32 index) {
	if (waitingForTouch) {
		bool hasInserted = false;
		if (IsButtonTapped (CardInsertBinding)) {
			if (!hasInserted) {
				memcpy (cardData + 0x2C, chipId, 33);
				memcpy (cardData + 0x50, accessCode, 21);
				touchCallback (0, 0, cardData, touchData);
			}
		}
	}

	UpdatePoll (windowHandle);
	inputData->Test      = IsButtonTapped (TestBinding) ? !inputData->Test : inputData->Test;
	inputData->Service   = IsButtonDown (ServiceBinding);
	inputData->Start     = IsButtonDown (StartBinding);
	inputData->TestUp    = IsButtonTapped (TestUpBinding);
	inputData->TestDown  = IsButtonTapped (TestDownBinding);
	inputData->TestEnter = IsButtonTapped (TestEnterBinding);

	inputData->Coin = IsButtonTapped (CoinBinding);

	inputData->SetGearIndex (inputData->GetGearIndex () + (IsButtonTapped (GearNextBinding) ? 1 : IsButtonTapped (GearPreviousBinding) ? -1 : 0));
	for (auto i = 0; i < COUNTOFARR (GearBindings); i++)
		if (IsButtonTapped (GearBindings[i])) inputData->SetGearIndex (i);

	inputData->Perspective = IsButtonTapped (PerspectiveBinding);
	inputData->Gas         = IsButtonDown (GasBinding) * INT8_MAX;
	inputData->Brake       = IsButtonDown (BrakeBinding) * INT8_MAX;
	auto left              = IsButtonDown (WheelLeftBinding) * INT8_MAX;
	auto right             = IsButtonDown (WheelRightBinding) * INT8_MAX;
	inputData->Wheel       = INT8_MAX - left + right;

	ffb::update ();

	return inputData;
}

HOOK (i32, WAJVGetStatus, ASLR (0x140017840)) { return 1; }
HOOK (void, WAJVSetPLParm, ASLR (0x140017B50), u8 a1, u16 a2, u8 a3) {}
HOOK (void, WAJVSetFunctionSettings, ASLR (0x140016AF0), i32 a1, i32 len, void *data) {}
HOOK (bool, WAJVSetOutput, ASLR (0x1400178B0), u32 index, void *output) { return 1; }
HOOK (void *, WAJVSioRead, ASLR (0x140017B10), u32 *out, i32 size) { return 0; }
HOOK (i32, WAJVSioSetMode, ASLR (0x140017980), u8 a1, u8 a2, u8 a3, u16 a4) { return 0; }
HOOK (i32, WAJVUpdate, ASLR (0x140016C10)) { return 1; }
HOOK (i32, WAJVGetNodeCount, ASLR (0x140017880)) { return 1; }
HOOK (void, Breakpoint, PROC_ADDRESS ("kernel32.dll", "DebugBreak")) {}
HOOK (i32, ShowMouse, PROC_ADDRESS ("user32.dll", "ShowCursor"), i32 show) { return originalShowMouse (true); }

HOOK (HWND, WindowCreateW, PROC_ADDRESS ("user32.dll", "CreateWindowExW"), int styleEx, wchar_t *className, wchar_t *windowName, int style, int x, int y, int width, int height, HWND parent,
      void *menu, void *instance, void *param) {
	HWND handle = originalWindowCreateW (styleEx, className, windowName, style, x, y, width, height, parent, menu, instance, param);
	if (windowHandle == 0) {
		toml_table_t *config = openConfig (configPath ("keyconfig.toml"));
		if (config) {
			SetConfigValue (config, "TEST", &TestBinding);
			SetConfigValue (config, "SERVICE", &ServiceBinding);
			SetConfigValue (config, "TEST_UP", &TestUpBinding);
			SetConfigValue (config, "TEST_DOWN", &TestDownBinding);
			SetConfigValue (config, "TEST_ENTER", &TestEnterBinding);

			SetConfigValue (config, "COIN_ADD", &CoinBinding);
			SetConfigValue (config, "CARD_INSERT", &CardInsertBinding);

			SetConfigValue (config, "GEAR_NEXT", &GearNextBinding);
			SetConfigValue (config, "GEAR_PREVIOUS", &GearPreviousBinding);
			SetConfigValue (config, "GEAR_NEUTRAL", &GearBindings[0]);
			SetConfigValue (config, "GEAR_FIRST", &GearBindings[1]);
			SetConfigValue (config, "GEAR_SECOND", &GearBindings[2]);
			SetConfigValue (config, "GEAR_THIRD", &GearBindings[3]);
			SetConfigValue (config, "GEAR_FOURTH", &GearBindings[4]);
			SetConfigValue (config, "GEAR_FIFTH", &GearBindings[5]);
			SetConfigValue (config, "GEAR_SIXTH", &GearBindings[6]);

			SetConfigValue (config, "PERSPECTIVE", &PerspectiveBinding);
			SetConfigValue (config, "GAS", &GasBinding);
			SetConfigValue (config, "BRAKE", &BrakeBinding);
			SetConfigValue (config, "WHEEL_LEFT", &WheelLeftBinding);
			SetConfigValue (config, "WHEEL_RIGHT", &WheelRightBinding);

			toml_free (config);
		}
		windowHandle = handle;
		InitializePoll (handle);
	}
	return handle;
}

HOOK (bool, MoviePlayerOpen, ASLR (0x1407A7040), void *a1, int movieId) { return true; }

BOOL
DllMain (HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		// debug stuff
		AllocConsole ();
		freopen ("CONOUT$", "w", stdout);

		toml_table_t *config = openConfig (configPath ((char *)"config.toml"));
		bool movies          = true;
		bool skipTerminal    = true;
		bool windowed        = true;
		if (config) {
			isTerminal   = readConfigBool (config, "terminal", isTerminal);
			movies       = readConfigBool (config, "movies", movies);
			skipTerminal = readConfigBool (config, "skipTerminal", skipTerminal);
			windowed     = readConfigBool (config, "windowed", windowed);
			strcpy_s (accessCode, readConfigString (config, "accessCode", accessCode));
			strcpy_s (chipId, readConfigString (config, "chipId", chipId));
		}

		INSTALL_HOOK (IoOpen);
		INSTALL_HOOK (DbCreate);
		INSTALL_HOOK (LoadCrown);
		INSTALL_HOOK (DebugPrint);
		if (windowed) INSTALL_HOOK (CreateDeviceAndSwapChain);

		INSTALL_HOOK (HaspDecrypt);
		INSTALL_HOOK (HaspEncrypt);
		INSTALL_HOOK (HaspLogin);
		INSTALL_HOOK (HaspLogout);
		INSTALL_HOOK (HaspWrite);
		INSTALL_HOOK (HaspGetSize);
		INSTALL_HOOK (HaspRead);

		INSTALL_HOOK (WAJVOpen);
		INSTALL_HOOK (WAJVGetInput);
		INSTALL_HOOK (WAJVGetStatus);
		INSTALL_HOOK (WAJVSetPLParm);
		INSTALL_HOOK (WAJVSetFunctionSettings);
		INSTALL_HOOK (WAJVSetOutput);
		INSTALL_HOOK (WAJVSioRead);
		INSTALL_HOOK (WAJVSioSetMode);
		INSTALL_HOOK (WAJVUpdate);
		INSTALL_HOOK (WAJVGetNodeCount);

		INSTALL_HOOK (Breakpoint);
		INSTALL_HOOK (ShowMouse);
		INSTALL_HOOK (WindowCreateW);
		if (!movies) INSTALL_HOOK (MoviePlayerOpen);

		// Display all debug print messages
		WRITE_MEMORY (ASLR (0x1409BD193), u8, 0xEB);

		WRITE_MEMORY (ASLR (0x1407420F9), u8, 0);

		if (isTerminal) {
			WRITE_NOP (ASLR (0x14073B905), 5);
			WRITE_MEMORY (ASLR (0x1409D8A00), u8, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3) // mov eax, 1; ret;
		}
		// Lan Setting
		WRITE_NOP (ASLR (0x140732F36), 2);
		WRITE_NOP (ASLR (0x140B1C743), 2);
		WRITE_MEMORY (ASLR (0x140B41DAA), u8, 0xEB); // Content router

		WRITE_NOP (ASLR (0x1407436E4), 6);
		WRITE_MEMORY (ASLR (0x1407436F9), u8, 0xEB);

		// Safenet check
		WRITE_NOP (ASLR (0x1409C800F), 2);

		// Thanks dmr
		if (skipTerminal) WRITE_NOP (ASLR (0x140744551), 5);

		// Mostly stolen from openparrot
		memset (haspBuffer, 0, 0xD40);
		if (isTerminal) strcpy ((char *)(haspBuffer + 0xD00), "285011501138");
		else strcpy ((char *)(haspBuffer + 0xD00), "285013501138");
		u8 crc = 0;
		for (int i = 0; i < 62; i++)
			crc += haspBuffer[0xD00 + i];
		haspBuffer[0xD3E] = crc;
		haspBuffer[0xD3F] = haspBuffer[0xD3E] ^ 0xFF;
	}
	return TRUE;
}
