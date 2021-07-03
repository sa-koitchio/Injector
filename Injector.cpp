// Injector.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>

using namespace std;

void getProcessId(const char* window_title, DWORD& process_id)
{
    GetWindowThreadProcessId(FindWindow(NULL, window_title), &process_id);
}

void showErr(const char* errTitle, const char* errMessage)
{
    MessageBox(0, errMessage, errTitle, NULL);
    exit(-1);
}

bool fileExists(string fileName)
{
    struct stat buffer;
    return (stat(fileName.c_str(), &buffer) == 0);
}

int main(int argc, char* argv[])
{
    DWORD processID = NULL;
    char dllPath[MAX_PATH];
    if (argc != 3)
    {
        showErr("Argument error", "Takes 2 Args: dllName , Window title");
        exit(-1);
    }



    const char* dllName = argv[1];
    const char* windowTitle = argv[2];

    if (!fileExists(dllName)) // File not found
    {
        showErr("File Not Found", "File doesn't exist");
    }
    if (GetFullPathName(dllName, MAX_PATH, dllPath, nullptr)) // The File path name couldnt be found
    {
        showErr("GetFullPathName", "Couldnt find path.");
    }

    getProcessId(windowTitle, processID);
    if(processID == NULL)
    {
        showErr("getProcessId", "Could not get process ID");
    }

    HANDLE h_process = OpenProcess(PROCESS_ALL_ACCESS, NULL, processID); // get process handle
    if (!h_process) // Process Handle is invalid
    {
        showErr("OpenProcess", "Could not verify process handle.");
    }
    void* allocatedMem = VirtualAllocEx(h_process, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocatedMem)
    {
        showErr("VirtualAllocEx", "Failed to allocate Memory.");
    }

    if (!WriteProcessMemory(h_process, nullptr, dllPath, MAX_PATH, nullptr))
    {
        showErr("WriteProcessMemory", "Failed to write process memory.");
    }

    HANDLE h_thread = CreateRemoteThread(h_process, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocatedMem, NULL, nullptr);
    if (!h_thread)
    {
        showErr("CreateRemoteThread", "Failed to create remote thread");
    }

    CloseHandle(h_process);
    VirtualFreeEx(h_process, allocatedMem, NULL, MEM_RELEASE);
    MessageBox(0, "Injected DLL ", "Success!", 0);

}

