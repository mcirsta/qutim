/****************************************************************************
 *  settingslayer_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef SETTINGSLAYER_P_H
#define SETTINGSLAYER_P_H

#include "settingslayer.h"
#include "settingswidget.h"
#include <QFormLayout>
#include <QLabel>

namespace qutim_sdk_0_3
{
	struct ConnectInfo
	{
		ConnectInfo(const char *s, QObject *r, const char *m) : signal(s), receiver(r), member(m) {}
		QByteArray signal;
		QPointer<QObject> receiver;
		QByteArray member;
	};

	class SettingsItemPrivate
	{
	public:
		SettingsItemPrivate() : gen(0), type(Settings::Invalid) {}
		mutable const ObjectGenerator *gen;
		Settings::Type type;
		QIcon icon;
		LocalizedString text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
		mutable QPointer<SettingsWidget> widget;
		QList<ConnectInfo> connections;
	};

	class AutoSettingsItemPrivate : public SettingsItemPrivate
	{
	public:
		QString config;
		QString group;
		QList<AutoSettingsItem::Entry *> entries;
	};

	struct AutoSettingsItem::EntryPrivate
	{
		LocalizedString text;
		const ObjectGenerator *gen;
		QString name;
		QList<QPair<QByteArray, QVariant> > properties;
	};

	typedef QPair<QWidget *, QByteArray> AutoSettingsEntryInfo;

	struct AutoSettingsWidgetPrivate
	{
		QList<AutoSettingsEntryInfo> entries;
	};

	class AutoSettingsWidget : public SettingsWidget
	{
		Q_OBJECT
	public:
		AutoSettingsWidget(AutoSettingsItemPrivate *pr);
		virtual ~AutoSettingsWidget();
	protected:
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
	private:
		AutoSettingsItemPrivate *p;
		QScopedPointer<AutoSettingsWidgetPrivate> g;
	};

	class AutoSettingsGenerator : public ObjectGenerator
	{
	public:
		AutoSettingsGenerator(AutoSettingsItemPrivate *pr) : p(pr) {}
	protected:
		virtual ~AutoSettingsGenerator() {}
		virtual QObject *generateHelper() const
		{
			qDebug("%s", Q_FUNC_INFO);
			if(m_object.isNull())
				m_object = new AutoSettingsWidget(p);
			return m_object.data();
		}
		virtual const QMetaObject *metaObject() const
		{
			return &AutoSettingsWidget::staticMetaObject;
		}
		virtual bool hasInterface(const char *id) const
		{
			return false;
		}
	private:
		AutoSettingsItemPrivate *p;
		mutable QPointer<QObject> m_object;
	};
}

#endif // SETTINGSLAYER_P_H
