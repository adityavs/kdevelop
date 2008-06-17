/* This file is part of KDevelop
    Copyright 2006-2008 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KDEV_DECLARATIONBUILDER_H
#define KDEV_DECLARATIONBUILDER_H

#include "language/duchain/basecontextbuilder.h"
#include "language/duchain/declaration.h"
#include "language/duchain/classfunctiondeclaration.h"
#include "language/duchain/symboltable.h"
#include "language/duchain/forwarddeclaration.h"
#include "language/duchain/identifiedtype.h"
#include "language/duchain/functiondeclaration.h"

namespace KDevelop
{
class Declaration;

/**
 * A class which iterates the AST to extract definitions of types.
 */
template<typename T>
class KDEVPLATFORMLANGUAGE_EXPORT AbstractDeclarationBuilder : public BaseContextBuilder<T>
{
public:
  AbstractDeclarationBuilder( EditorIntegrator* editor, bool ownsEditorIntegrator )
    : BaseContextBuilder<T>(editor, ownsEditorIntegrator)
  {
  }

  virtual TopDUContext* buildDeclarations( const IndexedString& url, T* node, 
                              const TopDUContextPointer& updateContext
                                    = TopDUContextPointer() )
  {
    return buildContexts(url, node, updateContext);
  }

protected:
  inline bool hasCurrentDeclaration() const { return !m_declarationStack.isEmpty(); }
  inline Declaration* currentDeclaration() const { return m_declarationStack.isEmpty() ? 0 : m_declarationStack.top(); }
  template<class DeclarationType>
  inline DeclarationType* currentDeclaration() const { return m_declarationStack.isEmpty() ? 0 : dynamic_cast<DeclarationType*>(m_declarationStack.top()); }

  inline void setComment(const QString& comment) { m_lastComment = comment; }

  /**
   * Register a new declaration with the definition-use chain.
   * Returns the new declaration created.
   * @param name When this is zero, the identifier given through customName is used.
   * \param range provide a valid AST node here if name is null
   */
  Declaration* openDeclaration(T* name, T* range, bool isFunction = false, bool isForward = false, bool isDefinition = false, bool isLocked = false)
  {
    if (!isLocked)
      DUChain::lock()->lockForWrite();

    SimpleRange newRange = editorFindRange(name ? name : range, name ? name : range);

    if (newRange.start >= newRange.end)
      kWarning() << "Range collapsed";

    QualifiedIdentifier id = identifierForNode(name);

    Identifier localId;

    if(!id.isEmpty()) {
      localId = id.last();
    }

    Declaration* declaration = 0;

    if (BaseContextBuilder<T>::recompiling()) {
      // Seek a matching declaration

      // Translate cursor to take into account any changes the user may have made since the text was retrieved
      SimpleRange translated = newRange;

      if (BaseContextBuilder<T>::editor()->smart()) {
        QMutexLocker smartLock(BaseContextBuilder<T>::editor()->smart()->smartMutex());
        translated = SimpleRange( BaseContextBuilder<T>::editor()->smart()->translateFromRevision(translated.textRange()) );
      }

      foreach( Declaration* dec, BaseContextBuilder<T>::currentContext()->allLocalDeclarations(localId) ) {

        if( BaseContextBuilder<T>::wasEncountered(dec) )
          continue;

        //This works because dec->textRange() is taken from a smart-range. This means that now both ranges are translated to the current document-revision.
        if (dec->range() == translated &&
            ((id.isEmpty() && dec->identifier().toString().isEmpty()) || (!id.isEmpty() && localId == dec->identifier())) &&
            dec->isDefinition() == isDefinition
            //&& extraDeclarationComparisons()
          )
        {
          if(BaseContextBuilder<T>::currentContext()->type() == DUContext::Class && !dynamic_cast<ClassMemberDeclaration*>(dec))
            continue;
          /*if(isNamespaceAlias && !dynamic_cast<NamespaceAliasDeclaration*>(dec)) {
            continue;
          } else */if (isForward && !dynamic_cast<ForwardDeclaration*>(dec)) {
            continue;
          } else if (isFunction) {
            if (BaseContextBuilder<T>::currentContext()->type() == DUContext::Class) {
              if (!dynamic_cast<ClassFunctionDeclaration*>(dec))
                continue;
            } else if (!dynamic_cast<AbstractFunctionDeclaration*>(dec)) {
              continue;
            }

          } else if (BaseContextBuilder<T>::currentContext()->type() == DUContext::Class) {
            if (!isForward && !dynamic_cast<ClassMemberDeclaration*>(dec)) //Forward-declarations are never based on ClassMemberDeclaration
              continue;
          }

          // Match
          declaration = dec;

          // Update access policy if needed
          // updateAccessPolicy()
          break;
        }
      }
    }

    if (!declaration) {
      KTextEditor::SmartRange* prior = BaseContextBuilder<T>::editor()->currentRange();
      KTextEditor::SmartRange* range = BaseContextBuilder<T>::editor()->createRange(newRange.textRange());

      BaseContextBuilder<T>::editor()->exitCurrentRange();
      //Q_ASSERT(range->start() != range->end());

      Q_ASSERT(BaseContextBuilder<T>::editor()->currentRange() == prior);

      if (isForward) {
        declaration = new ForwardDeclaration(BaseContextBuilder<T>::editor()->currentUrl(), newRange, BaseContextBuilder<T>::currentContext());
      } else if (isFunction) {
        if (BaseContextBuilder<T>::currentContext()->type() == DUContext::Class) {
          declaration = new ClassFunctionDeclaration(BaseContextBuilder<T>::editor()->currentUrl(), newRange, BaseContextBuilder<T>::currentContext());
        } else {
          declaration = new FunctionDeclaration(BaseContextBuilder<T>::editor()->currentUrl(), newRange, BaseContextBuilder<T>::currentContext());
        }
      } else if (BaseContextBuilder<T>::currentContext()->type() == DUContext::Class) {
          declaration = new ClassMemberDeclaration(BaseContextBuilder<T>::editor()->currentUrl(), newRange, BaseContextBuilder<T>::currentContext());
      } else {
        declaration = new Declaration(BaseContextBuilder<T>::editor()->currentUrl(), newRange, BaseContextBuilder<T>::currentContext());
      }
      
      declaration->setSmartRange(range);
      declaration->setDeclarationIsDefinition(isDefinition);
      declaration->setIdentifier(localId);

      switch (BaseContextBuilder<T>::currentContext()->type()) {
        case DUContext::Global:
        case DUContext::Namespace:
        case DUContext::Class:
          SymbolTable::self()->addDeclaration(declaration);
          break;
        default:
          break;
      }
    }

    declaration->setComment(m_lastComment);
    m_lastComment.clear();

    BaseContextBuilder<T>::setEncountered(declaration);

    m_declarationStack.push(declaration);

    if (!isLocked)
      DUChain::lock()->releaseWriteLock();

    return declaration;
  }

  /// Same as the above, but sets it as the definition too
  Declaration* openDefinition(T* name, T* range, bool isFunction = false)
  {
    return openDeclaration(name, range, isFunction, false, true);
  }

  ForwardDeclaration* openForwardDeclaration(T* name, T* range)
  {
    return static_cast<ForwardDeclaration*>(openDeclaration(name, range, false, true));
  }
    
  void eventuallyAssignInternalContext()
  {
    if (BaseContextBuilder<T>::lastContext()) {
      DUChainWriteLocker lock(DUChain::lock());

      if( dynamic_cast<ClassFunctionDeclaration*>(currentDeclaration()) )
        Q_ASSERT( !static_cast<ClassFunctionDeclaration*>(currentDeclaration())->isConstructor() || currentDeclaration()->context()->type() == DUContext::Class );

      if(BaseContextBuilder<T>::lastContext() && (BaseContextBuilder<T>::lastContext()->type() == DUContext::Class || BaseContextBuilder<T>::lastContext()->type() == DUContext::Other || BaseContextBuilder<T>::lastContext()->type() == DUContext::Function || BaseContextBuilder<T>::lastContext()->type() == DUContext::Template ) )
      {
        if( !BaseContextBuilder<T>::lastContext()->owner() || !BaseContextBuilder<T>::wasEncountered(BaseContextBuilder<T>::lastContext()->owner()) ) { //if the context is already internalContext of another declaration, leave it alone
          currentDeclaration()->setInternalContext(BaseContextBuilder<T>::lastContext());

          if( currentDeclaration()->range().start >= currentDeclaration()->range().end )
            kDebug() << "Warning: Range was invalidated";

          BaseContextBuilder<T>::clearLastContext();
        }
      }
    }
  }
    
  virtual void closeDeclaration()
  {
    m_declarationStack.pop();
  }
    
  void abortDeclaration()
  {
    m_declarationStack.pop();
  }

#if 0
  ///Creates a declaration of the given type, or if the current declaration is a template-declaration, it creates a template-specialized version of that type.
  template<class DeclarationType>
  DeclarationType* specialDeclaration( KTextEditor::SmartRange* smartRange, const SimpleRange& range );
  ///Creates a declaration of the given type, or if the current declaration is a template-declaration, it creates a template-specialized version of that type.
  template<class DeclarationType>
  DeclarationType* specialDeclaration( KTextEditor::SmartRange* smartRange, const SimpleRange& range, int scope );
#endif

private:
  QStack<Declaration*> m_declarationStack;
  QString m_lastComment;
};

}

#endif // KDEV_DECLARATIONBUILDER_H
