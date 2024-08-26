
#include "header.hpp"
#include <windows.h>
#include <SFML/Window.hpp>

#if !defined(__unix__) && !defined(__unix)

namespace rawinput {

int deltaX = 0;
int deltaY = 0;
POINT currentPos;
POINT prevPos;
int screenWidth;
int screenHeight;

const int THRESHOLD = 10;
const int REQUIRED_FRAMES = 5;



class MouseTracker {
public:
    MouseTracker() : frameCount(0) {}

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  

    bool isCentered() {
        
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            RECT rcApp;
            GetWindowRect(hwnd, &rcApp);
            if(rcApp.right != NULL && rcApp.bottom != NULL){
                screenWidth = rcApp.right;
                screenHeight = rcApp.bottom;
            }
        }

        POINT cursorPos;
        GetCursorPos(&cursorPos);
        // std::cout << screenWidth << std::endl;
        int screenX = (cursorPos.x / screenWidth);
        int screenY = (cursorPos.y / screenHeight);

        int centerX = (screenWidth / 2) + screenWidth * screenX;
        int centerY = (screenHeight / 2) + screenHeight * screenY;

        if (cursorPos.x >= centerX - THRESHOLD && cursorPos.x <= centerX + THRESHOLD &&
            cursorPos.y >= centerY - THRESHOLD && cursorPos.y <= centerY + THRESHOLD) {
            frameCount++;
        } else {
            frameCount = 0;
        }

        return frameCount >= REQUIRED_FRAMES;
    }

    void setWindowHandle(HWND hwnd) {
        windowHandle = hwnd;
    }

private:
    int frameCount;
    HWND windowHandle;
};


MouseTracker mouseTracker;


bool isMouseBeingCentered(){
    return mouseTracker.isCentered();
}

void RegisterRawInputDevices(HWND hwnd) {
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // Generic desktop controls
    rid.usUsage = 0x02; // Mouse
    rid.dwFlags = RIDEV_INPUTSINK; // Capture input even when not in the foreground
    rid.hwndTarget = hwnd;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        MessageBox(NULL, "Failed to register raw input device.", "Error", MB_OK);
    }
}

void ProcessRawInput(LPARAM lParam) {
    UINT dwSize = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    if (dwSize > 0) {
        RAWINPUT* pRawInput = (RAWINPUT*)malloc(dwSize);
        if (pRawInput) {
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                if (pRawInput->header.dwType == RIM_TYPEMOUSE) {
                    deltaX = pRawInput->data.mouse.lLastX;
                    deltaY = pRawInput->data.mouse.lLastY;
                }
            }
            free(pRawInput);
        }
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INPUT:
        ProcessRawInput(lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



bool init(HINSTANCE hInstance){
      // rawinput window
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SFMLWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "Bongo Cat for osu!", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    RegisterRawInputDevices(hwnd);
    mouseTracker.setWindowHandle(hwnd);
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    std::cout << screenWidth;
    return 1;
}

POINT getMousePos(){
    currentPos.x += (float)deltaX * data::cfg["decoration"]["fpsSensitivity"].asFloat();
    currentPos.y += (float)deltaY * data::cfg["decoration"]["fpsSensitivity"].asFloat();
    deltaX = 0;
    deltaY = 0;

    // drift the mouse towards center overtime
    int speed = data::cfg["decoration"]["fpsReturnSpeed"].asInt();
    if(prevPos.x == currentPos.x && prevPos.y == currentPos.y){
        int targetX = screenWidth / 2;
        int targetY = screenHeight / 2;

        if (currentPos.x < targetX - speed - 5) {
            currentPos.x += speed; // Move right
        } else if (currentPos.x > targetX +  speed + 5) {
            currentPos.x -= speed; // Move left
        }

        if (currentPos.y < targetY - speed - 5) {
            currentPos.y += speed; // Move down
        } else if (currentPos.y > targetY +  speed + 5) {
            currentPos.y -= speed; // Move up
        }
    }
    prevPos = currentPos;

    // clamp to screen's resolution
    currentPos.x = std::clamp((int)currentPos.x, 0, (int) screenWidth - speed - 5);
    currentPos.y = std::clamp((int)currentPos.y, 0, (int) screenHeight - speed - 5);

    return currentPos;
}





}
#endif
