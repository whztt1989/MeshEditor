/********************************************************************************
** Form generated from reading UI file 'meshcut.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHCUT_H
#define UI_MESHCUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <hoopsview.h>

QT_BEGIN_NAMESPACE

class Ui_MeshCut
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QPushButton *pb_Load_Mesh_File_For_Cut;
    QPushButton *pb_Load_Cut_CAD_Model;
    QHBoxLayout *horizontalLayout_11;
    QPushButton *pb_Cut_Mesh_By_CAD_Model;
    QPushButton *pb_Revert_Cut_Mesh_By_CAD_Model;
    QPushButton *pb_Attach_And_Smooth_Cut_Mesh;
    QPushButton *pb_Save_Cut_Mesh;
    QSpacerItem *verticalSpacer;
    HoopsView *hoopsview1;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    HoopsView *hoopsview2;
    QLabel *label_2;
    HoopsView *hoopsview3;

    void setupUi(QWidget *MeshCut)
    {
        if (MeshCut->objectName().isEmpty())
            MeshCut->setObjectName(QString::fromUtf8("MeshCut"));
        MeshCut->resize(896, 566);
        horizontalLayout = new QHBoxLayout(MeshCut);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        frame = new QFrame(MeshCut);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMaximumSize(QSize(150, 16777215));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pb_Load_Mesh_File_For_Cut = new QPushButton(frame);
        pb_Load_Mesh_File_For_Cut->setObjectName(QString::fromUtf8("pb_Load_Mesh_File_For_Cut"));

        verticalLayout->addWidget(pb_Load_Mesh_File_For_Cut);

        pb_Load_Cut_CAD_Model = new QPushButton(frame);
        pb_Load_Cut_CAD_Model->setObjectName(QString::fromUtf8("pb_Load_Cut_CAD_Model"));

        verticalLayout->addWidget(pb_Load_Cut_CAD_Model);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        pb_Cut_Mesh_By_CAD_Model = new QPushButton(frame);
        pb_Cut_Mesh_By_CAD_Model->setObjectName(QString::fromUtf8("pb_Cut_Mesh_By_CAD_Model"));

        horizontalLayout_11->addWidget(pb_Cut_Mesh_By_CAD_Model);

        pb_Revert_Cut_Mesh_By_CAD_Model = new QPushButton(frame);
        pb_Revert_Cut_Mesh_By_CAD_Model->setObjectName(QString::fromUtf8("pb_Revert_Cut_Mesh_By_CAD_Model"));

        horizontalLayout_11->addWidget(pb_Revert_Cut_Mesh_By_CAD_Model);


        verticalLayout->addLayout(horizontalLayout_11);

        pb_Attach_And_Smooth_Cut_Mesh = new QPushButton(frame);
        pb_Attach_And_Smooth_Cut_Mesh->setObjectName(QString::fromUtf8("pb_Attach_And_Smooth_Cut_Mesh"));

        verticalLayout->addWidget(pb_Attach_And_Smooth_Cut_Mesh);

        pb_Save_Cut_Mesh = new QPushButton(frame);
        pb_Save_Cut_Mesh->setObjectName(QString::fromUtf8("pb_Save_Cut_Mesh"));

        verticalLayout->addWidget(pb_Save_Cut_Mesh);

        verticalSpacer = new QSpacerItem(20, 334, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(frame);

        hoopsview1 = new HoopsView(MeshCut);
        hoopsview1->setObjectName(QString::fromUtf8("hoopsview1"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(hoopsview1->sizePolicy().hasHeightForWidth());
        hoopsview1->setSizePolicy(sizePolicy);
        hoopsview1->setMinimumSize(QSize(250, 0));

        horizontalLayout->addWidget(hoopsview1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(MeshCut);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        hoopsview2 = new HoopsView(MeshCut);
        hoopsview2->setObjectName(QString::fromUtf8("hoopsview2"));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(hoopsview2->sizePolicy().hasHeightForWidth());
        hoopsview2->setSizePolicy(sizePolicy2);
        hoopsview2->setMinimumSize(QSize(250, 0));

        verticalLayout_2->addWidget(hoopsview2);

        label_2 = new QLabel(MeshCut);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setFont(font);

        verticalLayout_2->addWidget(label_2);

        hoopsview3 = new HoopsView(MeshCut);
        hoopsview3->setObjectName(QString::fromUtf8("hoopsview3"));
        sizePolicy2.setHeightForWidth(hoopsview3->sizePolicy().hasHeightForWidth());
        hoopsview3->setSizePolicy(sizePolicy2);
        hoopsview3->setMinimumSize(QSize(250, 0));

        verticalLayout_2->addWidget(hoopsview3);


        horizontalLayout->addLayout(verticalLayout_2);


        retranslateUi(MeshCut);

        QMetaObject::connectSlotsByName(MeshCut);
    } // setupUi

    void retranslateUi(QWidget *MeshCut)
    {
        MeshCut->setWindowTitle(QApplication::translate("MeshCut", "MeshCut", 0, QApplication::UnicodeUTF8));
        pb_Load_Mesh_File_For_Cut->setText(QApplication::translate("MeshCut", "Load Mesh File", 0, QApplication::UnicodeUTF8));
        pb_Load_Cut_CAD_Model->setText(QApplication::translate("MeshCut", "Load Cut CAD Model", 0, QApplication::UnicodeUTF8));
        pb_Cut_Mesh_By_CAD_Model->setText(QApplication::translate("MeshCut", "Cut Mesh", 0, QApplication::UnicodeUTF8));
        pb_Revert_Cut_Mesh_By_CAD_Model->setText(QApplication::translate("MeshCut", "Revert", 0, QApplication::UnicodeUTF8));
        pb_Attach_And_Smooth_Cut_Mesh->setText(QApplication::translate("MeshCut", "Attach && Smooth", 0, QApplication::UnicodeUTF8));
        pb_Save_Cut_Mesh->setText(QApplication::translate("MeshCut", "Save", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MeshCut", "Part for Matching:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MeshCut", "Part Left:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeshCut: public Ui_MeshCut {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHCUT_H
