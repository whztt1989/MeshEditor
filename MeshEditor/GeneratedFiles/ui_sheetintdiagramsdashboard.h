/********************************************************************************
** Form generated from reading UI file 'sheetintdiagramsdashboard.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHEETINTDIAGRAMSDASHBOARD_H
#define UI_SHEETINTDIAGRAMSDASHBOARD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_SheetIntDiagramsDashboard
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;

    void setupUi(QDialog *SheetIntDiagramsDashboard)
    {
        if (SheetIntDiagramsDashboard->objectName().isEmpty())
            SheetIntDiagramsDashboard->setObjectName(QString::fromUtf8("SheetIntDiagramsDashboard"));
        SheetIntDiagramsDashboard->resize(889, 593);
        gridLayout_2 = new QGridLayout(SheetIntDiagramsDashboard);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        retranslateUi(SheetIntDiagramsDashboard);

        QMetaObject::connectSlotsByName(SheetIntDiagramsDashboard);
    } // setupUi

    void retranslateUi(QDialog *SheetIntDiagramsDashboard)
    {
        SheetIntDiagramsDashboard->setWindowTitle(QApplication::translate("SheetIntDiagramsDashboard", "SheetIntDiagramsDashboard", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SheetIntDiagramsDashboard: public Ui_SheetIntDiagramsDashboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHEETINTDIAGRAMSDASHBOARD_H
