#ifndef TSOLUTIONBROWSER_H
#define TSOLUTIONBROWSER_H

#include <QtWebEngineWidgets/QWebEngineView>
#include <QToolBar>
#include <QWidget>

class TSolutionBrowser : public QWidget
{
    Q_OBJECT
private:    
    QWebEngineView *WebView;
    QToolBar *ToolBar;    
    void RegisterToolButton(const char* Img, QString ToolTip, QWidget* Receiver, const char* Slot);

public:
    explicit TSolutionBrowser(QWidget *parent = 0);
    void Load(QString DocumentFolder, QString FileName);
    void SetCaption(QString Caption);
signals:

public slots:
    void ZoomIn();
    void ZoomOut();
    void Close();
    void Save();
    void Print();
    void ExportToPDF();

};

#endif // TSOLUTIONBROWSER_H
