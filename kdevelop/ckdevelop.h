/***************************************************************************
                     ckdevelop.h - the mainclass in kdevelop   
                             -------------------                                         

    begin                : 20 Jul 1998                                        
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

#ifndef CKDEVELOP_H
#define CKDEVELOP_H

#include "cproject.h"
#include "ctreehandler.h"
#include "structdef.h"      // needed for TEditInfo
#include "resource.h"

#include <kdialog.h>
#include <kiconloader.h>
#include <kmainwindow.h>
#include <kprocess.h>

#include <qguardedptr.h>
#include <qlist.h>
#include <qstrlist.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class QSplitter;
class QProgressBar;
class QWhatsThis;
class QTimer;

class CProject;
class CConfigA2psDlg;
class CConfigEnscriptDlg;
class CDocBrowser;
class CClassView;
class DocTreeView;
class CRealFileView;
class CLogFileView;
class KSwallowWidget;
class CAddExistingFileDlg;
class QListViewItem;
class CErrorMessageParser;
class GrepDialog;
class KHTMLView;
class KHTMLPart;
class CParsedMethod;
class CParsedContainer;
class CParsedClass;
class KDlgEdit;
class KDlgEditWidget;
class KDlgPropWidget;
class KDlgWidgets;
class KDlgDialogs;
class KDlgItems;
class KStatusBar;
class CTabCtl;
class CEditWidget;
class COutputWidget;
class CKDevAccel;
//struct TFileInfo;
class KProcess;
class KWriteView;
//class KProgress;

// Debugger classes
class VarViewer;
class DbgController;
class FrameStack;
class BreakpointManager;
class Breakpoint;
class Disassemble;
class DbgToolbar;

/** the mainclass in kdevelop
  *@author Sandy Meier
  */
class CKDevelop : public KMainWindow {
  Q_OBJECT
public:
  /**constructor*/
  CKDevelop(bool witharg=true);
  /**destructor*/
  virtual ~CKDevelop();
  void initView();
  void initConnections();
  void initKeyAccel();
  void initMenuBar();
  void initToolBar();
  void initStatusBar();
  void initWhatsThis();
  void initProject(bool witharg);
  void initDebugger();


  /** Remove a specified file from the edit_infos struct
   *  and leave the widgets in a proper state
   *  @param filename           The filename you want to remove.
   */
  void removeFileFromEditlist(const char *filename);

  /** Change a text string for search in a way,
   *  which makes it useable for a regular expression search.
   *  This means converting reg. exp. special chars like $, [, !, ecc.
   *  to \$, \[, \!
   *  @param szOldString   the string, you want to convert.
   *  @param bForGrep      special handling for using resultstring with grep
   */
  QString realSearchText2regExp(const char *szOldString, bool bForGrep=false);

  /* get the info structure from the filename */
  TEditInfo *getInfoFromFilename(const QString &filename);

  /** syncs modified-flag in edit_info-structs
   *
   *  @param sFilename   the filename you want to set
   *  @param bModified   changing edit_info-elment to this value
   */
  bool setInfoModified(const QString &sFilename, bool bModified=true);

  /** checks the project files
   *  if there is one file modified or younger than the binary
   */
  bool isProjectDirty();

  /** checks the named file
   *  if it is a shell script
   */
  bool isAScript(const QString &);

  /*
     parses only the files listed in the stringlist
  */
  void refreshClassViewByFileList(QStrList * iFileList);
  /** Checks if the filename passed as param
   *   is an Untitled (means new file generated by KDevelop)
   *
   *  @param szFilename   the filename you want to test
   */
  static bool isUntitled(const char *szFilename);
  /** The complete file save as handling
   *  @return true if it succeeded
   */
  bool fileSaveAs();
  bool saveFileFromTheCurrentEditWidget();

  void refreshTrees(QStrList * iFileList = NULL);
  void refreshTrees(TFileInfo *info);

  void setKeyAccel();
  void setToolmenuEntries();
	
  void initKDlg();
  void initKDlgMenuBar();
  void initKDlgToolBar();
//  void initKDlgStatusBar();
//  void initKDlgKeyAccel();  not needed because of setKeyAccel(); connecting and disconnecting accelerators

  /** sets the Main window caption on startup if in KDlgedit mode, used by main() */
  void setKDlgCaption();
  /** sets the Main window caption for KDevelop */
  void setMainCaption(int tab_item=-1);
  			
  void newFile(bool add_to_project,const char* dir=0);
  /** read the projectfile from the disk*/
  bool readProjectFile(QString file);

  /** Add a file with a specified type to the project. 
   *  
   *  @param complete_filename   The absolute filename.
   *  @param type                Type of file.
   *  @param refresh             If to refresh the trees.
   *  @return true if a new subdir was added.
   */
  bool addFileToProject(QString complete_filename, ProjectFileType type, bool refreshTrees=true);
  void addRecentProject(const char* file);
  void switchToWorkspace(int id);

  /** Switch the view to a certain file.
   * @param filename the absolute filename
   * @param bForceReload if true then enforce updating widget text from file
   * @param bShowModifiedBox if true no messagebox is shown, if the file was modified outside the editor
   */
  void switchToFile(QString filename, bool bForceReload=false,bool bShowModifiedBox=true); // filename = abs

  /** Switch to a certain line in a certain file.
   *  @param filename Absolute filename of the file to switch to.
   *  @param lineNo   The line in the file to switch to.
   */
  void switchToFile(QString filename, int lineNo);

  /** set the correct toolbar and menubar,if a process is running
    * @param enable if true than enable,otherwise disable
    */
  void setToolMenuProcess(bool enable);
  void setDebugMenuProcess(bool enable);

  KDlgEditWidget* kdlg_get_edit_widget() { return kdlg_edit_widget; }
  KDlgPropWidget* kdlg_get_prop_widget() { return kdlg_prop_widget; }
  KDlgWidgets* kdlg_get_widgets_view()   { return kdlg_widgets_view; }
  KDlgDialogs* kdlg_get_dialogs_view()   { return kdlg_dialogs_view; }
  KDlgItems*   kdlg_get_items_view()     { return kdlg_items_view; }
  KStatusBar*  kdlg_get_statusbar()      { return m_statusBar; }
  CTabCtl* kdlg_get_tabctl()             { return  kdlg_tabctl;}

  /** Get the current project. */
  CProject* getProject()                 {return prj;}


 public slots:

  void enableCommand(int id_);
  void disableCommand(int id_);

  ////////////////////////
  // FILE-Menu entries
  ///////////////////////
 
  /** generate a new file*/
  void slotFileNew();
  /** generate a new file in dir*/
  void slotFileNew(const char* dir);
  /**open a file*/
  void slotFileOpen();
  /** opens a file from the file_open_popup that is a delayed popup menu 
   *installed in the open file button of the toolbar */
  void slotFileOpen( int id_ );
  /** close the current file*/
  void slotFileClose();
  void slotFileCloseAll();
  /** save the current file,if Untitled a dialog ask for a valid name*/
  void slotFileSave();
  /** save all files*/
  void slotFileSaveAll();
  /** save the current file under a different filename*/
  void slotFileSaveAs();
  /** opens the printing dialog */
  void slotFilePrint();
  /** quit kdevelop*/
  void slotFileQuit();

  ////////////////////////
  // EDIT-Menu entries
  ///////////////////////
  /** Undo last editing step */
  void slotEditUndo();
  /** Redo last editing step */
  void slotEditRedo();
  /** cuts a selection to the clipboard */
  void slotEditCut();
  /** copies a selection to the clipboard */
  void slotEditCopy();
  /** inserts the clipboard contents to the cursor position */
  void slotEditPaste();
  /** inserts a file at the cursor position */
  void slotEditInsertFile();
  /** opens the search dialog for the editing widget */
  void slotEditSearch();
  /** repeat last search */
  void slotEditRepeatSearch();
  /** acts on grep to search the selected word by keyboard shortcut */
  void slotEditSearchText();
  /** search in files, use grep and find*/
  void slotEditSearchInFiles();
  /** called by popups in the edit and brwoser widgets to grep a string */
  void slotEditSearchInFiles(QString);
  /** runs ispell check on the actual editwidget */
  /*  void slotEditSpellcheck(); */
  /** opens the search and replace dialog */
  void slotEditReplace();
  void slotEditIndent();
  void slotEditUnindent();
  void slotEditComment();
  void slotEditUncomment();
  /** selects the whole editing widget text */
  void slotEditSelectAll();
  /** inverts the selection */
  void slotEditInvertSelection();
  /** remove all text selections */
  void slotEditDeselectAll();
  
  ////////////////////////
  // VIEW-Menu entries
  ///////////////////////
  /** opens the goto line dialog */
  void slotViewGotoLine();
  /** jump to the next error, based on the make output*/
  void slotViewNextError();
  /** jump to the previews error, based on the make output*/
  void slotViewPreviousError();
  /** dis-/enables the treeview */
  void slotViewTTreeView();
  void showTreeView(bool show=true);
  /** dis-/enables the outputview */
  void slotViewTOutputView();
  void showOutputView(bool show=true);
  /** en-/disable the toolbar */
  void slotViewTStdToolbar();
  /** en-/disable the browser toolbar */
  void slotViewTBrowserToolbar();
  /** en-/disable the statusbar */
  void slotViewTStatusbar();
  /** refresh all trees and other widgets*/
  void slotViewRefresh();
  
  ////////////////////////
  // PROJECT-Menu entries
  ///////////////////////
  /** generates a new project with KAppWizard*/
  void slotProjectNewAppl();
  /** calls kimport to generate a new project by
    * requesting a project directory, writes project file
    * and loads the new project */
  void slotProjectGenerate();
  /** opens a projectfile and close the old one*/
  void slotProjectOpen();
  /** opens a project file from the recent project menu in the project menu by getting the project entry and
   * calling slotProjectOpenCmdl()
   */
  void slotProjectOpenRecent(int id_);
  /** opens a project committed by comandline or kfm */
  //MB  cannot guard with #ifdefs here - moc skips this :(
   /** switchs between kdoc1 and doxygen as API documentation tool
   */
  void slotSwitchDocTool();
  /** Configures Doxygen */
  void slotConfigureDoxygen();
  //MB end
  void slotProjectOpenCmdl(QString prjfile);
  /** close the current project,return false if  canceled*/
  bool slotProjectClose();
  /** add a new file to the project-same as file new */
  void slotProjectAddNewFile();
  /** opens the add existing files dialog */
  void slotProjectAddExistingFiles();
  /** helper methods for slotProjectAddExistingFiles() */
  void slotAddExistingFiles();
  /** add a new po file to the project*/
  void slotProjectAddNewTranslationFile();
  /** remove a project file */
  void slotProjectRemoveFile();
  /** opens the New class dialog */
  void slotProjectNewClass();
  /** opens the new class dialog when called from file/classviewer with directory where to create the new class*/
  void slotProjectNewClass(const char* folder);
  /** opens the properties dialog for the project files */
  void slotProjectFileProperties();
  /** opens the properties dialog for project files,rel_name is selected, used by RFV,LFV*/
  void slotShowFileProperties(QString rel_name);
  /** opens the project options dialog */
  void slotProjectOptions();
  /** selects the project workspace */
  void slotProjectWorkspaces(int);
  void slotProjectMessages();
  void slotProjectAPI();
  void slotProjectManual();
  void slotProjectMakeDistSourceTgz();

  ////////////////////////
  // BUILD-Menu entries
  ///////////////////////
  /** compile the actual sourcefile using setted options */
  void slotBuildCompileFile();
  void slotBuildMake();
  //   void slotBuildMakeWith();
  void slotBuildRebuildAll();
  void slotBuildCleanRebuildAll();
  void slotBuildStop();
  void slotBuildRun();
  void slotBuildRunWithArgs();
  void slotStartRun(bool bWithArgs=false);
  void slotBuildDebug(bool bWithArgs=false);
  void slotBuildDebugStart();
  void slotBuildDistClean();
  void slotBuildMakeClean();
  void slotBuildAutoconf();
  void slotBuildConfigure();

  /** connects the menu AND toolbar up to the debug functions
      Most functions are in dbgController which is constructed when
      the user wants the debugger */
  void slotDebugActivator(int id);
  /** this starts the "real" debug session
      it is a successor for slotBuildDebug(true)
      if you start this version instead of slotBuildDebug()
      no check for rebuilding will be made
  */
  void slotStartDebugRunWithArgs();
  /** this starts the "real" debug session
      it is a successor for slotBuildDebug(false)
      if you start this version instead of slotBuildDebug()
      no check for rebuilding will be made
  */
  void slotStartDebug();
  /** Starts up the debugger, and gets it running. This may
      instantiate a debugger if it doesn't exist */
  void slotDebugRun();
  /** Stops and kills the debugger */
  void slotDebugStop();
  /** display the given breakpoint*/
  void slotDebugRefreshBPState(const Breakpoint* BP);
  /** BP state has changed do something (maybe) */
  void slotDebugBPState(Breakpoint* BP);
   /** Dialog showing various views of the debugged program
      memory, libraries, disassembled code */
  void slotDebugMemoryView();
  /** follows the source position in the editor of the debugger*/
  void slotDebugShowStepInSource(const QString& filename, int linenumber,
                                  const QString& address);
  /** follows the source position in the editor of the debugger*/
  void slotDebugGoToSourcePosition(const QString& filename,int linenumber);
  /** Shows the debugger status on the status line */
  void slotDebugStatus(const QString& status, int statusFlag);
  /** Shows the debugger output */
  void slotDebugReceivedStdout(const QString& buffer);
  /** Enter a pid and get the debugger to attach to it */
  void slotDebugAttach();
  /** Set the internal debugger arguments */
  void slotDebugRunWithArgs();
  /** Setect a core file and examine the contents with the debugger */
  void slotDebugExamineCore();
  /** Allow the user to select an arbitary executable to debug */
  void slotDebugNamedFile();
  /** Intial debugger setup */
  void setupInternalDebugger();

  void slotDebugRunToCursor();
  void slotDebugStepInto();
  void slotDebugStepIntoIns();
  void slotDebugStepOver();
  void slotDebugStepOverIns();
  void slotDebugBreakInto();
  void slotDebugStepOutOff();
  void slotDebugToggleBreakpoint();
  void slotDebugInterrupt();

  ////////////////////////
  // TOOLS-Menu entries
  ///////////////////////
  void slotToolsTool(int tool);

  ////////////////////////
  // OPTIONS-Menu entries
  ///////////////////////
  void slotOptionsEditor();
  void slotOptionsEditorColors();
  void slotOptionsSyntaxHighlightingDefaults();
  void slotOptionsSyntaxHighlighting();
  /** shows the Browser configuration dialog */
  void slotOptionsDocBrowser();
  /** shows the Tools-menu configuration dialog */
  void slotOptionsToolsConfigDlg();
  /** shows the spellchecker config dialog */
  void slotOptionsSpellchecker();
  /** shows the configuration dialog for enscript-printing */
  void slotOptionsConfigureEnscript();
  /** shows the configuration dialog for a2ps printing */
  void slotOptionsConfigureA2ps();
  /** show a configure-dialog for kdevelop*/
  void slotOptionsKDevelop();
  /** sets the make command after it is changed in the Setup dialog */
  void slotOptionsMake();
  /** dis-/enables autosaving by setting in the Setup dialog */
  void slotOptionsAutosave(bool);
  /** sets the autosaving time intervall */
  void slotOptionsAutosaveTime(int);
  /** dis-/enalbes autoswitch by setting bAutoswitch */
  void slotOptionsAutoswitch(bool);
  /** toggles between autoswitching to CV or LFV */
  void slotOptionsDefaultCV(bool);
  /** shows the Update dialog and sends output to the messages widget */
  void slotOptionsUpdateKDEDocumentation();
  /** shows the create search database dialog called by the setup button */
  void  slotOptionsCreateSearchDatabase();
  
  ////////////////////////
  // BOOKMARKS-Menu entries
  ///////////////////////
//  void slotBookmarksSet();
  void slotBookmarksToggle();
  void slotBookmarksClear();
  void slotBookmarksBrowserSelected(int);
 	void slotBookmarksNext();
	void slotBookmarksPrevious();

  ////////////////////////
  // HELP-Menu entries
  ///////////////////////
  /** goes one page back in the documentation browser */
  void slotHelpBack();
  /** goes one page forward in the documentatio browser */
  void slotHelpForward();
  /** goes to the page in the history list by delayed popup menu on the 
   *  back-button on the browser toolbar */
  void slotHelpHistoryBack( int id_);
  /** goes to the page in the history list by delayed popup menu on the
   * forward-button on the browser toolbar */
  void slotHelpHistoryForward(int id_);
  /** reloads the currently opened page */
  void slotHelpBrowserReload();
  /** search marked text */
  void slotHelpSearchText();
  /** search marked text with a text string */
  void slotHelpSearchText(QString text);
  /** shows the Search for Help on.. dialog to insert a search expression */
  void slotHelpSearch();
  /** shows the KDevelop manual */
  void slotHelpContents();
  /** shows the KDevelop Programming handbook */
  void slotHelpProgramming();
  /** shows the Tutorial handbook */
  void slotHelpTutorial();
  /** shows the KDE library reference */
  void slotHelpKDELibRef();
  /** shows the KDE 2 developer guide */
  void slotHelpKDE2DevGuide();
  /** shows the C/C++-referenc */
  void slotHelpReference();
  /** shows the tip of the day */
  void slotHelpTipOfDay();
  /**  open the KDevelop Homepage with kfm/konqueror*/
  void slotHelpHomepage();
  /** shows the bug report dialog*/
  void slotHelpBugReport();
  /** shows the API of the current project */
  void slotHelpAPI();
  /** shows the manual of the current project */
  void slotHelpManual();
  /** shows the aboutbox of KDevelop */
  void slotHelpAbout();
  /** switches to the handbook selected: manual, programming, tutorial... */  
  void showDocHelp(const char *bookname);

  void slotHelpDlgNotes();

  void slotGrepDialogItemSelected(QString filename,int linenumber);
  
  ////////////////////////
  // KDlgEdit-View-Menu entries -- managed by kdevelop
  ///////////////////////
  void slotKDlgViewPropView();
  void slotKDlgViewToolbar();
  void slotKDlgViewStatusbar();

  //////////////////////////////////
  // Classbrowser wizardbutton slots
  //////////////////////////////////
  /** View the class header */
  void slotClassbrowserViewClass();
  /** View the graphical classtree. */
  void slotClassbrowserViewTree();
  /** View the declaration of a class/method. */
  void slotClassbrowserViewDeclaration();
  /** View the definition of a class/method. */
  void slotClassbrowserViewDefinition();
  /** Add a new method to a class. */
  void slotClassbrowserNewMethod();
  /** Add a new attribute to a class. */
  void slotClassbrowserNewAttribute();
  
  ////////////////////////
  // All slots which are used if the user clicks or selects something in the view
  ///////////////////////
  /** swich construction for the toolbar icons, selecting the right slots */
  void slotToolbarClicked(int);
  /** click on the main window tabs: header, source,documentation or tools*/
  void slotSTabSelected(int item);
  /** click on the output window tabs: messages, stdout, stderr, breakpoint,
      disassemble, frame stack*/
  void slotOTabSelected(int item);

  /** set the window tab automatically without click */
  void slotSCurrentTab(int item);
  /** click on the treeview tabs: cv,lfv,wfv,doc*/
  void slotTTabSelected(int item);
  /** set the tree tab automatically without click */
  void slotTCurrentTab(int item);
	
  ///////////// -- the methods for the treeview selection
  /** click action on LFV */
  void slotLogFileTreeSelected(QString file);
  /** click action on RFV */
  void slotRealFileTreeSelected(QString file);
  /** click action on DOC */
  void slotDocTreeSelected(QString url_file);
  /** selection of classes in the browser toolbar */
  void slotClassChoiceCombo(int index);
  /** selection of methods in the browser toolbar */
  void slotMethodChoiceCombo(int index);
  /** add a file to the project */
  void slotAddFileToProject(QString abs_filename);
  void delFileFromProject(QString rel_filename);
  /* a slot shell to removeFileFromEditlist */
  void slotRemoveFileFromEditlist(const QString &absFilename);

  /////////some slots for VCS interaction
  
  void slotUpdateFileFromVCS(QString file);
  void slotCommitFileToVCS(QString file);
  void slotUpdateDirFromVCS(QString dir);
  void slotCommitDirToVCS(QString dir);

  //////////////// -- the methods for the statusbar items
  /** change the status message to text */
  void slotStatusMsg(const char *text);
  /** change the status message of the whole statusbar temporary */
  void slotStatusHelpMsg(const char *text);
  /** switch argument for Statusbar help entries on slot selection */
  void statusCallback(int id_);
  /** change Statusbar status of INS and OVR */
  void slotNewStatus();
  /** change copy & cut status */
  void slotCPPMarkStatus(KWriteView *, bool);
  void slotHEADERMarkStatus(KWriteView *, bool);
  void slotBROWSERMarkStatus(KHTMLPart *, bool);
  /** recognize change of Clipboard data */
  void slotClipboardChanged(KWriteView *, bool);
  /** change Statusbar status of Line and Column */
  void slotNewLineColumn();
  void slotNewUndo();

  void slotBufferMenu(const QPoint& pos);
/*  void slotShowC();
  void slotShowHeader();
  void slotShowHelp();
  void slotShowTools();*/
  void slotToggleLast();

  void slotMenuBuffersSelected(int id);
  void slotClickedOnMessagesWidget();
  

  void slotURLSelected(KHTMLPart* widget,const QString& url,int,const char*);
  void slotDocumentDone();
  void slotURLonURL(const QString& url);

  void slotReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotReceivedStderr(KProcess* proc,char* buffer,int buflen);

  void slotApplReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotApplReceivedStderr(KProcess* proc,char* buffer,int buflen);
  void slotApplReceivedStdout(const char* buffer);
  void slotApplReceivedStderr(const char* buffer);

  void switchToKDevelop();
  void switchToKDlgEdit();

  void slotSearchReceivedStdout(KProcess* proc,char* buffer,int buflen);
  void slotProcessExited(KProcess* proc);
  void slotSearchProcessExited(KProcess*);
  
  //////////////// -- the methods for signals generated from the CV
  /** Add a method to a class. Brings up a dialog and lets the
   * user fill it out.
   * @param aClassName      The class to add the method to.
   */
  void slotCVAddMethod( const char *aClassName );

  /** Add a method to a class.
   * user fill it out.
   * @param aClassName      The class to add the method to.
   * @param aMethod         The method to add to the class.
   */
  void slotCVAddMethod( const char *aClassName, CParsedMethod *aMethod );

  /** Add an attribute to a class.
   * @param aClassName      The class to add the attribute to.
   */
  void slotCVAddAttribute( const char *aClassName );
  
  /** Delete an method.
   * @param aClassName Name of the class holding the method. NULL for global functions.
   * @param aMethodName Name of the method(with arguments) to delete.
   */
  void slotCVDeleteMethod( const char *aClassName,const char *aMethodName );

  /** The user wants to view the declaration of a method. 
   * @param className Name of the class holding the declaration. NULL for globals.
   * @param declName Name of the declaration item.
   * @param type Type of declaration item
   */
  void slotCVViewDeclaration( const char *parentPath, 
                              const char *itemName, 
                              THType parentType,
                              THType itemType );

  /** slot to views the class definition from
      the graphical classbrowser. */
  void slotSwitchToFile( const QString &, int );

  /** The user wants to view the definition of a method/attr...
   * @param className Name of the class holding the definition. NULL for globals.
   * @param declName Name of the definition item.
   * @param type Type of definition item.
   */
  void slotCVViewDefinition( const char *parentPath, 
                             const char *itemName, 
                             THType parentType,
                             THType itemType );

protected: // Protected methods

  /** The user selected a class in the classcombo.
   * @param aName Name of the class.
   */
  void CVClassSelected( const char *aName );

  /** The user selected a method in the methodcombo.
   * @param aName Name of the method.
   */
  void CVMethodSelected( const char *aName );

  /** 
   * Goto the definition of the specified item.
   * 
   * @param parentPath Path of the container. Empty for globals.
   * @param itemName   Name of the selected item. Empty for containers.
   * @param parentType Type of container. Ignored for globals.
   * @param itemType   Type of the seleceted item. Ignored for containers.
   */
  void CVGotoDefinition( const char *parentPath, 
                         const char *itemName, 
                         THType parentType,
                         THType itemType );

  /** 
   * Goto the declaration of the specified item.
   *
   * @param parentPath Path of the container. Empty for globals.
   * @param itemName   Name of the selected item. Empty for containers.
   * @param parentType Type of container. Ignored for globals.
   * @param itemType   Type of the seleceted item. Ignored for containers.
   */
  void CVGotoDeclaration( const char *parentPath, 
                         const char *itemName, 
                         THType parentType,
                         THType itemType );
  
  /** 
   * Returns the class for the supplied classname. 
   *
   * @param parentPath Path of the container.
   * @param parentType Type of container.
   */
  CParsedContainer *CVGetContainer( const char *parentPath, 
                                    THType parentType );

	/**
	 * Open an URL in the Documentation Browser.
	 *
	 * @param file The URL to open.
	 */
	void openBrowserBookmark(char* file);

  /** Update the class combo with all classes in alpabetical order. */
  void CVRefreshClassCombo();

  /** Update the method combo with the methods from the selected
   * class.
   * @param aClass Class to update the methodcombo from.
   */
  void CVRefreshMethodCombo( CParsedClass *aClass );

public: // Public methods
  bool isFileInBuffer(QString abs_filename);

  /** a tool meth,used in the search engine*/
  int searchToolGetNumber(QString str);
  QString searchToolGetTitle(QString str);
  QString searchToolGetURL(QString str);
  void saveCurrentWorkspaceIntoProject();

  /** called if a new subdirs was added to the project, shows a messagebox and start autoconf...*/
  void newSubDir();
protected:
  /** reads all options and initializes values*/
  void readOptions();
  /** saves all options on queryExit() */
  void saveOptions();
  /** save the project of the current window and close the swallow widget. 
   * If project closing is cancelled, returns false */
  virtual bool queryClose();
  /** saves all options by calling saveOptions() */
  virtual bool queryExit();
  /** saves the currently opened project by the session manager and write 
   * the project file to the session config*/
  virtual void saveProperties(KConfig* );
  /** initializes the session windows and opens the projects of the last
   * session */
  virtual void readProperties(KConfig* );
	
private:
  //the menus for kdevelop main
  QPopupMenu* file_menu;				
  QPopupMenu* recent_projects_menu;
  //MB
  #ifndef WITH_KDOC2
  QPopupMenu* doctool_menu;
  enum DOCTOOL {DT_KDOC, DT_DOX};
  int doctool;
  #endif
  //MB end
  QPopupMenu* edit_menu;
  QPopupMenu* view_menu;
  QPopupMenu* bookmarks_menu;
  QPopupMenu* doc_bookmarks;

  QPopupMenu* project_menu;
//  QPopupMenu* workspaces_submenu;
  QPopupMenu* build_menu;
  QPopupMenu* debug_menu;
  QPopupMenu* tools_menu;
  QPopupMenu* options_menu;
  QPopupMenu* menu_buffers;
  QPopupMenu* help_menu;
  QWhatsThis* whats_this;
	
  QPopupMenu* history_prev;
  QPopupMenu* history_next;
  QPopupMenu* file_open_popup;
  /** Popup menu for the classbrowser wizard button. */
  QPopupMenu* classbrowser_popup;

  /** Tells if the next click on the classwizard toolbar button should show
   * the declaration or the definition of the selected item. */
  bool cv_decl_or_impl;

  QStrList file_open_list;	
  // the menus for the dialogeditor- specific. other menus inserted as the standard above
  QPopupMenu* kdlg_file_menu;
  QPopupMenu* kdlg_edit_menu;
  QPopupMenu* kdlg_view_menu;
  QPopupMenu* kdlg_project_menu;
  QPopupMenu* kdlg_build_menu;
  QPopupMenu* kdlg_debug_menu;
  QPopupMenu* kdlg_tools_menu;
  QPopupMenu* kdlg_options_menu;
  QPopupMenu* kdlg_help_menu;

  QStrList tools_exe;
  QStrList tools_entry;
  QStrList tools_argument;
  	
  KMenuBar* kdev_menubar;
  KMenuBar* kdlg_menubar;

  QSplitter* mainSplitter;
  QSplitter* topSplitter;
  /** Divides the topSplitter for edit and properties widget
   * of the dialogeditor */
  QSplitter* kdlgTopSplitter;
  
  /** main class for the dialogeditor- 
   *  handles menu/toolbar etc. events specified for the dialogeditor. */
  KDlgEdit* kdlgedit;
  
  /** If this to true, the user wants a beep after a 
   *  process,slotProcessExited() */
  bool beep; 
  
  
  KIconLoader icon_loader;
  /** for tools,compiler,make,kodc */
  KShellProcess process;
  /** only for selfmade appl */
  KShellProcess appl_process;
  /**  for kdoc,sgmltools ... */
  KShellProcess shell_process;
  /** search with glimpse */
  KShellProcess search_process;
  /** at the moment only one project at the same time */
  CProject* prj;

  CKDevAccel *accel;
  KConfig* config;
  int act_outbuffer_len;

  QStrList recent_projects;
  // for the browser
  QStrList history_list;
  QStrList history_title_list;
  QStrList doc_bookmarks_list;
  QStrList doc_bookmarks_title_list;
	
  QList<TEditInfo> edit_infos;

  ///////////////////////////////
  //some widgets for the mainview
  ///////////////////////////////

  /** The tabbar for the sourcescode und browser. */
  CTabCtl* s_tab_view;
  /** The tabbar for the trees. */
  CTabCtl* t_tab_view;
  /** The tabbar for the output_widgets. */
  CTabCtl* o_tab_view;

  /** The tabbar for the kdlg view. */
  CTabCtl* kdlg_tabctl;
  /** The editing view of kdlg. */
  KDlgEditWidget* kdlg_edit_widget;
  /** The properties window of kdlg. */
  KDlgPropWidget* kdlg_prop_widget;
  /** The first tab of kdlg_tabctl. */
  KDlgWidgets* kdlg_widgets_view;
  /** The second tab of kldg_tabctl. */
  KDlgDialogs* kdlg_dialogs_view;
  /** the third tab of kldg_tabctl. */
  KDlgItems*   kdlg_items_view;

  CEditWidget* edit_widget; // a pointer to the actual editwidget
  CEditWidget* header_widget; // the editwidget for the headers/resources
  CEditWidget* cpp_widget;    //  the editwidget for cpp files
  CDocBrowser* browser_widget;
  KSwallowWidget* swallow_widget;
 
  /** The classview. */
  CClassView* class_tree;
  /** The logical fileview. */
  CLogFileView* log_file_tree;
  /** The real fileview. */
  CRealFileView* real_file_tree;
  /** The documentation tree. */
  DocTreeView* doc_tree;

  /** Output from the compiler ... */
  COutputWidget* messages_widget;
  /** stdin and stdout output. */
  COutputWidget* stdin_stdout_widget;
  /** stderr output. */
  COutputWidget* stderr_widget;

  int tree_view_pos;
  int output_view_pos;
  int properties_view_pos;
  int workspace;

  CErrorMessageParser* error_parser;
  QString version;
  QString kdev_caption;
  QString kdlg_caption;
  bool project;

  bool  prev_was_search_result;

  // Autosaving elements
  /** The autosave timer. */
  QTimer* saveTimer;
  /** Tells if autosaving is enabled. */
  bool bAutosave;
  /** The autosave interval. */
  int saveTimeout;

  bool bAutoswitch;
  bool bDefaultCV;
  bool bKDevelop;
//  KProgress* statProg;
  QGuardedPtr<QProgressBar> statProg;
  //some vars for the searchengine
  QString search_output;
  QString doc_search_display_text, doc_search_text;
  // for more then one job in proc;checked in slotProcessExited(KProcess* proc);
  // values are "run","make" "refresh";
  QString next_job;
  QString make_cmd;
//   QString make_with_cmd;

  CConfigEnscriptDlg* enscriptconf;
  CConfigA2psDlg* a2psconf;

  CAddExistingFileDlg* add_dlg;
  GrepDialog* grep_dlg;

  int lasttab;
  QString lastfile;
  QString lastOpenDir;

  /*********************Debugger additions *******************/
  /** The controller for the debugging program (eg gdb) */
  DbgController* dbgController;
  /** The floating toolbar - always on top */
  DbgToolbar* dbgToolbar;
  /** The debugger's tree of local variables - on tree view tab */
  VarViewer* var_viewer;
  /** Manages a list of breakpoints - Always active - on output tab */
  BreakpointManager* brkptManager;
  /** Manages a frame stack list - on output tab */
  FrameStack* frameStack;
  /** show disassembled code being run - on output tab */
   Disassemble* disassemble;
 
  /** debug aid. Switch on using compile switch GDB_MONITOR
      or DBG_MONITOR  - on output tab */
  COutputWidget* dbg_widget;
  /** */
  bool dbgInternal;
  /** The external debugger command to run */
  QString dbgExternalCmd;
  /** Protect the gdbcontroller deletion. */
  bool dbgShuttingDown;
  /** Have both float and normal toolbar */
  bool dbgEnableFloatingToolbar;
  /************* END Debugger additions ********************/

  /** memory effect on open file dialog box*/
  QString _LastOpenDir;

  bool useGlimpse;
  bool useHtDig;
  bool lastShutdownOK;
  bool m_statusBarIsKDevelop;
  KStatusBar* m_statusBar;
};

class SaveAllDialog : public KDialog
{
  Q_OBJECT

  public:
    enum SaveAllResult{Yes=1, No=2, SaveAll=3, Cancel=4};

    SaveAllDialog(const QString& filename, CProject* prj);
    ~SaveAllDialog();

    SaveAllResult result();

  private slots:
    void yes();
    void no();
    void saveAll();
    void cancel();

  private:
    SaveAllResult m_result;
};

#endif
