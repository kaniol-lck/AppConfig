#ifndef CONFIGWRAPPERWIDGET_H
#define CONFIGWRAPPERWIDGET_H

#include "qmenu.h"
#include "qspinbox.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QToolButton>

template<typename T>
class Wrapper
{
public:
    virtual void genWidget(QWidget *parentWidget) = 0;
    // return if name has been used
    virtual bool setName(const QString &name) = 0;
    virtual T get() = 0;
    virtual void set(const T &val) = 0;
    virtual void setHint(const QString &hint) = 0;
    virtual QWidget *widget() = 0;
};

template<typename WidgetT, typename T>
class WidgetWrapper : public Wrapper<T>
{
public:
    virtual void genWidget(QWidget *parentWidget) override
    {
        widget_ = new WidgetT(parentWidget);
    }

    virtual bool setName(const QString &name[[maybe_unused]]) override
    {
        return false;
    }

    WidgetT *widgetT()
    {
        return widget_;
    }
    QWidget *widget() override
    {
        return widget_;
    }

    virtual T get() override{ return {}; };
    virtual void set(const T &val[[maybe_unused]]) override{};
    virtual void setHint(const QString &hint[[maybe_unused]]) override{};
protected:
    WidgetT *widget_;
};

template<typename T>
class Generator
{
public:
    virtual std::shared_ptr<Wrapper<T>> genWrapper()
    {
        return nullptr;
    };

    virtual void applyAttrSetter(QWidget *widget) = 0;
};


template<typename WrapperT, typename T>
class WrapperGenerator : public Generator<T>
{
public:
    template<typename... Args>
    WrapperGenerator(Args&&... args){
        initWith(std::forward<Args>(args)...);
    }
    template<typename... Args>
    void initWith(Args&&... args){
        initFunc_ = [args...](){
            return std::make_shared<WrapperT>(args...);
        };
    }
    std::shared_ptr<Wrapper<T>> genWrapper() override
    {
        return initFunc_();
    }
    template<typename... Args>
    static auto makePtr(Args&&... args)
    {
        return std::make_shared<WrapperGenerator<WrapperT, T>>(std::forward<Args>(args)...);
    }
    void setAttrSetter(const std::function<void (QWidget *)> &newAttrSetter)
    {
        attrSetter_ = newAttrSetter;
    }

    virtual void applyAttrSetter(QWidget *widget) override
    {
        if(attrSetter_) attrSetter_(widget);
    }
private:
    std::function<std::shared_ptr<Wrapper<T>>()> initFunc_;
    std::function<void (QWidget*)> attrSetter_;
};


class LineEditWrapper : public WidgetWrapper<QLineEdit, QString>
{
public:
    QString get() override
    {
        return widgetT()->text();
    }
    void set(const QString &value) override
    {
        widgetT()->setText(value);
    }
    void setHint(const QString &hint) override
    {
        widgetT()->setPlaceholderText(hint);
    }
};

class SpinBoxWrapper : public WidgetWrapper<QSpinBox, int>
{
public:
    int get() override
    {
        return widgetT()->value();
    }
    void set(const int &value) override
    {
        widgetT()->setValue(value);
    }
};

class DoubleSpinBoxWrapper : public WidgetWrapper<QDoubleSpinBox, double>
{
public:
    double get() override
    {
        return widgetT()->value();
    }
    void set(const double &value) override
    {
        widgetT()->setValue(value);
    }
};

class CheckBoxWrapper : public WidgetWrapper<QCheckBox, bool>
{
public:
    bool setName(const QString &name) override
    {
        widgetT()->setText(name);
        return true;
    }

    bool get() override
    {
        return widgetT()->isChecked();
    }
    void set(const bool &value) override
    {
        widgetT()->setChecked(value);
    }
};

class ComboBoxWrapper : public WidgetWrapper<QComboBox, int>
{
public:
    ComboBoxWrapper(const QStringList &items)
    {
        for(int i = 0; i < items.count(); i++)
            itemMap_[i] = items.at(i);
    }

    ComboBoxWrapper(const QMap<int, QString> &itemMap):
        itemMap_(itemMap)
    {}

    void genWidget(QWidget *parentWidget) override
    {
        WidgetWrapper<QComboBox, int>::genWidget(parentWidget);
        for(auto &&[i, item] : itemMap_.asKeyValueRange())
            widgetT()->addItem(item, i);
    }

    int get() override
    {
        return widgetT()->currentData().toInt();
    }
    void set(const int &value) override
    {
        for(auto i = 0; i < widgetT()->count(); i++){
            if(widgetT()->itemData(i).toInt() == value)
                widgetT()->setCurrentIndex(i);
        }
    }
private:
    QMap<int, QString> itemMap_;
};

class ComboBoxStrWrapper : public WidgetWrapper<QComboBox, QString>
{
public:
    ComboBoxStrWrapper(const QStringList &items, const QStringList &strs) :
        wrapper_(items)
    {
        for(int i = 0; i < strs.count(); i++)
            strMap_[i] = strs.at(i);
    }

    ComboBoxStrWrapper(const QMap<int, QString> &itemMap, const QMap<int, QString> &strMap):
        wrapper_(itemMap),
        strMap_(strMap)
    {}

    void genWidget(QWidget *parentWidget) override
    {
        wrapper_.genWidget(parentWidget);
        widget_ = wrapper_.widgetT();
    }

    QString get() override
    {
        if(auto i = wrapper_.get(); strMap_.contains(i))
            return strMap_.value(i);
        return {};
    }
    void set(const QString &value) override
    {
        for(auto &&[i, str] : strMap_.asKeyValueRange())
            if(value == str) wrapper_.set(i);
    }
private:
    ComboBoxWrapper wrapper_;
    QMap<int, QString> strMap_;
};

class FilePathWrapper : public WidgetWrapper<QWidget, QString>
{
public:
    FilePathWrapper(const QString &caption = "", const QString &filter = "") :
        caption_(caption),
        filter_(filter)
    {}

    void genWidget(QWidget *parentWidget) override
    {
        WidgetWrapper<QWidget, QString>::genWidget(parentWidget);

        auto layout = new QHBoxLayout(widgetT());
        layout->setContentsMargins(0, 0, 0, 0);
        line_ = new QLineEdit(parentWidget);
        layout->addWidget(line_);
        auto button = new QToolButton(parentWidget);
        button->setText("...");
        layout->addWidget(button);

        QObject::connect(button, &QToolButton::clicked, parentWidget, [this]{
            auto fileName = QFileDialog::getOpenFileName(widgetT(), caption_, line_->text(), filter_);
            if(fileName.isEmpty()) return;
            set(fileName);
        });
    }

    QString get() override
    {
        return line_->text();
    }
    void set(const QString &value) override
    {
        line_->setText(value);
    }
private:
    QString caption_;
    QString filter_;
    QLineEdit *line_;
};

class StringListWrapper : public WidgetWrapper<QListView, QStringList>
{
public:
    void genWidget(QWidget *parentWidget) override
    {
        WidgetWrapper<QListView, QStringList>::genWidget(parentWidget);

        auto view = widgetT();
        view->setEditTriggers(QAbstractItemView::DoubleClicked |
                              QAbstractItemView::EditKeyPressed |
                              QAbstractItemView::AnyKeyPressed);
        model_ = new QStandardItemModel(view);
        view->setModel(model_);
        view->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(view, &QListView::customContextMenuRequested, parentWidget, [=, this](auto pos){
            auto menu = new QMenu(view);
            menu->addAction("Add", [this, view = widgetT()]{
                model_->appendRow(new QStandardItem);
                view->edit(model_->index(model_->rowCount() - 1, 0));
            });
            if(auto index = view->indexAt(pos);
                index.isValid())
                menu->addAction("Remove", [this, index]{
                    model_->removeRow(index.row());
                });
            menu->exec(view->mapToGlobal(pos));
        });
    }

    QStringList get() override
    {
        QStringList list;
        for(auto row = 0; row < model_->rowCount(); row++)
            list << model_->item(row)->text();
        return list;
    }
    void set(const QStringList &value) override
    {
        for(auto &&str : value)
            model_->appendRow(new QStandardItem(str));
    }
private:
    QStandardItemModel *model_;
};


#endif // CONFIGWRAPPERWIDGET_H
