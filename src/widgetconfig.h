#ifndef WIDGETCONFIG_H
#define WIDGETCONFIG_H

#include <QVariant>

class QWidget;

template<typename T>
class WidgetConfig
{
public:
    WidgetConfig() {}
    virtual ~WidgetConfig() = default;

    virtual std::tuple<QWidget*, std::function<T ()>> configWidget(QWidget *parentWidget, const T &value) = 0;

    void applyAttr(QWidget *widget){
        if(attrSetter_) attrSetter_(widget);
    }
    void setAttr(const std::function<void (QWidget *)> &newAttrSetter){
        attrSetter_ = newAttrSetter;
    }

private:
    std::function<void (QWidget*)> attrSetter_;
};

class EmptyConfig : public WidgetConfig<QVariant>
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value)override;
};

class LineEditConfig : public WidgetConfig<QString>
{
public:
    std::tuple<QWidget*, std::function<QString()>> configWidget(QWidget *parentWidget, const QString &value)override;
};

class SpinBoxConfig : public WidgetConfig<int>
{
public:
    std::tuple<QWidget*, std::function<int()>> configWidget(QWidget *parentWidget, const int &value)override;
};

class DoubleSpinBoxConfig : public WidgetConfig<double>
{
public:
    std::tuple<QWidget*, std::function<double()>> configWidget(QWidget *parentWidget, const double &value)override;
};

class CheckBoxConfig : public WidgetConfig<bool>
{
public:
    std::tuple<QWidget*, std::function<bool()>> configWidget(QWidget *parentWidget, const bool &value)override;
};

class ComboBoxConfig : public WidgetConfig<int>
{
public:
    ComboBoxConfig(QStringList items);
    std::tuple<QWidget*, std::function<int()>> configWidget(QWidget *parentWidget, const int &value)override;

private:
    QStringList items_;
};

class FilePathConfig : public WidgetConfig<QString>
{
public:
    FilePathConfig(const QString &caption = "", const QString &filter = "");
    std::tuple<QWidget*, std::function<QString()>> configWidget(QWidget *parentWidget, const QString &value)override;

private:
    QString caption_;
    QString filter_;
};

class StringListConfig : public WidgetConfig<QStringList>
{
public:
    std::tuple<QWidget*, std::function<QStringList()>> configWidget(QWidget *parentWidget, const QStringList &value)override;

};

#endif // WIDGETCONFIG_H
