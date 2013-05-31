/*
 * LoadData.cpp
 *
 *  Created on: 2013-04-03
 *      Author: Yasir
 */

#include "LoadData.hpp"
#include <bb/device/DisplayInfo>
using namespace bb::device;

QString station, direction;
QStringList stations;
int timeHour, timeMinute;
QDomElement root;
bool failFlag = false;
QList<QStringList> buses;
int stationsToAddToScreen;
/*
 * 	LoadData class constuctor
 * 	Inputs: parent, QString fileInput
 * 	Constructor takes in a route number as fileInput, such as fileInput = 10
 *	and looks for a file with name route10.xml. If it's not found, it lets caller
 *	know to create it, and if it is found it loads up specified station and time info.
 */
LoadData::LoadData(QObject* parent, QString fileInput) {
	buses.clear();
	stations.clear();
	failFlag = false; //Fail flag set to true if file is not found
	QString fileName = "route" + fileInput + ".xml";
	//Open file in specified folder with inputted file name
	QString appFolder(QDir::currentPath());
	QString fileExtension = appFolder + "/app/native/assets/schedule/"
			+ fileName;
	QFile *file = new QFile(fileExtension);

	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		////qDebug() << "Failed to open " + fileName;
		//Now download file...
		failFlag = true;
		return;
	}
	qDebug() << "Start load data: " + fileName;
	QDomDocument doc("route"); //Get the root node, route
	QString bl = "h";
	QString *errorMsg = &bl;
	//If the xml file is corrupted & doesn't have root node "route", then redownload file.
	if (!doc.setContent(file, true, errorMsg)) {
		////qDebug() << "I failed to open " + fileName;
		file->close();
		//Delete file, redownload
		file->remove();
		failFlag = true;
		////qDebug() << *errorMsg;
		return;
	}
	file->close();
	root = doc.documentElement(); //Setup root node

}
LoadData::~LoadData() {
	// TODO Auto-generated destructor stub
}

/*
 * 	initializeBusArrays method
 * 	Inputs: none
 * 	Outputs: none
 *	Sets values for all stations and bus arrays to "N.A". This way, when adding times
 *	and a time is not found for a value, it is already set to "N.A" when going to display
 */
void LoadData::initializeBusArrays() {
	//Initialize all lists to N.A so you dont have to do it at the end
	stations.clear();
	buses.clear();
	for (int c = 0; c < 3; c++) {
		QStringList temp;
		for (int d = 0; d < stationsToAddToScreen; d++) {
			temp.append("N.A");
			if (c == 0)
				stations.append("N.A");
		}
		buses.append(temp);
		temp.clear();
	}
}

bool LoadData::getFailFlag() {
	return failFlag;
}

QList<QStringList> LoadData::getBuses() {
	return buses;
}

QStringList LoadData::getStations() {
	return stations;
}

void LoadData::setDirection(QString d) {
	direction = d;
}

void LoadData::setStation(QString s) {
	station = s;
}

void LoadData::setHour(int h) {
	timeHour = h;
}

void LoadData::setMinute(int m) {
	timeMinute = m;
}
/*
 * 	parseXML method
 * 	Inputs: none
 *	Outputs: none
 * 	Using root (which is already initialized to the file in constructor), gets the children
 *	to find the correct station at which the text box input equals station name in XML.
 *	Runs methods to find correct times according to current time
 */
void LoadData::parseXML() {

	//Get first child and check its direction with what user entered
	QDomNode n = root.firstChild();
	QDomElement e = n.toElement();
	QDomAttr a = e.attributeNode("number");
	//If direction isnt same as input, go to next sibling
	if (a.value().compare(direction) != 0) {
		n = n.nextSibling();
		e = n.toElement();
		a = e.attributeNode("number");
	}
	//Now check if direction is same after moving to next sibling
	if (a.value().compare(direction) == 0) {
		n = n.firstChild();
		e = n.toElement();
		a = e.attributeNode("number");

		//add check for day here, for now there will always be no siblings for day
		//and hence the app only supports day = 1 (Mon. - Fri.)
		stationsToAddToScreen = n.childNodes().length();//Number of stations for this route & direction
		//Get the first node for child of day (which are the station names)
		//qDebug() << "e.nodeName: " + e.nodeName() + " a.value: " + a.value();
		n = n.firstChild();
		e = n.toElement();
		a = e.attributeNode("name");
		QString temp = checkSpecialCases(a.value());
		int nodeNumber = 1;
		//Go through all the station node siblings until the correct station is found
		while ((QString::compare(temp, station, Qt::CaseInsensitive) != 0)
				&& !n.isNull()) {
			//qDebug() << "Comparing " + a.value() + " or " + temp + " against " + station + " = " + QString::number(QString::compare(temp, station, Qt::CaseInsensitive));
			n = n.nextSibling();
			e = n.toElement();
			a = e.attributeNode("name");
			temp = checkSpecialCases(a.value());
			nodeNumber++;
		}

		//If we went through all of them, there is an error if it is still not found; return
		if (QString::compare(temp, station, Qt::CaseInsensitive) != 0)
			return;
		stationsToAddToScreen = stationsToAddToScreen  - nodeNumber + 1;//Number of stations for this route & direction
		initializeBusArrays(); //Initialize all arrays to "N.A"
		QDomNode m = n;
		//Access the children of the correct station node (times at that station)
		n = n.firstChild();
		addBusTimes(n); //Run method to add first row of times

		int i;
		for (i = 1; !m.isNull(); i++) { //Get times for next 4 (rows) of stations
			QString name = m.toElement().attributeNode("name").value();
			qDebug() << "Adding row: " << name;
			stations[i - 1] = name; //Set name of station in station array to be displayed
			m = m.nextSibling();
			if (!m.isNull())
				addSubsequentBusTimes(m.firstChild(), i); //Add ith row of station
			else
				break;
		}
		if (!m.isNull())
			stations[i - 1] = m.toElement().attributeNode("name").value();
	}

}

/*
 * 	addBusTimes method
 * 	Inputs: QDomNode n
 *	Outputs: none
 * 	Takes input of first child in a station (which equals the first time for that child).
 *	Looks through all children for this parent node and find which time and hour values
 *	are after the current time and hour. Adds the next of these to bus time list
 */
void LoadData::addBusTimes(QDomNode n) {
	QStringList k;
	bool ok;
	int j = 0;
	int r, q;
	QDomElement e;
	do {
		k.clear();
		e = n.toElement(); //Get the current node (time) and split by "."
		k = e.text().split(".");
		//qDebug() << e.text();
		r = k[0].toInt(&ok, 10); //Hour
		q = k[1].toInt(&ok, 10); //Minute
		if (r <= 3) //Special case when time is 00, 01, 02 or 03
			r = r + 24;
		//Add into array if its time is later than the current time
		if (r > timeHour || (r == timeHour && q >= timeMinute)) {
			//buses.at(j)[0] = e.text();
			QStringList temp = buses.at(j);
			temp[0] = e.text();
			buses.replace(j, temp);
			//qDebug() << " Added "  << temp[0];
			j++;
		}
		n = n.nextSibling(); //Go to the next sibling in nodes
		//Stop running loop if you get to null node or you've added 5 buses
		if (j >= 3 || n.isNull())
			break;
	} while (true);
}

/*
 * 	addSubsequentBusTimes method
 * 	Inputs: QDomNode n, int whichStationToAdd
 *	Same as addBusTimes, but for other stations after 1
 *	TODO: combine these two functions.
 *	Starting from the first child (n) in a station node passed in, checks the
 *	"whichStationToAdd"th station to add the row of times for that station for all 5
 *	all 5 bus times. Assumes that first row was already added.
 */
void LoadData::addSubsequentBusTimes(QDomNode n, int whichStationToAdd) {
	bool ok;
	QDomElement e;
	QStringList values;
	int r = 2, q = 0, index = 1;
	while (!n.isNull() && index < 4) {
		r = 2;
		values.clear();
		//Get the previous/last time (previous row) stored for this current station
		//If last station was N.A (no stop), then check further back in list.
		QStringList temp = buses.at(index - 1);
		values = temp[whichStationToAdd - 1].split(".");

		while ((values[0].compare("N") == 0) && r <= whichStationToAdd) {
			values = temp[whichStationToAdd - r].split(".");
			r++;
		}
		if (values[0].compare("N") == 0) {
			index++;
			QDomNode bb = n.parentNode();
			n = bb.firstChild();
			qDebug() << "Reset N";
			continue;
		}

		//qDebug() << " oldHour " + values[0] + " oldMinute " + values[1] + " num " + values[2];
		//Get integer values out of the previous hour and minute as well as store previous bus number
		int oldHour = values[0].toInt(&ok, 10);
		int oldMinute = values[1].toInt(&ok, 10);
		QString oldBusNumber = values[2];

		//Get time and bus number info for current element in parent station node
		values.clear();
		e = n.toElement();
		values = e.text().split(".");
		r = values[0].toInt(&ok, 10);
		q = values[1].toInt(&ok, 10);
		if (r <= 3)
			r = r + 24;
		//If the value for current time is more than last time...
		if (r > oldHour || (r == oldHour && q > oldMinute)) {
			//qDebug() << " r " + values[0] + " q " + values[1] + " num " + values[2] + " index = " + QString::number(index);
			//Store the value in appropriate bus array
			if (values[2].compare(oldBusNumber) == 0) {
				//qDebug() << "added: " + e.text() + " After comparing " + values[2] + " And " + oldBusNumber;
				QStringList temp = buses.at(index - 1);
				temp[whichStationToAdd] = e.text();
				buses.replace(index - 1, temp);
				index++;
			}
		}
		//Only add 5 maximum buses
		if (index > 3) {
			//qDebug("Done adding!");
			break;
		}

		//Move to next sibling only if next sibling is not null
		QDomNode m = n.nextSibling();
		if (!m.isNull())
			n = n.nextSibling();
		//If next sibling is null then move onto next bus number (index) and move back to first child of station node
		else if (m.isNull() && index != 3) {
			index++;
			QDomNode bb = n.parentNode();
			n = bb.firstChild();
			//qDebug() << "Reset n";
		} else
			n = n.nextSibling();
	}
}

QString LoadData::checkSpecialCases(QString name) {
	if (name.compare("Erin Mills&Eglinton") == 0)
		return "Mississauga Erin Mills&Eglinton";
	else if (name.compare("St.Catharines Fairview M.") == 0)
		return "St.Catharines Fairview Mall";
	else if (name.compare("Grimsby ") == 0)
		return "Grimsby Carpool Lot";
	else if (name.compare("Stoney Creek") == 0)
		return "Stoney Creek Nash Rd & Barton St.";
	else if (name.compare("Mdwvale Derry & Winston") == 0)
		return "Meadowvale Derry & Winston Ch.";
	else if (name.compare("Mdwvale Erin Mills&Miss.") == 0)
		return "Meadowvale Erin Mills & Miss.";
	else if (name.compare("Meadowvale  Business Park") == 0)
		return "Meadowvale Business Park Syntex & Financial";
	else if (name.compare("Rockwood") == 0)
		return "Rockwood Alma St. & Main St.";
	else if (name.compare("Brampton Hwy10 & Bovaird") == 0)
		return "Brampton Hurontario & Bovaird";
	else if (name.compare("Bronte   Hwy407/Bronte Rd") == 0)
		return "Bronte Carpool Lot Hwy407&Bronte Rd";
	else if (name.compare("TrinityCommon Hwy410&Bova") == 0)
		return "Trinity Common Hwy. 410 & Bovaird";
	else if (name.compare("Hwy 27 & Rexdale Blvd.") == 0)
		return "Etobicoke Hwy 27 & Rexdale";
	else if (name.compare("Dixon @ Martin Grove") == 0)
		return "Etobicoke Dixon @ Martin Grove";
	else if (name.compare("Broadway & Blind Line") == 0)
		return "Orangeville Broadway & Blind Line";
	else if (name.compare("Broadway & Hwy 10") == 0)
		return "Orangeville Broadway & Hwy 10";
	else if (name.compare("Snelgrove at Mayfield") == 0)
		return "Snelgrove Hurontario & Mayfield Rd.";
	else if (name.compare("Nobleton at Hwy 27") == 0)
		return "Nobleton King Rd @ Hwy. 27";
	else if (name.compare("Woodbridge Rutherford") == 0)
		return "Woodbrige Rutherford & Vaughan Mills";
	else if (name.compare("Woodbridge (7 & Hwy 50)") == 0)
		return "Woodbridge (Hwy 50 & 7)";
	else if (name.compare("Humber College Blvd&Hwy27") == 0)
		return "Toronto Hwy. 27 @ Humber College Blvd";
	else if (name.compare("Hurontario and 407") == 0)
		return "Mississauga Hurontario & 407";
	else if (name.compare("Centennial Progress Camp.") == 0)
		return "Centennial College Progress Campus";
	else if (name.compare("U of T @ Scarborough") == 0)
		return "University Of Toronto @ Scarborough";
	else if (name.compare("Hwy 407 at Brock Road") == 0)
		return "Brougham Hwy 407 @ Brock Road";
	else if (name.compare("Brooklin") == 0)
		return "Brooklin Winchester @ Anderson";
	else if (name.compare("Aurora Rd. & Hwy. 404") == 0)
		return "Aurora Carpool Lot Aurora Rd. & Hwy 404";
	else if (name.compare("Newmarket 9&400 MTO") == 0)
		return "Newmarket Hwy. 9 & Hwy. 400";
	else if (name.compare("Major Mack & 400") == 0)
		return "Maple Major Mackenzie & Hwy. 400";
	else if (name.compare("Port Bolster Lake R./Hw48") == 0)
		return "Port Bolster Lake Ridge Rd. @ Hwy. 48";
	else if (name.compare("Whitby Brock @ Dundas") == 0)
		return "Whitby Dundas @ Brock";
	else if (name.compare("Port Union & Kingston") == 0)
		return "North Rouge Kingston Rd. & Port Union";
	else
		return name;
}
