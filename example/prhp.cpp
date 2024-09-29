#include "prhp.h"
#include "MyConfig.hpp"

#include <QTableView>
#include <QTreeView>
#include <QScrollArea>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSpinBox>


PrHp::PrHp(QWidget *parent)
    : QMainWindow(parent)
    , tabWidget_(new QTabWidget(this))
{
    setCentralWidget(tabWidget_);

    auto config = new MyConfig(this);

    // listener example
    connect(config, &MyConfig::configChanged, this, [](auto key){
        qDebug() << key << "changed";
    });
    connect(config->common_ui_toolBar.listener(), &ConfigListener::configChanged, this, []{
        qDebug() << "group changed!!!!";
    });
    connect(config->common_ui_toolBar.checkListener(), &ConfigListener::configChanged, this, []{
        qDebug() << "group check state changed!!!!";
    });
    auto listener = new ConfigListener(config);
    listener->listenConfigItem(config->common_set1);
    listener->listenConfigItem(config->common_set2);
    connect(listener, &ConfigListener::configChanged, this, []{
        qDebug() << "common_set1 or common_set2 changed!!!!";
    });

    // UI generation example
    // Group Box Style
    {
        auto widget = new QWidget;
        auto handler = config->makeLayout(widget);
        connect(addWidgetTab(scrolled(widget), "Group Box Style"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
    }

    // Table View Style
    {
        auto view = new QTableView;
        auto handler = config->makeTableView(view);
        connect(addWidgetTab(view, "QTable View Style"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
    }

    // Tree View Style
    {
        auto view = new QTreeView;
        auto handler = config->makeTreeView(view);
        connect(addWidgetTab(view, "QTree View Style"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
    }

    // Combined Style
    {
        auto combined = new QWidget;
        auto layout = new QHBoxLayout;
        combined->setLayout(layout);
        auto view = new QListWidget(this);
        auto stackWidget = new QStackedWidget(this);
        layout->addWidget(view, 1);
        layout->addWidget(stackWidget, 3);
        view->addItems({
            tr("Common"),
            tr("Download"),
            tr("Account"),
        });
        auto widget1 = new QWidget(this);
        auto widget2 = new QWidget(this);
        auto widget3 = new QWidget(this);
        stackWidget->addWidget(scrolled(widget1));
        stackWidget->addWidget(scrolled(widget2));
        stackWidget->addWidget(scrolled(widget3));
        connect(view, &QListWidget::currentRowChanged, stackWidget, &QStackedWidget::setCurrentIndex);

        auto handler = config->common.makeLayout(widget1);
        config->download.makeLayout(handler, widget2);
        config->account.makeLayout(handler, widget3);
        connect(addWidgetTab(combined, "Combined Style"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
    }
}

QScrollArea *PrHp::scrolled(QWidget *widget)
{
    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widget);
    return scrollArea;
}

QPushButton *PrHp::addWidgetTab(QWidget *widget, const QString &name)
{
    auto w = new QWidget;
    auto layout = new QVBoxLayout(w);
    layout->addWidget(widget);
    auto button = new QPushButton(w);
    button->setText(tr("Apply Changes"));
    layout->addWidget(button);
    layout->setContentsMargins(0, 0, 0, 0);
    tabWidget_->addTab(w, name);
    return button;
}
