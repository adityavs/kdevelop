/***************************************************************************
 *   Copyright (C) 2003 by Roberto Raggi                                   *
 *   roberto@kdevelop.org                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "codeinformationrepository.h"
#include "cpp_tags.h"

#include <kdebug.h>

/// @todo move in utils.cpp
static QValueList<KTextEditor::CompletionEntry>
my_unique( const QValueList<KTextEditor::CompletionEntry>& entryList )
{

    QValueList< KTextEditor::CompletionEntry > l;
    QMap<QString, bool> map;
    QValueList< KTextEditor::CompletionEntry >::ConstIterator it=entryList.begin();
    while( it != entryList.end() ){
        KTextEditor::CompletionEntry e = *it++;
        QString key = e.type + " " +
                      e.text + " " +
                      e.prefix + " " +
                      e.postfix + " ";
        if( map.find(key) == map.end() ){
            map[ key ] = TRUE;
            l << e;
        }
    }
    return l;
}

CodeInformationRepository::CodeInformationRepository()
{
}

CodeInformationRepository::~CodeInformationRepository()
{
}

void CodeInformationRepository::addCatalog( const QString & id, Catalog * catalog )
{
    m_catalogs[ id ] = catalog;
}

void CodeInformationRepository::removeCatalog( const QString & id )
{
    m_catalogs.remove( id );
}

QValueList<Tag> CodeInformationRepository::query( const QValueList<Catalog :: QueryArgument> & args )
{
    kdDebug(9007) << "CodeInformationRepository::query()" << endl;

    QValueList<Tag> tags;

    QMap<QString, Catalog*>::Iterator it = m_catalogs.begin();
    while( it != m_catalogs.end() ){
        Catalog* catalog = it.data();
        ++it;

        tags += catalog->query( args );
    }

    return tags;
}

QValueList<Tag> CodeInformationRepository::getTagsInFile( const QString & fileName )
{
    kdDebug(9007) << "CodeInformationRepository::getTagsInFile()" << endl;

    QValueList<Catalog::QueryArgument> args;
    args << Catalog::QueryArgument( "fileName", fileName );

    QMap<QString, Catalog*>::Iterator it = m_catalogs.begin();
    while( it != m_catalogs.end() ){
        Catalog* catalog = it.data();
        ++it;

        QValueList<Tag> tags = catalog->query( args );

        if( tags.size() )
            return tags;
    }

    return QValueList<Tag>();
}

QValueList<Tag> CodeInformationRepository::getTagsInScope( const QStringList & scope, bool isInstance )
{
    kdDebug(9007) << "CodeInformationRepository::getTagsInScope()" << endl;

    QValueList<Tag> tags;
    QValueList<Catalog::QueryArgument> args;

#if 0
    args.clear();
    args << Catalog::QueryArgument( "kind", Tag::Kind_Namespace )
	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );

    args.clear();
    args << Catalog::QueryArgument( "kind", Tag::Kind_Class )
	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );
#endif

    args.clear();
    args << Catalog::QueryArgument( "kind", Tag::Kind_FunctionDeclaration )
	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );

    args.clear();
    args << Catalog::QueryArgument( "kind", Tag::Kind_Variable )
    	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );

    if( true /*!isInstance*/ ){
	args.clear();
	args << Catalog::QueryArgument( "kind", Tag::Kind_Enumerator )
	    << Catalog::QueryArgument( "scope", scope );
	tags += query( args );
    }

    return tags;
}

QValueList<KTextEditor::CompletionEntry> CodeInformationRepository::getEntriesInScope( const QStringList & scope, bool isInstance, bool recompute )
{
    kdDebug(9007) << "CodeInformationRepository::getEntriesInScope()" << endl;

    if( !recompute && !scope.size() && m_globalEntries.size() )
	return m_globalEntries;
    else if( scope.size() == 0 ){
	m_globalEntries = my_unique( toEntryList( getTagsInScope(scope, isInstance) ) );
	return m_globalEntries;
    }

    return toEntryList( getTagsInScope(scope, isInstance) );
}


QValueList<Tag> CodeInformationRepository::getBaseClassList( const QString& className )
{
    kdDebug(9007) << "CodeInformationRepository::getBaseClasseList()" << endl;

    if( className.isEmpty() )
        return QValueList<Tag>();

    QValueList<Catalog::QueryArgument> args;
    args << Catalog::QueryArgument( "kind", Tag::Kind_Base_class )
    	<< Catalog::QueryArgument( "name", className );
    return query( args );
}

QValueList<Tag> CodeInformationRepository::getClassOrNamespaceList( const QStringList & scope )
{
    kdDebug(9007) << "CodeInformationRepository::getClassOrNamespaceList()" << endl;

    QValueList<Tag> tags;
    QValueList<Catalog::QueryArgument> args;

    args << Catalog::QueryArgument( "kind", Tag::Kind_Namespace )
	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );

    args.clear();
    args << Catalog::QueryArgument( "kind", Tag::Kind_Class )
    	<< Catalog::QueryArgument( "scope", scope );
    tags += query( args );

    return tags;
}

QValueList<Tag> CodeInformationRepository::getTagsInScope( const QString & name, const QStringList & scope )
{
    QValueList<Tag> tags;
    QValueList<Catalog::QueryArgument> args;

    args.clear();
    args << Catalog::QueryArgument( "scope", scope )
	<< Catalog::QueryArgument( "name", name );

    tags += query( args );

    return tags;
}

KTextEditor::CompletionEntry CodeInformationRepository::toEntry( Tag & tag, CppCodeCompletion::CompletionMode completionMode )
{
    KTextEditor::CompletionEntry entry;

    if( tag.name().isEmpty() )
	return entry;

    switch( tag.kind() ){
	case Tag::Kind_Typedef:
	    entry.prefix = "typedef";
	    entry.text = tag.name();
	break;

	case Tag::Kind_Class:
	    entry.prefix = "class";
	    entry.text = tag.name();
	break;

	case Tag::Kind_Namespace:
	    entry.prefix = "namespace";
	    entry.text = tag.name();
	break;

	case Tag::Kind_FunctionDeclaration:
	    //case Tag::Kind_Function:
	{
	    entry.text = tag.name();
	    entry.text += "(";

	    CppFunction<Tag> tagInfo( tag );
	    QStringList arguments = tagInfo.arguments();
	    QStringList argumentNames = tagInfo.argumentNames();

	    QString signature;
	    for( uint i=0; i<arguments.size(); ++i ){
		signature += arguments[ i ];
		if( completionMode == CppCodeCompletion::NormalCompletion ){
		    QString argName = argumentNames[ i ];
		    if( !argName.isEmpty() )
		        signature += QString::fromLatin1( " " ) + argName;
		}

		if( i != (arguments.size()-1) ){
		    signature += ", ";
		}
	    }

	    if( signature.isEmpty() )
		entry.text += ")";
	    else
		entry.postfix = signature + " )";

	    if( tagInfo.isConst() )
		entry.postfix += " const";

	    if( completionMode != CppCodeCompletion::NormalCompletion ){
	        entry.text += entry.postfix;
		entry.postfix = QString::null;
	    }

	    QString comment = tag.attribute("description").toString();
	    if (!comment.isNull())
		entry.comment = comment;
	    //else
	    //entry.comment = "no documentation available!";
	}

	break;

	case Tag::Kind_Enumerator:
        case Tag::Kind_Variable:
	    entry.text = tag.name();
	    break;

	default:
	;
    }

    return entry;
}

QValueList<KTextEditor :: CompletionEntry> CodeInformationRepository::toEntryList( const QValueList<Tag> & tags, CppCodeCompletion::CompletionMode completionMode )
{
    QValueList<KTextEditor :: CompletionEntry> entryList;
    QMap<QString, bool> ns;

    QValueList<Tag>::ConstIterator it = tags.begin();
    while( it != tags.end() ){
	Tag tag = *it;
	++it;

	KTextEditor::CompletionEntry entry = toEntry( tag, completionMode );
	if( !entry.text.isEmpty() )
	    entryList << entry;
    }

    return entryList;
}


