#include "tsolutionbrowser.h"

#include <QAction>
#include <QFileDialog>
#include <QUrl>
#include <QVBoxLayout>

#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>

TSolutionBrowser::TSolutionBrowser(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::Window);

    QVBoxLayout* layout = new QVBoxLayout(this);
    WebView = new QWebEngineView(this);
    ToolBar = new QToolBar(this);
    ToolBar->setIconSize(QSize(48, 48));

    layout->addWidget(ToolBar);
    layout->addWidget(WebView);
    setLayout(layout);

    // RegisterToolButton(":formulas/Pictures/Formulas/arrow_left_48.png", "Back", WebView, SLOT(back()));
    // RegisterToolButton(":formulas/Pictures/Formulas/arrow_right_48.png", "Forward", WebView, SLOT(forward()));
    RegisterToolButton(":Commands/Pictures/Commands/Zoom In_48x48.png", "ZoomIn", this, SLOT(ZoomIn()));
    RegisterToolButton(":Commands/Pictures/Commands/Zoom Out_48x48.png", "ZoomOut", this, SLOT(ZoomOut()));
    ToolBar->addSeparator();

    // RegisterToolButton(":Commands/Pictures/Commands/floppy_disk_48.png", "Save", this, SLOT(Save()));
    RegisterToolButton(":Commands/Pictures/Commands/file_pdf.png", "Export PDF", this, SLOT(ExportToPDF()));
    RegisterToolButton(":Commands/Pictures/Commands/print.png", "Print", this, SLOT(Print()));

    ToolBar->addSeparator();
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ToolBar->addWidget(spacer);

    ToolBar->addSeparator();
    RegisterToolButton(":Commands/Pictures/Commands/cross_48.png", "Close", this, SLOT(Close()));

    SetCaption("Mathstyle Pro Browser");
    setMinimumSize(800, 600);
};

void TSolutionBrowser::RegisterToolButton(const char* Img, QString ToolTip, QWidget* Receiver, const char* Slot) {
    QIcon* Icon = new QIcon(Img);
    ToolBar->addAction(*Icon, ToolTip, Receiver, Slot);
    delete Icon;
}

void TSolutionBrowser::Load(QString DocumentFolder, QString FileName) {
    QUrl URL = QUrl::fromLocalFile(DocumentFolder + "/" + FileName);
    // URL.setPath(DocumentFolder);
    WebView->load(URL);
    // WebView->load(QUrl("http://mail.ru"));
    // WebView->setHtml(FileName, QUrl(URL));
}

void TSolutionBrowser::SetCaption(QString Caption) {
    this->setWindowTitle(Caption);
}

void TSolutionBrowser::ZoomIn() {
    WebView->setZoomFactor(2.0 * WebView->zoomFactor());
}
void TSolutionBrowser::ZoomOut() {
    WebView->setZoomFactor(0.5 * WebView->zoomFactor());
}
void TSolutionBrowser::Save() {}

void TSolutionBrowser::Print() {
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        WebView->print(&printer);
    }
}

void TSolutionBrowser::ExportToPDF() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export as Pdf.."), "", tr("PDF files (*.pdf)"));
    if (fileName.isEmpty())
        return;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setFullPage(false);
    printer.setPageOrientation(QPageLayout::Orientation::Portrait);
    printer.setPageSize(QPageSize::A4);
    WebView->print(&printer);
}
void TSolutionBrowser::Close() {
    this->close();
}
