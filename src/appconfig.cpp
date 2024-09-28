#include "appconfig.h"

QWidget *CommonNode::makeLayout(QWidget *parentWidget, QFormLayout *layout, bool showTitle)
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
        subNode->makeLayout(groupBox, layout, true);
    return groupBox;
}

CommonNode::CommonNode(AppConfig *appConfig) :
    appConfig_(appConfig)
{}

void CommonNode::addNode(CommonNode *child){
    list_ << child;
}

void CommonNode::makeLayout(QWidget *widget, bool showTitle)
{
    auto layout = new QFormLayout(widget);
    widget->setLayout(layout);
    makeLayout(widget, layout, showTitle);
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
#ifdef ALLOW_CONFIG_GROUP_EMBED
        key_ = parentGroup->key_ + "/" + prefix;
#else
        key_ = parentGroup->key_ + "_" + prefix;
#endif

    } else{
        appConfig->addNode(this);
        key_ = prefix;
    }
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

QWidget *CheckableConfigGroup::makeLayout(QWidget *parentWidget, QFormLayout *layout, bool showTitle)
{
    auto widget = ConfigGroup::makeLayout(parentWidget, layout, showTitle);
    if(showTitle){
        auto groupBox = qobject_cast<QGroupBox*>(widget);
        groupBox->setCheckable(true);
        groupBox->setChecked(get());
        QObject::connect(appConfig_, &AppConfig::applyed, [=, this]{
            if(auto value = groupBox->isChecked();
                value != appConfig_->settings()->value(key_, defaultEnable_)){
                appConfig_->settings()->setValue(key_, value);
                emit appConfig_->configChanged(key_);
            }
        });
    }
    return widget;
}
