/***************************************************************************
 *   Copyright 2006-2007 Alexander Dymo  <adymo@kdevelop.org>       *
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
#include "areaoperationtest.h"

#include <QtTest/QtTest>
#include <QDockWidget>
#include <QListView>
#include <QTextEdit>
#include <QSplitter>

#include <kdebug.h>
#include <kapplication.h>

#include <sublime/view.h>
#include <sublime/area.h>
#include <sublime/sublimedefs.h>
#include <sublime/tooldocument.h>
#include <sublime/urldocument.h>
#include <sublime/controller.h>
#include <sublime/mainwindow.h>
#include <sublime/container.h>

#include "areaprinter.h"

using namespace Sublime;

struct ViewCounter {
    ViewCounter(): count(0) {}
    Area::WalkerMode operator()(AreaIndex *index)
    {
        count += index->views().count();
        return Area::ContinueWalker;
    }
    int count;
};

void AreaOperationTest::init()
{
    m_controller = new Controller(this);
    Document *doc1 = new UrlDocument(m_controller, KUrl::fromPath("~/foo.cpp"));
    Document *doc2 = new UrlDocument(m_controller, KUrl::fromPath("~/boo.cpp"));
    Document *doc3 = new UrlDocument(m_controller, KUrl::fromPath("~/moo.cpp"));
    Document *doc4 = new UrlDocument(m_controller, KUrl::fromPath("~/zoo.cpp"));

    //documents for toolviews
    Document *tool1 = new ToolDocument("tool1", m_controller, new SimpleToolWidgetFactory<QListView>("tool1"));
    Document *tool2 = new ToolDocument("tool2", m_controller, new SimpleToolWidgetFactory<QTextEdit>("tool2"));
    Document *tool3 = new ToolDocument("tool3", m_controller, new SimpleToolWidgetFactory<QTextEdit>("tool3"));

    //areas (aka perspectives)
    //view object names are in form AreaNumber.DocumentNumber.ViewNumber
    //"tool" prefix is there for tooldocument views
    m_area1 = new Area(m_controller, "Area 1");
    m_pView111 = doc1->createView();
    m_pView111->setObjectName("view1.1.1");
    m_area1->addView(m_pView111);
    m_pView121 = doc2->createView();
    m_pView121->setObjectName("view1.2.1");
    m_area1->addView(m_pView121);
    m_pView122 = doc2->createView();
    m_pView122->setObjectName("view1.2.2");
    m_area1->addView(m_pView122);
    m_pView131 = doc3->createView();
    m_pView131->setObjectName("view1.3.1");
    m_area1->addView(m_pView131);

    View *view = tool1->createView();
    view->setObjectName("toolview1.1.1");
    m_area1->addToolView(view, Sublime::Left);
    view = tool2->createView();
    view->setObjectName("toolview1.2.1");
    m_area1->addToolView(view, Sublime::Bottom);
    view = tool2->createView();
    view->setObjectName("toolview1.2.2");
    m_area1->addToolView(view, Sublime::Bottom);

    m_area2 = new Area(m_controller, "Area 2");
    View *view211 = doc1->createView();
    view211->setObjectName("view2.1.1");
    m_area2->addView(view211);
    View *view212 = doc1->createView();
    view212->setObjectName("view2.1.2");
    m_area2->addView(view212);
    View *view221 = doc2->createView();
    view221->setObjectName("view2.2.1");
    m_area2->addView(view221, view211, Qt::Vertical);
    View *view231 = doc3->createView();
    view231->setObjectName("view2.3.1");
    m_area2->addView(view231, view221, Qt::Horizontal);
    View *view241 = doc4->createView();
    view241->setObjectName("view2.4.1");
    m_area2->addView(view241, view212, Qt::Vertical);
    view = tool1->createView();
    view->setObjectName("toolview2.1.1");
    m_area2->addToolView(view, Sublime::Bottom);
    view = tool2->createView();
    view->setObjectName("toolview2.2.1");
    m_area2->addToolView(view, Sublime::Right);
    view = tool3->createView();
    view->setObjectName("toolview2.3.1");
    m_area2->addToolView(view, Sublime::Top);
    view = tool3->createView();
    view->setObjectName("toolview2.3.2");
    m_area2->addToolView(view, Sublime::Top);

    m_area3 = new Area(m_controller, "Area 3");
    View *view0 = doc1->createView();
    view0->setObjectName("view3.1.1");
    m_area3->addView(view0);
    View *view1 = doc2->createView();
    view1->setObjectName("view3.1.2");
    m_area3->addView(view1, view0);
    View *view2 = doc3->createView();
    view2->setObjectName("view3.1.3");
    m_area3->addView(view2, view1);
    View *view3 = doc4->createView();
    view3->setObjectName("view3.1.4");
    m_area3->addView(view3, view1);

    m_controller->addDefaultArea(m_area1);
    m_controller->addDefaultArea(m_area2);
    m_controller->addDefaultArea(m_area3);

}

void AreaOperationTest::cleanup()
{
    delete m_area1;
    delete m_area2;
    delete m_controller;
    m_area1 = 0;
    m_area2 = 0;
    m_controller = 0;
}

void AreaOperationTest::areaConstruction()
{
    //check if areas has proper object names
    QCOMPARE(m_area1->objectName(), QString("Area 1"));
    QCOMPARE(m_area2->objectName(), QString("Area 2"));

    //check that area1 contents is properly initialised
    AreaViewsPrinter viewsPrinter1;
    m_area1->walkViews(viewsPrinter1, m_area1->rootIndex());
    QCOMPARE(viewsPrinter1.result, QString("\n\
[ view1.1.1 view1.2.1 view1.2.2 view1.3.1 ]\n\
"));
    AreaToolViewsPrinter toolViewsPrinter1;
    m_area1->walkToolViews(toolViewsPrinter1, Sublime::AllPositions);
    QCOMPARE(toolViewsPrinter1.result, QString("\n\
toolview1.1.1 [ left ]\n\
toolview1.2.1 [ bottom ]\n\
toolview1.2.2 [ bottom ]\n\
"));

    //check that area2 contents is properly initialised
    AreaViewsPrinter viewsPrinter2;
    m_area2->walkViews(viewsPrinter2, m_area2->rootIndex());
    QCOMPARE(viewsPrinter2.result, QString("\n\
[ vertical splitter ]\n\
    [ vertical splitter ]\n\
        [ view2.1.1 view2.1.2 ]\n\
        [ view2.4.1 ]\n\
    [ horizontal splitter ]\n\
        [ view2.2.1 ]\n\
        [ view2.3.1 ]\n\
"));
    AreaToolViewsPrinter toolViewsPrinter2;
    m_area2->walkToolViews(toolViewsPrinter2, Sublime::AllPositions);
    QCOMPARE(toolViewsPrinter2.result, QString("\n\
toolview2.1.1 [ bottom ]\n\
toolview2.2.1 [ right ]\n\
toolview2.3.1 [ top ]\n\
toolview2.3.2 [ top ]\n\
"));
}

void AreaOperationTest::mainWindowConstruction()
{
    //====== check for m_area1 ======
    MainWindow mw1(m_controller);
    m_controller->showArea(m_area1, &mw1);
    checkArea1(&mw1);

/////////////
 //====== check for m_area2 ======
    MainWindow mw2(m_controller);
    m_controller->showArea(m_area2, &mw2);
    checkArea2(&mw2);
}

void AreaOperationTest::checkArea1(MainWindow *mw)
{
    Area *area = mw->area();
    //check that all docks have their widgets
    foreach (View *dock, mw->toolDocks()) {
        //QVERIFY(dock->widget() != 0);
        QVERIFY(dock->hasWidget());
    }
    QCOMPARE(mw->toolDocks().count(), area->toolViews().count());

    //check that mainwindow have all splitters and widgets in splitters inside centralWidget
    QWidget *central = mw->centralWidget();
    QVERIFY(central != 0);
    QVERIFY(central->inherits("QWidget"));

    QWidget *splitter = central->findChild<QSplitter*>();
    QVERIFY(splitter);
    QVERIFY(splitter->inherits("QSplitter"));

    //check that we have a container and 4 views inside
    Container *container = splitter->findChild<Sublime::Container*>();
    QVERIFY(container);
    ViewCounter c;
    area->walkViews(c, area->rootIndex());
    QCOMPARE(container->count(), c.count);
    for (int i = 0; i < container->count(); ++i)
        QVERIFY(container->widget(i) != 0);
}

void AreaOperationTest::checkArea2(MainWindow *mw)
{
    Area *area = mw->area();
    //check that all docks have their widgets
    foreach (View *dock, mw->toolDocks()) {
        //QVERIFY(dock->widget() != 0);
        QVERIFY(dock->hasWidget());
    }
    QCOMPARE(mw->toolDocks().count(), area->toolViews().count());

    //check that mainwindow have all splitters and widgets in splitters inside centralWidget
    QWidget *central = mw->centralWidget();
    QVERIFY(central != 0);
    QVERIFY(central->inherits("QWidget"));

    QWidget *splitter = central->findChild<QSplitter*>();
    QVERIFY(splitter);
    QVERIFY(splitter->inherits("QSplitter"));

    //check that we have 4 properly initialized containers
    QList<Container*> containers = splitter->findChildren<Sublime::Container*>();
    QCOMPARE(containers.count(), 4);

    int widgetCount = 0;
    foreach (Container *c, containers)
    {
        for (int i = 0; i < c->count(); ++i)
            QVERIFY(c->widget(i) != 0);
        widgetCount += c->count();
    }

    ViewCounter c;
    area->walkViews(c, area->rootIndex());
    QCOMPARE(widgetCount, c.count);

    //check that we have 7 splitters: 2 vertical and 1 horizontal, rest is not split
    QList<QSplitter*> splitters = splitter->findChildren<QSplitter*>();
    splitters.append(qobject_cast<QSplitter*>(splitter));
    QCOMPARE(splitters.count(), 6+1); //6 child splitters + 1 central itself = 7 splitters
    int verticalSplitterCount = 0;
    int horizontalSplitterCount = 0;
    foreach (QSplitter *s, splitters)
    {
        if (s->count() == 1)
            continue;   //this is a splitter with container inside, its orientation is not relevant
        if (s->orientation() == Qt::Vertical)
            verticalSplitterCount += 1;
        else
            horizontalSplitterCount += 1;
    }
    QCOMPARE(verticalSplitterCount, 2);
    QCOMPARE(horizontalSplitterCount, 1);
}

void AreaOperationTest::areaCloning()
{
    //show m_area1 in MainWindow1
    MainWindow mw1(m_controller);
    m_controller->showArea(m_area1, &mw1);
    checkArea1(&mw1);

    //now try to show the same area in MainWindow2 and check that we get a clone
    MainWindow mw2(m_controller);
    m_controller->showArea(m_area1, &mw2);

    //two mainwindows have different areas
    QVERIFY(mw1.area() != mw2.area());
    //the area for the second mainwindow is a clone of the
    //original area and should have the same name.
    QVERIFY(mw2.area()->objectName() == mw1.area()->objectName());

    //check mainwindow layouts - original and copy
    checkArea1(&mw1);
    checkArea1(&mw2);
}

/*! Functor used by areaSwitchingInSameMainWindow()
    Walks all Views and checks if they got a widget.
    hasWidget will be set to false if any View lacks a widget.*/
struct AreaWidgetChecker {
    AreaWidgetChecker(): foundViewWithoutWidget(false), failureMessage("") {}
    Area::WalkerMode operator()(AreaIndex *index)
    {
        foreach (View *view, index->views())
        {
            if (!view->hasWidget()) {
                failureMessage += view->objectName() + " has no widget\n";
                foundViewWithoutWidget = true;
            }
        }
        return Area::ContinueWalker;
    }
    Area::WalkerMode operator()(View *view, Sublime::Position)
    {
        if (!view->hasWidget()) {
            foundViewWithoutWidget = true;
            failureMessage += view->objectName() + " has no widget\n";
        }
        return Area::ContinueWalker;
    }
    char* message() {
        return qstrdup(failureMessage.toLatin1().data());
    }
    bool foundViewWithoutWidget;
    QString failureMessage;
};

void AreaOperationTest::areaSwitchingInSameMainwindow()
{
    MainWindow mw(m_controller);
    m_controller->showArea(m_area1, &mw);
    checkArea1(&mw);

    m_controller->showArea(m_area2, &mw);
    checkArea2(&mw);

    //check what happened to area1 widgets, they should be intact
    AreaWidgetChecker checker;
    m_area1->walkViews(checker, m_area1->rootIndex());
    m_area1->walkToolViews(checker, Sublime::AllPositions);
    QVERIFY2(!checker.foundViewWithoutWidget, checker.message());
}

void AreaOperationTest::simpleViewAdditionAndDeletion()
{
    // set TabBarOpenAfterCurrent=0, otherwise we'd have a different order of tabs
    int oldTabBarOpenAfterCurrent;
    {
        KConfigGroup uiGroup = KSharedConfig::openConfig()->group("UiSettings");
        oldTabBarOpenAfterCurrent = uiGroup.readEntry("TabBarOpenAfterCurrent", 1);
        uiGroup.writeEntry("TabBarOpenAfterCurrent", 0);
        uiGroup.sync();
    }
    m_controller->loadSettings();

    MainWindow mw(m_controller);
    m_controller->addMainWindow(&mw);

    m_controller->showArea(m_area1, &mw);
    checkArea1(&mw);

    Document *doc5 = new UrlDocument(m_controller, KUrl::fromPath("~/new.cpp"));
    View *view = doc5->createView();
    view->setObjectName("view1.5.1");
    m_area1->addView(view);

    checkAreaViewsDisplay(&mw, m_area1,
        QString("\n[ view1.1.1 view1.2.1 view1.2.2 view1.3.1 view1.5.1 ]\n"), 
        1, 1, "Added an url view (view1.5.1)");

    //now remove view and check that area is valid
    delete m_area1->removeView(view);

    checkAreaViewsDisplay(&mw, m_area1,
        QString("\n[ view1.1.1 view1.2.1 view1.2.2 view1.3.1 ]\n"), 
        1, 1, "Removed the url view (view1.5.1)");

    //now remove all other views one by one and leave an empty container
    QList<View*> list(m_area1->views());
    foreach (View *view, list)
        delete m_area1->removeView(view);

    checkAreaViewsDisplay(&mw, m_area1,
        QString("\n[ horizontal splitter ]\n"), 
        0, 1, "Removed all views. Only horizontal splitter should remain.");

    //add a view again and check that mainwindow is correctly reconstructed
    view = doc5->createView();
    view->setObjectName("view1.5.1");
    m_area1->addView(view);

    checkAreaViewsDisplay(&mw, m_area1,
        QString("\n[ view1.5.1 ]\n"), 
        1, 1, "Added a single view to previously emptied mainwindow.");

    {
        KConfigGroup uiGroup = KSharedConfig::openConfig()->group("UiSettings");
        uiGroup.writeEntry("TabBarOpenAfterCurrent", oldTabBarOpenAfterCurrent);
        uiGroup.sync();
    }
    m_controller->loadSettings();
}

void AreaOperationTest::complexViewAdditionAndDeletion()
{
    Area *area = m_area2;
    MainWindow mw(m_controller);
    m_controller->addMainWindow(&mw);

    m_controller->showArea(m_area2, &mw);

    Document *doc5 = new UrlDocument(m_controller, KUrl::fromPath("~/new.cpp"));
    View *view = doc5->createView();
    view->setObjectName("view2.5.1");

    View *view221 = findNamedView(area, "view2.2.1");
    QVERIFY(view221);
    area->addView(view, view221, Qt::Vertical);

    checkAreaViewsDisplay(&mw, area, QString("\n\
[ vertical splitter ]\n\
    [ vertical splitter ]\n\
        [ view2.1.1 view2.1.2 ]\n\
        [ view2.4.1 ]\n\
    [ horizontal splitter ]\n\
        [ vertical splitter ]\n\
            [ view2.2.1 ]\n\
            [ view2.5.1 ]\n\
        [ view2.3.1 ]\n\
"), 5, 8+1);

    //now delete view221
    delete area->removeView(view221);

    checkAreaViewsDisplay(&mw, area, QString("\n\
[ vertical splitter ]\n\
    [ vertical splitter ]\n\
        [ view2.1.1 view2.1.2 ]\n\
        [ view2.4.1 ]\n\
    [ horizontal splitter ]\n\
        [ view2.5.1 ]\n\
        [ view2.3.1 ]\n\
"), 4, 6+1);

    //remove one more view, this time the one inside non-empty container
    View *view211 = findNamedView(area, "view2.1.1");
    delete m_area2->removeView(view211);

    checkAreaViewsDisplay(&mw, area, QString("\n\
[ vertical splitter ]\n\
    [ vertical splitter ]\n\
        [ view2.1.2 ]\n\
        [ view2.4.1 ]\n\
    [ horizontal splitter ]\n\
        [ view2.5.1 ]\n\
        [ view2.3.1 ]\n\
"), 4, 6+1);

    //and now remove all remaining views one by one
    delete m_area2->removeView(findNamedView(area, "view2.1.2"));
    checkAreaViewsDisplay(&mw, area, QString("\n\
[ vertical splitter ]\n\
    [ view2.4.1 ]\n\
    [ horizontal splitter ]\n\
        [ view2.5.1 ]\n\
        [ view2.3.1 ]\n\
"), 3, 4+1);

    delete m_area2->removeView(findNamedView(area, "view2.4.1"));
    checkAreaViewsDisplay(&mw, area, QString("\n\
[ horizontal splitter ]\n\
    [ view2.5.1 ]\n\
    [ view2.3.1 ]\n\
"), 2, 2+1);

    delete m_area2->removeView(findNamedView(area, "view2.5.1"));
    checkAreaViewsDisplay(&mw, area, QString("\n\
[ view2.3.1 ]\n\
"), 1, 1);

    delete m_area2->removeView(findNamedView(area, "view2.3.1"));
    checkAreaViewsDisplay(&mw, area, QString("\n\
[ horizontal splitter ]\n\
"), 0, 1);
}

void AreaOperationTest::toolViewAdditionAndDeletion()
{
    MainWindow mw(m_controller);
    m_controller->showArea(m_area1, &mw);
    checkArea1(&mw);

    Document *tool4 = new ToolDocument("tool4", m_controller, new SimpleToolWidgetFactory<QTextEdit>("tool4"));
    View *view = tool4->createView();
    view->setObjectName("toolview1.4.1");
    m_area1->addToolView(view, Sublime::Right);

    //check that area is in valid state
    AreaToolViewsPrinter toolViewsPrinter1;
    m_area1->walkToolViews(toolViewsPrinter1, Sublime::AllPositions);
    QCOMPARE(toolViewsPrinter1.result, QString("\n\
toolview1.1.1 [ left ]\n\
toolview1.2.1 [ bottom ]\n\
toolview1.2.2 [ bottom ]\n\
toolview1.4.1 [ right ]\n\
"));

    //check that mainwindow has newly added toolview
    foreach (View *dock, mw.toolDocks())
        QVERIFY(dock->widget() != 0);
    QCOMPARE(mw.toolDocks().count(), m_area1->toolViews().count());

    //now remove toolview
    m_area1->removeToolView(view);

    AreaToolViewsPrinter toolViewsPrinter2;
    //check that area doesn't have it anymore
    m_area1->walkToolViews(toolViewsPrinter2, Sublime::AllPositions);
    QCOMPARE(toolViewsPrinter2.result, QString("\n\
toolview1.1.1 [ left ]\n\
toolview1.2.1 [ bottom ]\n\
toolview1.2.2 [ bottom ]\n\
"));

    //check that mainwindow has newly added toolview
    foreach (View *dock, mw.toolDocks())
        QVERIFY(dock->widget() != 0);
    QCOMPARE(mw.toolDocks().count(), m_area1->toolViews().count());
}



void AreaOperationTest::testAddingViewAfter()
{

    QList<View*> list(m_area3->views());
    foreach (View *view, list){
        kDebug() << "name of view : " << view->objectName() << " , it's index : " << m_area3->views().indexOf(view);
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AreaOperationTest::splitViewActiveTabsTest()
{
    MainWindow mw(m_controller);
    m_controller->showArea(m_area1, &mw);
    checkArea1(&mw);

    // at first show of the area, the active view should be m_pView111
    QCOMPARE(mw.activeView(), m_pView111);

    // Try to get to the main container :
    // get the central widget
    QWidget *pCentral = mw.centralWidget();
    QVERIFY(pCentral);
    QVERIFY(pCentral->inherits("QWidget"));

    // get its first splitter
    QWidget *pSplitter = pCentral->findChild<QSplitter*>();
    QVERIFY(pSplitter);
    QVERIFY(pSplitter->inherits("QSplitter"));

    // finally, get the splitter's container
    Container *pContainer = pSplitter->findChild<Sublime::Container*>();
    QVERIFY(pContainer);

    // verify that the current active widget in the container is the one in activeview (m_pView111)
    QCOMPARE(pContainer->currentWidget(), mw.activeView()->widget());

    // activate the second tab of the area (view212)
    mw.activateView(m_pView121);

    // verify that the active view was correctly updated to m_pView121
    QCOMPARE(mw.activeView(), m_pView121);

    // check if the container's current widget was updated to the active view's
    QCOMPARE(pContainer->currentWidget(), mw.activeView()->widget());

    // now, create a split view of the active view (m_pView121)
    Sublime::View *pNewView = mw.activeView()->document()->createView();
    pNewView->setObjectName("splitOf" + mw.activeView()->objectName());
    m_area1->addView(pNewView, mw.activeView(), Qt::Vertical);

    // verify that creating a new view did not break the central widget
    QCOMPARE(pCentral, mw.centralWidget());

    // verify that creating a new view did not break the main splitter
    QCOMPARE(pSplitter, pCentral->findChild<QSplitter*>());

    // creating a new view created two new children splitters, get them
    QVERIFY(pSplitter->findChildren<QSplitter*>().size() == 2);
    QWidget *pFirstSplitter = pSplitter->findChildren<QSplitter*>().at(0);
    QVERIFY(pFirstSplitter);
    QWidget *pSecondSplitter = pSplitter->findChildren<QSplitter*>().at(1);
    QVERIFY(pSecondSplitter);

    // for each splitter, get the corresponding container
    Container *pFirstContainer = pFirstSplitter->findChild<Sublime::Container*>();
    QVERIFY(pFirstContainer);
    Container *pSecondContainer = pSecondSplitter->findChild<Sublime::Container*>();
    QVERIFY(pSecondContainer);

    // the active view should have remained view121
    QCOMPARE(mw.activeView(), m_pView121);

    // pFirstContainer should contain the newView's widget
    QVERIFY(pFirstContainer->hasWidget(pNewView->widget()));

    // the new view's widget should be the current widget of the new container
    QCOMPARE(pFirstContainer->currentWidget(), pNewView->widget());

    // pSecondContainer should contain all the old views widgets
    QVERIFY(pSecondContainer->hasWidget(m_pView111->widget()));
    QVERIFY(pSecondContainer->hasWidget(m_pView121->widget()));
    QVERIFY(pSecondContainer->hasWidget(m_pView122->widget()));
    QVERIFY(pSecondContainer->hasWidget(m_pView131->widget()));

    // the active widget should be the current widget of the second container
    QCOMPARE(pSecondContainer->currentWidget(), mw.activeView()->widget());

    ////////////////////////////////////////////////////////////////////////////
    // now, activate the new view and check if all went well
    mw.activateView(pNewView);

    // active view should now be newView
    QCOMPARE(mw.activeView(), pNewView);

    // the active widget should be the current widget of the new container
    QCOMPARE(pFirstContainer->currentWidget(), mw.activeView()->widget());

    // the current widget of the old container should have remained view121's
    QCOMPARE(pSecondContainer->currentWidget(), m_pView121->widget());

    ////////////////////////////////////////////////////////////////////////////
    // now delete newView and check area state
    delete m_area1->removeView(pNewView);

    // verify that deleting the view did not broke the central widget
    QCOMPARE(pCentral, mw.centralWidget());

    // removing the view should have destroyed the sub splitters and containers,
    // so get the main one and verify that deleting the view did not break it
    QCOMPARE(pSplitter, pCentral->findChild<QSplitter*>());

    // get the new container inside the main splitter
    pContainer = pSplitter->findChild<Sublime::Container*>();
    QVERIFY(pContainer);

    // active view should now be back to m_pView121 again
    QCOMPARE(mw.activeView(), m_pView121);

    // check also the container current widget
    QCOMPARE(pContainer->currentWidget(), mw.activeView()->widget());
}

void AreaOperationTest::checkAreaViewsDisplay(MainWindow *mw, Area *area,
    const QString &printedAreas, int containerCount, int splitterCount, QString location)
{
    //check area
    AreaViewsPrinter viewsPrinter;
    area->walkViews(viewsPrinter, area->rootIndex());
    QCOMPARE(viewsPrinter.result, printedAreas);

    //check mainwindow
    QWidget *central = mw->centralWidget();
    QVERIFY(central != 0);
    QVERIFY(central->inherits("QWidget"));

    QWidget *splitter = central->findChild<QSplitter*>();
    QVERIFY(splitter);
    QVERIFY(splitter->inherits("QSplitter"));

    //check containers
    QList<Container*> containers = splitter->findChildren<Sublime::Container*>();
    QString failMsg = QString("\nFailure while checking area contents @ %1\n"
                              "Expected %2 containers in central splitter but got %3 \n").
                      arg(location).arg(containerCount).arg(containers.count());
    QVERIFY2(containers.count() == containerCount, failMsg.toLatin1().data());

    int widgetCount = 0;
    foreach (Container *c, containers)
    {
        for (int i = 0; i < c->count(); ++i)
        {
            QVERIFY(c->widget(i) != 0);
            QVERIFY(c->widget(i)->parentWidget() != 0);
        }
        widgetCount += c->count();
    }

    ViewCounter c;
    area->walkViews(c, area->rootIndex());
    QCOMPARE(widgetCount, c.count);

    QList<QSplitter*> splitters = splitter->findChildren<QSplitter*>();
    splitters.append(qobject_cast<QSplitter*>(splitter));
    QCOMPARE(splitters.count(), splitterCount);
}

View *AreaOperationTest::findNamedView(Area *area, const QString &name)
{
    foreach (View *view, area->views())
        if (view->objectName() == name)
            return view;
    return 0;
}

///////////
QTEST_MAIN(AreaOperationTest)

