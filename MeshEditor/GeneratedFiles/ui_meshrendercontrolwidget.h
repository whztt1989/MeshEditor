/********************************************************************************
** Form generated from reading UI file 'meshrendercontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESHRENDERCONTROLWIDGET_H
#define UI_MESHRENDERCONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MeshRenderControlWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_Mesh_Boundary;
    QGridLayout *gridLayout;
    QCheckBox *checkBox_Boundary_Faces_Indices;
    QCheckBox *checkBox_Boundary_Edges;
    QCheckBox *checkBox_Boundary_Edges_Indices;
    QCheckBox *checkBox_Boundary_Vertices_Indices;
    QCheckBox *checkBox_Boundary_Vertices;
    QCheckBox *checkBox_Boundary_Faces;
    QCheckBox *checkBox_Boundary_Cells;
    QCheckBox *checkBox_Boundary_Cells_Indices;
    QGroupBox *groupBox_Mesh_Inner;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBox_Inner_Vertices;
    QCheckBox *checkBox_Inner_Vertices_Indices;
    QCheckBox *checkBox_Inner_Edges;
    QCheckBox *checkBox_Inner_Edges_Indices;
    QCheckBox *checkBox_Inner_Faces_Indices;
    QCheckBox *checkBox_Inner_Faces;
    QCheckBox *checkBox_Inner_Cells;
    QCheckBox *checkBox_Inner_Cells_Indices;

    void setupUi(QWidget *MeshRenderControlWidget)
    {
        if (MeshRenderControlWidget->objectName().isEmpty())
            MeshRenderControlWidget->setObjectName(QString::fromUtf8("MeshRenderControlWidget"));
        MeshRenderControlWidget->resize(237, 132);
        horizontalLayout = new QHBoxLayout(MeshRenderControlWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        groupBox_Mesh_Boundary = new QGroupBox(MeshRenderControlWidget);
        groupBox_Mesh_Boundary->setObjectName(QString::fromUtf8("groupBox_Mesh_Boundary"));
        groupBox_Mesh_Boundary->setCheckable(true);
        gridLayout = new QGridLayout(groupBox_Mesh_Boundary);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        checkBox_Boundary_Faces_Indices = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Faces_Indices->setObjectName(QString::fromUtf8("checkBox_Boundary_Faces_Indices"));

        gridLayout->addWidget(checkBox_Boundary_Faces_Indices, 2, 1, 1, 1);

        checkBox_Boundary_Edges = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Edges->setObjectName(QString::fromUtf8("checkBox_Boundary_Edges"));
        checkBox_Boundary_Edges->setChecked(true);

        gridLayout->addWidget(checkBox_Boundary_Edges, 1, 0, 1, 1);

        checkBox_Boundary_Edges_Indices = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Edges_Indices->setObjectName(QString::fromUtf8("checkBox_Boundary_Edges_Indices"));

        gridLayout->addWidget(checkBox_Boundary_Edges_Indices, 1, 1, 1, 1);

        checkBox_Boundary_Vertices_Indices = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Vertices_Indices->setObjectName(QString::fromUtf8("checkBox_Boundary_Vertices_Indices"));
        checkBox_Boundary_Vertices_Indices->setEnabled(true);

        gridLayout->addWidget(checkBox_Boundary_Vertices_Indices, 0, 1, 1, 1);

        checkBox_Boundary_Vertices = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Vertices->setObjectName(QString::fromUtf8("checkBox_Boundary_Vertices"));

        gridLayout->addWidget(checkBox_Boundary_Vertices, 0, 0, 1, 1);

        checkBox_Boundary_Faces = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Faces->setObjectName(QString::fromUtf8("checkBox_Boundary_Faces"));
        checkBox_Boundary_Faces->setChecked(true);

        gridLayout->addWidget(checkBox_Boundary_Faces, 2, 0, 1, 1);

        checkBox_Boundary_Cells = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Cells->setObjectName(QString::fromUtf8("checkBox_Boundary_Cells"));

        gridLayout->addWidget(checkBox_Boundary_Cells, 3, 0, 1, 1);

        checkBox_Boundary_Cells_Indices = new QCheckBox(groupBox_Mesh_Boundary);
        checkBox_Boundary_Cells_Indices->setObjectName(QString::fromUtf8("checkBox_Boundary_Cells_Indices"));

        gridLayout->addWidget(checkBox_Boundary_Cells_Indices, 3, 1, 1, 1);


        horizontalLayout->addWidget(groupBox_Mesh_Boundary);

        groupBox_Mesh_Inner = new QGroupBox(MeshRenderControlWidget);
        groupBox_Mesh_Inner->setObjectName(QString::fromUtf8("groupBox_Mesh_Inner"));
        groupBox_Mesh_Inner->setCheckable(true);
        groupBox_Mesh_Inner->setChecked(false);
        gridLayout_2 = new QGridLayout(groupBox_Mesh_Inner);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(3);
        checkBox_Inner_Vertices = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Vertices->setObjectName(QString::fromUtf8("checkBox_Inner_Vertices"));

        gridLayout_2->addWidget(checkBox_Inner_Vertices, 0, 0, 1, 1);

        checkBox_Inner_Vertices_Indices = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Vertices_Indices->setObjectName(QString::fromUtf8("checkBox_Inner_Vertices_Indices"));
        checkBox_Inner_Vertices_Indices->setEnabled(false);

        gridLayout_2->addWidget(checkBox_Inner_Vertices_Indices, 0, 1, 1, 1);

        checkBox_Inner_Edges = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Edges->setObjectName(QString::fromUtf8("checkBox_Inner_Edges"));
        checkBox_Inner_Edges->setChecked(true);

        gridLayout_2->addWidget(checkBox_Inner_Edges, 1, 0, 1, 1);

        checkBox_Inner_Edges_Indices = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Edges_Indices->setObjectName(QString::fromUtf8("checkBox_Inner_Edges_Indices"));

        gridLayout_2->addWidget(checkBox_Inner_Edges_Indices, 1, 1, 1, 1);

        checkBox_Inner_Faces_Indices = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Faces_Indices->setObjectName(QString::fromUtf8("checkBox_Inner_Faces_Indices"));

        gridLayout_2->addWidget(checkBox_Inner_Faces_Indices, 2, 1, 1, 1);

        checkBox_Inner_Faces = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Faces->setObjectName(QString::fromUtf8("checkBox_Inner_Faces"));

        gridLayout_2->addWidget(checkBox_Inner_Faces, 2, 0, 1, 1);

        checkBox_Inner_Cells = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Cells->setObjectName(QString::fromUtf8("checkBox_Inner_Cells"));

        gridLayout_2->addWidget(checkBox_Inner_Cells, 3, 0, 1, 1);

        checkBox_Inner_Cells_Indices = new QCheckBox(groupBox_Mesh_Inner);
        checkBox_Inner_Cells_Indices->setObjectName(QString::fromUtf8("checkBox_Inner_Cells_Indices"));

        gridLayout_2->addWidget(checkBox_Inner_Cells_Indices, 3, 1, 1, 1);


        horizontalLayout->addWidget(groupBox_Mesh_Inner);


        retranslateUi(MeshRenderControlWidget);

        QMetaObject::connectSlotsByName(MeshRenderControlWidget);
    } // setupUi

    void retranslateUi(QWidget *MeshRenderControlWidget)
    {
        MeshRenderControlWidget->setWindowTitle(QApplication::translate("MeshRenderControlWidget", "MeshRenderControlWidget", 0, QApplication::UnicodeUTF8));
        groupBox_Mesh_Boundary->setTitle(QApplication::translate("MeshRenderControlWidget", "\347\275\221\346\240\274\350\276\271\347\225\214", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Faces_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Edges->setText(QApplication::translate("MeshRenderControlWidget", "\350\276\271", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Edges_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Vertices_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Vertices->setText(QApplication::translate("MeshRenderControlWidget", "\347\202\271", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Faces->setText(QApplication::translate("MeshRenderControlWidget", "\351\235\242", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Cells->setText(QApplication::translate("MeshRenderControlWidget", "\344\275\223", 0, QApplication::UnicodeUTF8));
        checkBox_Boundary_Cells_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        groupBox_Mesh_Inner->setTitle(QApplication::translate("MeshRenderControlWidget", "\347\275\221\346\240\274\345\206\205\351\203\250", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Vertices->setText(QApplication::translate("MeshRenderControlWidget", "\347\202\271", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Vertices_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Edges->setText(QApplication::translate("MeshRenderControlWidget", "\350\276\271", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Edges_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Faces_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Faces->setText(QApplication::translate("MeshRenderControlWidget", "\351\235\242", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Cells->setText(QApplication::translate("MeshRenderControlWidget", "\344\275\223", 0, QApplication::UnicodeUTF8));
        checkBox_Inner_Cells_Indices->setText(QApplication::translate("MeshRenderControlWidget", "\345\272\217\345\217\267", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MeshRenderControlWidget: public Ui_MeshRenderControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESHRENDERCONTROLWIDGET_H
