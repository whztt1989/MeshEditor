/********************************************************************************
** Form generated from reading UI file 'sheetintdiagramwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHEETINTDIAGRAMWIDGET_H
#define UI_SHEETINTDIAGRAMWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SheetIntDiagramWidget
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget_info;
    QLabel *label_Sheet_Index;
    QPushButton *pushButton_Show_Sheet_In_Mesh;
    QPushButton *pushButton_Show_SID_In_Out_Window;

    void setupUi(QWidget *SheetIntDiagramWidget)
    {
        if (SheetIntDiagramWidget->objectName().isEmpty())
            SheetIntDiagramWidget->setObjectName(QString::fromUtf8("SheetIntDiagramWidget"));
        SheetIntDiagramWidget->resize(416, 401);
        verticalLayout_2 = new QVBoxLayout(SheetIntDiagramWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget_info = new QWidget(SheetIntDiagramWidget);
        widget_info->setObjectName(QString::fromUtf8("widget_info"));
        widget_info->setMinimumSize(QSize(0, 30));
        widget_info->setMaximumSize(QSize(16777215, 30));
        label_Sheet_Index = new QLabel(widget_info);
        label_Sheet_Index->setObjectName(QString::fromUtf8("label_Sheet_Index"));
        label_Sheet_Index->setGeometry(QRect(0, 10, 81, 20));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        label_Sheet_Index->setFont(font);
        pushButton_Show_Sheet_In_Mesh = new QPushButton(widget_info);
        pushButton_Show_Sheet_In_Mesh->setObjectName(QString::fromUtf8("pushButton_Show_Sheet_In_Mesh"));
        pushButton_Show_Sheet_In_Mesh->setGeometry(QRect(90, 0, 81, 30));
        pushButton_Show_SID_In_Out_Window = new QPushButton(widget_info);
        pushButton_Show_SID_In_Out_Window->setObjectName(QString::fromUtf8("pushButton_Show_SID_In_Out_Window"));
        pushButton_Show_SID_In_Out_Window->setGeometry(QRect(170, 0, 81, 30));

        verticalLayout->addWidget(widget_info);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(SheetIntDiagramWidget);

        QMetaObject::connectSlotsByName(SheetIntDiagramWidget);
    } // setupUi

    void retranslateUi(QWidget *SheetIntDiagramWidget)
    {
        SheetIntDiagramWidget->setWindowTitle(QApplication::translate("SheetIntDiagramWidget", "SheetIntDiagramWidget", 0, QApplication::UnicodeUTF8));
        label_Sheet_Index->setText(QApplication::translate("SheetIntDiagramWidget", "Sheet 0", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Sheet_In_Mesh->setText(QApplication::translate("SheetIntDiagramWidget", "\345\234\250\347\275\221\346\240\274\344\270\255\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        pushButton_Show_SID_In_Out_Window->setText(QApplication::translate("SheetIntDiagramWidget", "\345\234\250\345\244\226\351\203\250\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SheetIntDiagramWidget: public Ui_SheetIntDiagramWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHEETINTDIAGRAMWIDGET_H
