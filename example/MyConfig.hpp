#ifndef MYCONFIG_HPP
#define MYCONFIG_HPP

#include "../src/appconfig.h"

#include <QColorDialog>
#include <QPushButton>
#include <QToolButton>

#include <qglobal.h>

// just a example to show how to customize default widget for configuring a certain type,
// ofc you should store QColor in string lol <3
class ColorConfig : public WidgetConfig
{
public:
    std::tuple<QWidget*, std::function<QVariant()>> configWidget(QWidget *parentWidget, const QVariant &value) override
    {
        auto color = std::make_shared<QColor>(value.value<QColor>());

        auto colorEditer = new QToolButton(parentWidget);
        colorEditer->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        QPixmap pixmap(64, 64);
        pixmap.fill(*color);
        colorEditer->setIcon(QIcon(pixmap));
        colorEditer->setText(QObject::tr("Select Color"));
        QObject::connect(colorEditer, &QToolButton::clicked, [=]{
            if(auto color2 = QColorDialog::getColor(*color, colorEditer);
                color2.isValid()) {
                *color = color2;
                QPixmap pixmap(64, 64);
                pixmap.fill(*color);
                colorEditer->setIcon(QIcon(pixmap));
            }
        });
        applyAttr(colorEditer);
        return { colorEditer, [color]{
                    return *color;
                }};
    }
};

template<>
struct defaultConfigWidget<QColor>{
    using Type = ColorConfig;
};

class MyConfig : public AppConfig
{
public:
    MyConfig(QObject *parent) :
        AppConfig(parent){

        // Setup display names
        exampleConfig.setName(tr("Example Config"));

        common.setName(tr("Common Settings"));
        common_ui.setName(tr("User Interface"));
        common_ui_toolBar.setName(tr("ToolBar"));
        download.setName(tr("Download Settings"));
        // ...

        // Setup widget config
        auto comboBoxConfig = std::make_shared<ComboBoxConfig>(
            QStringList{ "item1", "item2", "item3" });
        common_set2.setWidgetConfig(comboBoxConfig);

        auto filePathConfig = std::make_shared<FilePathConfig>();
        common_ui_path1.setWidgetConfig(filePathConfig);
        common_ui_path2.setWidgetConfig(filePathConfig);
    }

    ADD_CONFIG(QString, exampleConfig, "default Value");

    ADD_GROUP(common)
    ADD_CONFIG(QString, set1, "", common);
    ADD_CONFIG(int, set2, 1, common);
    ADD_CONFIG(QColor, set3, "red", common);

    ADD_GROUP(ui, common);
    ADD_CONFIG(QString, set1, "", common_ui);
    ADD_CONFIG(QString, set2, "", common_ui);
    ADD_CONFIG(int, set3, 42, common_ui);
    ADD_CONFIG(double, set4, 66, common_ui);
    ADD_CONFIG(QStringList, set5, {}, common_ui);
    ADD_CONFIG(QString, path1, "", common_ui);
    ADD_CONFIG(QString, path2, "", common_ui);

    ADD_CHECKABLE_GROUP(toolBar, true, common_ui);
    ADD_CONFIG(bool, toolBar1, false, common_ui_toolBar);
    ADD_CONFIG(bool, toolBar2, false, common_ui_toolBar);

    ADD_GROUP(download)
    ADD_CONFIG(QString, set1, "", download);
    ADD_CONFIG(QString, set2, "", download);
    ADD_CONFIG(bool, set3, false, download);

    ADD_GROUP(account)
    ADD_CONFIG(QString, set1, "", account);
    ADD_CONFIG(QColor, set2, "yellow", account);
    ADD_CONFIG(bool, set3, false, account);
};

#endif // MYCONFIG_HPP
