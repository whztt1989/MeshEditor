/********************************************************************************
** Form generated from reading UI file 'manualmeshmatch.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANUALMESHMATCH_H
#define UI_MANUALMESHMATCH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <hoopsview.h>
#include <mesheditcontroller.h>

QT_BEGIN_NAMESPACE

class Ui_ManualMeshMatch
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_Open;
    QPushButton *pushButton_Open_2;
    QPushButton *pushButton_Init_Matching;
    QPushButton *pushButton_Auto_Match;
    QPushButton *pushButton_Match_Two_Chords;
    QPushButton *pushButton_Get_Polyline;
    QPushButton *pushButton_Redraw_Matched_Chords;
    QPushButton *pushButton_Check_Match;
    QPushButton *pushButton_Show_Chord_Pairs;
    QPushButton *pushButton_Show_Interface;
    QPushButton *pushButton_Save;
    QPushButton *pushButton_Merge;
    QPushButton *pushButton_See_Only_Left;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    MeshEditController *mesh_edit_controller1;
    HoopsView *hoopsview1;
    HoopsView *hoopsview2;
    MeshEditController *mesh_edit_controller2;

    void setupUi(QWidget *ManualMeshMatch)
    {
        if (ManualMeshMatch->objectName().isEmpty())
            ManualMeshMatch->setObjectName(QString::fromUtf8("ManualMeshMatch"));
        ManualMeshMatch->resize(1307, 719);
        verticalLayout = new QVBoxLayout(ManualMeshMatch);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(6, 3, -1, -1);
        pushButton_Open = new QPushButton(ManualMeshMatch);
        pushButton_Open->setObjectName(QString::fromUtf8("pushButton_Open"));

        horizontalLayout_5->addWidget(pushButton_Open);

        pushButton_Open_2 = new QPushButton(ManualMeshMatch);
        pushButton_Open_2->setObjectName(QString::fromUtf8("pushButton_Open_2"));

        horizontalLayout_5->addWidget(pushButton_Open_2);

        pushButton_Init_Matching = new QPushButton(ManualMeshMatch);
        pushButton_Init_Matching->setObjectName(QString::fromUtf8("pushButton_Init_Matching"));

        horizontalLayout_5->addWidget(pushButton_Init_Matching);

        pushButton_Auto_Match = new QPushButton(ManualMeshMatch);
        pushButton_Auto_Match->setObjectName(QString::fromUtf8("pushButton_Auto_Match"));
        pushButton_Auto_Match->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);\n"
"font: 75 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

        horizontalLayout_5->addWidget(pushButton_Auto_Match);

        pushButton_Match_Two_Chords = new QPushButton(ManualMeshMatch);
        pushButton_Match_Two_Chords->setObjectName(QString::fromUtf8("pushButton_Match_Two_Chords"));

        horizontalLayout_5->addWidget(pushButton_Match_Two_Chords);

        pushButton_Get_Polyline = new QPushButton(ManualMeshMatch);
        pushButton_Get_Polyline->setObjectName(QString::fromUtf8("pushButton_Get_Polyline"));

        horizontalLayout_5->addWidget(pushButton_Get_Polyline);

        pushButton_Redraw_Matched_Chords = new QPushButton(ManualMeshMatch);
        pushButton_Redraw_Matched_Chords->setObjectName(QString::fromUtf8("pushButton_Redraw_Matched_Chords"));

        horizontalLayout_5->addWidget(pushButton_Redraw_Matched_Chords);

        pushButton_Check_Match = new QPushButton(ManualMeshMatch);
        pushButton_Check_Match->setObjectName(QString::fromUtf8("pushButton_Check_Match"));

        horizontalLayout_5->addWidget(pushButton_Check_Match);

        pushButton_Show_Chord_Pairs = new QPushButton(ManualMeshMatch);
        pushButton_Show_Chord_Pairs->setObjectName(QString::fromUtf8("pushButton_Show_Chord_Pairs"));

        horizontalLayout_5->addWidget(pushButton_Show_Chord_Pairs);

        pushButton_Show_Interface = new QPushButton(ManualMeshMatch);
        pushButton_Show_Interface->setObjectName(QString::fromUtf8("pushButton_Show_Interface"));

        horizontalLayout_5->addWidget(pushButton_Show_Interface);

        pushButton_Save = new QPushButton(ManualMeshMatch);
        pushButton_Save->setObjectName(QString::fromUtf8("pushButton_Save"));

        horizontalLayout_5->addWidget(pushButton_Save);

        pushButton_Merge = new QPushButton(ManualMeshMatch);
        pushButton_Merge->setObjectName(QString::fromUtf8("pushButton_Merge"));

        horizontalLayout_5->addWidget(pushButton_Merge);

        pushButton_See_Only_Left = new QPushButton(ManualMeshMatch);
        pushButton_See_Only_Left->setObjectName(QString::fromUtf8("pushButton_See_Only_Left"));

        horizontalLayout_5->addWidget(pushButton_See_Only_Left);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mesh_edit_controller1 = new MeshEditController(ManualMeshMatch);
        mesh_edit_controller1->setObjectName(QString::fromUtf8("mesh_edit_controller1"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mesh_edit_controller1->sizePolicy().hasHeightForWidth());
        mesh_edit_controller1->setSizePolicy(sizePolicy);
        mesh_edit_controller1->setMinimumSize(QSize(150, 500));
        mesh_edit_controller1->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(mesh_edit_controller1);

        hoopsview1 = new HoopsView(ManualMeshMatch);
        hoopsview1->setObjectName(QString::fromUtf8("hoopsview1"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(hoopsview1->sizePolicy().hasHeightForWidth());
        hoopsview1->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(hoopsview1);

        hoopsview2 = new HoopsView(ManualMeshMatch);
        hoopsview2->setObjectName(QString::fromUtf8("hoopsview2"));
        sizePolicy1.setHeightForWidth(hoopsview2->sizePolicy().hasHeightForWidth());
        hoopsview2->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(hoopsview2);

        mesh_edit_controller2 = new MeshEditController(ManualMeshMatch);
        mesh_edit_controller2->setObjectName(QString::fromUtf8("mesh_edit_controller2"));
        sizePolicy.setHeightForWidth(mesh_edit_controller2->sizePolicy().hasHeightForWidth());
        mesh_edit_controller2->setSizePolicy(sizePolicy);
        mesh_edit_controller2->setMinimumSize(QSize(150, 500));
        mesh_edit_controller2->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(mesh_edit_controller2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ManualMeshMatch);

        QMetaObject::connectSlotsByName(ManualMeshMatch);
    } // setupUi

    void retranslateUi(QWidget *ManualMeshMatch)
    {
        ManualMeshMatch->setWindowTitle(QApplication::translate("ManualMeshMatch", "ManualMeshMatch", 0, QApplication::UnicodeUTF8));
        pushButton_Open->setText(QApplication::translate("ManualMeshMatch", "\346\211\223\345\274\200", 0, QApplication::UnicodeUTF8));
        pushButton_Open_2->setText(QApplication::translate("ManualMeshMatch", "\346\211\223\345\274\2002", 0, QApplication::UnicodeUTF8));
        pushButton_Init_Matching->setText(QApplication::translate("ManualMeshMatch", "\345\210\235\345\247\213\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Auto_Match->setText(QApplication::translate("ManualMeshMatch", "\350\207\252\345\212\250\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Match_Two_Chords->setText(QApplication::translate("ManualMeshMatch", "Chord\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Polyline->setText(QApplication::translate("ManualMeshMatch", "Polyline", 0, QApplication::UnicodeUTF8));
        pushButton_Redraw_Matched_Chords->setText(QApplication::translate("ManualMeshMatch", "\351\207\215\347\273\230\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Check_Match->setText(QApplication::translate("ManualMeshMatch", "\345\214\271\351\205\215\346\243\200\346\265\213", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Chord_Pairs->setText(QApplication::translate("ManualMeshMatch", "\346\237\245\347\234\213\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Interface->setText(QApplication::translate("ManualMeshMatch", "\346\237\245\347\234\213\350\264\264\345\220\210\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Save->setText(QApplication::translate("ManualMeshMatch", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        pushButton_Merge->setText(QApplication::translate("ManualMeshMatch", "\345\220\210\345\271\266", 0, QApplication::UnicodeUTF8));
        pushButton_See_Only_Left->setText(QApplication::translate("ManualMeshMatch", "\345\217\252\347\234\213\345\267\246\350\276\271", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ManualMeshMatch: public Ui_ManualMeshMatch {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUALMESHMATCH_H
