#include "message.h"
#include "ui_message.h"

message::message(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::message)
{
    ui->setupUi(this);
}

message::~message()
{
    delete ui;
}

void message::on_pushButton_released()
{
    this->close();
}

void message::on_textBrowser_highlighted(const QString &arg1)
{

}

void message::on_textChanged() {
    ui->textBrowser->setText("hoolaa");
}
