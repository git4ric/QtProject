//TODO: Custom icons for tabbed pane icons (GO and Presto)
//TODO: Add coming soon screen for PRESTO tab


// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "applicationui.hpp"


#include <bb/cascades/ActionBarPlacement>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Slider>
#include <bb/cascades/Stacklayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Button>
#include <bb/cascades/NavigationPaneProperties>
#include <qt4/QtCore/QTime>
#include <qt4/QtCore/QStringList>
#include <bb/cascades/TextStyle>
#include <bb/cascades/FontWeight>
#include <bb/cascades/FontSize>
#include <qt4/QtCore/QFile>
#include <bb/cascades/Dialog>
#include <QTextStream>
#include <bb/cascades/TextField>
#include <bb/cascades/TextArea>
#include <bb/cascades/KeyListener>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/ScrollView>
#include <bb/cascades/ScrollMode>
#include <bps/virtualkeyboard.h>
#include <bb/cascades/Tab>
#include "data.hpp"

using namespace bb::cascades;


QStringList stationName;
TextField *suggestion1, *suggestion2, *suggestion3, *suggestion4, *textInput;
ImageView *logo;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{


		navigationPane = NavigationPane::create();
		// Create the root container
		Container* contentContainer = Container::create()
	                         .top(100);
		contentContainer->setBackground(Color::fromARGB(0x005A5A5A));


		StackLayout *pStackLayout = new StackLayout();
		pStackLayout->setOrientation( LayoutOrientation::TopToBottom );
		contentContainer->setLayout(pStackLayout);

		logo = ImageView::create("asset:///images/go-transit.png");
		logo->setHorizontalAlignment(HorizontalAlignment::Center);


		//dropDownFrom = DropDown::create().title("Starting from...");
		textInput = new TextField();
		textInput->setHintText("Starting from...");

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
			QByteArray line = file.readLine();
			stationName.append(line);
			//option = Option::create().text(line);
			//dropDownFrom->add(option);

		}
		//dropDownFrom->setHorizontalAlignment(HorizontalAlignment::Center);

		Button* findSchedule = Button::create()
		.text("Find Schedule")
		.image(Image("asset:///myImage.png"));
		findSchedule->setHorizontalAlignment(HorizontalAlignment::Center);

		contentContainer->add(logo);
		//contentContainer->add(dropDownFrom);
		contentContainer->add(textInput);
		contentContainer->add(suggestion1);
		contentContainer->add(suggestion2);
		contentContainer->add(suggestion3);
		contentContainer->add(suggestion4);
		contentContainer->add(findSchedule);

		bool res = QObject::connect(findSchedule, SIGNAL(clicked()), this,
		                            SLOT(handleOakvilleClick()));
		Q_ASSERT(res);

		res = QObject::connect(textInput, SIGNAL(textChanging(QString)), this,
				                            SLOT(showSuggestions(QString)));

		res = QObject::connect(textInput, SIGNAL(focusedChanged(bool)), this,
						                            SLOT(removeUI(bool)));

		res = QObject::connect(suggestion1, SIGNAL(focusedChanged(bool)), this,
						                            SLOT(suggestionOneHighlight()));

		res = QObject::connect(suggestion2, SIGNAL(focusedChanged(bool)), this,
						                            SLOT(suggestionTwoHighlight()));

		res = QObject::connect(suggestion3, SIGNAL(focusedChanged(bool)), this,
						                            SLOT(suggestionThreeHighlight()));

		res = QObject::connect(suggestion4, SIGNAL(focusedChanged(bool)), this,
						                            SLOT(suggestionFourHighlight()));
		// Indicate that the variable res isn't used in the rest of the app, to prevent
		// a compiler warning
		Q_UNUSED(res);

		TabbedPane* tabbedPane = TabbedPane::create() .showTabsOnActionBar(true);
			Tab* tab = new Tab();
			tab->setTitle("GO Transit");
			tab->setDescription("Get Bus route timings from any station for GO Transit!");

			Tab* tab2 = new Tab();
			tab2->setTitle("PRESTO Card");
			tab2->setDescription("Get balance and other info about your PRESTO Card!");

			Tab* tab3 = new Tab();
			tab3->setTitle("Settings");
			tab3->setDescription("App settings and specifications");

		page = new Page();
		page->setContent(contentContainer);
		navigationPane->push(page);
		tab->setContent(navigationPane);
		tabbedPane->add(tab);
		tabbedPane->add(tab2);
		tabbedPane->add(tab3);

		app->setScene(tabbedPane);

	     //   virtualkeyboard_change_options(VIRTUALKEYBOARD_LAYOUT_NUM_PUNC, VIRTUALKEYBOARD_ENTER_DEFAULT);
}

void ApplicationUI::removeUI(bool focused){
	if (focused)
		logo->setVisible(false);
	else
		logo->setVisible(true);

}
void ApplicationUI::suggestionOneHighlight(){
	textInput->setText(suggestion1->text());
	suggestion1->setVisible(false);
	suggestion2->setVisible(false);
	suggestion3->setVisible(false);
	suggestion4->setVisible(false);
}

void ApplicationUI::suggestionTwoHighlight(){
	textInput->setText(suggestion2->text());
		suggestion1->setVisible(false);
		suggestion2->setVisible(false);
		suggestion3->setVisible(false);
		suggestion4->setVisible(false);


}

void ApplicationUI::suggestionThreeHighlight(){
	textInput->setText(suggestion3->text());
		suggestion1->setVisible(false);
		suggestion2->setVisible(false);
		suggestion3->setVisible(false);
		suggestion4->setVisible(false);


}

void ApplicationUI::suggestionFourHighlight(){
	textInput->setText(suggestion4->text());
		suggestion1->setVisible(false);
		suggestion2->setVisible(false);
		suggestion3->setVisible(false);
		suggestion4->setVisible(false);


}
void ApplicationUI::showSuggestions(QString text){//Called whenever a new character is put into textfield
	QString s1 = "",s2 = "",s3 = "",s4 = "";
	for (int i = 0; i < stationName.length(); i++){
		QString r = stationName[i].left(text.length());
		 int x = QString::compare(r, text, Qt::CaseInsensitive);  // x == 0, if two strings are same
		if (x == 0){
			if (s1 == "")
				s1 = stationName[i];
			else if (s2 == "")
				s2 = stationName[i];
			else if (s3 == "")
				s3 = stationName[i];
			else if (s4 == "")
				s4 = stationName[i];
			else
				break;
		}

	}
	if (s1 != ""){
	suggestion1->setText(s1);
	suggestion1->setVisible(true);}
	else
		suggestion1->setVisible(false);
	if (s2 != ""){
	suggestion2->setText(s2);
	suggestion2->setVisible(true);}
	else
		suggestion2->setVisible(false);
	if (s3 != ""){
	suggestion3->setText(s3);
	suggestion3->setVisible(true);}
	else
		suggestion3->setVisible(false);
	if (s4 != ""){
	suggestion4->setText(s4);
	suggestion4->setVisible(true);}
	else
		suggestion4->setVisible(false);
}

void ApplicationUI::handleOakvilleClick()
{
	Data* data = new Data(this);
	data->fetchData();

	    Page* root = new Page();
	    QString labelText = "Label";
	    Label *label = new Label();

	    TextStyle *blueStyle = new TextStyle();
	    blueStyle->setFontWeight(FontWeight::Bold);
	    blueStyle->setFontSizeValue(FontSize::XXLarge);
	    blueStyle->setColor(Color::Blue);

	    QDateTime dt = QDateTime::currentDateTime();
	    QTime t = QTime::currentTime();

	    QString str = dt.toString();
	    int hour = t.hour();
	    int minute = t.minute();

	    /*QString hour = QString::number(t.hour());
	    QString minute = QString::number(t.minute());
	    QString second = QString::number(t.second());*/
	    QString fileName;
	    if (dropDownTo->selectedOption() == McMaster)
	    	fileName = "app/native/assets/schedule/fromMcMaster.txt";
	    else if (dropDownTo->selectedOption() == Oakville)
	    	fileName = "app/native/assets/schedule/fromOakville.txt";

	    QFile file(fileName);
	    QString output;
	    QString process;
	    bool set = false;
	    			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	    			         return;

	    			     while (!file.atEnd()) {
	    			         QByteArray line = file.readLine();
	    			         process = line;
	    			         QStringList p = process.split(" ");
	    			         QStringList q = p[0].split(":");
	    			         bool ok;
	    			         int h = q[0].toInt(&ok,10);
	    			         int m = q[1].toInt(&ok,10);
	    			         if (h == hour)
	    			         {
	    			        	 if (m > minute){
	    			        		 output = output + "Next bus --> ";
	    			        		 set = true;}
	    			         }
	    			         else if (h > hour && set == false)
	    			         {
	    			             	output = output + "Next bus --> ";
	    			             	set = true;}
	    			         output = output + line;
	    			     }
	    			     if (set == false){
	    			    	 QString newOuput = output;
	    			    	 output = "Next bus -->" + output;;

	    			     }
	    			 // QByteArray ba = line.toLatin1();
	    			 // const char *c_str2 = ba.data();
	    			 // fprintf(stdout, c_str2);
	    QString text = "Time: " + str;
	    label->setText(text);
	    label->textStyle()->setBase(*blueStyle);
	    TextArea *ta = new TextArea();
	    ta->setEditable(false);
	    ta->setText(output);


	    Container* contentContainer = Container::create()
	    	                         .top(100);
	    contentContainer->setBackground(Color::fromARGB(0xff9A9A9A));
		label->setHorizontalAlignment(HorizontalAlignment::Center);
		ta->setHorizontalAlignment(HorizontalAlignment::Center);

		contentContainer->add(label);
		contentContainer->add(ta);

		root->setContent(contentContainer);

	    navigationPane->push(root);

}

void popFinished(bb::cascades::Page* page)
{
    delete page;
}





