#ifndef PRHP_H
#define PRHP_H

#include <QMainWindow>

class PreferencesHelper;

class PrHp : public QMainWindow
{
    Q_OBJECT

public:
    PrHp(QWidget *parent = nullptr);
    ~PrHp() = default;

private:
    PreferencesHelper *helper1_;
    PreferencesHelper *helper2_;
    PreferencesHelper *helper3_;
    PreferencesHelper *helper4_;
};
#endif // PRHP_H
