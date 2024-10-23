#include "appconfig.h"

#include <QLabel>

QWidget *CommonNode::makeLayout(ApplyHandler *handler, QWidget *parentWidget, QFormLayout *layout, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key() : name_;
    QWidget *widget;
    if(showTitle){
        auto groupBox = new QGroupBox(text, parentWidget);
        widget = groupBox;
    } else
        widget = new QWidget(parentWidget);

    layout->setWidget(layout->rowCount(), QFormLayout::SpanningRole, widget);
    layout = new QFormLayout(widget);
    widget->setLayout(layout);
    for(auto &&subNode : list_)
        subNode->makeLayout(handler, widget, layout, true);
    return widget;
}

QStandardItem *CommonNode::makeTableView(ApplyHandler *handler, QTableView *view, QStandardItemModel *model, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key() : name_;
    QStandardItem *item = nullptr;
    if(showTitle){
        item = new QStandardItem();
        item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        model->appendRow(item);
        auto index = model->indexFromItem(item);
        view->setIndexWidget(index, makeLabel(view));
        view->setSpan(model->rowCount() - 1, 0, 1, 2);
    }

    for(auto subNode : list_)
        subNode->makeTableView(handler, view, model, true);
    return item;
}

QStandardItem *CommonNode::makeTreeView(ApplyHandler *handler, QTreeView *view, QStandardItemModel *model, QModelIndex parent, bool showTitle)
{
    //title - group
    auto text = name_.isEmpty()? key() : name_;
    QStandardItem *item = nullptr;
    if(showTitle){
        item = new QStandardItem(text);
        item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        if(parent.isValid())
            model->itemFromIndex(parent)->appendRow(item);
        else
            model->appendRow(item);
        auto index = model->indexFromItem(item);
        view->setIndexWidget(index, makeLabel(view));
        view->setFirstColumnSpanned(model->rowCount(parent) - 1, parent, true);
        parent = model->indexFromItem(item);
    }

    for(auto subNode : list_)
        subNode->makeTreeView(handler, view, model, parent, true);
    return item;
}

CommonNode::CommonNode(AppConfig *appConfig):
    appConfig_(appConfig)
{}

CommonNode::CommonNode(AppConfig *appConfig, const QString &key, ConfigGroup* parentGroup) :
    appConfig_(appConfig)
{
    if(parentGroup){
        parentGroup->addNode(this);
        keys_ << parentGroup->keys() << key;
    } else{
        appConfig->addNode(this);
        keys_ << key;
    }
}

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
#ifdef Q_OS_WIN
    return keys_.join('/');
#else
    return key_.join('_');
#endif
}

QStringList CommonNode::keys() const
{
    return keys_;
}

QString CommonNode::name() const
{
    return name_;
}

void CommonNode::setName(const QString &newName)
{
    name_ = newName;
}

QString CommonNode::displayName() const
{
    return name_.isEmpty()? key() : name_;
}

QString CommonNode::tooltip() const
{
    return tooltip_;
}

void CommonNode::setTooltip(const QString &newTooltip)
{
    tooltip_ = newTooltip;
}

QLabel *CommonNode::makeLabel(QWidget *parent)
{
    auto label = new QLabel(parent);
    auto &&tt = tooltip();
    if(tt.isEmpty()){
        label->setText(displayName());
    } else{
        label->setText(displayName() + R"(<span style="color:red">[?]</span>)");
        label->setToolTip(tt);
    }
    return label;
}


AppConfig::AppConfig(QObject *parent, QSettings *settings) :
    QObject(parent),
    CommonNode(this)
{
    if(!settings)
        settings = new QSettings(this);
    settings_ = std::make_shared<Settings>(settings);
}

AppConfig::AppConfig(QObject *parent, std::shared_ptr<VariantContainer> settings) :
    QObject(parent),
    CommonNode(this),
    settings_(settings)
{}

std::shared_ptr<VariantContainer> &AppConfig::settings()
{
    return settings_;
}


ConfigGroup::ConfigGroup(AppConfig *appConfig, const QString &prefix, ConfigGroup *parentGroup):
    CommonNode(appConfig, prefix, parentGroup)
{}

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
    appConfig_->settings()->set(keys_, defaultEnable_);
}

void CheckableConfigGroup::set(bool val){
    appConfig_->settings()->set(keys_, val);
}

bool CheckableConfigGroup::get() const{
    return appConfig_->settings()->get(keys_, defaultEnable_).toBool();
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
                value != appConfig_->settings()->get(keys_, defaultEnable_)){
                appConfig_->settings()->set(keys_, value);
                emit appConfig_->configChanged(key());
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
                value != appConfig_->settings()->get(keys_, defaultEnable_)){
                appConfig_->settings()->set(keys_, value);
                emit appConfig_->configChanged(key());
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
                value != appConfig_->settings()->get(keys_, defaultEnable_)){
                appConfig_->settings()->set(keys_, value);
                emit appConfig_->configChanged(key());
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
