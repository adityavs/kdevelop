/***************************************************************************
                          kdlgnewdialogdlg.cpp  -  description                              
                             -------------------                                         
    begin                : Mon Apr 12 1999                                           
    copyright            : (C) 1999 by Sandy Meier                         
    email                : smeier@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "kdlgnewdialogdlg.h"
#include "../cproject.h"

#include <kapp.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstddirs.h>

KDlgNewDialogDlg::KDlgNewDialogDlg(QWidget *parent, const char *name,CProject* prj ) : QDialog(parent,name,true) {
  project = prj;

	QButtonGroup* qtarch_ButtonGroup_1;
	qtarch_ButtonGroup_1 = new QButtonGroup( this, "ButtonGroup_1" );
	qtarch_ButtonGroup_1->setGeometry( 10, 10, 220, 190 );
	qtarch_ButtonGroup_1->setMinimumSize( 0, 0 );
	qtarch_ButtonGroup_1->setMaximumSize( 32767, 32767 );
	qtarch_ButtonGroup_1->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_1->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_1->setFrameStyle( 49 );
	qtarch_ButtonGroup_1->setTitle( i18n("Dialog Class") );
	qtarch_ButtonGroup_1->setAlignment( 1 );

	qwidget_radio_button = new QRadioButton( this, "qwidget_radio_button" );
	qwidget_radio_button->setGeometry( 20, 30, 80, 30 );
	qwidget_radio_button->setMinimumSize( 0, 0 );
	qwidget_radio_button->setMaximumSize( 32767, 32767 );
	qwidget_radio_button->setFocusPolicy( QWidget::TabFocus );
	qwidget_radio_button->setBackgroundMode( QWidget::PaletteBackground );
	qwidget_radio_button->setFontPropagation( QWidget::NoChildren );
	qwidget_radio_button->setPalettePropagation( QWidget::NoChildren );
	qwidget_radio_button->setText( "QWidget" );
	qwidget_radio_button->setAutoRepeat( FALSE );
	qwidget_radio_button->setAutoResize( FALSE );
	
	qdialog_radio_button = new QRadioButton( this, "qdialog_radio_button" );
	qdialog_radio_button->setGeometry( 110, 30, 100, 30 );
	qdialog_radio_button->setMinimumSize( 0, 0 );
	qdialog_radio_button->setMaximumSize( 32767, 32767 );
	qdialog_radio_button->setFocusPolicy( QWidget::TabFocus );
	qdialog_radio_button->setBackgroundMode( QWidget::PaletteBackground );
	qdialog_radio_button->setFontPropagation( QWidget::NoChildren );
	qdialog_radio_button->setPalettePropagation( QWidget::NoChildren );
	qdialog_radio_button->setText( "QDialog" );
	qdialog_radio_button->setAutoRepeat( FALSE );
	qdialog_radio_button->setAutoResize( FALSE );
	qdialog_radio_button->setChecked( TRUE );

	qframe_radio_button = new QRadioButton( this, "qframe_radio_button" );
	qframe_radio_button->setGeometry( 20, 70, 70, 30 );
	qframe_radio_button->setMinimumSize( 0, 0 );
	qframe_radio_button->setMaximumSize( 32767, 32767 );
	qframe_radio_button->setFocusPolicy( QWidget::TabFocus );
	qframe_radio_button->setBackgroundMode( QWidget::PaletteBackground );
	qframe_radio_button->setFontPropagation( QWidget::NoChildren );
	qframe_radio_button->setPalettePropagation( QWidget::NoChildren );
	qframe_radio_button->setText( "QFrame" );
	qframe_radio_button->setAutoRepeat( FALSE );
	qframe_radio_button->setAutoResize( FALSE );

	qtabdialog_radio_button = new QRadioButton( this, "qtabdialog_radio_button" );
	qtabdialog_radio_button->setGeometry( 110, 70, 100, 30 );
	qtabdialog_radio_button->setMinimumSize( 0, 0 );
	qtabdialog_radio_button->setMaximumSize( 32767, 32767 );
	qtabdialog_radio_button->setFocusPolicy( QWidget::TabFocus );
	qtabdialog_radio_button->setBackgroundMode( QWidget::PaletteBackground );
	qtabdialog_radio_button->setFontPropagation( QWidget::NoChildren );
	qtabdialog_radio_button->setPalettePropagation( QWidget::NoChildren );
	qtabdialog_radio_button->setText( "QTabDialog" );
	qtabdialog_radio_button->setAutoRepeat( FALSE );
	qtabdialog_radio_button->setAutoResize( FALSE );
	qtabdialog_radio_button->setEnabled(false);

	custom_radio_button = new QRadioButton( this, "custom_radio_button" );
	custom_radio_button->setGeometry( 20, 110, 100, 30 );
	custom_radio_button->setMinimumSize( 0, 0 );
	custom_radio_button->setMaximumSize( 32767, 32767 );
	custom_radio_button->setFocusPolicy( QWidget::TabFocus );
	custom_radio_button->setBackgroundMode( QWidget::PaletteBackground );
	custom_radio_button->setFontPropagation( QWidget::NoChildren );
	custom_radio_button->setPalettePropagation( QWidget::NoChildren );
	custom_radio_button->setText( i18n("Custom") );
	custom_radio_button->setAutoRepeat( FALSE );
	custom_radio_button->setAutoResize( FALSE );

	custom_prob_gbox = new QButtonGroup( this, "custom_prob_gbox" );
	custom_prob_gbox->setGeometry( 10, 210, 220, 160 );
	custom_prob_gbox->setMinimumSize( 0, 0 );
	custom_prob_gbox->setMaximumSize( 32767, 32767 );
	custom_prob_gbox->setFocusPolicy( QWidget::NoFocus );
	custom_prob_gbox->setBackgroundMode( QWidget::PaletteBackground );
	custom_prob_gbox->setFontPropagation( QWidget::NoChildren );
	custom_prob_gbox->setPalettePropagation( QWidget::NoChildren );
	custom_prob_gbox->setFrameStyle( 49 );
	custom_prob_gbox->setTitle( i18n("Custom Properties:") );
	custom_prob_gbox->setAlignment( 1 );

	custom_class_label = new QLabel( this, "custom_class_label" );
	custom_class_label->setGeometry( 20, 230, 100, 30 );
	custom_class_label->setMinimumSize( 0, 0 );
	custom_class_label->setMaximumSize( 32767, 32767 );
	custom_class_label->setFocusPolicy( QWidget::NoFocus );
	custom_class_label->setBackgroundMode( QWidget::PaletteBackground );
	custom_class_label->setFontPropagation( QWidget::NoChildren );
	custom_class_label->setPalettePropagation( QWidget::NoChildren );
	custom_class_label->setText(i18n("Custom Class:"));
	custom_class_label->setAlignment( 289 );
	custom_class_label->setMargin( -1 );
	
	custom_class_edit = new QLineEdit( this, "custom_class_edit" );
	custom_class_edit->setGeometry( 20, 260, 200, 30 );
	custom_class_edit->setMinimumSize( 0, 0 );
	custom_class_edit->setMaximumSize( 32767, 32767 );
	custom_class_edit->setFocusPolicy( QWidget::StrongFocus );
	custom_class_edit->setBackgroundMode( QWidget::PaletteBase );
	custom_class_edit->setFontPropagation( QWidget::NoChildren );
	custom_class_edit->setPalettePropagation( QWidget::NoChildren );
	custom_class_edit->setText( "" );
	custom_class_edit->setMaxLength( 32767 );
	custom_class_edit->setEchoMode( QLineEdit::Normal );
	custom_class_edit->setFrame( TRUE );

	custom_header_label = new QLabel( this, "custom_header_label" );
	custom_header_label->setGeometry( 20, 300, 100, 30 );
	custom_header_label->setMinimumSize( 0, 0 );
	custom_header_label->setMaximumSize( 32767, 32767 );
	custom_header_label->setFocusPolicy( QWidget::NoFocus );
	custom_header_label->setBackgroundMode( QWidget::PaletteBackground );
	custom_header_label->setFontPropagation( QWidget::NoChildren );
	custom_header_label->setPalettePropagation( QWidget::NoChildren );
	custom_header_label->setText( i18n("Custom Header:") );
	custom_header_label->setAlignment( 289 );
	custom_header_label->setMargin( -1 );
	
	custom_header_edit = new QLineEdit( this, "custom_header_edit" );
	custom_header_edit->setGeometry( 20, 330, 200, 30 );
	custom_header_edit->setMinimumSize( 0, 0 );
	custom_header_edit->setMaximumSize( 32767, 32767 );
	custom_header_edit->setFocusPolicy( QWidget::StrongFocus );
	custom_header_edit->setBackgroundMode( QWidget::PaletteBase );
	custom_header_edit->setFontPropagation( QWidget::NoChildren );
	custom_header_edit->setPalettePropagation( QWidget::NoChildren );
	custom_header_edit->setText( "" );
	custom_header_edit->setMaxLength( 32767 );
	custom_header_edit->setEchoMode( QLineEdit::Normal );
	custom_header_edit->setFrame( TRUE );
	
	QButtonGroup* qtarch_ButtonGroup_2;
	qtarch_ButtonGroup_2 = new QButtonGroup( this, "ButtonGroup_2" );
	qtarch_ButtonGroup_2->setGeometry( 250, 10, 290, 190 );
	qtarch_ButtonGroup_2->setMinimumSize( 0, 0 );
	qtarch_ButtonGroup_2->setMaximumSize( 32767, 32767 );
	qtarch_ButtonGroup_2->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_2->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_2->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_2->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_2->setFrameStyle( 49 );
	qtarch_ButtonGroup_2->setTitle( i18n("Files:") );
	qtarch_ButtonGroup_2->setAlignment( 1 );
	
	QLabel* qtarch_Label_7;
	qtarch_Label_7 = new QLabel( this, "Label_7" );
	qtarch_Label_7->setGeometry( 260, 30, 100, 30 );
	qtarch_Label_7->setMinimumSize( 0, 0 );
	qtarch_Label_7->setMaximumSize( 32767, 32767 );
	qtarch_Label_7->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_7->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_7->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_7->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_7->setText(i18n("Classname:"));
	qtarch_Label_7->setAlignment( 289 );
	qtarch_Label_7->setMargin( -1 );

	classname_edit = new QLineEdit( this, "classname_edit" );
	classname_edit->setGeometry( 390, 30, 140, 30 );
	classname_edit->setMinimumSize( 0, 0 );
	classname_edit->setMaximumSize( 32767, 32767 );
	classname_edit->setFocusPolicy( QWidget::StrongFocus );
	classname_edit->setBackgroundMode( QWidget::PaletteBase );
	classname_edit->setFontPropagation( QWidget::NoChildren );
	classname_edit->setPalettePropagation( QWidget::NoChildren );
	classname_edit->setText( "" );
 	classname_edit->setMaxLength( 32767 );
	classname_edit->setEchoMode( QLineEdit::Normal );
	classname_edit->setFrame( TRUE );

	QLabel* qtarch_Label_3;
	qtarch_Label_3 = new QLabel( this, "Label_3" );
	qtarch_Label_3->setGeometry( 260, 70, 130, 30 );
	qtarch_Label_3->setMinimumSize( 0, 0 );
	qtarch_Label_3->setMaximumSize( 32767, 32767 );
	qtarch_Label_3->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_3->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_3->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_3->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_3->setText(i18n("Header File:") );
	qtarch_Label_3->setAlignment( 289 );
	qtarch_Label_3->setMargin( -1 );

	header_edit = new QLineEdit( this, "header_edit" );
	header_edit->setGeometry( 390, 70, 140, 30 );
	header_edit->setMinimumSize( 0, 0 );
	header_edit->setMaximumSize( 32767, 32767 );
	header_edit->setFocusPolicy( QWidget::StrongFocus );
	header_edit->setBackgroundMode( QWidget::PaletteBase );
	header_edit->setFontPropagation( QWidget::NoChildren );
	header_edit->setPalettePropagation( QWidget::NoChildren );
	header_edit->setText( "" );
	header_edit->setMaxLength( 32767 );
	header_edit->setEchoMode( QLineEdit::Normal );
	header_edit->setFrame( TRUE );

	QLabel* qtarch_Label_4;
	qtarch_Label_4 = new QLabel( this, "Label_4" );
	qtarch_Label_4->setGeometry( 260, 110, 130, 30 );
	qtarch_Label_4->setMinimumSize( 0, 0 );
	qtarch_Label_4->setMaximumSize( 32767, 32767 );
	qtarch_Label_4->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_4->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_4->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_4->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_4->setText(i18n("C++ File:"));
	qtarch_Label_4->setAlignment( 289 );
	qtarch_Label_4->setMargin( -1 );

	cpp_edit = new QLineEdit( this, "cpp_edit" );
	cpp_edit->setGeometry( 390, 110, 140, 30 );
	cpp_edit->setMinimumSize( 0, 0 );
	cpp_edit->setMaximumSize( 32767, 32767 );
	cpp_edit->setFocusPolicy( QWidget::StrongFocus );
	cpp_edit->setBackgroundMode( QWidget::PaletteBase );
	cpp_edit->setFontPropagation( QWidget::NoChildren );
	cpp_edit->setPalettePropagation( QWidget::NoChildren );
	cpp_edit->setText( "" );
	cpp_edit->setMaxLength( 32767 );
	cpp_edit->setEchoMode( QLineEdit::Normal );
	cpp_edit->setFrame( TRUE );

	QLabel* qtarch_Label_5;
	qtarch_Label_5 = new QLabel( this, "Label_5" );
	qtarch_Label_5->setGeometry( 260, 150, 130, 30 );
	qtarch_Label_5->setMinimumSize( 0, 0 );
	qtarch_Label_5->setMaximumSize( 32767, 32767 );
	qtarch_Label_5->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_5->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_5->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_5->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_5->setText(i18n("Data File:") );
	qtarch_Label_5->setAlignment( 289 );
	qtarch_Label_5->setMargin( -1 );
	
	data_edit = new QLineEdit( this, "data_edit" );
	data_edit->setGeometry( 390, 150, 140, 30 );
	data_edit->setMinimumSize( 0, 0 );
	data_edit->setMaximumSize( 32767, 32767 );
	data_edit->setFocusPolicy( QWidget::StrongFocus );
	data_edit->setBackgroundMode( QWidget::PaletteBase );
	data_edit->setFontPropagation( QWidget::NoChildren );
	data_edit->setPalettePropagation( QWidget::NoChildren );
	data_edit->setText( "" );
	data_edit->setMaxLength( 32767 );
	data_edit->setEchoMode( QLineEdit::Normal );
	data_edit->setFrame( TRUE );


  QButtonGroup* qtarch_ButtonGroup_3;
  qtarch_ButtonGroup_3 = new QButtonGroup( this, "ButtonGroup_3" );
	qtarch_ButtonGroup_3->setGeometry( 250, 210, 290, 160 );
	qtarch_ButtonGroup_3->setMinimumSize( 0, 0 );
	qtarch_ButtonGroup_3->setMaximumSize( 32767, 32767 );
	qtarch_ButtonGroup_3->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_3->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_3->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_3->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_3->setFrameStyle( 49 );
	qtarch_ButtonGroup_3->setTitle( i18n("Location:") );
	qtarch_ButtonGroup_3->setAlignment( 1 );
	
	QLabel* qtarch_Label_6;
	qtarch_Label_6 = new QLabel( this, "Label_6" );
	qtarch_Label_6->setGeometry( 260, 260, 100, 30 );
	qtarch_Label_6->setMinimumSize( 0, 0 );
	qtarch_Label_6->setMaximumSize( 32767, 32767 );
	qtarch_Label_6->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_6->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_6->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_6->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_6->setText(i18n("Directory:"));
	qtarch_Label_6->setAlignment( 289 );
	qtarch_Label_6->setMargin( -1 );

	loc_edit = new QLineEdit( this, "loc_edit" );
	loc_edit->setGeometry( 260, 290, 230, 30 );
	loc_edit->setMinimumSize( 0, 0 );
	loc_edit->setMaximumSize( 32767, 32767 );
	loc_edit->setFocusPolicy( QWidget::StrongFocus );
	loc_edit->setBackgroundMode( QWidget::PaletteBase );
	loc_edit->setFontPropagation( QWidget::NoChildren );
	loc_edit->setPalettePropagation( QWidget::NoChildren );
	loc_edit->setText( "" );
	loc_edit->setMaxLength( 32767 );
	loc_edit->setEchoMode( QLineEdit::Normal );
	loc_edit->setFrame( TRUE );

	loc_button = new QPushButton( this, "loc_button" );
	loc_button->setGeometry( 500, 290, 30, 30 );
	loc_button->setMinimumSize( 0, 0 );
	loc_button->setMaximumSize( 32767, 32767 );
	loc_button->setFocusPolicy( QWidget::TabFocus );
	loc_button->setBackgroundMode( QWidget::PaletteBackground );
	loc_button->setFontPropagation( QWidget::NoChildren );
	loc_button->setPalettePropagation( QWidget::NoChildren );
	QPixmap pix = BarIcon("open");
	loc_button->setPixmap(pix);
	loc_button->setAutoRepeat( FALSE );
	loc_button->setAutoResize( FALSE );

	qtarch_ButtonGroup_1->insert( qdialog_radio_button );
	qtarch_ButtonGroup_1->insert( qwidget_radio_button );
	qtarch_ButtonGroup_1->insert( qtabdialog_radio_button );
	qtarch_ButtonGroup_1->insert( qframe_radio_button );
	qtarch_ButtonGroup_1->insert( custom_radio_button );
	
	qtarch_ButtonGroup_3->insert( loc_button );
	
	ok_button = new QPushButton( this, "ok_button" );
	ok_button->setGeometry( 150, 400, 100, 30 );
	ok_button->setMinimumSize( 0, 0 );
	ok_button->setMaximumSize( 32767, 32767 );
	ok_button->setFocusPolicy( QWidget::TabFocus );
	ok_button->setBackgroundMode( QWidget::PaletteBackground );
	ok_button->setFontPropagation( QWidget::NoChildren );
	ok_button->setPalettePropagation( QWidget::NoChildren );
	ok_button->setText(i18n("OK"));
	ok_button->setAutoRepeat( FALSE );
	ok_button->setAutoResize( FALSE );
	ok_button->setDefault( TRUE );

	cancel_button = new QPushButton( this, "cancel_button" );
	cancel_button->setGeometry( 300, 400, 100, 30 );
	cancel_button->setMinimumSize( 0, 0 );
	cancel_button->setMaximumSize( 32767, 32767 );
	cancel_button->setFocusPolicy( QWidget::TabFocus );
 	cancel_button->setBackgroundMode( QWidget::PaletteBackground );
	cancel_button->setFontPropagation( QWidget::NoChildren );
	cancel_button->setPalettePropagation( QWidget::NoChildren );
	cancel_button->setText(i18n("Cancel"));
 	cancel_button->setAutoRepeat( FALSE );
	cancel_button->setAutoResize( FALSE );

	resize( 550,450 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
	
	//set custom disabled
	setCustomPropsEnabled(false);

	// other settings
	loc_edit->setText(prj->getProjectDir() + prj->getSubDir());
	setCaption(i18n("Generate Dialog... (C++ Sourcecode)"));

	header_modified = false;
	source_modified = false;
	data_modified = false;
	
	// connections
	connect(classname_edit,SIGNAL(textChanged(const QString&)),SLOT(slotClassEditChanged(const QString&)));
	connect(header_edit,SIGNAL(textChanged(const QString&)),SLOT(slotHeaderEditChanged(const QString&)));
	connect(cpp_edit,SIGNAL(textChanged(const QString&)),SLOT(slotSourceEditChanged(const QString&)));
	connect(data_edit,SIGNAL(textChanged(const QString&)),SLOT(slotDataEditChanged(const QString&)));
	
	connect(ok_button,SIGNAL(clicked()),SLOT(slotOKClicked()));
	connect(cancel_button,SIGNAL(clicked()),SLOT(reject()));
	connect(loc_button,SIGNAL(clicked()),SLOT(slotLocButtonClicked()));
	
	connect(qdialog_radio_button,SIGNAL(clicked()),SLOT(slotRadioButtonClicked()));
	connect(qwidget_radio_button,SIGNAL(clicked()),SLOT(slotRadioButtonClicked()));
	connect(qtabdialog_radio_button,SIGNAL(clicked()),SLOT(slotRadioButtonClicked()));
	connect(qframe_radio_button,SIGNAL(clicked()),SLOT(slotRadioButtonClicked()));
	connect(custom_radio_button,SIGNAL(clicked()),SLOT(slotRadioButtonClicked()));
}
KDlgNewDialogDlg::~KDlgNewDialogDlg(){
}

void KDlgNewDialogDlg::slotOKClicked(){
  if(QString(classname_edit->text()) == "") {
    KMessageBox::error(this,i18n("You must enter a classname!"));return;
  }
  if(QString(header_edit->text()) == "") {
    KMessageBox::error(this,i18n("You must enter a name for the header-file!"));return;
  }
  if(QString(cpp_edit->text()) == "") {
    KMessageBox::error(this,i18n("You must enter a name for the implementation-file!"));return;
  }
  if(QString(data_edit->text()) == "") {
    KMessageBox::error(this,i18n("You must enter a name for the data-file!"));return;
  }
  if(custom_radio_button->isChecked() &&  QString(custom_header_edit->text()) == ""){
    KMessageBox::error(this,i18n("You must enter a name for the header-file\n for the custom class!"));return;
  }
  if(custom_radio_button->isChecked() &&  QString(custom_class_edit->text()) == ""){
    KMessageBox::error(this,i18n("You must enter a name for the custom class!"));return;
  }
  if(QString(loc_edit->text()).contains(project->getProjectDir()) == 0 ){
    KMessageBox::error(this,i18n("You must choose a location,that is in your project-dir!"));return;
  }
  accept();
}
void KDlgNewDialogDlg::slotLocButtonClicked(){
  QString str=  KFileDialog::getExistingDirectory(loc_edit->text());
  if(!str.isEmpty()){
    loc_edit->setText(str);
  }
}
void KDlgNewDialogDlg::slotRadioButtonClicked(){
  if(custom_radio_button->isChecked()){
    setCustomPropsEnabled(true);
  }
  else{
    setCustomPropsEnabled(false);
  }
}

void KDlgNewDialogDlg::setCustomPropsEnabled(bool show){
  if(show){
    custom_header_label->setEnabled(true);
    custom_class_label->setEnabled(true);
    custom_header_edit->setEnabled(true);
    custom_class_edit->setEnabled(true);
    custom_prob_gbox->setEnabled(true);
  }
  else{
    custom_header_label->setEnabled(false);
    custom_class_label->setEnabled(false);
    custom_header_edit->setEnabled(false);
    custom_class_edit->setEnabled(false);
    custom_prob_gbox->setEnabled(false);
  }
}
void  KDlgNewDialogDlg::slotClassEditChanged(const QString& text){
  QString str = text;
  if(!header_modified){
    header_edit->setText(str.lower() + ".h");
  }
  if(!source_modified){
    cpp_edit->setText(str.lower() +".cpp");
  }
  if(!data_modified){
    data_edit->setText(str.lower() +"data.cpp");
  }
  
}
void KDlgNewDialogDlg::slotHeaderEditChanged(const QString&){
  if(header_edit->hasFocus()){
    header_modified = true;
  }
}
void KDlgNewDialogDlg::slotSourceEditChanged(const QString&){
  if(cpp_edit->hasFocus()){
    source_modified = true;
  }
}
void KDlgNewDialogDlg::slotDataEditChanged(const QString&){
  if(data_edit->hasFocus()){
    data_modified = true;
  }
}
QString KDlgNewDialogDlg::getBaseClass(){
  if(qwidget_radio_button->isChecked()) return "QWidget";
  if(qframe_radio_button->isChecked()) return "QFrame";
  if(qdialog_radio_button->isChecked()) return "QDialog";
  if(qtabdialog_radio_button->isChecked()) return "QTabDialog";
  if(custom_radio_button->isChecked()) return custom_class_edit->text();
  return "QDialog";
}
QString  KDlgNewDialogDlg::getBaseClassHeader(){
  if(qwidget_radio_button->isChecked()) return "qwidget.h";
  if(qframe_radio_button->isChecked()) return "qframe.h";
  if(qdialog_radio_button->isChecked()) return "qdialog.h";
  if(qtabdialog_radio_button->isChecked()) return "qtabdialog.h";
  if(custom_radio_button->isChecked()) return custom_header_edit->text();
  return QString("qdialog.h");
}




