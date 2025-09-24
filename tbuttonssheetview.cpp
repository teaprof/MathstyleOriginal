#include "tbuttonssheetview.h"
TKeywordsModel::TKeywordsModel(QObject* parent) : QAbstractListModel(parent) {}

int TKeywordsModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return List.count();
}
int TKeywordsModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant TKeywordsModel::data(const QModelIndex& index, int role) const {
    Q_UNUSED(index);
    if (role == Qt::DisplayRole) {
        if (index.row() < List.count()) return List[index.row()];
    }
    return QVariant();
}

void TKeywordsModel::AddString(QString Str) {
    beginResetModel();
    List.push_back(Str);
    endResetModel();
}

void TKeywordsModel::clear() {
    beginResetModel();
    List.clear();
    endResetModel();
}

QVariant TKeywordsModel::headerData(int section, Qt::Orientation o, int role) const {
    Q_UNUSED(section);
    Q_UNUSED(o);
    if (role == Qt::DisplayRole) {
        return "a";
    }
    return QVariant();
}
const QStringList* TKeywordsModel::StringList() const {
    return &List;
}

//************************************************************************************

TButtonsSheetView::TButtonsSheetView(QWidget* parent) : QWidget(parent) {
    Layout = new QVBoxLayout(this);
    // Layout->setMargin(0);
    Model = new TKeywordsModel(this);

    ListView = new QListView(this);

    Layout->addWidget(ListView);
    ListView->setModel(Model);

    setLayout(Layout);

    connect(ListView, SIGNAL(clicked(QModelIndex)), SLOT(OnKeywordPressed(QModelIndex)));

    //    QString Style;
    //    ListView->setStyleSheet(Style);
}

TButtonsSheetView::~TButtonsSheetView() {
    ClearStrings();
}

void TButtonsSheetView::ClearStrings() {
    Model->clear();
}

void TButtonsSheetView::AddString(QString Word) {
    if (Model->StringList()->contains(Word) == false) {
        Model->AddString(Word);
    };
}

void TButtonsSheetView::OnKeywordPressed(const QModelIndex& Index) {
    QVariant Data = Model->data(Index);
    if (Data.isValid()) emit Picked(Data.toString());
}
