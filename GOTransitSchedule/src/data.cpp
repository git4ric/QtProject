/*
 * data.cpp
 *
 *  Created on: 2013-03-03
 *      Author: user
 */

#include "data.hpp"

#include <qt4/QtCore/QFile>

Data::Data(QObject* parent){
	_manager = new QNetworkAccessManager(this);
	QObject::connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
}

Data::~Data(){

}

void Data::fetchData(){
	_manager->get(QNetworkRequest(QUrl("http://www.gotransit.com/publicroot/en/schedules/pubsched.aspx?table=20&direction=0&day=1&page=1&new=")));
}
void Data::replyFinished(QNetworkReply* reply){
	const QByteArray arr = reply->readAll();
	QFile file(QString("/var/tmp/output.txt"));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		 return;
	 file.write(arr);
	 file.close();
}
