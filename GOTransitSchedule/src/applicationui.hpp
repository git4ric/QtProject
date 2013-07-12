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
#include <bb/cascades/ScrollView>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollMode>
#include <bb/cascades/Tab>
#include <bb/cascades/Sheet>
#include <bb/cascades/WebView>
#include <bb/system/InvokeManager>
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
	void onOpenWebsiteClicked();
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
private:

	bb::cascades::Label *label;
	bb::cascades::Page *page, *page2,*page3;
	bb::cascades::DropDown *dropDownTo;
	bb::cascades::DropDown *dropDownFrom;
	bb::cascades::Option *Oakville;
	bb::cascades::Option *McMaster;
	bb::cascades::Option *option;
	bb::cascades::Container* _page3Container;
	QStringList stationName;
	bb::system::InvokeManager* _manager;
	bb::system::InvokeTargetReply* _reply;
	bb::cascades::Tab* _tab2;
	bb::cascades::Sheet* _webSheet;
	QTimer *timer;
	Data* data;
	bool _saveData;
};


#endif /* ApplicationUI_HPP_ */
