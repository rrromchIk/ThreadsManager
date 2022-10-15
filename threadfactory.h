#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <math.h>

class ThreadFactory {
private:
    //for all threads
    int numOfThreads;
    HANDLE *threads;
    DWORD *threadsID;

    //for updating tanble
    HANDLE tableManager;

    //MUTEX
    HANDLE mutex;

    //SEMAPHORE
    HANDLE semaphore;

    //BUSY WAITING
    static long isBusy;

    //common resource
    static long sumOfElements;

    //array for calculation sum
    int* array;
    int arraySize;
    void initialiseArray();
    void calculateSumInCertainInterval(int start, int end);

    //deviding array into intervals
    int begin;
    int end;

public:
    ThreadFactory(int numOfThreads, int arraySize);
    void setPriority(int priority);
    void resumeThread();
    void suspendThread();
    void terminateThread();
    void terminateAllThreads();

    int getSum() const;
    std::string arrayToString() const;
    ~ThreadFactory();

    friend DWORD WINAPI processingArray(LPVOID lpParameter);
    friend DWORD WINAPI updateTable(LPVOID lpParameter);
};


