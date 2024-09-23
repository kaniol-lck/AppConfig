#ifndef PREFERENCENODE_H
#define PREFERENCENODE_H

#include "widgetconfig.h"
#include <QList>

class PreferenceNode
{
    friend class PreferencesHelper;
public:
    // root node
    PreferenceNode() = default;
    ~PreferenceNode();

    PreferenceNode *addGroup(const QString &groupPrefix, const QString &name, bool checkable = false, bool defaultVal = true);
    PreferenceNode *addConfig(WidgetConfig *widgetConfig, const QString &key, const QString &name, const QVariant &defaultVal = {});

private:
    QList<PreferenceNode*> list_;
    WidgetConfig *widgetConfig_ = nullptr;
    QString key_;
    QString name_;
    QVariant defaultVal_;
    bool checkable_;
};

#endif // PREFERENCENODE_H
