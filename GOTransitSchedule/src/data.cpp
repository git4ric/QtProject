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
#include <qt4/QtCore/QTimer>

Data::Data(QObject* parent){
	_manager = new QNetworkAccessManager(this);
	_stopFetchFlag = false;
	QObject::connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
	_baseUrl = "http://www.gotransit.com/publicroot/en/schedules/pubsched.aspx?table=20&direction=0&day=1&page=1&new=";
}

Data::~Data(){

}

void Data::fetchData(){
	int temp = _baseUrl.indexOf("page=1",0);
	qDebug() << temp;
	while(!_stopFetchFlag){
		QTimer::singleShot(4000,this,SLOT(sendRequest(_baseUrl)));
	}
}

void Data::sendRequest(QString str){
	_manager->get(QNetworkRequest(QUrl(str)));
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

	if(arr.contains("Trip Departure Time or Transfer required") and !_stopFetchFlag){
		processFile();
	}
	else{
		_stopFetchFlag = true;
	}

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
