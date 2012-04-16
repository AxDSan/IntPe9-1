#ifndef CORE_H
#define CORE_H

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
	QPixmap getIcon();
	QString getExeName();

	void addPid(int pid);
	void deletePid(int pid);


private:
	QFileInfo _dll;
	QList<int> _pidList;

};

#endif