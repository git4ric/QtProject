/*
 * data.cpp
 *
 *  Created on: 2013-03-03
 *      Author: user
 */

#include "data.hpp"
 #include <QString>
 #include <QStringList>

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
	QFile file(QString("data/settings.txt"));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		 return;
	 file.write(arr);
	 QString input = arr;
	 processFile(input);
	 file.close();
}
QString Data::processFile(QString input){
	//Store station list (with time) into an array
	QString hour;
	QString minute;
	QStringList stationList = input.split("Takes you to the next time period.");
	for (int i = 0; i < stationList.length(); i++){
		//TimeList[1] will store name of this station followed by times listed out
		QStringList timeList = stationList[i].split("station=");
		//Gets name of this station
		int indexEnd = timeList[1].indexOf("</a"); //Indicates where end of station name is
		int indexBegin = 15;
		QString nameOfStation = timeList[1].mid(indexBegin, indexEnd);//Gets everything left of indexEnd (name of station)
														//for (int j = 0; j < timeList.length(); j++){
		int j = 0;
			while (j < 500){ //Fix this condition
				//Get each hour
				int lineStart = nameOfStation.indexOf(j, "&nbsp;");
				hour = nameOfStation.mid(lineStart - 2, lineStart); //Hour of this bus
				minute = nameOfStation.mid(lineStart+ 8, lineStart + 10); //Minute of this bus
				j = line;

			}
		}
	}
		
}


