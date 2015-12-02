/********************************************************************************
** Form generated from reading UI file 'filecontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILECONTROLWIDGET_H
#define UI_FILECONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileControlWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton_Open;
    QPushButton *pushButton_Save_As;
    QPushButton *pushButton_Save;
    QPushButton *pushButton_Close;

    void setupUi(QWidget *FileControlWidget)
    {
        if (FileControlWidget->objectName().isEmpty())
            FileControlWidget->setObjectName(QString::fromUtf8("FileControlWidget"));
        FileControlWidget->resize(94, 128);
        FileControlWidget->setMaximumSize(QSize(250, 210));
        gridLayout = new QGridLayout(FileControlWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_Open = new QPushButton(FileControlWidget);
        pushButton_Open->setObjectName(QString::fromUtf8("pushButton_Open"));

        gridLayout->addWidget(pushButton_Open, 0, 0, 1, 1);

        pushButton_Save_As = new QPushButton(FileControlWidget);
        pushButton_Save_As->setObjectName(QString::fromUtf8("pushButton_Save_As"));
        pushButton_Save_As->setEnabled(false);

        gridLayout->addWidget(pushButton_Save_As, 1, 0, 1, 1);

        pushButton_Save = new QPushButton(FileControlWidget);
        pushButton_Save->setObjectName(QString::fromUtf8("pushButton_Save"));
        pushButton_Save->setEnabled(false);

        gridLayout->addWidget(pushButton_Save, 2, 0, 1, 1);

        pushButton_Close = new QPushButton(FileControlWidget);
        pushButton_Close->setObjectName(QString::fromUtf8("pushButton_Close"));
        pushButton_Close->setEnabled(false);

        gridLayout->addWidget(pushButton_Close, 3, 0, 1, 1);


        retranslateUi(FileControlWidget);

        QMetaObject::connectSlotsByName(FileControlWidget);
    } // setupUi

    void retranslateUi(QWidget *FileControlWidget)
    {
        FileControlWidget->setWindowTitle(QApplication::translate("FileControlWidget", "FileControlWidget", 0, QApplication::UnicodeUTF8));
        pushButton_Open->setText(QApplication::translate("FileControlWidget", "\346\211\223\345\274\200", 0, QApplication::UnicodeUTF8));
        pushButton_Save_As->setText(QApplication::translate("FileControlWidget", "\345\217\246\345\255\230\344\270\272...", 0, QApplication::UnicodeUTF8));
        pushButton_Save->setText(QApplication::translate("FileControlWidget", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        pushButton_Close->setText(QApplication::translate("FileControlWidget", "\345\205\263\351\227\255", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FileControlWidget: public Ui_FileControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILECONTROLWIDGET_H
