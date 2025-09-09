#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTcpSocket>
#include <QMainWindow>
#include <QTcpServer>
#include <QStandardItemModel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void onNewConnection();
    void onReadyRead();
private:
    QStandardItemModel *model;

    void startListening();
    // void receieveXML();
    QTcpServer * server;
};
#endif // MAINWINDOW_H
