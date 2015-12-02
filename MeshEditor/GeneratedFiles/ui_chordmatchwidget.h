/********************************************************************************
** Form generated from reading UI file 'chordmatchwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHORDMATCHWIDGET_H
#define UI_CHORDMATCHWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChordMatchWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget_Chord_Pairs;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Delete_Pair;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *ChordMatchWidget)
    {
        if (ChordMatchWidget->objectName().isEmpty())
            ChordMatchWidget->setObjectName(QString::fromUtf8("ChordMatchWidget"));
        ChordMatchWidget->resize(198, 495);
        verticalLayout = new QVBoxLayout(ChordMatchWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listWidget_Chord_Pairs = new QListWidget(ChordMatchWidget);
        listWidget_Chord_Pairs->setObjectName(QString::fromUtf8("listWidget_Chord_Pairs"));

        horizontalLayout->addWidget(listWidget_Chord_Pairs);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_Delete_Pair = new QPushButton(ChordMatchWidget);
        pushButton_Delete_Pair->setObjectName(QString::fromUtf8("pushButton_Delete_Pair"));

        horizontalLayout_2->addWidget(pushButton_Delete_Pair);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(ChordMatchWidget);

        QMetaObject::connectSlotsByName(ChordMatchWidget);
    } // setupUi

    void retranslateUi(QWidget *ChordMatchWidget)
    {
        ChordMatchWidget->setWindowTitle(QApplication::translate("ChordMatchWidget", "ChordMatchWidget", 0, QApplication::UnicodeUTF8));
        pushButton_Delete_Pair->setText(QApplication::translate("ChordMatchWidget", "\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ChordMatchWidget: public Ui_ChordMatchWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHORDMATCHWIDGET_H
