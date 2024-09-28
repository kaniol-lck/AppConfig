#include "preferenceshelper.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QSettings>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>

PreferencesHelper::PreferencesHelper(QObject *parent, QSettings *settings)
    : QObject{parent}
    , settings_(settings)
{
    if(!settings_) settings_ = new QSettings(this);
}

void PreferencesHelper::makeView(PreferenceNode *node, QTableView *view, bool showTitle)
{
    auto model = new QStandardItemModel(this);
    view->setModel(model);
    makeTableView(node, view, model, "", "", showTitle);

    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PreferencesHelper::makeTableView(PreferenceNode *node, QTableView *view, QStandardItemModel *model, const QString &prefix, const QString &namePrefix, bool showTitle)
{
    auto key = node->key_;
    if(!prefix.isEmpty())
        key.prepend(prefix + "/");
    auto name = node->name_;
    if(!namePrefix.isEmpty())
        name.prepend(namePrefix + " / ");
    // qDebug() << key;
    if(!node->widgetConfig_){
        //title - group
        if(showTitle){
            auto item = new QStandardItem(name);
            if(node->checkable_){
                item->setCheckable(true);
                if(settings_->value(key, node->defaultVal_).toBool())
                    item->setCheckState(Qt::Checked);
                connect(this, &PreferencesHelper::applyed, this, [=, this]{
                    settings_->setValue(key, item->checkState() == Qt::Checked);
                });
            }
            item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
            model->appendRow(item);
            view->setSpan(model->rowCount() - 1, 0, 1, 2);
        }

        for(auto subNode : node->list_)
            makeTableView(subNode, view, model, key, name, true);
    } else{
        auto item1 = new QStandardItem(node->name_);
        auto item2 = new QStandardItem();
        model->appendRow({ item1, item2 });

        auto index = model->indexFromItem(item2);
        auto [widget, valueGetter] = node->widgetConfig_->configWidget(view, settings_->value(key));
        view->setIndexWidget(index, widget);
        connect(this, &PreferencesHelper::applyed, this, [=, this]{
            settings_->setValue(key, valueGetter());
        });
    }
}

void PreferencesHelper::makeView(PreferenceNode *node, QTreeView *view, bool showTitle)
{
    auto model = new QStandardItemModel(this);
    model->setColumnCount(2);
    view->setModel(model);
    makeTreeView(node, view, model, {}, "", showTitle);

    view->header()->setVisible(false);
    view->header()->setSectionResizeMode(QHeaderView::Stretch);
    view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->expandAll();
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PreferencesHelper::makeTreeView(PreferenceNode *node, QTreeView *view, QStandardItemModel *model, QModelIndex index, const QString &prefix, bool showTitle)
{
    auto key = node->key_;
    if(!prefix.isEmpty())
        key.prepend(prefix + "/");
    // qDebug() << key;
    if(!node->widgetConfig_){
        //title - group
        if(showTitle){
            auto item = new QStandardItem(node->name_);
            if(node->checkable_){
                item->setCheckable(true);
                if(settings_->value(key, node->defaultVal_).toBool())
                    item->setCheckState(Qt::Checked);
                connect(this, &PreferencesHelper::applyed, this, [=, this]{
                    settings_->setValue(key, item->checkState() == Qt::Checked);
                });
            }
            item->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
            if(index.isValid())
                model->itemFromIndex(index)->appendRow(item);
            else
                model->appendRow(item);
            view->setFirstColumnSpanned(model->rowCount(index) - 1, index, true);
            index = model->indexFromItem(item);
        }

        for(auto subNode : node->list_)
            makeTreeView(subNode, view, model, index, key, true);
    } else{
        auto item1 = new QStandardItem(node->name_);
        auto item2 = new QStandardItem();
        model->itemFromIndex(index)->appendRow({ item1, item2 });

        auto index = model->indexFromItem(item2);
        auto [widget, valueGetter] = node->widgetConfig_->configWidget(view, settings_->value(key));
        view->setIndexWidget(index, widget);
        connect(this, &PreferencesHelper::applyed, this, [=, this]{
            settings_->setValue(key, valueGetter());
        });
    }
}

void PreferencesHelper::makeLayout(PreferenceNode *node, QWidget *widget, bool showTitle)
{
    auto layout = new QFormLayout(widget);
    widget->setLayout(layout);
    makeLayout(node, widget, layout, "", showTitle);
}

void PreferencesHelper::makeLayout(PreferenceNode *node, QWidget *parentWidget, QFormLayout *layout, const QString &prefix, bool showTitle)
{
    auto key = node->key_;
    if(!prefix.isEmpty())
        key.prepend(prefix + "/");
    // qDebug() << key;
    if(!node->widgetConfig_){
        //title - group
        if(showTitle){
            auto text = node->name_;
            auto groupBox = new QGroupBox(text, parentWidget);
            if(node->checkable_){
                groupBox->setCheckable(true);
                groupBox->setChecked(settings_->value(key, node->defaultVal_).toBool());
                connect(this, &PreferencesHelper::applyed, this, [=, this]{
                    settings_->setValue(key, groupBox->isChecked());
                });
            }

            layout->setWidget(layout->rowCount(), QFormLayout::SpanningRole, groupBox);
            layout = new QFormLayout(groupBox);
            groupBox->setLayout(layout);
            for(auto subNode : node->list_)
                makeLayout(subNode, groupBox, layout, key, true);

            //disable widgets
            connect(groupBox, &QGroupBox::clicked, this, [=](bool checked){
                for(auto widget : groupBox->findChildren<QWidget*>())
                    widget->setEnabled(checked);
            });
        } else{
            for(auto subNode : node->list_)
                makeLayout(subNode, parentWidget, layout, key, true);
        }
    } else{
        auto text = node->name_;

        auto [widget, valueGetter] = node->widgetConfig_->configWidget(parentWidget, settings_->value(key));
        layout->addRow(text, widget);
        connect(this, &PreferencesHelper::applyed, this, [=, this]{
            settings_->setValue(key, valueGetter());
        });
    }
}

void PreferencesHelper::applyChanges()
{
    emit applyed();
}

void PreferencesHelper::setSettings(QSettings *newSettings)
{
    if(settings_) settings_->deleteLater();
    settings_ = newSettings;
}
