/***************************************************************************
                          cbugreportdlg.h  -  description                              
                             -------------------                                         
    begin                : Thu May 6 1999                                           
    copyright            : (C) 1999 by Stefan Bartel                         
    email                : bartel@rz.uni-potsdam.de                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef CBUGREPORTDLG_H
#define CBUGREPORTDLG_H

#include "structdef.h"

#include <qtabdialog.h>

class QLineEdit;
class QComboBox;
class QRadioButton;
class QMultiLineEdit;


/** creates a dialog to fill in a bugreport and sends it to the KDevelop Team
  *@author Stefan Bartel 
  */

class CBugReportDlg : public QTabDialog  {
   Q_OBJECT

public:
	CBugReportDlg(QWidget *parent,
	                const char *name,
	                TBugReportInfo buginfo,
	                const QString& bug_email);
	~CBugReportDlg();
	QString name;
	QString email_address;
	QString qt_version;
	QString kde_version;
	QString os;
	QString compiler;
	QString sendmail_command;
	
protected:
		// Attention
		QLineEdit* sendmail_edit;
    // General Information
    QLineEdit* name_edit;
    QLineEdit* email_edit;
    QLineEdit* subject_edit;
    QComboBox* class_combo;
    QRadioButton* priority_low;
    QRadioButton* priority_medium;
    QRadioButton* priority_high;
    QRadioButton* severity_harmless;
    QRadioButton* severity_serious;
    QRadioButton* severity_critical;
    QComboBox* location_combo;
    // System Information
    QLineEdit* kdevelop_version_edit;
    QLineEdit* kde_version_edit;
    QLineEdit* qt_version_edit;
    QLineEdit* os_edit;
    QLineEdit* compiler_edit;
    QMultiLineEdit* misc_mledit;
    // Problem Description
    QMultiLineEdit* description_mledit;
    QMultiLineEdit* repeat_mledit;
    QMultiLineEdit* fix_mledit;

    QString strBugID;
    QString BugEmail;
   

    //* sends the bugreport via the program "mail" to the bugreport-address of the KDevelop team
    bool sendEmail();
    //* generates the body of the bugreport-mail ans saves it in $HOME/.kde/share/apps/kdevelop
    bool generateEmail();

    protected slots:
     void ok();
};

#endif

