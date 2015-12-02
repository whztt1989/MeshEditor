/********************************************************************************
** Form generated from reading UI file 'groupcontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GROUPCONTROLWIDGET_H
#define UI_GROUPCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GroupControlWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton_Setting;
    QPushButton *pushButton_Clear_Highlights;
    QPushButton *pushButton_Refresh;
    QListWidget *listWidget_Groups;
    QPushButton *pushButton_Delete;
    QPushButton *pushButton_Show_All;
    QPushButton *pushButton_Calc_Irre_Degree;
    QPushButton *pushButton_Save;
    QPushButton *pushButton_Hide_All;
    QPushButton *pushButton_Calc_Int_Line;
    QPushButton *pushButton_Show_Boundary_Chord;
    QPushButton *pushButton_Show_Info;

    void setupUi(QWidget *GroupControlWidget)
    {
        if (GroupControlWidget->objectName().isEmpty())
            GroupControlWidget->setObjectName(QString::fromUtf8("GroupControlWidget"));
        GroupControlWidget->resize(425, 128);
        gridLayout = new QGridLayout(GroupControlWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_Setting = new QPushButton(GroupControlWidget);
        pushButton_Setting->setObjectName(QString::fromUtf8("pushButton_Setting"));

        gridLayout->addWidget(pushButton_Setting, 3, 1, 1, 1);

        pushButton_Clear_Highlights = new QPushButton(GroupControlWidget);
        pushButton_Clear_Highlights->setObjectName(QString::fromUtf8("pushButton_Clear_Highlights"));

        gridLayout->addWidget(pushButton_Clear_Highlights, 2, 1, 1, 1);

        pushButton_Refresh = new QPushButton(GroupControlWidget);
        pushButton_Refresh->setObjectName(QString::fromUtf8("pushButton_Refresh"));

        gridLayout->addWidget(pushButton_Refresh, 0, 1, 1, 1);

        listWidget_Groups = new QListWidget(GroupControlWidget);
        listWidget_Groups->setObjectName(QString::fromUtf8("listWidget_Groups"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget_Groups->sizePolicy().hasHeightForWidth());
        listWidget_Groups->setSizePolicy(sizePolicy);
        listWidget_Groups->setAlternatingRowColors(true);
        listWidget_Groups->setSelectionMode(QAbstractItemView::MultiSelection);
        listWidget_Groups->setSelectionRectVisible(false);

        gridLayout->addWidget(listWidget_Groups, 0, 0, 4, 1);

        pushButton_Delete = new QPushButton(GroupControlWidget);
        pushButton_Delete->setObjectName(QString::fromUtf8("pushButton_Delete"));

        gridLayout->addWidget(pushButton_Delete, 1, 1, 1, 1);

        pushButton_Show_All = new QPushButton(GroupControlWidget);
        pushButton_Show_All->setObjectName(QString::fromUtf8("pushButton_Show_All"));

        gridLayout->addWidget(pushButton_Show_All, 3, 2, 1, 1);

        pushButton_Calc_Irre_Degree = new QPushButton(GroupControlWidget);
        pushButton_Calc_Irre_Degree->setObjectName(QString::fromUtf8("pushButton_Calc_Irre_Degree"));

        gridLayout->addWidget(pushButton_Calc_Irre_Degree, 0, 2, 1, 1);

        pushButton_Save = new QPushButton(GroupControlWidget);
        pushButton_Save->setObjectName(QString::fromUtf8("pushButton_Save"));

        gridLayout->addWidget(pushButton_Save, 1, 2, 1, 1);

        pushButton_Hide_All = new QPushButton(GroupControlWidget);
        pushButton_Hide_All->setObjectName(QString::fromUtf8("pushButton_Hide_All"));

        gridLayout->addWidget(pushButton_Hide_All, 2, 2, 1, 1);

        pushButton_Calc_Int_Line = new QPushButton(GroupControlWidget);
        pushButton_Calc_Int_Line->setObjectName(QString::fromUtf8("pushButton_Calc_Int_Line"));

        gridLayout->addWidget(pushButton_Calc_Int_Line, 0, 3, 1, 1);

        pushButton_Show_Boundary_Chord = new QPushButton(GroupControlWidget);
        pushButton_Show_Boundary_Chord->setObjectName(QString::fromUtf8("pushButton_Show_Boundary_Chord"));

        gridLayout->addWidget(pushButton_Show_Boundary_Chord, 1, 3, 1, 1);

        pushButton_Show_Info = new QPushButton(GroupControlWidget);
        pushButton_Show_Info->setObjectName(QString::fromUtf8("pushButton_Show_Info"));

        gridLayout->addWidget(pushButton_Show_Info, 2, 3, 1, 1);


        retranslateUi(GroupControlWidget);

        QMetaObject::connectSlotsByName(GroupControlWidget);
    } // setupUi

    void retranslateUi(QWidget *GroupControlWidget)
    {
        GroupControlWidget->setWindowTitle(QApplication::translate("GroupControlWidget", "GroupControlWidget", 0, QApplication::UnicodeUTF8));
        pushButton_Setting->setText(QApplication::translate("GroupControlWidget", "\350\256\276\347\275\256", 0, QApplication::UnicodeUTF8));
        pushButton_Clear_Highlights->setText(QApplication::translate("GroupControlWidget", "\346\270\205\351\231\244\351\253\230\344\272\256", 0, QApplication::UnicodeUTF8));
        pushButton_Refresh->setText(QApplication::translate("GroupControlWidget", "\345\210\267\346\226\260", 0, QApplication::UnicodeUTF8));
        pushButton_Delete->setText(QApplication::translate("GroupControlWidget", "\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Show_All->setText(QApplication::translate("GroupControlWidget", "\346\230\276\347\244\272\345\205\250\351\203\250", 0, QApplication::UnicodeUTF8));
        pushButton_Calc_Irre_Degree->setText(QApplication::translate("GroupControlWidget", "\344\270\215\350\247\204\345\210\231\345\272\246", 0, QApplication::UnicodeUTF8));
        pushButton_Save->setText(QApplication::translate("GroupControlWidget", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        pushButton_Hide_All->setText(QApplication::translate("GroupControlWidget", "\351\232\220\350\227\217\345\205\250\351\203\250", 0, QApplication::UnicodeUTF8));
        pushButton_Calc_Int_Line->setText(QApplication::translate("GroupControlWidget", "\346\261\202\347\233\270\344\272\244\347\272\277", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Boundary_Chord->setText(QApplication::translate("GroupControlWidget", "\346\230\276\347\244\272chord", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Info->setText(QApplication::translate("GroupControlWidget", "\346\230\276\347\244\272\344\277\241\346\201\257", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GroupControlWidget: public Ui_GroupControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPCONTROLWIDGET_H
