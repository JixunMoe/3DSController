#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "keys.h"
#include "wireless.h"

#include "settings.h"

struct settings settings;

struct settings defaultSettings = {
	port: 8889,
	throttle: 20,
	circlePad: joystick1,
	cStick: joystick2,
	touch: mouse,
	mouseSpeed: 4,
	axisMax: 128,
	axisOffset: 0,
	A: { 1, {'A'} },
	B: { 1, {'B'} },
	X: { 1, {'X'} },
	Y: { 1, {'Y'} },
	L: { 1, {'L'} },
	R: { 1, {'R'} },
	ZL: { 1, {'Q'} },
	ZR: { 1, {'W'} },
	Left: { 0, {VK_LEFT} },
	Right: { 0, {VK_RIGHT} },
	Up: { 0, {VK_UP} },
	Down: { 0, {VK_DOWN} },
	Start: { 0, {VK_RETURN} },
	Select: { 0, {VK_BACK} },
	Tap: { 0, {'T'} },
};

unsigned int readNumber (char *str) {
    if (str[0] == '0' && str[1] == 'x') {
        // HEX
        unsigned int x = 0;
        unsigned int i = 2;
        for (; str[i]; i++) {
            x <<= 4;

            if (str[i] >= '0' && str[i] <= '9') {
                x += (str[i] - '0');
            } else if (str[i] >= 'a' && str[i] <= 'f') {
                x += (str[i] - 'a' + 10);
            } else if (str[i] >= 'A' && str[i] <= 'F') {
                x += (str[i] - 'A' + 10);
            } else {
                printf("[WARN] readNumber: Invalid hex number in config file, ignore %c \n", str[i]);
                x >>= 4;
            }
        }
        return x;
    } else {
        // DEC
        return atoi(str);
    }
}

static bool getSetting(char *name, char *src, char *dest) {
	char *start = strstr(src, name);

	if(start) {
		start += strlen(name);

		char *end = start + strlen(start);
		if(strstr(start, "\n") - 1 < end) end = strstr(start, "\n");
		size_t size = (size_t)end - (size_t)start;

		strncpy(dest, start, size);
		dest[size] = '\0';

        printf("\nconfig: %s raw: %s", name, dest);
		return true;
	}

	return false;
}

static struct keyMapping getButton(char *string) {
	struct keyMapping k = { 1, {0} };

	k.useJoypad = 0;
	if(strcmp(string, "SPACE") == 0) k.virtualKey = VK_SPACE;
	else if(strcmp(string, "CLICK") == 0) k.virtualKey = VK_LBUTTON;
	else if(strcmp(string, "RIGHT CLICK") == 0) k.virtualKey = VK_RBUTTON;
	else if(strcmp(string, "ENTER") == 0) k.virtualKey = VK_RETURN;
	else if(strcmp(string, "BACKSPACE") == 0) k.virtualKey = VK_BACK;
	else if(strcmp(string, "SHIFT") == 0) k.virtualKey = VK_SHIFT;
	else if(strcmp(string, "TAB") == 0) k.virtualKey = VK_TAB;
	else if(strcmp(string, "LEFT") == 0) k.virtualKey = VK_LEFT;
	else if(strcmp(string, "RIGHT") == 0) k.virtualKey = VK_RIGHT;
	else if(strcmp(string, "UP") == 0) k.virtualKey = VK_UP;
	else if(strcmp(string, "DOWN") == 0) k.virtualKey = VK_DOWN;
	else if(strcmp(string, "PAGE UP") == 0) k.virtualKey = VK_PRIOR;
	else if(strcmp(string, "PAGE DOWN") == 0) k.virtualKey = VK_NEXT;
	else if(strcmp(string, "WINDOWS") == 0) k.virtualKey = VK_LWIN;
	else if(strcmp(string, "NONE") == 0) k.virtualKey = 0;

    else if (strncmp(string, "JOY", 3) == 0) {
        unsigned int code = readNumber(string + 3);

        if (code) {
            code --;
            printf(", code %d", code);

            int padNum = 1 + (code >> 3);
            int padMask = BIT(code % 8);

            k.useJoypad    = padNum;
            k.joypadButton = padMask;

            printf(", useJoypad: %d, padMask: %d", padNum, padMask);
        }
    }

    else if (strncmp(string, "KEY", 3) == 0) {
        unsigned int code = readNumber(string + 3);
        k.virtualKey = code;
        printf(", key: 0x%04x / %d", code, code);
    }

	else k.virtualKey = (int)string[0];

	return k;
}

bool readSettings(void) {
	FILE *f;
	size_t len = 0;
	char *buffer = NULL;

	memcpy(&settings, &defaultSettings, sizeof(struct settings));

	f = fopen("3DSController.ini", "rb");
	if(!f) {
		return false;
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);

	buffer = malloc(len);
	if(!buffer) {
		fclose(f);
		return false;
	}

	fread(buffer, 1, len, f);

	char setting[64] = { '\0' };

	if(getSetting("Port: ", buffer, setting)) {
		sscanf(setting, "%d", &settings.port);
	}

	if(getSetting("Throttle: ", buffer, setting)) {
		sscanf(setting, "%d", &settings.throttle);
	}

	if(getSetting("AxisMax: ", buffer, setting)) {
		sscanf(setting, "%d", &settings.axisMax);
		settings.axisOffset = (128 - settings.axisMax);

		printf("\nAxis offset: %d", settings.axisOffset);
	}

	if(getSetting("Circle Pad: ", buffer, setting)) {
		if(strcmp(setting, "MOUSE") == 0) settings.circlePad = mouse;
		else if(strcmp(setting, "JOYSTICK1") == 0) settings.circlePad = joystick1;
		else if(strcmp(setting, "JOYSTICK2") == 0) settings.circlePad = joystick2;
	}

	if(getSetting("C Stick: ", buffer, setting)) {
		if(strcmp(setting, "MOUSE") == 0) settings.cStick = mouse;
		else if(strcmp(setting, "JOYSTICK1") == 0) settings.cStick = joystick1;
		else if(strcmp(setting, "JOYSTICK2") == 0) settings.cStick = joystick2;
	}

	if(getSetting("Touch: ", buffer, setting)) {
		if(strcmp(setting, "MOUSE") == 0) settings.touch = mouse;
		else if(strcmp(setting, "JOYSTICK1") == 0) settings.touch = joystick1;
		else if(strcmp(setting, "JOYSTICK2") == 0) settings.touch = joystick2;
	}

	if(getSetting("Mouse Speed: ", buffer, setting)) {
		sscanf(setting, "%d", &settings.mouseSpeed);
	}

	if(getSetting("A: ", buffer, setting)) settings.A = getButton(setting);
	if(getSetting("B: ", buffer, setting)) settings.B = getButton(setting);
	if(getSetting("X: ", buffer, setting)) settings.X = getButton(setting);
	if(getSetting("Y: ", buffer, setting)) settings.Y = getButton(setting);
	if(getSetting("OL: ", buffer, setting))	settings.L = getButton(setting);
	if(getSetting("OR: ", buffer, setting)) settings.R = getButton(setting);
	if(getSetting("ZL: ", buffer, setting)) settings.ZL = getButton(setting);
	if(getSetting("ZR: ", buffer, setting)) settings.ZR = getButton(setting);
	if(getSetting("Left: ", buffer, setting)) settings.Left = getButton(setting);
	if(getSetting("Right: ", buffer, setting)) settings.Right = getButton(setting);
	if(getSetting("Up: ", buffer, setting)) settings.Up = getButton(setting);
	if(getSetting("Down: ", buffer, setting)) settings.Down = getButton(setting);
	if(getSetting("Start: ", buffer, setting)) settings.Start = getButton(setting);
	if(getSetting("Select: ", buffer, setting)) settings.Select = getButton(setting);
	if(getSetting("Tap: ", buffer, setting)) settings.Tap = getButton(setting);

	fclose(f);

	printf("\n\n");

	return true;
}
