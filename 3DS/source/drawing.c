#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <3ds.h>

#include "drawing.h"

static const char fonts[] = { //Fonte 8x8 1BPP
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 //tile:0
	, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0, 0x18, 0x0 //tile:1
	, 0x6c, 0x6c, 0x6c, 0x0, 0x0, 0x0, 0x0, 0x0 //tile:2
	, 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x0 //tile:3
	, 0x18, 0x7e, 0xc0, 0x7c, 0x6, 0xfc, 0x18, 0x0 //tile:4
	, 0x0, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x0 //tile:5
	, 0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x0 //tile:6
	, 0x30, 0x30, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0 //tile:7
	, 0xc, 0x18, 0x30, 0x30, 0x30, 0x18, 0xc, 0x0 //tile:8
	, 0x30, 0x18, 0xc, 0xc, 0xc, 0x18, 0x30, 0x0 //tile:9
	, 0x0, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x0, 0x0 //tile:10
	, 0x0, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x0, 0x0 //tile:11
	, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x18, 0x30 //tile:12
	, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x0, 0x0, 0x0 //tile:13
	, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18, 0x18, 0x0 //tile:14
	, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x0 //tile:15
	, 0x7c, 0xce, 0xde, 0xf6, 0xe6, 0xc6, 0x7c, 0x0 //tile:16
	, 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x0 //tile:17
	, 0x7c, 0xc6, 0x6, 0x7c, 0xc0, 0xc0, 0xfe, 0x0 //tile:18
	, 0xfc, 0x6, 0x6, 0x3c, 0x6, 0x6, 0xfc, 0x0 //tile:19
	, 0xc, 0xcc, 0xcc, 0xcc, 0xfe, 0xc, 0xc, 0x0 //tile:20
	, 0xfe, 0xc0, 0xfc, 0x6, 0x6, 0xc6, 0x7c, 0x0 //tile:21
	, 0x7c, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0x7c, 0x0 //tile:22
	, 0xfe, 0x6, 0x6, 0xc, 0x18, 0x30, 0x30, 0x0 //tile:23
	, 0x7c, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0x7c, 0x0 //tile:24
	, 0x7c, 0xc6, 0xc6, 0x7e, 0x6, 0x6, 0x7c, 0x0 //tile:25
	, 0x0, 0x18, 0x18, 0x0, 0x0, 0x18, 0x18, 0x0 //tile:26
	, 0x0, 0x18, 0x18, 0x0, 0x0, 0x18, 0x18, 0x30 //tile:27
	, 0xc, 0x18, 0x30, 0x60, 0x30, 0x18, 0xc, 0x0 //tile:28
	, 0x0, 0x0, 0x7e, 0x0, 0x7e, 0x0, 0x0, 0x0 //tile:29
	, 0x30, 0x18, 0xc, 0x6, 0xc, 0x18, 0x30, 0x0 //tile:30
	, 0x3c, 0x66, 0xc, 0x18, 0x18, 0x0, 0x18, 0x0 //tile:31
	, 0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x7e, 0x0 //tile:32
	, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0x0 //tile:33
	, 0xfc, 0xc6, 0xc6, 0xfc, 0xc6, 0xc6, 0xfc, 0x0 //tile:34
	, 0x7c, 0xc6, 0xc0, 0xc0, 0xc0, 0xc6, 0x7c, 0x0 //tile:35
	, 0xf8, 0xcc, 0xc6, 0xc6, 0xc6, 0xcc, 0xf8, 0x0 //tile:36
	, 0xfe, 0xc0, 0xc0, 0xf8, 0xc0, 0xc0, 0xfe, 0x0 //tile:37
	, 0xfe, 0xc0, 0xc0, 0xf8, 0xc0, 0xc0, 0xc0, 0x0 //tile:38
	, 0x7c, 0xc6, 0xc0, 0xc0, 0xce, 0xc6, 0x7c, 0x0 //tile:39
	, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x0 //tile:40
	, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x0 //tile:41
	, 0x6, 0x6, 0x6, 0x6, 0x6, 0xc6, 0x7c, 0x0 //tile:42
	, 0xc6, 0xcc, 0xd8, 0xf0, 0xd8, 0xcc, 0xc6, 0x0 //tile:43
	, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0x0 //tile:44
	, 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x0 //tile:45
	, 0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x0 //tile:46
	, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x0 //tile:47
	, 0xfc, 0xc6, 0xc6, 0xfc, 0xc0, 0xc0, 0xc0, 0x0 //tile:48
	, 0x7c, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x6 //tile:49
	, 0xfc, 0xc6, 0xc6, 0xfc, 0xd8, 0xcc, 0xc6, 0x0 //tile:50
	, 0x7c, 0xc6, 0xc0, 0x7c, 0x6, 0xc6, 0x7c, 0x0 //tile:51
	, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0 //tile:52
	, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfe, 0x0 //tile:53
	, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x38, 0x0 //tile:54
	, 0xc6, 0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0x6c, 0x0 //tile:55
	, 0xc6, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0xc6, 0x0 //tile:56
	, 0xc6, 0xc6, 0xc6, 0x7c, 0x18, 0x30, 0xe0, 0x0 //tile:57
	, 0xfe, 0x6, 0xc, 0x18, 0x30, 0x60, 0xfe, 0x0 //tile:58
	, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x0 //tile:59
	, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x2, 0x0 //tile:60
	, 0x3c, 0xc, 0xc, 0xc, 0xc, 0xc, 0x3c, 0x0 //tile:61
	, 0x10, 0x38, 0x6c, 0xc6, 0x0, 0x0, 0x0, 0x0 //tile:62
	, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff //tile:63
	, 0x18, 0x18, 0xc, 0x0, 0x0, 0x0, 0x0, 0x0 //tile:64
	, 0x0, 0x0, 0x7c, 0x6, 0x7e, 0xc6, 0x7e, 0x0 //tile:65
	, 0xc0, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xfc, 0x0 //tile:66
	, 0x0, 0x0, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x0 //tile:67
	, 0x6, 0x6, 0x6, 0x7e, 0xc6, 0xc6, 0x7e, 0x0 //tile:68
	, 0x0, 0x0, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x0 //tile:69
	, 0x1c, 0x36, 0x30, 0x78, 0x30, 0x30, 0x78, 0x0 //tile:70
	, 0x0, 0x0, 0x7e, 0xc6, 0xc6, 0x7e, 0x6, 0xfc //tile:71
	, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x0 //tile:72
	, 0x18, 0x0, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x0 //tile:73
	, 0x6, 0x0, 0x6, 0x6, 0x6, 0x6, 0xc6, 0x7c //tile:74
	, 0xc0, 0xc0, 0xcc, 0xd8, 0xf8, 0xcc, 0xc6, 0x0 //tile:75
	, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x0 //tile:76
	, 0x0, 0x0, 0xcc, 0xfe, 0xfe, 0xd6, 0xd6, 0x0 //tile:77
	, 0x0, 0x0, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x0 //tile:78
	, 0x0, 0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x0 //tile:79
	, 0x0, 0x0, 0xfc, 0xc6, 0xc6, 0xfc, 0xc0, 0xc0 //tile:80
	, 0x0, 0x0, 0x7e, 0xc6, 0xc6, 0x7e, 0x6, 0x6 //tile:81
	, 0x0, 0x0, 0xfc, 0xc6, 0xc0, 0xc0, 0xc0, 0x0 //tile:82
	, 0x0, 0x0, 0x7e, 0xc0, 0x7c, 0x6, 0xfc, 0x0 //tile:83
	, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0xe, 0x0 //tile:84
	, 0x0, 0x0, 0xc6, 0xc6, 0xc6, 0xc6, 0x7e, 0x0 //tile:85
	, 0x0, 0x0, 0xc6, 0xc6, 0xc6, 0x7c, 0x38, 0x0 //tile:86
	, 0x0, 0x0, 0xc6, 0xc6, 0xd6, 0xfe, 0x6c, 0x0 //tile:87
	, 0x0, 0x0, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x0 //tile:88
	, 0x0, 0x0, 0xc6, 0xc6, 0xc6, 0x7e, 0x6, 0xfc //tile:89
	, 0x0, 0x0, 0xfe, 0xc, 0x38, 0x60, 0xfe, 0x0 //tile:90
	, 0xe, 0x18, 0x18, 0x70, 0x18, 0x18, 0xe, 0x0 //tile:91
	, 0x18, 0x18, 0x18, 0x0, 0x18, 0x18, 0x18, 0x0 //tile:92
	, 0x70, 0x18, 0x18, 0xe, 0x18, 0x18, 0x70, 0x0 //tile:93
	, 0x76, 0xdc, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 //tile:94
	, 0x0, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x0 //tile:95
};

inline void clearScreen(void) {
	u8 *frame = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	memset(frame, 0, 320 * 240 * 3);
}

void drawPixelRGBFramebuffer(u8 *fb, int x, int y, u8 r, u8 g, u8 b) {
	y = 240 - y;
	x = x;
	u32 v = (y + x * 240) * 3;
	u8 *topLeftFB = fb ? fb : gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
	topLeftFB[v] = (b >> 3) + ((g & 0x1C) << 3);
	topLeftFB[v+1] = ((g & 0xE0) >> 5) + (r & 0xF8);
}

inline void drawBoxFramebuffer(u8 *fb, int x, int y, int width, int height, u8 r, u8 g, u8 b) {
	int lx, ly;
	for(lx = x; lx < x + width; lx++) {
		for(ly = y; ly < y + height; ly++) {
			drawPixelRGBFramebuffer(fb, lx, ly, r, g, b);
		}
	}
}

void drawStringFramebuffer(u8 *fb, int sx, int sy, char *text, ...) {
	char str[1024];
	va_list args;
	va_start(args, text);
	vsnprintf(str, 1023, text, args);
	va_end(args);
	
	int i;
	for(i = 0; i < strlen(str); i++) {
		int fntnum = (str[i] - 32) & 0xFF;
		int y;
		for(y = 0; y < 8; y++) {
			int currbyte = fonts[(fntnum * 8) + y];
			//Desenha sprite de 1BPP
			int x;
			int mult = 0x80;
			for(x = 0; x < 8; x++) {
				if((currbyte & mult) == mult) {
					drawPixelRGBFramebuffer(fb, sx + x, sy + y, 0xFF, 0xFF, 0xFF);
					drawPixelRGBFramebuffer(fb, sx + x, sy + y + 1, 0, 0, 0); //Sombra
				}
				mult /= 2;
			}
		}
		sx += 8;
	}
}

static u32 brightnessMain;
static u32 brightnessSub;

void disableBacklight() {
	u32 off = 0;
	
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTMAIN, &brightnessMain, 4);
	GSPGPU_ReadHWRegs(REG_LCDBACKLIGHTSUB, &brightnessSub, 4);
	
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTMAIN, &off, 4);
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTSUB, &off, 4);
}

void enableBacklight() {
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTMAIN, &brightnessMain, 4);
	GSPGPU_WriteHWRegs(REG_LCDBACKLIGHTSUB, &brightnessSub, 4);
}
