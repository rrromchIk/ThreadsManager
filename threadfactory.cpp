#include "threadfactory.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

long ThreadFactory::sumOfElements = 0;

std::string getPriority(HANDLE handle) {
    int priority = GetThreadPriority(handle);//GetPriorityClass(handle);
    switch(priority) {
        case THREAD_PRIORITY_LOWEST: return "Idle";
        case THREAD_PRIORITY_BELOW_NORMAL: return "Below normal";
        case THREAD_PRIORITY_NORMAL: return "Normal";
        case THREAD_PRIORITY_ABOVE_NORMAL: return "Above normal";
        case THREAD_PRIORITY_HIGHEST: return "High";
        case THREAD_PRIORITY_TIME_CRITICAL: return "Time critical";
        default: return "Unknown";
    }
}

std::string getStatus(HANDLE handle) {
    DWORD status = WaitForSingleObject(handle, 0);
        switch(status) {
            case WAIT_OBJECT_0: return "Terminated";
            default:
                int value = SuspendThread(handle);
                ResumeThread(handle);
                if(value == 0){
                    return "Running";
                }
                return "Suspended";
        }
}


DWORD WINAPI updateTable(LPVOID lpParameter) {
    ThreadFactory* manager = (ThreadFactory*) lpParameter;

    FILETIME creationTime, exitTime, kernelTime, userTime;

    bool *statuses = new bool[manager->numOfThreads];
    for(int i = 0; i < manager->numOfThreads; i++) {
        statuses[i] = true;
    }
    std::string status;

    QTableWidgetItem *item;
    Qt::ItemFlags flags;
    for(int i = 0; i < MainWindow::ui->processesTable->rowCount(); ++i) {
        for(int j = 0; j < MainWindow::ui->processesTable->columnCount(); ++j) {
            item = new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignCenter);
            flags = item->flags();
            flags &= ~Qt::ItemIsEditable;
            item->setFlags(flags);
            MainWindow::ui->processesTable->setItem(i, j, item);
        }
    }

    while(1) {
        for(int i = 0; i < manager->numOfThreads; i++) {
            if(!statuses[i])
                continue;
            GetThreadTimes(manager->threads[i], &creationTime, &exitTime, &kernelTime, &userTime);
            status = getStatus(manager->threads[i]);

            MainWindow::ui->processesTable->item(i, 0)->setText(QString::number(manager->threadsID[i]));
            MainWindow::ui->processesTable->item(i, 1)->setText(QString::fromStdString(status));
            MainWindow::ui->processesTable->item(i, 2)->setText(QString::fromStdString(getPriority(manager->threads[i])));
            MainWindow::ui->processesTable->item(i, 3)->setText("Not Terminated");

            if(status == "Terminated") {
                MainWindow::ui->processesTable->item(i, 3)->setText(QString::number((exitTime.dwLowDateTime - creationTime.dwLowDateTime) * pow(10.0, -7), 'g', 5) + "s");
                statuses[i] = false;
            }
            MainWindow::ui->sumLineEdit->setText(QString::number(manager->getSum()));
        }
        Sleep(100);
    }
}

DWORD WINAPI processingArray(LPVOID lpParameter) {
    ThreadFactory* treads = (ThreadFactory*) lpParameter;
    treads->calculateSumInCertainInterval(treads->begin, treads->end);
    return 0;
}

ThreadFactory::ThreadFactory(int numOfThreads, int arraySize) {
    mutex = CreateMutexA(NULL, false, NULL);
    this->sumOfElements = 0;
    this->numOfThreads = numOfThreads;
    this->arraySize = arraySize;
    this->threads = new HANDLE[numOfThreads];
    this->threadsID = new DWORD[numOfThreads];
    initialiseArray();
    int gap;

    MainWindow::ui->processesTable->setRowCount(this->numOfThreads);
    tableManager = CreateThread(NULL, NULL, updateTable, this, NULL, NULL);

    for(int i = 0; i < numOfThreads; ++i) {
        gap = arraySize / numOfThreads;
        begin = gap * i;
        if(i == numOfThreads - 1) {
            gap = arraySize - gap * (numOfThreads - 1);
        }
        end = begin + gap;

        threads[i] = CreateThread(NULL, NULL, processingArray, new ThreadFactory(*this), NULL, &threadsID[i]);
    }

    //WaitForMultipleObjects(numOfThreads, threads, true, INFINITE);
}

void ThreadFactory::calculateSumInCertainInterval(int begin, int end) {
    for(int i = begin; i < end; ++i) {
        WaitForSingleObject(mutex, INFINITE);
        sumOfElements += array[i];
        ReleaseMutex(mutex);
        //InterlockedExchangeAdd(&sumOfElements, array[i]);
        Sleep(100);
    }
}

void ThreadFactory::initialiseArray() {
    this->array = new int[arraySize];
    //array[0] = 2;
    for(int i = 0; i < arraySize; ++i) {
        array[i] = 2;//array[i-1] * i + exp(i);
    }
}

void ThreadFactory::setPriority(int priority) {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    SetThreadPriority(threads[items[0]->row()], priority);
}

void ThreadFactory::resumeThread() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    ResumeThread(threads[items[0]->row()]);
}

void ThreadFactory::suspendThread() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    SuspendThread(threads[items[0]->row()]);
}

void ThreadFactory::terminateThread() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    TerminateThread(threads[items[0]->row()], EXIT_SUCCESS);
}

int ThreadFactory::getSum() const {
    return this->sumOfElements;
}

std::string ThreadFactory::arrayToString() const {
    std::string str = "[";
    for(int i = 0; i < arraySize; ++i) {
        str += QString::number(array[i]).toStdString();
        if(i != arraySize - 1)
            str += ", ";
    }
    str += "]";
    return str;
}

ThreadFactory::~ThreadFactory() {
    for (int i = 0; i < numOfThreads; ++i) {
        TerminateThread(threads[i], EXIT_SUCCESS);
        CloseHandle(threads[i]);
    }
    TerminateThread(tableManager, EXIT_SUCCESS);
    CloseHandle(mutex);
    CloseHandle(tableManager);

    delete this->threads;
    delete this->threadsID;

    MainWindow::ui->processesTable->setRowCount(1);
}
