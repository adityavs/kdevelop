/***************************************************************************
               ceditwidget.cpp  -  a abstraction layer for an editwidget
                             -------------------

    begin                : 23 Aug 1998
    copyright            : (C) 1998 by Sandy Meier
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

#include "ceditwidget.h"
#include "cppcodecompletion.h"

#include <kdebug.h>
#include <kregexp.h>

#include "kwdoc.h"
#include "highlight.h"
#include "cproject.h"
#include "docviewman.h"

#include <qpopupmenu.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qmap.h>
#include <assert.h>
#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>

#include "resource.h"

HlManager hlManager; //highlight manager


static QValueList<CompletionEntry> getAllWords( const QString& text,
                                                const QString& prefix )
{
    QMap<QString, bool> map;
    QValueList<CompletionEntry> entries;
#if QT_VERSION < 300
    QRegExp rx(QString("[^a-zA-Z0-9_]") + prefix + "[a-zA-Z0-9_]+[^a-zA-Z0-9_]");
#else
    QRegExp rx( QString("\\b") + prefix + "[a-zA-Z0-9_]+\\b" );
#endif
    int idx = 0;
    int pos = 0;
    int len = 0;
    while( (pos = rx.match(text, idx, &len)) != -1 ){
#if QT_VERSION < 300
        QString word = text.mid( pos+1, len-2 );
#else
	QString word = text.mid( pos, len );
#endif
        if( map.find(word) == map.end() ){
            CompletionEntry e;
            e.text = word;
            entries << e;
            map[ word ] = TRUE;
        }
        idx = pos + len + 1;
    }
    return entries;
}


/*********************************************************************
 *                                                                   *
 *                     CREATION RELATED METHODS                      *
 *                                                                   *
 ********************************************************************/

/*------------------------------------------- CEditWidget::CEditWidget()
 * CEditWidget()
 *   Constructor.
 *
 * Parameters:
 *   parent         Parent widget.
 *   name           The name of this widget.
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CEditWidget::CEditWidget(QWidget* parent, const char* name, KWriteDoc* doc, CClassStore* pStore) :
    KWrite(doc, parent, name), m_pStore( pStore )
{
  setFocusProxy (kWriteView);
  pop = new QPopupMenu();
  //  pop->insertItem(i18n("Open: "),this,SLOT(open()),0,6);
  pop->insertItem(SmallIconSet("undo"),i18n("Undo"),this,SLOT(undo()),0,ID_EDIT_UNDO);
  pop->insertItem(SmallIconSet("redo"),i18n("Redo"),this,SLOT(redo()),0,ID_EDIT_REDO);
  pop->insertSeparator();
  pop->insertItem(SmallIconSet("editcut"),i18n("Cut"),this,SLOT(cut()),0,ID_EDIT_CUT);
  pop->insertItem(SmallIconSet("editcopy"),i18n("Copy"),this,SLOT(copy()),0,ID_EDIT_COPY);
  pop->insertItem(SmallIconSet("editpaste"),i18n("Paste"),this,SLOT(paste()),0,ID_EDIT_PASTE);
  pop->setItemEnabled(ID_EDIT_CUT,false);
  pop->setItemEnabled(ID_EDIT_COPY,false);
  pop->setItemEnabled(ID_EDIT_PASTE,false);

  pop->insertSeparator();
  pop->insertItem(/*SmallIconSet("grep"),*/i18n("Switch Header/Source"),this,SLOT(slotEmitTagSwitchTo()),0,ID_EDIT_TAGS_SWITCH);
  pop->insertItem(/*SmallIconSet("grep"),*/i18n("Open File"),this,SLOT(slotEmitTagOpenFile()),0,ID_EDIT_TAGS_OPEN);
  pop->insertItem(/*SmallIconSet("grep"),*/i18n("Goto Definition"),this,SLOT(slotEmitTagDefinition()),0,ID_EDIT_TAGS_DEFINITION);
  pop->insertItem(/*SmallIconSet("grep"),*/i18n("Goto Declaration"),this,SLOT(slotEmitTagDeclaration()),0,ID_EDIT_TAGS_DECLARATION);

  pop->insertSeparator();
  pop->insertItem(SmallIconSet("grep"),"",this,SLOT(slotGrepText()),0,ID_EDIT_SEARCH_IN_FILES);
  pop->insertItem(SmallIconSet("help"),"",this,SLOT(slotLookUp()),0,ID_HELP_SEARCH_TEXT);
  pop->insertItem(SmallIconSet("help"),"",this,SLOT(slotManpage()),0,ID_HELP_MANPAGE);
//  bookmarks.setAutoDelete(true);
  pop->insertSeparator();
  pop->insertItem(SmallIconSet("dbgrunto"),i18n("Run to cursor"),this,SLOT(slotRunToCursor()),0,ID_EDIT_RUN_TO_CURSOR);
  pop->insertItem(SmallIconSet("dbgwatchvar"),"",this,SLOT(slotAddWatchVariable()),0,ID_EDIT_ADD_WATCH_VARIABLE);

  m_pCodeCompletion = new CppCodeCompletion( this, m_pStore );

  // connect( kWriteDoc, SIGNAL(textChanged()), this, SLOT(slotTextChanged()) );
}

/*-------------------------------------- CEditWidget::~CEditWidget()
 * ~CEditWidget()
 *   Destructor.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
CEditWidget::~CEditWidget() {
}

/*********************************************************************
 *                                                                   *
 *                          PUBLIC METHODS                           *
 *                                                                   *
 ********************************************************************/

/*--------------------------------------------- CEditWidget::getName()
 * getName()
 *   Get the current filename.
 *
 * Parameters:
 *   -
 * Returns:
 *   QString      The current filename.
 *-----------------------------------------------------------------*/
QString CEditWidget::getName(){
  QString s(QString::null);
  if (kWriteDoc)
    s=kWriteDoc->fileName();
  if (s.isNull()) s = "";
  return s;
}

/*--------------------------------------------- CEditWidget::loadFile()
 * loadFile()
 *   Loads the file given in name into the editor.
 *
 * Parameters:
 *   filename     Name of the file to load.
 *   mode         Current not used.
 *
 * Returns:
 *   int          The line at which the file got loaded.
 *-----------------------------------------------------------------*/
//int CEditWidget::loadFile(QString filename, int /*mode*/) {
//  KWrite::loadFile(filename);
//  return 0;
//}

/*------------------------------------------- CEditWidget::setFocus()
 * setFocus()
 *   Make the edit view get the input focus.
 *
 * Parameters:
 *   -
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CEditWidget::setFocus(){
  KWrite::setFocus();
  kWriteView->setFocus();
}

/*--------------------------------------------- CEditWidget::gotoPos()
 * gotoPos()
 *   Goto the start of the line that contains the specified position.
 *
 * Parameters:
 *   pos          Position to go to
 *   text         The editwidget text.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CEditWidget::gotoPos(int pos,QString text_str){

  //  kdDebug() << endl << "POS: " << pos;
  // calculate the line
  QString last_textpart = text_str.right(text_str.length()-pos); // the second part of the next,after the pos
  int line = text_str.contains("\n") - last_textpart.contains("\n");
  //  kdDebug() << endl << "LINE:" << line;
  setCursorPosition(line,0);
  setFocus();
}

/*------------------------------------- CEditWidget::deleteInterval()
 * deleteInterval()
 *   Delete an interval of lines.
 *
 * Parameters:
 *   startAt       Line to start deleting at.
 *   endAt         Line to end deleting at.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CEditWidget::deleteInterval( uint startAt, uint endAt )
{
  assert( startAt <= endAt );

  QCString txt = text();
  int startPos = getLinePos( txt, startAt );

  // We find the end of the line by increasing the linecounter and
  // subtracting by one to make pos point at the last \n character.
  int endPos = getLinePos( txt, endAt + 1 );
  endPos--;

  // Remove the interval.
  txt.remove( startPos, endPos - startPos );

  // Update the editwidget with the new text.
  setText( txt );

  // Set the buffer as modified.
  toggleModified( true );
}

/*------------------------------------- CEditWidget::deleteInterval()
 * deleteInterval()
 *   Insert the string at the given line.
 *
 * Parameters:
 *   toInsert      Text to insert.
 *   atLine        Line to start inserting the text.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CEditWidget::insertAtLine( const char *toInsert, uint atLine )
{
  assert( toInsert != NULL );
//  assert( atLine >= 0 );      uint is always >=0 ???

  QCString txt = text();
  int pos = getLinePos( txt, atLine );
  txt.insert( pos, toInsert );

  // Update the editwidget with the new text.
  setText( txt );

  // Set the buffer as modified.
  toggleModified( true );
}

/*---------------------------------------------- CEditWidget::append()
 * append()
 *   Append a text at the end of the file.
 *
 * Parameters:
 *   toAdd         Text to append.
 *
 * Returns:
 *   -
 *-----------------------------------------------------------------*/
void CEditWidget::append( const char *toAdd )
{
  QCString txt = text();
  txt.append( toAdd );
  setText( txt );
}

/*********************************************************************
 *                                                                   *
 *                          PROTECTED METHODS                        *
 *                                                                   *
 ********************************************************************/

/*------------------------------------------ CEditWidget::getLinePos()
 * getLinePos()
 *   Get the startposition in the buffer of a line
 *
 * Parameters:
 *   buf        Buffer to search in.
 *   aLine      Linenumber to find position to.
 *
 * Returns:
 *   int        The 0-based bufferposition or -1 if not found.
 *-----------------------------------------------------------------*/
int CEditWidget::getLinePos( const char *buf, uint aLine )
{
  uint line=0;
  int pos=0;

  while( line < aLine )
  {
    pos++;
    if( buf[ pos ] == '\n' )
      line++;
  }

  // Pos currently points at the last \n, that's why we add 1.
  return pos + 1;
}

void CEditWidget::mousePressEvent(QMouseEvent* event){
  if(event->button() == RightButton){

    if(event->state() & ControlButton) {
      emit bufferMenu(this->mapToGlobal(event->pos()));
      return;
    }

    int state;
    int pos;
    state = undoState();
    //undo
    if(state & 1){
      pop->setItemEnabled(ID_EDIT_UNDO,true);
    }
    else{
      pop->setItemEnabled(ID_EDIT_UNDO,false);
    }
    //redo
    if(state & 2){
      pop->setItemEnabled(ID_EDIT_REDO,true);
    }
    else{
      pop->setItemEnabled(ID_EDIT_REDO,false);
    }

    // check for highlighted text first
    QString str = markedText();
    if(!str.isEmpty()){
      pop->setItemEnabled(ID_EDIT_CUT,true);
      pop->setItemEnabled(ID_EDIT_COPY,true);
    }
    else{
      pop->setItemEnabled(ID_EDIT_CUT,false);
      pop->setItemEnabled(ID_EDIT_COPY,false);
    }

    QClipboard *cb = kapp->clipboard();
    QString text=cb->text();
    if(text.isEmpty())
      pop->setItemEnabled(ID_EDIT_PASTE,false);
    else
      pop->setItemEnabled(ID_EDIT_PASTE,true);

    if(str == ""){
      str = word(event->x()- (iconBorderWidth-2) ,event->y());
    }

    str.replace(QRegExp("^\n"), "");
    pos=str.find("\n");
    if (pos>-1)
     str=str.left(pos);

    /* this is to shorten the UI feedback, we will continue
     * to use searchtext for the actual searches
     */
    searchtext = str;
    if(str.length() > 20 ){
      str = str.left(20) + "...";
    }

    pop->setItemEnabled(ID_HELP_SEARCH_TEXT, !str.isEmpty());
    pop->setItemEnabled(ID_EDIT_SEARCH_IN_FILES, !str.isEmpty());
    pop->setItemEnabled(ID_HELP_MANPAGE, !str.isEmpty());

    pop->setItemEnabled(ID_EDIT_ADD_WATCH_VARIABLE, !str.isEmpty());  // TODO: only enable in debugger mode
    pop->setItemEnabled(ID_EDIT_RUN_TO_CURSOR, true);	                // TODO: only enable in debugger mode
    pop->setItemEnabled(ID_EDIT_STEP_OUT_OFF, true);	                // TODO: only enable in debugger mode

    pop->setItemEnabled(ID_EDIT_TAGS_SWITCH,true);
    pop->setItemEnabled(ID_EDIT_TAGS_OPEN,false);
    pop->setItemEnabled(ID_EDIT_TAGS_DEFINITION,false);
    pop->setItemEnabled(ID_EDIT_TAGS_DECLARATION,false);
    CProject* prj = currentProject();
    if ((!str.isEmpty()) && prj)
    {
      CTagsDataBase& tagsDB = prj->ctagsDataBase();
      if (tagsDB.is_initialized()) {
        kdDebug() << "found tags data base\n";
        // remember that we are searching for searchtext not for str!
        const CTagList* taglist = tagsDB.ctaglist(searchtext);
        if (taglist)
        {
          int ntags = taglist->count();
          kdDebug() << "found: " << ntags << " entries for: "
                    << searchtext << "\n";
          if (taglist->nFileTags()) {
            kdDebug() << "number of FileTags: " << taglist->nFileTags() << "\n";
            pop->setItemEnabled(ID_EDIT_TAGS_OPEN,true);
            pop->changeItem(/*SmallIconSet("help"),*/i18n("Open File: ") + str,ID_EDIT_TAGS_OPEN);
          }
          if (taglist->nDefinitionTags()) {
            kdDebug() << "number of DefinitionTags: " << taglist->nDefinitionTags() << "\n";
            pop->setItemEnabled(ID_EDIT_TAGS_DEFINITION,true);
            pop->changeItem(/*SmallIconSet("help"),*/i18n("Goto Definition: ") + str,ID_EDIT_TAGS_DEFINITION);
          }
          if (taglist->nDeclarationTags()) {
            kdDebug() << "number of DeclarationTags: " << taglist->nDeclarationTags() << "\n";
            pop->setItemEnabled(ID_EDIT_TAGS_DECLARATION,true);
            pop->changeItem(/*SmallIconSet("help"),*/i18n("Goto Declaration: ") + str,ID_EDIT_TAGS_DECLARATION);
          }
        }
      }
    }
    pop->changeItem(SmallIconSet("grep"),i18n("grep: ") + str,ID_EDIT_SEARCH_IN_FILES); // the grep entry
    pop->changeItem(SmallIconSet("help"),i18n("look up: ") + str,ID_HELP_SEARCH_TEXT); // the lookup entry
    pop->changeItem(SmallIconSet("help"),i18n("manpage: ") + str,ID_HELP_MANPAGE);
    pop->changeItem(SmallIconSet("dbgwatchvar"),i18n("Watch: ") + str,ID_EDIT_ADD_WATCH_VARIABLE); // the lookup entry

    pop->popup(this->mapToGlobal(event->pos()));
  }
}

/*********************************************************************
 *                                                                   *
 *                              SLOTS                                *
 *                                                                   *
 ********************************************************************/

void CEditWidget::slotEmitTagSwitchTo(){
  emit tagSwitchTo();
}
void CEditWidget::slotEmitTagOpenFile(){
  emit tagOpenFile(searchtext);
}
void CEditWidget::slotEmitTagDefinition(){
  emit tagDefinition(searchtext);
}
void CEditWidget::slotEmitTagDeclaration(){
  emit tagDeclaration(searchtext);
}
void CEditWidget::slotManpage(){
  emit manpage("man:/"+searchtext); //   +"(3)");
}
void CEditWidget::slotLookUp(){
  emit lookUp(searchtext);
}
void CEditWidget::slotGrepText(){
  emit grepText(searchtext);
}

// Support for debugging the current project
#include <qfileinfo.h>

void CEditWidget::slotRunToCursor(){
    QFileInfo fi( getName() );
    emit runToCursor(fi.fileName(), currentLine()+1);
}

void CEditWidget::slotAddWatchVariable(){
  if (!searchtext.isEmpty())
    emit addWatchVariable(searchtext);
}

void CEditWidget::expandText()
{
    kdDebug() << "CEditWidget::expandText()" << endl;

    QString prefix = currentWord();
    if( !prefix.isEmpty() ){
        QValueList<CompletionEntry> entries = getAllWords( text(),
                                                           prefix );
#if QT_VERSION < 300
        if( entries.count() == 1 ){
            insertText( entries[ 0 ].text.mid(currentWord().length()) );
        } else if( entries.count() ){
#else
        if( entries.size() == 1 ){
            insertText( entries[ 0 ].text.mid(currentWord().length()) );
        } else if( entries.size() ){
#endif
            m_pCodeCompletion->showCompletionBox( entries, prefix.length() );
        }
    }
}

void CEditWidget::completeText()
{
    m_pCodeCompletion->completeText();
}

#include "ceditwidget.moc"
