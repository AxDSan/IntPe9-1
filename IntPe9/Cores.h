#ifndef CORES_H
#define CORES_H

#include <QAbstractListModel>
#include <QVector>
#include <QVariant>
#include <QPixmap>
#include <QThread>
#include <QMetaObject>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QList>

#include "Communication.h"
#include "Models/Core.h"
//Views
#include "ui_Main.h"

#define COLUMNS 3

typedef QMap<int,Core*> InjectMap;
class Cores : public QAbstractListModel
{
	Q_OBJECT

public:
	Cores(Ui::mainView *view, QObject *parent = 0);
	~Cores();
	bool inject(Core *core, int pid);

	//Table implementation
	int columnCount(const QModelIndex &parent) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool injectAllCores();
	void cleanInjectedList();
	Core *haveCore(QString name);
	bool hasCommunication(uint32 pid);

public slots:
	void listenerLoop();
	void readCores(QString path);

private:
	bool _running;
	QThread *_thread;
	QVector<Core*> _cores;
	QVector<Communication*> _communications;
	InjectMap _injected;

	Ui::mainView *_view;
};

#endif