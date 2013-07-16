// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
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
#include "data.hpp"
#include "LoadData.hpp"
namespace bb { namespace cascades { class Application; }}
namespace bb { namespace cascades { class Container; }}

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
	void getDeviceInformation();
	void loadMoreTimes(bb::cascades::TouchEvent* event);
	void loadMoreStations(bb::cascades::TouchEvent* event);
	void removeUI(bool focused);
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
	void loadServiceUpdates();
	void closeSheet();
	void help_clicked();
private:

	bb::cascades::Label *label;
	bb::cascades::Page *page, *page2;
	bb::cascades::DropDown *dropDownTo;
	bb::cascades::DropDown *dropDownFrom;
	bb::cascades::Option *Oakville;
	bb::cascades::Option *McMaster;
	bb::cascades::Option *option;
	QStringList stationName;
	bb::system::InvokeManager* _manager;
	bb::system::InvokeTargetReply* _reply;
	QTimer *timer;
	Data* data;
	bool _saveData;
	bb::cascades::Sheet* _webSheet;
	bb::cascades::Container* _page3Container;

};


#endif /* ApplicationUI_HPP_ */
