/***************************************************************************
 *   Copyright 2007 Alexander Dymo <adymo@kdevelop.org>             *
 *   Copyright 2007 Kris Wong <kris.p.wong@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#ifndef CORE_H
#define CORE_H

#include "shellexport.h"
#include <icore.h>

namespace KDevelop {

class UiController;
class PluginController;
class ProjectController;
class LanguageController;
class PartController;
class DocumentController;

class KDEVPLATFORMSHELL_EXPORT Core: public ICore {
public:
    static void initialize();
    static Core *self();

    virtual ~Core();

    /** @copydoc ICore::uiController() */
    virtual IUiController *uiController();

    /** @copydoc ICore::pluginController() */
    virtual IPluginController *pluginController();

    /** @copydoc ICore::projectController() */
    virtual IProjectController *projectController();

    /** @copydoc ICore::languageController() */
    virtual ILanguageController *languageController();

    /** @copydoc ICore::partManager() */
    virtual KParts::PartManager *partManager();

    /** @copydoc ICore::documentController() */
    virtual IDocumentController *documentController();

    /// The following methods may only be used within the shell.

    /** @return ui controller */
    UiController *uiControllerInternal();

    /** @return plugin controller */
    PluginController *pluginControllerInternal();

    /** @return project controller */
    ProjectController *projectControllerInternal();

    /** @return language controller */
    LanguageController *languageControllerInternal();

    /** @return part manager */
    PartController *partManagerInternal();

    /** @return document controller */
    DocumentController *documentControllerInternal();

    void deinitialize();

protected:
    void cleanup();

private:
    Core(QObject *parent = 0);
    static Core *m_self;
    struct CorePrivate *d;
};

}

#endif

