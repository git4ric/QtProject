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

class Data: public QObject
{
	Q_OBJECT
public:
	Data(QObject* parent);
	~Data();
	void processFile();
	QString processStationName(QString input);
	QPair<QString,QString> processTime(QString input);

	void fetchData();
private:
	QNetworkAccessManager* _manager;
	bool _stopFetchFlag;
	QString _baseUrl;
public slots:
	void replyFinished(QNetworkReply*);
	void sendRequest(QString);

};

#endif /* DATA_HPP_ */
