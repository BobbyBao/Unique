
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "BaseApp.h"

#include <string.h>
#include <stdio.h>
#include "Math/MathDefs.h"
using namespace Unique;

BaseApp::BaseApp(){
	mouseCaptured = false;

	speed = 1000;
	time = 0;

	colorBits = 32;
	depthBits = 24;
	stencilBits = 0;

	benchMarkFile = NULL;
}

BaseApp::~BaseApp(){

	if (benchMarkFile) fclose(benchMarkFile);

}

void BaseApp::drawGUI(){
	//switchTo2DMode(false);
	renderer->setup2DMode(0, (float) width, 0, (float) height);


	if (showFPS){
		static float accTime = 0.1f;
		static int fps = 0;
		static int nFrames = 0;

		if (accTime > 0.1f){
			fps = (int) (nFrames / accTime + 0.5f);
			nFrames = 0;
			accTime = 0;
		}
		accTime += frameTime;
		nFrames++;

		char str[16];
		sprintf(str, "%d", fps);

		renderer->drawText(str, 8, 8, 30, 38, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	}

#ifdef PROFILE
	const char *profileString = renderer->getProfileString();
	if (profileString[0]){
		renderer->drawText(profileString, 8, 80, 20, 24, defaultFont, linearClamp, blendSrcAlpha, noDepthTest);
	}
#endif
}

void BaseApp::initTime(){
	time = 0;
	frameTime = 0;

	::initTime();
	start = curr = getCurrentTime();
}

bool BaseApp::load()
{
	linearClamp = renderer->addSamplerState(LINEAR, CLAMP, CLAMP, CLAMP);
	defaultFont = renderer->addFont("Assets/Textures/Fonts/Future.dds", "Assets/Textures/Fonts/Future.font", linearClamp);
	blendSrcAlpha = renderer->addBlendState(BlendOp::SRC_ALPHA, BlendOp::ONE_MINUS_SRC_ALPHA);
	noDepthTest  = renderer->addDepthState(false, false);
	noDepthWrite = renderer->addDepthState(true,  false);
	cullNone  = renderer->addRasterizerState(CullMode::CULL_NONE);
	cullBack  = renderer->addRasterizerState(CullMode::CULL_BACK);
	cullFront = renderer->addRasterizerState(CullMode::CULL_FRONT);

	return true;
}

void BaseApp::updateTime(){
	timestamp prev = curr;

	curr = getCurrentTime();
	frameTime = getTimeDifference(prev, curr);
	time = getTimeDifference(start, curr);
}

void BaseApp::makeFrame(){
//	if (!configDialog->isVisible()) controls();

	renderer->resetStatistics();

#ifdef PROFILE
	if (keys[KEY_F11]){
		renderer->profileFrameStart(frameTime);
	}
#endif

	beginFrame();
		drawFrame();
		drawGUI();
	endFrame();

#ifdef PROFILE
	renderer->profileFrameEnd();
#endif


#define SAMPLE_INTERVAL 0.1f

	// Output frameTimes if enabled
	static float accTime = 0;
	static int nFrames = 0;

	if (benchMarkFile){
		accTime += frameTime;
		nFrames++;

		if (accTime >= SAMPLE_INTERVAL){
			fprintf(benchMarkFile, "%f\n", nFrames / accTime);

			nFrames = 0;
			accTime = 0;
		}
	} else {
		nFrames = 0;
		accTime = 0;
	}
}

void BaseApp::resetCamera(){
	camPos = vec3(0, 0, 0);
	wx = wy = 0;
}

void BaseApp::moveCamera(const vec3 &dir){
	camPos += dir * (frameTime * speed);
}

void BaseApp::controls(){
	// Compute directional vectors from euler angles
	float cosX = Cos(wx), sinX = Sin(wx), cosY = Cos(wy), sinY = Sin(wy);
	vec3 dx(cosY, 0, sinY);
	vec3 dy(-sinX * sinY,  cosX, sinX * cosY);
	vec3 dz(-cosX * sinY, -sinX, cosX * cosY);

	vec3 dir(0, 0, 0);
	if (keys[leftKey])     dir -= dx;
	if (keys[rightKey])    dir += dx;
	if (keys[downKey])     dir -= dy;
	if (keys[upKey])       dir += dy;
	if (keys[backwardKey]) dir -= dz;
	if (keys[forwardKey])  dir += dz;

	float lenSq = dot(dir, dir);
	if (lenSq > 0){
		moveCamera(dir * (1.0f / Sqrt(lenSq)));
	}

	dir = vec3(0, 0, 0);
	if (xStrafeAxis >= 0) dir += joystickAxes[xStrafeAxis] * (invertXStrafeAxis? -dx : dx);
	if (yStrafeAxis >= 0) dir += joystickAxes[yStrafeAxis] * (invertYStrafeAxis? -dy : dy);
	if (zStrafeAxis >= 0) dir += joystickAxes[zStrafeAxis] * (invertZStrafeAxis? -dz : dz);

	if (dot(dir, dir) > 0){
		moveCamera(dir);
	}

	if (xTurnAxis >= 0) wx += (invertXTurnAxis? -2.0f : 2.0f) * joystickAxes[xTurnAxis] * frameTime;
	if (yTurnAxis >= 0) wy += (invertYTurnAxis? -2.0f : 2.0f) * joystickAxes[yTurnAxis] * frameTime;
}

bool BaseApp::onMouseMove(const int x, const int y, const int deltaX, const int deltaY){
	if (mouseCaptured){
#if defined(__APPLE__)
		wx -= (invertMouse? 1 : -1) * mouseSensibility * deltaY;
		wy -= mouseSensibility * deltaX;
#else
		static bool changed = false;
		if (changed = !changed){
			wx += (invertMouse? 1 : -1) * mouseSensibility * (height / 2 - y);
			wy += mouseSensibility * (width / 2 - x);
			setCursorPos(width / 2, height / 2);
		}
#endif

		return true;
	} 
	return false;
}

bool BaseApp::onMouseButton(const int x, const int y, const MouseButton button, const bool pressed){
	if (!mouseCaptured){
		if (button == MOUSE_LEFT && pressed){
			captureMouse(true);
			return true;
		}
	}
	return false;
}

bool BaseApp::onMouseWheel(const int x, const int y, const int scroll){
	if (!mouseCaptured){
	}
	return false;
}

bool BaseApp::onKey(const uint key, const bool pressed){
//#if defined(DEBUG) && defined(WIN32)
#ifdef WIN32
	if (pressed && key == KEY_F12){
		if (OpenClipboard(hwnd)){
			EmptyClipboard();

			char str[256];
			int len = sprintf(str, "camPos = vec3(%.15ff, %.15ff, %.15ff);\r\nwx = %.15ff;\r\nwy = %.15ff;\r\n", camPos.x, camPos.y, camPos.z, wx, wy);

			HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len + 1);
			char *mem = (char *) GlobalLock(handle);
			if (mem != NULL){
				strcpy(mem, str);
				GlobalUnlock(handle);
				SetClipboardData(CF_TEXT, handle);
			}
			CloseClipboard();
		}
	}
#endif

	if (pressed && key == screenshotKey){
		if (!saveScreenshot()){
			ErrorMsg("Couldn't save screenshot");
		}
		return true;
	}

	if (pressed && key == benchmarkKey){
		if (benchMarkFile){
			fclose(benchMarkFile);
			benchMarkFile = NULL;
		} else {
			benchMarkFile = fopen("Benchmark.xls", "w");
			fprintf(benchMarkFile, "Frames/s\n");
		}
		return true;
	}


	bool processed = false;

	if (!mouseCaptured){
	}

	if (!processed){
		if (pressed){
			processed = true;
			if (key == KEY_ESCAPE){
				if (!mouseCaptured || (fullscreen && mouseCaptured)){
					closeWindow(true, true);
				} else {
					captureMouse(false);
				}
			} else if (key == fpsKey){
				showFPS = !showFPS;
			} else if (key == resetKey){
				resetCamera();
			} else if (key == optionsKey){
				
			} else {
				processed = false;
			}
		}
	}

	if (key < elementsOf(keys)) keys[key] = pressed;

	return processed;
}

bool BaseApp::onJoystickAxis(const int axis, const float value){
	if (axis >= 8) return false;

	bool processed = false;
	
	const float deadZone = 0.2f;

	if (Abs(value) < deadZone){
		joystickAxes[axis] = 0;
	} else {
		if (value > 0.0f){
			joystickAxes[axis] = (value - deadZone) / (1.0f - deadZone);
		} else {
			joystickAxes[axis] = (value + deadZone) / (1.0f - deadZone);
		}
	}

	return processed;
}

bool BaseApp::onJoystickButton(const int button, const bool pressed){
	if (button >= 32) return false;

	bool processed = false;
	
	if (!processed){
		if (pressed){
			processed = true;
			if (button == optionsButton){
				
			} else {
				processed = false;
			}
		}
	}

	joystickButtons[button] = pressed;

	return processed;
}

void BaseApp::onSize(const int w, const int h){
	width  = w;
	height = h;

	char str[256];
	sprintf(str, "%s (%dx%d)", getTitle(), w, h);
	setWindowTitle(str);

}

void BaseApp::onClose(){
	captureMouse(false);
}

void BaseApp::toggleFullscreen() {
	closeWindow(false, true);
}

void BaseApp::closeWindow(const bool quit, const bool callUnLoad){
	done = quit;

	if (callUnLoad){
		onClose();
		unload();
	}
	exitAPI();

#if defined(__APPLE__)
	QuitApplicationEventLoop();
#endif
}

#if defined(_WIN32)

void BaseApp::captureMouse(const bool value){
#ifndef NO_CAPTURE_MOUSE
	if (mouseCaptured != value){
		static POINT point;
		if (value){
			GetCursorPos(&point);
			setCursorPos(width / 2, height / 2);
		} else {
			SetCursorPos(point.x, point.y);
		}
		ShowCursor(mouseCaptured);
		mouseCaptured = value;
	}
#endif
}

void BaseApp::setCursorPos(const int x, const int y){
	POINT point = { x, y };
	ClientToScreen(hwnd, &point);
	SetCursorPos(point.x, point.y);
}

void BaseApp::setWindowTitle(const char *title){
	SetWindowText(hwnd, title);
}

#elif defined(LINUX)

void BaseApp::captureMouse(const bool value){
#ifndef NO_CAPTURE_MOUSE
	if (mouseCaptured != value){
		static int mouseX, mouseY;
		if (value){
			XGrabPointer(display, window, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, window, blankCursor, CurrentTime);

			int rx, ry;
			unsigned int mask;
			Window root, child;
			XQueryPointer(display, window, &root, &child, &rx, &ry, &mouseX, &mouseY, &mask);
			setCursorPos(width / 2, height / 2);
		} else {
			setCursorPos(mouseX, mouseY);
			XUngrabPointer(display, CurrentTime);
		}

		mouseCaptured = value;
	}
#endif
}

void BaseApp::setCursorPos(const int x, const int y){
	XWarpPointer(display, None, window, 0, 0, 0, 0, x, y);
}

void BaseApp::setWindowTitle(const char *title){
	XSetStandardProperties(display, window, title, title, /*icon*/None, NULL, 0, NULL);
}

#elif defined(__APPLE__)

void BaseApp::captureMouse(const bool value){
#ifndef NO_CAPTURE_MOUSE
	if (mouseCaptured != value){
		//static int mouseX, mouseY;
		if (value){
			CGDisplayHideCursor(kCGDirectMainDisplay);

			setCursorPos(width / 2, height / 2);
		} else {
			CGDisplayShowCursor(kCGDirectMainDisplay);

			// TODO: Fix ...
			//setCursorPos(mouseX, mouseY);
		}
		CGAssociateMouseAndMouseCursorPosition(mouseCaptured);

		mouseCaptured = value;
	}
#endif
}

void BaseApp::setCursorPos(const int x, const int y){
	Rect structureBounds;
	GetWindowBounds(window, kWindowStructureRgn, &structureBounds);

	CGPoint point;
	point.x = x + structureBounds.left;
	point.y = y + structureBounds.bottom - height;

//	CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, point);
	CGWarpMouseCursorPosition(point);
}

void BaseApp::setWindowTitle(const char *title){
	SetWindowTitleWithCFString(window, CFStringCreateWithCString(NULL, title, kCFStringEncodingMacRoman));
}

#endif

bool BaseApp::saveScreenshot(){
	char path[256];

#if defined(_WIN32)
	SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE);
#elif defined(LINUX) || defined(__APPLE__)
	strcpy(path, getenv("HOME"));
	strcat(path, "/Desktop");
#endif

	FILE *file;
	size_t pos = strlen(path);

	strcpy(path + pos, "/Screenshot00."
/*
#if !defined(NO_PNG)
	"png"
#elif !defined(NO_TGA)
	"tga"
#else*/
	"dds"
//#endif

	);

	pos += 11;

	int i = 0;
	do {
		path[pos]     = '0' + (i / 10);
		path[pos + 1] = '0' + (i % 10);

		if ((file = fopen(path, "r")) != NULL){
			fclose(file);
		} else {
			Image img;
			if (captureScreenshot(img)){
/*
#if !defined(NO_PNG)
				return img.savePNG(path);
#elif !defined(NO_TGA)
				return img.saveTGA(path);
#else*/
				return img.saveDDS(path);
//#endif
			}
			return false;
		}
		i++;
	} while (i < 100);

	return false;
}
