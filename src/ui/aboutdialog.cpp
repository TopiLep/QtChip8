#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutDialog::aboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::aboutDialog)
{
    ui->setupUi(this);

    ui->Version->setText(QString(APP_VERSION));

    QObject::connect(ui->aboutQt, &QPushButton::clicked, this, [this]() {
        QMessageBox::aboutQt(this, "About Qt");
    });
}

aboutDialog::~aboutDialog()
{
    delete ui;
}
