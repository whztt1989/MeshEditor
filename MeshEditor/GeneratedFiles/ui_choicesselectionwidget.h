/********************************************************************************
** Form generated from reading UI file 'choicesselectionwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHOICESSELECTIONWIDGET_H
#define UI_CHOICESSELECTIONWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChoicesSelectionWidget
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *lb_Choices1;
    QComboBox *cb_Choices1;
    QLabel *lb_Choices2;
    QComboBox *cb_Choices2;
    QLabel *lb_Choices3;
    QComboBox *cb_Choices3;
    QLabel *lb_Choices4;
    QComboBox *cb_Choices4;
    QPushButton *pb_OK;

    void setupUi(QWidget *ChoicesSelectionWidget)
    {
        if (ChoicesSelectionWidget->objectName().isEmpty())
            ChoicesSelectionWidget->setObjectName(QString::fromUtf8("ChoicesSelectionWidget"));
        ChoicesSelectionWidget->resize(137, 156);
        verticalLayout = new QVBoxLayout(ChoicesSelectionWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lb_Choices1 = new QLabel(ChoicesSelectionWidget);
        lb_Choices1->setObjectName(QString::fromUtf8("lb_Choices1"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lb_Choices1);

        cb_Choices1 = new QComboBox(ChoicesSelectionWidget);
        cb_Choices1->setObjectName(QString::fromUtf8("cb_Choices1"));

        formLayout->setWidget(0, QFormLayout::FieldRole, cb_Choices1);

        lb_Choices2 = new QLabel(ChoicesSelectionWidget);
        lb_Choices2->setObjectName(QString::fromUtf8("lb_Choices2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lb_Choices2);

        cb_Choices2 = new QComboBox(ChoicesSelectionWidget);
        cb_Choices2->setObjectName(QString::fromUtf8("cb_Choices2"));

        formLayout->setWidget(1, QFormLayout::FieldRole, cb_Choices2);

        lb_Choices3 = new QLabel(ChoicesSelectionWidget);
        lb_Choices3->setObjectName(QString::fromUtf8("lb_Choices3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, lb_Choices3);

        cb_Choices3 = new QComboBox(ChoicesSelectionWidget);
        cb_Choices3->setObjectName(QString::fromUtf8("cb_Choices3"));

        formLayout->setWidget(2, QFormLayout::FieldRole, cb_Choices3);

        lb_Choices4 = new QLabel(ChoicesSelectionWidget);
        lb_Choices4->setObjectName(QString::fromUtf8("lb_Choices4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, lb_Choices4);

        cb_Choices4 = new QComboBox(ChoicesSelectionWidget);
        cb_Choices4->setObjectName(QString::fromUtf8("cb_Choices4"));

        formLayout->setWidget(3, QFormLayout::FieldRole, cb_Choices4);


        verticalLayout->addLayout(formLayout);

        pb_OK = new QPushButton(ChoicesSelectionWidget);
        pb_OK->setObjectName(QString::fromUtf8("pb_OK"));

        verticalLayout->addWidget(pb_OK);


        retranslateUi(ChoicesSelectionWidget);

        QMetaObject::connectSlotsByName(ChoicesSelectionWidget);
    } // setupUi

    void retranslateUi(QWidget *ChoicesSelectionWidget)
    {
        ChoicesSelectionWidget->setWindowTitle(QApplication::translate("ChoicesSelectionWidget", "ChoicesSelectionWidget", 0, QApplication::UnicodeUTF8));
        lb_Choices1->setText(QApplication::translate("ChoicesSelectionWidget", "\351\200\211\351\241\2711\357\274\232", 0, QApplication::UnicodeUTF8));
        lb_Choices2->setText(QApplication::translate("ChoicesSelectionWidget", "\351\200\211\351\241\2712\357\274\232", 0, QApplication::UnicodeUTF8));
        lb_Choices3->setText(QApplication::translate("ChoicesSelectionWidget", "\351\200\211\351\241\2713\357\274\232", 0, QApplication::UnicodeUTF8));
        lb_Choices4->setText(QApplication::translate("ChoicesSelectionWidget", "\351\200\211\351\241\2714\357\274\232", 0, QApplication::UnicodeUTF8));
        pb_OK->setText(QApplication::translate("ChoicesSelectionWidget", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ChoicesSelectionWidget: public Ui_ChoicesSelectionWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOICESSELECTIONWIDGET_H
