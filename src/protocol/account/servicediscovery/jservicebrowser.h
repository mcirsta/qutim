#ifndef JSERVICEBROWSER_H
#define JSERVICEBROWSER_H

#include <QDialog>
#include <QKeyEvent>
#include <gloox/jid.h>
#include <qutim/icon.h>
#include "jservicereceiver.h"
#include "ui_jservicebrowser.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	using namespace gloox;

	class JAccount;
	class JDiscoItem;
	class JServiceDiscovery;

	struct JServiceBrowserPrivate;

	class JServiceBrowser : public QDialog, public JServiceReceiver
	{
		Q_OBJECT
		public:
			JServiceBrowser(JAccount *account, QWidget *parent = 0);
			~JServiceBrowser();
			virtual void setInfo(int id);
			virtual void setItems(int id, const QList<JDiscoItem *> &items);
			virtual void setError(int id, JDiscoItem * di) {}
		protected:
			bool eventFilter(QObject *obj, QEvent *event);
			void searchServer(const QString &server);
			void getInfo(QTreeWidgetItem *item);
			void hideControls();
			void setItemVisible(QTreeWidgetItem *item, bool visibility);
			QList<QTreeWidgetItem *> findItems(QTreeWidgetItem *item, const QString &text);
			void setBranchVisible(QList<QTreeWidgetItem *> items);
			QString setServiceIcon(JDiscoItem *di);
		private slots:
			void getItems(QTreeWidgetItem *item);
			void showControls(QTreeWidgetItem*, int);
			void filterItem(const QString &mask);
			void closeEvent(QCloseEvent*);
			void showFilterLine(bool show);
			void on_searchButton_clicked();
			void on_closeButton_clicked();
			/*void on_joinButton_clicked();
			void on_registerButton_clicked();
			void on_searchFormButton_clicked();
			void on_executeButton_clicked();
			void on_showVCardButton_clicked();
			void on_addProxyButton_clicked();
			void on_addRosterButton_clicked();*/
		private:
			QScopedPointer<JServiceBrowserPrivate> p;
		/*signals:
			void finishSearch();
			void joinConference(const QString &conference);
			void registerTransport(const QString &transport);
			void executeCommand(const QString &command, const QString &node);
			void showVCard(const QString &jid);
			void addProxy(const JID &proxy);
			void addContact(const QString &jid, const QString &nick);
			void searchService(const QString &type, const QString &jid);*/
	};
}

#endif // JSERVICEBROWSER_H
