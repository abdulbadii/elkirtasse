/****************************************************************************
//   elkirtasse Copyright (C) 2010 yahia abouzakaria <yahiaui@gmail.com>
//
//      This program comes with ABSOLUTELY NO WARRANTY; for details type `show
w'.
//      This is free software, and you are welcome to redistribute it
//      under certain conditions; type `show c' for details.
//
//  The hypothetical commands `show w' and `show c' should show the appropriate
//  parts of the General Public License.  Of course, your program's commands
//  might be different; for a GUI interface, you would use an "about box".
//
//    You should also get your employer (if you work as a programmer) or school,
//  if any, to sign a "copyright disclaimer" for the program, if necessary.
//  For more information on this, and how to apply and follow the GNU GPL, see
//  <http://www.gnu.org/licenses/>.
//
//    The GNU General Public License does not permit incorporating your program
//  into proprietary programs.  If your program is a subroutine library, you
//  may consider it more useful to permit linking proprietary applications with
//  the library.  If this is what you want to do, use the GNU Lesser General
//  Public License instead of this License.  But first, please read
//  <http://www.gnu.org/philosophy/why-not-lgpl.html>.
// ----------------------------------------------------------
** If you have questions regarding the use of this file, please contact
** abouzakaria (yahiaui@gmail.com)
** $elkirtasse_END_LICENSE$
**
****************************************************************************/

#ifdef Q_WS_WIN
	#include "dialogmdb.h"
#else
	#include "mdbexport.h"
#endif
#include "about.h"
#include "dialog.h"
#include "dialogactions.h"
#include "dialogconfigbooks.h"
#include "dialogconvert.h"
#include "dialogfavo.h"
#include "dialogfind.h"
#include "dialoggoto.h"
#include "dialogimportdvd.h"
#include "dialogoption.h"
#include "dialogupdat.h"
#include "mainwindow.h"
#include "print.h"
#include "ui_mainwindow.h"
#include "mw.h"
#include <QDockWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QStyleFactory>
#include <QWhatsThis>
#include <QtGui>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <stdexcept>
// تحميل البرنامج
MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
    view = new View(this);
	QDir dir;
	QString h = dir.homePath() + "/";
	MW::pathUser = h + ".kirtasse/";
	if (! dir.exists(MW::pathUser))                  // التاكد من وجود مجلد المكتبة
		for (auto n: {"data","books","download"})
			dir.mkpath( MW::pathUser + n);
	else
		for (auto n : {"data", "books", "download"})        // التاكد من وجود مجلد البياات
									//  التاكد من وجود مجلد الكتاب
									// التاكد من وجود مجلد مؤقت
			{
			QString p = MW::pathUser + n;
			if (! dir.exists(p)) dir.mkdir(p);
		}
	if (!dir.exists( h + ".fonts"))	dir.mkdir( h + ".fonts");		// التاكد من وجود مجلد مؤقت

	QDir appDir( QCoreApplication::applicationDirPath());
	if (QRegularExpression{"^/(?:usr/(local/)?)?bin$"}.match(appDir.absolutePath()).hasMatch()
		|| appDir.absolutePath() == h+".local/bin") {
		appDir.cdUp();
		MW::pathApp = appDir.absolutePath() + "/share/elkirtasse/";
		if (! dir.exists(MW::pathApp)) {
			if (! dir.mkdir(MW::pathApp))
			throw std::runtime_error(("Failed installation: missing:" + MW::pathApp).toStdString());
		}
	} else {
		bool noBuilD;
		while ((noBuilD=appDir.dirName() != "build") && appDir.cdUp()) ;
		if (noBuilD || !appDir.cdUp())
			throw std::runtime_error("No usr/share/elkirtasse folder in the project directory");
		else
			MW::pathApp = appDir.absolutePath() + "/usr/share/elkirtasse/";
	}
	qDebug() << "Mw::pathApp= "<<MW::pathApp;	qDebug() << "Mw::pathUser= "<<MW::pathUser;
	QFile file;
	QString H = h + ".kirtasse/", F = "data/group.xml";
	if (!file.exists( H + F))
		file.copy( MW::pathApp + F, H + F);
	F = "data/bookslist.xml";
	if (!file.exists( H + F))
		file.copy( MW::pathApp + F, H + F);
	F = ".fonts/trado.ttf";
	if (!file.exists( h + F))
		file.copy( MW::pathApp + "data/" +F, h + F);

	message = new Messages;
	findBook = new FindBook;
	databook = new DataBook;

	m_currentIndex = 0;
    for (int r = 0; r < 10; ++r) {
        m_isTefsir[r] = false;
        m_treeFahrasIsModified[r] = false;
        m_bookIsModified[r] = false;
        m_toolEditFahrass[r] = false;
    }
    m_treeGroupIsModified = false;
    ui->lineEdit->setVisible(false);
    m_listTab.append("0");
    m_findIsTitle = true;
    txtBrowserBook = ui->tabWidget->currentWidget()->findChild<QTextBrowser*>();
    // تحميل ملف السمة وبيانات الطول و العرض ووو
    //******************** عمليات التحميل ***
	loadLayout();
	langLoc = QLocale::system().name().section('_',0,0).toStdString();
	static const unordered_map< int,string > map
	{ {1,langLoc}, {2,"pk"}, {3,"en"}, {4,"fr"} };
	if (auto it = map.find(m_lng); it != map.end())
		setLanguage( QString(MW::pathApp + "translat/kirtasse_%1.qm").arg(it->second) );

    updateIconEndAction();
    tabCreatFahrass();

	if (m_iconsThemes)
        chargeIconsThemes();
    m_listActions = new QList<QAction*>;
    m_listActions->append(ui->menu_file->actions());
    m_listActions->append(ui->menuEdit->actions());
    m_listActions->append(ui->menuView->actions());
    m_listActions->append(ui->menuTools->actions());
    m_listActions->append(ui->menuHelp->actions());
    m_listActions->append(AC_prevHistorie);
    m_listActions->append(AC_nextHistorie);
    toolRubon = new toolrubon();
    addToolRubonBar(m_isStandardToolBar);
    ui->menuBar->setVisible(m_isStandardToolBar);

    // message->m_pathCostum=m_pathCostm;
    message->favorite_charge(ui->treeWidget_fav, m_icondir, m_icondfile);
    message->treeChargeJozaa(ui->treeWidget_curaan); // تحميل اجزاء القرءان
    message->recentCharge(); // تحميل قائمة الكتب السابقة
    chargeGroupe();
    emit bookAvailable(false);
    on_actionPageIntro_triggered();

    ui->actionDownloadBooks->setEnabled(loadPlugin());
    ui->actionShamilaCdrom->setEnabled(loadPluginCdrom());

    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2) {
        if (args.at(1) == "-f")  this->showFullScreen();
    }
    else if (args.count() == 3) {
        QString arg = args.at(1);
        QString argStyle = args.at(2);
        qDebug() << "arg :" << argStyle;
        if (arg.toLower() == "-s") {
            m_isStyleCostum = true;
            m_styleCostum = argStyle;
            creatStyle();
        }
    }
}
// عند خروج البرنامج
MainWindow::~MainWindow() {
    message->recentSave();         // حفظ الكتب السابقة
    if (!this->isFullScreen()) saveLayou();
    message->favorite_save(ui->treeWidget_fav);
}

void MainWindow::setLanguage(const QString &qmFilePath) {
	qDebug() << "Lang File:" <<qmFilePath;
	qApp->removeTranslator(&m_translator);
	if (m_translator.load(qmFilePath)) {
		qApp->installTranslator(&m_translator);
		QEvent ev{QEvent::LanguageChange};	// LanguageChange to all widgets
		for (QWidget *w : qApp->allWidgets())
			QCoreApplication::sendEvent(w, &ev);
	}
}

void MainWindow::closeEvent(QCloseEvent* e) {
    findBook->stopFind = true;
    if (m_treeGroupIsModified == true) {
        int reponse = QMessageBox::warning(
            this, tr("تنبيه"),
            tr("هل تريد حفظ التغييرات المدخلة على مجموعة الكتب "),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reponse == QMessageBox::Save) {
            if (message->treeSaveGroupe(ui->treeWidget_books)) {
                QMessageBox::information(this, tr("معلومات"),
                    tr("تمت عملية حفظ مجموعة الكتب بنجاح"));
                m_treeGroupIsModified = false;
            }
        } else if (reponse == QMessageBox::Cancel) {
            e->ignore();
            return;
        }
    }

    int tabcount = ui->tabWidget->count();
    // حفظ السمة والبيانات عند الخروج
    for (int r = 0; r < tabcount; ++r) {
        QTextBrowser* txtBrowser = ui->tabWidget->widget(r)->findChild<QTextBrowser*>();
        QString name = txtBrowser->objectName();
        QString str = name.section('_', -1);
        QVariant indx = str;
        if (!m_bookName[indx.toInt()].isEmpty()) { // حفظ قائمة الكتب السابقة
            if (txtBrowser->document()->isModified()) {
                databook->boocNumIndex = indx.toInt();
                databook->updatPage("", txtBrowser->document()->toPlainText(), true);
                m_bookIsModified[indx.toInt()] = true;
            }
            if (closeMessages(m_bookTitle[indx.toInt()], indx.toInt()) == false) {
                e->ignore();
                return;
            }
            message->recentChange(m_bookName[indx.toInt()],
                m_bookTitle[indx.toInt()],
                m_bookAuthor[indx.toInt()], QString::number(databook->currentPosition[indx.toInt()]),
                m_recentNbr, m_isTefsir[indx.toInt()]);
        }
    }
    e->accept();
}

bool MainWindow::closeMessages(QString title, int index) {
    if (m_treeFahrasIsModified[index] == true) {
        int reponse = QMessageBox::warning(
            this, tr("تنبيه"), tr("هل تريد حفظ فهرسة : ") + title,
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reponse == QMessageBox::Save) {
            saveFahrass(index);

        } else if (reponse == QMessageBox::Cancel) {

            return false;
        }
    }
    if (m_bookIsModified[index] == true) {
        int reponse = QMessageBox::warning(
            this, tr("تنبيه"),
            tr("هل تريد حفظ التغييرات على كتاب : ") + title + " ؟",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (reponse == QMessageBox::Save) {
            saveCurrentBook(index);

        } else if (reponse == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {

    int b = listLineEditName.indexOf(obj->objectName());

    if (obj->objectName() == listLineEditName.at(b)) {

        if (event->type() == QEvent::FocusIn && obj->property("text") == tr("بحث")) {
            obj->setProperty("text", "");
            obj->setProperty("styleSheet", "");
            return true;
        } else if (event->type() == QEvent::FocusOut && obj->property("text").toString().isEmpty()) {
            obj->setProperty("text", tr("بحث"));
            obj->setProperty("styleSheet",
                "QLineEdit {font: italic;color: rgb(115, 115, 115);}");
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);

    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);

        if (m_layouDir)
        {
            QApplication::setLayoutDirection(Qt::LeftToRight);
            QLocale::setDefault(QLocale(QLocale::Latin));
        }
        else
        {
            QApplication::setLayoutDirection(Qt::RightToLeft);
            QLocale::setDefault(QLocale(QLocale::Arabic));
        }
    }
}

void MainWindow::updateIconEndAction() {
    //*******************menu tool**************
    ui->menuDockTooBar->addAction(ui->dockWidget_books->toggleViewAction());
    ui->dockWidget_books->toggleViewAction()->setShortcut(
        QKeySequence(tr("Ctrl+Shift+b")));
    ui->dockWidget_books->toggleViewAction()->setIcon(
        QIcon(":/images/image/view-books.png"));
    ui->menuDockTooBar->addAction(ui->dockWidget_curaan->toggleViewAction());
    ui->dockWidget_curaan->toggleViewAction()->setShortcut(
        QKeySequence(tr("Ctrl+Shift+c")));
    ui->dockWidget_curaan->toggleViewAction()->setIcon(
        QIcon(":/images/image/view-curan.png"));
    ui->menuDockTooBar->addAction(ui->dockWidget_fahras->toggleViewAction());
    ui->dockWidget_fahras->toggleViewAction()->setShortcut(
        QKeySequence(tr("Ctrl+Shift+l")));
    ui->dockWidget_fahras->toggleViewAction()->setIcon(
        QIcon(":/images/image/view-fahrasse.png"));
    ui->menuDockTooBar->addAction(ui->dockWidget_favorite->toggleViewAction());
    ui->dockWidget_favorite->toggleViewAction()->setShortcut(
        QKeySequence(tr("Ctrl+Shift+v")));
    ui->dockWidget_favorite->toggleViewAction()->setIcon(
        QIcon(":/images/image/view-favo.png"));
    ui->menuDockTooBar->addAction(ui->dockWidget_find->toggleViewAction());
    ui->dockWidget_find->toggleViewAction()->setShortcut(
        QKeySequence(tr("Ctrl+Shift+f")));
    ui->dockWidget_find->toggleViewAction()->setIcon(
        QIcon(":/images/image/view-find.png"));
    ui->menuDockTooBar->addSeparator();
    // ui->menuDockTooBar->addAction(ui->mainToolBar->toggleViewAction());
    //   ui->mainToolBar->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+Shift+t")));
    // ui->menuDockTooBar->addAction(ui->toolBar_navegator->toggleViewAction());
    // ui->toolBar_navegator->toggleViewAction()->setShortcut(QKeySequence(tr("Ctrl+Shift+n")));
    //*******************icons******************

    //********انشاء شجرة البحوث*************
    ui->treeWidget_books->setColumnHidden(2, true);
    ui->treeWidget_books->setColumnWidth(0, 200);
    ui->treeWidget_books->header()->setVisible(true);
    ui->treeWidget_fav->setColumnWidth(0, 300);
    ui->treeWidget_fav->header()->setVisible(true);
    //******اضافات الى شريط المعلومات*******

    ui->action_stopFind->setVisible(false);

    ui->action_stopFind->setIconText(tr("توقيف البحث"));
    connect(ui->action_stopFind, SIGNAL(triggered()), this, SLOT(stopFind()));
    labelProgress = new QLabel;
    ui->statusBar->addWidget(labelProgress);
    progressBarFind = new QProgressBar();
    labelProgressImg = new QLabel;
    ui->statusBar->addWidget(labelProgressImg);
    ui->statusBar->addWidget(progressBarFind);

    labelProgressInfo = new QLabel;
    ui->statusBar->addWidget(labelProgressInfo);
    labelProgress->setVisible(false);
    labelProgressImg->setVisible(false);
    //  BtnStopFind->setVisible(false);
    progressBarFind->setVisible(false);
    ui->frameEdit->setVisible(false);
    ui->widgetEditBook->setVisible(false);
    //************favotite شريط ادوات المفضلة**********
    QLineEdit* lineEditFindFavo = new QLineEdit(this);
    lineEditFindFavo->setMaximumWidth(200);
    ui->toolBarFavo->insertWidget(ui->actionFavSave, lineEditFindFavo);
    ui->toolBarFavo->insertSeparator(ui->actionFavSave);

    ui->horizontalLayoutFavo->addWidget(ui->toolBarFavo);
    //*******شريط ادوات شجرة نتائج البحث

    labelTextFind = new QLabel();
    ui->toolBarFind->insertWidget(ui->actionFindItemUp, labelTextFind);
    ui->toolBarFind->insertSeparator(ui->actionFindItemUp);
    ui->horizontalLayoutFind->addWidget(ui->toolBarFind);
    //******* اضافة القوائم الى شجرة الكتب menu************
    AC_bookOpenInNewTab = new QAction(style()->standardPixmap(QStyle::SP_FileDialogNewFolder),
        tr("فتح في لسان جديد"), this);
    AC_bookOpen = new QAction(style()->standardPixmap(QStyle::SP_DirOpenIcon),
        tr("فتح الكتاب المحدد"), this);
    AC_bookRemove = new QAction(style()->standardPixmap(QStyle::SP_DialogCancelButton),
        tr("حذف الكتاب المحدد"), this);
    AC_bookUpdat = new QAction(tr("تحرير بيانات الكتاب المحدد"), this);
    AC_bookPath = new QAction(tr("مسار الكتاب المحدد"), this);
    AC_bookRename = new QAction(tr("اعادة تسمية دليل الكتاب"), this);
    AC_bookDownload = new QAction(tr(" تحميل الكتاب المحدد"), this);
    AC_groupeRemove = new QAction(style()->standardPixmap(QStyle::SP_DialogCancelButton),
        tr("حذف العنصر المحدد"), this);
    AC_groupeAdd = new QAction(style()->standardPixmap(QStyle::SP_FileDialogNewFolder),
        tr("إظافة مجموعةالى القسم المحدد"), this);
    AC_categorieAdd = new QAction(style()->standardPixmap(QStyle::SP_FileDialogNewFolder),
        tr("إظافة قسم جديد"), this);
    AC_groupeRename = new QAction(tr("اعادة تسمية العنصر المحدد"), this);
    //****اضافات الى شريط الادواتت المؤثرات toolbar****
    labelAnim = new QLabel(ui->toolBarAnim);
    labelAnim->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->toolBarAnim->addSeparator();
    ui->toolBarAnim->addWidget(labelAnim);
    ui->frame_5->layout()->addWidget(ui->toolBarAnim);
    //*******شريط ادوات الرئيسي
    lineEditSearchInDoc = new QLineEdit();
    lineEditSearchInDoc->setMaximumWidth(250);
    AC_GoSearchInDoc = new QAction(QIcon(":/images/image/arrow-left.png"),
        tr("البحث في الصفحة"), this);
    AC_GoSearchInCurBook = new QAction(QIcon(":/images/image/FIN_book.png"),
        tr("البحث في الكتاب الحالي"), this);
    //    ui->mainToolBar->addWidget(lineEditSearchInDoc);
    //    ui->mainToolBar->addAction(AC_GoSearchInDoc);
    //    ui->mainToolBar->addSeparator();
    //    ui->mainToolBar->addAction(AC_GoSearchInCurBook);
    //**القوائم المنسدلة للكتب السابقة combobox recent**
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this,
            SLOT(openRecentFile()));
        ui->menuRecent->addAction(recentFileActs[i]);
        message->recentFileActs[i] = recentFileActs[i];
    }
    //    ui->mainToolBar->addSeparator();
    //    ui->mainToolBar->addAction(ui->menuRecent->menuAction());
    //**********شريط ادوات التنقل بين الصفحات
    labelPagePart = new QLabel("");
    // ui->toolBar_navegator->addWidget(labelPagePart);
    AC_nextHistorie = new QAction(QIcon(":/images/image/undo-next.png"),
        tr("التنقل التالي"), this);
    AC_prevHistorie = new QAction(QIcon(":/images/image/undo-back.png"),
        tr("التنقل السابق"), this);
    AC_prevHistorie->setEnabled(false);
    AC_nextHistorie->setEnabled(false);
    // ui->toolBar_navegator->addAction(AC_prevHistorie);
    // ui->toolBar_navegator->addAction(AC_nextHistorie);
    //***********************الممؤثرات
    // الحركيةanimation*****************************
    //     labelBetaka=new QLabel();
    //     labelBetaka->setLayoutDirection(Qt::RightToLeft);

    //    m_scene.addWidget(labelBetaka);
    //    m_scene.setBackgroundBrush(labelBetaka->palette().base());
    //    labelBetaka->setBackgroundRole(QPalette::Base);
    //    ui->graphicsView->setScene(&m_scene);
    //    labelBetaka->setFixedWidth(400);
    //    labelBetaka->setFixedHeight(400);
    //    labelBetaka->setAlignment(Qt::AlignTop);
    //    labelBetaka->setAutoFillBackground(true);
    //    startAnimationPixmap();
#if QT_VERSION >= 0x040600
    view->treeView = ui->treeWidget_books;
    view->lineEdit = ui->lineEdit;
    view->label = labelAnim;
    view->chargePixmap();
    ui->horizontalLayout_7->addWidget(view);
#endif
    //---------------------------عناصر تحرير الكتاب
    ui->horizontalLayout_5->addWidget(ui->toolBarEditBook);

    /////////////////عناصر شجرة الكتب
    QLineEdit* lineEditFind = new QLineEdit(this);
    ui->toolBarBooks->insertWidget(ui->actionFindBkOrHt, lineEditFind);
    ui->horizontalLayoutBook->addWidget(ui->toolBarBooks);
    //--------------عناصر الفهرسة
    QToolBar* barFahras = new QToolBar;
    barFahras->setIconSize(QSize(18, 18));
    lineEditFindFah = new QLineEdit(this);
    AC_expandTreeFah = new QAction(QIcon(":/images/image/btnfolder.png"),
        tr("توسيع الكل"), this);
    AC_colapsTreeFah = new QAction(QIcon(":/images/image/colapse.png"),
        tr("غلق الكل"), this);
    barFahras->addWidget(lineEditFindFah);
    barFahras->addSeparator();
    barFahras->addAction(AC_expandTreeFah);
    barFahras->addAction(AC_colapsTreeFah);
    ui->horizontalLayoutFahras->addWidget(barFahras);
    ui->horizontalLayoutBookEdit->addWidget(ui->toolBarEditBook_2);
    //**********************************
    // tabWidget
    QToolButton* btnAddtab = new QToolButton(this);
    btnAddtab->setIcon(
        QIcon::fromTheme("tab-new", QIcon(":/images/image/tab-new.png")));
    btnAddtab->setStyleSheet(
        " QToolButton  {  border: 0px solid #8f8f91;}"
        " QToolButton:hover  {  border: 1px solid #8f8f91; border-radius: 3px;}");
    btnAddtab->setToolTip(tr("لسان جديد"));
    connect(btnAddtab, SIGNAL(clicked()), this, SLOT(tabCreatTabNew()));
    ui->tabWidget->setCornerWidget(btnAddtab, Qt::TopLeftCorner);

    //**************whatsthis**************
#ifndef QT_NO_WHATSTHIS
    AC_prevHistorie->setWhatsThis(
        tr("الرجوع الى التنقلات التي قمت بها سابقا وليس الى الصفحة السابقة"));
    AC_nextHistorie->setWhatsThis(
        tr("الرجوع الى التنقلات التالية وليس الى الصفحة التالية"));
    AC_GoSearchInDoc->setWhatsThis(tr(
        "انقر على الزر للبحث في الصفحة الحالية عن النص المدخل في مربع النص"));
    AC_GoSearchInCurBook->setWhatsThis(tr(
        "انقر على الزر للبحث في الكتاب الحالي عن النص المدخل في مربع النص"));
    lineEditFindFah->setWhatsThis(
        tr("ادخل نص البحث هنا ليتم فلترة الفهرسة كلما ادخلت حرفا جديدا"));
    lineEditFind->setWhatsThis(
        tr("ادخل نص البحث هنا ليتم فلترة الفهرسة كلما ادخلت حرفا جديدا"));

#endif // QT_NO_WHATSTHIS
    //***********الاشارات signal end slot****
    connect(AC_bookOpen, SIGNAL(triggered()), this, SLOT(menuOpenBook()));
    connect(AC_bookOpenInNewTab, SIGNAL(triggered()), this,
        SLOT(tabOpenInNewTab()));
    connect(AC_bookRemove, SIGNAL(triggered()), this, SLOT(menu_BookRemove()));
    connect(AC_bookUpdat, SIGNAL(triggered()), this, SLOT(menu_BookUpdat()));
    connect(AC_bookPath, SIGNAL(triggered()), this, SLOT(menu_BookPath()));
    connect(AC_bookRename, SIGNAL(triggered()), this, SLOT(menu_renameBook()));
    connect(AC_bookDownload, SIGNAL(triggered()), this,
        SLOT(menu_downloadBook()));
    connect(AC_groupeRemove, SIGNAL(triggered()), this,
        SLOT(menu_GroupeRemove()));
    connect(AC_groupeAdd, SIGNAL(triggered()), this, SLOT(menu_GroupeAdd()));
    connect(AC_categorieAdd, SIGNAL(triggered()), this,
        SLOT(menu_CategorieAdd()));
    connect(AC_groupeRename, SIGNAL(triggered()), this,
        SLOT(menu_GroupeRename()));
    connect(AC_GoSearchInDoc, SIGNAL(triggered()), this, SLOT(searchIndoc()));
    connect(AC_GoSearchInCurBook, SIGNAL(triggered()), this,
        SLOT(findInCurrentBook()));
    connect(lineEditSearchInDoc, SIGNAL(returnPressed()), this,
        SLOT(searchIndoc()));
    connect(txtBrowserBook, SIGNAL(highlighted(const QUrl)), this,
        SLOT(textBookHighlighted(QUrl)));
    connect(txtBrowserBook, SIGNAL(anchorClicked(const QUrl)), this,
        SLOT(textBookAnchorClicked(QUrl)));
    connect(AC_nextHistorie, SIGNAL(triggered()), this, SLOT(historieNext()));
    connect(AC_prevHistorie, SIGNAL(triggered()), this, SLOT(historiePrev()));
    connect(ui->actionItemDown, SIGNAL(triggered()), this,
        SLOT(itemFahrasMoveDown()));
    connect(ui->actionItemFahUp, SIGNAL(triggered()), this,
        SLOT(itemFahrasMoveUp()));
    connect(ui->actionItemFahRen, SIGNAL(triggered()), this,
        SLOT(itemFahrasRename()));
    connect(ui->actionItemFahDelet, SIGNAL(triggered()), this,
        SLOT(itemFahrasRemove()));
    connect(ui->actionFahAddItem, SIGNAL(triggered()), this,
        SLOT(itemFahrasAdd()));
    connect(ui->actionFahAddChild, SIGNAL(triggered()), this,
        SLOT(itemFahrasAddChild()));
    connect(ui->actionSaveEdit, SIGNAL(triggered()), this,
        SLOT(saveBookEndFahras()));
    connect(lineEditFind, SIGNAL(textChanged(QString)), this,
        SLOT(searchInTreeBook(QString)));
    connect(ui->actionBookExpAll, SIGNAL(triggered()), ui->treeWidget_books,
        SLOT(expandAll()));
    connect(ui->actionBookColpAll, SIGNAL(triggered()), ui->treeWidget_books,
        SLOT(collapseAll()));
    connect(ui->actionFindBkOrHt, SIGNAL(triggered()), this,
        SLOT(findBookOrHautor()));
    connect(ui->treeWidgetFind, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
        this, SLOT(showResultFind(QTreeWidgetItem*)));
    connect(lineEditFindFavo, SIGNAL(textChanged(QString)), this,
        SLOT(find_favo_textChanged(QString)));
    //****************

    connect(this, SIGNAL(bookAvailable(bool)), ui->action_next,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->action_prev,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->action_first,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->action_last,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->action_edit,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->action_favorit,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionGotoPage,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionOrganizeFahrasse,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionBookInfo,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionHiperlink,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), AC_GoSearchInCurBook,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionArchive,
        SLOT(setEnabled(bool)));
    connect(this, SIGNAL(bookAvailable(bool)), ui->actionExport,
        SLOT(setEnabled(bool)));
    // connect(this, SIGNAL(bookAvailable(bool)),AC_GoSearchInDoc,
    // SLOT(setEnabled(bool)));
    listLineEditName << lineEditSearchInDoc->objectName()
                     << ui->lineEdit_find_cur->objectName()
                     << ui->lineEdit_FindRowat->objectName()
                     << lineEditFindFah->objectName()
                     << lineEditFindFavo->objectName()
                     << lineEditFind->objectName();
    lineEditSearchInDoc->installEventFilter(this);
    lineEditSearchInDoc->setText(tr("بحث"));
    lineEditSearchInDoc->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");

    ui->lineEdit_find_cur->installEventFilter(this);
    ui->lineEdit_find_cur->setText(tr("بحث"));
    ui->lineEdit_find_cur->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");

    ui->lineEdit_FindRowat->installEventFilter(this);
    ui->lineEdit_FindRowat->setText(tr("بحث"));
    ui->lineEdit_FindRowat->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");

    lineEditFindFah->installEventFilter(this);
    lineEditFindFah->setText(tr("بحث"));
    lineEditFindFah->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");

    lineEditFindFavo->installEventFilter(this);
    lineEditFindFavo->setText(tr("بحث"));
    lineEditFindFavo->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");

    lineEditFind->installEventFilter(this);
    lineEditFind->setText(tr("بحث"));
    lineEditFind->setStyleSheet(
        "QLineEdit {font: italic;color: rgb(115, 115, 115);}");
    //***********

    //-------***************icons--------------------
    m_icondfile.addPixmap(QPixmap(style()->standardPixmap(QStyle::SP_FileIcon)),
        QIcon::Normal, QIcon::Off);
    m_icondir.addPixmap(QPixmap(style()->standardPixmap(QStyle::SP_DirIcon)),
        QIcon::Normal, QIcon::Off);
    m_iconGroup.addPixmap(QPixmap(QString::fromUtf8(":/images/image/groupb.png")),
        QIcon::Normal, QIcon::Off);
    ui->btnRowaInfo->setIcon(
        style()->standardPixmap(QStyle::SP_MessageBoxInformation));
    ui->action_stopFind->setIcon(style()->standardPixmap(QStyle::SP_MediaStop));

    ui->actionFavItemDown->setIcon(style()->standardPixmap(QStyle::SP_ArrowDown));
    ui->actionFavItemUp->setIcon(style()->standardPixmap(QStyle::SP_ArrowUp));
    ui->actionFindItemDown->setIcon(
        style()->standardPixmap(QStyle::SP_ArrowDown));
    ui->actionFindItemUp->setIcon(style()->standardPixmap(QStyle::SP_ArrowUp));
    ui->actionFindLoad->setIcon(style()->standardPixmap(QStyle::SP_DirOpenIcon));
    //    labelBetaka->setPixmap(QPixmap(QString::fromUtf8(":/images/image/groupbook.png")));
    ui->actionItemFahUp->setIcon(style()->standardPixmap(QStyle::SP_ArrowUp));
    ui->actionItemDown->setIcon(style()->standardPixmap(QStyle::SP_ArrowDown));
    // m_icondir= QIcon(style()->standardPixmap(QStyle::SP_DirIcon));
    ui->actionBookInfo->setIcon(
        style()->standardPixmap(QStyle::SP_MessageBoxInformation));
    if (this->layoutDirection() == Qt::LeftToRight) {
        ui->action_first->setIcon(QIcon(":/images/image/go-last.png"));
        ui->action_prev->setIcon(QIcon(":/images/image/go-next.png"));
        ui->action_next->setIcon(QIcon(":/images/image/go-back.png"));
        ui->action_last->setIcon(QIcon(":/images/image/go-first.png"));
    }
}

void MainWindow::addToolRubonBar(bool rubon) {

    this->addToolBar(Qt::TopToolBarArea, toolRubon);
    toolRubon->setToolIconSize(m_toolIconSize, m_textUnderIcon);
    if (rubon == true) {
        toolRubon->clear();
        int count = m_listActToAdd.count();
        for (int i = 0; i < count; i++) {
            int indx = m_listActToAdd.at(i).toInt();
            if (indx == 1000) {
                toolRubon->addSeparator();
            } else if (indx == 2000) {
                toolRubon->addSeparator();
                toolRubon->addWidget(lineEditSearchInDoc);
                toolRubon->addAction(AC_GoSearchInDoc);
                toolRubon->addAction(AC_GoSearchInCurBook);
                toolRubon->addAction(ui->actionFind_all);

            } else {
                toolRubon->addAction(m_listActions->at(indx));
            }
        }
        /*
        toolRubon->addAction(ui->actionOption);
        toolRubon->addAction(ui->action_fulscreen);
        toolRubon->addAction(ui->menuRecent->menuAction());
        toolRubon->addSeparator();
        toolRubon->addActions(ui->menuEdit->actions());
        toolRubon->addWidget(labelPagePart);
        toolRubon->addAction(AC_prevHistorie);
        toolRubon->addAction(AC_nextHistorie);
        toolRubon->removeAction(ui->actionFind_all);
        toolRubon->addSeparator();
        toolRubon->addWidget(lineEditSearchInDoc);
        toolRubon->addAction(AC_GoSearchInDoc);
        toolRubon->addAction(AC_GoSearchInCurBook);
        toolRubon->addAction(ui->actionFind_all);
        */
        return;
    }

    toolRubon->setRubonStyle();

    toolRubon->toolFile->addActions(ui->menu_file->actions());
    //  toolRubon->toolView->addActions(ui->menuView->actions());
    QWidget* widget = new QWidget;
    QHBoxLayout* horizontalLayoutTool = new QHBoxLayout(widget);
    QSpacerItem* horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayoutTool->addItem(horizontalSpacer);
    toolRubon->toolFile->insertWidget(ui->action_close, widget);

    ui->action_last->setIcon(QIcon(":/images/image/go-first.png"));

    toolRubon->toolEdit->addActions(ui->menuEdit->actions());
    //    toolRubon->toolEdit->addWidget(labelPagePart);
    toolRubon->toolEdit->addSeparator();
    toolRubon->toolEdit->addAction(AC_prevHistorie);
    toolRubon->toolEdit->addAction(AC_nextHistorie);
    toolRubon->toolEdit->addSeparator();
    toolRubon->toolEdit->addAction(ui->actionZoumIn);
    toolRubon->toolEdit->addAction(ui->actionZoomOut);
    toolRubon->toolEdit->removeAction(ui->actionFind_all);

    toolRubon->tooTools->addAction(ui->actionOption);
    toolRubon->tooTools->addAction(ui->actionControle);
    toolRubon->tooTools->addSeparator();
    toolRubon->tooTools->addActions(ui->menuTools->actions());

    toolRubon->toolFind->addAction(ui->actionFind_all);
    toolRubon->toolFind->addSeparator();
    toolRubon->toolFind->addWidget(lineEditSearchInDoc);
    toolRubon->toolFind->addAction(AC_GoSearchInDoc);
    toolRubon->toolFind->addSeparator();
    toolRubon->toolFind->addAction(AC_GoSearchInCurBook);
    toolRubon->toolView->addAction(ui->action_fulscreen);
    toolRubon->toolView->addSeparator();
    toolRubon->toolView->addActions(ui->menuDockTooBar->actions());

    toolRubon->toolHelp->addActions(ui->menuHelp->actions());
}

void MainWindow::chargeIconsThemes() {
    ui->pushButtonEditClose->setIcon(QIcon::fromTheme(
        "edit-delete",
        QIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton))));
    ui->actionNewPageAfter->setIcon(QIcon::fromTheme(
        "document-new", QIcon(style()->standardPixmap(QStyle::SP_FileIcon))));
    ui->actionNewPageBefor->setIcon(QIcon::fromTheme(
        "document-new", QIcon(style()->standardPixmap(QStyle::SP_FileIcon))));
    AC_nextHistorie->setIcon(
        QIcon::fromTheme("edit-undo", QIcon(":/images/image/undo-back.png")));
    AC_prevHistorie->setIcon(
        QIcon::fromTheme("edit-redo", QIcon(":/images/image/undo-next.png")));
    AC_GoSearchInDoc->setIcon(
        QIcon::fromTheme("dialog-ok", QIcon(":/images/image/arrow-left.png")));
    AC_groupeRename->setIcon(
        QIcon::fromTheme("edit-rename", QIcon(":/images/image/edit-rename.png")));
    AC_GoSearchInCurBook->setIcon(
        QIcon::fromTheme("edit-find", QIcon(":/images/image/FIN_book.png")));
    AC_expandTreeFah->setIcon(QIcon::fromTheme(
        "view-sort-ascending", QIcon(":/images/image/btnfolder.png")));
    AC_colapsTreeFah->setIcon(QIcon::fromTheme(
        "view-sort-descending", QIcon(":/images/image/colapse.png")));
    ui->actionBookExpAll->setIcon(QIcon::fromTheme(
        "view-sort-ascending", QIcon(":/images/image/btnfolder.png")));
    ui->actionBookColpAll->setIcon(QIcon::fromTheme(
        "view-sort-descending", QIcon(":/images/image/colapse.png")));
    ui->action_fulscreen->setIcon(QIcon::fromTheme(
        "view-fullscreen", QIcon(":/images/image/view-fullscreen.png")));
    ui->actionOption->setIcon(QIcon::fromTheme(
        "document-properties", QIcon(":/images/image/config-background.png")));
    ui->actionHiperlink->setIcon(
        QIcon::fromTheme("insert-link", QIcon(":/images/image/insert-link.png")));
    ui->action_favorit->setIcon(QIcon::fromTheme(
        "bookmark-new", QIcon(":/images/image/bookmark-new.png")));
    ui->actionNewTab->setIcon(
        QIcon::fromTheme("tab-new", QIcon(":/images/image/tab-new.png")));
    ui->action_edit->setIcon(
        QIcon::fromTheme("document-edit", QIcon(":/images/image/bookEdit.png")));
    ui->menuRecent->setIcon(QIcon::fromTheme("document-open-recent",
        QIcon(":/images/image/recent.png")));
    if (this->layoutDirection() == Qt::RightToLeft) {
        ui->action_first->setIcon(
            QIcon::fromTheme("go-last", QIcon(":/images/image/go-first.png")));
        ui->action_prev->setIcon(
            QIcon::fromTheme("go-next", QIcon(":/images/image/go-back.png")));
        ui->action_next->setIcon(
            QIcon::fromTheme("go-previous", QIcon(":/images/image/go-next.png")));
        ui->action_last->setIcon(
            QIcon::fromTheme("go-first", QIcon(":/images/image/go-last.png")));

    } else {
        ui->action_first->setIcon(
            QIcon::fromTheme("go-first", QIcon(":/images/image/go-last.png")));
        ui->action_prev->setIcon(
            QIcon::fromTheme("go-previous", QIcon(":/images/image/go-next.png")));
        ui->action_next->setIcon(
            QIcon::fromTheme("go-next", QIcon(":/images/image/go-back.png")));
        ui->action_last->setIcon(
            QIcon::fromTheme("go-last", QIcon(":/images/image/go-first.png")));
    }

    ui->actionGotoPage->setIcon(
        QIcon::fromTheme("go-top", QIcon(":/images/image/go-page.png")));
    ui->action_import->setIcon(QIcon::fromTheme(
        "stock_new-text", QIcon(":/images/image/address-book-new.png")));
    ui->actionMdb->setIcon(QIcon::fromTheme(
        "stock_new-dir", QIcon(":/images/image/address-book-new.png")));
    ui->action_addBoook->setIcon(QIcon::fromTheme(
        "stock_new-dir", QIcon(":/images/image/address-book-new.png")));
    ui->actionArchive->setIcon(QIcon::fromTheme(
        "add-folder-to-archive", QIcon(":/images/image/tar-gz.png")));
    ui->actionExport->setIcon(QIcon::fromTheme("document-export"));
    ui->actionPrint->setIcon(QIcon::fromTheme("document-print"));
    ui->actionPrintPreview->setIcon(QIcon::fromTheme("gtk-print-preview"));
    ui->actionZoumIn->setIcon(QIcon::fromTheme("zoom-in"));
    ui->actionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    ui->btnGoFindFah->setIcon(
        QIcon::fromTheme("dialog-ok", QIcon(":/images/image/arrow-left.png")));
    ui->actionFontCostum->setIcon(QIcon::fromTheme(
        "format-text-underline", QIcon(":/images/image/draw-text.png")));
    ui->actionBookInfo->setIcon(QIcon::fromTheme(
        "documentinfo",
        QIcon(style()->standardPixmap(QStyle::SP_MessageBoxInformation))));
}

bool MainWindow::loadPlugin() {
    QDir pluginsDir( MW::pathApp);
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    if (!QFile::exists( MW::pathApp + "plugins")) {
        if (QFile::exists("/usr/lib/elkirtasse")) {
            pluginsDir.setPath("/usr/lib/elkirtasse");
        } else if (QFile::exists("/usr/lib64/elkirtasse")) {
            pluginsDir.setPath("/usr/lib64/elkirtasse");
        }
    }

    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject* plugin = pluginLoader.instance();

        if (plugin) {

            netInterface = qobject_cast<NetInterface*>(plugin);

            if (netInterface) {
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::loadPluginCdrom() {
    QDir pluginsDir( MW::pathApp);
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    if (!QFile::exists( MW::pathApp + "plugins")) {
        if (QFile::exists("/usr/lib/elkirtasse")) {
            pluginsDir.setPath("/usr/lib/elkirtasse");
        } else if (QFile::exists("/usr/lib64/elkirtasse")) {
            pluginsDir.setPath("/usr/lib64/elkirtasse");
        }
    }
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject* plugin = pluginLoader.instance();

        if (plugin) {

            shamilaInterface = qobject_cast<ShamilaInterface*>(plugin);

            if (shamilaInterface) {
                return true;
            }
        }
    }

    return false;
}

bool MainWindow::loadPluginRowat() {
    QDir pluginsDir( MW::pathApp);
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    if (!QFile::exists( MW::pathApp + "plugins")) {
        if (QFile::exists("/usr/lib/elkirtasse")) {
            pluginsDir.setPath("/usr/lib/elkirtasse");
        } else if (QFile::exists("/usr/lib64/elkirtasse")) {
            pluginsDir.setPath("/usr/lib64/elkirtasse");
        }
    }
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject* plugin = pluginLoader.instance();

        if (plugin) {

            rowatInterface = qobject_cast<RowatInterface*>(plugin);

            if (rowatInterface) {
                return true;
            }
        }
    }
    return false;
}

// open book****فتح الكتب*****************
void MainWindow::open_dataBase(QString name, QString title, QString Autor,
    int position) // فتح الكتاب
{
    // اذا كان نفس الكتاب مفتوحا
    int tabcount = ui->tabWidget->count();
    for (int r = 0; r < tabcount; ++r) {
        QTextBrowser* txtBrowser = ui->tabWidget->widget(r)->findChild<QTextBrowser*>();
        QString txtname = txtBrowser->objectName();
        QString str = txtname.section('_', -1);
        QVariant indx = str;
        if (m_bookName[indx.toInt()] == name) {

            ui->tabWidget->setCurrentIndex(r);
            return;
        }
    }
    // حفظ التغييرات اذا كان هناك تغيير
    if (txtBrowserBook->document()->isModified()) {
        databook->updatPage("", txtBrowserBook->document()->toPlainText(), true);
        m_bookIsModified[m_currentIndex] = true;
    }

    if (closeMessages(m_bookTitle[m_currentIndex], m_currentIndex) == false) {

        return;
    }
    // التأكد من وجود الكتاب في المسار
    QString bkpath; // مسار الكتاب
    QString idparent = name.section("_", 0, 0);
    idparent = idparent.remove("bk");
    QFile file;
    if (file.exists(m_pathCostm + "/" + name + "/book.xml")) {
        if (m_currentIndex == 0) {
            tabCreatTabNew();
        }
        bkpath = m_pathCostm + "/" + name + "/book.xml";
        m_bookPath[m_currentIndex] = m_pathCostm + "/" + name;
    } else if (file.exists(m_pathCostm + "/" + idparent + "/" + name + "/book.xml")) {

        if (m_currentIndex == 0) {
            tabCreatTabNew();
        }
        bkpath = m_pathCostm + "/" + idparent + "/" + name + "/book.xml";
        m_bookPath[m_currentIndex] = m_pathCostm + "/" + idparent + "/" + name;

    } else if (file.exists( MW::pathApp + "books/" + name + "/book.xml")) {
        if (m_currentIndex == 0) {
            tabCreatTabNew();
        }
        bkpath = MW::pathApp + "books/" + name + "/book.xml";
        m_bookPath[m_currentIndex] = MW::pathApp + "books/" + name;

    } else {
        // QMessageBox::information(this,tr("خطأ"),m_pathCostm + "/" + name
        // +tr( "غير موجود ") );

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("  الكتب غير موجود في هذا الدليل \n ") + m_pathCostm + "/" + name);
        msgBox.setInformativeText(tr("هل تريد تحميله من الانترنت ؟  "));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {

        case QMessageBox::Yes:
            menu_downloadBook();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
        return;
    }
    //***********************************
    file.close();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // حفظ الكتاب السابق
    if (!m_bookName[m_currentIndex].isEmpty()) {
        QVariant idbk = databook->currentPosition[databook->boocNumIndex];
        message->recentChange(m_bookName[m_currentIndex],
            m_bookTitle[m_currentIndex],
            m_bookAuthor[m_currentIndex], idbk.toString(),
            m_recentNbr, m_isTefsir[m_currentIndex]);
        message->recentCharge();
    }
    // فتح الكتاب
    if (databook->openBook(name, position, bkpath) == true) {
        m_bookName[m_currentIndex] = name;
        m_bookTitle[m_currentIndex] = title;
        m_bookAuthor[m_currentIndex] = Autor;
        bookValueChanged(position);
        m_isTefsir[m_currentIndex] = databook->isTefsir;

        emit bookAvailable(true);
    } else {
        QMessageBox::information(this, tr("خطأ"),
            bkpath + tr("التنسيق غير صحيح "));
        QApplication::restoreOverrideCursor();
        return;
    }
    // تحميل الشجرة الكتاب
    message->treeChargeFahrass(treeViewFahras[m_currentIndex],
        m_bookPath[m_currentIndex]);
    // استعادة اصل المتغيرات
    m_listHistoriePrev[m_currentIndex].clear();
    m_listHistorieNext[m_currentIndex].clear();
    m_listHistoriePrev[m_currentIndex].append(position);
    AC_prevHistorie->setEnabled(false);
    AC_nextHistorie->setEnabled(false);
    ui->dockWidget_fahras->setVisible(true);
    treeViewFahras[m_currentIndex]->headerItem()->setText(
        0, m_bookTitle[m_currentIndex]);
    treeViewFahras[m_currentIndex]->setColumnCount(2);
    treeViewFahras[m_currentIndex]->setColumnHidden(1, true);
    treeViewFahras[m_currentIndex]->setDragDropMode(
        QAbstractItemView::NoDragDrop);
    treeViewFahras[m_currentIndex]->setSelectionMode(
        QAbstractItemView::SingleSelection);
    m_treeFahrasIsModified[m_currentIndex] = false;
    m_bookIsModified[m_currentIndex] = false;
    ui->frameEdit->setVisible(false);
    ui->widgetEditBook->setVisible(false);
    m_toolEditFahrass[m_currentIndex] = false;
    txtBrowserBook->setReadOnly(true);
    txtBrowserBook->setUndoRedoEnabled(false);
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
        m_bookTitle[m_currentIndex]);
    this->tabifyDockWidget(ui->dockWidget_books, ui->dockWidget_fahras);
    if (message->loadBookInfo(m_bookPath[m_currentIndex]) == true) {
        m_bookBetaka[m_currentIndex] = message->infoBookBetaka;
        ui->actionBookInfo->setVisible(true);
    }

    QApplication::restoreOverrideCursor();
}

// menufile----------------------------
void MainWindow::on_action_import_triggered() // استراد ملفات نصية
{
    dialogconvert* dlgc = new dialogconvert(this);
    dlgc->m_pathCostum = m_pathCostm;
    if (dlgc->exec() == QDialog::Accepted) {
        chargeGroupe();
    }
    delete dlgc;
}

void MainWindow::on_actionMdb_triggered() {
    // #ifdef   Q_WS_WIN
    //     Dialogmdb *dlg=new Dialogmdb(this);
    //     dlg->m_path=m_pathCostm;
    //     if ( dlg->exec() == QDialog::Accepted ){
    //         chargeGroupe();
    //     }
    // #else
    //     mdbexport *dlg=new mdbexport(this);
    //     dlg->m_path=m_pathCostm;
    //     if ( dlg->exec() == QDialog::Accepted ){
    //         chargeGroupe();
    //     }
    // #endif
    //     delete dlg;
}

void MainWindow::on_action_addBoook_triggered() // اظافة كتاب
{
    Dialog* dlg = new Dialog(this);
    dlg->m_path = m_pathCostm;
    if (dlg->exec() == QDialog::Accepted) {
        chargeGroupe();
    }
    delete dlg;
}

void MainWindow::on_actionExport_triggered() // تصدير الكتاب
{
    if (m_bookName[m_currentIndex].isEmpty()) {
        return;
    }
    Print* dlg = new Print(this);
    dlg->treeviw = treeViewFahras[m_currentIndex];
    dlg->setpathApp( MW::pathApp);
    dlg->setBookCurentPath(m_bookPath[m_currentIndex]);
    dlg->setBookColorBack(m_WebColorBack); // لون خلفية النص
    dlg->setBookFontTitle(m_WebFontTitle);
    dlg->setBookFont(m_WebFont); // نوع النص
    dlg->setBookFontColor(m_WebFontColor);
    dlg->setBookFontTitleColor(m_WebFontTitleColor);
    dlg->setBookFontPrefertColor(m_WebFontPrefertColor);
    dlg->setBookTitle(m_bookTitle[m_currentIndex]);
    dlg->setBookAuthor(m_bookAuthor[m_currentIndex]);
    dlg->setBookBetaka(m_bookBetaka[m_currentIndex]);
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionArchive_triggered() {
    if (m_bookName[m_currentIndex].isEmpty())
        return;

    QString fileName = m_bookPath[m_currentIndex];
    QDir dirF(fileName);
    if (!dirF.exists()) {
        QMessageBox::information(this, tr("معلومات"),
            tr("لا يوجد كتاب لارشفته"));
        return;
    }
    QString dirParent = fileName;
    QString dirName = dirF.dirName();
    dirParent.replace(dirName, "");
    dirParent.replace("//", "");

    QString title = m_bookTitle[m_currentIndex];
    title.replace(" ", "_");
    QProcess prosses;
    QProgressDialog progress(tr("الرجاء الانتظار...."), tr("الغاء"), 0, 0,
        0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setEnabled(false);
    progress.show();
    qApp->processEvents();
#ifdef Q_WS_WIN
    prosses.setWorkingDirectory(QApplication::applicationDirPath());
    QString setTar = "\"" + QDir::homePath() + "/" + title + ".tar\"";
    QString setGz = "\"" + QDir::homePath() + "/" + title + ".tar.gz\"";

    prosses.start("7z a " + setTar + " \"" + dirParent + dirName + "/\"");
    while (prosses.state() == QProcess::Running) {
        qApp->processEvents();
        progress.setValue(progress.value() + 1);
    }
    prosses.start("7z a " + setGz + " " + setTar);
    while (prosses.state() == QProcess::Running) {
        qApp->processEvents();
        progress.setValue(progress.value() + 1);
    }
    QDir mydir(QDir::homePath());
    mydir.remove(title + ".tar");
#else
    QDir::setCurrent(dirParent);
    prosses.execute("tar -cvzf " + QDir::homePath() + "/" + title + ".tar.gz " + dirName);
    while (prosses.state() == QProcess::Running) {
        qApp->processEvents();
        progress.setValue(progress.value() + 1);
    }
#endif
    QMessageBox::information(this, tr("معلومات"),
        tr("لقد تم انشاء الارشيف بنجاح") + "\n" + QDir::homePath() + "/" + title + ".tar.gz");
}

void MainWindow::on_actionBookInfo_triggered() {
    if (m_bookName[m_currentIndex].isEmpty())
        return;

    QString imgScreenPath = m_pathCostm + "/" + m_bookName[m_currentIndex] + "/screenshot.png";
    if (!QFile::exists(imgScreenPath)) {
        imgScreenPath = ":/images/image/groupbook.png";
    }
    QPixmap pixmapG(imgScreenPath);
    QMessageBox msgBox(this);
    msgBox.setLayoutDirection(Qt::RightToLeft);
    msgBox.setIconPixmap(pixmapG);
    msgBox.setWindowTitle(tr("معلومات عن الكتاب"));
    QString tit = " <span style='font-family:" + m_WebFontTitle + ";  font-size:14pt; color:" + m_WebFontTitleColor + ";'>" + m_bookTitle[m_currentIndex] + "</span>";
    msgBox.setText(tit);
    msgBox.setInformativeText(tr("المؤلف : ") + m_bookAuthor[m_currentIndex] + "\n" + m_bookBetaka[m_currentIndex] + "\n المسار : " + m_bookPath[m_currentIndex]);
    msgBox.exec();
}

void MainWindow::on_actionNewTab_triggered() { tabCreatTabNew(); }

void MainWindow::on_actionPrint_triggered() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog* dlg = new QPrintDialog(&printer, this);
    if (txtBrowserBook->textCursor().hasSelection())
        dlg->setOptions(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        txtBrowserBook->print(&printer);
    }
    delete dlg;
#endif
}

void MainWindow::on_actionPrintPreview_triggered() {
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
        SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void MainWindow::on_action_close_triggered() {
    // qApp->quit();
    if (this->close() == true)
        qApp->quit();
}

// menuEdit----------------------------

void MainWindow::on_action_first_triggered() // السجل الاول
{
    bookValueChanged(0);
}

void MainWindow::on_action_next_triggered() // السجل التالي
{

    if (databook->currentPosition[m_currentIndex] == databook->rowsCount[m_currentIndex])
        return;
    bookValueChanged(databook->currentPosition[m_currentIndex] + 1);
}

void MainWindow::on_action_prev_triggered() // السجل السابق
{
    if (! databook->currentPosition[m_currentIndex])	return;
    bookValueChanged(databook->currentPosition[m_currentIndex] - 1);
}

void MainWindow::on_action_last_triggered() // السجل الاخير
{
    bookValueChanged(databook->rowsCount[m_currentIndex]);
}
void MainWindow::on_actionGotoPage_triggered() {
    if (m_bookName[m_currentIndex].isEmpty()) {
        return;
    }
    dialoggoto* dlggo = new dialoggoto(this);
    dlggo->pathBook = m_bookPath[m_currentIndex] + "/book.xml";
    dlggo->rowCount = databook->rowsCount[m_currentIndex];
    dlggo->counter = databook->currentPosition[m_currentIndex];
    dlggo->updateValue();
    if (dlggo->exec() == QDialog::Accepted) {
        int id = dlggo->counter - 1;
        bookValueChanged(id);
    }
    delete dlggo;
}

void MainWindow::on_actionFind_all_triggered() // تنفيذ البحث في مجموعة الكتب
{
    if (findBook->isFindRuned)	return;
    Dialogfind* dlg = new Dialogfind(this);
    dlg->bookPath = m_pathCostm;
    if (dlg->exec() == QDialog::Accepted) {
        m_textfind = dlg->findString;
        if (m_textfind.length() < 3)
            return;
        showfind(false);

        labelTextFind->setText(tr("نص البحث  : ") + m_textfind);
        labelProgress->setVisible(true);
        labelProgressImg->setVisible(true);
        //  BtnStopFind->setVisible(true);
        ui->action_stopFind->setVisible(true);
        findBook->pathCostum = m_pathCostm;
        findBook->findFirst = dlg->findFirst;
        findBook->findToList = dlg->findList;
        findBook->findMultiFind = dlg->MultiFind;
        findBook->findAnd = dlg->findAnd;
        findBook->findSawabik = dlg->findSawabik;
        findBook->findText = m_textfind;
        findBook->resultCount = 0;
        findBook->progressBar1 = progressBarFind;
        //  findBook->pushButtonStop=BtnStopFind;
        findBook->buttonStop = ui->action_stopFind;
        findBook->treeView = ui->treeWidgetFind;
        findBook->labelProgress = labelProgress;
        findBook->label_progressImg = labelProgressImg;
        findBook->isNassToFind = dlg->findIsNass;
        findBook->noHamza = dlg->noHamza;
        this->isNassToFind = dlg->findIsNass;
        // m_isRawiToFind=false;
        ui->treeWidgetFind->clear();
        findBook->findInAllBook(dlg->findIsNass);
    }
    delete dlg;
}

void MainWindow::on_action_edit_triggered() // تحرير الفهرسة
{
    if (!m_bookName[m_currentIndex].isEmpty()) {
        txtBrowserBook->setReadOnly(false);
        txtBrowserBook->setUndoRedoEnabled(true);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
            tr("تحرير : ") + m_bookTitle[m_currentIndex]);
        ui->frameEdit->setVisible(true); // اظهار ادوات التحرير
        ui->widgetEditBook->setVisible(true);
        treeViewFahras[m_currentIndex]->setDragDropMode(
            QAbstractItemView::InternalMove); // تفعيل عملية النقل بالفأرة
        treeViewFahras[m_currentIndex]->setSelectionMode(
            QAbstractItemView::ExtendedSelection);
        m_toolEditFahrass[m_currentIndex] = true;
    }
}

void MainWindow::on_action_favorit_triggered() {
    if (m_bookName[m_currentIndex].isEmpty())
        return;
    dialogfavo* dlg = new dialogfavo(this);
    dlg->iconFolder = m_icondir;
    dlg->iconFile = m_icondfile;
    dlg->view = ui->treeWidget_fav;
    dlg->bookTitle = m_bookTitle[m_currentIndex];
    dlg->bookName = m_bookName[m_currentIndex];
    dlg->bookAut = m_bookAuthor[m_currentIndex];
    QVariant idbk = databook->currentPosition[databook->boocNumIndex];
    dlg->bookId = idbk.toString();
    dlg->charge_combo();
    dlg->exec();
    delete dlg;
}

void MainWindow::on_actionHiperlink_triggered() {
    if (txtBrowserBook->textCursor().selectedText().isEmpty()) {
        return;
    }
    QString oldtxt = txtBrowserBook->textCursor().selectedText();

    Wizard = new ClassWizard(this);
    Wizard->setWizardStyle(QWizard::ModernStyle);
    if (Wizard->exec() == QDialog::Accepted) {
        // QMessageBox::information(this,"", Wizard->urlTexte);
        QString urltext = "<a href='" + Wizard->urlTexte + "'>" + oldtxt + "</a>";
        txtBrowserBook->textCursor().insertHtml(urltext);
        databook->updatPage(oldtxt, urltext, false);
        m_bookIsModified[m_currentIndex] = true;
        txtBrowserBook->document()->setModified(false);
    }
}

void MainWindow::on_actionOrganizeFahrasse_triggered() {
    databook->treeOrganizFahrass(treeViewFahras[m_currentIndex],
        m_bookPath[m_currentIndex]);
    m_treeFahrasIsModified[m_currentIndex] = true;
}

void MainWindow::on_actionUpdatGroupRemove_triggered() {
    message->m_pathCostum = m_pathCostm;
    message->treeUpdateGroupe(ui->treeWidget_books, true);
    m_treeGroupIsModified = true;
}

void MainWindow::on_actionUpdatGroup_triggered() {
    message->m_pathCostum = m_pathCostm;
    message->treeUpdateGroupe(ui->treeWidget_books, false);
}
// menuView-----------------------------

void MainWindow::on_action_fulscreen_triggered() // ملأ الشاشة
{
    if (this->windowState() == Qt::WindowFullScreen) {
        ui->statusBar->setVisible(true);
        ui->menuBar->setParent(this);
        this->setMenuBar(ui->menuBar);
        ui->menuBar->setStyleSheet("");
        ui->menuBar->setVisible(m_isStandardToolBar);

        loadLayout();

        this->showNormal();
    } else {
        saveLayou();
        ui->dockWidget_books->setVisible(false);
        ui->dockWidget_curaan->setVisible(false);
        ui->dockWidget_fahras->setVisible(false);
        ui->dockWidget_favorite->setVisible(false);
        ui->dockWidget_find->setVisible(false);
        toolRubon->setVisible(false);
        //  ui->mainToolBar->setVisible(false);
        ui->statusBar->setVisible(false);

        ui->menuBar->setParent(ui->tabWidget);
        ui->menuBar->setVisible(true);
        ui->menuBar->setStyleSheet(
            "QMenuBar {background-color: rgba(255, 255, 255, 0);}");
        ui->menuBar->setGeometry(0, 0, 23, 23);

        this->showFullScreen();
    }
}

void MainWindow::on_actionOption_triggered() {
	Dialogoption dlg(this);
    dlg.setColorBack(m_WebColorBack);
    dlg.setColorFont(m_WebFontColor);
    dlg.setColorFind(m_WebFontFindColor);
    dlg.setColorPrefer(m_WebFontPrefertColor);
    dlg.setcolorTitle(m_WebFontTitleColor);
    dlg.setRecentNbr(m_recentNbr);
    dlg.setPathCostm(m_pathCostm);
    dlg.setWebCadre(m_isCadre);
    QFont font;
    font.fromString(m_WebFont);
    dlg.setFontName(font);
    dlg.setFontTitleSize(m_WebFontTitleSize);
    dlg.setFontTitleName(m_WebFontTitle);
    dlg.setFontHachiaName(m_WebFontHachia);
    dlg.setFontKhasse(m_WebFontKhase);
    dlg.setThemesStyle(m_myStyleName);
    dlg.setCadreFolder(m_CadrFolder);
    dlg.setIconsThemes(m_iconsThemes);
    dlg.setLng(m_lng);
    dlg.setLayouDir(m_layouDir);
    dlg.setArabicNumber(m_arabicNmber);
    dlg.setStandardTool(m_isStandardToolBar);
    dlg.setTxtUnderIcon(m_textUnderIcon);
    dlg.setIconSize(m_toolIconSize);
    dlg.setStyleSheetName(m_styleCostum);
    dlg.setStyleType(m_isStyleCostum);
    if (dlg.exec() == QDialog::Accepted) {
        m_WebColorBack = dlg.getColorBack();
        m_WebFontColor = dlg.getColorFont();
        m_WebFontFindColor = dlg.getColorFind();
        m_WebFontPrefertColor = dlg.getColorPrefer();
        m_WebFontTitleColor = dlg.getcolorTitle();
        txtBrowserBook->setFont(dlg.getFontName());
        m_WebFont = dlg.getFontName().toString();
        m_isCadre = dlg.getWebCadre();
        m_myStyleName = dlg.getThemesStyle();
        m_recentNbr = dlg.getRecentNbr();
        m_pathCostm = dlg.getPathCostm();
        m_WebFontTitle = dlg.getFontTitleName();
        m_WebFontTitleSize = dlg.getFontTitleSize();
        m_WebFontHachia = dlg.getFontHachiaName();
        m_WebFontKhase = dlg.getFontKhasse();
        m_CadrFolder = dlg.getCadreFolder();
        m_iconsThemes = dlg.getIconsThemes();
        m_arabicNmber = dlg.getArabicNumber();
        m_lng = dlg.getLng();
        m_isStandardToolBar = dlg.getsetStandardTool();
        m_textUnderIcon = dlg.getTxtUnderIcon();
        m_toolIconSize = dlg.getIconSize();
        m_layouDir = dlg.getLayouDir();
        m_styleCostum = dlg.getStyleSheetName();
        m_isStyleCostum = dlg.getStyleType();
        toolRubon->setToolIconSize(m_toolIconSize, m_textUnderIcon);

		static const unordered_map< int,string > map
		{ {1,langLoc}, {2,"pk"}, {3,"en"}, {4,"fr"} };
		if (auto it = map.find(m_lng); it != map.end())
			setLanguage( QString(MW::pathApp + "translat/kirtasse_%1.qm").arg(it->second) );

        if (m_layouDir)
            qApp->setLayoutDirection(Qt::LeftToRight);
        else
            qApp->setLayoutDirection(Qt::RightToLeft);
        creatStyle();
        if (m_currentIndex > 0)
            convertTextToHtml(databook->bookNass[m_currentIndex]);
        if (m_iconsThemes)	chargeIconsThemes();
    }
}

void MainWindow::on_actionRemoveTechkil_triggered() {
    if (ui->actionRemoveTechkil->isChecked()) {
        databook->removeTechkile = true;
    } else {
        databook->removeTechkile = false;
    }
    bookValueChanged(databook->currentPosition[m_currentIndex]);
}

void MainWindow::on_actionPageIntro_triggered() {
    if (ui->tabIntro->isHidden()) {

        ui->tabWidget->insertTab(0, ui->tabIntro, tr("مكتبة القرطاس"));
        ui->stackedWidget->insertWidget(0, ui->pageIntro);
        ui->tabWidget->setCurrentWidget(ui->tabIntro);
    }
    ui->stackedWidget_intro->setCurrentWidget(ui->page_anim);
    labelAnim->setText(tr(""));
    view->chargeEnrto();
}

void MainWindow::on_actionZoumIn_triggered() { txtBrowserBook->zoomIn(); }

void MainWindow::on_actionZoomOut_triggered() { txtBrowserBook->zoomOut(); }

// menuHelp-----------------------------------
void MainWindow::on_action_about_triggered() // حول البرنامج
{
    About* dlg = new About(this);

    if (dlg->exec() == QDialog::Accepted || QDialog::Rejected) {
    }
    delete dlg;
}

void MainWindow::on_actionHelp_triggered()
{
    if (ui->tabIntro->isHidden()) {

        ui->tabWidget->insertTab(0, ui->tabIntro, tr("مكتبة القرطاس"));
        ui->stackedWidget->insertWidget(0, ui->pageIntro);
    }
    ui->stackedWidget_intro->setCurrentWidget(ui->page_text);
    ui->treeWidget_rowatFind->setVisible(false);

    ui->tabWidget->setCurrentWidget(ui->tabIntro);
    labelAnim->setText(tr("التعليمات"));
    QFile file( MW::pathApp + "data/help.html");
    if (!file.exists()) {
        ui->textBrowser_0->setText(tr("ملف التعليمات  : ") + MW::pathApp + "data/help.html\n" + tr("غير موجود"));
        return;
    }
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        
        QString str = QString::fromUtf8(data);
        str.replace("<img src=\"", "<img src=\"" + MW::pathApp + "data/");
        if (Qt::mightBeRichText(str)) {
            ui->textBrowser_0->setHtml(str);
            ui->textBrowser_0->setOpenLinks(true);
        }
        file.close();
    }
}

void MainWindow::on_actionWhatsThis_triggered()
{
    QWhatsThis::enterWhatsThisMode();
}

//**treeViewBook-----------------------------------
void MainWindow::on_treeWidget_books_itemSelectionChanged() // تحديد عنصر الكتاب
                                                            // في الشجرة
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (!item->data(1, 1).isNull()) {
        QString imgScreenPath = m_pathCostm + "/" + item->data(1, 1).toString() + "/screenshot.png";
        if (!QFile::exists(imgScreenPath)) {
            imgScreenPath = ":/images/image/booki.png";
        }
        QString imgScreen = QString("<p align=\"center\" style=\" margin-top:0px; "
                                    "margin-bottom:0px; margin-left:0px; margin-right:0px; "
                                    "-qt-block-indent:0; text-indent:0px;\"><img src= %1 /></p>")
                                .arg(imgScreenPath);

        QString txtBookInfo = imgScreen + tr("الكتاب :") + item->text(0) + "<br>" + tr("المؤلف :") + item->text(1) + "<br>" + item->text(2);

        ui->textEditInfoBook->setText(txtBookInfo);
        //   startAnimationBitaka();
    } else {

        QString txtGroupInfo = "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; "
                               "margin-left:0px; margin-right:0px; -qt-block-indent:0; "
                               "text-indent:0px;\">"
            + item->text(0) + "</p>"
                              "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; "
                              "margin-bottom:0px; margin-left:0px; margin-right:0px; "
                              "-qt-block-indent:0; text-indent:0px;\"></p>"
                              "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; "
                              "margin-left:0px; margin-right:0px; -qt-block-indent:0; "
                              "text-indent:0px;\"><img src=\":/images/image/groupbook.png\" /></p>";
        ui->textEditInfoBook->setText(txtGroupInfo);
        //   labelBetaka->setPixmap(QPixmap(QString::fromUtf8(":/images/image/groupbook.png")));

        //           startAnimationPixmap();
    }
}

void MainWindow::on_treeWidget_books_itemActivated(
    QTreeWidgetItem* item) // تفعيل عنصر شجرة الكتب
{
    QString x = item->data(1, 1).toString();
    if (!x.isEmpty()) {
        open_dataBase(x, item->text(0), item->text(1), 0);
        // QVariant tf = item->data(2,1).toString();
        //  m_isTefsir[m_currentIndex]=tf.toBool();
    }
}

void MainWindow::
    on_treeWidget_books_customContextMenuRequested() // قائمة منسدلة فرعية لشجرة
                                                     // الكتب
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (!item->data(1, 1).isNull()) {
        QMenu menu(this);
        //     menu.setLayoutDirection(Qt::RightToLeft);
        menu.addAction(AC_bookOpen);
        menu.addAction(AC_bookOpenInNewTab);
        menu.addAction(AC_bookUpdat);
        menu.addSeparator();
        menu.addAction(AC_bookRemove);
        menu.addSeparator();
        menu.addAction(AC_bookPath);
        menu.addAction(AC_bookRename);
        menu.addAction(AC_bookDownload);
        menu.exec(QCursor::pos());
    } else {
        QMenu menu(this);
        //      menu.setLayoutDirection(Qt::RightToLeft);
        menu.addAction(AC_categorieAdd);
        menu.addAction(AC_groupeAdd);
        menu.addAction(AC_groupeRename);
        menu.addSeparator();
        menu.addAction(AC_groupeRemove);
        menu.exec(QCursor::pos());
    }
}

void MainWindow::menuOpenBook() // النقر على قائمة فتح كتاب
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (!item->data(1, 1).isNull()) {
        open_dataBase(item->data(1, 1).toString(), item->text(0), item->text(1), 0);
    }
}

void MainWindow::menu_BookRemove() // حذف الكتاب
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (!item->data(1, 1).isNull()) {
        QString myiddata = item->data(1, 1).toString();
        QMessageBox msgBox;
        msgBox.setText(tr("هل تريد حذف : ") + item->text(0) + tr(" ؟"));
        msgBox.setInformativeText(tr("انقر على نعم لحذفه من المكتبة فقط \n") + tr("او نعم للكل لحذفه من المكتبة ومن جهازك"));
        msgBox.setDetailedText(tr("اذا نقرت على نعم للكل سيتم حذف الكتاب من "
                                      "الدليل التالي ومن المكتبة\n")
            + m_pathCostm + myiddata);
        msgBox.setIcon(QMessageBox::Question);
        //   msgBox.setLayoutDirection(Qt::RightToLeft);
        msgBox.setWindowTitle(tr("تأكيد الحذف"));
        msgBox.setStandardButtons(QMessageBox::YesAll | QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::YesAll);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::YesAll:
            message->treeMenuRemoveBook(myiddata, true);
            chargeGroupe();
            break;
        case QMessageBox::Yes:
            message->treeMenuRemoveBook(myiddata, false);
            chargeGroupe();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            break;
        }
    }
}

void MainWindow::menu_BookUpdat() // تغيير بيانات الكتاب
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    dialogupdat* dlgc = new dialogupdat(this);
    dlgc->creatNewBook = "update";
    dlgc->m_pathCostum = m_pathCostm;
    dlgc->BookName = item->data(1, 1).toString();
    dlgc->addGroupId = item->parent()->data(2, 1).toString();
    dlgc->bookTitle = item->text(0);
    dlgc->bookAutor = item->text(1);
    dlgc->bookBetaka = item->text(2);

    dlgc->recharge();
    if (dlgc->exec() == QDialog::Accepted) {
        chargeGroupe();
    }
    delete dlgc;
}
void MainWindow::menu_BookPath() // معرفة مسار الكتاب
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (!item->data(1, 1).isNull()) {
        QString name = item->data(1, 1).toString();
        QString path;
        QString idparent = name.section("_", 0, 0);
        idparent = idparent.remove("bk");
        QFile file;
        if (file.exists(m_pathCostm + "/" + name)) {
            path = m_pathCostm + "/" + name;
        } else if (file.exists(m_pathCostm + "/" + idparent + "/" + name)) {
            path = m_pathCostm + "/" + idparent + "/" + name;
        } else if (file.exists( MW::pathApp + "books/" + name)) {
            path = MW::pathApp + "books/" + name;
        } else {
            path = name + tr(" : غير موجود في الجهاز");
        }
        QMessageBox::information(this, tr("مسار الكتاب"), path);
    }
}
void MainWindow::menu_downloadBook()
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();

    QString name = item->data(1, 1).toString();

    QFile file;
    if (file.exists(m_pathCostm + "/" + name)) {
        QMessageBox::information(
            this, "", tr("الكتاب موجود في جهازك\n") + m_pathCostm + "/" + name);
        return;
    } else if (file.exists( MW::pathApp + "books/" + name)) {
        QMessageBox::information(this, "",
            tr("الكتاب موجود في جهازك\n") + MW::pathApp + "books/" + "/" + name);
        return;
    }

    int idGroup = item->parent()->data(2, 1).toInt();
    QString drbox;
    if (idGroup < 36) {
        if (idGroup == 27) {
            drbox = "http://dl.dropbox.com/u/13013844/" + QString::number(idGroup) + "/";
        } else {
            drbox = "http://dl.dropbox.com/u/7206075/" + QString::number(idGroup) + "/";
        }
    } else if (idGroup > 35) {
        drbox = "http://dl.dropbox.com/u/13013844/" + QString::number(idGroup) + "/";
    }

    QString idurl = drbox + name + ".tar.gz";
    //   QMessageBox::information(this,"",idurl);

    //---------------------------
    if (loadPlugin() == false)
        return;

    QString targzName = netInterface->loadFile(idurl);
    if (targzName.isEmpty())
        return;
    //   QString targzName=name+".tar.gz";

    QString orgPath = QDir::homePath() + "/.kirtasse/download/" + targzName;
    // QMessageBox::information(0,"orgPath",orgPath);

    QFile filex;
    if (!filex.exists(orgPath)) {

        return;
    }
    if (message->loadTarGz(orgPath) == false)
        return;
    if (!filex.exists(QDir::homePath() + "/.kirtasse/download/bookinfo.info"))
        return;
    QDir dir(m_pathCostm);
    if (dir.mkdir(name) == true) {
        QFile file;
        file.copy(QDir::homePath() + "/.kirtasse/download/book.xml",
            m_pathCostm + "/" + name + "/book.xml");
        file.copy(QDir::homePath() + "/.kirtasse/download/title.xml",
            m_pathCostm + "/" + name + "/title.xml");
        file.copy(QDir::homePath() + "/.kirtasse/download/bookinfo.info",
            m_pathCostm + "/" + name + "/bookinfo.info");
    }
    if (filex.exists(m_pathCostm + "/" + name + "/book.xml"))
        QMessageBox::information(
            this, "", tr("تمت العملية بنجاح \n") + m_pathCostm + "/" + name);

    message->removeTempDirs(QDir::homePath() + "/.kirtasse/download");
    open_dataBase(name, item->text(0), item->text(1), 0);
}
void MainWindow::menu_renameBook()
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    QString name = item->data(1, 1).toString();
    QString path;
    QFile file;
    if (file.exists(m_pathCostm + "/" + name)) {
        path = m_pathCostm;
    } else if (file.exists( MW::pathApp + "books/" + name)) {
        path = MW::pathApp + "books/";
    }

    QDir dir(path);
    QInputDialog* dff = new QInputDialog(this);
    dff->setOkButtonText(tr("موافق"));
    //    dff->setLayoutDirection(Qt::RightToLeft);
    dff->setCancelButtonText(tr("الغاء"));
    dff->setLabelText(tr("الاسم القديم : ") + name + "\n" + tr("يحبذ ان لا يحتوي الاسم على فراغات"));
    dff->setWindowTitle(tr("اعادة تسمية دليل الكتاب"));
    dff->setTextValue(item->text(0).replace(" ", "_"));
    if (dff->exec() == QInputDialog::Accepted) {
        if (dff->textValue().isEmpty()) {
            return;
        }
        if (file.exists(path + "/" + dff->textValue())) {
            QMessageBox::critical(this, tr("خطأ"),
                tr("يوجد مجلد بنفس الاسم"));
            return;
        }
        if (dir.rename(name, dff->textValue()) == true) {
            item->setData(1, 1, dff->textValue());
            message->treeSaveGroupe(ui->treeWidget_books);

            QMessageBox::information(
                this, tr("معلومات"),
                tr("تمت عملية اعادة تسمية دليل الكتاب بنجاح"));
        }
    }
}

void MainWindow::menu_Add(bool categorie)
{
    QInputDialog* dff = new QInputDialog(this);
    dff->setOkButtonText(tr("موافق"));
    dff->setLayoutDirection(Qt::RightToLeft);
    dff->setCancelButtonText(tr("الغاء"));
    if (categorie == true) {
        dff->setLabelText(tr("ادخل اسما للقسم الجديد"));
        dff->setWindowTitle(tr("قسم جديد"));
    } else {
        dff->setLabelText(tr("ادخل اسما للمجموعة الجديدة"));
        dff->setWindowTitle(tr("مجموعة جديدة"));
    }

    QString text;
    if (dff->exec() == QInputDialog::Accepted) {
        text = dff->textValue();
        //***********
        QVariant newId;
        newId = message->genirateId(ui->treeWidget_books);
        //*************
        QTreeWidgetItem* curentitem; // العنصر المحدد
        QTreeWidgetItem* item; // العنصر الذي سنظيفه
        curentitem = ui->treeWidget_books->currentItem();
        if (categorie == true) { // اذا لم يكن هناك تحديد سيظاف الى الجذر
            item = new QTreeWidgetItem(ui->treeWidget_books);

        } else {
            if (!curentitem->parent()) { // اذا لم يكن هناك تحديد سيظاف الى الجذر
                item = new QTreeWidgetItem(curentitem);
            } else { // اما ان كان هناك عنصر محدد سيظاف بعده
                item = new QTreeWidgetItem(curentitem->parent());
            }
        }
        item->setText(0, text);
        item->setData(2, 1, newId.toString());
        item->setIcon(0, m_iconGroup);
        item->setSelected(true);
        //  curentitem->setSelected(false);
        ui->treeWidget_books->setCurrentItem(item); // تفعيل العنصر المظاف
        m_treeGroupIsModified = true;
        if (m_treeGroupIsModified == true) {
            int reponse = QMessageBox::warning(
                this, tr("تنبيه"),
                tr("هل تريد حفظ التغييرات المدخلة على مجموعة الكتب "),
                QMessageBox::Yes | QMessageBox::No);
            if (reponse == QMessageBox::Yes) {
                if (message->treeSaveGroupe(ui->treeWidget_books)) {
                    QMessageBox::information(this, tr("معلومات"),
                        tr("تمت عملية حفظ مجموعة الكتب بنجاح"));
                    m_treeGroupIsModified = false;
                }
            }
        }
    }
}

void MainWindow::menu_CategorieAdd() { menu_Add(true); }

void MainWindow::menu_GroupeAdd() // اضافة مجمعة جديدة الى الشجرة
{
    menu_Add(false);
}
void MainWindow::menu_GroupeRemove() // حذف المجموعة
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    if (item->childCount() > 0) {
        QMessageBox::information(
            this, tr("تنبيه"),
            tr("المحموعة تحتوي على عناصر تحتهاقم بحذف هذه العناصر أولا"));
    } else {
        int reponse = QMessageBox::question(
            this, tr("معلومات"), tr("هل تريد حذف المجموعة المحددة؟ "),
            QMessageBox::Yes | QMessageBox::No);
        if (reponse == QMessageBox::Yes) {
            if (message->treeviewItemRemove(ui->treeWidget_books) == true) {
                m_treeGroupIsModified = true;
            }
        }
    }
}
void MainWindow::menu_GroupeRename() // اعادة تسمية المجموعة
{
    QTreeWidgetItem* item = ui->treeWidget_books->currentItem();
    QInputDialog* dff = new QInputDialog(this);
    dff->setOkButtonText(tr("موافق"));
    dff->setLayoutDirection(Qt::RightToLeft);
    dff->setCancelButtonText(tr("الغاء"));
    dff->setLabelText(tr("ادخل النص الجديد"));
    dff->setWindowTitle(tr("اعادة تسمية المجموعة"));
    dff->setTextValue(item->text(0));
    if (dff->exec() == QInputDialog::Accepted) {
        if (dff->textValue().isEmpty()) {
            return;
        }
        item->setText(0, dff->textValue());
        m_treeGroupIsModified = true;
    }
}

//**treeViewFahras---------------------------------------
void MainWindow::fahras_itemClicked(
    QTreeWidgetItem* item) // النقر على عنصر شجرة الفهرسة
{
    if (m_toolEditFahrass[m_currentIndex] == false) {
        bookValueChanged(item->data(1, 1).toInt() - 1);
        int index = txtBrowserBook->toPlainText().indexOf(item->text(0));
        if (index > 0) {
            QTextCursor cursor(txtBrowserBook->document());
            cursor.setPosition(index, QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
            txtBrowserBook->setTextCursor(cursor);
        }
        treeViewFahras[m_currentIndex]->setCurrentItem(item);
    }
    //
}

void MainWindow::fahras_itemActivated(QTreeWidgetItem* item)
{
    databook->newPosition[m_currentIndex] = item->data(1, 1).toInt() - 1;
    bookValueChanged(item->data(1, 1).toInt() - 1);
    int index = txtBrowserBook->toPlainText().indexOf(item->text(0));
    if (index > 0) {
        QTextCursor cursor(txtBrowserBook->document());
        cursor.setPosition(index, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
        txtBrowserBook->setTextCursor(cursor);
    }
    treeViewFahras[m_currentIndex]->setCurrentItem(item);
}

void MainWindow::on_pushButtonEditClose_clicked()
{
    txtBrowserBook->setReadOnly(true);
    txtBrowserBook->setUndoRedoEnabled(false);
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
        m_bookTitle[m_currentIndex]);
    ui->frameEdit->setVisible(false);
    ui->widgetEditBook->setVisible(false);
    m_toolEditFahrass[m_currentIndex] = false;
    treeViewFahras[m_currentIndex]->setDragDropMode(
        QAbstractItemView::NoDragDrop);
    treeViewFahras[m_currentIndex]->setSelectionMode(
        QAbstractItemView::SingleSelection);
}
void MainWindow::itemFahrasAdd() { addItemInTreeFahras(false); }

void MainWindow::itemFahrasAddChild() { addItemInTreeFahras(true); }
void MainWindow::itemFahrasRename()
{
    QTreeWidgetItem* item = treeViewFahras[m_currentIndex]->currentItem();
    if (!item) {
        return;
    }
    QInputDialog* dff = new QInputDialog(this);
    dff->setOkButtonText(tr("موافق"));
    dff->setLayoutDirection(Qt::RightToLeft);
    dff->setCancelButtonText(tr("الغاء"));
    dff->setLabelText(tr("ادخل اسما جديدا"));
    dff->setWindowTitle(tr("اعادة تسمية العنصر"));
    dff->setTextValue(item->text(0));
    if (dff->exec() == QInputDialog::Accepted) {
        QString newtext = dff->textValue();
        if (newtext.isEmpty()) {
            return;
        }
        item->setText(0, newtext);
        m_treeFahrasIsModified[m_currentIndex] = true;
    }
}
void MainWindow::itemFahrasRemove() // حذف عنصر من الفهرسة
{
    if (message->treeviewItemRemove(treeViewFahras[m_currentIndex]) == true) {
        m_treeFahrasIsModified[m_currentIndex] = true;
    }
}

void MainWindow::itemFahrasMoveDown() // نقل عنصر للاسفل في الفهرسة
{
    if (message->treeviewItemDown(treeViewFahras[m_currentIndex]) == true) {
        m_treeFahrasIsModified[m_currentIndex] = true;
    }
}

void MainWindow::itemFahrasMoveUp() // نقل عنصر للاعلى في الفهرسة
{
    if (message->treeviewItemUp(treeViewFahras[m_currentIndex]) == true) {
        m_treeFahrasIsModified[m_currentIndex] = true;
    }
}

void MainWindow::on_btnGoFindFah_clicked() // انشاء فهارس
{
    QRegularExpression rx;
    QRegularExpression rx2;
    if (ui->checkBoxNumber->isChecked()) {
        rx.setPattern("(\\d+)(" + ui->lineEditNiveau1->text() + ")");
    } else {
        if (ui->lineEditNiveau1->text().isEmpty())
            return;
        rx.setPattern(ui->lineEditNiveau1->text());
    }
    if (ui->checkBoxNumber2->isChecked()) {
        rx2.setPattern("(\\d+)(" + ui->lineEditNiveau2->text() + ")");
    } else {

        rx2.setPattern(ui->lineEditNiveau2->text());
    }

    findBook->findFahariss(treeViewFahras[m_currentIndex],
        m_bookPath[m_currentIndex], rx, rx2,
        databook->rowsCount[m_currentIndex]);
    m_treeFahrasIsModified[m_currentIndex] = true;
}

//**treeViewFavorit--------------------------------
void MainWindow::on_treeWidget_fav_itemActivated(QTreeWidgetItem* item)
{
    if (item->data(1, 1).toString().isEmpty()) {
        return;
    }
    QVariant bkpage = item->data(2, 1);
    QVariant bknam = item->data(1, 1);
    if (bknam.toString() == m_bookName[m_currentIndex]) {
        databook->newPosition[m_currentIndex] = bkpage.toInt();
        bookValueChanged(bkpage.toInt());
    } else {
        open_dataBase(item->data(1, 1).toString(), item->text(1), item->text(2),
            bkpage.toInt());
    }
}

void MainWindow::on_actionFavSave_triggered()
{
    message->favorite_save(ui->treeWidget_fav);
}

void MainWindow::on_actionFavItemRen_triggered()
{
    QTreeWidgetItem* item = ui->treeWidget_fav->currentItem();
    if (!item) {
        return;
    }
    QInputDialog* dff = new QInputDialog(this);
    dff->setOkButtonText(tr("موافق"));
    dff->setLayoutDirection(Qt::RightToLeft);
    dff->setCancelButtonText(tr("الغاء"));
    dff->setLabelText(tr("ادخل اسما جديدا"));
    dff->setWindowTitle(tr("اعادة تسمية العنصر"));
    dff->setTextValue(item->text(0));
    if (dff->exec() == QInputDialog::Accepted) {
        QString newtext = dff->textValue();
        if (newtext.isEmpty()) {
            return;
        }
        item->setText(0, newtext);
    }
}

void MainWindow::on_actionFavItemUp_triggered()
{
    QTreeWidgetItem* item = ui->treeWidget_fav->currentItem();
    if (!item) {
        return;
    }
    message->treeviewItemUp(ui->treeWidget_fav);
}

void MainWindow::on_actionFavItemDown_triggered()
{
    QTreeWidgetItem* item = ui->treeWidget_fav->currentItem();
    if (!item) {
        return;
    }
    message->treeviewItemDown(ui->treeWidget_fav);
}

void MainWindow::on_actionFavDelete_triggered()
{
    QTreeWidgetItem* item = ui->treeWidget_fav->currentItem();
    if (!item) {
        return;
    }
    QString text;
    if (item->childCount() > 0) {
        text = tr(
            "العنصر الذي تريد حذفه يحتوي على عناصر اخرى هل تريد حذفه رغم ذلك ؟");
    } else {
        text = tr("هل تريد حذف العنصر المحدد");
    }
    int reponse = QMessageBox::warning(this, tr("تنبيه"), text,
        QMessageBox::Yes | QMessageBox::No);
    if (reponse == QMessageBox::No) {
        return;
    }
    message->treeviewItemRemove(ui->treeWidget_fav);
}

//**treeVeiwCuran-----------------------------
void MainWindow::on_treeWidget_curaan_itemActivated(
    QTreeWidgetItem* item) // تفعيل عنصر شجرة القرءان
{

    if (m_isTefsir[m_currentIndex] == true) {
        ui->treeWidget_curaan->setCursor(QCursor(Qt::WaitCursor));
        QString mysora = item->data(1, 1).toString().section('_', 0, 0);
        QString myaya = item->data(1, 1).toString().section('_', -1);

        if (databook->findAya(mysora, myaya) == true) {
            bookValueChanged(databook->currentPosition[m_currentIndex]);
            ui->treeWidget_curaan->setCursor(QCursor(Qt::ArrowCursor));
            return;
        }
    }
    ui->treeWidget_curaan->setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::on_pushButton_gofind_clicked()
{
    if (ui->lineEdit_find_cur->text() != tr("بحث")) {
        ui->pushButton_gofind->setCursor(QCursor(Qt::WaitCursor));
        findBook->searchInTreeview(ui->treeWidget_curaan,
            ui->lineEdit_find_cur->text(), 0);
        ui->pushButton_gofind->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::on_lineEdit_find_cur_textChanged(QString searchString)
{
    if (searchString.isEmpty()) {
        findBook->searchInTreeview(ui->treeWidget_curaan,
            ui->lineEdit_find_cur->text(), 0);
    }
}

void MainWindow::on_comboBox_currentIndexChanged(
    int index) // codexml souara  //فتح السور فتح الاجزاء
{
    ui->treeWidget_curaan->clear();
    if (index == 0) {
        message->treeChargeJozaa(ui->treeWidget_curaan);
    } else {
        message->treeChargeSoura(ui->treeWidget_curaan);
    }
}

void MainWindow::on_pushButton_gotafsir_clicked() // فتح كتاب التفسير
{
    int index = ui->comboBox_tafasir->currentIndex();
    QVariant comboR_data = ui->comboBox_tafasir->itemData(index);
    QString myname = comboR_data.toString().section('/', 0, 0);
    QString myaut = comboR_data.toString().section('/', -1);
    if (!myname.isEmpty()) {

        open_dataBase(myname, ui->comboBox_tafasir->currentText(), myaut, 0);
        m_isTefsir[m_currentIndex] = true;
    }
}

//**treeViewFind---------------------------

void MainWindow::on_actionFindItemUp_triggered()
{
    QTreeWidgetItem* item = ui->treeWidgetFind->currentItem();
    if (!item) {
        return;
    }
    message->treeviewItemUp(ui->treeWidgetFind);
}

void MainWindow::on_actionFindItemDown_triggered()
{
    QTreeWidgetItem* item = ui->treeWidgetFind->currentItem();
    if (!item) {
        return;
    }
    message->treeviewItemDown(ui->treeWidgetFind);
}

void MainWindow::on_actionFindDelete_triggered()
{
    QTreeWidgetItem* item = ui->treeWidgetFind->currentItem();
    if (!item) {
        return;
    }
    QString text = tr("هل تريد حذف العنصر المحدد");

    int reponse = QMessageBox::warning(this, tr("تنبيه"), text,
        QMessageBox::Yes | QMessageBox::No);
    if (reponse == QMessageBox::No) {
        return;
    }
    message->treeviewItemRemove(ui->treeWidgetFind);
}

void MainWindow::on_actionFindSave_triggered()
{

    if (ui->treeWidgetFind->topLevelItemCount() > 0) {
        QString homeDir = QDir::homePath();
        QString fn = QFileDialog::getSaveFileName(this, tr("Save File..."), homeDir,
            tr("xml files (*.xml );"));
        findBook->findText = m_textfind;
        findBook->isNassToFind = isNassToFind;
        if (findBook->saveResultFind(ui->treeWidgetFind, fn) == true) {
            QMessageBox::information(this, tr("معلومات"),
                tr("تمت العملية الحفظ بنجاح"));
        } else {
            QMessageBox::information(this, tr("خطأ"),
                tr("لا يمكن كتابة الملف"));
        }
    }
}

void MainWindow::on_actionFindLoad_triggered()
{

    QString homeDir = QDir::homePath();
    QString fn = QFileDialog::getOpenFileName(
        this, tr("Open File..."), homeDir, tr("XML files (*.xml);;All files(*)"));
    QFile file(fn);
    if (file.exists()) {
        ui->treeWidgetFind->setColumnWidth(0, 500);
        ui->treeWidgetFind->setColumnWidth(1, 30);
        ui->treeWidgetFind->setColumnWidth(2, 50);
        ui->treeWidgetFind->clear();
        findBook->loadResultFind(ui->treeWidgetFind, fn);
        m_textfind = findBook->findText;
        isNassToFind = findBook->isNassToFind;
        labelTextFind->setText(tr("نص البحث  : ") + m_textfind);
    }
}

//**Animation----------------------------------------

void MainWindow::on_actionAnimGroup_triggered()
{
    ui->stackedWidget_intro->setCurrentWidget(ui->page_anim);

    view->chargeLevelOne();
    view->setFocus();
    // ui->stackedWidget_intro->setCurrentWidget(ui->page_configure);
}

void MainWindow::on_actionAnimHistorie_triggered()
{
    ui->stackedWidget_intro->setCurrentWidget(ui->page_anim);
    int count = 0;
    for (int i = 0; i < 16; i++) {
        if (recentFileActs[i]->isVisible()) {
            count++;
            view->recentFileActs[i] = recentFileActs[i];
        }
    }
    view->recentNbr = count + 1;
    view->chargeRecent();
    view->setFocus();
}

void MainWindow::on_actionTreeD_toggled(bool checked)
{
    view->threeD = checked;
    view->setFocus();
}

void MainWindow::on_lineEdit_textChanged(QString)
{
    if (!ui->lineEdit->text().isNull() | !ui->lineEdit->text().isEmpty()) {
        QString myid = ui->lineEdit->text().section('/', 0, 0);
        QString myname = ui->lineEdit->text().section('/', 1, 1);
        QString myaut = ui->lineEdit->text().section('/', 2, 2);
        QString mypage = ui->lineEdit->text().section('/', -1);
        open_dataBase(myid, myname, myaut, mypage.toInt());
    }
}

// void MainWindow::startAnimationBitaka()
//{
// #if QT_VERSION >= 0x040600
////    QPropertyAnimation *anim = new QPropertyAnimation(labelBetaka, "pos");
////    anim->setEasingCurve(QEasingCurve::OutCirc);
////    labelBetaka->setWordWrap(true);

////    anim->setStartValue(QPointF(0, 0));
////    anim->setEndValue(QPointF(300, 0));
////    anim->setDuration(500);
////    anim->setLoopCount(1); // forever
////    anim->start();
// #endif
// }

// void MainWindow::startAnimationPixmap()
//{
// #if QT_VERSION >= 0x040600
////    QPropertyAnimation *anim = new QPropertyAnimation(labelBetaka, "pos");
////    anim->setEasingCurve(QEasingCurve::OutBounce);
////
/// labelBetaka->setPixmap(QPixmap(QString::fromUtf8(":/images/image/groupbook.png")));
////    anim->setStartValue(QPointF(300,300));
////    anim->setEndValue(QPointF(300,0));
////    anim->setDuration(1000);
////    anim->setLoopCount(1);
////    anim->start();
// #endif
// }

// tabs---------------------------
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    QTextBrowser* txtBrowser = ui->tabWidget->widget(index)->findChild<QTextBrowser*>();
    QString name = txtBrowser->objectName();
    QString str = name.section('_', -1);
    if (!m_bookName[str.toInt()].isEmpty()) { // حفظ قائمة الكتب السابقة

        if (txtBrowser->document()->isModified()) {
            databook->updatPage("", txtBrowser->document()->toPlainText(), true);
            m_bookIsModified[str.toInt()] = true;
        }

        if (closeMessages(m_bookTitle[str.toInt()], str.toInt()) == false) {

            return;
        }

        QVariant idbk = databook->currentPosition[databook->boocNumIndex];
        message->recentChange(m_bookName[str.toInt()], m_bookTitle[str.toInt()],
            m_bookAuthor[str.toInt()], idbk.toString(),
            m_recentNbr, m_isTefsir[str.toInt()]);
        message->recentCharge();
    }
    m_bookName[str.toInt()] = "";
    m_listTab.removeOne(str);
    ui->tabWidget->removeTab(index);
    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(index));
    ui->stackedWidget->setCurrentIndex(ui->tabWidget->currentIndex());
    databook->claerBook(str.toInt());
    m_listHistoriePrev[str.toInt()].clear();
    m_listHistorieNext[str.toInt()].clear();
    if (ui->tabWidget->count() > 1) {
        ui->tabWidget->setTabsClosable(true);
    } else {
        ui->tabWidget->setTabsClosable(false);
    }
}

void MainWindow::on_tabWidget_currentChanged(int)
{

    txtBrowserBook = ui->tabWidget->currentWidget()->findChild<QTextBrowser*>();
    QString name = txtBrowserBook->objectName();
    QString str = name.section('_', -1);
    m_currentIndex = str.toInt();
    ;
    databook->boocNumIndex = str.toInt();

    if (ui->tabWidget->count() > 1) {
        ui->tabWidget->setTabsClosable(true);
    } else {
        ui->tabWidget->setTabsClosable(false);
    }

    if (m_toolEditFahrass[m_currentIndex] == true) {
        ui->frameEdit->setVisible(true); // اظهار ادوات التحرير
        ui->widgetEditBook->setVisible(true);

    } else {
        ui->frameEdit->setVisible(false);
        ui->widgetEditBook->setVisible(false);
    }
    ui->stackedWidget->setCurrentIndex(ui->tabWidget->currentIndex());

    emit bookAvailable(!m_bookName[m_currentIndex].isEmpty());

    QFont font;
    font.fromString(m_WebFont);
    if (txtBrowserBook->objectName() != "textBrowser_0") {
        //  QMessageBox::information(this,"",txtBrowserBook->objectName());
        txtBrowserBook->setFont(font);
    }
}

void MainWindow::tabCreatTabNew()
{
    txtBrowserBook = ui->tabWidget->currentWidget()->findChild<QTextBrowser*>();
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/images/image/booki.png")),
        QIcon::Normal, QIcon::Off);
    if (ui->tabWidget->count() == 10) {
        QMessageBox::information(this, tr("معلومات"),
            tr("لايمكن فتح اكثر من عشرة السنة"));
        return;
    }
    //*****************************
    QWidget* tab;
    QVBoxLayout* horizontalLayout;
    QGridLayout* gridLayout;

    // QTextBrowser *textBrowser;
    tab = new QWidget();
    tab->setLayoutDirection(Qt::RightToLeft);
    horizontalLayout = new QVBoxLayout(tab);
    horizontalLayout->setSpacing(3);
    horizontalLayout->setContentsMargins(3, 3, 3, 3);
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    txtBrowserBook = new QTextBrowser(tab);

    if (m_isCadre == true) {
        txtBrowserBook->setFrameShape(QFrame::NoFrame);
        tabCreatCadre(gridLayout);
    }

    gridLayout->addWidget(txtBrowserBook, 1, 1, 1, 1);

    horizontalLayout->addLayout(gridLayout);
    ui->tabWidget->addTab(tab, icon, tr("مكتبة القرطاس"));

    //*******************************
    QVariant d;
    for (int r = 1; r < 10; r++) // تعريف وحيد للسان الجديد
    {
        d = r;
        if (m_listTab.lastIndexOf(d.toString()) == -1) {
            m_listTab.append(d.toString());
            txtBrowserBook->setObjectName("textBrowser_" + d.toString());
            m_currentIndex = d.toInt();
            break;
        }
    }
    QWidget* firstPageWidget = new QWidget;
    ui->stackedWidget->addWidget(firstPageWidget);
    ui->stackedWidget->setCurrentWidget(firstPageWidget);
    ui->tabWidget->setCurrentWidget(tab);
    // معرفة مربع النص في اللسان النشط
    txtBrowserBook = ui->tabWidget->currentWidget()->findChild<QTextBrowser*>();

    txtBrowserBook->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(txtBrowserBook, SIGNAL(customContextMenuRequested(QPoint)), this,
        SLOT(textBrewserCustomMenu()));

    connect(txtBrowserBook, SIGNAL(highlighted(const QUrl)), this,
        SLOT(textBookHighlighted(QUrl)));
    connect(txtBrowserBook, SIGNAL(anchorClicked(const QUrl)), this,
        SLOT(textBookAnchorClicked(QUrl)));

    connect(txtBrowserBook, SIGNAL(copyAvailable(bool)), ui->actionFahAddChild,
        SLOT(setEnabled(bool)));
    connect(txtBrowserBook, SIGNAL(copyAvailable(bool)), ui->actionFahAddItem,
        SLOT(setEnabled(bool)));
    connect(txtBrowserBook, SIGNAL(copyAvailable(bool)), ui->actionFontCostum,
        SLOT(setEnabled(bool)));

    emit bookAvailable(false);
    tabCreatFahrass();
}

void MainWindow::tabCreatFahrass()
{
    QGridLayout* gridLayout;
    gridLayout = new QGridLayout(ui->stackedWidget->currentWidget());
    gridLayout->setSpacing(3);
    gridLayout->setContentsMargins(3,3,3,3);
    treeViewFahras[m_currentIndex] = new QTreeWidget(ui->stackedWidget->currentWidget());
    treeViewFahras[m_currentIndex]->headerItem()->setText(0, "");
    gridLayout->addWidget(treeViewFahras[m_currentIndex], 0, 0, 1, 1);
    treeViewFahras[m_currentIndex]->show();
    connect(treeViewFahras[m_currentIndex],
        SIGNAL(itemActivated(QTreeWidgetItem*, int)), this,
        SLOT(fahras_itemActivated(QTreeWidgetItem*)));
    connect(treeViewFahras[m_currentIndex],
        SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
        SLOT(fahras_itemClicked(QTreeWidgetItem*)));
    connect(lineEditFindFah, SIGNAL(textChanged(QString)), this,
        SLOT(searchInTreeFah(QString)));
    connect(AC_expandTreeFah, SIGNAL(triggered()), treeViewFahras[m_currentIndex],
        SLOT(expandAll()));
    connect(AC_colapsTreeFah, SIGNAL(triggered()), treeViewFahras[m_currentIndex],
        SLOT(collapseAll()));
}
void MainWindow::tabCreatCadre(QGridLayout* gridLayout)
{
    QString imgPath = MW::pathApp + "data/images/" + m_CadrFolder;
    if (m_CadrFolder == tr("الافتراضي")) {
        imgPath = ":/images/image";
    } else {
        QFile file;
        if (!file.exists(imgPath + "/top.png") || !file.exists(imgPath + "/top_right.png") || !file.exists(imgPath + "/left.png") || !file.exists(imgPath + "/right.png") || !file.exists(imgPath + "/bot.png") || !file.exists(imgPath + "/bot_right.png") || !file.exists(imgPath + "/top_left.png") || !file.exists(imgPath + "/bot_left.png")) {
            m_CadrFolder = tr("الافتراضي");
            imgPath = ":/images/image";
        }
    }

    QPixmap pix(imgPath + "/top_right.png");
    QSize size(pix.width(), pix.height());

    QWidget *frame_top, *frame_topRight, *frame_left, *frame_right, *frame_bot,
        *frame_botRight, *frame_topLeft, *frame_botLeft;

    frame_top = new QWidget();
    frame_top->setStyleSheet("background-image: url(" + imgPath + "/top.png);");

    gridLayout->addWidget(frame_top, 0, 1, 1, 1);

    frame_topRight = new QWidget();
    frame_topRight->setMinimumSize(size);
    frame_topRight->setStyleSheet("background-image: url(" + imgPath + "/top_right.png);");

    gridLayout->addWidget(frame_topRight, 0, 0, 1, 1);

    frame_left = new QWidget();
    frame_left->setStyleSheet("background-image: url(" + imgPath + "/left.png);");

    gridLayout->addWidget(frame_left, 1, 2, 1, 1);

    frame_right = new QWidget();
    frame_right->setStyleSheet("background-image: url(" + imgPath + "/right.png);");

    gridLayout->addWidget(frame_right, 1, 0, 1, 1);

    frame_bot = new QWidget();
    frame_bot->setStyleSheet("background-image: url(" + imgPath + "/bot.png);");

    gridLayout->addWidget(frame_bot, 2, 1, 1, 1);

    frame_botRight = new QWidget();
    frame_botRight->setMinimumSize(size);
    frame_botRight->setStyleSheet("background-image: url(" + imgPath + "/bot_right.png);");

    gridLayout->addWidget(frame_botRight, 2, 0, 1, 1);

    frame_topLeft = new QWidget();
    frame_topLeft->setMinimumSize(size);
    frame_topLeft->setStyleSheet("background-image: url(" + imgPath + "/top_left.png);");

    gridLayout->addWidget(frame_topLeft, 0, 2, 1, 1);

    frame_botLeft = new QWidget();
    frame_botLeft->setMinimumSize(size);
    frame_botLeft->setStyleSheet("background-image: url(" + imgPath + "/bot_left.png);");

    gridLayout->addWidget(frame_botLeft, 2, 2, 1, 1);
}

void MainWindow::tabOpenInNewTab()
{
    tabCreatTabNew();
    menuOpenBook();
}

void MainWindow::chargeGroupe() // تحميل شجرة الكتب
{
    message->comboGroup = ui->comboBox_tafasir;
    message->treeChargeGroupe(ui->treeWidget_books, 0, false);
}

// save load other************************************************************

void MainWindow::saveLayou() // حفظ البيانات الى ملف
{
    QSettings settings( MW::pathUser + "/data/setting.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("style", m_myStyleName);
    settings.setValue("font", m_WebFont);
    settings.setValue("fontTitle", m_WebFontTitle);
    settings.setValue("fontTitleSize", m_WebFontTitleSize);
    settings.setValue("FontKhase", m_WebFontKhase);
    settings.setValue("fontHachia", m_WebFontHachia);
    settings.setValue("bcolor", m_WebColorBack);
    settings.setValue("WebFontColor", m_WebFontColor);
    settings.setValue("WebFontFindColor", m_WebFontFindColor);
    settings.setValue("WebFontPrefertColor", m_WebFontPrefertColor);
    settings.setValue("WebFontTitleColor", m_WebFontTitleColor);
    settings.setValue("recentNbr", m_recentNbr);
    settings.setValue("pathCostm", m_pathCostm);
    settings.setValue("isThreeD", ui->actionTreeD->isChecked());
    settings.setValue("WebCadre", m_isCadre);
    settings.setValue("iconsThemes", m_iconsThemes);

    settings.setValue("CadrFolder", m_CadrFolder);
    settings.setValue("lng", m_lng);
    settings.setValue("layouDir", m_layouDir);
    settings.setValue("arabicNmber", m_arabicNmber);

    settings.setValue("isStandardToolBar", m_isStandardToolBar);

    settings.setValue("toolIconSize", m_toolIconSize);
    settings.setValue("textUnderIcon", m_textUnderIcon);
    settings.setValue("listActToAdd", m_listActToAdd);
    settings.setValue("styleCostum", m_styleCostum);
    settings.setValue("isStyleCostum", m_isStyleCostum);

    settings.setValue("geo_data", saveGeometry());
    settings.setValue("layout_data", saveState(0));

    settings.endGroup();
}

void MainWindow::loadLayout() // Get settings
{

    QByteArray layo_data = QByteArray::fromHex(
        "(000000ff00000000fd00000002000000010000010600000276fc0200000001fc0000004"
        "000000276000001330007"
        "fffffa000000020100000003fb000000220064006f0063006b0057006900640067006500"
        "74005f0063007500720061006100"
        "6e0100000000ffffffff000000b600fffffffb000000220064006f0063006b0057006900"
        "64006700650074005f00660061006"
        "80072006100730100000000ffffffff000000970007fffffb000000200064006f0063006"
        "b005700690064006700650074005f0"
        "062006f006f006b0073010000045200000106000000780000058c0000000300000558000"
        "000c7fc0100000001fc0000000000000"
        "5580000000000fffffffaffffffff0200000002fb000000260064006f0063006b0057006"
        "90064006700650074005f0066006100760"
        "06f00720069007400650000000000ffffffff000000aa00fffffffb0000001e0064006f0"
        "063006b005700690064006700650074005"
        "f00660069006e006400000002490000006d0000006d00ffffff0000044c0000027600000"
        "004000000040000000800000008fc000000"
        "01000000020000000300000016006d00610069006e0054006f006f006c00420061007201"
        "00000000ffffffff00000000000000000000"
        "00220074006f006f006c004200610072005f006e006100760065006700610074006f0072"
        "0100000203ffffffff000000000000000000"
        "00001a0074006f006f006c0042006100720072006500630065006e007401000002a9ffff"
        "ffff0000000000000000)");
    QSettings settings( MW::pathUser + "/data/setting.ini", QSettings::IniFormat);
    // QSettings settings("Kirtasse", "setting");
    settings.beginGroup("MainWindow");
    m_myStyleName = settings.value("style", "").toString();
    m_WebFont = settings.value("font", tr("Traditional Arabic,20,-1,5,50,0,0,0,0,0"))
                    .toString();
#ifdef Q_WS_X11

    m_WebFontTitle = settings.value("fontTitle", "KacstTitle").toString();
    m_WebFontHachia = settings.value("fontHachia", "KacstBook").toString();
#else
    m_WebFontTitle = settings.value("fontTitle", "Andalus").toString();
    m_WebFontHachia = settings.value("fontHachia", "Traditional Arabic").toString();
#endif
    m_WebFontKhase = settings.value("FontKhase", "Jameel Noori Nastaleeq").toString();
    m_WebFontTitleSize = settings.value("fontTitleSize", "20").toString();

    m_WebColorBack = settings.value("bcolor", "#F9F9B9").toString();
    m_WebFontColor = settings.value("WebFontColor", "#000000").toString();
    m_WebFontFindColor = settings.value("WebFontFindColor", "#aa5500").toString();
    m_WebFontPrefertColor = settings.value("WebFontPrefertColor", "#264c00").toString();
    m_WebFontTitleColor = settings.value("WebFontTitleColor", "#ff0000").toString();
    m_recentNbr = settings.value("recentNbr", 7).toInt();
    m_pathCostm = settings.value("pathCostm", MW::pathUser + "/books").toString();
    message->m_pathCostum = m_pathCostm;
    //message->m_pathCostum = settings.value("pathCostm", MW::pathUser + "/books").toString();
    ui->actionTreeD->setChecked(settings.value("isThreeD", false).toBool());
    m_lng = settings.value("lng", 3).toInt();
    m_isCadre = settings.value("WebCadre", true).toBool();
    m_CadrFolder = settings.value("CadrFolder", "default").toString();
    m_iconsThemes = settings.value("iconsThemes", false).toBool();
    m_layouDir = settings.value("layouDir", false).toBool();
    m_arabicNmber = settings.value("arabicNmber", 0).toInt();
    m_isStandardToolBar = settings.value("isStandardToolBar", true).toBool();
    m_toolIconSize = settings.value("toolIconSize", 22).toInt();
    m_textUnderIcon = settings.value("textUnderIcon", false).toBool();
    m_styleCostum = settings.value("styleCostum", "").toString();
    m_isStyleCostum = settings.value("isStyleCostum", false).toBool();
    m_listActToAdd = settings
                         .value("listActToAdd", m_listActToAdd << 29 << 28 << 12 << 1000 << 15 << 16 << 17 << 18 << 1000 << 20 << 21 << 22 << 23 << 24 << 1000 << 26 << 48 << 49 << 2000)
                         .toList();
    if (m_layouDir)
        qApp->setLayoutDirection(Qt::LeftToRight);
    else
        qApp->setLayoutDirection(Qt::RightToLeft);

    view->threeD = settings.value("isThreeD", false).toBool();
    this->restoreGeometry(settings.value("geo_data").toByteArray());
    this->restoreState(settings.value("layout_data", layo_data).toByteArray());
    settings.endGroup();
    QFont font;
    font.fromString(m_WebFont);
    if (txtBrowserBook->objectName() != "textBrowser_0") {
        txtBrowserBook->setFont(font);
    } else {
        ui->textBrowser_0->setFontFamily(font.family());
    }
    creatStyle();
}

void MainWindow::creatStyle()
{
    if (m_isStyleCostum) {
        qApp->setStyle(("Cleanlooks"));
        QFile qss(m_styleCostum);
		if (!qss.open(QFile::ReadOnly))
			qWarning() << "Error:" << qss.errorString();return;
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        qDebug() << "m_styleCostum" << m_styleCostum;
    } else {
        qApp->setStyleSheet("");
		QApplication::setStyle(QStyleFactory::create( m_myStyleName == "" ? "qt6gtk2": m_myStyleName));
    }
}

// fahrrase ***  تحرير الفهرسةوالكتاب**********************************
void MainWindow::addItemInTreeFahras(bool child) // اظافة عنصر الى الفهرسة
{
    QString text = txtBrowserBook->textCursor().selectedText();
    if (text.isEmpty()) {
        return;
    }
    QTreeWidgetItem* curentitem; // العنصر المحدد
    QTreeWidgetItem* item; // العنصر الذي سنظيفه
    curentitem = treeViewFahras[m_currentIndex]->currentItem();
    if (child == false) { // اظافة عنصر جديد بعد المحدد
        if (!curentitem) { // اذا لم يكن هناك تحديد سيظاف الى الجذر
            item = new QTreeWidgetItem(treeViewFahras[m_currentIndex]);
        } else { // اما ان كان هناك عنصر محدد سيظاف بعده
            item = new QTreeWidgetItem(curentitem->parent(), curentitem);
            if (!curentitem->parent()) { // اذا لم يكن له اب سيضاف الى الجذر بعد
                                         // المحدد
                item = new QTreeWidgetItem(treeViewFahras[m_currentIndex], curentitem);
            }
        }
    }
    if (child == true) { // اظافة ابن الى المحدد
        if (!curentitem) {
            return;
        } // اذا لم يكن هناك تحديد سيخرج
        item = new QTreeWidgetItem(curentitem);
        if (item->parent()) {
            item->parent()->setExpanded(true);
        }
    }
    item->setText(0, text);

    item->setData(1, 1, databook->currentPosition[m_currentIndex] + 1);
    item->setSelected(true);
    treeViewFahras[m_currentIndex]->setCurrentItem(item); // تفعيل العنصر المظاف
    m_treeFahrasIsModified[m_currentIndex] = true;
}

void MainWindow::saveBookEndFahras() // زر حفظ الفهرسة المحررة
{
    if (m_treeFahrasIsModified[m_currentIndex] == true) {
        saveFahrass(m_currentIndex);
    }
    if (m_bookIsModified[m_currentIndex] == true) {
        saveCurrentBook(m_currentIndex);
    }
}

void MainWindow::saveFahrass(int index)
{
    message->m_pathCostum = m_pathCostm;
    if (message->fahrasSave(treeViewFahras[index], m_bookName[index]) == true) {
        QMessageBox::information(this, tr("معلومات"),
            tr("تمت عملية حفظ الفهرسة بنجاح"));

    } else {
        QMessageBox::information(this, tr("خطأ"), tr("خطأ في كتابة الملف"));
    }
    m_treeFahrasIsModified[index] = false;
}

// text browser***عمليات على النص**********************************
void MainWindow::textBookAnchorClicked(QUrl txtUrl)
{
    ui->textBrowser_0->setOpenExternalLinks(false);
    ui->textBrowser_0->setOpenLinks(false);
    QString textCode = txtUrl.toString();
    if (textCode.isEmpty()) {
        return;
    }
    if (textCode.contains("javascript")) {
        ui->textBrowser_0->setOpenLinks(true);
        return;
    }
    if (textCode.contains("#*")) {
        textCode.remove("#*");
        if (textCode.contains("(") || textCode.contains(":")) {
            int t = textCode.indexOf("(", 0);
            textCode.remove(t, textCode.length());
            int d = textCode.indexOf(":", 0);
            textCode.remove(d, textCode.length());
        }

        rowatInterface->rawiFind(textCode.trimmed(), ui->treeWidget_rowat,
            ui->treeWidget_rowatFind);
        showfind(true);

        //    m_isRawiToFind=true;

        return;
    }
    if (textCode.contains("http://")) {
        ui->textBrowser_0->setOpenLinks(false);
        ui->textBrowser_0->setOpenExternalLinks(false);
        QString nameBk = ui->textBrowser_0->textCursor().block().text();
        QMessageBox::information(this, "name", nameBk + "\n" + textCode);

    } else if (textCode.contains("file://") || textCode.contains("javascript")) {

        ui->textBrowser_0->setOpenLinks(true);
    } else {

        // QString nameTexte=textCode.section('/', 0,0);
        QString bookTexte = textCode.section('/', 1, 1);
        QString authorTexte = textCode.section('/', 2, 2);
        QString idBookTexte = textCode.section('/', 3, 3);
        QString pageTexte = textCode.section('/', -1);
        QVariant pos = pageTexte;
        if (m_bookName[m_currentIndex] == idBookTexte) {
            bookValueChanged(pos.toInt());
        } else {
            open_dataBase(idBookTexte, bookTexte, authorTexte, 0);
            bookValueChanged(pos.toInt());
        }
    }
}

void MainWindow::textBookHighlighted(QUrl txtUrl)
{
    QString textCode = txtUrl.toString();
    if (textCode.isEmpty() || textCode.contains("www.intro")) {
        ui->statusBar->showMessage("");
        return;
    }
    if (textCode.contains("#*")) {
        ui->statusBar->showMessage(textCode.remove("#*"));
        return;
    }
    if (textCode.contains("http://")) {
        ui->textBrowser_0->setOpenExternalLinks(false);
        ui->textBrowser_0->setOpenLinks(false);
        ui->statusBar->showMessage(txtUrl.toString());

    } else if (textCode.contains("file://")) {

        ui->textBrowser_0->setOpenLinks(true);
    } else {
        QString nameTexte = textCode.section('/', 0, 0);
        QString bookTexte = textCode.section('/', 1, 1);
        QString authorTexte = textCode.section('/', 2, 2);
        ui->statusBar->showMessage(nameTexte + tr(" كتاب :") + bookTexte + tr(" المؤلف :") + authorTexte);
    }
}

void MainWindow::convertTextToHtml(QString txt) // تحويل بيانات النص
{
    //  QLocale::setDefault(QLocale(QLocale::Arabic));
    
    if (m_arabicNmber == 1) {
        txtBrowserBook->setLocale(QLocale(QLocale::Latin));
    } else if (m_arabicNmber == 2) {
        txtBrowserBook->setLocale(QLocale(QLocale::Arabic));
    }
    txt.replace(QByteArray("\n"), QByteArray("<br>"));
    txt.replace(QByteArray("\r"), QByteArray("<br>"));
    txt.insert(txt.length(), "</font></body>");
    //*******************************************
    txt.replace("$", "");
    int id = databook->currentPosition[m_currentIndex] + 1;
    QVariant dd = id;
    QTreeWidgetItem* item = new QTreeWidgetItem;
    QList<QTreeWidgetItem*> found = treeViewFahras[m_currentIndex]->findItems(
        dd.toString(), Qt::MatchExactly | Qt::MatchRecursive,
        1); // تنشاء قائمة بكل العناصر
    QLocale lc(txtBrowserBook->locale().name());
    lc.setNumberOptions(QLocale::OmitGroupSeparator);
    QRegularExpression rxD("(\\d+)");

    int pos = 0;
    while (pos < txt.length()) {
        QRegularExpressionMatch match = rxD.match(txt, pos);
        if (!match.hasMatch())
            break;

        pos = match.capturedStart();
        txt.remove(pos, match.capturedLength());
        txt.insert(pos, lc.toString(match.captured(1).toInt()));

        pos += match.capturedLength();
    }

    foreach (item, found) {
        treeViewFahras[m_currentIndex]->setCurrentItem(item);
        QString title = " <span style='font-family:" + m_WebFontTitle + ";  font-size:" + m_WebFontTitleSize + "pt; color:" + m_WebFontTitleColor + ";'>" + item->text(0) + "</span>";
        QString txtItem = item->text(0).trimmed();

        txt.replace(txtItem + "<br>", "<br>" + title + "<br>");
        txt.replace("  " + txtItem, "<br>" + title + "<br>");
        // txt.replace("<br> "+ txtItem,"<br>"+title);
        txt.replace("<br>" + txtItem + "</font>", "<br>" + title + "</font>");
        txt.replace(txtItem + " <br>", "<br>" + title + "<br>");
        txt.replace(txtItem + " :", "<br>" + title + ":<br>");
        txt.replace(txtItem + ":", "<br>" + title + ":<br>");
        //  txt.replace( txtItem+".","<br>"+ title+"txt.<br>");
        txt.replace(txtItem + " ؟", "<br>" + title + " ؟<br>");
        txt.replace("[ " + txtItem + " ]", "<br>" + title + "<br>");
        txt.replace("[" + txtItem + "]", "<br>" + title + "<br>");

        if (txt.trimmed() == txtItem) { //
            txt = title;
        }
    }
    //********************************************xtToHtml
    //-************
    txt.replace("begen!§", " < font face='" + m_WebFontKhase + "'>");
    txt.replace("end!§", "</font>");
    //-***************
    QRegularExpression rxi("_____+");
    QRegularExpression rxd("ـــــ+");
    txt.replace(rxi,
        " < font face='" + m_WebFontHachia + "' size='1'><br>__________");
    txt.replace(rxd, " < font  face='" + m_WebFontHachia + "' size='1'><br>__________");
    txt.replace("صلى الله عليه وسلم", "< font color='" + m_WebFontPrefertColor + "' >صلى الله عليه وسلم</font> ");
    txt.replace("C تعالى", "< font color='" + m_WebFontPrefertColor + "' >رحمه الله تعالى</font> ");
    txt.replace("{", "< font color='" + m_WebFontPrefertColor + "' >﴿</font> ");
    txt.replace("}", "< font color='" + m_WebFontPrefertColor + "' >﴾</font> ");

    txt.replace("رضي الله عنهم ", tr("< font color='") + m_WebFontPrefertColor + tr("'>رضي الله عنهم </font> "));
    txt.replace("رضي الله عنه ", tr("< font color='") + m_WebFontPrefertColor + tr("'>رضي الله عنه </font> "));
    txt.replace("رضي الله عنهما ", tr("< font color='") + m_WebFontPrefertColor + tr("' >رضي الله عنهما </font> "));
    txt.replace(" A ", tr("< font color='") + m_WebFontPrefertColor + tr("'> صلى الله عليه سلم </font> "));
    txt.replace("Bه", tr("< font color='") + m_WebFontPrefertColor + tr("'>رضي الله عنه </font> "));
    txt.replace(" D ", tr("< font color='") + m_WebFontPrefertColor + tr("'>  تعالى </font> "));
    txt.replace(" E ", tr("< font color='") + m_WebFontPrefertColor + tr("'> عليه السلام </font> "));
    txt.replace("النبي e", tr("< font color='") + m_WebFontPrefertColor + tr("'> النبي صلى الله عليه سلم </font> "));

    if (txt.contains("**")) {
        QRegularExpression rx("(\\d\\d? )[(]");
        txt.replace(rx, "<br>(");
        QRegularExpression rx1("(\\d\\d?)[(]");
        txt.replace(rx1, "<br>(");
        txt.replace(") (", ")<br>(");
        txt.replace(")(", ")<br>(");
    }
    txt.replace("<br><br>", "<br>");
    txt.replace("<br> <br>", "<br>");
    txt.replace("<br>  <br>", "<br>");

    QString bgrcolor = "<body bgcolor='" + m_WebColorBack + "'>< font color='" + m_WebFontColor + "' >";

    if (txtBrowserBook->objectName() != "textBrowser_0") {
        txtBrowserBook->setHtml(bgrcolor + txt.trimmed());
    }
}

void MainWindow::saveCurrentBook(int index)
{
    databook->boocNumIndex = index;
    if (databook->saveBook() == true) {
        QMessageBox::information(this, tr("معلومات"),
            tr("تمت عملية حفظ الكتاب بنجاح"));
    }
    m_bookIsModified[index] = false;
}

// navigation***التقل بين السجلات*******************************
void MainWindow::bookValueChanged(int pos) // التنقل بين السجلات حسب قيمة
{
    if (m_bookName[m_currentIndex].isEmpty()) {
        AC_prevHistorie->setEnabled(false);
        AC_nextHistorie->setEnabled(false);
        return;
    }
    if (txtBrowserBook->document()->isModified()) {
        databook->updatPage("", txtBrowserBook->document()->toPlainText().trimmed(),
            true);
        m_bookIsModified[m_currentIndex] = true;
    }
    databook->moveToPosition(pos);
    if (m_listHistoriePrev[m_currentIndex].isEmpty()) {
        m_listHistoriePrev[m_currentIndex].append(
            databook->currentPosition[m_currentIndex]);
    } else if (m_listHistoriePrev[m_currentIndex].last() != databook->currentPosition[m_currentIndex]) {

        m_listHistoriePrev[m_currentIndex].append(
            databook->currentPosition[m_currentIndex]);
    }
    if (m_listHistoriePrev[m_currentIndex].count() > 1) {
        AC_prevHistorie->setEnabled(true);
    } else {
        AC_prevHistorie->setEnabled(false);
    }
    if (m_listHistorieNext[m_currentIndex].count() > 1) {
        AC_nextHistorie->setEnabled(true);
    } else {
        AC_nextHistorie->setEnabled(false);
    }
    convertTextToHtml(databook->bookNass[m_currentIndex]);
    QString page = databook->bookPage[m_currentIndex];
    QString part = databook->bookPart[m_currentIndex];

    labelPagePart->setText(tr("الصفحة :") + page + "  " + tr("الجزء :") + part);
    labelProgressInfo->setText(
        tr(" كتاب : ") + ui->tabWidget->tabText(ui->tabWidget->currentIndex()) + tr(".  المؤلف : ") + m_bookAuthor[m_currentIndex] + ". " + labelPagePart->text());
}

void MainWindow::historieNext()
{
    if (m_bookName[m_currentIndex].isEmpty()) {
        return;
    }
    if (m_listHistorieNext[m_currentIndex].isEmpty() || m_listHistorieNext[m_currentIndex].count() < 1) {
        AC_nextHistorie->setEnabled(false);
        return;
    }
    int p = m_listHistorieNext[m_currentIndex].last();
    m_listHistorieNext[m_currentIndex].removeLast();
    if (m_listHistorieNext[m_currentIndex].count() < 1) {
        AC_nextHistorie->setEnabled(false);
    }
    m_listHistoriePrev[m_currentIndex].append(p);
    AC_prevHistorie->setEnabled(true);
    databook->moveToPosition(p);
    convertTextToHtml(databook->bookNass[m_currentIndex]);
    QString page = databook->bookPage[m_currentIndex];
    QString part = databook->bookPart[m_currentIndex];
    labelPagePart->setText(tr("الصفحة :") + page + "  " + tr("الجزء :") + part);
    labelProgressInfo->setText(
        tr(" كتاب : ") + ui->tabWidget->tabText(ui->tabWidget->currentIndex()) + tr(".  المؤلف : ") + m_bookAuthor[m_currentIndex] + ". " + labelPagePart->text());
}

void MainWindow::historiePrev()
{
    if (m_bookName[m_currentIndex].isEmpty()) {
        return;
    }
    if (m_listHistoriePrev[m_currentIndex].isEmpty() || m_listHistoriePrev[m_currentIndex].count() < 2) {
        AC_prevHistorie->setEnabled(false);
        return;
    }

    int p = m_listHistoriePrev[m_currentIndex].last();
    m_listHistoriePrev[m_currentIndex].removeLast();
    if (m_listHistoriePrev[m_currentIndex].count() < 2) {
        AC_prevHistorie->setEnabled(false);
    }
    m_listHistorieNext[m_currentIndex].append(p);
    AC_nextHistorie->setEnabled(true);

    int i = m_listHistoriePrev[m_currentIndex].last();
    databook->moveToPosition(i);
    convertTextToHtml(databook->bookNass[m_currentIndex]);
    QString page = databook->bookPage[m_currentIndex];
    QString part = databook->bookPart[m_currentIndex];
    labelPagePart->setText(tr("الصفحة :") + page + "  " + tr("الجزء :") + part);
    labelProgressInfo->setText(
        tr(" كتاب : ") + ui->tabWidget->tabText(ui->tabWidget->currentIndex()) + tr(".  المؤلف : ") + m_bookAuthor[m_currentIndex] + ". " + labelPagePart->text());
}
// find***عمليات البحث***********************************

void MainWindow::find_favo_textChanged(
    QString searchString) // البحث في شجرة المفضلة
{
    if (searchString != tr("بحث"))
        findBook->searchInTreeview(ui->treeWidget_fav, searchString, 0);
}
void MainWindow::searchInTreeFah(QString searchString) // البحث في شجرة الفهرسة
{
    if (searchString != tr("بحث"))
        findBook->searchInTreeview(treeViewFahras[m_currentIndex], searchString, 0);
}
void MainWindow::searchInTreeBook(QString searchString) // البحث في شجرة الكتب
{
    int colum = 0;
    if (m_findIsTitle == false) {
        colum = 1;
    }
    if (searchString != tr("بحث"))
        findBook->searchInTreeview(ui->treeWidget_books, searchString, colum);
}
void MainWindow::showResultFind(QTreeWidgetItem* item) // تفعيل عناصر شجرة البحث
{

    QVariant bkpage = item->data(0, 1);

    QString x = item->data(3, 1).toString();
    open_dataBase(x, item->text(3), item->text(4), bkpage.toInt() - 1);
    bookValueChanged(bkpage.toInt() - 1);
    m_isTefsir[m_currentIndex] = item->data(2, 1).toBool();

    if (isNassToFind == true) {
        findBook->searchInDoc(txtBrowserBook->document(), m_textfind,
            QColor(m_WebFontFindColor));
    } else {
        QTreeWidgetItem* itemF = new QTreeWidgetItem;
        QList<QTreeWidgetItem*> found = treeViewFahras[m_currentIndex]->findItems(
            item->text(0), Qt::MatchExactly | Qt::MatchRecursive, 0);
        foreach (itemF, found) {
            treeViewFahras[m_currentIndex]->setCurrentItem(itemF);
        }
    }
}
void MainWindow::findInCurrentBook() // البحث في الكتاب الحالي
{
    findBook->pathCostum = m_bookPath[m_currentIndex];
    if (findBook->isFindRuned == true) {
        return;
    }
    if (m_bookName[m_currentIndex].isEmpty()) {
        return;
    }
    if (lineEditSearchInDoc->text().isEmpty()) {
        return;
    }
    if (lineEditSearchInDoc->text() == tr("بحث")) {
        return;
    }
    m_textfind = lineEditSearchInDoc->text();

    showfind(false);

    labelTextFind->setText(tr("نص البحث  : ") + m_textfind);
    progressBarFind->setMaximum(databook->rowsCount[m_currentIndex]);
    progressBarFind->setVisible(true);
    //  BtnStopFind->setVisible(true);
    ui->action_stopFind->setVisible(true);
    findBook->findMultiFind = false;
    findBook->findSawabik = true;
    findBook->findToList = false;
    findBook->resultCount = 0;
    findBook->isTefsir = m_isTefsir[m_currentIndex];
    findBook->findTitle = m_bookTitle[m_currentIndex];
    findBook->findAuthor = m_bookAuthor[m_currentIndex];
    findBook->findName = m_bookName[m_currentIndex];
    findBook->findText = m_textfind;
    findBook->progressBar1 = progressBarFind;
    findBook->label_progressImg = labelProgressImg;
    // findBook->pushButtonStop=BtnStopFind;
    findBook->buttonStop = ui->action_stopFind;
    findBook->treeView = ui->treeWidgetFind;
    findBook->isNassToFind = true;
    isNassToFind = true;

    ui->treeWidgetFind->clear();
    // QToolTip::showText(QPoint
    // (ui->statusBar->width(),ui->statusBar->pos().y()),tr("يمكنك ايقاف عملية
    // البحث هنا"),BtnStopFind);
    //  m_isRawiToFind=false;
    findBook->findOneBook(m_bookPath[m_currentIndex]);
}
void MainWindow::findBookOrHautor() // البحث عن كتاب
{
    if (ui->actionFindBkOrHt->isChecked()) {
        ui->actionFindBkOrHt->setToolTip(tr("البحث عن مؤلف"));
        m_findIsTitle = false;
    } else {
        ui->actionFindBkOrHt->setToolTip(tr("البحث عن كتاب"));
        m_findIsTitle = true;
    }
}

void MainWindow::stopFind() // توقيف عملية البحث
{
    findBook->stopFind = true;
}

void MainWindow::searchIndoc()
{
    if (lineEditSearchInDoc->text() == tr("بحث")) {
        return;
    }
    if (!lineEditSearchInDoc->text().isEmpty()) {
        findBook->findToList = false;
        if (findBook->searchInDoc(txtBrowserBook->document(),
                lineEditSearchInDoc->text(),
                QColor(m_WebFontFindColor))
            == false) {
            QMessageBox::information(this, tr("معلومات"),
                tr("النص غير موجود في الصفحة"));
        }
    }
}

void MainWindow::printPreview(QPrinter* printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    txtBrowserBook->print(printer);
#endif
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {

        QVariant txtdata = action->data();
        QString myid = txtdata.toString().section('/', 0, 0);
        QString myaut = txtdata.toString().section('/', 2, 2);
        QString myname = txtdata.toString().section('/', 1, 1);
        QVariant mytefsir = txtdata.toString().section('/', -1);
        open_dataBase(myname, action->text(), myaut, myid.toInt());
        m_isTefsir[m_currentIndex] = mytefsir.toBool();
    }
}
///*********************plugins********************

void MainWindow::on_comboBox_rowat_currentIndexChanged(int index)
{
    if (loadPluginRowat() == false) {
        QMessageBox::information(
            this, tr("تعليمات"),
            tr("تأكد من ان الاضافة موجودة في دليل البرنامج"));
        return;
    }
    QFile file( MW::pathApp + "data/rowaInfo.xml");

    if (index == 1) {
        if (!file.exists()) {
            QMessageBox::information(
                this, tr("تعليمات"),
                tr(
                    "قاعدة بيانات تراجم الرواة غير موجدة الرجاء تحميل قاعدة البيانات "
                    "من موقع المكتبة وقم بنسخها الى مجلد data في دليل البرنامج"));
            ui->comboBox_rowat->setCurrentIndex(0);
            return;
        }

        if (ui->treeWidget_rowat->topLevelItemCount() < 1)
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        rowatInterface->treeChargeRowtName(ui->treeWidget_rowat);
        // Rowat->chargeRowat();
        // Rowat->openXml();
        QApplication::restoreOverrideCursor();
    } else {
        on_actionPageIntro_triggered();
    }
    ui->stackedWidget_books->setCurrentIndex(index);
}

void MainWindow::on_lineEdit_FindRowat_textChanged(QString searchString)
{
    if (searchString != tr("بحث"))
        findBook->searchInTreeview(ui->treeWidget_rowat, searchString, 0);
}

void MainWindow::on_treeWidget_rowat_itemActivated(QTreeWidgetItem* item, int)
{
    ui->treeWidget_rowatFind->setVisible(false);
    openRawi(item, true);
}
void MainWindow::openRawi(QTreeWidgetItem* item, bool addToTataboa)
{
    int pos = item->data(1, 1).toInt();

    if (ui->tabIntro->isHidden()) {

        ui->tabWidget->insertTab(0, ui->tabIntro, tr("مكتبة القرطاس"));
        ui->stackedWidget->insertWidget(0, ui->pageIntro);
        ui->tabWidget->setCurrentWidget(ui->tabIntro);
    }
    ui->stackedWidget_intro->setCurrentWidget(ui->page_text);

    labelAnim->setText(tr("تراجم الرواة"));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->textBrowser_0->setHtml(rowatInterface->readxml(pos));
    //    ui->textBrowser_0->setHtml(Rowat->moveToPosition(pos));

    QApplication::restoreOverrideCursor();
    if (addToTataboa == true) {

        QTreeWidgetItem* itemR = new QTreeWidgetItem;
        QList<QTreeWidgetItem*> foundR = ui->treeWidget_rowatTabia->findItems(
            item->text(0), Qt::MatchExactly, 0); // تنشاء قائمة بكل العناصر

        foreach (itemR, foundR) {
            ui->treeWidget_rowatTabia->setCurrentItem(itemR);
            return;
        }
        QTreeWidgetItem* itemF = new QTreeWidgetItem(ui->treeWidget_rowatTabia);
        itemF->setText(0, item->text(0));
        itemF->setData(1, 1, ui->textBrowser_0->toHtml());
        ui->treeWidget_rowatTabia->setCurrentItem(itemF);
    }
}

void MainWindow::on_treeWidget_rowatTabia_itemActivated(QTreeWidgetItem* item,
    int)
{
    ui->treeWidget_rowatFind->setVisible(false);
    ui->textBrowser_0->setHtml(item->data(1, 1).toString());
}
void MainWindow::showfind(bool isrowat)
{
    //  ui->treeWidgetFind->clear();
    if (isrowat == true) {
        if (ui->treeWidget_rowatFind->topLevelItemCount() == 1) {
            openRawi(ui->treeWidget_rowatFind->topLevelItem(0), true);
            ui->treeWidget_rowatFind->setVisible(false);
            ui->treeWidget_rowatFind->clear();
        } else if (ui->treeWidget_rowatFind->topLevelItemCount() > 1) {
            ui->treeWidget_rowatFind->setVisible(true);
        } else {
            ui->treeWidget_rowatFind->setVisible(false);
            ui->treeWidget_rowatFind->clear();
            //              سعيد بن أبى هلال الليثى مولاهم//
            QMessageBox::information(
                this, tr("تراجم"),
                tr("ربما لم يترجم له في التهذيبين \n حاول البحث عنه في مربع "
                       "البحث اعلى قائمة الرواة"));
        }
    } else {
        ui->treeWidgetFind->setColumnWidth(0, 500);
        ui->treeWidgetFind->setColumnWidth(1, 30);
        ui->treeWidgetFind->setColumnWidth(2, 50);
        ui->toolBarFind->setVisible(true);
        ui->dockWidget_find->setVisible(true);
    }
}

void MainWindow::on_treeWidget_rowatFind_itemActivated(QTreeWidgetItem* item,
    int)
{

    openRawi(item, true);
}

void MainWindow::on_btnRowaInfo_clicked()
{
    QString romoz = tr(
        "<span style=\"color:#ff0000;\">خ</span>=البخاري , <span "
        "style=\"color:#ff0000;\">م</span>=مسلم , <span "
        "style=\"color:#ff0000;\">د</span>=أبو داود , <span "
        "style=\"color:#ff0000;\">ت</span>=الترمذي , <span "
        "style=\"color:#ff0000;\">س</span> =النسائي , <span "
        "style=\"color:#ff0000;\"><span "
        "style=\"color:#ff0000;\">ق</span></span>=ابن ماجه<br>"
        "<span style=\"color:#ff0000;\">بخ</span> =البخاري في الأدب المفرد , "
        "<span style=\"color:#ff0000;\">عخ</span> =البخاري في خلق أفعال "
        "العباد<br>"
        "<span style=\"color:#ff0000;\">سي</span>=النسائي في عمل اليوم والليلة, "
        "<span style=\"color:#ff0000;\">ي</span> =البخاري في جزء رفع اليدين<br>"
        "<span style=\"color:#ff0000;\">عس</span>= النسائي في مسند علي           "
        "           , <span style=\"color:#ff0000;\">مد</span>=أبو داود في "
        "المراسيل<br>"
        "<span style=\"color:#ff0000;\">تمييز</span>=لم يخرج له أحد من الستة     "
        "            , <span style=\"color:#ff0000;\">فق</span>=ابن ماجه في "
        "التفسير<br>"
        "<span style=\"color:#ff0000;\">خت</span> = البخاري تعليقا               "
        "      ,  <span style=\"color:#ff0000;\">صد</span>=أبو داود في فضائل "
        "الأنصار<br>"
        "<span style=\"color:#ff0000;\">خد</span>=أبو داود في الناسخ والمنسوخ    "
        "            ,  <span style=\"color:#ff0000;\">ل</span>=أبو داود في "
        "المسائل<br>"
        "<span style=\"color:#ff0000;\">ص</span>=النسائي في خصائص علي            "
        ", <span style=\"color:#ff0000;\">ر</span>=البخاري في جزء القراءة خلف "
        "الإمام<br>"
        "<span style=\"color:#ff0000;\">كن</span>=النسائي في مسند مالك , <span "
        "style=\"color:#ff0000;\">تم</span>=الترمذي في الشمائل , <span "
        "style=\"color:#ff0000;\">قد</span>=أبو داود في القدر");
    QMessageBox::information(this, tr("رموز كتب السنة"), romoz);
}

void MainWindow::textBrewserCustomMenu()
{

    QMenu* menu = txtBrowserBook->createStandardContextMenu();
    menu->setLayoutDirection(Qt::RightToLeft);
    menu->addSeparator();
    if (m_toolEditFahrass[m_currentIndex] == false) {
        menu->addAction(ui->action_next);
        menu->addAction(ui->action_prev);
        menu->addAction(ui->action_first);
        menu->addAction(ui->action_last);
    } else {
        menu->addAction(ui->actionFahAddItem);
        menu->addAction(ui->actionFahAddChild);
        menu->addSeparator();
        menu->addAction(ui->actionNewPageAfter);
        menu->addAction(ui->actionNewPageBefor);
        menu->addAction(ui->actionRemovePage);
        menu->addSeparator();
        menu->addAction(ui->actionFontCostum);
    }

    menu->exec(QCursor::pos());
}

void MainWindow::on_actionNewPageAfter_triggered()
{
    databook->insertPage(true);
    bookValueChanged(databook->currentPosition[m_currentIndex]);
    m_bookIsModified[m_currentIndex] = true;
}

void MainWindow::on_actionNewPageBefor_triggered()
{
    databook->insertPage(false);
    bookValueChanged(databook->currentPosition[m_currentIndex]);
    m_bookIsModified[m_currentIndex] = true;
}

void MainWindow::on_actionRemovePage_triggered()
{
    databook->removeCurPage();
    bookValueChanged(databook->currentPosition[m_currentIndex]);
    m_bookIsModified[m_currentIndex] = true;
}

void MainWindow::on_actionFontCostum_triggered()
{
    QString text = txtBrowserBook->textCursor().selectedText();
    txtBrowserBook->setText(txtBrowserBook->document()->toPlainText().replace(
        text, "begen!§" + text + "end!§"));
    databook->updatPage("", txtBrowserBook->toPlainText(), true);
    convertTextToHtml(databook->bookNass[m_currentIndex]);
    m_bookIsModified[m_currentIndex] = true;
}

void MainWindow::on_actionCreatNewBook_triggered()
{

    dialogupdat* dlgc = new dialogupdat(this);
    dlgc->creatNewBook = "creat";
    dlgc->m_pathCostum = m_pathCostm;
    dlgc->bookTitle = tr("كتاب جديد");
    dlgc->bookAutor = tr("مؤلف جديد");

    dlgc->recharge();
    if (dlgc->exec() == QDialog::Accepted) {
        chargeGroupe();
    }
    delete dlgc;
}

void MainWindow::on_actionDownloadBooks_triggered()
{
    if (loadPlugin() == false)
        return;

    QString targzName = netInterface->execPlugin();
    //  QString targzName=
    //  netInterface->loadFile("http://dl.dropbox.com/u/7206075/9/bk9_4639.tar.gz");
    if (targzName.isEmpty())
        return;
    QString orgPath = QDir::homePath() + "/.kirtasse/download/" + targzName;
    // QMessageBox::information(0,"orgPath",orgPath);

    QFile file;
    if (!file.exists(orgPath)) {

        return;
    }
    if (message->loadTarGz(orgPath) == false)
        return;
    if (!file.exists(QDir::homePath() + "/.kirtasse/download/bookinfo.info"))
        return;
    if (message->loadBookInfo(QDir::homePath() + "/.kirtasse/download") == false)
        return;

    dialogupdat* dlgc = new dialogupdat(this);
    dlgc->creatNewBook = "add";
    dlgc->setWindowTitle(tr("اضافة كتاب جديد"));
    dlgc->m_pathCostum = m_pathCostm;
    dlgc->bookTitle = message->infoBookTitle;
    dlgc->bookAutor = message->infoBookAutor;
    dlgc->bookBetaka = message->infoBookBetaka;
    dlgc->recharge();
    if (dlgc->exec() == QDialog::Accepted) {
        chargeGroupe();
        message->removeTempDirs(QDir::homePath() + "/.kirtasse/download");
    } else {
        message->removeTempDirs(QDir::homePath() + "/.kirtasse/download");
    }

    delete dlgc;
}

void MainWindow::on_actionShamilaCdrom_triggered()
{
    message->removeTempDirs(QDir::homePath() + "/.kirtasse/temp");
    if (loadPluginCdrom() == false)
        return;
    QString newBooksPath = shamilaInterface->execPlugin();

    if (newBooksPath.isEmpty())
        return;

    QMessageBox::information(
        this, tr("معلومات"),
        tr("لقد تم تحويل كتب الشاملة بنجاح\n"
               "كما تم اعادة تسمية القائمة السابقة في المسار التالي اذا احتجت "
               "لاسترجاعها")
            + QDir::homePath() + tr("/.kirtasse/data/group.xml.old\n"
                                        " كما سيتم استخدام المسار التالي للكتب \n")
            + newBooksPath);

    m_pathCostm = newBooksPath;
    chargeGroupe();
}

void MainWindow::on_actionControle_triggered()
{
    DialogConfigBooks* dlgc = new DialogConfigBooks(this);
    dlgc->setPathCostum(m_pathCostm);
    if (dlgc->exec() == QDialog::Accepted) {
        m_pathCostm = dlgc->pathCostm;
        chargeGroupe();
    }
    delete dlgc;
}

void MainWindow::on_actionDvd_triggered()
{
    DialogImportDvd* dlgc = new DialogImportDvd(this);
    dlgc->setBooksPath(m_pathCostm);
    dlgc->setAppPath( MW::pathApp);
    if (dlgc->exec() == QDialog::Accepted) {
        m_pathCostm = dlgc->getBooksPath();
        chargeGroupe();
    }
    delete dlgc;
}

void MainWindow::on_actionCostumActions_triggered()
{
    DialogActions* dlg = new DialogActions(this);
    dlg->setListActions(m_listActions, m_listActToAdd);
    if (dlg->exec() == QDialog::Accepted) {
        m_listActToAdd = dlg->listActToAdd;
        if (m_isStandardToolBar == true) {
            addToolRubonBar(true);
        }
    }
    delete dlg;
}
