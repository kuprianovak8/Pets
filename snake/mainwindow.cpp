#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(320, 380);
    m_gameField = new GameField();
    m_helpField = new HelpField();
    connect(m_gameField, &GameField::ChangeTextSignal, m_helpField, &HelpField::ChangeTextSlot); // соединяем виджеты и слоты
    m_centralWidget = new QWidget();
    m_gridLayout = new QGridLayout();
    m_gridLayout->addWidget(m_gameField, 0, 0);
    m_gridLayout->addWidget(m_helpField, 1, 0);
    m_centralWidget->setLayout(m_gridLayout);
    setCentralWidget(m_centralWidget);
    setStyleSheet("background: rgb(34, 139, 34);");
}

MainWindow::~MainWindow()
{
}

