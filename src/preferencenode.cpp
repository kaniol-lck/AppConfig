#include "preferencenode.h"

PreferenceNode::~PreferenceNode()
{
    for(auto &&i : list_)
        delete i;
}

PreferenceNode *PreferenceNode::addGroup(const QString &groupPrefix, const QString &name, bool checkable, bool defaultVal)
{
    auto node = new PreferenceNode;
    node->key_ = groupPrefix;
    node->name_ = name;
    node->checkable_ = checkable;
    node->defaultVal_ = defaultVal;
    list_ << node;
    return node;
}

PreferenceNode *PreferenceNode::addConfig(WidgetConfig *widgetConfig, const QString &key, const QString &name, const QVariant &defaultVal)
{
    auto node = new PreferenceNode;
    node->widgetConfig_ = widgetConfig;
    node->key_ = key;
    node->name_ = name;
    node->defaultVal_ = defaultVal;
    list_ << node;
    return node;
}
