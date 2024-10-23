#ifndef MYCONFIG_HPP
#define MYCONFIG_HPP

#include "../src/appconfig.h"

#include <QColorDialog>
#include <QPushButton>
#include <QToolButton>

#include <qglobal.h>

// just a example to show how to customize default widget for configuring a certain type,
// ofc you should store QColor in string lol <3
class ColorWrapper : public WidgetWrapper<QToolButton, QColor>
{
public:
    void genWidget(QWidget *parentWidget) override
    {
        WidgetWrapper<QToolButton, QColor>::genWidget(parentWidget);

        auto colorEditer = widgetT();
        colorEditer->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        colorEditer->setText(QObject::tr("Select Color"));

        QObject::connect(colorEditer, &QToolButton::clicked, [this]{
            if(auto color = QColorDialog::getColor(get(), widgetT());
                color.isValid()) {
                set(color);
            }
        });
    }
    QColor get() override
    {
        return color_;
    }
    void set(const QColor &value) override
    {
        color_ = value;
        QPixmap pixmap(64, 64);
        pixmap.fill(color_);
        widgetT()->setIcon(QIcon(pixmap));
    }
private:
    QColor color_;
};

template<>
struct defaultConfigWidget<QColor>{
    using Type = ColorWrapper;
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

        common.setTooltip("This is a tooltip on title");
        common_set1.setTooltip("This is a tooltip");
        common_set1.setHint("This is a hint");

        common_set2.setGenerator<ComboBoxWrapper>(QStringList{ "item1", "item2", "item3" });

        auto filePath = WrapperGenerator<FilePathWrapper, QString>::makePtr("Select image files plz", "*.jpg;*.png");
        common_ui_path1.setGeneratorPtr(filePath);
        common_ui_path2.setGeneratorPtr(filePath);
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
    ADD_CONFIG(QStringList, set5, (QStringList{ "123", "456" }), common_ui);
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
