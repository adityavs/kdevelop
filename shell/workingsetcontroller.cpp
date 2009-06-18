/*
    Copyright David Nolden  <david.nolden.kdevelop@art-master.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "workingsetcontroller.h"
#include <kconfiggroup.h>
#include <kconfig.h>
#include <kglobal.h>
#include <ksharedconfig.h>
#include <kcolorutils.h>
#include <sublime/view.h>
#include <sublime/areaindex.h>
#include <sublime/document.h>
#include <interfaces/idocument.h>
#include "core.h"
#include "documentcontroller.h"
#include <sublime/area.h>
#include "mainwindow.h"
#include <qboxlayout.h>
#include <klocalizedstring.h>
#include <util/pushvalue.h>
#include <kiconeffect.h>
#include <qapplication.h>
#include <util/activetooltip.h>
#include <qevent.h>
#include <qmenu.h>
#include <sublime/urldocument.h>

using namespace KDevelop;

bool WorkingSet::m_loading = false;

//Random set of icons that are well distinguishable from each other. If the user doesn't have them, they won't be used.
QStringList setIcons = QStringList() << "chronometer" << "games-config-tiles" << "im-user" << "irc-voice" << "irc-operator" << "office-chart-pie" << "office-chart-ring" << "speaker" << "view-pim-notes" << "esd" << "akonadi" << "kleopatra" << "nepomuk" << "package_edutainment_art" << "package_games_amusement" << "package_games_sports" << "package_network" << "package_office_database" << "package_system_applet" << "package_system_emulator" << "preferences-desktop-notification-bell" << "wine" << "utilities-desktop-extra" << "step" << "preferences-web-browser-cookies" << "preferences-plugin" << "preferences-kcalc-constants" << "preferences-desktop-icons" << "tagua" << "inkscape" << "java" << "kblogger" << "preferences-desktop-personal" << "emblem-favorite" << "face-smile-big" << "face-embarrassed" << "user-identity" << "mail-tagged" << "media-playlist-suffle" << "weather-clouds";

WorkingSetController::WorkingSetController(Core* core) : m_core(core)
{
    //Load all working-sets
    KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
    foreach(QString set, setConfig.groupList())
        getWorkingSet(set);
}

void WorkingSetController::cleanup()
{
    foreach(WorkingSet* set, m_workingSets)
        delete set;
    m_workingSets.clear();
}


bool WorkingSetController::usingIcon(QString icon)
{
    foreach(WorkingSet* set, m_workingSets)
        if(set->iconName() == icon)
            return true;
    return false;
}

bool WorkingSetController::iconValid(QString icon)
{
    return !KIconLoader::global()->iconPath(icon, KIconLoader::Small, true).isNull();
}


WorkingSet* WorkingSetController::newWorkingSet(QString prefix)
{
    QString newId = QString("%1_%2").arg(prefix).arg(qrand() % 10000000);
    return getWorkingSet(newId);
}

WorkingSet* WorkingSetController::getWorkingSet(QString id)
{
    if(!m_workingSets.contains(id)) {
        QString icon;
        for(int a = 0; a < 100; ++a) {
            int pick = (qHash(id) + a) % setIcons.size(); ///@todo Pick icons semantically, by content, and store them in the config
            if(!usingIcon(setIcons[pick])) {
                if(iconValid(setIcons[pick])) {
                    icon = setIcons[pick];
                break;
                }
            }
        }
        if(icon.isEmpty()) {
            kDebug() << "found no icon for working-set" << id;
            icon = "invalid";
        }
        m_workingSets[id] = new WorkingSet(id, icon);
        emit workingSetAdded(id);
    }
    
    return m_workingSets[id];
}

void deleteGroupRecursive(KConfigGroup group) {
//     kDebug() << "deleting" << group.name();
    foreach(QString entry, group.entryMap().keys()) {
//         kDebug() << "deleting entry" << entry;
        group.deleteEntry(entry);
    }
    Q_ASSERT(group.entryMap().isEmpty());
    
    foreach(QString subGroup, group.groupList()) {
        deleteGroupRecursive(group.group(subGroup));
        group.deleteGroup(subGroup);
    }
    //Why doesn't this work?
//     Q_ASSERT(group.groupList().isEmpty());
    group.deleteGroup();
}

void WorkingSet::saveFromArea(Sublime::Area* area, Sublime::AreaIndex* areaIndex)
{
    if(m_id.isEmpty()) {
        Q_ASSERT(areaIndex->viewCount() == 0 && !areaIndex->isSplitted());
        return;
    }
    kDebug() << "saving" << m_id << "from area";
    
    ///@todo Make the working-sets session-specific
    KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
    KConfigGroup group = setConfig.group(m_id);
    deleteGroupRecursive(group);
    saveFromArea(area, areaIndex, group);
    if(isEmpty())
        deleteGroupRecursive(group);
}

void WorkingSet::saveFromArea(Sublime::Area* a, Sublime::AreaIndex * area, KConfigGroup & group)
{
    if (area->isSplitted()) {
        group.writeEntry("Orientation", area->orientation() == Qt::Horizontal ? "Horizontal" : "Vertical");

        if (area->first()) {
            KConfigGroup subgroup(&group, "0");
            subgroup.deleteGroup();
            saveFromArea(a, area->first(), subgroup);
        }

        if (area->second()) {
            KConfigGroup subgroup(&group, "1");
            subgroup.deleteGroup();
            saveFromArea(a, area->second(), subgroup);
        }
    } else {
        group.writeEntry("View Count", area->viewCount());

        int index = 0;
        foreach (Sublime::View* view, area->views()) {
            group.writeEntry(QString("View %1 Type").arg(index), view->document()->documentType());
            group.writeEntry(QString("View %1").arg(index), view->document()->documentSpecifier());
            QString state = view->viewState();
            if (!state.isEmpty())
                group.writeEntry(QString("View %1 State").arg(index), state);

            ++index;
        }
    }
}


bool WorkingSet::isEmpty() const
{
    if(m_id.isEmpty())
        return true;
    KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
    KConfigGroup group = setConfig.group(m_id);
    return !group.hasKey("Orientation") && group.readEntry("View Count", 0) == 0;
}

struct DisconnectMainWindowsFromArea
{
    DisconnectMainWindowsFromArea(Sublime::Area* area) : m_area(area) {
        if(area) {
            
            foreach(Sublime::MainWindow* window, Core::self()->uiControllerInternal()->mainWindows()) {
                if(window->area() == area) {
                    mainWindows << window;
                    bool hadTempArea = false;
                    foreach(Sublime::Area* tempArea, Core::self()->uiControllerInternal()->areas(window)) {
                        if(tempArea != area) {
                            ///@todo This is insanely ugly..
                            window->setUpdatesEnabled(false);
                            kDebug() << "changing temporarily to area" << tempArea->objectName();
                            Core::self()->uiControllerInternal()->showArea(tempArea->objectName(), window); //Show another area temporarily
                            hadTempArea = true;
                            break;
                        }
                    }
                    Q_ASSERT(hadTempArea);
                }
            }
        }
    }
    
    ~DisconnectMainWindowsFromArea() {
        if(m_area) {
            foreach(Sublime::MainWindow* window, mainWindows) {
                kDebug() << "changing back";
                Core::self()->uiControllerInternal()->showArea(m_area, window);
                window->setUpdatesEnabled(true);
            }
        }
    }

    Sublime::Area* m_area;
    QList<Sublime::MainWindow*> mainWindows;
};

void loadFileList(QStringList& ret, KConfigGroup group)
{
    if (group.hasKey("Orientation")) {
        QStringList subgroups = group.groupList();

        if (subgroups.contains("0")) {

            {
                KConfigGroup subgroup(&group, "0");
                loadFileList(ret, subgroup);
            }

            if (subgroups.contains("1")) {
                KConfigGroup subgroup(&group, "1");
                loadFileList(ret, subgroup);
            }
        }

    } else {

        int viewCount = group.readEntry("View Count", 0);
        for (int i = 0; i < viewCount; ++i) {
            QString type = group.readEntry(QString("View %1 Type").arg(i), "");
            QString specifier = group.readEntry(QString("View %1").arg(i), "");

            ret << specifier;
        }
    }
}
QStringList WorkingSet::fileList() const
{
    if(m_id.isEmpty())
        return QStringList();
    
    QStringList ret;
    KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
    KConfigGroup group = setConfig.group(m_id);

    loadFileList(ret, group);
    return ret;
}

void WorkingSet::loadToArea(Sublime::Area* area, Sublime::AreaIndex* areaIndex, bool clear) {
    PushValue<bool> enableLoading(m_loading, true);
    
    DisconnectMainWindowsFromArea disconnectArea(area);
    
    kDebug() << "loading working-set" << m_id << "into area" << area;
    
    if(clear) {
        kDebug() << "clearing area with working-set" << area->workingSet();
        area->clearViews();
    }
    
    if(m_id.isEmpty())
        return;
    
    KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
    KConfigGroup group = setConfig.group(m_id);

    loadToArea(area, areaIndex, group);
}

void WorkingSet::loadToArea(Sublime::Area* area, Sublime::AreaIndex* areaIndex, KConfigGroup group)
{
    if (group.hasKey("Orientation")) {
        QStringList subgroups = group.groupList();

        if (subgroups.contains("0")) {
            if (!areaIndex->isSplitted()) {
                areaIndex->split(group.readEntry("Orientation", "Horizontal") == "Vertical" ? Qt::Vertical : Qt::Horizontal);
            }

            KConfigGroup subgroup(&group, "0");
            loadToArea(area, areaIndex->first(), subgroup);

            if (subgroups.contains("1")) {
                Q_ASSERT(areaIndex->isSplitted());
                KConfigGroup subgroup(&group, "1");
                loadToArea(area, areaIndex->second(), subgroup);
            }
        }

    } else {
        while (areaIndex->isSplitted()) {
            areaIndex = areaIndex->first();
            Q_ASSERT(areaIndex);// Split area index did not contain a first child area index if this fails
        }

        int viewCount = group.readEntry("View Count", 0);
        for (int i = 0; i < viewCount; ++i) {
            QString type = group.readEntry(QString("View %1 Type").arg(i), "");
            QString specifier = group.readEntry(QString("View %1").arg(i), "");

            bool viewExists = false;
            foreach (Sublime::View* view, areaIndex->views()) {
                if (view->document()->documentSpecifier() == specifier) {
                    viewExists = true;
                    break;
                }
            }

            if (viewExists)
                continue;

            IDocument* doc = Core::self()->documentControllerInternal()->openDocument(specifier,
                             KTextEditor::Cursor::invalid(), IDocumentController::DoNotActivate | IDocumentController::DoNotCreateView);
            Sublime::Document *document = dynamic_cast<Sublime::Document*>(doc);
            if (document) {
                Sublime::View* view = document->createView();

                QString state = group.readEntry(QString("View %1 State").arg(i), "");
                if (!state.isEmpty())
                    view->setState(state);

                area->addView(view, areaIndex);
            } else {
                kWarning() << "Unable to create view of type " << type;
            }
        }
    }
}

void WorkingSet::deleteSet()
{
    if(m_areas.isEmpty() && !m_id.isEmpty()) {
        KConfigGroup setConfig(KGlobal::config(), "Working File Sets");
        KConfigGroup group = setConfig.group(m_id);
        deleteGroupRecursive(group);
        emit setChangedSignificantly();
    }
}

QWidget* WorkingSetController::createSetManagerWidget(MainWindow* parent, bool local, Sublime::Area* fixedArea) {
#if QT_VERSION >= 0x040500
    return new WorkingSetWidget(parent, this, local, fixedArea);
#else
    return 0;
#endif
}

WorkingSetWidget::WorkingSetWidget(MainWindow* parent, WorkingSetController* controller, bool mini, Sublime::Area* fixedArea) : QWidget(parent), m_mini(mini), m_mainWindow(parent), m_fixedArea(fixedArea) {
    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    if(!m_fixedArea)
        connect(parent, SIGNAL(areaChanged(Sublime::Area*)), this, SLOT(areaChanged(Sublime::Area*)));
    
    connect(controller, SIGNAL(workingSetAdded(QString)), this, SLOT(workingSetsChanged()));
    connect(controller, SIGNAL(workingSetRemoved(QString)), this, SLOT(workingSetsChanged()));
    
    Sublime::Area* area = parent->area();
    if(m_fixedArea)
        area = m_fixedArea;
    if(area)
        areaChanged(area);
    
    workingSetsChanged();
}

void WorkingSetWidget::areaChanged(Sublime::Area* area)
{
    if(m_connectedArea) {
        disconnect(m_connectedArea, SIGNAL(changingWorkingSet(Sublime::Area*,QString,QString)), this, SLOT(changingWorkingSet(Sublime::Area*,QString,QString)));
    }
        
    //Queued connect so the change is already applied to the area when we start processing
    connect(area, SIGNAL(changingWorkingSet(Sublime::Area*,QString,QString)), this, SLOT(changingWorkingSet(Sublime::Area*,QString,QString)), Qt::QueuedConnection);
    
    m_connectedArea = area;
    
    changingWorkingSet(area, QString(), area->workingSet());
}

void WorkingSetWidget::changingWorkingSet(Sublime::Area*, QString, QString)
{
    workingSetsChanged();
}

QString htmlColorElement(int element) {
    QString ret = QString("%1").arg(element, 2, 16, QChar('0'));
    return ret;
}

QString htmlColor(QColor color) {
    return "#" + htmlColorElement(color.red()) + htmlColorElement(color.green()) + htmlColorElement(color.blue());
}

class WorkingSetToolTipWidget : public QWidget {
    public:
    WorkingSetToolTipWidget(QWidget* parent, WorkingSet* set) : QWidget(parent), m_set(set) {
        QFrame* frame = new QFrame(this);
        frame->setFrameStyle(QFrame::Panel | QFrame::Plain);
        frame->setLineWidth(1);
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(frame);
        QVBoxLayout* layout2 = new QVBoxLayout(frame);
        layout2->setMargin(0);
        QStringList files = m_set->fileList();
        QLabel* label = new QLabel(i18n("Working Set:\n%1", files.join("\n")));
        layout2->addWidget(label);
    }
    
    private:
        WorkingSet* m_set;
};


void WorkingSetToolButton::contextMenuEvent(QContextMenuEvent* ev)
{
    QToolButton::contextMenuEvent(ev);

    QMenu menu;
    Sublime::MainWindow* mainWindow = dynamic_cast<Sublime::MainWindow*>(Core::self()->uiController()->activeMainWindow());
    Q_ASSERT(mainWindow);
    if(m_set->id() == mainWindow->area()->workingSet()) {
        menu.addAction(i18n("Close Working Set (Left Click)"), this, SLOT(closeSet()));
        menu.addAction(i18n("Duplicate Working Set"), this, SLOT(duplicateSet()));
    }else{
        menu.addAction(i18n("Load Working Set (Left Click)"), this, SLOT(loadSet()));
        menu.addAction(i18n("Merge Working Set"), this, SLOT(mergeSet()));
        menu.addSeparator();
        menu.addAction(i18n("Intersect Working Set"), this, SLOT(intersectSet()));
        menu.addAction(i18n("Subtract Working Set"), this, SLOT(subtractSet()));
    }
    menu.actions()[0]->setIcon(KIcon(m_set->iconName()));
    
    if(!m_set->hasConnectedAreas()) {
        menu.addSeparator();
        menu.addAction(i18n("Delete Working Set"), m_set, SLOT(deleteSet()));
    }
    menu.exec(ev->globalPos());
    
    ev->accept();
}


void WorkingSetToolButton::filterViews(QSet< QString > keepFiles)
{
    if(!Core::self()->documentControllerInternal()->saveAllDocumentsForWindow(mainWindow(), KDevelop::IDocument::Default))
        return;
    
    foreach(Sublime::View* view, mainWindow()->area()->views()) {
        
        Sublime::UrlDocument* urlDoc = dynamic_cast<Sublime::UrlDocument*>(view->document());
        if(urlDoc && !keepFiles.contains(urlDoc->documentSpecifier()))
            mainWindow()->area()->closeView(view);
    }
}

void WorkingSetToolButton::intersectSet()
{
    filterViews(Core::self()->workingSetControllerInternal()->getWorkingSet(mainWindow()->area()->workingSet())->fileList().toSet() & m_set->fileList().toSet());
}

void WorkingSetToolButton::subtractSet()
{
    filterViews(Core::self()->workingSetControllerInternal()->getWorkingSet(mainWindow()->area()->workingSet())->fileList().toSet() - m_set->fileList().toSet());
}

void WorkingSetToolButton::mergeSet()
{
    QSet< QString > loadFiles = m_set->fileList().toSet() - Core::self()->workingSetControllerInternal()->getWorkingSet(mainWindow()->area()->workingSet())->fileList().toSet();
    foreach(QString file, loadFiles)
        Core::self()->documentController()->openDocument(KUrl(file));
}

void WorkingSetToolButton::duplicateSet()
{
    if(!Core::self()->documentControllerInternal()->saveAllDocumentsForWindow(mainWindow(), KDevelop::IDocument::Default))
        return;
    WorkingSet* set = Core::self()->workingSetControllerInternal()->newWorkingSet("clone");
    set->saveFromArea(mainWindow()->area(), mainWindow()->area()->rootIndex());
    mainWindow()->area()->setWorkingSet(set->id());
}

void WorkingSetToolButton::loadSet()
{
    if(!Core::self()->documentControllerInternal()->saveAllDocumentsForWindow(mainWindow(), KDevelop::IDocument::Default))
        return;
    mainWindow()->area()->setWorkingSet(QString(m_set->id()));
}

void WorkingSetToolButton::closeSet()
{
    if(!Core::self()->documentControllerInternal()->saveAllDocumentsForWindow(mainWindow(), KDevelop::IDocument::Default))
        return;
    mainWindow()->area()->setWorkingSet(QString());
}

bool WorkingSetToolButton::event(QEvent* e)
{
    if(e->type() == QEvent::ToolTip) {
        e->accept();
        static QPointer<KDevelop::ActiveToolTip> tooltip;
        static WorkingSetToolButton* oldTooltipButton;
        if(tooltip && oldTooltipButton == this)
            return true;
        
        delete tooltip;
        oldTooltipButton = this;
        
        tooltip = new KDevelop::ActiveToolTip(Core::self()->uiControllerInternal()->activeMainWindow(), QCursor::pos() + QPoint(10, 20));
        tooltip->addExtendRect(QRect(parentWidget()->mapToGlobal(geometry().topLeft()), parentWidget()->mapToGlobal(geometry().bottomRight())));
        QVBoxLayout* layout = new QVBoxLayout(tooltip);
        layout->setMargin(0);
        layout->addWidget(new WorkingSetToolTipWidget(tooltip, m_set));
        tooltip->resize( tooltip->sizeHint() );
        ActiveToolTip::showToolTip(tooltip);
        return true;
    }
    return QToolButton::event(e);
}

void WorkingSetWidget::workingSetsChanged()
{
    kDebug() << "re-creating widget" << m_connectedArea << m_fixedArea << m_mini;
    foreach(QToolButton* button, m_buttons.keys())
        delete button;
    m_buttons.clear();
    
    foreach(WorkingSet* set, Core::self()->workingSetControllerInternal()->allWorkingSets()) {
        
        disconnect(set, SIGNAL(setChangedSignificantly()), this, SLOT(workingSetsChanged()));
        connect(set, SIGNAL(setChangedSignificantly()), this, SLOT(workingSetsChanged()));
        
        if(m_mini && set->id() != m_connectedArea->workingSet()) {
//             kDebug() << "skipping" << set->id() << ", searching" << m_connectedArea->workingSet();
            continue; //In "mini" mode, show only the current working set
        }
        if(set->isEmpty()) {
//             kDebug() << "skipping" << set->id() << "because empty";
            continue;
        }
//         kDebug() << "adding button for" << set->id();
        QToolButton* butt = new WorkingSetToolButton(this, set);
        butt->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
        
        QColor activeBgColor = palette().color(QPalette::Active, QPalette::Highlight);
        QColor normalBgColor = palette().color(QPalette::Active, QPalette::Base);
        QColor useColor;
        if(m_mainWindow && m_mainWindow->area() && m_mainWindow->area()->workingSet() == set->id()) {
            useColor = KColorUtils::mix(normalBgColor, activeBgColor, 0.6);
            butt->setIcon(set->activeIcon());
        }else{
            useColor = KColorUtils::mix(normalBgColor, activeBgColor, 0.2);
            butt->setIcon(set->inactiveIcon());
        }
        
        QString sheet = QString("QToolButton { background : %1}").arg(htmlColor(useColor));
        butt->setStyleSheet(sheet);

        m_layout->addWidget(butt);
        connect(butt, SIGNAL(clicked(bool)), SLOT(buttonTriggered()));
        m_buttons[butt] = set;
    }
    update();
}

void WorkingSetWidget::buttonTriggered()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Q_ASSERT(button);
    Q_ASSERT(m_buttons.contains(button));
    
    //Only close the working-set if the file was saved before
    if(!Core::self()->documentControllerInternal()->saveAllDocumentsForWindow(m_mainWindow, KDevelop::IDocument::Default))
        return;
    
    if(m_mainWindow->area()->workingSet() == m_buttons[button]->id()) {
        //Create a new working-set
//         if(!m_mini) {
                m_mainWindow->area()->setWorkingSet(QString());
//         }else{
//         }
    }else{
        m_mainWindow->area()->setWorkingSet(m_buttons[button]->id());
    }
}

void WorkingSet::changingWorkingSet(Sublime::Area* area, QString from, QString to) {
    kDebug() << "changing working-set from" << from << "to" << to << ", local: " << m_id << "area" << area;
    Q_ASSERT(from == m_id);
    if (from == to)
        return;
    Q_ASSERT(m_areas.contains(area));
    disconnectArea(area);
    WorkingSet* newSet = Core::self()->workingSetControllerInternal()->getWorkingSet(to);
    newSet->connectArea(area);
    kDebug() << "update ready";
}

void WorkingSet::changedWorkingSet(Sublime::Area* area, QString from, QString to) {
    kDebug() << "changed working-set from" << from << "to" << to << ", local: " << m_id << "area" << area;
    Q_ASSERT(to == m_id);
    loadToArea(area, area->rootIndex(), !from.isEmpty());
    kDebug() << "update ready";
}

void WorkingSet::areaViewAdded(Sublime::AreaIndex*, Sublime::View*) {
    Sublime::Area* area = qobject_cast<Sublime::Area*>(sender());
    Q_ASSERT(area);
    Q_ASSERT(area->workingSet() == m_id);

    kDebug() << "added view in" << area << ", id" << m_id;
    if (m_loading) {
        kDebug() << "doing nothing because loading";
        return;
    }
    if (m_id.isEmpty()) {
        //Spawn a new working-set
 
        WorkingSet* set = Core::self()->workingSetControllerInternal()->newWorkingSet(area->objectName());
        set->saveFromArea(area, area->rootIndex());
        area->setWorkingSet(set->id());
        return;
    }
    changed(area);
}

void WorkingSet::areaViewRemoved(Sublime::AreaIndex*, Sublime::View*) {
    Sublime::Area* area = qobject_cast<Sublime::Area*>(sender());
    Q_ASSERT(area);
    Q_ASSERT(area->workingSet() == m_id);
    
    kDebug() << "removed view in" << area << ", id" << m_id;
    if (m_loading) {
        kDebug() << "doing nothing because loading";
        return;
    }
    changed(area);
    if (isEmpty()) {
        kDebug() << "setting zero working set, because area" << area->objectName() << "is empty";
        area->setWorkingSet(QString()); //Set the empty working-set if the area has been emptied
    }
}

WorkingSet::WorkingSet(QString id, QString icon) : m_id(id), m_iconName(icon) {
    //Give the working-set icons one color, so they are less disruptive
    KIconEffect effect;
    QImage imgActive(KIconLoader::global()->loadIcon(icon, KIconLoader::NoGroup, 16).toImage());
    QImage imgInactive = imgActive;
    
    QColor activeIconColor = QApplication::palette().color(QPalette::Active, QPalette::Highlight);
    QColor inActiveIconColor = QApplication::palette().color(QPalette::Active, QPalette::Base);
    
    KIconEffect::colorize(imgActive, KColorUtils::mix(inActiveIconColor, activeIconColor, 0.7), 0.5);
    KIconEffect::colorize(imgInactive, KColorUtils::mix(inActiveIconColor, activeIconColor, 0.3), 0.5);
    
    m_activeIcon = QIcon(QPixmap::fromImage(imgActive));
    m_inactiveIcon = QIcon(QPixmap::fromImage(imgActive));
    //effect.apply(KIconLoader::global()->loadIcon(icon, KIconLoader::NoGroup, 16), KIconLoader::NoGroup, );
}

MainWindow* WorkingSetToolButton::mainWindow() const {
    MainWindow* ret = dynamic_cast<MainWindow*>(Core::self()->uiController()->activeMainWindow());
    Q_ASSERT(ret);
    return ret;
}

#include "workingsetcontroller.moc"











