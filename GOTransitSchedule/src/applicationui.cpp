/*
  TODO: Custom icons for tabbed pane icons (GO and Presto)
  TODO: Add coming soon screen for PRESTO tab
  TODO: Input from textbox should go to function call (hard coded Bramalea for now till names are fixed in code) (DONE!)
            Make sure all station names in file name are same as HTML names. (DONE!)
  TODO: Fix direction issue (some directions are reversed in app as opposed to documents) (DONE!)
  TODO: Find a better dialog box...
  TODO: Combine 5   QStringList's for bus1,bus2,etc. into 1 QList<QStringList> (DONE!)
  TODO: When you select a station, then a route, then go back to edit station text box, it crashes... No idea why (DONE!)
  TODO: Add times for next stations in bus time lists and add station names in output page (DONE!)
  TODO: Make text input suggestions searchable by .contains as well as .compare/.equals (DONE!)
  TODO: Fix heights and widths of display text areas (DONE!)
*/
//quick todo, logo and download all scheds... test
// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/ActionBarPlacement>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Stacklayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Button>
#include <bb/cascades/NavigationPaneProperties>
#include <qt4/QtCore/QTime>
#include <qt4/QtCore/QStringList>
#include <bb/cascades/TextStyle>
#include <bb/cascades/FontWeight>
#include <bb/cascades/FontSize>
#include <bb/device/DisplayInfo>
#include <qt4/QtCore/QFile>
#include <bb/cascades/Dialog>
#include <bb/cascades/TextArea>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Dialog>
#include <bb/cascades/ScrollMode>
#include <bps/virtualkeyboard.h>
#include <bb/cascades/Tab>
#include <bb/cascades/ProgressIndicator>

using namespace bb::cascades;
using namespace bb::device;

QString routeFile;
int stationsToShow, indexOfCurrentStationShowing;
bb::cascades::NavigationPane* navigationPane;
Button* fetchData;
Dialog* pMyDialog;
Label *lab,  *loadMoreButton, *redDisplay;
ProgressIndicator *pI;
QList<bb::cascades::TextArea*> listOfTextAreas;
Label *moreStationsButton ;
LoadData *localFile;
OrientationSupport *thisOrientation;
int deviceWidth, deviceHeight;
float progressIndicatorValue = 0.0;
    bb::cascades::TextField *suggestion1, *suggestion2, *suggestion3, *suggestion4, *textInput;
    bb::cascades::ImageView *logo;
DropDown *routeDropDown, *directionDropDown;
bool Q10 = false;
Container* contentContainer;

void ApplicationUI::getDeviceInformation(){
	OrientationSupport *support = OrientationSupport::instance();

	DisplayInfo display;
	    deviceWidth = display.pixelSize().width() ;
	    deviceHeight = display.pixelSize().height() ;
	    if (deviceHeight > 1000){
	        Q10 = false;
	        stationsToShow = 5; //How many stations (rows) we show at a time
	    }
	    else{
	        Q10 = true;
	        stationsToShow = 4;
	    }
	    if (support->orientation() == UIOrientation::Landscape){
	    	deviceHeight = display.pixelSize().width() ;
	    	deviceWidth = display.pixelSize().height() ;
	    }
}

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{
	getDeviceInformation();
    _manager = new bb::system::InvokeManager(this);
        _saveData = false;
        navigationPane = NavigationPane::create();
        QObject::connect(navigationPane, SIGNAL(popTransitionEnded(bb::cascades::Page*)), this, SLOT(popFinished(bb:cascades::Page*)));
        // Create the root container
        contentContainer = Container::create().top(100);
        StackLayout *pStackLayout = new StackLayout();
        pStackLayout->setOrientation( LayoutOrientation::TopToBottom );
        contentContainer->setLayout(pStackLayout);


        TextStyle *whiteStyle = new TextStyle();
            whiteStyle->setColor(Color::White);

        //dropDownFrom = DropDown::create().title("Starting from...");
        textInput = new TextField();
        textInput->setHintText("Starting from (Start typing station name)...");

        suggestion1 = new TextField();
        suggestion1->setVisible(false);
        suggestion1->setHintText("");

        suggestion2 = new TextField();
        suggestion2->setVisible(false);
        suggestion2->setHintText("");

        suggestion3 = new TextField();
        suggestion3->setVisible(false);
        suggestion3->setHintText("");

        suggestion4 = new TextField();
        suggestion4->setVisible(false);
        suggestion4->setHintText("");

        QString appFolder(QDir::currentPath());
        QString fileName = appFolder + "/app/native/assets/schedule/stations.txt";
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qDebug() << "Could not open file for parsing";
            return;
        }

         while (!file.atEnd()) {
            QString line = file.readLine();
            QStringList firstElement = line.split(",");
            stationName.append(firstElement[0]);
        }
        file.close();

        lab = new Label();
        lab->setText("This application or its creators are not\nassociated with GO Transit or Metrolinx.");
        if (Q10)
            lab->textStyle()->setBase(*whiteStyle);
        lab->setHorizontalAlignment(HorizontalAlignment::Center);
        lab->setMultiline(true);

        routeDropDown = DropDown::create().title("Select a Route (Choose a station first)");
        routeDropDown->setHorizontalAlignment(HorizontalAlignment::Center);
        routeDropDown->setEnabled(false);
        routeDropDown->setFocusPolicy(bb::cascades::FocusPolicy::Default);
        logo = new ImageView();
        OrientationSupport *support = OrientationSupport::instance();
        logo->setHorizontalAlignment(HorizontalAlignment::Center);

            if (support->orientation() == UIOrientation::Portrait){
                        logo->setHorizontalAlignment(HorizontalAlignment::Center);
                    routeDropDown->setPreferredWidth(690);
                    if (!Q10){
                        logo->setImage("asset:///images/logo-portrait.png");
                        lab->setTopMargin(160);
                        lab->setText("This application or its creators are not\nassociated with GO Transit or Metrolinx.");
                        contentContainer->setTopPadding(100);
                    }else{
                        qDebug() << "Q10 so set landscape image";
                        logo->setImage("asset:///images/logo-landscape.png");
                        contentContainer->setTopPadding(0);
                        lab->setTopMargin(0);
                        lab->setText("This application or its creators are notassociated with GO Transit or Metrolinx.");
                    }
            }
            else {
                logo->setImage("asset:///images/logo-landscape.png");
                if (!Q10)
                    routeDropDown->setPreferredWidth(1200);
                else
                    routeDropDown->setPreferredWidth(690);
                lab->setTopMargin(0);
                contentContainer->setTopPadding(0);
                lab->setText("This application or its creators are not associated with GO Transit or Metrolinx.");
            }
        directionDropDown = DropDown::create().title("Select direction of travel");
        directionDropDown->setHorizontalAlignment(HorizontalAlignment::Center);
        directionDropDown->setEnabled(false);
        if (support->orientation() == UIOrientation::Portrait)
            directionDropDown->setPreferredWidth(690);
        else {
            if (!Q10)
                directionDropDown->setPreferredWidth(1200);
            else
                directionDropDown->setPreferredWidth(690);
        }
        fetchData = Button::create().text("Find Schedule");
        fetchData->setEnabled(false);
        fetchData->setHorizontalAlignment(HorizontalAlignment::Center);

        contentContainer->add(logo);
        contentContainer->add(textInput);
        contentContainer->add(suggestion1);
        contentContainer->add(suggestion2);
        contentContainer->add(suggestion3);
        contentContainer->add(suggestion4);
        contentContainer->add(routeDropDown);
        contentContainer->add(directionDropDown);
        contentContainer->add(fetchData);
        contentContainer->add(lab);
        bool res = QObject::connect(fetchData, SIGNAL(clicked()), this,SLOT(onClicked()));
                Q_ASSERT(res);

        res = QObject::connect(textInput, SIGNAL(textChanging(QString)), this,
                                            SLOT(showSuggestions(QString)));
        Q_ASSERT(res);
        res = QObject::connect(textInput, SIGNAL(focusedChanged(bool)), this,
                                                    SLOT(removeUI(bool)));
        Q_ASSERT(res);
        res = QObject::connect(suggestion1, SIGNAL(focusedChanged(bool)), this,
                                                    SLOT(suggestionOneHighlight()));
        Q_ASSERT(res);
        res = QObject::connect(suggestion2, SIGNAL(focusedChanged(bool)), this,
                                                    SLOT(suggestionTwoHighlight()));
        Q_ASSERT(res);
        res = QObject::connect(suggestion3, SIGNAL(focusedChanged(bool)), this,
                                                    SLOT(suggestionThreeHighlight()));
        Q_ASSERT(res);
        res = QObject::connect(suggestion4, SIGNAL(focusedChanged(bool)), this,
                                                    SLOT(suggestionFourHighlight()));

        res = QObject::connect(routeDropDown, SIGNAL(selectedIndexChanged(int)), this,
                                                    SLOT(addDirection(int)));

        res = QObject::connect(directionDropDown, SIGNAL(selectedIndexChanged(int)), this,
                                                            SLOT(enableButton()));
        Q_ASSERT(res);
        // Indicate that the variable res isn't used in the rest of the app, to prevent
        // a compiler warning
        Q_UNUSED(res);

        TabbedPane* tabbedPane = TabbedPane::create() .showTabsOnActionBar(true);
        Tab* tab = new Tab();
        tab->setTitle("GO Transit");
        tab->setDescription("Get Bus route timings from any station for GO Transit!");

        //Tab* tab2 = new Tab();
        //tab2->setTitle("PRESTO Card");
        //tab2->setDescription("Get balance and other info about your PRESTO Card!");

        Tab* tab3 = new Tab();
        tab3->setTitle("Disclaimer");
        tab3->setDescription("App settings and specifications");

        page = new Page();
        if (Q10)
                    contentContainer->setBackground(Color::Black);
        else
                    contentContainer->setBackground(Color::White);

        page->setContent(contentContainer);
        navigationPane->push(page);
        tab->setContent(navigationPane);
        tabbedPane->add(tab);
        //tabbedPane->add(tab2);
        createSettingsTab();
        tab3->setContent(page2);

        tabbedPane->add(tab3);

        res = QObject::connect(OrientationSupport::instance(), SIGNAL(displayDirectionAboutToChange(bb::cascades::DisplayDirection::Type,
                        bb::cascades::UIOrientation::Type)), this, SLOT(onDisplayDirectionAboutToChange()));

        timer = new QTimer(this);
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));

        app->setScene(tabbedPane);

}
void ApplicationUI::onDisplayDirectionAboutToChange(){
    OrientationSupport *support = OrientationSupport::instance();
    if (support->orientation() == UIOrientation::Portrait) {//Changing to landscape

        logo->setImage("asset:///images/logo-landscape.png");
        if (!Q10){
            routeDropDown->setPreferredWidth(1200);
            directionDropDown->setPreferredWidth(1200);
        }else{
            routeDropDown->setPreferredWidth(690);
            directionDropDown->setPreferredWidth(690);
        }
        lab->setTopMargin(0);
        contentContainer->setTopPadding(0);
        lab->setText("This application or its creators are not associated with GO Transit or Metrolinx.");
    }
    else{ //Changing to portrait
        routeDropDown->setPreferredWidth(690);
        directionDropDown->setPreferredWidth(690);
        if (!Q10){
            logo->setImage("asset:///images/logo-portrait.png");
            contentContainer->setTopPadding(100);
            lab->setTopMargin(160);
            lab->setText("This application or its creators are not\nassociated with GO Transit or Metrolinx.");
        }else{
            logo->setImage("asset:///images/logo-landscape.png");
            contentContainer->setTopPadding(0);
            lab->setTopMargin(0);
            lab->setText("This application or its creators are not associated with GO Transit or Metrolinx.");
        }
    }
}

void ApplicationUI::createSettingsTab(){
    page2 = new Page();
    Container* contentContainer = Container::create().top(65);
    TextStyle *whiteStyle = new TextStyle();
                whiteStyle->setColor(Color::White);

    StackLayout *pStackLayout = new StackLayout();
    pStackLayout->setOrientation( LayoutOrientation::TopToBottom );
    contentContainer->setLayout(pStackLayout);
    if (Q10)
        contentContainer->setBackground(Color::Black);
    else
        contentContainer->setBackground(Color::White);
    Button* pButton1 = Button::create().text("Open GO Transit schedule website");
    TextArea *textArea = new TextArea();
    TextArea *textArea2 = new TextArea();
    textArea2->setText("DISCLAIMER:");
    textArea->setText("This application, its creators, or contributors are NOT in any way associated with GO Transit, MetroLinx, or any of its partners. Everything used in this application has been used with permission from its owners and its copyright holders. "
            "The creators of this application take no responsibility for incorrect, inaccurate or missing information in regards to schedules. This app is limited at the moment to schedules ONLY ON WEEKDAYS. The app does not show special trips such as Wonderland.");
    if (Q10){
    textArea->textStyle()->setBase(*whiteStyle);
    textArea2->textStyle()->setBase(*whiteStyle);
    }
    textArea->setEditable(false);
    textArea2->setEditable(false);
    pButton1->setHorizontalAlignment(HorizontalAlignment::Center);
    textArea->setHorizontalAlignment(HorizontalAlignment::Center);
    textArea2->setHorizontalAlignment(HorizontalAlignment::Center);
    contentContainer->add( pButton1 );
    contentContainer->add( textArea2 );
    contentContainer->add( textArea );
    QObject::connect(pButton1,SIGNAL(clicked()),this,SLOT(onOpenWebsiteClicked()));
    page2->setContent(contentContainer);

}
void ApplicationUI::onOpenWebsiteClicked(){
    bb::system::InvokeRequest requestWebsite;
    requestWebsite.setTarget("sys.browser");
    requestWebsite.setUri(QUrl("http://www.gotransit.com/publicroot/en/schedules/findsch.aspx?station=&frMap=Y&new="));
    requestWebsite.setAction("bb.action.OPEN");

    _reply = _manager->invoke(requestWebsite);
    QObject::connect(_reply,SIGNAL(finished()),this,SLOT(onReplyFinished()));
}

void ApplicationUI::onReplyFinished(){
    if(_reply->isFinished()){
        qDebug() << "Reply finished. Things look fine";
        bb::system::InvokeReplyError::Type error = _reply->error();
        if( error == bb::system::InvokeReplyError::None){
            qDebug() << "Things look awesome!";
        }
        else if( error == bb::system::InvokeReplyError::BadRequest){
            qDebug() << "Bad Request";
        }
        else if( error == bb::system::InvokeReplyError::NoTarget){
            qDebug() << "No Target";
        }
        else if( error == bb::system::InvokeReplyError::InsufficientPrivileges){
            qDebug() << "Insufficient priviledges";
        }
        else{
            qDebug() << "Oh fuck me";
        }
    }
    else{
        qDebug() << "Something bad happened";
    }
}
void ApplicationUI::enableButton(){
    if (routeDropDown->selectedIndex() != routeDropDown->SelectedIndexNone
        && directionDropDown->selectedIndex() != directionDropDown->SelectedIndexNone
        && textInput->text().compare("") != 0)
    fetchData->setEnabled(true);
}
void ApplicationUI::onClicked(){
    QObject::disconnect(thisOrientation, SIGNAL(displayDirectionAboutToChange(bb::cascades::DisplayDirection::Type,
                       bb::cascades::UIOrientation::Type)), this, SLOT(scheduleUIDirectionChanging()));
    if (routeDropDown->selectedIndex() == routeDropDown->SelectedIndexNone
             || directionDropDown->selectedIndex() == directionDropDown->SelectedIndexNone
            || textInput->text().compare("") == 0)
        return;

    TextStyle *blueStyle = new TextStyle();
    blueStyle->setFontWeight(FontWeight::Bold);
    blueStyle->setFontSizeValue(FontSize::XXLarge);
    blueStyle->setColor(Color::Blue);

    if (_saveData){//If new schedule needs to fetched/taken from site
    data = new Data(this, routeFile);
    pMyDialog = new Dialog();
        Container *pC = new Container();
        DockLayout *pD = new DockLayout();
        pC->setLayout(pD);
        Label *pL = new Label();
        pL->textStyle()->setBase(*blueStyle);
        pL->setText("DOWNLOADING SCHEDULE... PLEASE WAIT...");
        pL->setHorizontalAlignment(HorizontalAlignment::Center);
        pL->setVerticalAlignment(VerticalAlignment::Center);
        pI = new ProgressIndicator();
        pI->setFromValue(0.0);
        pI->setToValue(100.0);
        progressIndicatorValue = 0.0;
        pI->setValue(progressIndicatorValue);
        pI->setHorizontalAlignment(HorizontalAlignment::Center);
        pI->setVerticalAlignment(VerticalAlignment::Bottom);
        pC->add(pL);
        pC->add(pI);
        pMyDialog->setContent(pC);
        pMyDialog->open();
    timer->start(1750);
    //data->fetchData();
    }
    else//if local XML file needs to be parsed and shown...
    {

        QString r = routeDropDown->at(routeDropDown->selectedIndex())->text();

        setRouteNumber(r);
        int d = directionDropDown->selectedIndex();
        localFile = new LoadData(this, routeFile);
        if (localFile->getFailFlag() == true){
            _saveData = true;
            onClicked();
            return;
        }
        if (d == 0)
            localFile->setDirection("0");
        else
            localFile->setDirection("1");

        localFile->setStation(textInput->text());
        localFile->setHour(QTime::currentTime().hour());
        localFile->setMinute(QTime::currentTime().minute());
//        localFile->setHour(QTime::currentTime().hour());
//        localFile->setMinute(QTime::currentTime().minute());
        localFile->parseXML();
        setUpScheduleUI();

    }
}
void ApplicationUI::setUpScheduleUI(){
	getDeviceInformation(); //Update what orientation it is
    bool showRedDisplay = false;
	Page* scheduleDisplay = new Page();
    QDateTime dt = QDateTime::currentDateTime();
    QTime t = QTime::currentTime();
    listOfTextAreas.clear();
    QString str = dt.toString();
    TextStyle *blueStyle = new TextStyle();
    blueStyle->setFontWeight(FontWeight::Bold);
    blueStyle->setFontSizeValue(FontSize::XXLarge);
    blueStyle->setColor(Color::Blue);
    TextStyle *redStyle = new TextStyle();
    redStyle ->setColor(Color::Red);
    thisOrientation = OrientationSupport::instance();
    QObject::connect(thisOrientation, SIGNAL(displayDirectionAboutToChange(bb::cascades::DisplayDirection::Type,
                       bb::cascades::UIOrientation::Type)), this, SLOT(scheduleUIDirectionChanging()));
    QString timeDisplay = t.toString("hh:mm:ss");
            QString text = "Time: " + timeDisplay;
            Label *label = new Label();
            label->setText(text);
            label->textStyle()->setBase(*blueStyle);

            Container* contentContainer = Container::create().top(30);
            if (Q10)
                contentContainer->setBackground(Color::Black);
            else
                contentContainer->setBackground(Color::White);

            Container* pContainer = new Container();
            StackLayout *pStackLayout = new StackLayout();
            pStackLayout->setOrientation( LayoutOrientation::TopToBottom );
            pContainer->setLayout(pStackLayout);
            Container* pContainer2;

            DockLayout *topTimeDisplayLayout = new DockLayout();
            contentContainer->setLayout(topTimeDisplayLayout);
            loadMoreButton = new Label();
            loadMoreButton->setText("<html><span style='text-decoration:underline'>More Times </span></html>");
            loadMoreButton->setHorizontalAlignment(HorizontalAlignment::Right);
            loadMoreButton->setVerticalAlignment(VerticalAlignment::Top);
            label->setHorizontalAlignment(HorizontalAlignment::Left);
            label->setVerticalAlignment(VerticalAlignment::Top);


            QList <QStringList> buses = localFile->getBuses();
            buses = specialCases(buses);
            for (int ii = 0; ii < 3; ii++) {
            		qDebug() << "BUS " + QString::number(ii + 1);
            		QStringList temporary = buses.at(ii);
            		for (int j = 0; j < buses.at(ii).length(); j++)
            			qDebug() << "Time: " + temporary[j];
            	}
            	qDebug() << "Finished displaying...";
            QStringList y;
            for (indexOfCurrentStationShowing = 0; indexOfCurrentStationShowing < stationsToShow+1 && indexOfCurrentStationShowing < buses.at(0).length()+1; indexOfCurrentStationShowing++) {//b = the container with the row of station
                pContainer2 = new Container();
                StackLayout *pStackLayout2 = new StackLayout();
                pStackLayout2->setOrientation( LayoutOrientation::LeftToRight);
                pContainer2->setLayout(pStackLayout2);

                for (int a = 0; a < 4; a++) { //a = each text box in the row and column
                TextArea *ta1 = new TextArea();
                ta1->setLeftMargin(0);
                ta1->setLeftPadding(0);
                ta1->setRightMargin(0);
                ta1->setRightPadding(0);
                ta1->setBottomMargin(5);
                ta1->setBottomPadding(5);
                ta1->setEditable(false);
                ta1->setPreferredWidth((deviceWidth - deviceWidth/2.0)/3.0);
                if (a == 0){//first column
                    ta1->setPreferredWidth(deviceWidth/2.0);
                    if (indexOfCurrentStationShowing == 0) //First row, first column (label)
                        ta1->setText("\t\tBuses\nStations");
                    else //First column, all station names
                        ta1->setText(localFile->getStations()[indexOfCurrentStationShowing-1]);
                }
                else { //Every other column after first
                    QStringList b1 = buses.at(a-1);
                    if (indexOfCurrentStationShowing == 0){//Displaying first row (bus numbers)
                        if (!b1[0].contains("N.A")){
                        	QString showText = b1[0].split(".")[2];
                        	showText.remove("_");
                        	ta1->setText("  " + showText);
                        }
                        else
                            ta1->setText("N.A");
                    }else{//Displaying all other rows (bus times)
                        y = b1[indexOfCurrentStationShowing-1].split(".");
                        if (y[0].contains("N") && y[1].contains("A"))
                            ta1->setText("  " + b1[indexOfCurrentStationShowing-1]);
                        else{
                        	ta1->setText(y[0] + ":" + y[1]);
                        	qDebug() << y[0] << y[1] << y[2];
                        	if (y[2].contains("_")){
                        		showRedDisplay = true;
                        		ta1->textStyle()->setBase(*redStyle);
                        	}
                        }
                    }
                }
                listOfTextAreas.append(ta1);
                if (ta1->text().contains("N.A") ){
                    if (indexOfCurrentStationShowing > 0 && a > 0){ //it is a bus time
                    	if (ta1->text().contains("N.A_"))
                        ta1->setText(" ");
                    	else{
                    		ta1->setText("<html>&#8595;</html>");
                    	}
                    }else{}
                        //ta1->setText("No Bus");
                }
                pContainer2->add(ta1);
                }
                //pContainer->setHorizontalAlignment(HorizontalAlignment::Center);
                pContainer->add(pContainer2);
            }
             QObject::connect(loadMoreButton, SIGNAL(touch(bb::cascades::TouchEvent*)),this,SLOT(loadMoreTimes(bb::cascades::TouchEvent*)));
             redDisplay = new Label();
             redDisplay->textStyle()->setBase(*redStyle);
             redDisplay->setMultiline(true);
             redDisplay->setText("This station is the last stop for this Bus/Train");
             redDisplay->setHorizontalAlignment(HorizontalAlignment::Center);
             if (!showRedDisplay)
             redDisplay->setVisible(false);
            moreStationsButton = new Label();
            moreStationsButton->setText("<html><span style='text-decoration:underline'>More Stations</span></html>");
            QObject::connect(moreStationsButton, SIGNAL(touch(bb::cascades::TouchEvent*)),this,SLOT(loadMoreStations(bb::cascades::TouchEvent*)));
            moreStationsButton->setHorizontalAlignment(HorizontalAlignment::Center);
            if (showRedDisplay || localFile->getStations().length() <= stationsToShow)
            	moreStationsButton->setVisible(false);
            else
            	moreStationsButton->setVisible(true);
            pContainer->setTopMargin(100);
            pContainer->setTopPadding(100);
            pContainer->add(redDisplay);
            pContainer->add(moreStationsButton);
            contentContainer->add(pContainer);
            contentContainer->add(label);
            contentContainer->add(loadMoreButton);

            scheduleDisplay->setContent(contentContainer);
            navigationPane->push(scheduleDisplay);

}
void ApplicationUI::scheduleUIDirectionChanging(){
	qDebug() << "Orientation about to change" << deviceWidth << deviceHeight;
	int tempNum = deviceWidth;
	deviceWidth = deviceHeight;
	deviceHeight = tempNum;
	TextArea *temp;
	for (int i = 0; i < listOfTextAreas.length(); i++){
		temp = listOfTextAreas.at(i);
		if (i % 4 == 0)
            temp->setPreferredWidth(deviceWidth/2.0);
		else
            temp->setPreferredWidth((deviceWidth - deviceWidth/2.0)/3.0);
		listOfTextAreas.replace(i,temp);
	}
}
void ApplicationUI::loadMoreStations(bb::cascades::TouchEvent* event){
	if(event->touchType() == bb::cascades::TouchType::Down){
		loadMoreButton->setVisible(false);
		QStringList stations = localFile->getStations();
		QList <QStringList> buses = localFile->getBuses();
		TextArea *textArea;
		indexOfCurrentStationShowing--;
//		for (int ii = 0; ii < 3; ii++) {
//			qDebug() << "BUS " + QString::number(ii + 1);
//			QStringList temporary = buses.at(ii);
//			for (int j = 0; j < buses.at(ii).length(); j++)
//				qDebug() << "Time: " + temporary[j];
//		}
//		for (int ii = 0; ii < stations.length(); ii++)
//			qDebug() << "i =" << ii << "... stations[i] =" <<stations[ii];
		qDebug() << "Finished displaying...";

		if (stations.length() < stationsToShow)
			return;
		int initialValue = indexOfCurrentStationShowing;
		//First update stations...
		int j;
		qDebug() << "stations.length = " << stations.length() << " stationsToShow =" << stationsToShow;
		int i;
		for (i = 0; i < stationsToShow; i++){
			j = 4*(i+1);
			textArea = listOfTextAreas.at(j);
			if (i + indexOfCurrentStationShowing < stations.length())
				textArea->setText(stations[i + indexOfCurrentStationShowing]);
			else{
				textArea->setText(" ");
				moreStationsButton->setVisible(false);
			}
			listOfTextAreas.replace(j,textArea);
		}

		for (int b = 0; b <buses.length(); b++){
			QStringList temp = buses.at(b);
			for (int a = temp.length()-1; a >= 0 ; a--){
				if (temp[a].compare("N.A") == 0)
					temp[a] = "N.A_";
				else
					a = -1;
			}
			buses.replace(b,temp);
		}

		//Now update times...
		int finalValue;
		for (int k = 0; k < buses.length(); k++){
		QStringList tempo = buses.at(k);
		for (i = (k+5); i < listOfTextAreas.length(); i+=4){
			textArea = listOfTextAreas.at(i);
			if (indexOfCurrentStationShowing < tempo.length()){
				if (tempo[indexOfCurrentStationShowing].contains("_"))
					textArea->setText(" ");
				else if (tempo[indexOfCurrentStationShowing].contains("N"))
					textArea->setText("<html>&#8595;</html>");
				else {
					QStringList temp = tempo[indexOfCurrentStationShowing].split(".");
					textArea->setText(temp[0] + ":" + temp[1]);
				}
			}
			else
				textArea->setText(" ");
			listOfTextAreas.replace(i,textArea);
			indexOfCurrentStationShowing++;
		}
		finalValue = indexOfCurrentStationShowing;
			indexOfCurrentStationShowing = initialValue;
		}
		indexOfCurrentStationShowing = finalValue;
	}

}
void ApplicationUI::loadMoreTimes(bb::cascades::TouchEvent* event){
	if(event->touchType() == bb::cascades::TouchType::Down){
		TextStyle *redStyle = new TextStyle();
		redStyle ->setColor(Color::Red);
		TextStyle *blackStyle = new TextStyle();
		blackStyle ->setColor(Color::Black);
		TextArea *temp;
		for(int i=7; i>4;i--){
			temp = listOfTextAreas.at(i);
			if(!temp->text().contains("N.A")){
				break;
			}
		}

		if(temp->text().contains("N.A") || temp->text().contains(" ") ){
			return;
		}
		qDebug() << "temp->text: " + temp->text();

		QStringList textAreaText;
		textAreaText = temp->text().split(":");

		bool ok; // to check for conversion errors
		int r = textAreaText[0].toInt(&ok, 10);//Hour
		int q = textAreaText[1].toInt(&ok, 10);//Minute
		q++;//Add one to q to get next bus after last one in row

		localFile->setHour(r);//Get updated times in a QList with all 3 buses.
		localFile->setMinute(q);
		localFile->parseXML();
		QList <QStringList> buses = localFile->getBuses();
		QStringList stations = localFile->getStations();
		int stationsSize = (stations.size() < 5) ? stations.size() : stationsToShow;

		buses = specialCases(buses);

		qDebug() << "Size of buses: " << QString::number(buses.size());
		int array[5] = {5,9,13,17,21};

		int terminate=0;
		for(int k=0;k<buses.size();k++){
			for(int l=0;l<stationsSize;l++){
				if(buses.at(k).at(l).compare("N.A_") == 0){
					terminate++;
					//qDebug() << "Terminate: " + QString::number(terminate);
				}
			}
		}

		// Do not update the times if ALL 15 times are returned as N.A_ which means there were no more buses on that route.
		if(terminate == stationsSize*buses.size()){
			redDisplay->setText("These are the last Bus/Train on this route.");
			redDisplay->setVisible(true);
			return;
		}


		for(int i=0;i<buses.size();i++){
			for(int j=0;j < stationsSize;j++){
				qDebug() << "Bus: " + buses.at(i).at(j);
				if(!buses.at(i).at(j).contains("N.A")){
					QString busName = buses.at(i).at(j).section(".",2,2);
					listOfTextAreas.at(array[j] + i)->textStyle()->setBase(*blackStyle);
					redDisplay->setVisible(false);
					if (busName.contains("_")){
						listOfTextAreas.at(array[j] + i)->textStyle()->setBase(*redStyle);
						redDisplay->setVisible(true);
						busName.remove("_");
					}
					QString busTime = buses.at(i).at(j).section(".",-3,-2);

					listOfTextAreas.at(i+1)->setText("  " + busName);
					// replace the "." to ":" so as to remain consistent between display times and bus times from buses QList
					listOfTextAreas.at(array[j] + i)->setText(busTime.replace(QString("."),QString(":")));

				}
				else{
					//Set to either an arrow, red text, or blank text
					if(buses.at(i).at(j).contains("_")){
						listOfTextAreas.at(array[j] + i)->setText(" ");
					}
					else{
						listOfTextAreas.at(array[j] + i)->setText("<html>&#8595;</html>");
					}
				}
			}
		}
	}
}

QList <QStringList> ApplicationUI::specialCases(QList <QStringList> buses){
    //Check for the special case when the station is a last station for a bus
    for (int b = 0; b < buses.length(); b++){
    	QStringList temp = buses.at(b);
    	int specialCase = 0;
    	for (int a = 0; a < temp.length(); a++){
    		if (temp[a].compare("N.A") != 0)
    			specialCase++;
    		if (specialCase > 1)
    			a = temp.length();//skip rest of loop
    	}
    	if (specialCase == 1 && !temp[0].contains("N.A")){
    		temp[0] = temp[0] + "_";
    		buses.replace(b,temp);
    	}
    }
    for (int b = 0; b <buses.length(); b++){
    	QStringList temp = buses.at(b);
    	for (int a = temp.length()-1; a >= 0 ; a--){
    		if (temp[a].compare("N.A") == 0)
    			temp[a] = "N.A_";
    		else
    			a = -1;
    	}
    	buses.replace(b,temp);
    }
    return buses;
}
void ApplicationUI::update(){
    progressIndicatorValue = progressIndicatorValue + 6;
    pI->setValue(progressIndicatorValue);
    data->update();

    if(data->_stopFetchFlag){
        qDebug() << "StopFetchFlag is now true";
        timer->stop();
    }
    if (data->_doneSaving){
        _saveData = false;
        pMyDialog->close();
        onClicked();
    }
    }
void ApplicationUI::removeUI(bool focused){
    if (focused){ // if it became in focus
        logo->setVisible(false);
        routeDropDown->blockSignals(true);
        if (textInput->text().length() < 1)
            showSuggestions("a");
        else
            showSuggestions(textInput->text());
        routeDropDown->blockSignals(false);
    }
    else{
        logo->setVisible(true);
        suggestion1->setVisible(false);
        suggestion2->setVisible(false);
        suggestion3->setVisible(false);
        suggestion4->setVisible(false);
        if (checkRoutes().length() != 0)
        	enableDropDown(checkRoutes());


    }
}
void ApplicationUI::setRouteNumber(QString r){
    qDebug() << "route = " + r  ;
    if (r.contains("Waterloo/Mississauga") )
        routeFile = "25";
    else if (r.contains("Lakeshore East") )
        routeFile = "10";
    else if (r.contains("Niagara Falls/Toronto") )
        routeFile = "12";
    else if (r.contains("Hamilton/Toronto Express") )
        routeFile = "16";
    else if (r.contains("Oakville /North York") )
        routeFile = "19";
    else if (r.contains("Milton/Oakville") )
        routeFile = "20";
    else if (r.contains("Milton("))
        routeFile = "21";
    else if (r.contains("Milton/North York") )
        routeFile = "27";
    else if (r.contains("Guelph/Mississauga") )
        routeFile = "29";
    else if (r.contains("Kitchener (formerly Georgetown)"))
        routeFile = "31";
    else if (r.contains("Brampton Trinity Common/North York"))
        routeFile = "32";
    else if (r.contains("Brampton/North York") )
        routeFile = "34";
    else if (r.contains("Orangeville/Brampton")  )
        routeFile = "37";
    else if (r.contains("Bolton/Malton/North York")  )
        routeFile = "38";
    else if (r.contains("Pearson Airport Express")  )
        routeFile = "40";
    else if (r.contains("407 West")  )
        routeFile = "46";
    else if (r.contains("407 East")  )
        routeFile = "52";
    else if (r.contains("Richmond Hill")  )
        routeFile = "61";
    else if (r.contains("Barrie(")  )
        routeFile = "65";
    else if (r.contains("Newmarket/North York Express")  )
        routeFile = "66";
    else if (r.contains("Keswick / North York")  )
        routeFile = "67";
    else if (r.contains("Stouffville")  )
        routeFile = "71";
    else if (r.contains("Beaverton/Pefferlaw")  )
        routeFile = "80";
    else if (r.contains("Port Perry/Whitby")  )
        routeFile = "81";
    else if (r.contains("Peterborough/Oshawa")  )
        routeFile = "88";
    else if (r.contains("Durham College/UOIT/Scarborough Express")  )
        routeFile = "93";
    else if (r.contains("Oshawa/Yorkdale")  )
        routeFile = "94";
    else if (r.contains("Oshawa/Finch(")  )
        routeFile = "95";
    else if (r.contains("Oshawa/Finch Express")  )
        routeFile = "96";
    else
        routeFile = "00"; //This should never happen...

}
void ApplicationUI::suggestionOneHighlight(){
    textInput->setText(suggestion1->text());
    suggestion1->setVisible(false);
    suggestion2->setVisible(false);
    suggestion3->setVisible(false);
    suggestion4->setVisible(false);
    BPS_API void virtualkeyboard_hide();
    virtualkeyboard_hide();
    enableDropDown(checkRoutes());
}

void ApplicationUI::suggestionTwoHighlight(){
    textInput->setText(suggestion2->text());
    suggestion1->setVisible(false);
    suggestion2->setVisible(false);
    suggestion3->setVisible(false);
    suggestion4->setVisible(false);
    BPS_API void virtualkeyboard_hide();
    virtualkeyboard_hide();
    enableDropDown(checkRoutes());
}

void ApplicationUI::suggestionThreeHighlight(){
    textInput->setText(suggestion3->text());
    suggestion1->setVisible(false);
    suggestion2->setVisible(false);
    suggestion3->setVisible(false);
    suggestion4->setVisible(false);
    BPS_API void virtualkeyboard_hide();
    virtualkeyboard_hide();
    enableDropDown(checkRoutes());
}

void ApplicationUI::suggestionFourHighlight(){
    textInput->setText(suggestion4->text());
    suggestion1->setVisible(false);
    suggestion2->setVisible(false);
    suggestion3->setVisible(false);
    suggestion4->setVisible(false);
    BPS_API void virtualkeyboard_hide();
    virtualkeyboard_hide();
    enableDropDown(checkRoutes());
}

void ApplicationUI::enableDropDown(QStringList elements){
    routeDropDown->blockSignals(true);
    routeDropDown->removeAll();
    routeDropDown->blockSignals(false);
    for (int i = 0; i < elements.length(); i++){
        Option *opt = new Option();
        opt->setText(elements[i]);
        routeDropDown->add(opt);
    }
    routeDropDown->setTitle("Route:");
    routeDropDown->setEnabled(true);
    routeDropDown->setExpanded(true);
}

void ApplicationUI::addDirection(int index){
    directionDropDown->removeAll();

    Option *routeOption = routeDropDown->at(index);
    QString routes = routeOption->text();
    int a = routes.lastIndexOf("(");
    int b = routes.lastIndexOf("-");
    int c = routes.lastIndexOf(")");
    Option *opt = new Option();
    QString optionA = "Towards " + routes.mid((a+1), (b-a-1));
    QString optionB = "Towards " + routes.mid((b+1), (c-b-1));
    opt->setText(optionA);
    directionDropDown->add(opt);
    Option *opt2 = new Option();
    opt2->setText(optionB);
    directionDropDown->add(opt2);

    directionDropDown->setTitle("Direction:");
    directionDropDown->setEnabled(true);

}
QStringList ApplicationUI::checkRoutes(){
    QStringList routes;
    directionDropDown->removeAll();
    QString selectedStation = textInput->text();
    int x;
    for (x = 0; x < stationName.length(); x++){
        if (QString::compare(selectedStation, stationName[x], Qt::CaseInsensitive) == 0)  // x == 0, if two strings are same
            break;
    }
    if (x == stationName.length()){
    	QStringList nullList;
    	return nullList;
    }
    QFile file(QDir::currentPath() + "/app/native/assets/schedule/stations.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Could not open file for parsing";
    }
    int index = 0;
    while (!file.atEnd()) {
        QString line = file.readLine();
        if (index == x){
            QStringList elements = line.split(",");
            int a = 1;
            while (a < elements.length()){
                routes.append(elements[a]);
                a++;
            }
            break;
        }
        else
        index++;
    }
    file.close();
    return routes;
}

void ApplicationUI::showSuggestions(QString text){//Called whenever a new character is put into textfield

    routeDropDown->setTitle("Select a Route (Choose a station first");
    routeDropDown->setEnabled(false);
    routeDropDown->blockSignals(true);
    directionDropDown->removeAll();
    routeDropDown->removeAll();
    routeDropDown->blockSignals(false);

    QString s1 = "",s2 = "",s3 = "",s4 = "";
    for (int i = 0; i < stationName.length(); i++)
    {
        QString r = stationName[i].left(text.length());
        int x = QString::compare(r, text, Qt::CaseInsensitive);  // x == 0, if two strings are same
        if (x == 0){
            if (s1 == ""){
                s1 = stationName[i];
            }
            else if (s2 == ""){
                s2 = stationName[i];
            }
            else if (s3 == ""){
                s3 = stationName[i];
            }
            else if (s4 == ""){
                s4 = stationName[i];
            }
            else{
                break;
            }
        }
    }
    for (int j = 0; j < stationName.length(); j++){
        QString r = stationName[j].left(text.length());
        if (stationName[j].contains(text)){
            if (s1 == ""){
                s1 = stationName[j];
            }
            else if (s2 == ""){
                if (stationName[j].compare(s1) != 0)
                s2 = stationName[j];
            }
            else if (s3 == ""){
                if (stationName[j].compare(s2) != 0 && stationName[j].compare(s1) != 0)
                s3 = stationName[j];
            }
            else if (s4 == ""){
                if (stationName[j].compare(s3) != 0 && stationName[j].compare(s2) != 0 && stationName[j].compare(s1) != 0)
                s4 = stationName[j];
            }
            else
                break;
        }
    }

    if (s1 != ""){
        suggestion1->setText(s1);
        suggestion1->setVisible(true);}
    else{
        suggestion1->setVisible(false);
    }

    if (s2 != ""){
        suggestion2->setText(s2);
        suggestion2->setVisible(true);}
    else{
        suggestion2->setVisible(false);
    }

    if (s3 != ""){
        suggestion3->setText(s3);
        suggestion3->setVisible(true);}
    else{
        suggestion3->setVisible(false);
    }

    if (s4 != ""){
        suggestion4->setText(s4);
        suggestion4->setVisible(true);}
    else{
        suggestion4->setVisible(false);
    }
}

void popFinished(bb::cascades::Page* page)
{
    delete page;
}
