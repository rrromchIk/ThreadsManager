#pragma once
#include <QMainWindow>
#include <QDebug>
#include <QTableWidgetItem>
#include "threadfactory.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static Ui::MainWindow *ui;

private:
    ThreadFactory* threadFactory;

private slots:
    void on_resumeButton_clicked();
    void on_suspendButton_clicked();
    void on_terminateButton_clicked();
    void on_setPriorityButton_clicked();
    void on_startButton_clicked();
};

