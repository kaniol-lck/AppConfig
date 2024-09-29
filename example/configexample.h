#ifndef CONFIGEXAMPLE_H
#define CONFIGEXAMPLE_H

#include <QMainWindow>

class QPushButton;
class QScrollArea;

class ConfigExample : public QMainWindow
{
    Q_OBJECT

public:
    ConfigExample(QWidget *parent = nullptr);
    ~ConfigExample() = default;

private:
    QTabWidget *tabWidget_;

    QScrollArea *scrolled(QWidget *widget);
    QPushButton *addWidgetTab(QWidget *widget, const QString &name);
};
#endif // CONFIGEXAMPLE_H
