/********************************************************************************
** Form generated from reading UI file 'mesheditwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHEDITWIDGET_H
#define UI_MESHEDITWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <hoopsview.h>

QT_BEGIN_NAMESPACE

class Ui_MeshEditWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QToolBox *toolBox;
    QWidget *page_3;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_Select_Edges_OK_For_Auto_LSI;
    QPushButton *pushButton_Get_Surf_Patches_For_Auto_LSI;
    QPushButton *pushButton_Set_Depth_Control_For_Auto_LSI;
    QPushButton *pushButton_Select_Constant_Faces_OK_For_Auto_LSI;
    QPushButton *pushButton_Get_Hexa_Set;
    QPushButton *pushButton_Get_Quad_Set_For_Auto_LSI;
    QPushButton *pushButton_Sheet_Inflate_For_Auto_LSI;
    QPushButton *pushButton_Reprocess_For_Auto_LSI;
    QSpacerItem *verticalSpacer;
    QWidget *page_5;
    QVBoxLayout *verticalLayout_10;
    QPushButton *pushButton_Select_Sheet_For_LSE;
    QPushButton *pushButton_Show_Int_Diagram_For_LSE;
    QPushButton *pushButton_Select_Interface_Faces_For_LSE;
    QPushButton *pushButton_Select_Constant_Faces_For_LSE;
    QSpinBox *spinBox_Extract_Depth;
    QPushButton *pushButton_Sep_Hex_mesh;
    QPushButton *pushButton_Extract_All_For_LSE;
    QPushButton *pushButton_Local_Inflation_For_LSE;
    QSpacerItem *verticalSpacer_9;
    QWidget *page_6;
    QSpinBox *spinBox_Depth_For_LC;
    QPushButton *pushButton_Set_Min_Depth_For_LC;
    QPushButton *pushButton_Non_Int_Loop_Connect_For_LC;
    QPushButton *pushButton_Search_Intersect_Path_For_LC;
    QPushButton *pushButton_Analyze_For_LC;
    QPushButton *pushButton_Int_Loop_Connect_For_LC;
    QWidget *page_8;
    QVBoxLayout *verticalLayout_5;
    QStackedWidget *stackedWidget_for_QS;
    QWidget *page_9;
    QVBoxLayout *verticalLayout_6;
    QPushButton *pushButton_Read_Data_For_QS;
    QPushButton *pushButton_Select_Edges_OK_For_QS;
    QPushButton *pushButton_Get_QS_By_PCS_For_QS;
    QPushButton *pushButton_Get_QS_By_Sweeping_For_QS;
    QPushButton *pushButton_Select_Constrait_Faces_OK_For_QS;
    QPushButton *pushButton_Pair_Int_Vhs_For_QS;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_Close_Loop_For_QS;
    QPushButton *pushButton_Append_Loop_For_QS;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_Get_Quad_Set_For_QS;
    QPushButton *pushButton_Optimize_Quad_Set_For_QS;
    QPushButton *pushButton_Optimize_Once_For_QS;
    QPushButton *pushButton_Save_Data_For_QS;
    QPushButton *pushButton_Inflate_Sheet_For_QS;
    QPushButton *pushButton_Postprocess_For_QS;
    QPushButton *pushButton_Show_Int_Diagram_For_QS;
    QSpacerItem *verticalSpacer_5;
    QWidget *page_10;
    QVBoxLayout *verticalLayout_7;
    QPushButton *pushButton_Add_Hexs_For_QS;
    QPushButton *pushButton_Current_Quality_For_QS;
    QPushButton *pushButton_Remove_Hexs_For_QS;
    QPushButton *pushButton_Modify_Quad_Set_OK_For_QS;
    QSpacerItem *verticalSpacer_6;
    QWidget *page_12;
    QVBoxLayout *verticalLayout_9;
    QPushButton *pushButton_Pair_One_Int_Vhs_OK_for_QS;
    QPushButton *pushButton_Next_Int_Edge_For_QS;
    QPushButton *pushButton_Select_Int_Edges_OK_For_QS;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *comboBox_cross_hexa_idx;
    QPushButton *pushButton_select_cross_hexa_OK;
    QPushButton *pushButton_Pair_Int_Vhs_Return_For_QS;
    QSpacerItem *verticalSpacer_8;
    QWidget *page_4;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QLineEdit *lineEdit_Vertices;
    QLabel *label_2;
    QLineEdit *lineEdit_Edges;
    QLabel *label_5;
    QLineEdit *lineEdit_Faces;
    QLabel *label_6;
    QLineEdit *lineEdit_Hexas;
    QPushButton *pushButton_Highlight_Mesh_Elements;
    QPushButton *pushButton_Clear_Highlight_Mesh_Elements;
    QSpacerItem *verticalSpacer_4;
    QWidget *page_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_7;
    QPushButton *pushButton_Select_Geom_Face;
    QPushButton *pushButton_Smooth_Faces;
    QPushButton *pushButton_Clear_Selected_Faces;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *spinBox_Smooth_Rounds;
    QPushButton *pushButton_Start_Smoothing;
    QSpacerItem *verticalSpacer_3;
    QWidget *page_7;
    QSpinBox *spinBox_Depth_For_DC;
    QPushButton *pushButton_Set_Depth_For_DC;
    QPushButton *pushButton_Depth_Cut_For_DC;
    QPushButton *pushButton_Save_For_DC;
    QPushButton *pushButton_Set_Start_Face_For_DC;
    QPushButton *pushButton_Create_Cut_Face_For_DC;
    QWidget *page_11;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_group_color;
    QPushButton *pushButton_Set_Color_For_PD;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_Line_Weight;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBox_Cell_Ratio;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_Add_Element_For_PD;
    QPushButton *pushButton_Remove_Element_For_PD;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *pushButton_Add_Cells_For_PD;
    QPushButton *pushButton_Remove_Cells_For_PD;
    QPushButton *pushButton_Append_Edges_on_Faces_For_PD;
    QPushButton *pushButton_Add_Element_Group_For_PD;
    QPushButton *pushButton_Recursive_Spreading;
    QPushButton *pushButton_Loop_Sep;
    QPushButton *pushButton_Get_st_Node;
    QPushButton *pushButton_Finally_Sep;
    QPushButton *pushButton_Sep_Hex_Seeds;
    QPushButton *pushButton_Int_Sep_Hex_Sets;
    QPushButton *pushButton_Get_Init_Quad_Set_For_PD;
    QPushButton *pushButton_Algo_Test_For_PD;
    QPushButton *pushButton_Show_Sheet_Edge_Face_For_PD;
    QPushButton *pushButton_Turn_To_Inflation_For_PD;
    QSpacerItem *verticalSpacer_7;
    QWidget *page;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_Test1;
    QPushButton *pushButton_Test2;
    QSpacerItem *verticalSpacer_2;
    HoopsView *hoopsview;

    void setupUi(QWidget *MeshEditWidget)
    {
        if (MeshEditWidget->objectName().isEmpty())
            MeshEditWidget->setObjectName(QString::fromUtf8("MeshEditWidget"));
        MeshEditWidget->resize(830, 781);
        horizontalLayout = new QHBoxLayout(MeshEditWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        toolBox = new QToolBox(MeshEditWidget);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolBox->sizePolicy().hasHeightForWidth());
        toolBox->setSizePolicy(sizePolicy);
        toolBox->setMaximumSize(QSize(120, 16777215));
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        page_3->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout = new QVBoxLayout(page_3);
        verticalLayout->setSpacing(3);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButton_Select_Edges_OK_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Select_Edges_OK_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_Auto_LSI"));
        pushButton_Select_Edges_OK_For_Auto_LSI->setEnabled(true);

        verticalLayout->addWidget(pushButton_Select_Edges_OK_For_Auto_LSI);

        pushButton_Get_Surf_Patches_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Get_Surf_Patches_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Get_Surf_Patches_For_Auto_LSI"));

        verticalLayout->addWidget(pushButton_Get_Surf_Patches_For_Auto_LSI);

        pushButton_Set_Depth_Control_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Set_Depth_Control_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Set_Depth_Control_For_Auto_LSI"));
        pushButton_Set_Depth_Control_For_Auto_LSI->setEnabled(false);

        verticalLayout->addWidget(pushButton_Set_Depth_Control_For_Auto_LSI);

        pushButton_Select_Constant_Faces_OK_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Select_Constant_Faces_OK_For_Auto_LSI"));
        pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setEnabled(false);

        verticalLayout->addWidget(pushButton_Select_Constant_Faces_OK_For_Auto_LSI);

        pushButton_Get_Hexa_Set = new QPushButton(page_3);
        pushButton_Get_Hexa_Set->setObjectName(QString::fromUtf8("pushButton_Get_Hexa_Set"));
        pushButton_Get_Hexa_Set->setEnabled(false);

        verticalLayout->addWidget(pushButton_Get_Hexa_Set);

        pushButton_Get_Quad_Set_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Get_Quad_Set_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Get_Quad_Set_For_Auto_LSI"));
        pushButton_Get_Quad_Set_For_Auto_LSI->setEnabled(false);

        verticalLayout->addWidget(pushButton_Get_Quad_Set_For_Auto_LSI);

        pushButton_Sheet_Inflate_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Sheet_Inflate_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Sheet_Inflate_For_Auto_LSI"));
        pushButton_Sheet_Inflate_For_Auto_LSI->setEnabled(false);

        verticalLayout->addWidget(pushButton_Sheet_Inflate_For_Auto_LSI);

        pushButton_Reprocess_For_Auto_LSI = new QPushButton(page_3);
        pushButton_Reprocess_For_Auto_LSI->setObjectName(QString::fromUtf8("pushButton_Reprocess_For_Auto_LSI"));
        pushButton_Reprocess_For_Auto_LSI->setEnabled(false);

        verticalLayout->addWidget(pushButton_Reprocess_For_Auto_LSI);

        verticalSpacer = new QSpacerItem(20, 316, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        toolBox->addItem(page_3, QString::fromUtf8("Sheet\347\224\237\346\210\220"));
        page_5 = new QWidget();
        page_5->setObjectName(QString::fromUtf8("page_5"));
        page_5->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_10 = new QVBoxLayout(page_5);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        pushButton_Select_Sheet_For_LSE = new QPushButton(page_5);
        pushButton_Select_Sheet_For_LSE->setObjectName(QString::fromUtf8("pushButton_Select_Sheet_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Select_Sheet_For_LSE);

        pushButton_Show_Int_Diagram_For_LSE = new QPushButton(page_5);
        pushButton_Show_Int_Diagram_For_LSE->setObjectName(QString::fromUtf8("pushButton_Show_Int_Diagram_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Show_Int_Diagram_For_LSE);

        pushButton_Select_Interface_Faces_For_LSE = new QPushButton(page_5);
        pushButton_Select_Interface_Faces_For_LSE->setObjectName(QString::fromUtf8("pushButton_Select_Interface_Faces_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Select_Interface_Faces_For_LSE);

        pushButton_Select_Constant_Faces_For_LSE = new QPushButton(page_5);
        pushButton_Select_Constant_Faces_For_LSE->setObjectName(QString::fromUtf8("pushButton_Select_Constant_Faces_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Select_Constant_Faces_For_LSE);

        spinBox_Extract_Depth = new QSpinBox(page_5);
        spinBox_Extract_Depth->setObjectName(QString::fromUtf8("spinBox_Extract_Depth"));
        spinBox_Extract_Depth->setValue(1);

        verticalLayout_10->addWidget(spinBox_Extract_Depth);

        pushButton_Sep_Hex_mesh = new QPushButton(page_5);
        pushButton_Sep_Hex_mesh->setObjectName(QString::fromUtf8("pushButton_Sep_Hex_mesh"));

        verticalLayout_10->addWidget(pushButton_Sep_Hex_mesh);

        pushButton_Extract_All_For_LSE = new QPushButton(page_5);
        pushButton_Extract_All_For_LSE->setObjectName(QString::fromUtf8("pushButton_Extract_All_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Extract_All_For_LSE);

        pushButton_Local_Inflation_For_LSE = new QPushButton(page_5);
        pushButton_Local_Inflation_For_LSE->setObjectName(QString::fromUtf8("pushButton_Local_Inflation_For_LSE"));

        verticalLayout_10->addWidget(pushButton_Local_Inflation_For_LSE);

        verticalSpacer_9 = new QSpacerItem(20, 195, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_10->addItem(verticalSpacer_9);

        toolBox->addItem(page_5, QString::fromUtf8("Sheet\345\210\240\351\231\244"));
        page_6 = new QWidget();
        page_6->setObjectName(QString::fromUtf8("page_6"));
        page_6->setGeometry(QRect(0, 0, 120, 529));
        spinBox_Depth_For_LC = new QSpinBox(page_6);
        spinBox_Depth_For_LC->setObjectName(QString::fromUtf8("spinBox_Depth_For_LC"));
        spinBox_Depth_For_LC->setGeometry(QRect(10, 40, 51, 22));
        spinBox_Depth_For_LC->setValue(2);
        pushButton_Set_Min_Depth_For_LC = new QPushButton(page_6);
        pushButton_Set_Min_Depth_For_LC->setObjectName(QString::fromUtf8("pushButton_Set_Min_Depth_For_LC"));
        pushButton_Set_Min_Depth_For_LC->setGeometry(QRect(70, 40, 41, 23));
        pushButton_Non_Int_Loop_Connect_For_LC = new QPushButton(page_6);
        pushButton_Non_Int_Loop_Connect_For_LC->setObjectName(QString::fromUtf8("pushButton_Non_Int_Loop_Connect_For_LC"));
        pushButton_Non_Int_Loop_Connect_For_LC->setGeometry(QRect(10, 100, 101, 23));
        pushButton_Search_Intersect_Path_For_LC = new QPushButton(page_6);
        pushButton_Search_Intersect_Path_For_LC->setObjectName(QString::fromUtf8("pushButton_Search_Intersect_Path_For_LC"));
        pushButton_Search_Intersect_Path_For_LC->setGeometry(QRect(10, 70, 101, 23));
        pushButton_Analyze_For_LC = new QPushButton(page_6);
        pushButton_Analyze_For_LC->setObjectName(QString::fromUtf8("pushButton_Analyze_For_LC"));
        pushButton_Analyze_For_LC->setGeometry(QRect(10, 10, 101, 23));
        pushButton_Int_Loop_Connect_For_LC = new QPushButton(page_6);
        pushButton_Int_Loop_Connect_For_LC->setObjectName(QString::fromUtf8("pushButton_Int_Loop_Connect_For_LC"));
        pushButton_Int_Loop_Connect_For_LC->setGeometry(QRect(10, 130, 101, 23));
        toolBox->addItem(page_6, QString::fromUtf8("loop\350\241\245\345\205\250"));
        page_8 = new QWidget();
        page_8->setObjectName(QString::fromUtf8("page_8"));
        page_8->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_5 = new QVBoxLayout(page_8);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        stackedWidget_for_QS = new QStackedWidget(page_8);
        stackedWidget_for_QS->setObjectName(QString::fromUtf8("stackedWidget_for_QS"));
        page_9 = new QWidget();
        page_9->setObjectName(QString::fromUtf8("page_9"));
        verticalLayout_6 = new QVBoxLayout(page_9);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        pushButton_Read_Data_For_QS = new QPushButton(page_9);
        pushButton_Read_Data_For_QS->setObjectName(QString::fromUtf8("pushButton_Read_Data_For_QS"));

        verticalLayout_6->addWidget(pushButton_Read_Data_For_QS);

        pushButton_Select_Edges_OK_For_QS = new QPushButton(page_9);
        pushButton_Select_Edges_OK_For_QS->setObjectName(QString::fromUtf8("pushButton_Select_Edges_OK_For_QS"));
        pushButton_Select_Edges_OK_For_QS->setEnabled(true);

        verticalLayout_6->addWidget(pushButton_Select_Edges_OK_For_QS);

        pushButton_Get_QS_By_PCS_For_QS = new QPushButton(page_9);
        pushButton_Get_QS_By_PCS_For_QS->setObjectName(QString::fromUtf8("pushButton_Get_QS_By_PCS_For_QS"));

        verticalLayout_6->addWidget(pushButton_Get_QS_By_PCS_For_QS);

        pushButton_Get_QS_By_Sweeping_For_QS = new QPushButton(page_9);
        pushButton_Get_QS_By_Sweeping_For_QS->setObjectName(QString::fromUtf8("pushButton_Get_QS_By_Sweeping_For_QS"));

        verticalLayout_6->addWidget(pushButton_Get_QS_By_Sweeping_For_QS);

        pushButton_Select_Constrait_Faces_OK_For_QS = new QPushButton(page_9);
        pushButton_Select_Constrait_Faces_OK_For_QS->setObjectName(QString::fromUtf8("pushButton_Select_Constrait_Faces_OK_For_QS"));

        verticalLayout_6->addWidget(pushButton_Select_Constrait_Faces_OK_For_QS);

        pushButton_Pair_Int_Vhs_For_QS = new QPushButton(page_9);
        pushButton_Pair_Int_Vhs_For_QS->setObjectName(QString::fromUtf8("pushButton_Pair_Int_Vhs_For_QS"));

        verticalLayout_6->addWidget(pushButton_Pair_Int_Vhs_For_QS);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_Close_Loop_For_QS = new QPushButton(page_9);
        pushButton_Close_Loop_For_QS->setObjectName(QString::fromUtf8("pushButton_Close_Loop_For_QS"));
        pushButton_Close_Loop_For_QS->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_4->addWidget(pushButton_Close_Loop_For_QS);

        pushButton_Append_Loop_For_QS = new QPushButton(page_9);
        pushButton_Append_Loop_For_QS->setObjectName(QString::fromUtf8("pushButton_Append_Loop_For_QS"));
        pushButton_Append_Loop_For_QS->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_4->addWidget(pushButton_Append_Loop_For_QS);


        verticalLayout_6->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        pushButton_Get_Quad_Set_For_QS = new QPushButton(page_9);
        pushButton_Get_Quad_Set_For_QS->setObjectName(QString::fromUtf8("pushButton_Get_Quad_Set_For_QS"));
        pushButton_Get_Quad_Set_For_QS->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(pushButton_Get_Quad_Set_For_QS);

        pushButton_Optimize_Quad_Set_For_QS = new QPushButton(page_9);
        pushButton_Optimize_Quad_Set_For_QS->setObjectName(QString::fromUtf8("pushButton_Optimize_Quad_Set_For_QS"));
        pushButton_Optimize_Quad_Set_For_QS->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_5->addWidget(pushButton_Optimize_Quad_Set_For_QS);


        verticalLayout_6->addLayout(horizontalLayout_5);

        pushButton_Optimize_Once_For_QS = new QPushButton(page_9);
        pushButton_Optimize_Once_For_QS->setObjectName(QString::fromUtf8("pushButton_Optimize_Once_For_QS"));

        verticalLayout_6->addWidget(pushButton_Optimize_Once_For_QS);

        pushButton_Save_Data_For_QS = new QPushButton(page_9);
        pushButton_Save_Data_For_QS->setObjectName(QString::fromUtf8("pushButton_Save_Data_For_QS"));

        verticalLayout_6->addWidget(pushButton_Save_Data_For_QS);

        pushButton_Inflate_Sheet_For_QS = new QPushButton(page_9);
        pushButton_Inflate_Sheet_For_QS->setObjectName(QString::fromUtf8("pushButton_Inflate_Sheet_For_QS"));

        verticalLayout_6->addWidget(pushButton_Inflate_Sheet_For_QS);

        pushButton_Postprocess_For_QS = new QPushButton(page_9);
        pushButton_Postprocess_For_QS->setObjectName(QString::fromUtf8("pushButton_Postprocess_For_QS"));

        verticalLayout_6->addWidget(pushButton_Postprocess_For_QS);

        pushButton_Show_Int_Diagram_For_QS = new QPushButton(page_9);
        pushButton_Show_Int_Diagram_For_QS->setObjectName(QString::fromUtf8("pushButton_Show_Int_Diagram_For_QS"));

        verticalLayout_6->addWidget(pushButton_Show_Int_Diagram_For_QS);

        verticalSpacer_5 = new QSpacerItem(20, 247, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_5);

        stackedWidget_for_QS->addWidget(page_9);
        page_10 = new QWidget();
        page_10->setObjectName(QString::fromUtf8("page_10"));
        verticalLayout_7 = new QVBoxLayout(page_10);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        pushButton_Add_Hexs_For_QS = new QPushButton(page_10);
        pushButton_Add_Hexs_For_QS->setObjectName(QString::fromUtf8("pushButton_Add_Hexs_For_QS"));

        verticalLayout_7->addWidget(pushButton_Add_Hexs_For_QS);

        pushButton_Current_Quality_For_QS = new QPushButton(page_10);
        pushButton_Current_Quality_For_QS->setObjectName(QString::fromUtf8("pushButton_Current_Quality_For_QS"));

        verticalLayout_7->addWidget(pushButton_Current_Quality_For_QS);

        pushButton_Remove_Hexs_For_QS = new QPushButton(page_10);
        pushButton_Remove_Hexs_For_QS->setObjectName(QString::fromUtf8("pushButton_Remove_Hexs_For_QS"));

        verticalLayout_7->addWidget(pushButton_Remove_Hexs_For_QS);

        pushButton_Modify_Quad_Set_OK_For_QS = new QPushButton(page_10);
        pushButton_Modify_Quad_Set_OK_For_QS->setObjectName(QString::fromUtf8("pushButton_Modify_Quad_Set_OK_For_QS"));

        verticalLayout_7->addWidget(pushButton_Modify_Quad_Set_OK_For_QS);

        verticalSpacer_6 = new QSpacerItem(20, 334, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_6);

        stackedWidget_for_QS->addWidget(page_10);
        page_12 = new QWidget();
        page_12->setObjectName(QString::fromUtf8("page_12"));
        verticalLayout_9 = new QVBoxLayout(page_12);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        pushButton_Pair_One_Int_Vhs_OK_for_QS = new QPushButton(page_12);
        pushButton_Pair_One_Int_Vhs_OK_for_QS->setObjectName(QString::fromUtf8("pushButton_Pair_One_Int_Vhs_OK_for_QS"));

        verticalLayout_9->addWidget(pushButton_Pair_One_Int_Vhs_OK_for_QS);

        pushButton_Next_Int_Edge_For_QS = new QPushButton(page_12);
        pushButton_Next_Int_Edge_For_QS->setObjectName(QString::fromUtf8("pushButton_Next_Int_Edge_For_QS"));

        verticalLayout_9->addWidget(pushButton_Next_Int_Edge_For_QS);

        pushButton_Select_Int_Edges_OK_For_QS = new QPushButton(page_12);
        pushButton_Select_Int_Edges_OK_For_QS->setObjectName(QString::fromUtf8("pushButton_Select_Int_Edges_OK_For_QS"));

        verticalLayout_9->addWidget(pushButton_Select_Int_Edges_OK_For_QS);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        comboBox_cross_hexa_idx = new QComboBox(page_12);
        comboBox_cross_hexa_idx->setObjectName(QString::fromUtf8("comboBox_cross_hexa_idx"));
        comboBox_cross_hexa_idx->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_3->addWidget(comboBox_cross_hexa_idx);

        pushButton_select_cross_hexa_OK = new QPushButton(page_12);
        pushButton_select_cross_hexa_OK->setObjectName(QString::fromUtf8("pushButton_select_cross_hexa_OK"));
        pushButton_select_cross_hexa_OK->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_3->addWidget(pushButton_select_cross_hexa_OK);


        verticalLayout_9->addLayout(horizontalLayout_3);

        pushButton_Pair_Int_Vhs_Return_For_QS = new QPushButton(page_12);
        pushButton_Pair_Int_Vhs_Return_For_QS->setObjectName(QString::fromUtf8("pushButton_Pair_Int_Vhs_Return_For_QS"));

        verticalLayout_9->addWidget(pushButton_Pair_Int_Vhs_Return_For_QS);

        verticalSpacer_8 = new QSpacerItem(20, 305, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_8);

        stackedWidget_for_QS->addWidget(page_12);

        verticalLayout_5->addWidget(stackedWidget_for_QS);

        toolBox->addItem(page_8, QString::fromUtf8("\345\233\233\350\276\271\345\275\242\351\235\242\351\233\206"));
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        page_4->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_4 = new QVBoxLayout(page_4);
        verticalLayout_4->setSpacing(3);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label = new QLabel(page_4);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_4->addWidget(label);

        lineEdit_Vertices = new QLineEdit(page_4);
        lineEdit_Vertices->setObjectName(QString::fromUtf8("lineEdit_Vertices"));

        verticalLayout_4->addWidget(lineEdit_Vertices);

        label_2 = new QLabel(page_4);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_4->addWidget(label_2);

        lineEdit_Edges = new QLineEdit(page_4);
        lineEdit_Edges->setObjectName(QString::fromUtf8("lineEdit_Edges"));

        verticalLayout_4->addWidget(lineEdit_Edges);

        label_5 = new QLabel(page_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_4->addWidget(label_5);

        lineEdit_Faces = new QLineEdit(page_4);
        lineEdit_Faces->setObjectName(QString::fromUtf8("lineEdit_Faces"));

        verticalLayout_4->addWidget(lineEdit_Faces);

        label_6 = new QLabel(page_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        verticalLayout_4->addWidget(label_6);

        lineEdit_Hexas = new QLineEdit(page_4);
        lineEdit_Hexas->setObjectName(QString::fromUtf8("lineEdit_Hexas"));

        verticalLayout_4->addWidget(lineEdit_Hexas);

        pushButton_Highlight_Mesh_Elements = new QPushButton(page_4);
        pushButton_Highlight_Mesh_Elements->setObjectName(QString::fromUtf8("pushButton_Highlight_Mesh_Elements"));

        verticalLayout_4->addWidget(pushButton_Highlight_Mesh_Elements);

        pushButton_Clear_Highlight_Mesh_Elements = new QPushButton(page_4);
        pushButton_Clear_Highlight_Mesh_Elements->setObjectName(QString::fromUtf8("pushButton_Clear_Highlight_Mesh_Elements"));

        verticalLayout_4->addWidget(pushButton_Clear_Highlight_Mesh_Elements);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_4);

        toolBox->addItem(page_4, QString::fromUtf8("\347\275\221\346\240\274\345\215\225\345\205\203\346\237\245\350\257\242"));
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        page_2->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_3 = new QVBoxLayout(page_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_7 = new QLabel(page_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_3->addWidget(label_7);

        pushButton_Select_Geom_Face = new QPushButton(page_2);
        pushButton_Select_Geom_Face->setObjectName(QString::fromUtf8("pushButton_Select_Geom_Face"));

        verticalLayout_3->addWidget(pushButton_Select_Geom_Face);

        pushButton_Smooth_Faces = new QPushButton(page_2);
        pushButton_Smooth_Faces->setObjectName(QString::fromUtf8("pushButton_Smooth_Faces"));

        verticalLayout_3->addWidget(pushButton_Smooth_Faces);

        pushButton_Clear_Selected_Faces = new QPushButton(page_2);
        pushButton_Clear_Selected_Faces->setObjectName(QString::fromUtf8("pushButton_Clear_Selected_Faces"));

        verticalLayout_3->addWidget(pushButton_Clear_Selected_Faces);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spinBox_Smooth_Rounds = new QSpinBox(page_2);
        spinBox_Smooth_Rounds->setObjectName(QString::fromUtf8("spinBox_Smooth_Rounds"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(spinBox_Smooth_Rounds->sizePolicy().hasHeightForWidth());
        spinBox_Smooth_Rounds->setSizePolicy(sizePolicy1);
        spinBox_Smooth_Rounds->setValue(1);

        horizontalLayout_2->addWidget(spinBox_Smooth_Rounds);

        pushButton_Start_Smoothing = new QPushButton(page_2);
        pushButton_Start_Smoothing->setObjectName(QString::fromUtf8("pushButton_Start_Smoothing"));
        QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_Start_Smoothing->sizePolicy().hasHeightForWidth());
        pushButton_Start_Smoothing->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(pushButton_Start_Smoothing);


        verticalLayout_3->addLayout(horizontalLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 476, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_3);

        toolBox->addItem(page_2, QString::fromUtf8("\347\275\221\346\240\274\344\274\230\345\214\226"));
        page_7 = new QWidget();
        page_7->setObjectName(QString::fromUtf8("page_7"));
        page_7->setGeometry(QRect(0, 0, 120, 529));
        spinBox_Depth_For_DC = new QSpinBox(page_7);
        spinBox_Depth_For_DC->setObjectName(QString::fromUtf8("spinBox_Depth_For_DC"));
        spinBox_Depth_For_DC->setGeometry(QRect(10, 40, 51, 22));
        spinBox_Depth_For_DC->setValue(1);
        pushButton_Set_Depth_For_DC = new QPushButton(page_7);
        pushButton_Set_Depth_For_DC->setObjectName(QString::fromUtf8("pushButton_Set_Depth_For_DC"));
        pushButton_Set_Depth_For_DC->setGeometry(QRect(70, 40, 41, 23));
        pushButton_Depth_Cut_For_DC = new QPushButton(page_7);
        pushButton_Depth_Cut_For_DC->setObjectName(QString::fromUtf8("pushButton_Depth_Cut_For_DC"));
        pushButton_Depth_Cut_For_DC->setGeometry(QRect(10, 100, 101, 23));
        pushButton_Save_For_DC = new QPushButton(page_7);
        pushButton_Save_For_DC->setObjectName(QString::fromUtf8("pushButton_Save_For_DC"));
        pushButton_Save_For_DC->setGeometry(QRect(10, 130, 101, 23));
        pushButton_Set_Start_Face_For_DC = new QPushButton(page_7);
        pushButton_Set_Start_Face_For_DC->setObjectName(QString::fromUtf8("pushButton_Set_Start_Face_For_DC"));
        pushButton_Set_Start_Face_For_DC->setGeometry(QRect(10, 10, 101, 23));
        pushButton_Create_Cut_Face_For_DC = new QPushButton(page_7);
        pushButton_Create_Cut_Face_For_DC->setObjectName(QString::fromUtf8("pushButton_Create_Cut_Face_For_DC"));
        pushButton_Create_Cut_Face_For_DC->setGeometry(QRect(10, 70, 101, 23));
        toolBox->addItem(page_7, QString::fromUtf8("\346\267\261\345\272\246\345\210\207\345\211\262"));
        page_11 = new QWidget();
        page_11->setObjectName(QString::fromUtf8("page_11"));
        page_11->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_8 = new QVBoxLayout(page_11);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_group_color = new QLabel(page_11);
        label_group_color->setObjectName(QString::fromUtf8("label_group_color"));
        label_group_color->setMinimumSize(QSize(30, 0));
        label_group_color->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 255, 0);"));

        horizontalLayout_6->addWidget(label_group_color);

        pushButton_Set_Color_For_PD = new QPushButton(page_11);
        pushButton_Set_Color_For_PD->setObjectName(QString::fromUtf8("pushButton_Set_Color_For_PD"));
        pushButton_Set_Color_For_PD->setMaximumSize(QSize(65, 16777215));

        horizontalLayout_6->addWidget(pushButton_Set_Color_For_PD);


        verticalLayout_8->addLayout(horizontalLayout_6);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_3 = new QLabel(page_11);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_8->addWidget(label_3);

        doubleSpinBox_Line_Weight = new QDoubleSpinBox(page_11);
        doubleSpinBox_Line_Weight->setObjectName(QString::fromUtf8("doubleSpinBox_Line_Weight"));
        doubleSpinBox_Line_Weight->setMaximumSize(QSize(50, 16777215));
        doubleSpinBox_Line_Weight->setDecimals(1);
        doubleSpinBox_Line_Weight->setValue(6);

        horizontalLayout_8->addWidget(doubleSpinBox_Line_Weight);


        verticalLayout_8->addLayout(horizontalLayout_8);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_4 = new QLabel(page_11);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_10->addWidget(label_4);

        doubleSpinBox_Cell_Ratio = new QDoubleSpinBox(page_11);
        doubleSpinBox_Cell_Ratio->setObjectName(QString::fromUtf8("doubleSpinBox_Cell_Ratio"));
        doubleSpinBox_Cell_Ratio->setMaximumSize(QSize(50, 16777215));
        doubleSpinBox_Cell_Ratio->setDecimals(1);
        doubleSpinBox_Cell_Ratio->setMaximum(1);
        doubleSpinBox_Cell_Ratio->setSingleStep(0.1);
        doubleSpinBox_Cell_Ratio->setValue(0.8);

        horizontalLayout_10->addWidget(doubleSpinBox_Cell_Ratio);


        verticalLayout_8->addLayout(horizontalLayout_10);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_Add_Element_For_PD = new QPushButton(page_11);
        pushButton_Add_Element_For_PD->setObjectName(QString::fromUtf8("pushButton_Add_Element_For_PD"));
        pushButton_Add_Element_For_PD->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_7->addWidget(pushButton_Add_Element_For_PD);

        pushButton_Remove_Element_For_PD = new QPushButton(page_11);
        pushButton_Remove_Element_For_PD->setObjectName(QString::fromUtf8("pushButton_Remove_Element_For_PD"));
        pushButton_Remove_Element_For_PD->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_7->addWidget(pushButton_Remove_Element_For_PD);


        verticalLayout_8->addLayout(horizontalLayout_7);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(0);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        pushButton_Add_Cells_For_PD = new QPushButton(page_11);
        pushButton_Add_Cells_For_PD->setObjectName(QString::fromUtf8("pushButton_Add_Cells_For_PD"));
        pushButton_Add_Cells_For_PD->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_9->addWidget(pushButton_Add_Cells_For_PD);

        pushButton_Remove_Cells_For_PD = new QPushButton(page_11);
        pushButton_Remove_Cells_For_PD->setObjectName(QString::fromUtf8("pushButton_Remove_Cells_For_PD"));
        pushButton_Remove_Cells_For_PD->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_9->addWidget(pushButton_Remove_Cells_For_PD);


        verticalLayout_8->addLayout(horizontalLayout_9);

        pushButton_Append_Edges_on_Faces_For_PD = new QPushButton(page_11);
        pushButton_Append_Edges_on_Faces_For_PD->setObjectName(QString::fromUtf8("pushButton_Append_Edges_on_Faces_For_PD"));

        verticalLayout_8->addWidget(pushButton_Append_Edges_on_Faces_For_PD);

        pushButton_Add_Element_Group_For_PD = new QPushButton(page_11);
        pushButton_Add_Element_Group_For_PD->setObjectName(QString::fromUtf8("pushButton_Add_Element_Group_For_PD"));

        verticalLayout_8->addWidget(pushButton_Add_Element_Group_For_PD);

        pushButton_Recursive_Spreading = new QPushButton(page_11);
        pushButton_Recursive_Spreading->setObjectName(QString::fromUtf8("pushButton_Recursive_Spreading"));

        verticalLayout_8->addWidget(pushButton_Recursive_Spreading);

        pushButton_Loop_Sep = new QPushButton(page_11);
        pushButton_Loop_Sep->setObjectName(QString::fromUtf8("pushButton_Loop_Sep"));

        verticalLayout_8->addWidget(pushButton_Loop_Sep);

        pushButton_Get_st_Node = new QPushButton(page_11);
        pushButton_Get_st_Node->setObjectName(QString::fromUtf8("pushButton_Get_st_Node"));

        verticalLayout_8->addWidget(pushButton_Get_st_Node);

        pushButton_Finally_Sep = new QPushButton(page_11);
        pushButton_Finally_Sep->setObjectName(QString::fromUtf8("pushButton_Finally_Sep"));

        verticalLayout_8->addWidget(pushButton_Finally_Sep);

        pushButton_Sep_Hex_Seeds = new QPushButton(page_11);
        pushButton_Sep_Hex_Seeds->setObjectName(QString::fromUtf8("pushButton_Sep_Hex_Seeds"));

        verticalLayout_8->addWidget(pushButton_Sep_Hex_Seeds);

        pushButton_Int_Sep_Hex_Sets = new QPushButton(page_11);
        pushButton_Int_Sep_Hex_Sets->setObjectName(QString::fromUtf8("pushButton_Int_Sep_Hex_Sets"));

        verticalLayout_8->addWidget(pushButton_Int_Sep_Hex_Sets);

        pushButton_Get_Init_Quad_Set_For_PD = new QPushButton(page_11);
        pushButton_Get_Init_Quad_Set_For_PD->setObjectName(QString::fromUtf8("pushButton_Get_Init_Quad_Set_For_PD"));

        verticalLayout_8->addWidget(pushButton_Get_Init_Quad_Set_For_PD);

        pushButton_Algo_Test_For_PD = new QPushButton(page_11);
        pushButton_Algo_Test_For_PD->setObjectName(QString::fromUtf8("pushButton_Algo_Test_For_PD"));

        verticalLayout_8->addWidget(pushButton_Algo_Test_For_PD);

        pushButton_Show_Sheet_Edge_Face_For_PD = new QPushButton(page_11);
        pushButton_Show_Sheet_Edge_Face_For_PD->setObjectName(QString::fromUtf8("pushButton_Show_Sheet_Edge_Face_For_PD"));

        verticalLayout_8->addWidget(pushButton_Show_Sheet_Edge_Face_For_PD);

        pushButton_Turn_To_Inflation_For_PD = new QPushButton(page_11);
        pushButton_Turn_To_Inflation_For_PD->setObjectName(QString::fromUtf8("pushButton_Turn_To_Inflation_For_PD"));

        verticalLayout_8->addWidget(pushButton_Turn_To_Inflation_For_PD);

        verticalSpacer_7 = new QSpacerItem(20, 235, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_7);

        toolBox->addItem(page_11, QString::fromUtf8("\350\256\272\346\226\207\347\273\230\345\210\266"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setGeometry(QRect(0, 0, 120, 529));
        verticalLayout_2 = new QVBoxLayout(page);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pushButton_Test1 = new QPushButton(page);
        pushButton_Test1->setObjectName(QString::fromUtf8("pushButton_Test1"));

        verticalLayout_2->addWidget(pushButton_Test1);

        pushButton_Test2 = new QPushButton(page);
        pushButton_Test2->setObjectName(QString::fromUtf8("pushButton_Test2"));

        verticalLayout_2->addWidget(pushButton_Test2);

        verticalSpacer_2 = new QSpacerItem(20, 467, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        toolBox->addItem(page, QString::fromUtf8("\346\265\213\350\257\225"));

        horizontalLayout->addWidget(toolBox);

        hoopsview = new HoopsView(MeshEditWidget);
        hoopsview->setObjectName(QString::fromUtf8("hoopsview"));
        toolBox->raise();

        horizontalLayout->addWidget(hoopsview);


        retranslateUi(MeshEditWidget);

        stackedWidget_for_QS->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MeshEditWidget);
    } // setupUi

    void retranslateUi(QWidget *MeshEditWidget)
    {
        MeshEditWidget->setWindowTitle(QApplication::translate("MeshEditWidget", "MeshEditWidget", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\351\200\211\345\217\226\347\275\221\346\240\274\350\276\271OK", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Surf_Patches_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\350\216\267\345\276\227\351\235\242\347\275\221\346\240\274\345\214\272\345\235\227", 0, QApplication::UnicodeUTF8));
        pushButton_Set_Depth_Control_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\350\256\276\347\275\256\346\267\261\345\272\246", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\351\200\211\345\217\226\344\270\215\345\217\257\345\217\230\351\235\242OK", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Hexa_Set->setText(QApplication::translate("MeshEditWidget", "\350\216\267\345\276\227\345\205\255\351\235\242\344\275\223\351\233\206\345\220\210", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Quad_Set_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\350\216\267\345\276\227\345\233\233\350\276\271\345\275\242\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Sheet_Inflate_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\347\224\237\346\210\220Sheet", 0, QApplication::UnicodeUTF8));
        pushButton_Reprocess_For_Auto_LSI->setText(QApplication::translate("MeshEditWidget", "\345\220\216\345\244\204\347\220\206\345\217\212\346\270\205\347\220\206", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_3), QApplication::translate("MeshEditWidget", "Sheet\347\224\237\346\210\220", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Sheet_For_LSE->setText(QApplication::translate("MeshEditWidget", "\351\200\211\346\213\251sheet", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Int_Diagram_For_LSE->setText(QApplication::translate("MeshEditWidget", "\346\230\276\347\244\272sheet\347\233\270\344\272\244\345\233\276", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Interface_Faces_For_LSE->setText(QApplication::translate("MeshEditWidget", "\351\200\211\346\213\251\350\264\264\345\220\210\351\235\242\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Constant_Faces_For_LSE->setText(QApplication::translate("MeshEditWidget", "\351\200\211\346\213\251\345\233\272\345\256\232\351\235\242\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Sep_Hex_mesh->setText(QApplication::translate("MeshEditWidget", "\345\210\207\345\210\206", 0, QApplication::UnicodeUTF8));
        pushButton_Extract_All_For_LSE->setText(QApplication::translate("MeshEditWidget", "\345\205\250\351\203\250\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Local_Inflation_For_LSE->setText(QApplication::translate("MeshEditWidget", "\345\261\200\351\203\250\347\224\237\346\210\220", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_5), QApplication::translate("MeshEditWidget", "Sheet\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Set_Min_Depth_For_LC->setText(QApplication::translate("MeshEditWidget", "\350\256\276\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Non_Int_Loop_Connect_For_LC->setText(QApplication::translate("MeshEditWidget", "\351\235\236\344\272\244\345\217\211\345\244\204\350\277\236\346\216\245", 0, QApplication::UnicodeUTF8));
        pushButton_Search_Intersect_Path_For_LC->setText(QApplication::translate("MeshEditWidget", "\346\220\234\347\264\242\344\272\244\345\217\211\350\267\257\345\276\204", 0, QApplication::UnicodeUTF8));
        pushButton_Analyze_For_LC->setText(QApplication::translate("MeshEditWidget", "\345\210\206\346\236\220", 0, QApplication::UnicodeUTF8));
        pushButton_Int_Loop_Connect_For_LC->setText(QApplication::translate("MeshEditWidget", "\344\272\244\345\217\211\345\244\204\350\277\236\346\216\245", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_6), QApplication::translate("MeshEditWidget", "loop\350\241\245\345\205\250", 0, QApplication::UnicodeUTF8));
        pushButton_Read_Data_For_QS->setText(QApplication::translate("MeshEditWidget", "\350\257\273\345\217\226", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Edges_OK_For_QS->setText(QApplication::translate("MeshEditWidget", "\351\200\211\345\217\226\347\275\221\346\240\274\350\276\271OK", 0, QApplication::UnicodeUTF8));
        pushButton_Get_QS_By_PCS_For_QS->setText(QApplication::translate("MeshEditWidget", "PCS\350\216\267\345\276\227\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Get_QS_By_Sweeping_For_QS->setText(QApplication::translate("MeshEditWidget", "\346\211\253\345\261\202\350\216\267\345\276\227\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Constrait_Faces_OK_For_QS->setText(QApplication::translate("MeshEditWidget", "\351\200\211\345\217\226\347\272\246\346\235\237\351\235\242OK", 0, QApplication::UnicodeUTF8));
        pushButton_Pair_Int_Vhs_For_QS->setText(QApplication::translate("MeshEditWidget", "\344\272\244\347\202\271\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Close_Loop_For_QS->setText(QApplication::translate("MeshEditWidget", "\345\260\201\351\227\255", 0, QApplication::UnicodeUTF8));
        pushButton_Append_Loop_For_QS->setText(QApplication::translate("MeshEditWidget", "\350\241\245\347\216\257", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Quad_Set_For_QS->setText(QApplication::translate("MeshEditWidget", "\350\216\267\345\276\227QS", 0, QApplication::UnicodeUTF8));
        pushButton_Optimize_Quad_Set_For_QS->setText(QApplication::translate("MeshEditWidget", "\344\274\230\345\214\226QS", 0, QApplication::UnicodeUTF8));
        pushButton_Optimize_Once_For_QS->setText(QApplication::translate("MeshEditWidget", "\345\215\225\346\254\241\344\274\230\345\214\226", 0, QApplication::UnicodeUTF8));
        pushButton_Save_Data_For_QS->setText(QApplication::translate("MeshEditWidget", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        pushButton_Inflate_Sheet_For_QS->setText(QApplication::translate("MeshEditWidget", "sheet\347\224\237\346\210\220", 0, QApplication::UnicodeUTF8));
        pushButton_Postprocess_For_QS->setText(QApplication::translate("MeshEditWidget", "\345\220\216\345\244\204\347\220\206", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Int_Diagram_For_QS->setText(QApplication::translate("MeshEditWidget", "\346\230\276\347\244\272\347\233\270\344\272\244\345\233\276", 0, QApplication::UnicodeUTF8));
        pushButton_Add_Hexs_For_QS->setText(QApplication::translate("MeshEditWidget", "\346\267\273\345\212\240", 0, QApplication::UnicodeUTF8));
        pushButton_Current_Quality_For_QS->setText(QApplication::translate("MeshEditWidget", "\345\275\223\345\211\215\350\264\250\351\207\217", 0, QApplication::UnicodeUTF8));
        pushButton_Remove_Hexs_For_QS->setText(QApplication::translate("MeshEditWidget", "\345\210\240\351\231\244", 0, QApplication::UnicodeUTF8));
        pushButton_Modify_Quad_Set_OK_For_QS->setText(QApplication::translate("MeshEditWidget", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Pair_One_Int_Vhs_OK_for_QS->setText(QApplication::translate("MeshEditWidget", "\344\272\244\347\202\271\351\205\215\345\257\271", 0, QApplication::UnicodeUTF8));
        pushButton_Next_Int_Edge_For_QS->setText(QApplication::translate("MeshEditWidget", "\344\270\213\344\270\200\346\235\241", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Int_Edges_OK_For_QS->setText(QApplication::translate("MeshEditWidget", "\351\200\211\346\213\251\347\273\223\346\235\237", 0, QApplication::UnicodeUTF8));
        comboBox_cross_hexa_idx->clear();
        comboBox_cross_hexa_idx->insertItems(0, QStringList()
         << QApplication::translate("MeshEditWidget", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MeshEditWidget", "2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MeshEditWidget", "3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MeshEditWidget", "4", 0, QApplication::UnicodeUTF8)
        );
        pushButton_select_cross_hexa_OK->setText(QApplication::translate("MeshEditWidget", "\347\241\256\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Pair_Int_Vhs_Return_For_QS->setText(QApplication::translate("MeshEditWidget", "\350\277\224\345\233\236", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_8), QApplication::translate("MeshEditWidget", "\345\233\233\350\276\271\345\275\242\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MeshEditWidget", "\347\202\271\357\274\232", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MeshEditWidget", "\350\276\271\357\274\232", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MeshEditWidget", "\351\235\242\357\274\232", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MeshEditWidget", "\344\275\223\357\274\232", 0, QApplication::UnicodeUTF8));
        pushButton_Highlight_Mesh_Elements->setText(QApplication::translate("MeshEditWidget", "\346\230\276\347\244\272", 0, QApplication::UnicodeUTF8));
        pushButton_Clear_Highlight_Mesh_Elements->setText(QApplication::translate("MeshEditWidget", "\346\270\205\351\231\244", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_4), QApplication::translate("MeshEditWidget", "\347\275\221\346\240\274\345\215\225\345\205\203\346\237\245\350\257\242", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MeshEditWidget", "\345\205\211\351\241\272", 0, QApplication::UnicodeUTF8));
        pushButton_Select_Geom_Face->setText(QApplication::translate("MeshEditWidget", "\351\200\211\346\213\251\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Smooth_Faces->setText(QApplication::translate("MeshEditWidget", "\345\205\211\351\241\272\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Clear_Selected_Faces->setText(QApplication::translate("MeshEditWidget", "\346\270\205\351\231\244\351\200\211\346\213\251", 0, QApplication::UnicodeUTF8));
        pushButton_Start_Smoothing->setText(QApplication::translate("MeshEditWidget", "\345\274\200\345\247\213", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_2), QApplication::translate("MeshEditWidget", "\347\275\221\346\240\274\344\274\230\345\214\226", 0, QApplication::UnicodeUTF8));
        pushButton_Set_Depth_For_DC->setText(QApplication::translate("MeshEditWidget", "\350\256\276\345\256\232", 0, QApplication::UnicodeUTF8));
        pushButton_Depth_Cut_For_DC->setText(QApplication::translate("MeshEditWidget", "\345\210\207\345\211\262", 0, QApplication::UnicodeUTF8));
        pushButton_Save_For_DC->setText(QApplication::translate("MeshEditWidget", "\344\277\235\345\255\230", 0, QApplication::UnicodeUTF8));
        pushButton_Set_Start_Face_For_DC->setText(QApplication::translate("MeshEditWidget", "\350\256\276\345\256\232\350\265\267\345\247\213\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Create_Cut_Face_For_DC->setText(QApplication::translate("MeshEditWidget", "\346\236\204\351\200\240\345\210\207\345\211\262\351\235\242", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_7), QApplication::translate("MeshEditWidget", "\346\267\261\345\272\246\345\210\207\345\211\262", 0, QApplication::UnicodeUTF8));
        label_group_color->setText(QString());
        pushButton_Set_Color_For_PD->setText(QApplication::translate("MeshEditWidget", "\350\256\276\347\275\256\351\242\234\350\211\262", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MeshEditWidget", "\347\272\277\345\256\275\357\274\232", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MeshEditWidget", "\344\275\223\345\244\247\345\260\217\357\274\232", 0, QApplication::UnicodeUTF8));
        pushButton_Add_Element_For_PD->setText(QApplication::translate("MeshEditWidget", "\345\242\236\345\212\240", 0, QApplication::UnicodeUTF8));
        pushButton_Remove_Element_For_PD->setText(QApplication::translate("MeshEditWidget", "\345\207\217\345\260\221", 0, QApplication::UnicodeUTF8));
        pushButton_Add_Cells_For_PD->setText(QApplication::translate("MeshEditWidget", "\345\242\236\345\212\240\344\275\223", 0, QApplication::UnicodeUTF8));
        pushButton_Remove_Cells_For_PD->setText(QApplication::translate("MeshEditWidget", "\345\210\240\351\231\244\344\275\223", 0, QApplication::UnicodeUTF8));
        pushButton_Append_Edges_on_Faces_For_PD->setText(QApplication::translate("MeshEditWidget", "\350\241\245\345\205\205\351\235\242\344\270\212\347\232\204\350\276\271", 0, QApplication::UnicodeUTF8));
        pushButton_Add_Element_Group_For_PD->setText(QApplication::translate("MeshEditWidget", "\346\267\273\345\212\240\346\270\262\346\237\223\347\273\204", 0, QApplication::UnicodeUTF8));
        pushButton_Recursive_Spreading->setText(QApplication::translate("MeshEditWidget", "\350\277\255\344\273\243\346\211\251\346\225\243", 0, QApplication::UnicodeUTF8));
        pushButton_Loop_Sep->setText(QApplication::translate("MeshEditWidget", "\350\276\271\347\225\214\347\216\257\345\214\272\345\237\237\345\210\206\345\211\262\346\200\247", 0, QApplication::UnicodeUTF8));
        pushButton_Get_st_Node->setText(QApplication::translate("MeshEditWidget", "s\345\222\214t", 0, QApplication::UnicodeUTF8));
        pushButton_Finally_Sep->setText(QApplication::translate("MeshEditWidget", "\345\210\206\345\211\262\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Sep_Hex_Seeds->setText(QApplication::translate("MeshEditWidget", "\345\210\206\345\211\262\344\275\223\347\247\215\345\255\220", 0, QApplication::UnicodeUTF8));
        pushButton_Int_Sep_Hex_Sets->setText(QApplication::translate("MeshEditWidget", "\347\233\270\344\272\244\345\210\206\345\211\262\344\275\223\347\247\215\345\255\220", 0, QApplication::UnicodeUTF8));
        pushButton_Get_Init_Quad_Set_For_PD->setText(QApplication::translate("MeshEditWidget", "\345\210\235\345\247\213\351\235\242\351\233\206", 0, QApplication::UnicodeUTF8));
        pushButton_Algo_Test_For_PD->setText(QApplication::translate("MeshEditWidget", "\347\256\227\346\263\225\346\265\213\350\257\225", 0, QApplication::UnicodeUTF8));
        pushButton_Show_Sheet_Edge_Face_For_PD->setText(QApplication::translate("MeshEditWidget", "sheet\350\276\271\345\222\214\351\235\242", 0, QApplication::UnicodeUTF8));
        pushButton_Turn_To_Inflation_For_PD->setText(QApplication::translate("MeshEditWidget", "\350\275\254\345\205\245inflation", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_11), QApplication::translate("MeshEditWidget", "\350\256\272\346\226\207\347\273\230\345\210\266", 0, QApplication::UnicodeUTF8));
        pushButton_Test1->setText(QApplication::translate("MeshEditWidget", "\346\265\213\350\257\225\344\270\200", 0, QApplication::UnicodeUTF8));
        pushButton_Test2->setText(QApplication::translate("MeshEditWidget", "\346\265\213\350\257\225\344\272\214", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page), QApplication::translate("MeshEditWidget", "\346\265\213\350\257\225", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeshEditWidget: public Ui_MeshEditWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHEDITWIDGET_H
