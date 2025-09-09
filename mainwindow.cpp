#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QDomDocument>

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

    model = new QStandardItemModel(0, 8, this);
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
    QXmlStreamReader xml(data);
    qDebug() << "Received:" << xml.text();

    QDomDocument xmlDoc;

    if(!xmlDoc.setContent(data))
    {
        qDebug() << "Invalid xml data!";
        return;
    }


    QDomElement xmlElemRoot = xmlDoc.documentElement();

    if(xmlElemRoot.tagName() != "pallets")
    {
        qDebug() << "Unexpected root element:" << xmlElemRoot.tagName();
        return;
    }

    QDomNode palletNode = xmlElemRoot.firstChild();

    while(!palletNode.isNull())
    {
        QDomElement palletElem = palletNode.toElement();
        if(!palletElem.isNull() && palletElem.tagName() == "pallet")
        {
            QString palletNumber = palletElem.attribute("number");
            QString palletWeight = palletElem.attribute("weight");

            QDomNode containerNode = palletElem.firstChild();
            while(!containerNode.isNull())
            {
                QDomElement containerElem = containerNode.toElement();
                if(!containerElem.isNull())
                {
                    QString type = containerElem.tagName();
                    QString code = containerElem.firstChildElement("code").text();
                    QString height = containerElem.firstChildElement("height").text();
                    QString weight = containerElem.firstChildElement("weight").text();
                    QString length = containerElem.firstChildElement("length").text();
                    QString diameter = containerElem.firstChildElement("diameter").text();
                    QString breadth = containerElem.firstChildElement("breadth").text();


                    QList<QStandardItem*> rowItems;
                    rowItems << new QStandardItem("") // empty column
                             << new QStandardItem(palletNumber)   // Pallet
                             << new QStandardItem(type)        // Container type
                             << new QStandardItem(code)        // Code
                             << new QStandardItem(height)      // Height
                             << new QStandardItem(!breadth.isEmpty() ? breadth : diameter) // Breadth or Diameter
                             << new QStandardItem(length)      // Length
                             << new QStandardItem(weight);     // Weight

                    model->appendRow(rowItems);
                }
                containerNode = containerNode.nextSibling();
            }
        }
        palletNode = palletNode.nextSibling();
    }

    // Optionally, send a reply back
    clientSocket->write("Message received!\n");
}

MainWindow::~MainWindow() {}
