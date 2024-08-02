#ifndef HELP_H
#define HELP_H

#include <QDialog>
#include <QTextBrowser>
#include <QVBoxLayout>

class MyDialog : public QDialog {
    Q_OBJECT

public:
    explicit MyDialog(QWidget *parent = nullptr);
    ~MyDialog();

private:
    QTextBrowser *textBrowser;
};

#endif // HELP_H
