#include "mainwindow.h"
#include "ui_mainwindow.h"

Ui::MainWindow *MainWindow::ui = new Ui::MainWindow;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui->setupUi(this);
    this->threadFactory = nullptr;
    //this->threadFactory = new ThreadFactory(25, 1000);
}

MainWindow::~MainWindow() {
    delete ui;
    if(threadFactory) {
        delete threadFactory;
    }
}

void MainWindow::on_startButton_clicked() {
    if (threadFactory) {
        delete threadFactory;
    }

    ui->proirityComboBox->setCurrentText("Below normal");
    ui->processesTable->setRowCount(1);

    threadFactory = new ThreadFactory(ui->numOfProcessesLineEdit->text().trimmed().toInt(), ui->arraySizeLineEdit->text().toInt());
}

void MainWindow::on_resumeButton_clicked() {
    if(threadFactory) {
        threadFactory->resumeThread();
    }
}

void MainWindow::on_suspendButton_clicked() {
    if(threadFactory) {
        threadFactory->suspendThread();
    }
}


void MainWindow::on_terminateButton_clicked() {
    if(threadFactory) {
        threadFactory->terminateThread();
    }
}

void MainWindow::on_setPriorityButton_clicked() {
    int priority;
    QString comboBoxState = ui->proirityComboBox->currentText();
    if(!comboBoxState.compare("Idle")) { priority = THREAD_PRIORITY_LOWEST; }
    else if (!comboBoxState.compare("Below normal")) priority = THREAD_PRIORITY_BELOW_NORMAL;
    else if (!comboBoxState.compare("Normal")) priority = THREAD_PRIORITY_NORMAL;
    else if (!comboBoxState.compare("Above normal")) priority = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (!comboBoxState.compare("High")) priority = THREAD_PRIORITY_HIGHEST;
    else if (!comboBoxState.compare("Time critical")) priority = THREAD_PRIORITY_TIME_CRITICAL;
    else priority = 0;
    if(threadFactory) {
        threadFactory->setPriority(priority);
    }
}
