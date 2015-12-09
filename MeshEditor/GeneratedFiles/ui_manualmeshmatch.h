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
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
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
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_Open;
    QPushButton *pushButton_Open_2;
    QPushButton *pushButton_Save;
    QSpacerItem *horizontalSpacer;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_Init_Matching;
    QFrame *line;
    QPushButton *pushButton_Match_Two_Chords;
    QPushButton *pushButton_Get_Polyline;
    QFrame *line_2;
    QPushButton *pushButton_Check_Match;
    QFrame *line_4;
    QPushButton *pushButton_Auto_Match;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_3;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pushButton_Redraw_Matched_Chords;
    QPushButton *pushButton_Show_Chord_Pairs;
    QPushButton *pushButton_Show_Interface;
    QFrame *line_3;
    QCheckBox *cb_Show_Left_Side;
    QCheckBox *cb_Show_Right_Side;
    QSpacerItem *horizontalSpacer_3;
    QWidget *tab_4;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pushButton_Merge;
    QSpacerItem *horizontalSpacer_4;
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
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, -1, -1);
        tabWidget = new QTabWidget(ManualMeshMatch);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setDocumentMode(false);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout_2 = new QHBoxLayout(tab);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_Open = new QPushButton(tab);
        pushButton_Open->setObjectName(QString::fromUtf8("pushButton_Open"));

        horizontalLayout_2->addWidget(pushButton_Open);

        pushButton_Open_2 = new QPushButton(tab);
        pushButton_Open_2->setObjectName(QString::fromUtf8("pushButton_Open_2"));

        horizontalLayout_2->addWidget(pushButton_Open_2);

        pushButton_Save = new QPushButton(tab);
        pushButton_Save->setObjectName(QString::fromUtf8("pushButton_Save"));

        horizontalLayout_2->addWidget(pushButton_Save);

        horizontalSpacer = new QSpacerItem(1017, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_7 = new QHBoxLayout(tab_2);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_Init_Matching = new QPushButton(tab_2);
        pushButton_Init_Matching->setObjectName(QString::fromUtf8("pushButton_Init_Matching"));

        horizontalLayout_7->addWidget(pushButton_Init_Matching);

        line = new QFrame(tab_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_7->addWidget(line);

        pushButton_Match_Two_Chords = new QPushButton(tab_2);
        pushButton_Match_Two_Chords->setObjectName(QString::fromUtf8("pushButton_Match_Two_Chords"));

        horizontalLayout_7->addWidget(pushButton_Match_Two_Chords);

        pushButton_Get_Polyline = new QPushButton(tab_2);
        pushButton_Get_Polyline->setObjectName(QString::fromUtf8("pushButton_Get_Polyline"));

        horizontalLayout_7->addWidget(pushButton_Get_Polyline);

        line_2 = new QFrame(tab_2);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        horizontalLayout_7->addWidget(line_2);

        pushButton_Check_Match = new QPushButton(tab_2);
        pushButton_Check_Match->setObjectName(QString::fromUtf8("pushButton_Check_Match"));

        horizontalLayout_7->addWidget(pushButton_Check_Match);

        line_4 = new QFrame(tab_2);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);

        horizontalLayout_7->addWidget(line_4);

        pushButton_Auto_Match = new QPushButton(tab_2);
        pushButton_Auto_Match->setObjectName(QString::fromUtf8("pushButton_Auto_Match"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        pushButton_Auto_Match->setFont(font);
        pushButton_Auto_Match->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_7->addWidget(pushButton_Auto_Match);

        horizontalSpacer_2 = new QSpacerItem(823, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        horizontalLayout_8 = new QHBoxLayout(tab_3);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        pushButton_Redraw_Matched_Chords = new QPushButton(tab_3);
        pushButton_Redraw_Matched_Chords->setObjectName(QString::fromUtf8("pushButton_Redraw_Matched_Chords"));

        horizontalLayout_8->addWidget(pushButton_Redraw_Matched_Chords);

        pushButton_Show_Chord_Pairs = new QPushButton(tab_3);
        pushButton_Show_Chord_Pairs->setObjectName(QString::fromUtf8("pushButton_Show_Chord_Pairs"));

        horizontalLayout_8->addWidget(pushButton_Show_Chord_Pairs);

        pushButton_Show_Interface = new QPushButton(tab_3);
        pushButton_Show_Interface->setObjectName(QString::fromUtf8("pushButton_Show_Interface"));

        horizontalLayout_8->addWidget(pushButton_Show_Interface);

        line_3 = new QFrame(tab_3);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_8->addWidget(line_3);

        cb_Show_Left_Side = new QCheckBox(tab_3);
        cb_Show_Left_Side->setObjectName(QString::fromUtf8("cb_Show_Left_Side"));
        cb_Show_Left_Side->setChecked(true);

        horizontalLayout_8->addWidget(cb_Show_Left_Side);

        cb_Show_Right_Side = new QCheckBox(tab_3);
        cb_Show_Right_Side->setObjectName(QString::fromUtf8("cb_Show_Right_Side"));
        cb_Show_Right_Side->setChecked(true);

        horizontalLayout_8->addWidget(cb_Show_Right_Side);

        horizontalSpacer_3 = new QSpacerItem(878, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_3);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        horizontalLayout_9 = new QHBoxLayout(tab_4);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pushButton_Merge = new QPushButton(tab_4);
        pushButton_Merge->setObjectName(QString::fromUtf8("pushButton_Merge"));

        horizontalLayout_9->addWidget(pushButton_Merge);

        horizontalSpacer_4 = new QSpacerItem(1179, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_4);

        tabWidget->addTab(tab_4, QString());

        horizontalLayout_5->addWidget(tabWidget);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mesh_edit_controller1 = new MeshEditController(ManualMeshMatch);
        mesh_edit_controller1->setObjectName(QString::fromUtf8("mesh_edit_controller1"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mesh_edit_controller1->sizePolicy().hasHeightForWidth());
        mesh_edit_controller1->setSizePolicy(sizePolicy1);
        mesh_edit_controller1->setMinimumSize(QSize(150, 500));
        mesh_edit_controller1->setMaximumSize(QSize(150, 16777215));
        tabWidget->raise();

        horizontalLayout->addWidget(mesh_edit_controller1);

        hoopsview1 = new HoopsView(ManualMeshMatch);
        hoopsview1->setObjectName(QString::fromUtf8("hoopsview1"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(hoopsview1->sizePolicy().hasHeightForWidth());
        hoopsview1->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(hoopsview1);

        hoopsview2 = new HoopsView(ManualMeshMatch);
        hoopsview2->setObjectName(QString::fromUtf8("hoopsview2"));
        sizePolicy2.setHeightForWidth(hoopsview2->sizePolicy().hasHeightForWidth());
        hoopsview2->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(hoopsview2);

        mesh_edit_controller2 = new MeshEditController(ManualMeshMatch);
        mesh_edit_controller2->setObjectName(QString::fromUtf8("mesh_edit_controller2"));
        sizePolicy1.setHeightForWidth(mesh_edit_controller2->sizePolicy().hasHeightForWidth());
        mesh_edit_controller2->setSizePolicy(sizePolicy1);
        mesh_edit_controller2->setMinimumSize(QSize(150, 500));
        mesh_edit_controller2->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(mesh_edit_controller2);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ManualMeshMatch);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ManualMeshMatch);
    } // setupUi

    void retranslateUi(QWidget *ManualMeshMatch)
    {
        ManualMeshMatch->setWindowTitle(QApplication::translate("ManualMeshMatch", "ManualMeshMatch", 0, QApplication::UnicodeUTF8));
        pushButton_Open->setText(QApplication::translate("ManualMeshMatch", "\346\211\223\345\274\200", 0, QApplication::UnicodeUTF8));
        pushButton_Open_2->setText(QApplication::translate("ManualMeshMatch", "\346\211\223\345\274\2002", 0, QApplication::UnicodeUTF8));
        pushButton_Save->setText(QApplication::translate("ManualMeshMatch", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ManualMeshMatch", "\346\226\207\344\273\266\346\223\215\344\275\234", 0, QApplication::UnicodeUTF8));
        pushButton_Init_Matching->setText(QApplication::translate("ManualMeshMatch", "\345\210\235\345\247\213\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Match_Two_Chords->setText(QApplication::translate("ManualMeshMatch", "Chord\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Polyline->setText(QApplication::translate("ManualMeshMatch", "\350\216\267\345\276\227Polyline", 0, QApplication::UnicodeUTF8));
        pushButton_Check_Match->setText(QApplication::translate("ManualMeshMatch", "\345\214\271\351\205\215\346\243\200\346\265\213", 0, QApplication::UnicodeUTF8));
        pushButton_Auto_Match->setText(QApplication::translate("ManualMeshMatch", "\350\207\252\345\212\250\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ManualMeshMatch", "Matching\346\223\215\344\275\234", 0, QApplication::UnicodeUTF8));
        pushButton_Redraw_Matched_Chords->setText(QApplication::translate("ManualMeshMatch", "\351\207\215\347\273\230\345\214\271\351\205\215", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Chord_Pairs->setText(QApplication::translate("ManualMeshMatch", "\346\237\245\347\234\213\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Interface->setText(QApplication::translate("ManualMeshMatch", "\346\237\245\347\234\213\350\264\264\345\220\210\351\235\242", 0, QApplication::UnicodeUTF8));
        cb_Show_Left_Side->setText(QApplication::translate("ManualMeshMatch", "\347\234\213\345\267\246\350\276\271", 0, QApplication::UnicodeUTF8));
        cb_Show_Right_Side->setText(QApplication::translate("ManualMeshMatch", "\347\234\213\345\217\263\350\276\271", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("ManualMeshMatch", "\346\270\262\346\237\223\302\267\346\237\245\347\234\213", 0, QApplication::UnicodeUTF8));
        pushButton_Merge->setText(QApplication::translate("ManualMeshMatch", "\345\220\210\345\271\266", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("ManualMeshMatch", "\345\220\216\347\273\255\346\223\215\344\275\234", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ManualMeshMatch: public Ui_ManualMeshMatch {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUALMESHMATCH_H
