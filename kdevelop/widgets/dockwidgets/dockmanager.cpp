/***************************************************************************
                dockmanager.cpp  -  Impl. DockWidget,DockManager
                             -------------------
    begin                : Now 21 21:08:00 1999
    copyright            : (C) 2000 by Judin Max (novaprint@mtu-net.ru)
    email                : novaprint@mtu-net.ru

		improved/changed by	 : Falk Brettschneider	(Jan 30 17:52 MET 2000)
													 email: gigafalk@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define DOCK_CONFIG_VERSION "0.0.2"

#include <kconfig.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <kapp.h>

#include <qobjcoll.h>
#include <qframe.h>
#include <qpainter.h>
#include <kwm.h>
#include <ktmainwindow.h>

#include "dockmainwindow.h"
#include "dockmanager.h"
#include "docksplitter.h"
#include "dockmovemanager.h"
#include "stabctl.h"

#include "dockback.xpm"
#include "notclose.xpm"

SDockButton::SDockButton( QWidget *parent, const char * name )
:QPushButton( parent, name )
{
  moveMouse = false;
}

SDockButton::~SDockButton()
{
}

void SDockButton::drawButton( QPainter* p )
{
  p->fillRect( 0,0, width(), height(), QBrush(colorGroup().brush(QColorGroup::Background)) );
  p->drawPixmap( (width() - pixmap()->width()) / 2, (height() - pixmap()->height()) / 2, *pixmap() );
  if ( moveMouse && !isDown() ){
    p->setPen( white );
    p->moveTo( 0, height() - 1 );
    p->lineTo( 0, 0 );
    p->lineTo( width() - 1, 0 );

    p->setPen( colorGroup().dark() );
    p->lineTo( width() - 1, height() - 1 );
    p->lineTo( 0, height() - 1 );
  }
  if ( isOn() || isDown() ){
    p->setPen( colorGroup().dark() );
    p->moveTo( 0, height() - 1 );
    p->lineTo( 0, 0 );
    p->lineTo( width() - 1, 0 );

    p->setPen( white );
    p->lineTo( width() - 1, height() - 1 );
    p->lineTo( 0, height() - 1 );
  }
}

void SDockButton::enterEvent( QEvent * )
{
  moveMouse = true;
  repaint();
}

void SDockButton::leaveEvent( QEvent * )
{
  moveMouse = false;
  repaint();
}

/*************************************************************************/

DockWidget::DockWidget( DockManager* dockManager, const char* name, const QPixmap &pixmap ) : QWidget( 0L, name )
  ,m_bDraggable(true)
	,m_oldBrotherDockWidget(0)
  ,m_curDockPos(DockNone)
  ,m_oldDockPos(DockNone)
{
  Parent = (QWidget*)dockManager->parent();

  eDocking = DockTop|DockLeft|DockRight|DockBottom|DockCenter;
  sDocking = DockTop|DockLeft|DockRight|DockBottom|DockCenter;

  isGroup = false;
  isTabGroup = false;

  manager = dockManager;
  installEventFilter( manager );
  manager->childDock->append( this );

  dockbackButton = new SDockButton( this, "DockWidgetDockBackButton");
  dockbackButton ->setFocusPolicy( NoFocus );
  dockbackButton->setPixmap(dockback_xpm);
  connect( dockbackButton, SIGNAL(clicked()), SLOT(slotDockBackButtonClick()));

  QPoint p = QPoint(0,0);
  widget = 0L;
  setMouseTracking( true );
  dockCaptionHeight = 10;

  drawBuffer = 0L;
  pix = new QPixmap(pixmap);

  recreateToDesktop( p );
}

DockWidget::~DockWidget()
{
  if ( !manager->undockProcess ){
    unDock();
  }
  delete pix;
  delete drawBuffer;
  manager->childDock->remove( this );
}

void DockWidget::recreateToDesktop( QPoint p )
{
  reparent(0, 0, p, false);
//F.B.  XSetTransientForHint( qt_xdisplay(), winId(), Parent->topLevelWidget()->winId());
  QApplication::syncX();
//F.B.  KWM::setDecoration(winId(), KWM::tinyDecoration);
  setMouseTracking( true );
  dockCaptionHeight = 15;
  buttonY = 2;
  dockbackButton->setGeometry( 0, 2 , 11, 11 );
  dockbackButton->show();

  m_curDockPos = DockNone;
}

void DockWidget::recreateTo( QWidget* s )
{
  reparent( s, 0, QPoint(0,0), false );
  QApplication::syncX();

  buttonY = 0;
  if ( s->inherits("KTMainWindow") || isGroup ){
    dockCaptionHeight = 0;
  } else {
    dockCaptionHeight = 10;

  }
  dockbackButton->setGeometry( 0, 0 , 9, 9 );
  dockbackButton->hide();
}

void DockWidget::show()
{
//QWidget::show();
//return;
  if ( manager->parent() == 0L ){
    QWidget::show();
    return;
  }
  if ( parentWidget() != 0L || ((DockMainWindow*)manager->parent())->isVisible()  )
    QWidget::show();
  if (m_curDockPos == DockNone)
 		QWidget::show();
}

bool DockWidget::event( QEvent *event )
{
  switch ( event->type() )
  {
    case QEvent::ChildInserted:
      if ( ((QChildEvent*)event)->child()->isA("SDockButton") ) break;
      if ( widget == 0L ){
        widget = (QWidget*)((QChildEvent*)event)->child();
        widget->setGeometry( crect() );
      }
      else debug("DockWidget: Only one child may be on this widget");
      break;
    case QEvent::ChildRemoved:
      if ( widget == ((QChildEvent*)event)->child() ) widget = 0L;
      break;
    case QEvent::Resize:
      dockbackButton->move( width() - dockbackButton->width() - 1, buttonY );
      if ( widget != 0L ){
        widget->setGeometry( crect() );
      }
      if ( drawBuffer != 0L ) delete drawBuffer;
      drawBuffer = new QPixmap( width(), height() );
      break;
    case QEvent::Paint:
      paintCaption();
      break;
/*    case QEvent::Show:
      if ( widget != 0L ) widget->show();
      break;*/
		case QEvent::Close:
			emit iMBeingClosed();
			break;
    default:
      break;
  }
  return QWidget::event( event );
}

QRect DockWidget::crect()
{
  if( m_bDraggable)
	  return QRect(0, dockCaptionHeight, width(), height()-dockCaptionHeight);
  else
    return rect();
}

void DockWidget::paintCaption()
{
  setCaption( name());

  if ( !m_bDraggable) return;
  if ( dockCaptionHeight == 0 ) return;
  QPainter p(this);
  QPainter paint;
  paint.begin( drawBuffer );
  paint.fillRect( drawBuffer->rect(), QBrush(colorGroup().brush(QColorGroup::Background)) );

	int delta = 0;
	if( (m_curDockPos == DockNone) && m_oldBrotherDockWidget)
	  delta = 12;

  paint.setPen( white );
  paint.drawLine( 1, 3, 1, 2 );
  paint.drawLine( 1, 2, width()-delta, 2 );

  paint.setPen( colorGroup().mid() );
  paint.drawLine( 1, 4, width()-delta, 4 );
  paint.drawLine( width()-delta, 4, width()-delta, 3 );

  paint.setPen( white );
  paint.drawLine( 1, 6, 1, 5 );
  paint.drawLine( 1, 5, width()-delta, 5 );

  paint.setPen( colorGroup().mid() );
  paint.drawLine( 1, 7, width()-delta, 7 );
  paint.drawLine( width()-delta, 7, width()-delta, 6 );
	
//	paint.setPen(black);
//	paint.setFont(*(manager->titleFont));
//	paint.drawText(0,0,width(), dockCaptionHeight, AlignLeft, name());

  bitBlt( this,0,0,drawBuffer,0,0,width(),dockCaptionHeight );
  paint.end();
}

void DockWidget::manualDock( DockWidget* target, DockPosition dockPos, QPoint pos,  int spliPos )
{
	// at first, go toplevel
  unDock();

	if ( (target != 0L) && (dockPos == DockNone) ) {
		qDebug("logical error in DockWidget::manualDock. 'DockNone' to a given DockWidget is nonsense!");
		return;
	}		

  if ( target != 0L ) {
		// yes there's a target to dock to
    QWidget* parentDock = target->parentWidget();

    if ( parentDock != 0L )
			// the target is not toplevel
      if ( parentDock->isA("STabCtl") ) {
				// docking to a tab widget
        recreateTo( parentDock );
        ((STabCtl*)parentDock)->insertPage( this, name() );
        ((STabCtl*)parentDock)->setPixmap( this, *pix );
        setDockTabName( (STabCtl*)parentDock );

				m_curDockPos = DockCenter;
        emit manager->change();
        return;
      }

		// create a new dockwidget that will contain the target and this
    DockWidget* newDock = new DockWidget( manager, "tempName", QPixmap("") );
		newDock->m_curDockPos = target->m_curDockPos;

    if ( dockPos == DockCenter ){
      newDock->isTabGroup = true;
      newDock->eDocking = (int)DockNone;
    } else {
      newDock->isGroup = true;
      newDock->eDocking = DockTop|DockLeft|DockRight|DockBottom;
    }

    if ( parentDock == 0L ){
			// dock to a toplevel dockwidget means newDock is toplevel now
      newDock->move( target->frameGeometry().topLeft() );
      newDock->resize( target->geometry().size() );
      if ( target->isVisibleToTLW() ) newDock->show();
    } else {
			// insert newDock in the parent dockwidget
      newDock->recreateTo( parentDock );
			if( isVisible())
				parentDock->show();
    }

		// redirect the dockback button to the new dockwidget
		if( target->m_oldBrotherDockWidget != 0L) {
			newDock->m_oldBrotherDockWidget = target->m_oldBrotherDockWidget;
  		QObject::connect( m_oldBrotherDockWidget, SIGNAL(iMBeingClosed()),
										    newDock, SLOT(slotOldBrotherIsLost()) );
			target->slotOldBrotherIsLost();
		}
		newDock->m_oldDockPos = target->m_oldDockPos;

    if ( dockPos == DockCenter )
    {
			// if to dock to the center of the target dockwidget,
			// dock center to newDock
      STabCtl* tab = new STabCtl( newDock, "_dock_tab");
      connect( tab, SIGNAL(tabShowPopup(int,QPoint)), manager, SLOT(slotTabShowPopup(int,QPoint)));
      target->recreateTo( tab );
      recreateTo( tab );

      tab->insertPage( target, target->name() );
      tab->setPixmap( target, *(target->pix) );

      tab->insertPage( this, name() );
      tab->setPixmap( this, *pix );

      setDockTabName( tab );
      tab->show();

			m_curDockPos = DockCenter;
			target->m_oldDockPos = target->m_curDockPos;
			target->m_curDockPos = DockCenter;
    }
		else {
			// if to dock not to the center of the target dockwidget,
			// dock to newDock
      DockSplitter* panner = 0L;
      if ( dockPos == DockTop  || dockPos == DockBottom )
		 		panner = new DockSplitter( newDock, "DockPaner", DockSplitter::Horizontal, KNewPanner::Percent, spliPos );
      if ( dockPos == DockLeft || dockPos == DockRight  )
				panner = new DockSplitter( newDock, "DockPaner", DockSplitter::Vertical , KNewPanner::Percent, spliPos );
      panner->setFocusPolicy( NoFocus );
      target->recreateTo( panner );
      recreateTo( panner );
			target->m_oldDockPos = target->m_curDockPos;
      if ( dockPos == DockRight) {
				panner->activate( target, this );
				m_curDockPos = DockRight;
        target->m_curDockPos = DockLeft;
      }
			else if( dockPos == DockBottom) {
				panner->activate( target, this );
				m_curDockPos = DockBottom;
				target->m_curDockPos = DockTop;
			}
			else if( dockPos == DockTop) {
				panner->activate( this, target );
				m_curDockPos = DockTop;
				target->m_curDockPos = DockBottom;
			}
			else if( dockPos == DockLeft) {
				panner->activate( this, target );
				m_curDockPos = DockLeft;
				target->m_curDockPos = DockRight;
			}
      target->show();
      show();
      panner->show();
    }

    if ( parentDock != 0L ){
      if ( parentDock->isA("DockSplitter") ){
        DockSplitter* sp = (DockSplitter*)parentDock;
        sp->deactivate();
        if ( sp->getFirst() == target )
          sp->activate( newDock, 0L );
        else
          sp->activate( 0L, newDock );
      }
      if ( parentDock->inherits("KTMainWindow") ){
        ((DockMainWindow*)parentDock)->setDockView( newDock );
      }
    }
    newDock->show();
    emit target->docking( this, dockPos );
    emit manager->replaceDock( target, newDock );
  } else {
		// if there's no target to dock to
      move( pos );
      show();
  }

  // for set DockWidget::widget
  qApp->processEvents();
  emit manager->change();
}

void DockWidget::unDock()
{
  QWidget* parentW = parentWidget();
  if ( parentW == 0L )  return; // Trivial move around desktop

  m_oldDockPos = m_curDockPos; //F.B.

  manager->undockProcess = true;
	
  bool isV = parentW->isVisibleToTLW();
  if ( parentW->isA("STabCtl") ){
    ((STabCtl*)parentW)->removePage( this );
    recreateToDesktop(QPoint(0,0));
    if ( ((STabCtl*)parentW)->pageCount() == 1 ){

      /* last subdock widget in the tab control*/
      DockWidget* lastTab = (DockWidget*)((STabCtl*)parentW)->getFirstPage();
      ((STabCtl*)parentW)->removePage( lastTab );
      lastTab->recreateToDesktop(QPoint(0,0));
      lastTab->move( parentW->mapToGlobal(parentW->frameGeometry().topLeft()) );

      /* STabCtl always have a parent is DockWidget*/
      DockWidget* parentOfTab = (DockWidget*)parentW->parent();
      delete parentW; // STabCtl

      /* may be DockSplitter on null (desktop) or DockMainWindow*/
      QWidget* parentOfDockWidget = parentOfTab->parentWidget();
      if ( parentOfDockWidget == 0L ){
          if ( isV ) lastTab->show();
      } else {
        if ( parentOfDockWidget->isA("DockSplitter") ){
          DockSplitter* split = (DockSplitter*)parentOfDockWidget;
          lastTab->recreateTo( split );
          split->deactivate();
          if ( split->getFirst() == parentOfTab ){
            split->activate( lastTab );
            if ( ((DockWidget*)split->parent())->splitterOrientation == (int)KNewPanner::Vertical )
              emit ((DockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, DockLeft );
            else
              emit ((DockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, DockTop );
          } else {
            split->activate( 0L, lastTab );
            if ( ((DockWidget*)split->parent())->splitterOrientation == (int)KNewPanner::Vertical )
              emit ((DockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, DockRight );
            else
              emit ((DockWidget*)split->getAnother(parentOfTab))->docking( parentOfTab, DockBottom );
          }
          split->show();
        }
        if ( parentOfDockWidget->inherits("KTMainWindow") ){
          lastTab->recreateTo( parentOfDockWidget );
          ((DockMainWindow*)parentOfDockWidget)->setDockView( lastTab );
//          if ( parentOfDockWidget->isVisible() ) ((DockMainWindow*)parentOfDockWidget)->show();
        }
        lastTab->show();
      }
      emit manager->replaceDock( parentOfTab, lastTab );
			lastTab->m_curDockPos = parentOfTab->m_curDockPos;
			if( parentOfTab->m_oldBrotherDockWidget)
				emit parentOfTab->iMBeingClosed();
      delete parentOfTab;

    } else {
      setDockTabName( (STabCtl*)parentW );
    }
    // for set DockWidget::widget
    qApp->processEvents();

    emit manager->change();
    manager->undockProcess = false;
    return;
  }

  if ( parentW->inherits("KTMainWindow") ){
    recreateToDesktop(QPoint(0,0));
    move(1,1);
    manager->undockProcess = false;
    return;
  }

  DockSplitter* parentSplitterOfDockWidget = (DockSplitter*)parentW;

  recreateToDesktop(QPoint(0,0));
  move(1,1);

  DockWidget* secondWidget = (DockWidget*)parentSplitterOfDockWidget->getAnother( this );
  DockWidget* group        = (DockWidget*)parentSplitterOfDockWidget->parentWidget();

  m_oldBrotherDockWidget = secondWidget; //F.B.
	QObject::connect( m_oldBrotherDockWidget, SIGNAL(iMBeingClosed()),
										this, SLOT(slotOldBrotherIsLost()) );

  if ( group->parentWidget() == 0L ){
    secondWidget->recreateToDesktop( QPoint(0,0) );
    secondWidget->setGeometry( group->frameGeometry().x(),
                             group->frameGeometry().y(),
                             group->geometry().size().width(),
                             group->geometry().size().height() );
  } else {
    QWidget* obj = group->parentWidget();
    secondWidget->recreateTo( obj );
    if ( obj->inherits("KTMainWindow") ){
      ((DockMainWindow*)obj)->setDockView( secondWidget );
//      if (obj->isVisible() ) ((DockMainWindow*)obj)->show();
    } else {
      secondWidget->recreateTo( obj );
      DockSplitter* parentOfGroup = (DockSplitter*)obj;
      parentOfGroup->deactivate();
      if ( parentOfGroup->getFirst() == group ) {
				parentOfGroup->activate( secondWidget);
        if ( ((DockWidget*)parentOfGroup->parent())->splitterOrientation == (int)KNewPanner::Vertical )
          emit ((DockWidget*)parentOfGroup->getAnother(group))->docking( group, DockLeft );
        else
          emit ((DockWidget*)parentOfGroup->getAnother(group))->docking( group, DockTop );
			} else {
        parentOfGroup->activate( 0L, secondWidget );
        if ( ((DockWidget*)parentOfGroup->parent())->splitterOrientation == (int)KNewPanner::Vertical )
          emit ((DockWidget*)parentOfGroup->getAnother(group))->docking( group, DockRight );
        else
          emit ((DockWidget*)parentOfGroup->getAnother(group))->docking( group, DockBottom );
			}
			parentOfGroup->show();
    }
  }
	secondWidget->m_curDockPos = group->m_curDockPos;
	secondWidget->m_oldDockPos = group->m_oldDockPos;
  emit manager->replaceDock( group, secondWidget );
  delete parentSplitterOfDockWidget;

  if ( isV ) secondWidget->show();

  // for set DockWidget::widget
  qApp->processEvents();

  emit manager->change();
  manager->undockProcess = false;
}

void DockWidget::setKTMainWindow( KTMainWindow* mw )
{
  mw->recreate(this, 0, QPoint(0,0), false);
  QApplication::syncX();
}

void DockWidget::slotDockBackButtonClick()
{
  if( m_oldBrotherDockWidget) {
		// search all children if it tries to dock back to a child
		bool found = false;
		QObjectList* cl = queryList("DockWidget");
   	QObjectListIt it( *cl );
	  QObject * obj;
   	while ( (obj=it.current()) != 0 ) {
      ++it;
      QWidget* widg = (QWidget*)obj;
      if( widg == m_oldBrotherDockWidget)
				found = true;
   	}
   	delete cl;

		if( !found) {
			// can dock back to the old brother dockwidget
			manualDock( m_oldBrotherDockWidget, m_oldDockPos);
			m_oldBrotherDockWidget = 0L;
			return;
		}
	}

	// else dockback to the dockmainwindow (default behaviour)
  ((DockMainWindow*)manager->parent())->dockIn(this, m_oldDockPos);
  m_oldBrotherDockWidget = 0L;
}

void DockWidget::setDockTabName( STabCtl* tab )
{
  QString listOfName;
  for ( QWidget* w = tab->getFirstPage(); w != 0L; w = tab->getNextPage( w ) ){
    listOfName.append( w->name() ).append(",");
  }
  listOfName.remove( listOfName.length()-1, 1 );

  tab->parent()->setName( listOfName );
  tab->parentWidget()->repaint( false ); // DockWidget->repaint
  if ( tab->parentWidget()->parent() != 0L )
    if ( tab->parentWidget()->parent()->isA("DockSplitter") ) ( (DockSplitter*)(tab->parentWidget()->parent()) )->updateName();
}

void DockWidget::slotOldBrotherIsLost()
{
	QObject::disconnect( m_oldBrotherDockWidget, SIGNAL(iMBeingClosed()),
										   this, SLOT(slotOldBrotherIsLost()) );
	m_oldBrotherDockWidget = 0;
	repaint();
}

/**************************************************************************************/

DockManager::DockManager( DockMainWindow* mainWindow, const char* name )
:QObject( mainWindow, name )
{
  undockProcess = false;

  menuData = new QList<menuDockData>;
  menuData->setAutoDelete( true );
  configMenuData = new QStrList();
  menuData->setAutoDelete( true );

  menu = new QPopupMenu();
  configMenu = new QPopupMenu();

  connect( menu, SIGNAL(aboutToShow()), SLOT(slotMenuPopup()) );
  connect( menu, SIGNAL(activated(int)), SLOT(slotMenuActivated(int)) );
  connect( configMenu, SIGNAL(activated(int)), SLOT(slotConfigMenuActivated(int)) );

  childDock = new QObjectList();
  childDock->setAutoDelete( false );
  mg = 0L;
  titleFont = new QFont("helvetica", 12, QFont::Bold);
  draging = false;
}

DockManager::~DockManager()
{
  delete menuData;
  delete configMenuData;
  delete menu;
  delete configMenu;
  delete titleFont;


  QObjectListIt it( *childDock );
  DockWidget * obj;

  while ( (obj=(DockWidget*)it.current()) != 0 ) {
//    ++it;
    delete obj;
  }
  delete childDock;
}

void DockManager::activate()
{
//  qApp->processEvents();

  QObjectListIt it( *childDock );
  DockWidget * obj;

  while ( (obj=(DockWidget*)it.current()) != 0 ) {
    ++it;
    if ( obj->parent() != 0L && !obj->parent()->isA("STabCtl") ){
        obj->show();
        if ( obj->widget != 0L ) obj->widget->show();
    }
    if ( obj->parent() == 0L ){
      obj->show();
      if ( obj->widget != 0L ) obj->widget->show();
    }
  }

  ( (QWidget*)parent() )->show();
  writeConfig( "default" );
  writeConfig( "lastsavedconfig" );
}

bool DockManager::eventFilter( QObject *obj, QEvent *event )
{
  if ( obj->isA("DockWidget") ){
    DockWidget* ww;
    switch ( event->type() )
    {
      case QEvent::MouseButtonRelease:
        if ( draging ){
          draging = false;
          drop();
        }
        break;
      case QEvent::MouseMove:
        if ( draging ) {
          ww = findDockWidgetAt( QCursor::pos() );
          DockWidget* oldMoveWidget = currentMoveWidget;
          if ( currentMoveWidget != 0L && ww == currentMoveWidget ) { //move
            dragMove( currentMoveWidget, currentMoveWidget->mapFromGlobal( QCursor::pos() ) );
            break;
          }

          if ( oldMoveWidget != 0L && ww != currentMoveWidget ) { //leave
            currentMoveWidget = ww;
            curPos = DockNone;
            mg->resize( storeW, storeH );
            mg->moveContinue();
          }
          if ( oldMoveWidget != ww && ww != 0L ) { //enter ww
            currentMoveWidget = ww;
            curPos = DockNone;
            storeW = mg->width();
            storeH = mg->height();
            mg->movePause();
          }
        } else {
          if ( ((QMouseEvent*)event)->state() == LeftButton ) startDrag( (DockWidget*)obj );
        }
        break;
      default:
        break;
    }
  }
  return false;
}

DockWidget* DockManager::findDockWidgetAt( const QPoint& pos )
{
  if (currentDragWidget->eDocking == (int)DockNone ) return 0L;

  QWidget* p = QApplication::widgetAt( pos );
  if ( p == 0L ) return 0L;
  QWidget* w = 0L;
  findChildDockWidget( w, p, p->mapFromParent(pos) );
  if ( w == 0L ){
    if ( !p->isA("DockWidget") ) return 0L;
    w = p;
  }
  if ( qt_find_obj_child( w, "DockSplitter", "DockPaner" ) != 0L ) return 0L;
  if ( qt_find_obj_child( w, "STabCtl", "_dock_tab" ) != 0L ) return 0L;
  if ( childDockWidgetList->find(w) != -1 ) return 0L;

  DockWidget* www = (DockWidget*)w;
  if ( www->sDocking == DockNone ) return 0L;

  DockPosition curPos = DockNone;
  QPoint cpos  = www->mapFromGlobal( pos );
  QRect r = www->crect();

  int ww = r.width() / 3;
  int hh = r.height() / 3;

	if ( cpos.y() <= hh ){
    curPos = DockTop;
	} else
    if ( cpos.y() >= 2*hh ){
      curPos = DockBottom;
    } else
      if ( cpos.x() <= ww ){
        curPos = DockLeft;
      } else
        if ( cpos.x() >= 2*ww ){
          curPos = DockRight;
        } else
            curPos = DockCenter;

  if ( !(www->sDocking & curPos) ) return 0L;
  if ( !(currentDragWidget->eDocking & (int)curPos) ) return 0L;

  return www;
}

void DockManager::findChildDockWidget( QWidget*& ww, const QWidget* p, const QPoint& pos )
{
  if ( p->children() ) {
    QWidget *w;
    QObjectListIt it( *p->children() );
    it.toLast();
    while ( it.current() ) {
      if ( it.current()->isWidgetType() ) {
        w = (QWidget*)it.current();
        if ( w->isVisible() && w->geometry().contains(pos) ) {
          if ( w->isA("DockWidget") ) ww = w;
          findChildDockWidget( ww, w, w->mapFromParent(pos) );
          return;
        }
      }
      --it;
    }
  }
  return;
}

void DockManager::findChildDockWidget( const QWidget* p, WidgetList*& list )
{
  if ( p->children() ) {
    QWidget *w;
    QObjectListIt it( *p->children() );
    it.toLast();
    while ( it.current() ) {
      if ( it.current()->isWidgetType() ) {
        w = (QWidget*)it.current();
        if ( w->isVisible() ) {
          if ( w->isA("DockWidget") ) list->append( w );
          findChildDockWidget( w, list );
        }
      }
      --it;
    }
  }
  return;
}

void DockManager::startDrag( DockWidget* w )
{
  currentMoveWidget = 0L;
	currentDragWidget = w;
  childDockWidgetList = new WidgetList();
  childDockWidgetList->append( w );
  findChildDockWidget( w, childDockWidgetList );

	if ( mg != 0L ) delete mg;
  mg = new DockMoveManager( w );
  curPos = DockNone;
  draging = true;
  mg->doMove( true, true, false);
}

void DockManager::dragMove( DockWidget* dw, QPoint pos )
{
  QRect r = dw->crect();
	QPoint p = dw->mapToGlobal( r.topLeft() );
  DockPosition oldPos = curPos;

  if ( dw->parent() != 0L )
    if ( dw->parent()->isA("STabCtl") ){
      curPos = DockCenter;
    	if ( oldPos != curPos ) mg->setGeometry( p.x()+2, p.y()+2, r.width()-4, r.height()-4 );
      return;
    }

  int w = r.width() / 3;
  int h = r.height() / 3;

	if ( pos.y() <= h ){
    curPos = DockTop;
    w = r.width();
	} else
    if ( pos.y() >= 2*h ){
      curPos = DockBottom;
      p.setY( p.y() + 2*h );
      w = r.width();
    } else
      if ( pos.x() <= w ){
        curPos = DockLeft;
        h = r.height();
      } else
        if ( pos.x() >= 2*w ){
          curPos = DockRight;
          p.setX( p.x() + 2*w );
          h = r.height();
        } else
          {
            curPos = DockCenter;
            p.setX( p.x() + w );
            p.setY( p.y() + h );
          }

	if ( oldPos != curPos ) mg->setGeometry( p.x(), p.y(), w, h );
}

void DockManager::drop()
{
  mg->stop();
  delete childDockWidgetList;
  if ( currentMoveWidget == 0L && currentDragWidget->parent() == 0L )
    currentDragWidget->move( mg->x(), mg->y() );
  else
    currentDragWidget->manualDock( currentMoveWidget, curPos , QPoint(mg->x(), mg->y()) );
}

void DockManager::writeConfig( const char* confName )
{
debug("BEGIN Write Config");
  KConfig* c = kapp->config();
  c->setGroup( QString(name())+"_config_"+confName );
  c->writeEntry( "Version", DOCK_CONFIG_VERSION );

	QStrList nameList;
	QStrList findList;
  QObjectListIt it( *childDock );
  DockWidget * obj;

	// collect DockWidget's name
	QStrList nList;
  while ( (obj=(DockWidget*)it.current()) != 0 ) {
	  ++it;
    debug("  +Add subdock %s", obj->name());
		nList.append( obj->name() );
  }

	nList.first();
  while ( nList.current() != 0L ){
    debug("  -Try to save %s", nList.current());
		obj = getDockWidgetFromName( nList.current() );
    QString cname = obj->name();
/*************************************************************************************************/
    if ( obj->isGroup ){
      if ( findList.find( obj->firstName ) != -1 && findList.find( obj->lastName ) != -1 ){

        c->writeEntry( cname+":type", "GROUP");
        if ( obj->parent() == 0L ){
          c->writeEntry( cname+":parent", "___null___");
          c->writeEntry( cname+":x", obj->frameGeometry().x());
          c->writeEntry( cname+":y", obj->frameGeometry().y());
          c->writeEntry( cname+":width", obj->width());
          c->writeEntry( cname+":height", obj->height());
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":parent", "yes");
        }
        c->writeEntry( cname+":first_name", obj->firstName );
        c->writeEntry( cname+":last_name", obj->lastName );
        c->writeEntry( cname+":orientation", obj->splitterOrientation );
        c->writeEntry( cname+":sepPos", ((DockSplitter*)obj->widget)->separatorPos() );

        nameList.append( obj->name() );
        findList.append( obj->name() );
        debug("  Save %s", nList.current());
        nList.remove();
        nList.first();
      } else {
/*************************************************************************************************/
        debug("  Skip %s", nList.current());
        if ( findList.find( obj->firstName ) == -1 )
          debug("  ? Not found %s", obj->firstName);
        if ( findList.find( obj->lastName ) == -1 )
          debug("  ? Not found %s", obj->lastName);
        nList.next();
        if ( nList.current() == 0L ) nList.first();
      }
    } else {
/*************************************************************************************************/
      if ( obj->isTabGroup){
        c->writeEntry( cname+":type", "TAB_GROUP");
        if ( obj->parent() == 0L ){
          c->writeEntry( cname+":parent", "___null___");
          c->writeEntry( cname+":x", obj->frameGeometry().x());
          c->writeEntry( cname+":y", obj->frameGeometry().y());
          c->writeEntry( cname+":width", obj->width());
          c->writeEntry( cname+":height", obj->height());
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":parent", "yes");
        }
        QStrList list;
        for ( QWidget* w = ((STabCtl*)obj->widget)->getFirstPage(); w != 0L; w = ((STabCtl*)obj->widget)->getNextPage( w ) ){
          list.append( w->name() );
        }
        c->writeEntry( cname+":tabNames", list );
        c->writeEntry( cname+":curTab", ((STabCtl*)obj->widget)->visiblePageId() );

        nameList.append( obj->name() );
        findList.append( obj->name() ); // not realy need !!!
        debug("  Save %s", nList.current());
        nList.remove();
        nList.first();
      } else {
/*************************************************************************************************/
        if ( obj->parent() == 0L ){
          c->writeEntry( cname+":type", "DOCK");
          c->writeEntry( cname+":x", obj->frameGeometry().x());
          c->writeEntry( cname+":y", obj->frameGeometry().y());
          c->writeEntry( cname+":width", obj->width());
          c->writeEntry( cname+":height", obj->height());
          c->writeEntry( cname+":visible", obj->isVisible());
        } else {
          c->writeEntry( cname+":type", "NULL_DOCK");
        }
        nameList.append( cname );
        debug("  Save %s", nList.current());
        findList.append( obj->name() );
        nList.remove();
        nList.first();
      }
    }
  }
  c->writeEntry( "NameList", nameList );

  DockMainWindow* m = (DockMainWindow*)parent();
  c->writeEntry( "Main:Geometry", KWM::geometry( m->winId() ) );
  c->writeEntry( "Main:visible", m->isVisible());

  // for DockMainWindow->setDockView() in reafConfig()
  c->writeEntry( "Main:view", m->getMainViewDockWidget()->name() );

  c->sync();
  debug("END Write Config");
}

void DockManager::readConfig( const char* confName )
{
  KConfig* c = kapp->config();
  c->setGroup( QString(name())+"_config_"+confName );
  QStrList nameList;
  c->readListEntry( "NameList", nameList );
  QString ver = c->readEntry( "Version", "0.0.1" );
  if ( nameList.count() < 1 || ver != DOCK_CONFIG_VERSION ){
    activate();
    return;
  }

  autoCreateDock = new QObjectList();
  autoCreateDock->setAutoDelete( true );

  QObjectListIt it( *childDock );
  DockWidget * obj;
  while ( (obj=(DockWidget*)it.current()) != 0 ) {
    ++it;
    if ( !obj->isGroup && !obj->isTabGroup && QString(obj->name()) != QString("MainWindowDockWidget") )
    {
      obj->unDock();
    }
  }

  nameList.first();
  while ( nameList.current() != 0L ){
    QString oname = nameList.current();

    QString type = c->readEntry( oname + ":type" );
    if ( type == "GROUP" ){
      DockWidget* first = getDockWidgetFromName( c->readEntry( oname + ":first_name" ) );
      DockWidget* last  = getDockWidgetFromName( c->readEntry( oname + ":last_name"  ) );

      int p = c->readNumEntry( oname + ":orientation" );
      if ( first != 0L && last != 0L ){
        first->manualDock( last, ( p == 0 ) ? DockLeft : DockTop );
        qApp->processEvents(); /* need to process InsertChildEvent to set member 'widget' */
        obj = getDockWidgetFromName( oname );
        ((DockSplitter*)obj->widget)->setSeparatorPos( c->readNumEntry( oname + ":sepPos" ) );
      }
    }

    if ( type == "TAB_GROUP" ){
      QStrList list;
      c->readListEntry( oname+":tabNames", list );
      DockWidget* d1 = getDockWidgetFromName( list.first() );
      list.next();
      DockWidget* d2 = getDockWidgetFromName( list.current() );
      d2->manualDock( d1, DockCenter );
      STabCtl* tab = (STabCtl*)d1->parent();
      list.next();
      while ( list.current() != 0L ){
        DockWidget* tabDock = getDockWidgetFromName( list.current() );
        tabDock->manualDock( d1, DockCenter );
        list.next();
      }
      tab->setVisiblePage( c->readNumEntry( oname+":curTab" ) );
    }

    if ( type == "DOCK" || c->readEntry( oname + ":parent") == "___null___" ){
      int x = c->readNumEntry( oname + ":x" );
      int y = c->readNumEntry( oname + ":y" );
      int w = c->readNumEntry( oname + ":width" );
      int h = c->readNumEntry( oname + ":height" );
      obj = getDockWidgetFromName( oname );
      obj->unDock();
      obj->setGeometry(x,y,w,h);
      if ( c->readBoolEntry( oname + ":visible" ) ){
        obj->show();
        obj->setGeometry(x,y,w,h);
      } else obj->hide();
    }
    obj = getDockWidgetFromName( oname );

    nameList.next();
	}

  DockMainWindow* m = (DockMainWindow*)parent();

  QString mv = c->readEntry( "Main:view" );
  if ( !mv.isEmpty()  && getDockWidgetFromName( mv ) != 0L ){
    DockWidget* mvd  = getDockWidgetFromName( mv );
    mvd->recreateTo( m );
    mvd->show();
    m->setDockView( mvd );
  }

  // delete all autocreate dock
  delete autoCreateDock;

  m->show();
  QRect mr = c->readRectEntry("Main:Geometry");
  KWM::setGeometry( m->winId(), mr );
}

DockWidget* DockManager::getDockWidgetFromName( const char* dockName )
{
  QObjectListIt it( *childDock );
  DockWidget * obj;
  while ( (obj=(DockWidget*)it.current()) != 0 ) {
    ++it;
		if ( QString(obj->name()) == QString(dockName) ) return obj;
  }
  DockWidget* autoCreate = new DockWidget( this, dockName, QPixmap("") );
  autoCreateDock->append( autoCreate );
	return autoCreate;
}

void DockManager::slotTabShowPopup( int id, QPoint pos )
{
  curTabDockWidget = (DockWidget*)((STabCtl*)sender())->page(id);
  QPopupMenu menu;
  menu.insertItem( "Undock", this, SLOT(slotUndockTab()) );
  menu.insertItem( "Hide", this, SLOT(slotHideTab()) );
  menu.exec( pos );
}

void DockManager::slotUndockTab()
{
  curTabDockWidget->manualDock( 0L, DockNone, curTabDockWidget->mapToGlobal(curTabDockWidget->frameGeometry().topLeft()));
  curTabDockWidget->show();
}

void DockManager::slotHideTab()
{
  curTabDockWidget->unDock();
}

void DockManager::slotMenuPopup()
{
  configMenu->clear();
  menu->clear();
  menuData->clear();

  QWidget* w_main = (QWidget*)parent();
  if ( w_main->isVisible() ){
    menu->insertItem( "Hide toplevel window", 0 );
    menuData->append( new menuDockData( w_main, true ) );
  } else {
    menu->insertItem( "Show toplevel window", 0 );
    menuData->append( new menuDockData( w_main, false ) );
  }
  menu->insertSeparator();

  QObjectListIt it( *childDock );
  DockWidget * obj;
  int numerator = 1;
  while ( (obj=(DockWidget*)it.current()) != 0 ) {
	  ++it;
    if ( !obj->isGroup && !obj->isTabGroup /*&& QString(obj->name()) != QString("MainWindowDockWidget")*/ )
    {
      if ( isDockVisible( obj ) )
      {
        menu->insertItem( *obj->pix, QString("Hide ") + obj->name(), numerator++ );
        menuData->append( new menuDockData( obj, true ) );
      }
      else
      {
    	menu->insertItem( *obj->pix, QString("Show ") + obj->name(), numerator++ );
        menuData->append( new menuDockData( obj, false ) );
      }
    }
  }
  menu->insertSeparator();

  configMenuData->clear();

  configMenu->insertItem( "Restore default configuration", numerator++ );
  configMenuData->append( "default" );

  configMenu->insertSeparator();
//  configMenu->insertItem( "Save As ..." );
//  configMenu->insertSeparator();
//  configMenu->insertItem( "Set default configuration" );


  menu->insertItem( "Config", configMenu );
}

void DockManager::slotMenuActivated( int id )
{
  menuDockData* data = menuData->at( id );

  QWidget * obj = data->dock;
  if ( obj->isA("DockWidget") ){
    if ( data->hide ){
//F.B.      ((DockWidget*)data->dock)->slotCloseButtonClick();
    } else {
      if ( parent() == 0L )
        makeDockVisible((DockWidget*)data->dock);
      else
        ((DockMainWindow*)parent())->makeDockVisible((DockWidget*)data->dock);
    }
  } else {
    if ( data->hide ){
      obj->hide();
    } else {
      obj->show();
    }
  }
}

void DockManager::slotConfigMenuActivated( int id )
{
  readConfig( configMenuData->at(id) );
}

void DockManager::makeDockVisible( DockWidget* dock )
{
  if ( dock == 0L || !((QWidget*)parent())->isVisible() || isDockVisible(dock) ) return;

  if ( dock->parent() != 0L && dock->parent()->isA("STabCtl") ){
    if ( dock->parentWidget()->isVisibleToTLW() ){
      ((STabCtl*)dock->parent())->setVisiblePage( dock );
    } else {
      dock->show();
    }
  } else {
    dock->show();
  }
}

bool DockManager::isDockVisible( DockWidget* dock )
{
  return dock->isVisibleToTLW();
}

DockWidget* DockManager::findWidgetParentDock( QWidget* w )
{
  QObjectListIt it( *childDock );
  DockWidget * dock;
  DockWidget * found = 0L;

  while ( (dock=(DockWidget*)it.current()) != 0 ) {
	  ++it;
    if ( dock->widget == w ){ found  = dock; break; }
  }
  return found;
}
