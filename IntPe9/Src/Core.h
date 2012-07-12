#ifndef CORE_H
#define CORE_H

#include <Common.h>

#include <QVariant>
#include <QAbstractListModel>
#include <QPixmap>
#include <QFileInfo>
#include <QList>

class Core : public QObject
{
	Q_OBJECT

public:
	Core(const QFileInfo *dll);
	~Core();

	//Custom stuff
	QString getFullPath();
	QVariant getField(int column);

	QString getName();
	QString getVersion();
	QString getProcessName();

	bool isEnabled();
	void setEnabled(bool state);

	void addPid(int pid);
	void deletePid(int pid);


private:
	QFileInfo _dll;
	QList<int> _pidList;
	bool _hasInfo;
	bool _isEnabled;
	CoreInfo _info;

};

#endif