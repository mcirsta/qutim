/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "emoeditplugin.h"
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <qutim/debug.h>
#include <QPainter>
#include <QTextList>

// FIXME: find way to create unique type id
static int emoticonsObjectType = QTextFormat::UserObject + 1;

class EmoEditTextIconFormat : public QTextCharFormat
{
public:
	enum Property {
			IconPath = QTextFormat::UserProperty + 1,
			IconText = QTextFormat::UserProperty + 2
	};
	
	EmoEditTextIconFormat(const EmoticonsTheme::Token &token)
	{
        setObjectType(emoticonsObjectType);
        QTextFormat::setProperty(IconPath, token.imgPath);
        QTextFormat::setProperty(IconText, token.text);
		setToolTip(token.text);
	}
};

EmoEditPlugin::EmoEditPlugin()
{
}

void EmoEditPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Developer"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "EmoEdit plugin"),
			QT_TRANSLATE_NOOP("Plugin", "Add emoticons to text edit widget"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon());
	m_inParsingState = false;
}

bool EmoEditPlugin::load()
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	foreach (ChatSession *session, ChatLayer::instance()->sessions())
		onSessionCreated(session);
	m_theme.reset(new EmoticonsTheme());
	return true;
}

bool EmoEditPlugin::unload()
{
	foreach (ChatSession *session, ChatLayer::instance()->sessions()) {
		disconnect(session, 0, this, 0);
		disconnect(session->getInputField(), 0, this, 0);
	}
	return true;
}

QSizeF EmoEditPlugin::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
	Q_UNUSED(doc);
	Q_UNUSED(posInDocument);
	const QTextCharFormat charFormat = format.toCharFormat();
	return QPixmap(charFormat.stringProperty(EmoEditTextIconFormat::IconPath)).size();
}

void EmoEditPlugin::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
							   int posInDocument, const QTextFormat &format)
{
	Q_UNUSED(doc);
	Q_UNUSED(posInDocument);
	const QTextCharFormat charFormat = format.toCharFormat();
	const QPixmap pixmap = QPixmap(charFormat.stringProperty(EmoEditTextIconFormat::IconPath));
	
	painter->drawPixmap(rect, pixmap, pixmap.rect());
}

void EmoEditPlugin::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	QTextDocument *doc = session->getInputField();
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(onSessionDestroyed(QObject*)));
	connect(doc, SIGNAL(contentsChanged()), this, SLOT(onDocumentContentsChanged()));
	doc->documentLayout()->registerHandler(emoticonsObjectType, this);
	onDocumentContentsChanged(doc);
}

void EmoEditPlugin::onSessionDestroyed(QObject *obj)
{
	ChatSession *session = static_cast<ChatSession*>(obj);
	Q_UNUSED(session);
}

void EmoEditPlugin::onDocumentContentsChanged(QTextDocument *doc)
{
	if (m_inParsingState)
		return;
	m_inParsingState = true;
	if (!doc)
		doc = qobject_cast<QTextDocument*>(sender());
	QTextBlock block = doc->firstBlock();
	while (block.length() > 0) {
		QString text = block.text();
		const QList<EmoticonsTheme::Token> tokens = m_theme->tokenize(text);
		if (tokens.isEmpty() 
			|| (tokens.size() == 1 && tokens.at(0).type == EmoticonsTheme::Text)) {
			block = block.next();
			continue;
		}
		QTextBlock nextBlock = block.next();
		QTextCursor cursor(block);
		for (int i = 0; i < tokens.size(); i++) {
			const EmoticonsTheme::Token &token = tokens.at(i);
			if (token.type == EmoticonsTheme::Image) {
				QTextCharFormat format = cursor.charFormat();
				cursor = doc->find(token.text, cursor, QTextDocument::FindCaseSensitively);
				
				EmoEditTextIconFormat icon(token);
				cursor.insertText(QString(QChar::ObjectReplacementCharacter), icon);
				cursor.clearSelection();
		
				cursor.setCharFormat(format);
			} else if (token.type == EmoticonsTheme::Text) {
				cursor.setPosition(cursor.position() + token.text.size(), QTextCursor::KeepAnchor);
			}
		}
		block = nextBlock;
	}
	m_inParsingState = false;
}

QUTIM_EXPORT_PLUGIN(EmoEditPlugin)
