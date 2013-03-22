// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/Option>
#include <bb/cascades/Label>
#include <bb/cascades/DropDown>

namespace bb { namespace cascades { class Application; }}

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
	void handleOakvilleClick();
	QStringList checkRoutes();
	void enableDropDown(QStringList elements);
	void removeUI(bool focused);
	void suggestionOneHighlight();
	void suggestionTwoHighlight();
	void suggestionThreeHighlight();
	void suggestionFourHighlight();
	void showSuggestions(QString text);

private:
	bb::cascades::Label *label;
	bb::cascades::Page *page;
	bb::cascades::DropDown *dropDownTo;
	bb::cascades::DropDown *dropDownFrom;
	bb::cascades::Option *Oakville;
	bb::cascades::Option *McMaster;
	bb::cascades::Option *option;
	bb::cascades::NavigationPane* navigationPane;

};


#endif /* ApplicationUI_HPP_ */
