/****************************************************************************
**
** Copyright AbouZakaria.
** All rights reserved.
** Contact: AbouZakaria (yahia67@live.com)
**
** $elkirtasse_BEGIN_LICENSE:GPL$
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** AbouZakaria (yahia67@live.com)
** $elkirtasse_END_LICENSE$
**
****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "messages.h"
#include "findbook.h"
#include "databook.h"
#include "classwizard.h"
#include "rowat.h"
//#include "kirtase/booksInterface.h"
#if QT_VERSION >= 0x040600
    #include "animation.h"
#endif

#include <QTextBrowser>
#include <QtGui/QMainWindow>
#include <QTreeWidgetItem>
#include <QDomDocument>
 #include <QStandardItem>
class QLabel;
class QTreeWidget;
class QLabel;
class QProgressBar;
class QComboBox;
class QPushButton;
class QLineEdit;


namespace Ui
{
    class MainWindowClass;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
    messages *Messages;
    findbook *Findbook;
    ClassWizard *Wizard;
    rowat *Rowat;
#if QT_VERSION >= 0x040600
    View *view;
#endif
public:
    MainWindow(QWidget *parent=0);
    ~MainWindow();
    databook *DataBook;

protected:
    virtual void closeEvent(QCloseEvent *e);
     bool eventFilter(QObject *obj, QEvent *ev);
signals:
    void bookAvailable(bool );
private:
    Ui::MainWindowClass *ui;
    //BooksInterface *booksInterface;
    QGraphicsScene m_scene;

    int m_currentIndex  ;
   // enum  { NumIndex = 10};
  // enum { MaxRecentFiles = 15 };
    static const int NumIndex=10 ;
    static const int MaxRecentFiles=16 ;
    int m_recentNbr;
    QTextBrowser *txtBrowserBook;

    QIcon m_icondir;
    QIcon m_icondfile;
    QIcon m_iconbook;
    QIcon m_iconaut;
    QIcon m_iconGroup;

    QTreeWidget *treeViewFahras[NumIndex];

    QLineEdit *lineEditSearchInDoc;
    QLineEdit *lineEditFindFah;


    QAction *AC_nextHistorie;
    QAction *AC_prevHistorie;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *AC_GoSearchInDoc;
    QAction *AC_GoSearchInCurBook;
    QAction *AC_bookRemove;                    //قائمة حذف الكتاب
    QAction *AC_bookOpenInNewTab;
    QAction *AC_bookRename;
    QAction *AC_bookOpen;                      //قائمة فتح كتاب
    QAction *AC_bookUpdat;                     //قائمة تعديل معلومات الكتاب
    QAction *AC_bookPath;
    QAction *AC_groupeRemove;
    QAction *AC_groupeAdd;
    QAction *AC_categorieAdd;
    QAction *AC_groupeRename;
    QAction *AC_colapsTreeFah;
    QAction *AC_expandTreeFah;

    QPushButton *BtnStopFind;

    QLabel *labelPagePart;                         //عنوان الصفحة
    QLabel *labelProgressInfo;                //معلومات عن الكتاب المفتوح
    QLabel *labelProgress;                     //عنوان شريط المعلومات
    QLabel *labelProgressImg;
    QLabel *labelBetaka;
    QLabel *labelAnim;
    QLabel *labelTextFind;

    QProgressBar *progressBarFind;                   //شريط الحالة للبحث

    QString m_textfind;                           //النص الذي يتم البحث عنه
    QString m_pathUser;                               //مسار البرنامج
    QString m_pathApp;
    QString m_pathCostm;
    QString m_bookName[NumIndex];                          //اسم مسار الكتاب الحالي
    QString m_bookTitle[NumIndex];                         //اسم الكتاب الحالي
    QString m_bookAuthor[NumIndex];                        //اسم المؤلف
    QString m_bookBetaka[NumIndex];
    QString m_bookPath[NumIndex];
    QString m_myStyleName;                            //السمة الحالية
    QString m_WebColorBack;                           //لون خلفية النص
    QString m_WebFont;                            //نوع النص
    QString m_WebFontTitle;
    QString m_WebFontTitleSize;
    QString m_WebFontHachia;
    QString m_WebFontColor;
    QString m_WebFontTitleColor;
    QString m_WebFontFindColor;
    QString m_WebFontPrefertColor;
QString m_CadrFolder;
QList<QString > listLineEditName;
    QList<QString> m_listTab;
    QList<int> m_listHistorieNext[NumIndex];
    QList<int> m_listHistoriePrev[NumIndex];
    bool isNassToFind;
    bool m_isTefsir[NumIndex];                                //هل كتاب التفسير يحتوي على فهرسة الايات والسور
    bool m_treeFahrasIsModified[NumIndex];
    bool m_toolEditFahrass[NumIndex];
    bool m_bookIsModified[NumIndex];
    bool m_treeGroupIsModified;
    bool m_findIsTitle;

    bool m_isCadre;
public slots:

private slots:

//**menuFile**


    void on_btnGoFindFah_clicked();
    void on_action_import_triggered();        //استراد ملفات نصية
    void on_actionMdb_triggered();            //كتب الشاملة
    void on_action_addBoook_triggered();      //تنفيذ نافذة اظافة كتاب
    void on_actionExport_triggered();         //تصدير الكتب
    void on_actionArchive_triggered();        //ضغط الكتاب
    void on_actionBookInfo_triggered();       //معلومات الكتاب الحالي
    void on_actionNewTab_triggered();         //لسان جديد
    void on_actionPrint_triggered();          //الطباعة
    void on_actionPrintPreview_triggered();   //معاينة قبل الطباعة
    void on_action_close_triggered();         //خروج
 //**menuEdit**
    void on_action_first_triggered();            //الصفحة الاولى
    void on_action_next_triggered();             //الصفحة التالية
    void on_action_prev_triggered();             //الصفحة السابقة
    void on_action_last_triggered();             //اخر صفحة
    void on_actionGotoPage_triggered();          //التنقل الى صفحة معينة
    //--------
    void on_actionFind_all_triggered();        //تنفيذ البحث في مجموعة كتب
    //----------
    void on_action_edit_triggered();          //زر وقائمة تفعيل التحرير
    void on_action_favorit_triggered();       //اضف للمفضلة
    void on_actionHiperlink_triggered();      //ارتباط تشعبي
    //---------
    void on_actionOrganizeFahrasse_triggered(); //ضبط روابط الفهرسة
    //---------
    void on_actionUpdatGroupRemove_triggered(); //حذف الكتب الغير متوفرة
    void on_actionUpdatGroup_triggered();       //تمييز الكتب الغير متوفرة
//**menuView**

    void on_action_fulscreen_triggered();       //ملأ الشاشة
    void on_actionOption_triggered();           //خيارات
    void on_actionRemoveTechkil_triggered();    //التشكيل
    void on_actionZoomOut_triggered();
    void on_actionZoumIn_triggered();
    //------------
    void on_actionPageIntro_triggered();        //صفحة البداية
//**menuHelp**
    void on_action_about_triggered();           //حول البرنامج
    void on_actionHelp_triggered();             //التعليمات
//void on_actionHtmlFile_triggered();          //تحميل الكتب
 void on_actionWhatsThis_triggered();
//**treeViewBook**
    void on_treeWidget_books_itemSelectionChanged();                    //عند تغير التحديد في شجرة الكتب
    void on_treeWidget_books_itemActivated(QTreeWidgetItem* item);      //تفعيل عنصر شجرة  الكتب
    void on_treeWidget_books_customContextMenuRequested();       //انشاء قائمة لشجرة الكتب
    void menuOpenBook();                           //تنفيذ قائمة فتح الكتاب
    void menu_BookRemove();                   //تنفيذ عملية حذف الكتاب
    void menu_BookUpdat();                    //تحرير بيانات الكتاب
    void menu_BookPath();                    //معرفة مسار الكتاب
    void menu_renameBook();                  //اعادة تسمية دليل الكتاب
    void menu_GroupeAdd();                   //اضافة مجموعة جديدة
    void menu_CategorieAdd();                //اضافة قسم جديد
    void menu_Add(bool categorie);           //
    void menu_GroupeRemove();                //حذف المجموعة
    void menu_GroupeRename();                //تسمية المجموعة
//**treeViewFahras**
    void fahras_itemActivated(QTreeWidgetItem* item);    //تفعيل عنصر شجرة الفهرسة
    void fahras_itemClicked(QTreeWidgetItem* item);       //النقر على عنوان الفهرسة
    void on_pushButtonEditClose_clicked();
    void itemFahrasAddChild();
    void itemFahrasAdd();
    void itemFahrasRename();
    void itemFahrasMoveUp();          //نقل عنصر للاعلى في الفهرسة
    void itemFahrasMoveDown();        //للاسفل
    void itemFahrasRemove();      //حذف العنصر

//**treeViewFavorit**
    void on_treeWidget_fav_itemActivated(QTreeWidgetItem* item);
    void on_actionFavItemRen_triggered();
    void on_actionFavItemDown_triggered();
    void on_actionFavItemUp_triggered();
    void on_actionFavDelete_triggered();
    void on_actionFavSave_triggered();
    void find_favo_textChanged(QString );  //البحث في شجرة المفضلة

//**treeVeiwCuran**
    void on_treeWidget_curaan_itemActivated(QTreeWidgetItem* item);     //تفعيل عنصر شجرة القرءان الكريم
    void on_pushButton_gofind_clicked();
    void on_lineEdit_find_cur_textChanged(QString );
    void on_comboBox_currentIndexChanged(int index);             //تحميل الاجزاء او السور
    void on_pushButton_gotafsir_clicked();          //تحمبل كتاب التفسير

//**treeViewFind**
    void showResultFind(QTreeWidgetItem* item);         //النقر على عنصر شجرة نتائج البحث
    void on_actionFindDelete_triggered();
    void on_actionFindItemDown_triggered();
    void on_actionFindItemUp_triggered();
    void on_actionFindLoad_triggered();
    void on_actionFindSave_triggered();

//**Animation**
    void on_actionTreeD_toggled(bool );
    void on_actionAnimHistorie_triggered();
    void on_actionAnimGroup_triggered();
    void on_lineEdit_textChanged(QString );
    void startAnimationBitaka();
    void startAnimationPixmap();

//**tabs**
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_currentChanged(int );

     void tabCreatFahrass();
     void tabCreatTabNew();
     void tabOpenInNewTab();
void tabCreatCadre(QGridLayout *gridLayout);

//rowat
void on_btnRowaInfo_clicked();
void on_treeWidget_rowatFind_itemActivated(QTreeWidgetItem* item, int );
void showfind(bool isrowat);
void on_treeWidget_rowatTabia_itemActivated(QTreeWidgetItem* item, int );
void openRawi(QTreeWidgetItem* item,bool addToTataboa);
void on_treeWidget_rowat_itemActivated(QTreeWidgetItem* item, int );
void on_lineEdit_FindRowat_textChanged(QString searchString);
void on_comboBox_rowat_currentIndexChanged(int index);

//-********functions**************

void historieNext();
void historiePrev();
void openRecentFile();
void findInCurrentBook();              //البحث في الكتاب الحالي


    void printPreview(QPrinter *printer);
    void saveFahrass(int index);
   // bool loadPlugin();

    void open_dataBase(QString name,QString title,QString Autor,int position);  //فتح الكتاب المحدد

    bool closeMessages(QString title,int index);

    void findBookOrHautor();                    // مؤلف البحث عن كتاب
    void searchInTreeFah(QString );//البحث في شجرة الفهرسة
    void searchInTreeBook(QString );        //البحث في شجرة الكتب

    void stopFind();                  //توقيف عملية البحث
    void searchIndoc();                 //البحث في الصفحة الحالية
    //عمليات التحميل
    void chargeGroupe();                //تحميل مجموعة الكتب
    void updateIconEndAction();
    //عمليات السمات والموضع والاحجام
    void saveLayou();                     //حفظ البيانات للموضع والسمة
    void loadLayout();                    //تحميل بيانات

    void bookValueChanged(int pos);          //تغير رقم السبين للتنقل بين الصفحات

    void saveCurrentBook(int index);
    // fahrasse عمليات تحرير الفهرسة

    void addItemInTreeFahras(bool child);              //اظافة عنصر الى الفهرسة
      //******
    void saveBookEndFahras();//حفظ الفهرسة
    //عملية تحويل النصوص
    void convertTextToHtml(QString txt);           //عملية تحويل النص
    void textBookHighlighted(QUrl );
    void textBookAnchorClicked(QUrl );
    void textBrewserCustomMenu();


};

#endif // MAINWINDOW_H
