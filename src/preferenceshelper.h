#ifndef PREFERENCESHELPER_H
#define PREFERENCESHELPER_H

#include "widgetconfig.h"
#include "preferencenode.h"

#include <QObject>
#include <QTableView>
#include <QStandardItemModel>
#include <QFormLayout>
#include <QSettings>

class QTreeView;
class PreferencesHelper : public QObject
{
    Q_OBJECT
public:
    explicit PreferencesHelper(QObject *parent = nullptr, QSettings *settings = nullptr);

    void makeView(PreferenceNode *node, QTableView *view, bool showTitle = false);
    void makeView(PreferenceNode *node, QTreeView *view, bool showTitle = false);
    void makeLayout(PreferenceNode *node, QWidget *widget, bool showTitle = false);

    void setSettings(QSettings *newSettings);

public slots:
    void applyChanges();

signals:
    void applyed();

private:
    QSettings *settings_;

    void makeView(PreferenceNode *node, QTableView *view, QStandardItemModel *model, const QString &prefix, bool showTitle);
    void makeTreeView(PreferenceNode *node, QTreeView *view, QStandardItemModel *model, QModelIndex index, const QString &prefix, bool showTitle);
    void makeLayout(PreferenceNode *node, QWidget *parentWidget, QFormLayout *layout, const QString &prefix, bool showTitle);
};

#endif // PREFERENCESHELPER_H
