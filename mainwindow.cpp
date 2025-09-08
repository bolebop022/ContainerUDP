#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

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

    mainVLayout->addLayout(listeningHLayout);
    mainVLayout->addWidget(tableView);
    setCentralWidget(central);
}

MainWindow::~MainWindow() {}
