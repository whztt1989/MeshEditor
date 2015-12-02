#include "stdafx.h"
#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>

#include "HDB.h"
#include "ha_bridge.h"
#include "license.hxx"
#include "ckoutcom.hxx"
#include "kernapi.hxx"
#include "spa_unlock_result.hxx"


HDB * m_pHDB=0; 

void initAcisHoops ()
{
	base_configuration base_config;
	logical ok = initialize_base( &base_config);
	const char *unlock_str = getenv("SPATIAL_LICENSE") ;

	spa_unlock_result out = spa_unlock_products( unlock_str );
	if ( out.get_state() == SPA_UNLOCK_FAIL ) {
		QMessageBox::warning(NULL, QObject::tr("License Invalid"), QObject::tr("The key provided for ACIS/InterOp is invalid. The application may not work.\nPlease add your unlock code from your spa_unlock_xxx.cpp to CDetectModifyApp::InitInstance function."));
	}
	else if ( out.get_state() == SPA_UNLOCK_PASS_WARN ) {
		char str[256];
		sprintf(str,"%s\n This evaluation key will expire in %i days.", out.get_message_text(), out.get_num_days());		
		QMessageBox::warning(NULL,str, "Spatial Eval Warning");
	}
	outcome o;
	o = api_start_modeller(0);
	check_outcome(o);
	o = api_initialize_hoops_acis_bridge();
	check_outcome(o);

	m_pHDB = new HDB(); 
	m_pHDB->Init();
	m_pHDB->SetDriverType("opengl");	
}

void termAcisHoops ()
{
	delete m_pHDB;
	api_terminate_hoops_acis_bridge();
	api_stop_modeller();
	terminate_base();
	HC_Reset_System(); 
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec *codec = QTextCodec::codecForName("GB2312");  

	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);  
	QTextCodec::setCodecForTr(codec);
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

	initAcisHoops();
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	MainWindow *w = new MainWindow ();
	w->show();
	int ret = a.exec();
	delete w;
	termAcisHoops();
	return ret;
}
