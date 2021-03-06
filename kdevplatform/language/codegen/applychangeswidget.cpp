/* Copyright 2008 Aleix Pol <aleixpol@gmail.com>
 * Copyright 2009 Ramón Zarazúa <killerfox512+kde@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "applychangeswidget.h"

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <KMimeTypeTrader>

#include <QAction>
#include <QDialogButtonBox>
#include <QDir>
#include <QLabel>
#include <QMimeType>
#include <QMimeDatabase>
#include <QPushButton>
#include <QSplitter>
#include <QTemporaryFile>
#include <QTabWidget>
#include <QVBoxLayout>

#include "coderepresentation.h"
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

namespace KDevelop
{

class ApplyChangesWidgetPrivate
{
public:

    explicit ApplyChangesWidgetPrivate(ApplyChangesWidget * p)
        : parent(p), m_index(0) {}
    ~ApplyChangesWidgetPrivate()
    {
        qDeleteAll(m_temps);
    }

    void createEditPart(const KDevelop::IndexedString& url);

    ApplyChangesWidget * const parent;
    int m_index;
    QList<KParts::ReadWritePart*> m_editParts;
    QList<QTemporaryFile * > m_temps;
    QList<IndexedString > m_files;
    QTabWidget * m_documentTabs;
    QLabel* m_info;
};

ApplyChangesWidget::ApplyChangesWidget(QWidget* parent)
    : QDialog(parent), d(new ApplyChangesWidgetPrivate(this))
{
    setSizeGripEnabled(true);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    auto mainLayout = new QVBoxLayout(this);
    auto okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ApplyChangesWidget::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ApplyChangesWidget::reject);

    QWidget* w=new QWidget(this);
    d->m_info=new QLabel(w);
    d->m_documentTabs = new QTabWidget(w);
    connect(d->m_documentTabs, &QTabWidget::currentChanged,
            this, &ApplyChangesWidget::indexChanged);

    QVBoxLayout* l = new QVBoxLayout(w);
    l->addWidget(d->m_info);
    l->addWidget(d->m_documentTabs);

    mainLayout->addWidget(w);
    mainLayout->addWidget(buttonBox);

    resize(QSize(800, 400));
}

ApplyChangesWidget::~ApplyChangesWidget() = default;

bool ApplyChangesWidget::hasDocuments() const
{
    return d->m_editParts.size() > 0;
}

KTextEditor::Document* ApplyChangesWidget::document() const
{
    return qobject_cast<KTextEditor::Document*>(d->m_editParts[d->m_index]);
}

void ApplyChangesWidget::setInformation(const QString & info)
{
    d->m_info->setText(info);
}

void ApplyChangesWidget::addDocuments(const IndexedString & original)
{
    int idx=d->m_files.indexOf(original);
    if(idx<0) {
        QWidget * w = new QWidget;
        d->m_documentTabs->addTab(w, original.str());
        d->m_documentTabs->setCurrentWidget(w);

        d->m_files.insert(d->m_index, original);
        d->createEditPart(original);
    } else {
        d->m_index=idx;
    }
}

bool ApplyChangesWidget::applyAllChanges()
{
    /// @todo implement safeguard in case a file saving fails

    bool ret = true;
    for(int i = 0; i < d->m_files.size(); ++i )
        if(d->m_editParts[i]->saveAs(d->m_files[i].toUrl())) {
            IDocument* doc = ICore::self()->documentController()->documentForUrl(d->m_files[i].toUrl());
            if(doc && doc->state()==IDocument::Dirty)
                doc->reload();
        } else
            ret = false;

    return ret;
}

}

namespace KDevelop
{

void ApplyChangesWidgetPrivate::createEditPart(const IndexedString & file)
{
    QWidget * widget = m_documentTabs->currentWidget();
    Q_ASSERT(widget);

    QVBoxLayout *m=new QVBoxLayout(widget);
    QSplitter *v=new QSplitter(widget);
    m->addWidget(v);

    QUrl url = file.toUrl();

    QMimeType mimetype = QMimeDatabase().mimeTypeForUrl(url);

    KParts::ReadWritePart* part=KMimeTypeTrader::self()->createPartInstanceFromQuery<KParts::ReadWritePart>(mimetype.name(), widget, widget);
    KTextEditor::Document* document=qobject_cast<KTextEditor::Document*>(part);
    Q_ASSERT(document);

    Q_ASSERT(document->action("file_save"));
    document->action("file_save")->setEnabled(false);

    m_editParts.insert(m_index, part);

    //Open the best code representation, even if it is artificial
    CodeRepresentation::Ptr repr = createCodeRepresentation(file);
    if(!repr->fileExists())
    {
        const QString templateName = QDir::tempPath() + QLatin1Char('/') + url.fileName().split('.').last();
        QTemporaryFile * temp(new QTemporaryFile(templateName));
        temp->open();
        temp->write(repr->text().toUtf8());
        temp->close();

        url = QUrl::fromLocalFile(temp->fileName());

        m_temps << temp;
    }
    m_editParts[m_index]->openUrl(url);

    v->addWidget(m_editParts[m_index]->widget());
    v->setSizes(QList<int>() << 400 << 100);
}

void ApplyChangesWidget::indexChanged(int newIndex)
{
    Q_ASSERT(newIndex != -1);
    d->m_index = newIndex;
}

void ApplyChangesWidget::updateDiffView(int index)
{
    d->m_index = index == -1 ? d->m_index : index;
}

}

