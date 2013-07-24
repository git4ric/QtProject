// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <qt4/QtCore/QObject>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/Option>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>
#include <bb/cascades/TextField>
#include <bb/cascades/ImageView>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeManager>
#include <bb/cascades/WebView>
#include <bb/cascades/Sheet>
#include <bb/cascades/TitleBar>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiPosition>
#include <bb/cascades/ImageButton>
#include <bb/cascades/Menu>
#include <bb/cascades/ActionItem>
#include <bb/cascades/SettingsActionItem>
#include <bb/system/SystemDialog>
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
#include <bb/cascades/CheckBox>
#include <bb/cascades/ToggleButton>

#include "data.hpp"
#include "LoadData.hpp"
namespace bb { namespace cascades { class Application; }}
namespace bb { namespace cascades { class Container; }}
namespace bb { namespace cascades { class Page; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() {}
    void loadServiceUpdates(QString link);

public slots:
	void update();
	void settings_clicked();
	void saveSettings();
	void reinitialize_App_Screen();
	void firstFavClicked(bb::cascades::TouchEvent* event);
	void secondFavClicked(bb::cascades::TouchEvent* event);
	void thirdFavClicked(bb::cascades::TouchEvent* event);
	void fourthFavClicked(bb::cascades::TouchEvent* event);
	QString addFavoriteStation(QString StationName);
	QString removeFavoriteStation(QString StationName);
	void onOpenWebsiteClicked();
	void getSettingsInfo();
	void scheduleUIDirectionChanging();
	void onReplyFinished();
	//void onPopTransitionEnded(bb::cascades::Page* page);
	void popFinished(bb::cascades::Page* page);
	void getDeviceInformation();
	void loadMoreTimes(bb::cascades::TouchEvent* event);
	void loadMoreStations(bb::cascades::TouchEvent* event);
	void removeUI(bool focused);
	void customTimeCheckBoxChanged(bool changed);
	void suggestionOneHighlight();
	void suggestionTwoHighlight();
	void suggestionThreeHighlight();
	void suggestionFourHighlight();
	void onDisplayDirectionAboutToChange();
	void showSuggestions(QString text);
	void onClicked();
	void createSettingsTab();
	void enableDropDown(QStringList elements);
	QStringList checkRoutes();
	QList <QStringList> specialCases(QList <QStringList> buses);
	void setUpScheduleUI();
	void addDirection(int index);
	void enableButton();
	void setRouteNumber(QString r);
	void openScheduleButtonClicked();
	void updateButtonClicked();
	void closeSheet();
	void help_clicked();
private:

	bb::cascades::Label *label;
	bb::cascades::Page *page, *page2, *_page3;;
	bb::cascades::Option *option;
	QStringList stationName;
	bb::system::InvokeManager* _manager;
	bb::system::InvokeTargetReply* _reply;
	QTimer *timer;
	Data* data;
	bool _saveData;
	bb::cascades::Sheet* _webSheet;
	bb::cascades::Container* _page3Container;
	QString routeFile;
	bb::cascades::TabbedPane *tabbedPane;
	int stationsToShow, indexOfCurrentStationShowing;
	bb::cascades::NavigationPane* navigationPane, *navigationPaneService, *navigationPaneDonate;
	bb::cascades::Button* fetchData;
	bb::cascades::Dialog* pMyDialog;
	bb::cascades::Label *lab,  *loadMoreButton, *redDisplay;
	bb::cascades::ProgressIndicator *pI;
	QList<bb::cascades::TextArea*> listOfTextAreas;
	bb::cascades::Label *moreStationsButton ;
	LoadData *localFile;
	bb::cascades::OrientationSupport *thisOrientation;
	int deviceWidth, deviceHeight;
	bb::cascades::Container *suggestion1Container,*suggestion2Container,*suggestion3Container,*suggestion4Container, *customTimeContainer ;
	bb::cascades::ImageButton* imageFavOne, *imageFavTwo, *imageFavThree, *imageFavFour;

	bb::cascades::TextField *suggestion1, *suggestion2, *suggestion3, *suggestion4, *textInput;
	bb::cascades::ImageView *logo;
	bb::cascades::DropDown *routeDropDown, *directionDropDown, *customTimeHour, *customTimeMinute;

	bb::cascades::Container* contentContainer;
	QList<QString> favStations;

	QList<bb::cascades::CheckBox*> c;
	bb::cascades::ToggleButton *t;
	QString hour, minute;


};


#endif /* ApplicationUI_HPP_ */
