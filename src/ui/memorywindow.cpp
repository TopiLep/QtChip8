#include "memorywindow.h"
#include "ui_memorywindow.h"

MemoryWindow::MemoryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MemoryWindow)
{
    ui->setupUi(this);

    //read only
    ui->MemoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->MemoryTable->setRowCount(256);
    ui->MemoryTable->setColumnCount(16);

    for (int i = 0; i < 16; i++)
    {
        ui->MemoryTable->setHorizontalHeaderItem(
            i,
            new QTableWidgetItem(QString::number(i, 16).toUpper())
            );

        ui->MemoryTable->setColumnWidth(i, 40);
    }

    for (int row = 0; row < 256; row++)
    {
        QString address =
            QString("%1")
                .arg(row * 16, 3, 16, QChar('0'))
                .toUpper();

        ui->MemoryTable->setVerticalHeaderItem(
            row,
            new QTableWidgetItem(address)
            );

        for (int col = 0; col < 16; ++col)
        {
            ui->MemoryTable->setItem(row, col, new QTableWidgetItem);
        }
    }
}

MemoryWindow::~MemoryWindow()
{
    delete ui;
}

void MemoryWindow::setMemory(const uint8_t *memory, uint16_t pc, uint16_t I)
{
    for (int address = 0; address < 4096; address++)
    {
        int row = address / 16;
        int column = address % 16;

        QString value =
            QString("%1")
                .arg(memory[address], 2, 16, QChar('0'))
                .toUpper();

        QTableWidgetItem *item = ui->MemoryTable->item(row, column);

        item->setText(value);
        item->setBackground(QBrush());
    }


    //nullptr checks before coloring, craches without it.
    //highlight pc
    QTableWidgetItem *pcItem = ui->MemoryTable->item(pc / 16, pc % 16);
    if (pcItem)
        pcItem->setBackground(Qt::green);

    //highlight I
    QTableWidgetItem *iItem = ui->MemoryTable->item(I / 16, I % 16);
    if (iItem)
        iItem->setBackground(Qt::blue);

}