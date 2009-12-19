/****************************************************************************
*  popupwidget.cpp
*
*  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
*
***************************************************************************
*                                                                         *
*   This library is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
*****************************************************************************/

#include "popupwidget.h"
#include <QVBoxLayout>
#include "manager.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QDesktopWidget>
#include <3rdparty/qtwin/qtwin.h>
#include <libqutim/emoticons.h>
#include <libqutim/chatunit.h>
#include <libqutim/messagesession.h>

namespace KineticPopups
{
	PopupWidget::PopupWidget (const ThemeHelper::PopupSettings &popupSettings)
	{
		//init browser
		setTheme(popupSettings);
		if (popupSettings.popupFlags & ThemeHelper::Preview) {
			setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
		}
		else {
			setWindowFlags(popup_settings.widgetFlags);
			setAttribute(Qt::WA_DeleteOnClose);
			if (popupSettings.popupFlags & ThemeHelper::Transparent) {
				setAttribute(Qt::WA_NoSystemBackground);
				setAttribute(Qt::WA_TranslucentBackground);
			}

			if (popupSettings.popupFlags & ThemeHelper::AeroThemeIntegration) {
				//init aero integration for win
				if (QtWin::isCompositionEnabled()) {
					QtWin::extendFrameIntoClientArea(this);
					setContentsMargins(0, 0, 0, 0);
				}
			}

		}
		setFrameShape ( QFrame::NoFrame );
		setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
		setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
		setContextMenuPolicy(Qt::NoContextMenu);
		setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
	}


	QSize PopupWidget::setData ( const QString& title, const QString& body, QObject *sender )
	{
		m_sender = sender;
		QString image_path = sender ? sender->property("avatar").toString() : QString();
		if(image_path.isEmpty())
			image_path = QLatin1String(":/icons/qutim_64");
		QString data = popup_settings.content;
		QString text = Emoticons::theme().parseEmoticons(Qt::escape(body));
		if (text.length() > Manager::self()->maxTextLength) {
			text.truncate(Manager::self()->maxTextLength);
			text.append("...");
		}
		data.replace ( "{title}", title );
		data.replace ( "{body}", text);
		data.replace ( "{imagepath}",image_path);
		document()->setTextWidth(popup_settings.defaultSize.width());
		document()->setHtml(data);
		int width = popup_settings.defaultSize.width();
		int height = document()->size().height();

		return QSize(width,height);
	}


	void PopupWidget::setTheme (const ThemeHelper::PopupSettings &popupSettings )
	{
		popup_settings = popupSettings;
		this->setStyleSheet (popup_settings.styleSheet);
	}


	void PopupWidget::mouseReleaseEvent ( QMouseEvent* ev )
	{
		if (ev->button() == Manager::self()->action1Trigger) {
			onAction1Triggered();
		}
		else if (ev->button() == Manager::self()->action2Trigger)
			onAction2Triggered();
		else
			return;
		emit actionActivated();
	}
	
	PopupWidget::~PopupWidget()
	{
	}


	void KineticPopups::PopupWidget::onAction1Triggered()
	{
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit))) {
			sess->setActive(true);
		}
		else {
			QWidget *widget = qobject_cast<QWidget *>(m_sender);
			if (widget)
				widget->raise();
		}
	}

	void KineticPopups::PopupWidget::onAction2Triggered()
	{
		ChatUnit *unit = qobject_cast<ChatUnit *>(m_sender);
		ChatSession *sess;
		if (unit && (sess = ChatLayer::get(unit,false))) {
			//TODO
			if (!sess->isActive())
				sess->deleteLater();
		}
	}

	void PopupWidget::onTimeoutReached()
	{
		//TODO
		emit actionActivated();
	}
}





