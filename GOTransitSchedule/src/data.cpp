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
	 file.close();
	 QFile file2("data/settings.txt");
	 if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
	 	{};
	 QString b;
	 QString stationName;
	 QString hour;
	 QString minute;
	 bool flag = true;
	 bool flag2 = false;
	 bool processTimeRun = false;
	 while (!file2.atEnd()) {

		 QByteArray line = file2.readLine();
		 if (line.contains("Schedule times"))
			 flag2 = true;
		 if (flag2 == true){
		 if (flag == true && line.contains("station=")){
			 stationName = processStationName(line);
			 flag = false;
		 }
		 else if (flag == false && line.contains("Trip Departure Time")){
			 processTime(line, &hour, &minute);
			 processTimeRun = true;
		 }
		 else if (processTimeRun == true && !line.contains("Trip Departure Time"))
			 flag = true;
		 	 processTimeRun = false;
		 }
	 }
	 file2.close();
	 processFile(b);

}
QString Data::processStationName(QString input){
	int indexBegin = input.indexOf(">");
	int indexEnd = input.indexOf("</a"); //Indicates where end of station name is
	indexEnd = indexEnd - indexBegin;
	QString nameOfStation = input.mid(indexBegin+1, indexEnd-1);//Gets everything left of indexEnd (name of station)
	return nameOfStation;
}

void Data::processTime(QString input, QString *hour, QString *minute){
	int lineStart = input.indexOf("&nbsp;");
	*hour = input.mid(lineStart - 2, 2); //Hour of this bus
	*minute = input.mid(lineStart+ 6, 2); //Minute of this bus
}


QString Data::processFile(QString input){
	//Store station list (with time) into an array
	QString hour;
	QString minute;
	QStringList stationList = input.split("Takes you to the next time period.");
	for (int i = 1; i < stationList.length(); i++){
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
				int lineStart = nameOfStation.indexOf("&nbsp;", j);
				hour = nameOfStation.mid(lineStart - 2, lineStart); //Hour of this bus
				minute = nameOfStation.mid(lineStart+ 6, lineStart + 8); //Minute of this bus
				j = lineStart;

			}
		}
	}


