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
#include <bb/cascades/TextArea>
#include <QLocale>
#include <QTranslator>
#include <bb/cascades/ScrollView>
#include <bb/cascades/ScrollMode>

#include "data.hpp"
#define QT_DECLARATIVE_DEBUG

#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;



ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{


		navigationPane = NavigationPane::create();
		// Create the root container
		Container* contentContainer = Container::create()
	                         .top(300);
		contentContainer->setBackground(Color::fromARGB(0xff7A7A7A));

		StackLayout *pStackLayout = new StackLayout();
		pStackLayout->setOrientation( LayoutOrientation::TopToBottom );
		contentContainer->setLayout(pStackLayout);

		ImageView* logo = ImageView::create("asset:///images/go-transit.png");
		logo->setHorizontalAlignment(HorizontalAlignment::Center);


		dropDownFrom = DropDown::create().title("Starting from...");
		McMaster = Option::create().text("McMaster");
		Oakville = Option::create().text("Oakville Carpool");
		dropDownFrom->add(McMaster);
		dropDownFrom->add(Oakville);
		dropDownFrom->setHorizontalAlignment(HorizontalAlignment::Center);

		dropDownTo = DropDown::create().title("Going to...");
		dropDownTo->add(McMaster);
		dropDownTo->add(Oakville);
		dropDownTo->setHorizontalAlignment(HorizontalAlignment::Center);

		Button* findSchedule = Button::create()
		.text("Find Schedule")
		.image(Image("asset:///myImage.png"));
		findSchedule->setHorizontalAlignment(HorizontalAlignment::Center);

		Button* button = Button::create()
		.text("From McMaster")
		.image(Image("asset:///myImage.png"));
		button->setHorizontalAlignment(HorizontalAlignment::Center);

		Button* button2 = Button::create()
		.text("From Oakville")
		.image(Image("asset:///myImage.png"));
		button2->setHorizontalAlignment(HorizontalAlignment::Center);

		Button* button3 = Button::create().text("Fetch Data");
		QObject::connect(button3,SIGNAL(clicked()),this,SLOT(onClicked()));

		contentContainer->add(logo);
		contentContainer->add(dropDownTo);
		contentContainer->add(dropDownFrom);
		contentContainer->add(findSchedule);
		contentContainer->add(button3);

		bool res = QObject::connect(findSchedule, SIGNAL(clicked()), this,
		                            SLOT(handleOakvilleClick()));
		Q_ASSERT(res);

		// Indicate that the variable res isn't used in the rest of the app, to prevent
		// a compiler warning
		Q_UNUSED(res);

		res = QObject::connect(button2, SIGNAL(clicked()), this,
				                            SLOT(handleOakvilleClick()));
		Q_ASSERT(res);
		// Indicate that the variable res isn't used in the rest of the app, to prevent
		// a compiler warning
		Q_UNUSED(res);



		page = new Page();
		page->setContent(contentContainer);
		navigationPane->push(page);
		app->setScene(navigationPane);

}
void ApplicationUI::onClicked(){
	Data* data = new Data(this);
	data->fetchData();
}
void ApplicationUI::handleOakvilleClick()
{
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





