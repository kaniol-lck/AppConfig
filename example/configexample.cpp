#include "configexample.h"
#include "MyConfig.hpp"

#include <QTableView>
#include <QTreeView>
#include <QScrollArea>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSpinBox>

ConfigExample::ConfigExample(QWidget *parent)
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

    // custom settings
    {
        auto settings = std::make_shared<MapSetting>();
        auto appConfig = new AppConfig(this, settings);
        auto attr1 = std::make_shared<ConfigItem<QString>>(appConfig, "attr1", "1");
        auto attr2 = std::make_shared<ConfigItem<QString>>(appConfig, "attr2", "i2");
        auto attr3 = std::make_shared<ConfigItem<int>>(appConfig, "attr3", 3);
        attr2->setGenerator<ComboBoxStrWrapper>(QStringList{ "item1", "item2" },
                                                QStringList{ "i1", "i2" });
        auto widget = new QWidget;
        auto handler = appConfig->makeLayout(widget);
        connect(addWidgetTab(scrolled(widget), "Variable Custom Settings"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
        connect(handler, &ApplyHandler::applyed, this, [=]{
            qDebug() << settings->variantMap();
            qDebug() << attr1->get();
            qDebug() << attr2->get();
            qDebug() << attr3->get();
        });
    }

    // json settings
    {
        class CustomConfig : public AppConfig
        {
        public:
            CustomConfig(QObject *parent, std::shared_ptr<JsonSetting> settings) :
                AppConfig(parent, settings){
            }

            ADD_CONFIG(QString, attr1, "test1")
            ADD_CONFIG(QString, attr2, "test2")
            ADD_GROUP(number)
            ADD_CONFIG(int, attr3, 42, number)
            ADD_CONFIG(int, attr4, 43, number)
        };
        auto settings = std::make_shared<JsonSetting>();
        settings->setObject(QJsonObject({
                                         {"attr1", "json"}
        }));
        auto custom = new CustomConfig(this, settings);
        auto widget = new QWidget;
        auto handler = custom->makeLayout(widget);
        connect(addWidgetTab(scrolled(widget), "JSON Custom Settings"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
        connect(handler, &ApplyHandler::applyed, this, [=]{
            qDebug() << settings->object();
        });
    }

    // Group Box Style
    {
        auto widget = new QWidget;
        auto handler = config->makeLayout(widget);
        connect(addWidgetTab(scrolled(widget), "Group Box Style"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
    }

    // Group Box Style 2
    {
        auto widget = new QWidget;
        auto handler = config->makeLayout2(widget);
        connect(addWidgetTab(scrolled(widget), "Group Box Style (Line)"), &QPushButton::clicked, handler, &ApplyHandler::applyed);
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

QScrollArea *ConfigExample::scrolled(QWidget *widget)
{
    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(widget);
    return scrollArea;
}

QPushButton *ConfigExample::addWidgetTab(QWidget *widget, const QString &name)
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
