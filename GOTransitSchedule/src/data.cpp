/*
 * data.cpp
 *
 *  Created on: 2013-03-03
 *      Author: user
 */

#include "data.hpp"
#include <qt4/QtCore/QString>
#include <qt4/QtCore/QStringList>
#include <qt4/QtCore/QDebug>
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
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		qDebug() << "Could not open file for writing html";
		return;
	}
	file.write(arr);
	file.close();
	processFile();
}
QString Data::processStationName(QString input){
	int indexBegin = input.indexOf(">");
	int indexEnd = input.indexOf("</a"); //Indicates where end of station name is
	indexEnd = indexEnd - indexBegin;
	QString nameOfStation = input.mid(indexBegin+1, indexEnd-1);//Gets everything left of indexEnd (name of station)
	return nameOfStation;
}

QPair<QString,QString> Data::processTime(QString input){
	int lineStart = input.indexOf("&nbsp;");
	QPair<QString,QString> time;
	time.first = input.mid(lineStart - 2, 2); //Hour of this bus
	time.second = input.mid(lineStart+ 6, 2); //Minute of this bus
	return time;
}

void Data::processFile(){
	QFile file("data/settings.txt");

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qDebug() << "Could not open file for parsing";
		return;
	}

	QStringList stationName;
	QPair<QString,QString> time;

	int i=0;


	/*
	 *  Finds station name using "/publicroot/en/travelling/stations.aspx?station=" query
	 *
	 *  Finds times based on color scheme on the page
	 *  id="G" - stands for Green times aka Train timings
	 *  id="Y" - stands for Yellow times aka Bus timings
	 *  id="P" - stands for Transfer Node - Logic sorts them out
	 *  id="E" - stands for Gray Table number - Logic sorts them out
	 */
	while (!file.atEnd()) {
		 QByteArray line = file.readLine();
		 if (line.contains("/publicroot/en/travelling/stations.aspx?station=")){
			 stationName.append(processStationName(line));
			 //filter = true;
			 qDebug() << stationName[i];
			 i++;
		 }
		 else if(line.contains("Transfer Node")){
			 qDebug() << "Transfer required after station: " << stationName[i-1];
		 }
		 else if (line.contains("id=\"G\"") or line.contains("id=\"Y\"")){
			 time = processTime(line);
			 qDebug() << "time is: " << time.first << "." << time.second;
		 }
	}
	file.close();
	qDebug() << "End of Parsing!";
}

/*QString Data::processFile(QString input){
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
	}*/


