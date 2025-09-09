#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QTcpServer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setMinimumSize(900, 200);

    QVBoxLayout * mainVLayout = new QVBoxLayout(central);
    QHBoxLayout * listeningHLayout = new QHBoxLayout();
    QPushButton * listenButton = new QPushButton("Start Listenting");
    QLabel * listenLabel = new QLabel("Listening on port 6164");

    QStandardItemModel *model = new QStandardItemModel(0, 8, this);
    model->setHorizontalHeaderLabels({" ", "Pallet", "Container", "Code", "Height", "Breadth/Diameter", "Length", "Weight"});

    // Create the table view
    QTableView *tableView = new QTableView(this);
    tableView->setModel(model);

    // Stretch columns to fit
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setVisible(false);

    listeningHLayout->addWidget(listenButton);
    listeningHLayout->addWidget(listenLabel);

    // Connect slots
    connect(listenButton, &QPushButton::clicked, this, &MainWindow::startListening);


    mainVLayout->addLayout(listeningHLayout);
    mainVLayout->addWidget(tableView);
    setCentralWidget(central);
}

void MainWindow::startListening()
{
    server = new QTcpServer(this);
    const QHostAddress * address = new QHostAddress("127.0.0.1");
    uint port = 6164;

    // connect(server, &MainWindow::newConnection, this, &MainWindow::onNewConnection);

    if (!server->listen(*address, port)) {
        QMessageBox::critical(this, "Error", "Could not start server: " + server->errorString());
        return;
    }

    QMessageBox::information(this, "Server", "Listening on port 6164...");

    connect(server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

void MainWindow::onNewConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();

    qDebug() << "New Client connection from"
             << clientSocket->peerAddress();

    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
}

void MainWindow::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    qDebug() << "Received:" << data;

    // Optionally, send a reply back
    clientSocket->write("Message received!\n");
}

MainWindow::~MainWindow() {}
