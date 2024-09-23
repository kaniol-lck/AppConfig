#ifndef WIDGETCONFIG_H
#define WIDGETCONFIG_H

#include <QVariant>

class QWidget;

class WidgetConfig
{
public:
    WidgetConfig();
    virtual ~WidgetConfig() = default;

    virtual std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) = 0;

    void applyAttr(QWidget *widget);
    void setAttr(const std::function<void (QWidget *)> &newAttrSetter);

private:
    std::function<void (QWidget*)> attrSetter_;
};

class LineEditConfig : public WidgetConfig
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class SpinBoxConfig : public WidgetConfig
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class CheckBoxConfig : public WidgetConfig
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;
};

class ComboBoxConfig : public WidgetConfig
{
public:
    ComboBoxConfig(QStringList items);
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;

private:
    QStringList items_;
};

class FilePathConfig : public WidgetConfig
{
public:
    FilePathConfig(const QString &caption = "", const QString &filter = "");
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override;

private:
    QString caption_;
    QString filter_;
};

#endif // WIDGETCONFIG_H
