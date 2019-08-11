#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    if (ui->spinBox->value() == 0) {
        ui->spinBox->setValue(ssDelay);
    }
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_spinBox_valueChanged(int arg1)
{

}


void Preferences::on_buttonBox_accepted()
{
    ssDelay = ui->spinBox->value();
}

void Preferences::on_pushButton_released()
{
    QString theDir;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    theDir,
                                                    QFileDialog::ShowDirsOnly);
}
