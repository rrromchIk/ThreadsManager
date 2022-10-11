#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <math.h>

class ThreadFactory {
private:
    int numOfThreads;
    HANDLE *threads;
    DWORD *threadsID;
    HANDLE tableManager;
    HANDLE mutex;

    static long sumOfElements;

    int* array;
    int arraySize;
    void initialiseArray();
    void calculateSumInCertainInterval(int start, int end);

    int begin;
    int end;
public:
    ThreadFactory(int numOfThreads, int arraySize);
    void setPriority(int priority);
    void resumeThread();
    void suspendThread();
    void terminateThread();
    int getSum() const;
    std::string arrayToString() const;
    ~ThreadFactory();

    friend DWORD WINAPI processingArray(LPVOID lpParameter);
    friend DWORD WINAPI updateTable(LPVOID lpParameter);
};


