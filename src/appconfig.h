#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "widgetconfig.h"
#include <QFormLayout>
#include <QSettings>
#include <QVariant>
#include <QWidget>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QSettings>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <type_traits>
#include <QLabel>

#define OVERRIDE(_1, _2, _3, _4, NAME, ...) NAME

#define CONCAT(a1, a2) a1##_##a2

#define ADD_GROUP1(group) \
ConfigGroup group = \
    ConfigGroup(this, #group);
#define ADD_GROUP2(group, parent) \
ConfigGroup CONCAT(parent, group) = \
    ConfigGroup(this, #group, &parent);
#define ADD_GROUP(...) OVERRIDE(__VA_ARGS__, _, _, ADD_GROUP2, ADD_GROUP1)(__VA_ARGS__)

#define ADD_CHECKABLE_GROUP1(group, defaultEnable) \
CheckableConfigGroup group = \
    CheckableConfigGroup(this, #group, defaultEnable);
#define ADD_CHECKABLE_GROUP2(group, defaultEnable, parent) \
CheckableConfigGroup CONCAT(parent, group) = \
    CheckableConfigGroup(this, #group, defaultEnable, &parent);
#define ADD_CHECKABLE_GROUP(...) OVERRIDE(__VA_ARGS__, _, ADD_CHECKABLE_GROUP2, ADD_CHECKABLE_GROUP1, _)(__VA_ARGS__)

#define ADD_CONFIG1(Type, config, defaultVal) \
ConfigItem<Type> config = \
    ConfigItem<Type>(this, #config, defaultVal);
#define ADD_CONFIG2(Type, config, defaultVal, group) \
ConfigItem<Type> CONCAT(group, config) = \
    ConfigItem<Type>(this, #config, defaultVal, &group);
#define ADD_CONFIG(...) OVERRIDE(__VA_ARGS__, ADD_CONFIG2, ADD_CONFIG1, _, _)(__VA_ARGS__)

class AppConfig;

class CommonNode
{
public:
    CommonNode(AppConfig *appConfig);

    void addNode(CommonNode* child);
    void makeLayout(QWidget *widget, bool showTitle = false);

    QString key() const;

    QString name() const;
    void setName(const QString &newName);

protected:
    QString name_;
    QString key_;
    QList<CommonNode*> list_;
    AppConfig *appConfig_;
protected:
    virtual QWidget *makeLayout(QWidget *parentWidget, QFormLayout *layout, bool showTitle);;
};

class AppConfig : public QObject, public CommonNode
{
    Q_OBJECT
public:
    AppConfig(QObject *parent = nullptr, QSettings *settings = nullptr);
    QSettings *settings();

signals:
    void applyed();
    void configChanged(const QString &key);

private:
    QSettings *settings_;
};

class ConfigGroup;
class ConfigListener : public QObject
{
    Q_OBJECT
public:
    ConfigListener(AppConfig *appConfig);

    void listenConfigGroup(const ConfigGroup &group);
    void listenConfigItem(const CommonNode &item);

signals:
    void configChanged();

private:
    QStringList listenedItemKeys_;
    QStringList listenedGroupKeys_;

    void processListen(const QString &key);
};

class ConfigGroup : public CommonNode
{
public:
    ConfigGroup(AppConfig *appConfig, const QString &prefix, ConfigGroup *parentGroup = nullptr);

    ConfigListener *listener();
private:
};

class CheckableConfigGroup : public ConfigGroup
{
public:
    CheckableConfigGroup(AppConfig *appConfig, const QString &prefix, bool defaultEnable, ConfigGroup *parentGroup = nullptr);

    void reset();
    void set(bool val);
    bool get() const;
    bool defaultVal() const;

    ConfigListener *checkListener();

protected:
    bool defaultEnable_;
    virtual QWidget *makeLayout(QWidget *parentWidget, QFormLayout *layout, bool showTitle);;
};

template<typename T>
struct defaultConfigWidget{
    using Type = EmptyConfig;
};

template<>
struct defaultConfigWidget<QString>{
    using Type = LineEditConfig;
};

template<>
struct defaultConfigWidget<bool>{
    using Type = CheckBoxConfig;
};

template<>
struct defaultConfigWidget<int>{
    using Type = SpinBoxConfig;
};

template<>
struct defaultConfigWidget<double>{
    using Type = DoubleSpinBoxConfig;
};

template<typename T>
class ConfigItem : public CommonNode
{
public:
    ConfigItem(AppConfig *appConfig, const QString &key, const T &defaultVal, ConfigGroup* parentGroup = nullptr):
        CommonNode(appConfig),
        defaultVal_(defaultVal),
        settings_(appConfig->settings())
    {    if(parentGroup){
            parentGroup->addNode(this);
            key_ = parentGroup->key() + "/" + key;
        } else{
            appConfig->addNode(this);
            key_ = key;
        }
    }

    ConfigListener *listener()
    {
        auto l = new ConfigListener(appConfig_);
        l->listenConfigItem(*this);
        return l;
    }

    void reset(){
        settings_->setValue(key_, defaultVal_);
    }

    void set(const T &val){
        settings_->setValue(key_, val);
    }

    T get() const{
        return settings_->value(key_, defaultVal_).template value<T>();
    }

    T defaultVal() const
    {
        return defaultVal_;
    }

    void setWidgetConfig(std::shared_ptr<WidgetConfig> newWidgetConfig)
    {
        widgetConfig_ = newWidgetConfig;
    }
private:
    T defaultVal_;
    QSettings *settings_;
    std::shared_ptr<WidgetConfig> widgetConfig_;

private:
    using DefaultConfigWidgetType = defaultConfigWidget<T>::Type;
    constexpr static bool isEmpty = std::is_same_v<DefaultConfigWidgetType, EmptyConfig>;

protected:
    QWidget *makeLayout(QWidget *parentWidget, QFormLayout *layout, bool showTitle[[maybe_unused]]) override
    {
        auto text = name_.isEmpty()? key_ : name_;

        if(isEmpty){
            auto label = new QLabel(QString("WARNING: %1 has no default widget to config")
                                        .arg(key_), parentWidget);
            label->setStyleSheet("color: red;");
            layout->setWidget(layout->rowCount(), QFormLayout::SpanningRole, label);
            return label;
        }
        QWidget *widget;
        std::function<QVariant ()> valueGetter;
        if(widgetConfig_){
            std::tie(widget, valueGetter) = widgetConfig_->configWidget(parentWidget, settings_->value(key_, defaultVal_));
        } else{
            DefaultConfigWidgetType widgetConfig;
            std::tie(widget, valueGetter) = widgetConfig.configWidget(parentWidget, settings_->value(key_, defaultVal_));
        }

        layout->addRow(text, widget);
        QObject::connect(appConfig_, &AppConfig::applyed, [=, this]{
            if(auto value = valueGetter();
                value != settings_->value(key_, defaultVal_)){
                settings_->setValue(key_, value);
                emit appConfig_->configChanged(key_);
            }
        });
        return widget;
    }
};


#endif // APPCONFIG_H
