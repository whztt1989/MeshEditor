/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *action_New_MeshEdit;
    QAction *action_Save;
    QAction *action_Save_As;
    QAction *action_Close;
    QAction *action_Exit;
    QAction *action_New_MeshMatch;
    QAction *action_New_ManualMeshMatch;
    QAction *action_New_MeshCut;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_4;
    QMenu *menu_2;
    QMenu *menu_3;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
        MainWindowClass->resize(1154, 871);
        action_New_MeshEdit = new QAction(MainWindowClass);
        action_New_MeshEdit->setObjectName(QString::fromUtf8("action_New_MeshEdit"));
        action_Save = new QAction(MainWindowClass);
        action_Save->setObjectName(QString::fromUtf8("action_Save"));
        action_Save_As = new QAction(MainWindowClass);
        action_Save_As->setObjectName(QString::fromUtf8("action_Save_As"));
        action_Close = new QAction(MainWindowClass);
        action_Close->setObjectName(QString::fromUtf8("action_Close"));
        action_Exit = new QAction(MainWindowClass);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        action_New_MeshMatch = new QAction(MainWindowClass);
        action_New_MeshMatch->setObjectName(QString::fromUtf8("action_New_MeshMatch"));
        action_New_ManualMeshMatch = new QAction(MainWindowClass);
        action_New_ManualMeshMatch->setObjectName(QString::fromUtf8("action_New_ManualMeshMatch"));
        action_New_MeshCut = new QAction(MainWindowClass);
        action_New_MeshCut->setObjectName(QString::fromUtf8("action_New_MeshCut"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, 0, 0);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));

        verticalLayout->addWidget(tabWidget);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1154, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_4 = new QMenu(menu);
        menu_4->setObjectName(QString::fromUtf8("menu_4"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        MainWindowClass->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menu->addAction(menu_4->menuAction());
        menu->addSeparator();
        menu->addAction(action_Close);
        menu->addAction(action_Exit);
        menu_4->addAction(action_New_MeshEdit);
        menu_4->addAction(action_New_ManualMeshMatch);
        menu_4->addAction(action_New_MeshCut);

        retranslateUi(MainWindowClass);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "MeshEditor\347\275\221\346\240\274\347\274\226\350\276\221", 0, QApplication::UnicodeUTF8));
        action_New_MeshEdit->setText(QApplication::translate("MainWindowClass", "MeshEdit\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
        action_Save->setText(QApplication::translate("MainWindowClass", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        action_Save_As->setText(QApplication::translate("MainWindowClass", "\345\217\246\345\255\230\344\270\272...", 0, QApplication::UnicodeUTF8));
        action_Close->setText(QApplication::translate("MainWindowClass", "\345\205\263\351\227\255", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("MainWindowClass", "\351\200\200\345\207\272", 0, QApplication::UnicodeUTF8));
        action_New_MeshMatch->setText(QApplication::translate("MainWindowClass", "MeshMatch\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
        action_New_ManualMeshMatch->setText(QApplication::translate("MainWindowClass", "MeshMatch\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
        action_New_MeshCut->setText(QApplication::translate("MainWindowClass", "MeshCut\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindowClass", "\346\226\207\344\273\266", 0, QApplication::UnicodeUTF8));
        menu_4->setTitle(QApplication::translate("MainWindowClass", "\346\226\260\345\273\272", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("MainWindowClass", "\350\247\206\345\233\276", 0, QApplication::UnicodeUTF8));
        menu_3->setTitle(QApplication::translate("MainWindowClass", "\345\270\256\345\212\251", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
