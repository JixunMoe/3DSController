// 3DS Controller Server

#define VERSION 0.6

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "wireless.h"
#include "keys.h"
#include "general.h"
#include "joystick.h"
#include "settings.h"
#include "keyboard.h"

int mi_x, ma_x;
int mi_y, ma_y;
bool readSettings_local ();

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow) {
	if(!readSettings_local()) {
		printf("Couldn't read settings file, using default key bindings.\n");
	}

	printf("3DS Controller Server %.1f\n", VERSION);
    printf("Mod by Jixun: ppsspp for windows support\n");

	DWORD screenWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD screenHeight = GetSystemMetrics(SM_CYSCREEN);

	double widthMultiplier = screenWidth / 320.0;
	double heightMultiplier = screenHeight / 240.0;

	bool vJoy = !!settings.bEnableJoy;
	UINT iInterface = 1;

	iReport.wAxisX = JOY_MIDDLE;
	iReport.wAxisY = JOY_MIDDLE;
	iReport.wAxisZ = JOY_MIDDLE;
	iReport.wAxisXRot = JOY_MIDDLE;
	iReport.wAxisYRot = JOY_MIDDLE;
	iReport.wAxisZRot = JOY_MIDDLE;
	iReport.wSlider = JOY_MIDDLE;
	iReport.wDial = JOY_MIDDLE;
	iReport.lButtons = 0;
	iReport.bHats = -1;

	if(vJoy && !vJoyEnabled()) {
		printf("vJoy failed (1)! Buttons will still work, but joy stick won't work.\n");
		vJoy = false;
	}

	enum VjdStat status = GetVJDStatus(iInterface);
	if(vJoy && (status == VJD_STAT_OWN || (status == VJD_STAT_FREE && !AcquireVJD(iInterface)))) {
		printf("vJoy failed (2)! Buttons will still work, but joy stick won't work.\n");
		vJoy = false;
	}

	ContPovNumber = GetVJDContPovNumber(iInterface);
	//int DiscPovNumber = GetVJDDiscPovNumber(iInterface);

	if(vJoy && !updateJoystick()) {
		printf("vJoy failed (3)! Buttons will still work, but joystick won't work.\n");
		vJoy = false;
	}

	initNetwork();

	char nButtons = GetVJDButtonNumber(iInterface);
	if(nButtons <16) printf("Your vJoy has less than 16 buttons (8 by default), some may not work!\n");

	printf("Port: %d\n", settings.port);

	printf("Running on: %s\n", hostName);

	printf("Your local IP(s):\n");
	printIPs();

	printf("\n");

	startListening();

	while(1) {
		memset(&buffer, 0, sizeof(struct packet));

		while(receiveBuffer(sizeof(struct packet)) <= 0) {
			// Waiting

			Sleep(settings.throttle);
		}

		keyboardActive = buffer.keyboardActive;

		switch(buffer.command) {
			case CONNECT:
				lastKeys = 0;
				currentKeys = 0;
				circlePad.x = 0;
				circlePad.y = 0;
				lastTouch.x = 0;
				lastTouch.y = 0;
				currentTouch.x = 0;
				currentTouch.y = 0;
				cStick.x = 0;
				cStick.y = 0;

				buffer.command = CONNECT;
				printf("3DS Connected!\n");

				Sleep(50);
				sendBuffer(1);

				Sleep(50);
				sendBuffer(1);

				Sleep(50);
				sendBuffer(1);
				break;

			case KEYS:
				lastKeys = currentKeys;
				if(currentKeys & KEY_TOUCH) lastTouch = currentTouch;

				// printf("currentKeys: %08x\n", currentKeys);
				memcpy(&currentKeys, &buffer.keys, 4);
				memcpy(&circlePad, &buffer.circlePad, 4);
				memcpy(&currentTouch, &buffer.touch, 4);
				memcpy(&cStick, &buffer.cStick, 4);

				handleKey(KEY_A, settings.A);
				handleKey(KEY_B, settings.B);
				handleKey(KEY_SELECT, settings.Select);
				handleKey(KEY_START, settings.Start);

				if (settings.bEnableJoy) {
                    iReport.bHats = -1;
                    handleDPAD(KEY_DUP, 0);
                    handleDPAD(KEY_DRIGHT, 1);
                    handleDPAD(KEY_DDOWN, 2);
                    handleDPAD(KEY_DLEFT, 3);
				} else {
                    handleKey(KEY_DRIGHT, settings.Right);
                    handleKey(KEY_DLEFT, settings.Left);
                    handleKey(KEY_DUP, settings.Up);
                    handleKey(KEY_DDOWN, settings.Down);
				}

				handleKey(KEY_R, settings.R);
				handleKey(KEY_L, settings.L);
				if (settings.triggerAsAxis && settings.bEnableJoy) {
                    // Due to lack support of 9/10th axis emulation,
                    // It's unable to emulate LT and RT for game
                    // Ori and the Blind Forest.
                    if (currentKeys & KEY_ZL) {
                        iReport.wAxisZ = -1;
                    } else if (currentKeys & KEY_ZR) {
                        iReport.wAxisZ = +1;
                    } else {
                        iReport.wAxisZ = 0x4000;
                    }
				} else {
                    handleKey(KEY_ZL, settings.ZL);
                    handleKey(KEY_ZR, settings.ZR);
				}
				handleKey(KEY_X, settings.X);
				handleKey(KEY_Y, settings.Y);
				//handleKey(KEY_LID, 'I');

				if(newpress(KEY_TOUCH)) {
					lastTouch.x = currentTouch.x;
					lastTouch.y = currentTouch.y;
				}

				if((currentKeys & KEY_TOUCH)) {
					if(keyboardActive) {
						if(newpress(KEY_TOUCH)) {
							char letter = currentKeyboardKey();
							if(letter) {
								simulateKeyNewpress(letter);
								simulateKeyRelease(letter);
							}
						}
					}
					else if(settings.touch == mouse) {
						if(settings.mouseSpeed) {
							POINT p;
							GetCursorPos(&p);
							SetCursorPos(p.x + (currentTouch.x - lastTouch.x) * settings.mouseSpeed, p.y + (currentTouch.y - lastTouch.y) * settings.mouseSpeed);
						}
						else {
							SetCursorPos((int)((double)currentTouch.x * widthMultiplier), (int)((double)currentTouch.y * heightMultiplier));
						}
					}
					else if(settings.touch == joystick1) {
						joyX = (currentTouch.x) * 128;
						joyY = (currentTouch.y) * 128;
					}

					else if(settings.touch == joystick2) {
						joyRX = (currentTouch.x) * 128;
						joyRY = (currentTouch.y) * 128;
					}
					else {
						handleKey(KEY_TOUCH, settings.Tap);
					}
				}

				if(settings.circlePad == mouse) {
					if(abs(circlePad.x) < settings.mouseSpeed * 3) circlePad.x = 0;
					if(abs(circlePad.y) < settings.mouseSpeed * 3) circlePad.y = 0;

					POINT p;
					GetCursorPos(&p);
					SetCursorPos(p.x + (circlePad.x * settings.mouseSpeed) / 32, p.y - (circlePad.y * settings.mouseSpeed) / 32);
				}
				else if(settings.circlePad == joystick1) {
					joyX = (circlePad.x + 156) * 101;
					joyY = (169 - circlePad.y) * 100;

					if (joyX < 0) {
                        printf("[WARN] joyX out of range, raw %d, mapped %ld\n", circlePad.x, joyX);
                        joyX = 0;
					}

					if (joyY < 0) {
                        printf("[WARN] joyY out of range, raw %d, mapped %ld\n", circlePad.y, joyY);
                        joyY = 0;
					}

                    // printf("raw:  (%d, %d)\n", circlePad.x, circlePad.y);
                    // printf("calc: (%ld, %ld)\n", joyX, joyY);
				}
				else if(settings.circlePad == joystick2) {
					joyRX = (circlePad.x + 128) * 128;
					joyRY = (128 - circlePad.y) * 128;
				}

				if(settings.cStick == mouse) {
					if(abs(cStick.x) < settings.mouseSpeed * 3) cStick.x = 0;
					if(abs(cStick.y) < settings.mouseSpeed * 3) cStick.y = 0;

					POINT p;
					GetCursorPos(&p);
					SetCursorPos(p.x + (cStick.x * settings.mouseSpeed) / 32, p.y - (cStick.y * settings.mouseSpeed) / 32);
				}
				else if(settings.cStick == joystick1) {
					joyX = (cStick.x + 146) * 224;
					joyY = (146 - cStick.y) * 224;
				}
				else if(settings.cStick == joystick2) {
					joyRX = (cStick.x + 146) * 224;
					joyRY = (146 - cStick.y) * 224;

					/*
					mi_x = min(cStick.x, mi_x);
					mi_y = min(cStick.y, mi_y);
					ma_x = max(cStick.x, ma_x);
					ma_y = max(cStick.y, ma_y);

                    printf("raw:  (%d, %d)\n", cStick.x, cStick.y);
                    printf("calc: (%ld, %ld)\n", joyRX, joyRY);
                    printf("min(%d, %d); max(%d, %d);\n", mi_x, mi_y, ma_x, ma_y);
                    */
				}

				break;
		}

		if(vJoy) updateJoystick();
	}

	error("accept()");
	return 0;
}

LPSTR W2A(LPWSTR pString, size_t pad) {
    int nSize = wcslen(pString);
    size_t charsNeeded = WideCharToMultiByte(CP_ACP, 0, pString, nSize, NULL, 0, NULL, NULL);

    char* buffer = malloc(charsNeeded + pad);
    WideCharToMultiByte(CP_ACP, 0, pString, nSize, buffer, charsNeeded, NULL, NULL);
    buffer[charsNeeded] = '\x00';
    return buffer;
}

bool readSettings_local () {
    LPWSTR szDefault = L"3DSController";
    LPWSTR *szArglist;
    LPWSTR lpConfigPath;
    int nArgs;
    int i;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if (nArgs > 1) {
        lpConfigPath = szArglist[1];
    } else {
        lpConfigPath = szDefault;
    }

    LPSTR lpPath = W2A(lpConfigPath, 5);
    i = strlen(lpPath);
    lpPath[i] = '.';
    lpPath[i + 1] = 'i';
    lpPath[i + 2] = 'n';
    lpPath[i + 3] = 'i';
    lpPath[i + 4] = '\x00';
    bool r = readSettings(lpPath);

    free(lpPath);
    LocalFree(szArglist);

    return r;
}
