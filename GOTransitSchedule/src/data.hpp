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

class Data: public QObject
{
	Q_OBJECT
public:
	Data(QObject* parent);
	~Data();
	QString processFile(QString input);
	QString processStationName(QString input);
	void processTime(QString input, QString *hour, QString *minute);

	void fetchData();
private:
	QNetworkAccessManager* _manager;
public slots:
	void replyFinished(QNetworkReply*);

};

#endif /* DATA_HPP_ */
