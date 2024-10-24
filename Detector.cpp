        #include <windows.h>
        #include <tlhelp32.h>
        #include <psapi.h>
        #include <iostream>
        #include <vector>

        // 用于检查进程中是否有对特定进程的内存读取操作
        bool IsReadingMemory(DWORD processID, HANDLE gameHandle) {
            // 创建一个进程快照
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
            if (snapshot == INVALID_HANDLE_VALUE) {
                std::cerr << "Failed to create process snapshot." << std::endl;
                return false;
            }

            MODULEENTRY32 moduleEntry;
            moduleEntry.dwSize = sizeof(MODULEENTRY32);

            if (Module32First(snapshot, &moduleEntry)) {
                do {
                    // 获取进程中的模块
                    HMODULE hModule = GetModuleHandle(moduleEntry.szModule);

                    if (hModule != NULL) {
                        // 获取模块的函数地址
                        FARPROC readMemoryFunc = GetProcAddress(hModule, "ReadProcessMemory");

                        if (readMemoryFunc != NULL) {
                            std::cout << "Process is accessing ReadProcessMemory function from: " << moduleEntry.szModule << std::endl;
                            CloseHandle(snapshot);
                            return true;
                        }
                    }
                } while (Module32Next(snapshot, &moduleEntry));
            }

            CloseHandle(snapshot);
            return false;
        }

        // 查找游戏进程
        DWORD FindProcessID(const std::wstring& processName) {
            DWORD processID = 0;
            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(PROCESSENTRY32);

            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot == INVALID_HANDLE_VALUE) {
                std::cerr << "Failed to create snapshot of processes." << std::endl;
                return 0;
            }

            if (Process32First(snapshot, &processEntry)) {
                do {
                    if (processName == processEntry.szExeFile) {
                        processID = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &processEntry));
            }

            CloseHandle(snapshot);
            return processID;
        }

        int main() {
            // 目标进程的名字，例如游戏进程
            std::wstring gameProcessName = L"XXX.exe"; // 读取游戏.exe文件名
            DWORD gameProcessID = FindProcessID(gameProcessName);
            if (gameProcessID == 0) {
                std::cerr << "Could not find the game process." << std::endl;
                return -1;
            }

            // 打开游戏进程句柄
            HANDLE gameHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, gameProcessID);
            if (!gameHandle) {
                std::cerr << "Failed to open game process." << std::endl;
                return -1;
            }

            std::cout << "Monitoring for memory reading programs..." << std::endl;

            // 获取所有进程并检查是否有可疑的进程正在读取内存
            while (true) {
                HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (snapshot == INVALID_HANDLE_VALUE) {
                    std::cerr << "Failed to create snapshot of processes." << std::endl;
                    break;
                }

                PROCESSENTRY32 processEntry;
                processEntry.dwSize = sizeof(PROCESSENTRY32);

                if (Process32First(snapshot, &processEntry)) {
                    do {
                        // 忽略游戏自身的进程
                        if (processEntry.th32ProcessID == gameProcessID) continue;

                        // 检查其他进程是否在读取游戏进程的内存
                        if (IsReadingMemory(processEntry.th32ProcessID, gameHandle)) {
                            std::cout << "Detected process reading memory: " << processEntry.szExeFile << std::endl;
                        }
                    } while (Process32Next(snapshot, &processEntry));
                }

                CloseHandle(snapshot);

                Sleep(5000); // 每5秒钟检查一次
            }

            CloseHandle(gameHandle);
            return 0;
        }
