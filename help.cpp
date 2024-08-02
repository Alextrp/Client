#include "help.h"

MyDialog::MyDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Помощь");
    resize(406, 520);

    QVBoxLayout *layout = new QVBoxLayout(this);
    textBrowser = new QTextBrowser(this);
    layout->addWidget(textBrowser);

    // Example text for the QTextBrowser
    textBrowser->setHtml("<ul>"
                         "<li>Заявка должна состоять из трех чисел, первое число от 0 до 15 (это вид кофигруции (см. ниже))</li>"
                         "<li>второе число от 0 до 2 (это приоритет)</li>"
                         "<li>третье число должно быть не меньше чем 80% от количества прожекторов в конфигурации</li>"
                         "<li>Пример 1,2,14</li>"
                         "<li>0 - одна строка;</li>"
                         "<li>1 - две строки;</li>"
                         "<li>2 - три строки;</li>"
                         "<li>3 - четыре строки;</li>"
                         "<li>4 - один столбец;</li>"
                         "<li>5 - два столбца;</li>"
                         "<li>6 - три столбца;</li>"
                         "<li>7 - четыре столбца;</li>"
                         "<li>8 - один прожектор;</li>"
                         "<li>9 - прямоугольник 1x2;</li>"
                         "<li>10 - прямоугольник 1x3;</li>"
                         "<li>11 - прямоугольник 2x3;</li>"
                         "<li>12 - квадрат 2x2;</li>"
                         "<li>13 - квадрат 3x3;</li>"
                         "<li>14 - квадрат 4x4;</li>"
                         "<li>15 - квадрат 8x8;</li>"
                         "</ul>");
}

MyDialog::~MyDialog()
{
    // Деструктор не требует дополнительных действий
}
