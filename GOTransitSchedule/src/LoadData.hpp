/*
 * LoadData.h
 *
 *  Created on: 2013-04-03
 *      Author: Yasir
 */

#ifndef LOADDATA_H_
#define LOADDATA_H_
#include <QObject>
#include <qt4/QtCore/QString>
#include <qt4/QtCore/QStringList>
 #include <QDir>
#include <qt4/QtCore/QTimer>
#include <qt4/QtCore/QFile>
#include <qt4/QtTest/QTest>
#include <qt4/QtCore/QRegExp>
#include <qt4/QtXml/QDomDocument>
#include <qt4/QtXml/QDomElement>
#include <qt4/QtXml/QDomNamedNodeMap>
#include <qt4/QtXml/QDomText>
#include <qt4/QtXml/QDomAttr>

#include <bb/data/XmlDataAccess>

class LoadData : public QObject
{
	Q_OBJECT
public:
	LoadData(QObject* parent, QString fileInput);
	~LoadData();
	void parseXML();
	void setHour(int h);
	void setMinute(int m);
	void setStation(QString s);
	void setDirection(QString d);
	void addBusTimes(QDomNode n);
	QStringList getStations();
	QList<QStringList> getBuses();
	bool getFailFlag();
	void initializeBusArrays();
	void addSubsequentBusTimes(QDomNode n, int index);
	QString checkSpecialCases(QString name);
};

#endif /* LOADDATA_H_ */
