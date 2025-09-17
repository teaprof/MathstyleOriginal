#ifndef TBUTTONSSHEETVIEW_H
#define TBUTTONSSHEETVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QAbstractListModel>
#include <QStringList>
#include <QModelIndex>

class TKeywordsModel : public QAbstractListModel
{
  Q_OBJECT

public:

  TKeywordsModel(QObject *parent = 0);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  void AddString(QString Str);
  void clear();
  QVariant headerData(int section, Qt::Orientation o, int role) const;
  const QStringList* StringList() const;
protected:
  QStringList List;
};

class TButtonsSheetView : public QWidget
{
    Q_OBJECT

    QVBoxLayout *Layout;
    QListView *ListView;
    TKeywordsModel* Model;

public:
    explicit TButtonsSheetView(QWidget *parent = 0);
    ~TButtonsSheetView();

    void ClearStrings();
    void AddString(QString Word); //adds string if Words does not contain it
signals:
    void Picked(QString Keyword);

public slots:
    void OnKeywordPressed(const QModelIndex& Index);

};

#endif // TBUTTONSSHEETVIEW_H
