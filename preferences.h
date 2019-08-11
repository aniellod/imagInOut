#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>

extern int ssDelay;

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();

private slots:
    void on_spinBox_valueChanged(int arg1);
    void on_buttonBox_accepted();

    void on_pushButton_released();

private:
   Ui::Preferences *ui;

//protected:

};

#endif // PREFERENCES_H
