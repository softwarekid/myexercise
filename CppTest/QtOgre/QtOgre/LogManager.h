#ifndef QTOGRE_LOGMANAGER_H_
#define QTOGRE_LOGMANAGER_H_

#include "Log.h"

#include <QTabWidget>
#include <QDialog>

namespace QtOgre
{
	class LogManager : public QDialog
	{
		Q_OBJECT

	public:
		LogManager(QWidget *parent = 0);

		Log* createLog(const QString& name);
		Log* getLogByName(const QString& name);

		//Sets the forceProcessEvents property on all the logs.
		//NOTE: Any logs added after this call will have the
		//default value, so you may need to call this again.
		void setForceProcessEvents(bool forceProcessEvents);

		void setVisibleLog(Log* log);

	private:
		QTabWidget* mLogTabs;
		
		QGridLayout* mGridLayout;

		QMap<QString, Log*> mLogs;
	};
}

#endif /*QTOGRE_LOGMANAGER_H_*/