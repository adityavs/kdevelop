/***************************************************************************
                 ctoolclass.h - some methods,that would be normal global,but I
		              hate global function :-)
                             -------------------                                         

    begin                : 20 Jan 1999                        
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

#ifndef CTOOLCLASS_H
#define CTOOLCLASS_H

#include <stdlib.h>

#include <qstring.h>

/**
 * some methods,that would be normaly global,but I hate global functions :-)
 * @author Sandy Meier
 */
class CToolClass {
public:
  /** search the program in the $PATH*/
  static bool searchProgram(QString name, bool allowWarningMsg=true);
  /** same as above but without warning message*/
  static bool searchInstProgram(QString name) { return searchProgram( name, false); };

  /** return the absolute filename if found, otherwise ""*/
  static QString findProgram(QString name);
  /** returns the relative path, to get from source_path to destpath (both are absolute)
      for examples:
      source_path: /home/smeier/testprj/
      dest_path:   /usr/lib/
      relative_path = ../../../user/lib/
   */
  static QString getRelativePath(QString source_dir,QString dest_dir);
};

#endif


