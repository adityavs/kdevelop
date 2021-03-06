/*
 * This file is part of qmljs, the QML/JS language support plugin for KDevelop
 * Copyright (c) 2014 Denis Steckelmacher <steckdenis@yahoo.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __MODULECOMPLETIONITEM_H__
#define __MODULECOMPLETIONITEM_H__

#include <language/codecompletion/codecompletionitem.h>

namespace QmlJS {

class ModuleCompletionItem : public KDevelop::CompletionTreeItem
{
public:
    enum Decoration {
        Import,        /*!< "import module", used for QML module imports */
        Quotes         /*!< Put quotes around the module name */
    };

    ModuleCompletionItem(const QString &name, Decoration decoration);

    QVariant data(const QModelIndex& index, int role, const KDevelop::CodeCompletionModel* model) const override;
    int inheritanceDepth() const override;
    int argumentHintDepth() const override;
    KTextEditor::CodeCompletionModel::CompletionProperties completionProperties() const override;

    void execute(KTextEditor::View* view, const KTextEditor::Range& word) override;

private:
    QString m_name;
    Decoration m_decoration;
};

}

#endif
