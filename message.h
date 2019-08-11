#ifndef MESSAGE_H
#define MESSAGE_H

#include <QFrame>

namespace Ui {
class message;
}

class message : public QFrame
{
    Q_OBJECT

public:
    explicit message(QWidget *parent = nullptr);
    ~message();

private slots:
    void on_pushButton_released();

    void on_textChanged();
    void on_textBrowser_highlighted(const QString &arg1);

private:
    Ui::message *ui;
};

#endif // MESSAGE_H
