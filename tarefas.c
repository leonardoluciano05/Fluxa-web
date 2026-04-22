#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <time.h>

double getCPUUsage()
{
    static FILETIME prevIdle = {0}, prevKernel = {0}, prevUser = {0};
    FILETIME idle, kernel, user;

    GetSystemTimes(&idle, &kernel, &user);

    ULONGLONG idleNow = *(ULONGLONG *)&idle;
    ULONGLONG kernelNow = *(ULONGLONG *)&kernel;
    ULONGLONG userNow = *(ULONGLONG *)&user;

    ULONGLONG idlePrev = *(ULONGLONG *)&prevIdle;
    ULONGLONG kernelPrev = *(ULONGLONG *)&prevKernel;
    ULONGLONG userPrev = *(ULONGLONG *)&prevUser;

    if (idlePrev == 0)
    {
        prevIdle = idle;
        prevKernel = kernel;
        prevUser = user;
        return 0.0;
    }

    ULONGLONG idleDiff = idleNow - idlePrev;
    ULONGLONG kernelDiff = kernelNow - kernelPrev;
    ULONGLONG userDiff = userNow - userPrev;

    ULONGLONG total = kernelDiff + userDiff;
    double usage = (1.0 - (double)idleDiff / total) * 100.0;

    prevIdle = idle;
    prevKernel = kernel;
    prevUser = user;

    return usage;
}

void getTimestamp(char *buffer)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", t);
}

int main()
{
    char cwd[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cwd);
    printf("Rodando em: %s\n", cwd);

    MEMORYSTATUSEX mem;
    mem.dwLength = sizeof(mem);

    while (1)
    {
        double cpu = getCPUUsage();

        // RAM
        GlobalMemoryStatusEx(&mem);
        DWORDLONG ramTotal = mem.ullTotalPhys;
        DWORDLONG ramUsada = ramTotal - mem.ullAvailPhys;

        // DISCO
        ULARGE_INTEGER totalBytes, freeBytes;
        GetDiskFreeSpaceEx("C:\\", NULL, &totalBytes, &freeBytes);

        unsigned long long discoTotal = totalBytes.QuadPart;
        unsigned long long discoUsado = discoTotal - freeBytes.QuadPart;

        // PROCESSOS + THREADS
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        int processos = 0;
        int threads = 0;

        if (Process32First(snap, &pe))
        {
            do
            {
                processos++;
                threads += pe.cntThreads;
            } while (Process32Next(snap, &pe));
        }

        CloseHandle(snap);

        // TIMESTAMP
        char timestamp[64];
        getTimestamp(timestamp);

        // 🔥 AQUI ESTÁ O CAMINHO CERTO
        FILE *json = fopen("json/sistema.json", "w");

        if (json == NULL)
        {
            printf("ERRO ao criar o JSON!\n");
            printf("Verifique se a pasta 'json' existe.\n");
            Sleep(2000);
            continue;
        }

        fprintf(json, "{\n");
        fprintf(json, "  \"timestamp\": \"%s\",\n", timestamp);
        fprintf(json, "  \"cpu_uso\": %.2f,\n", cpu);
        fprintf(json, "  \"ram_total\": %llu,\n", ramTotal);
        fprintf(json, "  \"ram_usada\": %llu,\n", ramUsada);
        fprintf(json, "  \"disco_total\": %llu,\n", discoTotal);
        fprintf(json, "  \"disco_usado\": %llu,\n", discoUsado);
        fprintf(json, "  \"processos\": %d,\n", processos);
        fprintf(json, "  \"threads\": %d\n", threads);
        fprintf(json, "}\n");

        fclose(json);

        printf("JSON atualizado!\n");

        Sleep(2000);
    }

    return 0;
}