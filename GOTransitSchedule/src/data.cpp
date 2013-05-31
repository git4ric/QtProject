/*
 * data.cpp
 *
 *  Created on: 2013-03-03
 *      Author: user
 */

#include "data.hpp"
#include <qt4/QtCore/QString>
#include <qt4/QtCore/QStringList>
#include <qt4/QtCore/QTimer>
#include <qt4/QtCore/QFile>
 #include <QDir>
#include <qt4/QtTest/QTest>
#include <qt4/QtCore/QRegExp>
#include <bb/data/XmlDataAccess>

//using namespace bb::cascades;
using namespace bb::data;
QDomElement day;
QString URLName;
QDomElement route;
QString routeNum;

Data::Data(QObject* parent, QString routeNumber){
	_manager = new QNetworkAccessManager(this);
	routeNum = routeNumber;
	// create the root tag:  <Route number="20" >
	route = _doc.createElement("route");
	route.setAttribute("number",routeNumber);
	_doc.appendChild(route);

	// add child element:  <direction="0">
	QDomElement direction = _doc.createElement("direction");
	direction.setAttribute("number",0);
	route.appendChild(direction);


	// add child element:  <day number="1">
	day = _doc.createElement("day");
	day.setAttribute("number",1);
	direction.appendChild(day);

	QObject::connect(_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));
	_baseUrl = "http://www.gotransit.com/publicroot/en/schedules/pubsched.aspx?table="+routeNumber+"&direction=0&day=1&page=1";
	_stopFetchFlag = false;
	_doneSaving = false;
	_direction = false;
	URLName = _baseUrl;
	qDebug() << "Last thing for today";
}

Data::~Data(){

}

void Data::fetchData(){//Updates URL

	QRegExp* pattern = new QRegExp("\\S*(page=[0-9]+)");
	int pos = pattern->indexIn(_baseUrl);
	QStringList list = pattern->capturedTexts();
	QString pageNo = list[1];

	QString number = pageNo.mid(5);
	int intNumb = number.toInt();

	QString newNumb = QString::number(intNumb + 1);

	QString str("page=");
	QString newPageNo = str.append(newNumb);

	_baseUrl.replace(QRegExp("page=[0-9]+"),newPageNo);

	qDebug() << _baseUrl;

}

void Data::update(){
	_manager->get(QNetworkRequest(QUrl(_baseUrl)));
	fetchData();
}

void Data::replyFinished(QNetworkReply* reply){
	const QByteArray arr = reply->readAll();
	QString appFolder(QDir::currentPath());
	QString fileName = appFolder + "/app/native/assets/schedule/settings.txt";
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		qDebug() << "Could not open file for writing html";
		return;
	}
	file.write(arr);
	file.close();

	QString check(arr);
	if(check.contains("<td title=\"Trip Departure Time or Transfer required\" id=\"Y\">")){
		processFile();
	}
	else if ( _direction == false){
		_direction = true;
		URLName.replace("direction=0", "direction=1");
		// add child element:  <direction="0">

		QDomElement direction = _doc.createElement("direction");
		direction.setAttribute("number",1);
		route.appendChild(direction);


		// add child element:  <day number="1">
		day = _doc.createElement("day");
		day.setAttribute("number",1);
		direction.appendChild(day);

		_baseUrl = URLName;
	}
	else{
		_stopFetchFlag = true;
		saveXMLFile();
	}
}

void Data::saveXMLFile(){
	const int indent = 4;
	QString appFolder(QDir::currentPath());
	QString fileName = appFolder + "/app/native/assets/schedule/route"+routeNum+".xml";
	QFile outputXmlFile(fileName);
		if (!outputXmlFile.open(QIODevice::WriteOnly | QIODevice::Text)){


		qDebug() << "Could not open file for writing XML";

		return;

		}
		QTextStream outputXml(&outputXmlFile);
		_doc.save(outputXml,indent);
		outputXmlFile.close();
		_doneSaving = true;
		qDebug() << "End of Xml output!";

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
	//TODO: Check all alphabet values using ASCII?
	if (input.mid(lineStart + 6, 1).compare("s") != 0 && input.mid(lineStart + 6, 1).compare("n") != 0
		&& input.mid(lineStart + 6, 1).compare("k") != 0 && input.mid(lineStart + 6, 1).compare("h") != 0
		&& input.mid(lineStart + 6, 1).compare("D") != 0 && input.mid(lineStart + 6, 1).compare("d") != 0){
	time.first = input.mid(lineStart - 2, 2); //Hour of this bus
	time.second = input.mid(lineStart+ 6, 2); //Minute of this bus
	}
	else {
		time.first = input.mid(lineStart - 4, 2);
		time.second = input.mid(lineStart - 2, 2);
	}
	return time;
}

void Data::processFile(){
	QString appFolder(QDir::currentPath());
	QString fileName;
//	if (_direction == false)
//	fileName = appFolder + "/app/native/assets/schedule/settings.txt";
//	else
		fileName = appFolder + "/app/native/assets/schedule/settings.txt";
	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qDebug() << "Could not open file for parsing";
		return;
	}

//===== Start XML creation logic ========================






	//====== Start Parsing Logic============================
	QStringList stationName;
	QStringList times;
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
	 QStringList busNumbers;
	 //std::vector <QDomElement> station;
	 QDomElement station;

	 int k = 0;
	while (!file.atEnd()) {
		 QByteArray line = file.readLine();
		 if (line.contains("id=\"E\""))
		 	 busNumbers.append(line.mid(line.indexOf("\"E\">") + 4, line.indexOf("</td>") - (line.indexOf("\"E\">") + 4)));
		 else if (line.contains("id=\"bbb\""))
			 busNumbers.append("Train");
		 QDomElement timeTag;
		 if (line.contains("/publicroot/en/travelling/stations.aspx?station=")){
			 QString name = processStationName(line);
			 name.replace("-", " ");
			 //Special case is for station names that are fucked up in HTML code.
			// name = checkSpecialCases(name);
			 stationName.append(name);
			 //filter = true;
			 qDebug() << stationName[i];
			 times.append(stationName[i]);
			 k = 0;
			 // Every time a new station is encountered, create a new tag eg: <station name="Milton GO">
			 station = _doc.createElement("station");
			 station.setAttribute("name",stationName[i]);
			 QString temp;
			 for(QDomElement n = day.firstChildElement("station"); !n.isNull(); n = n.nextSiblingElement("station"))
			 {
			 	if(stationName[i].compare(n.attribute("name",QString(""))) == 0){
			 		station = n;
			 		break;
			 	}
			 }
			 day.appendChild(station);

			 i++;
		 }
		 else if(line.contains("Transfer Node")){
			 qDebug() << "Transfer required after station: " << stationName[i-1];
		 }
		 else if (line.contains("id=\"G\"") or line.contains("id=\"Y\"") or line.contains("id=\"y\"")){
			 time = processTime(line);
			 time.second = time.second + "." + busNumbers[k];
			 k++;
			 qDebug() << "time is: " << time.first << "." << time.second;
			 times.append(time.first + "." + time.second);

			 // Every time a new time is encountered, create a new tag eg: <time>17.35</time>
			 timeTag = _doc.createElement("time");
			 QString timeDot = time.first.append(".");
			 QString fullTime = timeDot.append(time.second);

			 QDomText timeText = _doc.createTextNode(fullTime);
			 timeTag.appendChild(timeText);

			 station.appendChild(timeTag);
		 }
		 else if (line.contains("id=\"bbb\"") or line.contains("id=\"YY\""))
			 k++;
	}
	file.close();
	qDebug() << "End of Parsing!";

	//===== End of Parsing logic===========
}


