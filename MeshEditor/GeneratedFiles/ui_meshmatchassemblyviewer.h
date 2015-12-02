/********************************************************************************
** Form generated from reading UI file 'meshmatchassemblyviewer.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHMATCHASSEMBLYVIEWER_H
#define UI_MESHMATCHASSEMBLYVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <hoopsview.h>

QT_BEGIN_NAMESPACE

class Ui_MeshMatchAssemblyViewer
{
public:
    QVBoxLayout *verticalLayout;
    HoopsView *hoopsview;
    QWidget *widget_2;
    QPushButton *pushButton_Refresh_Matching;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;

    void setupUi(QWidget *MeshMatchAssemblyViewer)
    {
        if (MeshMatchAssemblyViewer->objectName().isEmpty())
            MeshMatchAssemblyViewer->setObjectName(QString::fromUtf8("MeshMatchAssemblyViewer"));
        MeshMatchAssemblyViewer->resize(470, 434);
        verticalLayout = new QVBoxLayout(MeshMatchAssemblyViewer);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        hoopsview = new HoopsView(MeshMatchAssemblyViewer);
        hoopsview->setObjectName(QString::fromUtf8("hoopsview"));

        verticalLayout->addWidget(hoopsview);

        widget_2 = new QWidget(MeshMatchAssemblyViewer);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMinimumSize(QSize(0, 60));
        widget_2->setMaximumSize(QSize(16777215, 60));
        pushButton_Refresh_Matching = new QPushButton(widget_2);
        pushButton_Refresh_Matching->setObjectName(QString::fromUtf8("pushButton_Refresh_Matching"));
        pushButton_Refresh_Matching->setGeometry(QRect(120, 10, 75, 23));
        radioButton = new QRadioButton(widget_2);
        radioButton->setObjectName(QString::fromUtf8("radioButton"));
        radioButton->setGeometry(QRect(10, 10, 121, 16));
        radioButton->setChecked(true);
        radioButton_2 = new QRadioButton(widget_2);
        radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
        radioButton_2->setGeometry(QRect(10, 30, 121, 16));

        verticalLayout->addWidget(widget_2);


        retranslateUi(MeshMatchAssemblyViewer);

        QMetaObject::connectSlotsByName(MeshMatchAssemblyViewer);
    } // setupUi

    void retranslateUi(QWidget *MeshMatchAssemblyViewer)
    {
        MeshMatchAssemblyViewer->setWindowTitle(QApplication::translate("MeshMatchAssemblyViewer", "MeshMatchAssemblyViewer", 0, QApplication::UnicodeUTF8));
        pushButton_Refresh_Matching->setText(QApplication::translate("MeshMatchAssemblyViewer", "\345\210\267\346\226\260\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        radioButton->setText(QApplication::translate("MeshMatchAssemblyViewer", "\345\256\214\346\225\264\347\275\221\346\240\274", 0, QApplication::UnicodeUTF8));
        radioButton_2->setText(QApplication::translate("MeshMatchAssemblyViewer", "\350\264\264\345\220\210\351\235\242\347\275\221\346\240\274", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeshMatchAssemblyViewer: public Ui_MeshMatchAssemblyViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHMATCHASSEMBLYVIEWER_H
