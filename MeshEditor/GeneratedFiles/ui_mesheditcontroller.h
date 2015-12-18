/********************************************************************************
** Form generated from reading UI file 'mesheditcontroller.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHEDITCONTROLLER_H
#define UI_MESHEDITCONTROLLER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeshEditController
{
public:
    QVBoxLayout *verticalLayout;
    QToolBox *toolBox;
    QWidget *page_Basic_Operations;
    QVBoxLayout *verticalLayout_18;
    QPushButton *pb_Get_Int_Quads_For_Pre;
    QPushButton *pb_Adjust_Int_Quads_For_Pre;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_19;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_9;
    QCheckBox *cb_Show_Body_Vertices;
    QCheckBox *cb_Show_Body_Edges;
    QCheckBox *cb_Show_Body_Faces;
    QCheckBox *cb_Show_Interfaces;
    QFrame *line_3;
    QLabel *label;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *cb_Show_Bnd_Vertices;
    QCheckBox *cb_Show_Bnd_Edges;
    QCheckBox *cb_Show_Bnd_Faces;
    QPushButton *pb_Show_Group_Manager;
    QSpacerItem *verticalSpacer_16;
    QWidget *page_Sheet_Inflation;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *pb_Select_Edges_For_SI;
    QVBoxLayout *verticalLayout_13;
    QPushButton *pb_Read_Loop_Ehs;
    QPushButton *pb_Save_Loop_Ehs;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_15;
    QPushButton *pb_Direct_Get_Quad_Set;
    QPushButton *pb_PCS_Depth_Get_Quad_Set;
    QPushButton *pb_Sweep_Get_Quad_Set;
    QPushButton *pb_Determine_Shrink_Set;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_8;
    QPushButton *pushButton_Modify_Quad_Set_For_SI;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_Add_Quad_For_SI;
    QPushButton *pushButton_Remove_Quad_For_SI;
    QPushButton *pushButton_Modify_Quad_Set_OK_For_SI;
    QPushButton *pushButton_Sheet_Inflation_For_SI;
    QPushButton *pushButton_postprocess_for_SI;
    QSpacerItem *verticalSpacer;
    QWidget *page_Sheet_Extraction;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButton_Select_Edges_For_SE;
    QPushButton *pushButton_Rotate_For_SE;
    QPushButton *pushButton_Select_Edges_OK_For_SE;
    QPushButton *pushButton_Extract_Sheet_For_SE;
    QPushButton *pushButton_Postprocess_For_SE;
    QSpacerItem *verticalSpacer_2;
    QWidget *page_Column_Collapse;
    QVBoxLayout *verticalLayout_7;
    QRadioButton *radioButton_Direct_Select_For_CC;
    QRadioButton *radioButton_Select_By_Sheet_For_CC;
    QStackedWidget *stackedWidget;
    QWidget *page_6;
    QVBoxLayout *verticalLayout_6;
    QPushButton *pushButton_Select_Face_For_CC;
    QPushButton *pushButton_Rotate_1_For_CC;
    QPushButton *pushButton_Select_Face_OK_For_CC;
    QSpacerItem *verticalSpacer_6;
    QWidget *page_7;
    QVBoxLayout *verticalLayout_5;
    QPushButton *pushButton_Select_Edges_For_CC;
    QPushButton *pushButton_Rotate_2_For_CC;
    QPushButton *pushButton_Select_Edges_OK_For_CC;
    QSpacerItem *verticalSpacer_5;
    QFrame *line;
    QPushButton *pushButton_Select_Collapse_Vertices_For_CC;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton_Rotate_3_For_CC;
    QPushButton *pushButton_Continue_Select_Vertices_For_CC;
    QPushButton *pushButton_Select_Collapse_Vertices_OK_For_CC;
    QFrame *line_2;
    QPushButton *pushButton_Columns_Collapse_For_CC;
    QPushButton *pushButton_Postprocess_For_CC;
    QSpacerItem *verticalSpacer_7;
    QWidget *page_3;
    QVBoxLayout *verticalLayout_16;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_12;
    QPushButton *pushButton_Select_Edges_For_CO;
    QPushButton *pushButton_Select_Edges_OK_For_CO;
    QFrame *line_4;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_11;
    QLineEdit *le_Chord_Idx_For_Co;
    QPushButton *pb_Select_Chord_By_Idx_For_CO;
    QGroupBox *groupBox_5;
    QVBoxLayout *verticalLayout_14;
    QPushButton *pb_Select_Edge_For_Adjust_Chord;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *pb_Select_As_1st_Edge_For_Chord;
    QPushButton *pb_Select_As_2nd_Edge_For_Chord;
    QPushButton *pb_Show_Chord_Info_For_CO;
    QPushButton *pb_Clear_Selection_For_CO;
    QSpacerItem *verticalSpacer_3;
    QWidget *page_4;
    QVBoxLayout *verticalLayout_12;
    QRadioButton *radioButton_Smooth_Whole_For_MO;
    QRadioButton *radioButton_Pair_Geom_Node_For_MO;
    QRadioButton *radioButton_Smooth_Faces_For_MO;
    QStackedWidget *stackedWidget_Smooth_Methods;
    QWidget *page_13;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout;
    QSpinBox *spinBox_Smooth_Times;
    QPushButton *pushButton_Smooth;
    QSpacerItem *verticalSpacer_9;
    QWidget *page_14;
    QVBoxLayout *verticalLayout_10;
    QPushButton *pushButton_Init_For_Move_Node_For_MO;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *pushButton_Select_Node_For_Move_For_MO;
    QPushButton *pushButton_Cancel_Select_Node_For_Move_For_MO;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_Move_Node_For_MO;
    QPushButton *pushButton_Move_Node_OK_For_MO;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_Select_Node_Pair_For_MO;
    QPushButton *pushButton_Pair_Node_OK_For_MO;
    QSpacerItem *verticalSpacer_4;
    QWidget *page_15;
    QVBoxLayout *verticalLayout_11;
    QPushButton *pushButton_Select_Geom_Face_For_MO;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_Rotate_1_For_MO;
    QPushButton *pushButton_Continue_Select_Face_For_MO;
    QPushButton *pushButton_Smooth_Geom_Face_For_MO;
    QPushButton *pushButton_Clear_Select_Faces_For_MO;
    QSpacerItem *verticalSpacer_10;
    QWidget *page_16;
    QVBoxLayout *verticalLayout_17;
    QPushButton *pushButton_Select_Edges_For_CL;
    QPushButton *pushButton_Rotate_Camera_For_CL;
    QPushButton *pushButton_Select_Edges_OK_For_CL;
    QPushButton *pushButton_Select_Constraint_Faces;
    QPushButton *pushButton_2;
    QPushButton *pushButton_Cluster_For_CL;
    QSpacerItem *verticalSpacer_15;
    QButtonGroup *buttonGroup_2;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *MeshEditController)
    {
        if (MeshEditController->objectName().isEmpty())
            MeshEditController->setObjectName(QString::fromUtf8("MeshEditController"));
        MeshEditController->resize(165, 633);
        verticalLayout = new QVBoxLayout(MeshEditController);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolBox = new QToolBox(MeshEditController);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        page_Basic_Operations = new QWidget();
        page_Basic_Operations->setObjectName(QString::fromUtf8("page_Basic_Operations"));
        page_Basic_Operations->setGeometry(QRect(0, 0, 124, 234));
        verticalLayout_18 = new QVBoxLayout(page_Basic_Operations);
        verticalLayout_18->setSpacing(6);
        verticalLayout_18->setContentsMargins(0, 0, 0, 0);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        pb_Get_Int_Quads_For_Pre = new QPushButton(page_Basic_Operations);
        pb_Get_Int_Quads_For_Pre->setObjectName(QString::fromUtf8("pb_Get_Int_Quads_For_Pre"));

        verticalLayout_18->addWidget(pb_Get_Int_Quads_For_Pre);

        pb_Adjust_Int_Quads_For_Pre = new QPushButton(page_Basic_Operations);
        pb_Adjust_Int_Quads_For_Pre->setObjectName(QString::fromUtf8("pb_Adjust_Int_Quads_For_Pre"));

        verticalLayout_18->addWidget(pb_Adjust_Int_Quads_For_Pre);

        groupBox = new QGroupBox(page_Basic_Operations);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_19 = new QVBoxLayout(groupBox);
        verticalLayout_19->setSpacing(6);
        verticalLayout_19->setContentsMargins(11, 11, 11, 11);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_19->addWidget(label_2);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        cb_Show_Body_Vertices = new QCheckBox(groupBox);
        cb_Show_Body_Vertices->setObjectName(QString::fromUtf8("cb_Show_Body_Vertices"));
        cb_Show_Body_Vertices->setMaximumSize(QSize(30, 16777215));
        cb_Show_Body_Vertices->setChecked(false);

        horizontalLayout_9->addWidget(cb_Show_Body_Vertices);

        cb_Show_Body_Edges = new QCheckBox(groupBox);
        cb_Show_Body_Edges->setObjectName(QString::fromUtf8("cb_Show_Body_Edges"));
        cb_Show_Body_Edges->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_9->addWidget(cb_Show_Body_Edges);

        cb_Show_Body_Faces = new QCheckBox(groupBox);
        cb_Show_Body_Faces->setObjectName(QString::fromUtf8("cb_Show_Body_Faces"));
        cb_Show_Body_Faces->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_9->addWidget(cb_Show_Body_Faces);


        verticalLayout_19->addLayout(horizontalLayout_9);

        cb_Show_Interfaces = new QCheckBox(groupBox);
        cb_Show_Interfaces->setObjectName(QString::fromUtf8("cb_Show_Interfaces"));
        cb_Show_Interfaces->setChecked(true);

        verticalLayout_19->addWidget(cb_Show_Interfaces);

        line_3 = new QFrame(groupBox);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout_19->addWidget(line_3);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_19->addWidget(label);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        cb_Show_Bnd_Vertices = new QCheckBox(groupBox);
        cb_Show_Bnd_Vertices->setObjectName(QString::fromUtf8("cb_Show_Bnd_Vertices"));
        cb_Show_Bnd_Vertices->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_8->addWidget(cb_Show_Bnd_Vertices);

        cb_Show_Bnd_Edges = new QCheckBox(groupBox);
        cb_Show_Bnd_Edges->setObjectName(QString::fromUtf8("cb_Show_Bnd_Edges"));
        cb_Show_Bnd_Edges->setMaximumSize(QSize(30, 16777215));
        cb_Show_Bnd_Edges->setChecked(true);

        horizontalLayout_8->addWidget(cb_Show_Bnd_Edges);

        cb_Show_Bnd_Faces = new QCheckBox(groupBox);
        cb_Show_Bnd_Faces->setObjectName(QString::fromUtf8("cb_Show_Bnd_Faces"));
        cb_Show_Bnd_Faces->setMaximumSize(QSize(30, 16777215));
        cb_Show_Bnd_Faces->setChecked(true);

        horizontalLayout_8->addWidget(cb_Show_Bnd_Faces);


        verticalLayout_19->addLayout(horizontalLayout_8);


        verticalLayout_18->addWidget(groupBox);

        pb_Show_Group_Manager = new QPushButton(page_Basic_Operations);
        pb_Show_Group_Manager->setObjectName(QString::fromUtf8("pb_Show_Group_Manager"));

        verticalLayout_18->addWidget(pb_Show_Group_Manager);

        verticalSpacer_16 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_18->addItem(verticalSpacer_16);

        toolBox->addItem(page_Basic_Operations, QString::fromUtf8("\345\237\272\346\234\254\346\223\215\344\275\234"));
        page_Sheet_Inflation = new QWidget();
        page_Sheet_Inflation->setObjectName(QString::fromUtf8("page_Sheet_Inflation"));
        page_Sheet_Inflation->setGeometry(QRect(0, 0, 132, 402));
        verticalLayout_2 = new QVBoxLayout(page_Sheet_Inflation);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, -1, 0, -1);
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(3);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        pb_Select_Edges_For_SI = new QPushButton(page_Sheet_Inflation);
        pb_Select_Edges_For_SI->setObjectName(QString::fromUtf8("pb_Select_Edges_For_SI"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pb_Select_Edges_For_SI->sizePolicy().hasHeightForWidth());
        pb_Select_Edges_For_SI->setSizePolicy(sizePolicy);
        pb_Select_Edges_For_SI->setMaximumSize(QSize(70, 50));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        pb_Select_Edges_For_SI->setFont(font);

        horizontalLayout_10->addWidget(pb_Select_Edges_For_SI);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(1);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        pb_Read_Loop_Ehs = new QPushButton(page_Sheet_Inflation);
        pb_Read_Loop_Ehs->setObjectName(QString::fromUtf8("pb_Read_Loop_Ehs"));
        pb_Read_Loop_Ehs->setMaximumSize(QSize(55, 16777215));

        verticalLayout_13->addWidget(pb_Read_Loop_Ehs);

        pb_Save_Loop_Ehs = new QPushButton(page_Sheet_Inflation);
        pb_Save_Loop_Ehs->setObjectName(QString::fromUtf8("pb_Save_Loop_Ehs"));
        pb_Save_Loop_Ehs->setMaximumSize(QSize(55, 16777215));

        verticalLayout_13->addWidget(pb_Save_Loop_Ehs);


        horizontalLayout_10->addLayout(verticalLayout_13);


        verticalLayout_2->addLayout(horizontalLayout_10);

        groupBox_2 = new QGroupBox(page_Sheet_Inflation);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_15 = new QVBoxLayout(groupBox_2);
        verticalLayout_15->setSpacing(6);
        verticalLayout_15->setContentsMargins(11, 11, 11, 11);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        pb_Direct_Get_Quad_Set = new QPushButton(groupBox_2);
        pb_Direct_Get_Quad_Set->setObjectName(QString::fromUtf8("pb_Direct_Get_Quad_Set"));
        pb_Direct_Get_Quad_Set->setChecked(false);

        verticalLayout_15->addWidget(pb_Direct_Get_Quad_Set);

        pb_PCS_Depth_Get_Quad_Set = new QPushButton(groupBox_2);
        pb_PCS_Depth_Get_Quad_Set->setObjectName(QString::fromUtf8("pb_PCS_Depth_Get_Quad_Set"));

        verticalLayout_15->addWidget(pb_PCS_Depth_Get_Quad_Set);

        pb_Sweep_Get_Quad_Set = new QPushButton(groupBox_2);
        pb_Sweep_Get_Quad_Set->setObjectName(QString::fromUtf8("pb_Sweep_Get_Quad_Set"));

        verticalLayout_15->addWidget(pb_Sweep_Get_Quad_Set);


        verticalLayout_2->addWidget(groupBox_2);

        pb_Determine_Shrink_Set = new QPushButton(page_Sheet_Inflation);
        pb_Determine_Shrink_Set->setObjectName(QString::fromUtf8("pb_Determine_Shrink_Set"));

        verticalLayout_2->addWidget(pb_Determine_Shrink_Set);

        groupBox_3 = new QGroupBox(page_Sheet_Inflation);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_8 = new QVBoxLayout(groupBox_3);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        pushButton_Modify_Quad_Set_For_SI = new QPushButton(groupBox_3);
        pushButton_Modify_Quad_Set_For_SI->setObjectName(QString::fromUtf8("pushButton_Modify_Quad_Set_For_SI"));

        verticalLayout_8->addWidget(pushButton_Modify_Quad_Set_For_SI);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_Add_Quad_For_SI = new QPushButton(groupBox_3);
        pushButton_Add_Quad_For_SI->setObjectName(QString::fromUtf8("pushButton_Add_Quad_For_SI"));
        pushButton_Add_Quad_For_SI->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_2->addWidget(pushButton_Add_Quad_For_SI);

        pushButton_Remove_Quad_For_SI = new QPushButton(groupBox_3);
        pushButton_Remove_Quad_For_SI->setObjectName(QString::fromUtf8("pushButton_Remove_Quad_For_SI"));
        pushButton_Remove_Quad_For_SI->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_2->addWidget(pushButton_Remove_Quad_For_SI);


        verticalLayout_8->addLayout(horizontalLayout_2);

        pushButton_Modify_Quad_Set_OK_For_SI = new QPushButton(groupBox_3);
        pushButton_Modify_Quad_Set_OK_For_SI->setObjectName(QString::fromUtf8("pushButton_Modify_Quad_Set_OK_For_SI"));

        verticalLayout_8->addWidget(pushButton_Modify_Quad_Set_OK_For_SI);


        verticalLayout_2->addWidget(groupBox_3);

        pushButton_Sheet_Inflation_For_SI = new QPushButton(page_Sheet_Inflation);
        pushButton_Sheet_Inflation_For_SI->setObjectName(QString::fromUtf8("pushButton_Sheet_Inflation_For_SI"));

        verticalLayout_2->addWidget(pushButton_Sheet_Inflation_For_SI);

        pushButton_postprocess_for_SI = new QPushButton(page_Sheet_Inflation);
        pushButton_postprocess_for_SI->setObjectName(QString::fromUtf8("pushButton_postprocess_for_SI"));
        pushButton_postprocess_for_SI->setFont(font);
        pushButton_postprocess_for_SI->setStyleSheet(QString::fromUtf8("color: rgb(0, 85, 0);"));

        verticalLayout_2->addWidget(pushButton_postprocess_for_SI);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        toolBox->addItem(page_Sheet_Inflation, QString::fromUtf8("Sheet\347\224\237\346\210\220"));
        page_Sheet_Extraction = new QWidget();
        page_Sheet_Extraction->setObjectName(QString::fromUtf8("page_Sheet_Extraction"));
        page_Sheet_Extraction->setGeometry(QRect(0, 0, 98, 163));
        verticalLayout_3 = new QVBoxLayout(page_Sheet_Extraction);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, -1, 0, -1);
        pushButton_Select_Edges_For_SE = new QPushButton(page_Sheet_Extraction);
        pushButton_Select_Edges_For_SE->setObjectName(QString::fromUtf8("pushButton_Select_Edges_For_SE"));

        verticalLayout_3->addWidget(pushButton_Select_Edges_For_SE);

        pushButton_Rotate_For_SE = new QPushButton(page_Sheet_Extraction);
        pushButton_Rotate_For_SE->setObjectName(QString::fromUtf8("pushButton_Rotate_For_SE"));

        verticalLayout_3->addWidget(pushButton_Rotate_For_SE);

        pushButton_Select_Edges_OK_For_SE = new QPushButton(page_Sheet_Extraction);
        pushButton_Select_Edges_OK_For_SE->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_SE"));

        verticalLayout_3->addWidget(pushButton_Select_Edges_OK_For_SE);

        pushButton_Extract_Sheet_For_SE = new QPushButton(page_Sheet_Extraction);
        pushButton_Extract_Sheet_For_SE->setObjectName(QString::fromUtf8("pushButton_Extract_Sheet_For_SE"));

        verticalLayout_3->addWidget(pushButton_Extract_Sheet_For_SE);

        pushButton_Postprocess_For_SE = new QPushButton(page_Sheet_Extraction);
        pushButton_Postprocess_For_SE->setObjectName(QString::fromUtf8("pushButton_Postprocess_For_SE"));
        pushButton_Postprocess_For_SE->setFont(font);
        pushButton_Postprocess_For_SE->setStyleSheet(QString::fromUtf8("color: rgb(0, 85, 0);"));

        verticalLayout_3->addWidget(pushButton_Postprocess_For_SE);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        toolBox->addItem(page_Sheet_Extraction, QString::fromUtf8("Sheet\345\210\240\351\231\244"));
        page_Column_Collapse = new QWidget();
        page_Column_Collapse->setObjectName(QString::fromUtf8("page_Column_Collapse"));
        page_Column_Collapse->setGeometry(QRect(0, 0, 108, 340));
        verticalLayout_7 = new QVBoxLayout(page_Column_Collapse);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, -1, 0, -1);
        radioButton_Direct_Select_For_CC = new QRadioButton(page_Column_Collapse);
        buttonGroup = new QButtonGroup(MeshEditController);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(radioButton_Direct_Select_For_CC);
        radioButton_Direct_Select_For_CC->setObjectName(QString::fromUtf8("radioButton_Direct_Select_For_CC"));
        radioButton_Direct_Select_For_CC->setChecked(true);

        verticalLayout_7->addWidget(radioButton_Direct_Select_For_CC);

        radioButton_Select_By_Sheet_For_CC = new QRadioButton(page_Column_Collapse);
        buttonGroup->addButton(radioButton_Select_By_Sheet_For_CC);
        radioButton_Select_By_Sheet_For_CC->setObjectName(QString::fromUtf8("radioButton_Select_By_Sheet_For_CC"));

        verticalLayout_7->addWidget(radioButton_Select_By_Sheet_For_CC);

        stackedWidget = new QStackedWidget(page_Column_Collapse);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setMaximumSize(QSize(16777215, 110));
        stackedWidget->setFrameShape(QFrame::Box);
        stackedWidget->setFrameShadow(QFrame::Sunken);
        page_6 = new QWidget();
        page_6->setObjectName(QString::fromUtf8("page_6"));
        verticalLayout_6 = new QVBoxLayout(page_6);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        pushButton_Select_Face_For_CC = new QPushButton(page_6);
        pushButton_Select_Face_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Face_For_CC"));

        verticalLayout_6->addWidget(pushButton_Select_Face_For_CC);

        pushButton_Rotate_1_For_CC = new QPushButton(page_6);
        pushButton_Rotate_1_For_CC->setObjectName(QString::fromUtf8("pushButton_Rotate_1_For_CC"));

        verticalLayout_6->addWidget(pushButton_Rotate_1_For_CC);

        pushButton_Select_Face_OK_For_CC = new QPushButton(page_6);
        pushButton_Select_Face_OK_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Face_OK_For_CC"));

        verticalLayout_6->addWidget(pushButton_Select_Face_OK_For_CC);

        verticalSpacer_6 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_6);

        stackedWidget->addWidget(page_6);
        page_7 = new QWidget();
        page_7->setObjectName(QString::fromUtf8("page_7"));
        verticalLayout_5 = new QVBoxLayout(page_7);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        pushButton_Select_Edges_For_CC = new QPushButton(page_7);
        pushButton_Select_Edges_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Edges_For_CC"));

        verticalLayout_5->addWidget(pushButton_Select_Edges_For_CC);

        pushButton_Rotate_2_For_CC = new QPushButton(page_7);
        pushButton_Rotate_2_For_CC->setObjectName(QString::fromUtf8("pushButton_Rotate_2_For_CC"));

        verticalLayout_5->addWidget(pushButton_Rotate_2_For_CC);

        pushButton_Select_Edges_OK_For_CC = new QPushButton(page_7);
        pushButton_Select_Edges_OK_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_CC"));

        verticalLayout_5->addWidget(pushButton_Select_Edges_OK_For_CC);

        verticalSpacer_5 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_5);

        stackedWidget->addWidget(page_7);

        verticalLayout_7->addWidget(stackedWidget);

        line = new QFrame(page_Column_Collapse);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_7->addWidget(line);

        pushButton_Select_Collapse_Vertices_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Select_Collapse_Vertices_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Collapse_Vertices_For_CC"));

        verticalLayout_7->addWidget(pushButton_Select_Collapse_Vertices_For_CC);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        pushButton_Rotate_3_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Rotate_3_For_CC->setObjectName(QString::fromUtf8("pushButton_Rotate_3_For_CC"));
        pushButton_Rotate_3_For_CC->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_3->addWidget(pushButton_Rotate_3_For_CC);

        pushButton_Continue_Select_Vertices_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Continue_Select_Vertices_For_CC->setObjectName(QString::fromUtf8("pushButton_Continue_Select_Vertices_For_CC"));
        pushButton_Continue_Select_Vertices_For_CC->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_3->addWidget(pushButton_Continue_Select_Vertices_For_CC);


        verticalLayout_7->addLayout(horizontalLayout_3);

        pushButton_Select_Collapse_Vertices_OK_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Select_Collapse_Vertices_OK_For_CC->setObjectName(QString::fromUtf8("pushButton_Select_Collapse_Vertices_OK_For_CC"));

        verticalLayout_7->addWidget(pushButton_Select_Collapse_Vertices_OK_For_CC);

        line_2 = new QFrame(page_Column_Collapse);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_7->addWidget(line_2);

        pushButton_Columns_Collapse_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Columns_Collapse_For_CC->setObjectName(QString::fromUtf8("pushButton_Columns_Collapse_For_CC"));

        verticalLayout_7->addWidget(pushButton_Columns_Collapse_For_CC);

        pushButton_Postprocess_For_CC = new QPushButton(page_Column_Collapse);
        pushButton_Postprocess_For_CC->setObjectName(QString::fromUtf8("pushButton_Postprocess_For_CC"));
        pushButton_Postprocess_For_CC->setFont(font);
        pushButton_Postprocess_For_CC->setStyleSheet(QString::fromUtf8("color: rgb(0, 85, 0);"));

        verticalLayout_7->addWidget(pushButton_Postprocess_For_CC);

        verticalSpacer_7 = new QSpacerItem(20, 62, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_7);

        toolBox->addItem(page_Column_Collapse, QString::fromUtf8("Column\345\241\214\347\274\251"));
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        page_3->setGeometry(QRect(0, 0, 147, 433));
        verticalLayout_16 = new QVBoxLayout(page_3);
        verticalLayout_16->setSpacing(6);
        verticalLayout_16->setContentsMargins(11, 11, 11, 11);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        verticalLayout_16->setContentsMargins(0, -1, 0, -1);
        groupBox_4 = new QGroupBox(page_3);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        verticalLayout_4 = new QVBoxLayout(groupBox_4);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, -1, 0, -1);
        label_3 = new QLabel(groupBox_4);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        pushButton_Select_Edges_For_CO = new QPushButton(groupBox_4);
        pushButton_Select_Edges_For_CO->setObjectName(QString::fromUtf8("pushButton_Select_Edges_For_CO"));
        pushButton_Select_Edges_For_CO->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_12->addWidget(pushButton_Select_Edges_For_CO);

        pushButton_Select_Edges_OK_For_CO = new QPushButton(groupBox_4);
        pushButton_Select_Edges_OK_For_CO->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_CO"));
        pushButton_Select_Edges_OK_For_CO->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_12->addWidget(pushButton_Select_Edges_OK_For_CO);


        verticalLayout_4->addLayout(horizontalLayout_12);

        line_4 = new QFrame(groupBox_4);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line_4);

        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_4->addWidget(label_4);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        le_Chord_Idx_For_Co = new QLineEdit(groupBox_4);
        le_Chord_Idx_For_Co->setObjectName(QString::fromUtf8("le_Chord_Idx_For_Co"));

        horizontalLayout_11->addWidget(le_Chord_Idx_For_Co);

        pb_Select_Chord_By_Idx_For_CO = new QPushButton(groupBox_4);
        pb_Select_Chord_By_Idx_For_CO->setObjectName(QString::fromUtf8("pb_Select_Chord_By_Idx_For_CO"));
        pb_Select_Chord_By_Idx_For_CO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_11->addWidget(pb_Select_Chord_By_Idx_For_CO);


        verticalLayout_4->addLayout(horizontalLayout_11);


        verticalLayout_16->addWidget(groupBox_4);

        groupBox_5 = new QGroupBox(page_3);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        verticalLayout_14 = new QVBoxLayout(groupBox_5);
        verticalLayout_14->setSpacing(6);
        verticalLayout_14->setContentsMargins(11, 11, 11, 11);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        verticalLayout_14->setContentsMargins(0, -1, 0, -1);
        pb_Select_Edge_For_Adjust_Chord = new QPushButton(groupBox_5);
        pb_Select_Edge_For_Adjust_Chord->setObjectName(QString::fromUtf8("pb_Select_Edge_For_Adjust_Chord"));

        verticalLayout_14->addWidget(pb_Select_Edge_For_Adjust_Chord);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(0);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        pb_Select_As_1st_Edge_For_Chord = new QPushButton(groupBox_5);
        pb_Select_As_1st_Edge_For_Chord->setObjectName(QString::fromUtf8("pb_Select_As_1st_Edge_For_Chord"));
        pb_Select_As_1st_Edge_For_Chord->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_13->addWidget(pb_Select_As_1st_Edge_For_Chord);

        pb_Select_As_2nd_Edge_For_Chord = new QPushButton(groupBox_5);
        pb_Select_As_2nd_Edge_For_Chord->setObjectName(QString::fromUtf8("pb_Select_As_2nd_Edge_For_Chord"));
        pb_Select_As_2nd_Edge_For_Chord->setMaximumSize(QSize(60, 16777215));

        horizontalLayout_13->addWidget(pb_Select_As_2nd_Edge_For_Chord);


        verticalLayout_14->addLayout(horizontalLayout_13);


        verticalLayout_16->addWidget(groupBox_5);

        pb_Show_Chord_Info_For_CO = new QPushButton(page_3);
        pb_Show_Chord_Info_For_CO->setObjectName(QString::fromUtf8("pb_Show_Chord_Info_For_CO"));

        verticalLayout_16->addWidget(pb_Show_Chord_Info_For_CO);

        pb_Clear_Selection_For_CO = new QPushButton(page_3);
        pb_Clear_Selection_For_CO->setObjectName(QString::fromUtf8("pb_Clear_Selection_For_CO"));

        verticalLayout_16->addWidget(pb_Clear_Selection_For_CO);

        verticalSpacer_3 = new QSpacerItem(20, 355, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_16->addItem(verticalSpacer_3);

        toolBox->addItem(page_3, QString::fromUtf8("Chord\346\223\215\344\275\234"));
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        page_4->setGeometry(QRect(0, 0, 126, 224));
        verticalLayout_12 = new QVBoxLayout(page_4);
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setContentsMargins(11, 11, 11, 11);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        radioButton_Smooth_Whole_For_MO = new QRadioButton(page_4);
        buttonGroup_2 = new QButtonGroup(MeshEditController);
        buttonGroup_2->setObjectName(QString::fromUtf8("buttonGroup_2"));
        buttonGroup_2->addButton(radioButton_Smooth_Whole_For_MO);
        radioButton_Smooth_Whole_For_MO->setObjectName(QString::fromUtf8("radioButton_Smooth_Whole_For_MO"));
        radioButton_Smooth_Whole_For_MO->setChecked(true);

        verticalLayout_12->addWidget(radioButton_Smooth_Whole_For_MO);

        radioButton_Pair_Geom_Node_For_MO = new QRadioButton(page_4);
        buttonGroup_2->addButton(radioButton_Pair_Geom_Node_For_MO);
        radioButton_Pair_Geom_Node_For_MO->setObjectName(QString::fromUtf8("radioButton_Pair_Geom_Node_For_MO"));

        verticalLayout_12->addWidget(radioButton_Pair_Geom_Node_For_MO);

        radioButton_Smooth_Faces_For_MO = new QRadioButton(page_4);
        buttonGroup_2->addButton(radioButton_Smooth_Faces_For_MO);
        radioButton_Smooth_Faces_For_MO->setObjectName(QString::fromUtf8("radioButton_Smooth_Faces_For_MO"));

        verticalLayout_12->addWidget(radioButton_Smooth_Faces_For_MO);

        stackedWidget_Smooth_Methods = new QStackedWidget(page_4);
        stackedWidget_Smooth_Methods->setObjectName(QString::fromUtf8("stackedWidget_Smooth_Methods"));
        page_13 = new QWidget();
        page_13->setObjectName(QString::fromUtf8("page_13"));
        verticalLayout_9 = new QVBoxLayout(page_13);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, -1, 0, -1);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        spinBox_Smooth_Times = new QSpinBox(page_13);
        spinBox_Smooth_Times->setObjectName(QString::fromUtf8("spinBox_Smooth_Times"));
        spinBox_Smooth_Times->setValue(5);

        horizontalLayout->addWidget(spinBox_Smooth_Times);

        pushButton_Smooth = new QPushButton(page_13);
        pushButton_Smooth->setObjectName(QString::fromUtf8("pushButton_Smooth"));
        pushButton_Smooth->setMaximumSize(QSize(50, 16777215));

        horizontalLayout->addWidget(pushButton_Smooth);


        verticalLayout_9->addLayout(horizontalLayout);

        verticalSpacer_9 = new QSpacerItem(20, 140, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_9);

        stackedWidget_Smooth_Methods->addWidget(page_13);
        page_14 = new QWidget();
        page_14->setObjectName(QString::fromUtf8("page_14"));
        verticalLayout_10 = new QVBoxLayout(page_14);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, -1, 0, -1);
        pushButton_Init_For_Move_Node_For_MO = new QPushButton(page_14);
        pushButton_Init_For_Move_Node_For_MO->setObjectName(QString::fromUtf8("pushButton_Init_For_Move_Node_For_MO"));

        verticalLayout_10->addWidget(pushButton_Init_For_Move_Node_For_MO);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        pushButton_Select_Node_For_Move_For_MO = new QPushButton(page_14);
        pushButton_Select_Node_For_Move_For_MO->setObjectName(QString::fromUtf8("pushButton_Select_Node_For_Move_For_MO"));
        pushButton_Select_Node_For_Move_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_6->addWidget(pushButton_Select_Node_For_Move_For_MO);

        pushButton_Cancel_Select_Node_For_Move_For_MO = new QPushButton(page_14);
        pushButton_Cancel_Select_Node_For_Move_For_MO->setObjectName(QString::fromUtf8("pushButton_Cancel_Select_Node_For_Move_For_MO"));
        pushButton_Cancel_Select_Node_For_Move_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_6->addWidget(pushButton_Cancel_Select_Node_For_Move_For_MO);


        verticalLayout_10->addLayout(horizontalLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        pushButton_Move_Node_For_MO = new QPushButton(page_14);
        pushButton_Move_Node_For_MO->setObjectName(QString::fromUtf8("pushButton_Move_Node_For_MO"));
        pushButton_Move_Node_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(pushButton_Move_Node_For_MO);

        pushButton_Move_Node_OK_For_MO = new QPushButton(page_14);
        pushButton_Move_Node_OK_For_MO->setObjectName(QString::fromUtf8("pushButton_Move_Node_OK_For_MO"));
        pushButton_Move_Node_OK_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(pushButton_Move_Node_OK_For_MO);


        verticalLayout_10->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_Select_Node_Pair_For_MO = new QPushButton(page_14);
        pushButton_Select_Node_Pair_For_MO->setObjectName(QString::fromUtf8("pushButton_Select_Node_Pair_For_MO"));
        pushButton_Select_Node_Pair_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_4->addWidget(pushButton_Select_Node_Pair_For_MO);

        pushButton_Pair_Node_OK_For_MO = new QPushButton(page_14);
        pushButton_Pair_Node_OK_For_MO->setObjectName(QString::fromUtf8("pushButton_Pair_Node_OK_For_MO"));
        pushButton_Pair_Node_OK_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_4->addWidget(pushButton_Pair_Node_OK_For_MO);


        verticalLayout_10->addLayout(horizontalLayout_4);

        verticalSpacer_4 = new QSpacerItem(20, 216, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_4);

        stackedWidget_Smooth_Methods->addWidget(page_14);
        page_15 = new QWidget();
        page_15->setObjectName(QString::fromUtf8("page_15"));
        verticalLayout_11 = new QVBoxLayout(page_15);
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setContentsMargins(11, 11, 11, 11);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(0, -1, 0, -1);
        pushButton_Select_Geom_Face_For_MO = new QPushButton(page_15);
        pushButton_Select_Geom_Face_For_MO->setObjectName(QString::fromUtf8("pushButton_Select_Geom_Face_For_MO"));

        verticalLayout_11->addWidget(pushButton_Select_Geom_Face_For_MO);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_Rotate_1_For_MO = new QPushButton(page_15);
        pushButton_Rotate_1_For_MO->setObjectName(QString::fromUtf8("pushButton_Rotate_1_For_MO"));
        pushButton_Rotate_1_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_7->addWidget(pushButton_Rotate_1_For_MO);

        pushButton_Continue_Select_Face_For_MO = new QPushButton(page_15);
        pushButton_Continue_Select_Face_For_MO->setObjectName(QString::fromUtf8("pushButton_Continue_Select_Face_For_MO"));
        pushButton_Continue_Select_Face_For_MO->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_7->addWidget(pushButton_Continue_Select_Face_For_MO);


        verticalLayout_11->addLayout(horizontalLayout_7);

        pushButton_Smooth_Geom_Face_For_MO = new QPushButton(page_15);
        pushButton_Smooth_Geom_Face_For_MO->setObjectName(QString::fromUtf8("pushButton_Smooth_Geom_Face_For_MO"));

        verticalLayout_11->addWidget(pushButton_Smooth_Geom_Face_For_MO);

        pushButton_Clear_Select_Faces_For_MO = new QPushButton(page_15);
        pushButton_Clear_Select_Faces_For_MO->setObjectName(QString::fromUtf8("pushButton_Clear_Select_Faces_For_MO"));

        verticalLayout_11->addWidget(pushButton_Clear_Select_Faces_For_MO);

        verticalSpacer_10 = new QSpacerItem(20, 220, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_10);

        stackedWidget_Smooth_Methods->addWidget(page_15);

        verticalLayout_12->addWidget(stackedWidget_Smooth_Methods);

        toolBox->addItem(page_4, QString::fromUtf8("\347\275\221\346\240\274\344\274\230\345\214\226"));
        stackedWidget_Smooth_Methods->raise();
        radioButton_Smooth_Whole_For_MO->raise();
        radioButton_Pair_Geom_Node_For_MO->raise();
        radioButton_Smooth_Faces_For_MO->raise();
        page_16 = new QWidget();
        page_16->setObjectName(QString::fromUtf8("page_16"));
        page_16->setGeometry(QRect(0, 0, 110, 192));
        verticalLayout_17 = new QVBoxLayout(page_16);
        verticalLayout_17->setSpacing(6);
        verticalLayout_17->setContentsMargins(11, 11, 11, 11);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        pushButton_Select_Edges_For_CL = new QPushButton(page_16);
        pushButton_Select_Edges_For_CL->setObjectName(QString::fromUtf8("pushButton_Select_Edges_For_CL"));

        verticalLayout_17->addWidget(pushButton_Select_Edges_For_CL);

        pushButton_Rotate_Camera_For_CL = new QPushButton(page_16);
        pushButton_Rotate_Camera_For_CL->setObjectName(QString::fromUtf8("pushButton_Rotate_Camera_For_CL"));
        QFont font1;
        font1.setBold(true);
        font1.setUnderline(false);
        font1.setWeight(75);
        pushButton_Rotate_Camera_For_CL->setFont(font1);
        pushButton_Rotate_Camera_For_CL->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 255);"));

        verticalLayout_17->addWidget(pushButton_Rotate_Camera_For_CL);

        pushButton_Select_Edges_OK_For_CL = new QPushButton(page_16);
        pushButton_Select_Edges_OK_For_CL->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_CL"));

        verticalLayout_17->addWidget(pushButton_Select_Edges_OK_For_CL);

        pushButton_Select_Constraint_Faces = new QPushButton(page_16);
        pushButton_Select_Constraint_Faces->setObjectName(QString::fromUtf8("pushButton_Select_Constraint_Faces"));

        verticalLayout_17->addWidget(pushButton_Select_Constraint_Faces);

        pushButton_2 = new QPushButton(page_16);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        verticalLayout_17->addWidget(pushButton_2);

        pushButton_Cluster_For_CL = new QPushButton(page_16);
        pushButton_Cluster_For_CL->setObjectName(QString::fromUtf8("pushButton_Cluster_For_CL"));

        verticalLayout_17->addWidget(pushButton_Cluster_For_CL);

        verticalSpacer_15 = new QSpacerItem(20, 262, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_17->addItem(verticalSpacer_15);

        toolBox->addItem(page_16, QString::fromUtf8("\345\233\233\350\276\271\345\275\242\351\235\242\351\233\206"));

        verticalLayout->addWidget(toolBox);


        retranslateUi(MeshEditController);

        toolBox->setCurrentIndex(4);
        toolBox->layout()->setSpacing(6);
        stackedWidget->setCurrentIndex(0);
        stackedWidget_Smooth_Methods->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MeshEditController);
    } // setupUi

    void retranslateUi(QWidget *MeshEditController)
    {
        MeshEditController->setWindowTitle(QApplication::translate("MeshEditController", "MeshEditController", 0, QApplication::UnicodeUTF8));
        pb_Get_Int_Quads_For_Pre->setText(QApplication::translate("MeshEditController", "\350\216\267\345\276\227IntF\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pb_Adjust_Int_Quads_For_Pre->setText(QApplication::translate("MeshEditController", "\350\260\203\346\225\264IntF\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MeshEditController", "\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MeshEditController", "ACIS\346\250\241\345\236\213\357\274\232", 0, QApplication::UnicodeUTF8));
        cb_Show_Body_Vertices->setText(QApplication::translate("MeshEditController", "\347\202\271", 0, QApplication::UnicodeUTF8));
        cb_Show_Body_Edges->setText(QApplication::translate("MeshEditController", "\350\276\271", 0, QApplication::UnicodeUTF8));
        cb_Show_Body_Faces->setText(QApplication::translate("MeshEditController", "\351\235\242", 0, QApplication::UnicodeUTF8));
        cb_Show_Interfaces->setText(QApplication::translate("MeshEditController", "Interfaces", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MeshEditController", "\347\275\221\346\240\274\357\274\232", 0, QApplication::UnicodeUTF8));
        cb_Show_Bnd_Vertices->setText(QApplication::translate("MeshEditController", "\347\202\271", 0, QApplication::UnicodeUTF8));
        cb_Show_Bnd_Edges->setText(QApplication::translate("MeshEditController", "\350\276\271", 0, QApplication::UnicodeUTF8));
        cb_Show_Bnd_Faces->setText(QApplication::translate("MeshEditController", "\351\235\242", 0, QApplication::UnicodeUTF8));
        pb_Show_Group_Manager->setText(QApplication::translate("MeshEditController", "\346\230\276\347\244\272Group\347\256\241\347\220\206", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_Basic_Operations), QApplication::translate("MeshEditController", "\345\237\272\346\234\254\346\223\215\344\275\234", 0, QApplication::UnicodeUTF8));
        pb_Select_Edges_For_SI->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\n"
"\347\275\221\346\240\274\350\276\271", 0, QApplication::UnicodeUTF8));
        pb_Read_Loop_Ehs->setText(QApplication::translate("MeshEditController", "\350\257\273\345\217\226", 0, QApplication::UnicodeUTF8));
        pb_Save_Loop_Ehs->setText(QApplication::translate("MeshEditController", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MeshEditController", "Quad\351\233\206\350\216\267\345\217\226", 0, QApplication::UnicodeUTF8));
        pb_Direct_Get_Quad_Set->setText(QApplication::translate("MeshEditController", "\347\233\264\346\216\245\350\216\267\345\217\226", 0, QApplication::UnicodeUTF8));
        pb_PCS_Depth_Get_Quad_Set->setText(QApplication::translate("MeshEditController", "PCS\346\267\261\345\272\246\350\216\267\345\217\226", 0, QApplication::UnicodeUTF8));
        pb_Sweep_Get_Quad_Set->setText(QApplication::translate("MeshEditController", "\346\211\253\345\261\202\350\216\267\345\217\226", 0, QApplication::UnicodeUTF8));
        pb_Determine_Shrink_Set->setText(QApplication::translate("MeshEditController", "\347\241\256\345\256\232ShrinkSet", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MeshEditController", "Quad\351\233\206\344\277\256\346\224\271", 0, QApplication::UnicodeUTF8));
        pushButton_Modify_Quad_Set_For_SI->setText(QApplication::translate("MeshEditController", "\344\277\256\346\224\271\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Add_Quad_For_SI->setText(QApplication::translate("MeshEditController", "\345\242\236\345\212\240", 0, QApplication::UnicodeUTF8));
        pushButton_Remove_Quad_For_SI->setText(QApplication::translate("MeshEditController", "\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Modify_Quad_Set_OK_For_SI->setText(QApplication::translate("MeshEditController", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Sheet_Inflation_For_SI->setText(QApplication::translate("MeshEditController", "\347\224\237\346\210\220sheet", 0, QApplication::UnicodeUTF8));
        pushButton_postprocess_for_SI->setText(QApplication::translate("MeshEditController", "\345\220\216\347\273\255\345\244\204\347\220\206", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_Sheet_Inflation), QApplication::translate("MeshEditController", "Sheet\347\224\237\346\210\220", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_For_SE->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\350\276\271", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_For_SE->setText(QApplication::translate("MeshEditController", "\346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_SE->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Extract_Sheet_For_SE->setText(QApplication::translate("MeshEditController", "\345\210\240\351\231\244Sheet", 0, QApplication::UnicodeUTF8));
        pushButton_Postprocess_For_SE->setText(QApplication::translate("MeshEditController", "\345\220\216\347\273\255\345\244\204\347\220\206", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_Sheet_Extraction), QApplication::translate("MeshEditController", "Sheet\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        radioButton_Direct_Select_For_CC->setText(QApplication::translate("MeshEditController", "\347\233\264\346\216\245\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        radioButton_Select_By_Sheet_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\232\350\277\207sheet\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Face_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_1_For_CC->setText(QApplication::translate("MeshEditController", "\346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Face_OK_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\350\276\271", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_2_For_CC->setText(QApplication::translate("MeshEditController", "\346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Collapse_Vertices_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\345\220\210\345\271\266\347\202\271\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_3_For_CC->setText(QApplication::translate("MeshEditController", "\346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Continue_Select_Vertices_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Collapse_Vertices_OK_For_CC->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\202\271\345\257\271\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Columns_Collapse_For_CC->setText(QApplication::translate("MeshEditController", "Column Collapse", 0, QApplication::UnicodeUTF8));
        pushButton_Postprocess_For_CC->setText(QApplication::translate("MeshEditController", "\345\220\216\347\273\255\345\244\204\347\220\206", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_Column_Collapse), QApplication::translate("MeshEditController", "Column\345\241\214\347\274\251", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MeshEditController", "\351\200\211\346\213\251Chord", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MeshEditController", "\351\274\240\346\240\207\351\200\211\346\213\251\357\274\232", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_For_CO->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\350\276\271", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_CO->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MeshEditController", "\347\264\242\345\274\225\351\200\211\346\213\251\357\274\232", 0, QApplication::UnicodeUTF8));
        pb_Select_Chord_By_Idx_For_CO->setText(QApplication::translate("MeshEditController", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("MeshEditController", "\350\260\203\346\225\264\347\216\257\345\275\242chord\346\226\271\345\220\221", 0, QApplication::UnicodeUTF8));
        pb_Select_Edge_For_Adjust_Chord->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\350\276\271", 0, QApplication::UnicodeUTF8));
        pb_Select_As_1st_Edge_For_Chord->setText(QApplication::translate("MeshEditController", "\347\254\254\344\270\200\346\235\241\350\276\271", 0, QApplication::UnicodeUTF8));
        pb_Select_As_2nd_Edge_For_Chord->setText(QApplication::translate("MeshEditController", "\347\254\254\344\272\214\346\235\241\350\276\271", 0, QApplication::UnicodeUTF8));
        pb_Show_Chord_Info_For_CO->setText(QApplication::translate("MeshEditController", "\346\230\276\347\244\272chord\344\277\241\346\201\257", 0, QApplication::UnicodeUTF8));
        pb_Clear_Selection_For_CO->setText(QApplication::translate("MeshEditController", "\346\270\205\351\231\244\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_3), QApplication::translate("MeshEditController", "Chord\346\223\215\344\275\234", 0, QApplication::UnicodeUTF8));
        radioButton_Smooth_Whole_For_MO->setText(QApplication::translate("MeshEditController", "\345\205\250\345\261\200\344\274\230\345\214\226", 0, QApplication::UnicodeUTF8));
        radioButton_Pair_Geom_Node_For_MO->setText(QApplication::translate("MeshEditController", "\345\207\240\344\275\225\347\202\271\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        radioButton_Smooth_Faces_For_MO->setText(QApplication::translate("MeshEditController", "\351\235\242\344\274\230\345\214\226", 0, QApplication::UnicodeUTF8));
        pushButton_Smooth->setText(QApplication::translate("MeshEditController", "\345\205\211\351\241\272", 0, QApplication::UnicodeUTF8));
        pushButton_Init_For_Move_Node_For_MO->setText(QApplication::translate("MeshEditController", "\345\210\235\345\247\213\345\214\226", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Node_For_Move_For_MO->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel_Select_Node_For_Move_For_MO->setText(QApplication::translate("MeshEditController", "\345\217\226\346\266\210", 0, QApplication::UnicodeUTF8));
        pushButton_Move_Node_For_MO->setText(QApplication::translate("MeshEditController", "\347\247\273\345\212\250", 0, QApplication::UnicodeUTF8));
        pushButton_Move_Node_OK_For_MO->setText(QApplication::translate("MeshEditController", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Node_Pair_For_MO->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        pushButton_Pair_Node_OK_For_MO->setText(QApplication::translate("MeshEditController", "\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Geom_Face_For_MO->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\345\207\240\344\275\225\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_1_For_MO->setText(QApplication::translate("MeshEditController", "\346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Continue_Select_Face_For_MO->setText(QApplication::translate("MeshEditController", "\347\273\247\347\273\255", 0, QApplication::UnicodeUTF8));
        pushButton_Smooth_Geom_Face_For_MO->setText(QApplication::translate("MeshEditController", "\345\205\211\351\241\272", 0, QApplication::UnicodeUTF8));
        pushButton_Clear_Select_Faces_For_MO->setText(QApplication::translate("MeshEditController", "\346\270\205\351\231\244\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_4), QApplication::translate("MeshEditController", "\347\275\221\346\240\274\344\274\230\345\214\226", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_For_CL->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\350\276\271", 0, QApplication::UnicodeUTF8));
        pushButton_Rotate_Camera_For_CL->setText(QApplication::translate("MeshEditController", " \346\227\213\350\275\254", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_CL->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Constraint_Faces->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\272\246\346\235\237\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MeshEditController", "\351\200\211\346\213\251\347\272\246\346\235\237\351\235\242\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        pushButton_Cluster_For_CL->setText(QApplication::translate("MeshEditController", "\346\261\202\345\211\262", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_16), QApplication::translate("MeshEditController", "\345\233\233\350\276\271\345\275\242\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeshEditController: public Ui_MeshEditController {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHEDITCONTROLLER_H
