#ifndef PRHP_H
#define PRHP_H

#include <QMainWindow>

class QPushButton;
class QScrollArea;

class PrHp : public QMainWindow
{
    Q_OBJECT

public:
    PrHp(QWidget *parent = nullptr);
    ~PrHp() = default;

private:
    QTabWidget *tabWidget_;

    QScrollArea *scrolled(QWidget *widget);
    QPushButton *addWidgetTab(QWidget *widget, const QString &name);
};
#endif // PRHP_H
