#include "preferencenode.h"
#include "preferenceshelper.h"
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
{
    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    auto scrollAreaWidgetContents = new QWidget;
    scrollArea->setWidget(scrollAreaWidgetContents);
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
    setCentralWidget(widget);
    layout->addWidget(scrollArea);

    auto config = new MyConfig(this);
    config->makeLayout(scrollAreaWidgetContents);

    connect(config, &MyConfig::configChanged, this, [](auto key){
        qDebug() << key << "changed";
    });

    auto button = new QPushButton(this);
    button->setText(tr("Apply Changes"));
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, config, &MyConfig::applyed);














    // config.makeLayout(scrollAreaWidgetContents);

    // auto tabWidget = new QTabWidget(this);
    // setCentralWidget(tabWidget);
    // // QVariant().value()

    // // add helper for each preferences widget set to avoid conflicts in applyChanges
    // helper1_ = new PreferencesHelper(this);
    // helper2_ = new PreferencesHelper(this);
    // helper3_ = new PreferencesHelper(this);
    // helper4_ = new PreferencesHelper(this);

    // auto addWidgetTab = [=](auto *widget, const QString &name){
    //     auto w = new QWidget;
    //     auto layout = new QVBoxLayout(w);
    //     layout->addWidget(widget);
    //     auto button = new QPushButton(w);
    //     button->setText(tr("Apply Changes"));
    //     layout->addWidget(button);
    //     layout->setContentsMargins(0, 0, 0, 0);
    //     tabWidget->addTab(w, name);
    //     return button;
    // };

    // // add containters
    // auto scrollArea = new QScrollArea;
    // scrollArea->setWidgetResizable(true);
    // auto scrollAreaWidgetContents = new QWidget;
    // scrollArea->setWidget(scrollAreaWidgetContents);
    // auto button1 = addWidgetTab(scrollArea, tr("Group Box"));
    // connect(button1, &QPushButton::clicked, helper1_, &PreferencesHelper::applyChanges);

    // auto tableview = new QTableView;
    // auto button2 = addWidgetTab(tableview, tr("Table View"));
    // connect(button2, &QPushButton::clicked, helper2_, &PreferencesHelper::applyChanges);

    // auto treeview = new QTreeView;
    // auto button3 = addWidgetTab(treeview, tr("Tree View"));
    // connect(button3, &QPushButton::clicked, helper3_, &PreferencesHelper::applyChanges);

    // auto combined = new QWidget;
    // auto layout = new QHBoxLayout;
    // combined->setLayout(layout);
    // auto view = new QListWidget(this);
    // auto stackWidget = new QStackedWidget(this);
    // layout->addWidget(view, 1);
    // layout->addWidget(stackWidget, 3);
    // view->addItems({
    //     tr("General"),
    //     tr("User Interface"),
    //     tr("Download"),
    // });
    // auto widget1 = new QWidget(this);
    // auto widget2 = new QWidget(this);
    // auto widget3 = new QWidget(this);
    // stackWidget->addWidget(widget1);
    // stackWidget->addWidget(widget2);
    // stackWidget->addWidget(widget3);
    // connect(view, &QListWidget::currentRowChanged, stackWidget, &QStackedWidget::setCurrentIndex);

    // auto button4 = addWidgetTab(combined, tr("Combined View"));
    // connect(button4, &QPushButton::clicked, helper4_, &PreferencesHelper::applyChanges);

    // // template configs
    // LineEditConfig lineEditConfig;
    // SpinBoxConfig spinBoxConfig;
    // spinBoxConfig.setAttr([](auto *widget){
    //     QSpinBox *spinBox = qobject_cast<QSpinBox*>(widget);
    //     spinBox->setMinimum(0);
    //     spinBox->setMaximum(25);
    // });
    // CheckBoxConfig checkBoxConfig;
    // ComboBoxConfig comboBoxConfig1({ "item1", "item2", "item3" });
    // ComboBoxConfig comboBoxConfig2({ "item4", "item5", "item6" });
    // FilePathConfig imageFilePathConfig(tr("Select a image file."), tr("Images (*.png *.jpg *.jpeg)"));

    // // setup PreferenceNode
    // PreferenceNode node;
    // auto generalGroup = node.addGroup("general", tr("General Setting"));
    // generalGroup->addConfig(&lineEditConfig, "config1", "Config1");
    // generalGroup->addConfig(&spinBoxConfig, "config2", "Config2");
    // generalGroup->addConfig(&spinBoxConfig, "config3", "Config3");
    // generalGroup->addConfig(&checkBoxConfig, "config4", "Config4");
    // auto subSettings1 = generalGroup->addGroup("sub1", "SubConfig1", true);
    // generalGroup->addConfig(&comboBoxConfig1, "config5", "Config5");
    // generalGroup->addConfig(&checkBoxConfig, "config6", "Config6");
    // generalGroup->addConfig(&imageFilePathConfig, "config7", "Config7");
    // auto subSettings2 = generalGroup->addGroup("sub2", "SubConfig2", true);

    // subSettings1->addConfig(&spinBoxConfig, "config2", "Config2");
    // subSettings1->addConfig(&spinBoxConfig, "config3", "Config3");
    // subSettings1->addConfig(&checkBoxConfig, "config4", "Config4");
    // subSettings1->addConfig(&comboBoxConfig2, "config5", "Config5");
    // subSettings2->addConfig(&lineEditConfig, "config2", "Config2");
    // subSettings2->addConfig(&comboBoxConfig1, "config3", "Config3");
    // subSettings2->addConfig(&checkBoxConfig, "config4", "Config4");

    // auto uiGroup = node.addGroup("ui", tr("User Interface Setting"), true, false);
    // uiGroup->addConfig(&lineEditConfig, "config1", "Config1");
    // uiGroup->addConfig(&checkBoxConfig, "config4", "Config4");
    // uiGroup->addConfig(&comboBoxConfig1, "config5", "Config5");
    // uiGroup->addConfig(&checkBoxConfig, "config6", "Config6");

    // auto downloadGroup = node.addGroup("download", tr("Download Setting"), true, false);
    // downloadGroup->addConfig(&lineEditConfig, "config1", "Config1");
    // downloadGroup->addConfig(&spinBoxConfig, "config2", "Config2");
    // downloadGroup->addConfig(&lineEditConfig, "config3", "Config3");
    // downloadGroup->addConfig(&checkBoxConfig, "config4", "Config4");
    // downloadGroup->addConfig(&comboBoxConfig1, "config5", "Config5");
    // downloadGroup->addConfig(&checkBoxConfig, "config6", "Config6");
    // downloadGroup->addConfig(&imageFilePathConfig, "config7", "Config7");
    // downloadGroup->addConfig(&imageFilePathConfig, "config8", "Config8");


    // // generate widgets
    // helper1_->makeLayout(&node, scrollAreaWidgetContents);
    // helper2_->makeView(&node, tableview);
    // helper3_->makeView(&node, treeview);

    // helper4_->makeLayout(generalGroup, widget1);
    // helper4_->makeLayout(uiGroup, widget2);
    // helper4_->makeLayout(downloadGroup, widget3);
}
