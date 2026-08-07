#include "memorywindow.h"
#include "ui_memorywindow.h"

MemoryWindow::MemoryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MemoryWindow)
{
    ui->setupUi(this);

    //SETUP MEMORY DISPLAY

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


    //Connections

    QObject::connect(ui->PcCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        m_highlightPC = (state == Qt::Checked);
    });

    QObject::connect(ui->IndexCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        m_highlightIndex = (state == Qt::Checked);
    });

    QObject::connect(ui->readOnlyCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        m_readOnly = (state == Qt::Checked);

        if(m_readOnly)
            ui->MemoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        else
            ui->MemoryTable->setEditTriggers(QAbstractItemView::DoubleClicked);

    });


    connect(ui->MemoryTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem *item) {
        int row = item->row();
        int col = item->column();
        int address = row * 16 + col;

        bool ok;
        uint8_t value = item->text().toUInt(&ok, 16);
        if (!ok) return;

        emit memoryEditRequest(address, value);
    });

}

MemoryWindow::~MemoryWindow()
{
    delete ui;
}

void MemoryWindow::setMemory(const uint8_t *memory, uint16_t pc, uint16_t I)
{

    int selectedRow = -1;
    int selectedColumn = -1;

    if (ui->MemoryTable->currentItem()) {
        selectedRow = ui->MemoryTable->currentItem()->row();
        selectedColumn = ui->MemoryTable->currentItem()->column();
    }

    for (int address = 0; address < 4096; address++)
    {    
        int row = address / 16;
        int column = address % 16;

        if(selectedRow == row && selectedColumn == column)
            continue;

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
    if (m_highlightPC) {
        QTableWidgetItem *pcItem = ui->MemoryTable->item(pc / 16, pc % 16);
        if (pcItem)
            pcItem->setBackground(Qt::green);
    }

    //highlight I
    if (m_highlightIndex) {
        QTableWidgetItem *iItem = ui->MemoryTable->item(I / 16, I % 16);
        if (iItem)
            iItem->setBackground(Qt::blue);
    }

}