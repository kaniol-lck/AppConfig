#include "widgetconfig.h"
#include "qboxlayout.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>
#include <QSpinBox>
#include <QListView>
#include <QStandardItemModel>
#include <QListWidget>
#include <QMenu>


std::tuple<QWidget *, std::function<QVariant ()> > EmptyConfig::configWidget(QWidget *parentWidget, const QVariant &value)
{
    return {};
}

std::tuple<QWidget *, std::function<QString ()> > LineEditConfig::configWidget(QWidget *parentWidget, const QString &value)
{
    auto line = new QLineEdit(parentWidget);
    line->setText(value);
    applyAttr(line);
    return { line, [line]{
                return line->text();
            }};
}

std::tuple<QWidget *, std::function<int ()> > SpinBoxConfig::configWidget(QWidget *parentWidget, const int &value)
{
    auto spinBox = new QSpinBox(parentWidget);
    spinBox->setValue(value);
    applyAttr(spinBox);
    return { spinBox, [spinBox]{
                return spinBox->value();
            }};
}

std::tuple<QWidget *, std::function<double ()> > DoubleSpinBoxConfig::configWidget(QWidget *parentWidget, const double &value)
{
    auto spinBox = new QDoubleSpinBox(parentWidget);
    spinBox->setValue(value);
    applyAttr(spinBox);
    return { spinBox, [spinBox]{
                return spinBox->value();
            }};
}

std::tuple<QWidget *, std::function<bool ()> > CheckBoxConfig::configWidget(QWidget *parentWidget, const bool &value)
{
    auto checkBox = new QCheckBox(parentWidget);
    checkBox->setChecked(value);
    applyAttr(checkBox);
    return { checkBox, [checkBox]{
                return checkBox->isChecked();
            } };
}

ComboBoxConfig::ComboBoxConfig(QStringList items) :
    items_(items)
{}

std::tuple<QWidget *, std::function<int ()> > ComboBoxConfig::configWidget(QWidget *parentWidget, const int &value)
{
    auto comboBox = new QComboBox(parentWidget);
    comboBox->addItems(items_);
    comboBox->setCurrentIndex(value);
    applyAttr(comboBox);
    return { comboBox, [comboBox]{
                return comboBox->currentIndex();
            }};
}

FilePathConfig::FilePathConfig(const QString &caption, const QString &filter) :
    caption_(caption),
    filter_(filter)
{}

std::tuple<QWidget *, std::function<QString ()> > FilePathConfig::configWidget(QWidget *parentWidget, const QString &value)
{
    auto widget = new QWidget(parentWidget);
    auto layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    auto line = new QLineEdit(parentWidget);
    line->setFrame(false);
    layout->addWidget(line);
    auto button = new QToolButton(parentWidget);
    button->setText("...");
    layout->addWidget(button);
    QObject::connect(button, &QToolButton::clicked, parentWidget, [=, caption = caption_, filter = filter_]{
        auto fileName = QFileDialog::getOpenFileName(parentWidget, caption, line->text(), filter);
        if(fileName.isEmpty()) return;
        line->setText(fileName);
    });

    line->setText(value);
    applyAttr(widget);
    return { widget, [line]{
                return line->text();
            }};
}

std::tuple<QWidget *, std::function<QStringList ()> > StringListConfig::configWidget(QWidget *parentWidget, const QStringList &value)
{
    auto view = new QListView(parentWidget);
    view->setEditTriggers(QAbstractItemView::DoubleClicked |
                            QAbstractItemView::EditKeyPressed |
                            QAbstractItemView::AnyKeyPressed);
    auto model = new QStandardItemModel(view);
    view->setModel(model);
    for(auto &&str : value){
        model->appendRow(new QStandardItem(str));
    }
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(view, &QListWidget::customContextMenuRequested, parentWidget, [=](auto pos){
        auto menu = new QMenu(view);
        menu->addAction("Add", [view, model]{
            model->appendRow(new QStandardItem);
            view->edit(model->index(model->rowCount() - 1, 0));
        });
        if(auto index = view->indexAt(pos);
            index.isValid())
            menu->addAction("Remove", [model, index]{
                model->removeRow(index.row());
            });
        menu->exec(view->mapToGlobal(pos));
    });
    return { view, [model]{
                QStringList list;
                for(auto row = 0; row < model->rowCount(); row++)
                    list << model->item(row)->text();
                return list;
            }};
}
