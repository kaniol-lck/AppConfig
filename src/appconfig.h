#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "configwrapperwidget.h"
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

class VariantContainer
{
public:
    virtual QVariant get(const QStringList &keys, const QVariant &defaultVal) = 0;
    virtual void set(const QStringList &keys, const QVariant &value) = 0;
};

class Settings : public VariantContainer
{
public:
    Settings(QSettings *settings) :
        settings_(settings)
    {}
    virtual QVariant get(const QStringList &keys, const QVariant &defaultVal){
        return settings_->value(key(keys), defaultVal);
    }
    virtual void set(const QStringList &keys, const QVariant &value){
        settings_->setValue(key(keys), value);
    }
private:
    QSettings *settings_;
    static QString key(const QStringList& keys)
    {
#ifdef Q_OS_WIN
        return keys.join('/');
#else
        return keys.join('_');
#endif
    }
};

class MapSetting : public VariantContainer
{
public:
    virtual QVariant get(const QStringList &keys, const QVariant &defaultVal)override{
        return map_.value(keys.join("/"), defaultVal);
    }
    virtual void set(const QStringList &keys, const QVariant &value){
        map_.insert(keys.join("/"), value);
    }

    QVariantMap variantMap() const
    {
        return map_;
    }

private:
    QVariantMap map_;
};

class AppConfig;

// Empty tool class
class ApplyHandler : public QObject
{
    Q_OBJECT
public:
    ApplyHandler(QObject *parent):
        QObject(parent){}
signals:
    void applyed();
};

class ConfigGroup;
class CommonNode
{
public:
    CommonNode(AppConfig *appConfig);
    CommonNode(AppConfig *appConfig, const QString &key, ConfigGroup* parentGroup = nullptr);

    void addNode(CommonNode* child);
    ApplyHandler *makeLayout(QWidget *widget, bool showTitle = false);
    ApplyHandler *makeLayout(ApplyHandler *handler, QWidget *widget, bool showTitle = false);

    ApplyHandler *makeTableView(QTableView *view, bool showTitle = false);
    ApplyHandler *makeTableView(ApplyHandler *handler, QTableView *view, bool showTitle = false);

    ApplyHandler *makeTreeView(QTreeView *view, bool showTitle = false);
    ApplyHandler *makeTreeView(ApplyHandler *handler, QTreeView *view, bool showTitle = false);

    QString key() const;
    QStringList keys() const;

    QString name() const;
    void setName(const QString &newName);

    QString displayName() const;

protected:
    QString name_;
    QStringList keys_;
    QList<CommonNode*> list_;
    AppConfig *appConfig_;

protected:
    virtual QWidget *makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle);
    virtual QStandardItem *makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle = false);
    virtual QStandardItem *makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle = false);
};

class AppConfig : public QObject, public CommonNode
{
    Q_OBJECT
public:
    AppConfig(QObject *parent = nullptr, QSettings *settings = nullptr);
    AppConfig(QObject *parent, std::shared_ptr<VariantContainer> settings);

    std::shared_ptr<VariantContainer> &settings();

signals:
    void configChanged(const QString &key);

private:
    std::shared_ptr<VariantContainer> settings_;
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
    virtual QWidget *makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle) override;
    virtual QStandardItem *makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle = false) override;
    virtual QStandardItem *makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle = false) override;
};

template<typename T>
struct defaultConfigWidget{
    using Type = WidgetWrapper<QWidget, T>;
};

template<>
struct defaultConfigWidget<QString>{
    using Type = LineEditWrapper;
};
template<>
struct defaultConfigWidget<int>{
    using Type = SpinBoxWrapper;
};
template<>
struct defaultConfigWidget<double>{
    using Type = DoubleSpinBoxWrapper;
};
template<>
struct defaultConfigWidget<bool>{
    using Type = CheckBoxWrapper;
};
template<>
struct defaultConfigWidget<QStringList>{
    using Type = StringListWrapper;
};


template<typename T>
class ConfigItem : public CommonNode
{
public:
    ConfigItem(AppConfig *appConfig, const QString &key, const T &defaultVal, ConfigGroup* parentGroup = nullptr):
        CommonNode(appConfig, key, parentGroup),
        defaultVal_(defaultVal)
    {}

    ConfigListener *listener(){
        auto l = new ConfigListener(appConfig_);
        l->listenConfigItem(*this);
        return l;
    }

    void reset(){
        appConfig_->settings()->set(keys_, defaultVal_);
    }

    void set(const T &val){
        appConfig_->settings()->set(keys_, val);
    }

    T get() const{
        return appConfig_->settings()->get(keys_, defaultVal_).template value<T>();
    }

    T defaultVal() const{
        return defaultVal_;
    }

    template<typename WrapperT, typename... Args>
    auto setGenerator(Args&&... args)
    {
        auto generator = std::make_shared<WrapperGenerator<WrapperT, T>>(std::forward<Args>(args)...);
        generator_ = generator;
        return generator;
    }

    void setGeneratorPtr(const std::shared_ptr<Generator<T>> &newGenerator)
    {
        generator_ = newGenerator;
    }

private:
    T defaultVal_;
    std::shared_ptr<Generator<T>> generator_;

private:
    using DefaultConfigWidgetType = defaultConfigWidget<T>::Type;
    constexpr static bool isEmpty = std::is_same_v<DefaultConfigWidgetType, WidgetWrapper<QWidget, T>>;

protected:
    void onConfigChanged()
    {
        qDebug() << "changed";
    };

    QWidget *applyWidgetConfig(ApplyHandler *handler, QWidget *parentWidget)
    {
        std::shared_ptr<Wrapper<T>> wrapper;
        if(!generator_)
            wrapper = std::make_shared<DefaultConfigWidgetType>();
        else
            wrapper = generator_->genWrapper();
        wrapper->genWidget(parentWidget);
        if(generator_) generator_->applyAttrSetter(wrapper->widget());
        wrapper->set(get());

        QObject::connect(handler, &ApplyHandler::applyed, [=, this]{
            if(auto value = wrapper->get();
                value != get()){
                set(value);
                emit appConfig_->configChanged(key());
            }
        });
        return wrapper->widget();
    }

    QWidget *makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle[[maybe_unused]]) override
    {
        if(isEmpty){
            auto label = new QLabel(QString("WARNING: %1 has no default widget to config")
                                        .arg(key()), parentWidget);
            label->setStyleSheet("color: red;");
            layout->setWidget(layout->rowCount(), QFormLayout::SpanningRole, label);
            return label;
        }

        auto widget = applyWidgetConfig(handler, parentWidget);
        layout->addRow(displayName(), widget);
        return widget;
    }
    QStandardItem *makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle[[maybe_unused]]) override
    {
        auto item1 = new QStandardItem(displayName());
        auto item2 = new QStandardItem();
        model->appendRow({ item1, item2 });

        auto index = model->indexFromItem(item2);
        auto widget = applyWidgetConfig(handler, view);
        view->setIndexWidget(index, widget);
        return item2;
    };
    virtual QStandardItem *makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle[[maybe_unused]]) override
    {
        auto item1 = new QStandardItem(displayName());
        auto item2 = new QStandardItem();
        if(parent.isValid())
            model->itemFromIndex(parent)->appendRow({ item1, item2 });
        else
            model->appendRow({ item1, item2 });

        auto index = model->indexFromItem(item2);
        auto widget = applyWidgetConfig(handler, view);
        view->setIndexWidget(index, widget);
        return item2;
    }
};


#endif // APPCONFIG_H
