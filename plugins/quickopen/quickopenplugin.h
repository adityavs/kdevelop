/*
 * This file is part of KDevelop
 *
 * Copyright 2007 David Nolden <david.nolden.kdevelop@art-master.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef QUICKOPENPLUGIN_H
#define QUICKOPENPLUGIN_H

#include <interfaces/iplugin.h>
#include <language/interfaces/iquickopen.h>
#include <QtCore/QVariant>
#include <language/interfaces/quickopendataprovider.h>
#include "ui_quickopen.h"

namespace KDevelop {
  class SimpleCursor;
}

class QuickOpenModel;
class QuickOpenWidget;

class QuickOpenPlugin : public KDevelop::IPlugin, public KDevelop::IQuickOpen
{
    Q_OBJECT
    Q_INTERFACES( KDevelop::IQuickOpen )

public:
    QuickOpenPlugin( QObject *parent, const QVariantList & = QVariantList() );
    virtual ~QuickOpenPlugin();

    static QuickOpenPlugin* self();
    
    // KDevelop::Plugin methods
    virtual void unload();
    
    enum ModelTypes {
      Files = 1,
      Functions = 2,
      Classes = 4,
      OpenFiles = 8,
      All = Files + Functions + Classes + OpenFiles
    };

    /**
     * Shows the quickopen dialog with the specified Model-types
     * @param modes A combination of ModelTypes
     * */
    void showQuickOpen( ModelTypes modes = All );

    virtual void registerProvider( const QStringList& scope, const QStringList& type, KDevelop::QuickOpenDataProviderBase* provider );

    virtual bool removeProvider( KDevelop::QuickOpenDataProviderBase* provider );

    virtual QSet<KDevelop::IndexedString> fileSet() const;

    //Frees the model by closing active quickopen dialoags, and retuns whether successful.
    bool freeModel();
    
public slots:
    void quickOpen();
    void quickOpenFile();
    void quickOpenFunction();
    void quickOpenClass();
    void quickOpenDeclaration();
    void quickOpenDefinition();
    void quickOpenNavigate();
    void quickOpenNavigateFunctions();
    void quickOpenLine(bool);

private slots:
    void storeScopes( const QStringList& );
    void storeItems( const QStringList& );

private:
    friend class QuickOpenLineEdit;
    QWidget* createQuickOpenLineWidget();

    QPair<KUrl, KDevelop::SimpleCursor> specialObjectJumpPosition() const;
    QWidget* specialObjectNavigationWidget() const;
    bool jumpToSpecialObject();
    
    QuickOpenModel* m_model;
    class ProjectFileDataProvider* m_projectFileData;
    class ProjectItemDataProvider* m_projectItemData;
    class OpenFilesDataProvider* m_openFilesData;
    QStringList lastUsedScopes;
    QStringList lastUsedItems;
  
    //We can only have one widget at a time, because we manipulate the model.
    QPointer<QObject> m_currentWidgetHandler;
};

///Will delete itself once the dialog is closed, so use QPointer when referencing it permanently
class QuickOpenWidget : public QFrame {
  Q_OBJECT
  public:
  /**
   * @param initialItems List of items that should initially be enabled in the quickopen-list. If empty, all are enabled.
   * @param initialScopes List of scopes that should initially be enabled in the quickopen-list. If empty, all are enabled.
   * @param listOnly when this is true, the given items will be listed, but all filtering using checkboxes is disabled.
   * @param noSearchFied when this is true, no search-line is shown.
   * */
  QuickOpenWidget( QString title, QuickOpenModel* model, const QStringList& initialItems, const QStringList& initialScopes, bool listOnly = false, bool noSearchField = false, QLineEdit* alterantiveSearchField = 0 );
  ~QuickOpenWidget();
  void setPreselectedText(const QString &text);
    void prepareShow();

  //Shows OK + Cancel. By default they are hidden  
  void showStandardButtons();
    
  signals:
  void scopesChanged( const QStringList& scopes );
  void itemsChanged( const QStringList& scopes );
  void ready();

  private slots:
  void currentChanged( const QModelIndex& current, const QModelIndex& previous );
  void currentChanged( const QItemSelection& current, const QItemSelection& previous );

  void accept();
  void textChanged( const QString& str );
  void updateProviders();
  void doubleClicked ( const QModelIndex & index );
  
  private:
  void callRowSelected();
  
  virtual bool eventFilter ( QObject * watched, QEvent * event );
  QuickOpenModel* m_model;
  bool m_expandedTemporary, m_hadNoCommandSinceAlt;
  QTime m_altDownTime;
  QString m_preselectedText;
  Ui::QuickOpen o;
  
  friend class QuickOpenWidgetDialog;
  friend class QuickOpenPlugin;
};

class QuickOpenWidgetDialog : public QObject {
  Q_OBJECT
  public:
    QuickOpenWidgetDialog( QString title, QuickOpenModel* model, const QStringList& initialItems, const QStringList& initialScopes, bool listOnly = false, bool noSearchField = false );
    ~QuickOpenWidgetDialog();
    ///Shows the dialog
    void run();
    QuickOpenWidget* widget() const {
      return m_widget;
    }
  private:
  QDialog* m_dialog; //Warning: m_dialog is also the parent
  QuickOpenWidget* m_widget;
};

class QuickOpenLineEdit : public QLineEdit {
  Q_OBJECT
  public:
    QuickOpenLineEdit() ;
    ~QuickOpenLineEdit() ;
    
    bool insideThis(QObject* object);
  private slots:
    void activate() ;
    void deactivate() ;
    void checkFocus();
  private:
    virtual void keyPressEvent(QKeyEvent* ev) ;
    virtual void focusInEvent(QFocusEvent* ev) ;
    virtual bool eventFilter(QObject* obj, QEvent* e) ;
    virtual void hideEvent(QHideEvent* );
    
    QPointer<QuickOpenWidget> m_widget;
};

#endif // QUICKOPENPLUGIN_H

// kate: space-indent on; indent-width 2; tab-width 4; replace-tabs on; auto-insert-doxygen on
