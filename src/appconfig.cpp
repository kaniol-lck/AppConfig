#include "appconfig.h"

QWidget *CommonNode::makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key_ : name_;
    QWidget *groupBox;
    if(showTitle)
        groupBox = new QGroupBox(text, parentWidget);
    else
        groupBox = new QWidget(parentWidget);

    layout->setWidget(layout->rowCount(), QFormLayout::SpanningRole, groupBox);
    layout = new QFormLayout(groupBox);
    groupBox->setLayout(layout);
    for(auto &&subNode : list_)
        subNode->makeLayout(handler, groupBox, layout, true);
    return groupBox;
}

QStandardItem *CommonNode::makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key_ : name_;
    QStandardItem *item = nullptr;
    if(showTitle){
        item = new QStandardItem(text);
        item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        model->appendRow(item);
        view->setSpan(model->rowCount() - 1, 0, 1, 2);
    }

    for(auto subNode : list_)
        subNode->makeTableView(handler, view, model, true);
    return item;
}

QStandardItem *CommonNode::makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key_ : name_;
    QStandardItem *item = nullptr;
    if(showTitle){
        item = new QStandardItem(text);
        item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        if(parent.isValid())
            model->itemFromIndex(parent)->appendRow(item);
        else
            model->appendRow(item);
        view->setFirstColumnSpanned(model->rowCount(parent) - 1, parent, true);
        parent = model->indexFromItem(item);
    }

    for(auto subNode : list_)
        subNode->makeTreeView(handler, view, model, parent, true);
    return item;
}

CommonNode::CommonNode(AppConfig *appConfig) :
    appConfig_(appConfig)
{}

void CommonNode::addNode(CommonNode *child){
    list_ << child;
}

ApplyHandler *CommonNode::makeLayout(QWidget *widget, bool showTitle)
{
    auto handler = new ApplyHandler(appConfig_);
    return makeLayout(handler, widget, showTitle);
}

ApplyHandler *CommonNode::makeLayout(ApplyHandler *handler, QWidget *widget, bool showTitle)
{
    auto layout = new QFormLayout(widget);
    widget->setLayout(layout);
    makeLayout(handler, widget, layout, showTitle);
    return handler;
}

ApplyHandler *CommonNode::makeTableView(QTableView *view, bool showTitle)
{
    auto handler = new ApplyHandler(appConfig_);
    return makeTableView(handler, view, showTitle);
}

ApplyHandler *CommonNode::makeTableView(ApplyHandler *handler, QTableView *view, bool showTitle)
{
    auto model = new QStandardItemModel(view);
    view->setModel(model);
    makeTableView(handler, view, model, showTitle);

    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return handler;
}

ApplyHandler *CommonNode::makeTreeView(QTreeView *view, bool showTitle)
{
    auto handler = new ApplyHandler(appConfig_);
    return makeTreeView(handler, view, showTitle);
}

ApplyHandler *CommonNode::makeTreeView(ApplyHandler *handler, QTreeView *view, bool showTitle)
{
    auto model = new QStandardItemModel(view);
    view->setModel(model);
    makeTreeView(handler, view, model, {}, showTitle);

    view->header()->setVisible(false);
    view->header()->setSectionResizeMode(QHeaderView::Stretch);
    view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->expandAll();
    return handler;
}

QString CommonNode::key() const
{
    return key_;
}

QString CommonNode::name() const
{
    return name_;
}

void CommonNode::setName(const QString &newName)
{
    name_ = newName;
}


AppConfig::AppConfig(QObject *parent, QSettings *settings) :
    QObject(parent),
    CommonNode(this),
    settings_(settings)
{
    if(!settings_)
        settings_ = new QSettings(this);
}

QSettings *AppConfig::settings()
{
    return settings_;
}


ConfigGroup::ConfigGroup(AppConfig *appConfig, const QString &prefix, ConfigGroup *parentGroup):
    CommonNode(appConfig){
    if(parentGroup){
        parentGroup->addNode(this);
#ifdef Q_OS_WIN
        if(appConfig_->settings()->format() == QSettings::NativeFormat)
            key_ = parentGroup->key_ + "/" + prefix;
        else
            key_ = parentGroup->key_ + "_" + prefix;
#else
        key_ = parentGroup->key_ + "_" + prefix;
#endif

    } else{
        appConfig->addNode(this);
        key_ = prefix;
    }
}

ConfigListener *ConfigGroup::listener()
{
    auto l = new ConfigListener(appConfig_);
    l->listenConfigGroup(*this);
    return l;
}


CheckableConfigGroup::CheckableConfigGroup(AppConfig *appConfig, const QString &prefix, bool defaultEnable, ConfigGroup *parentGroup) :
    ConfigGroup(appConfig, prefix, parentGroup),
    defaultEnable_(defaultEnable)
{}

void CheckableConfigGroup::reset(){
    appConfig_->settings()->setValue(key_, defaultEnable_);
}

void CheckableConfigGroup::set(bool val){
    appConfig_->settings()->setValue(key_, val);
}

bool CheckableConfigGroup::get() const{
    return appConfig_->settings()->value(key_, defaultEnable_).toBool();
}

bool CheckableConfigGroup::defaultVal() const
{
    return defaultEnable_;
}

ConfigListener *CheckableConfigGroup::checkListener()
{
    auto l = new ConfigListener(appConfig_);
    l->listenConfigItem(*this);
    return l;
}

QWidget *CheckableConfigGroup::makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle)
{
    auto widget = ConfigGroup::makeLayout(handler, parentWidget, layout, showTitle);
    if(showTitle){
        auto groupBox = qobject_cast<QGroupBox*>(widget);
        groupBox->setCheckable(true);
        groupBox->setChecked(get());
        QObject::connect(handler, &ApplyHandler::applyed, [=, this]{
            if(auto value = groupBox->isChecked();
                value != appConfig_->settings()->value(key_, defaultEnable_)){
                appConfig_->settings()->setValue(key_, value);
                emit appConfig_->configChanged(key_);
            }
        });
    }
    return widget;
}

QStandardItem *CheckableConfigGroup::makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle)
{
    auto item = ConfigGroup::makeTableView(handler, view, model, showTitle);
    if(showTitle){
        item->setCheckable(true);
        item->setCheckState(get()? Qt::Checked : Qt::Unchecked);
        QObject::connect(handler, &ApplyHandler::applyed, [=, this]{
            if(auto value = item->checkState() == Qt::Checked;
                value != appConfig_->settings()->value(key_, defaultEnable_)){
                appConfig_->settings()->setValue(key_, value);
                emit appConfig_->configChanged(key_);
            }
        });
    }
    return item;
}

QStandardItem *CheckableConfigGroup::makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle)
{
    auto item = ConfigGroup::makeTreeView(handler, view, model, parent, showTitle);
    if(showTitle){
        item->setCheckable(true);
        item->setCheckState(get()? Qt::Checked : Qt::Unchecked);
        QObject::connect(handler, &ApplyHandler::applyed, [=, this]{
            if(auto value = item->checkState() == Qt::Checked;
                value != appConfig_->settings()->value(key_, defaultEnable_)){
                appConfig_->settings()->setValue(key_, value);
                emit appConfig_->configChanged(key_);
            }
        });
    }
    return item;
}


ConfigListener::ConfigListener(AppConfig *appConfig)
{
    connect(appConfig, &AppConfig::configChanged, this, &ConfigListener::processListen);
}

void ConfigListener::listenConfigGroup(const ConfigGroup &group)
{
    listenedGroupKeys_ << group.key();
}

void ConfigListener::listenConfigItem(const CommonNode &item)
{
    listenedItemKeys_ << item.key();
}
void ConfigListener::processListen(const QString &key)
{
    if(listenedItemKeys_.contains(key)){
        emit configChanged();
        return;
    }
    for(auto &&groupKey :listenedGroupKeys_){
        if(key.startsWith(groupKey)){
            emit configChanged();
            return;
        }
    }
}

