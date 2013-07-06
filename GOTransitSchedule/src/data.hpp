/*
 * data.hpp
 *
 *  Created on: 2013-03-03
 *      Author: user
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include <QObject>
#include <qt4/QtNetwork/QNetworkAccessManager>
#include <qt4/QtNetwork/QNetworkRequest>
#include <qt4/QtNetwork/QNetworkReply>
#include <qt4/QtCore/QPair>
#include <qt4/QtCore/QDebug>
#include <qt4/QtXml/QDomDocument>
#include <qt4/QtXml/QDomElement>
#include <qt4/QtXml/QDomText>

class Data: public QObject
{
	Q_OBJECT
public:
	Data(QObject* parent, QString routeNumber);
	~Data();
	void processFile();
	QString processStationName(QString input);
	QPair<QString,QString> processTime(QString input);
	bool _stopFetchFlag, _direction, _doneSaving;
	bool hasTimeTag(QDomElement*, QDomText*);

private:
	QNetworkAccessManager* _manager;
	QString _baseUrl;
	QDomDocument _doc;

public slots:
	void replyFinished(QNetworkReply*);
	void update();
	void fetchData();
	void saveXMLFile();
};

#endif /* DATA_HPP_ */
