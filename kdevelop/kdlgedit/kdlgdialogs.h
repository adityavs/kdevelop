/***************************************************************************
                          kdlgdialogs.h  -  description                              
                             -------------------                                         
    begin                : Wed Mar 17 1999                                           
    copyright            : (C) 1999 Sandy Meier
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


#ifndef KDLGDIALOGS_H
#define KDLGDIALOGS_H

#include "../ctreeview.h"
#include <kpopupmenu.h>
#include <qwidget.h>

class CProject;
class QListViewItem;

/**
  *@author Sandy Meier <smeier@rz.uni-potsdam.de>
  */

class KDlgDialogs : public CTreeView  {
  Q_OBJECT

public:
  KDlgDialogs(QWidget *parent=0, const char *name=0);
  ~KDlgDialogs();
  void refresh(CProject* prj);
  /** Initialize popupmenus. */
  virtual void initPopups();
  /** Get the current popupmenu. */
  virtual KPopupMenu *getCurrentPopup();
  
signals:
  void kdlgdialogsSelected(QString dialog_file);
  void newDialog();
  // *.kdevdlg
  void deleteDialog(QString dialog_filename);

public  slots:
  void slotSelectionChanged( QListViewItem* item);
  void slotNewDialog();
  void slotDeleteDialog();

protected:
  CProject*  project;
  // only new
  KPopupMenu dialog_pop;
  // new and delete
  KPopupMenu dialog_pop2;

  QString current_dialog;
};

#endif
