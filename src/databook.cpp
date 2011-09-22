#include "databook.h"

databook::databook()
{
    boocNumIndex=0;
    currentPosition[boocNumIndex]=0;
    //prevPosition[boocNumIndex]=0;
    newPosition[boocNumIndex]=0;
    removeTechkile=false;
}
databook::~databook()
{

}
bool  databook::openBook(QString bookName,int position,QString bkpath)
{


    QFile file(bkpath);
    if( !file.open(QIODevice::ReadOnly))                            //فتح الملف
    {

        return false;
    }

    claerBook(boocNumIndex);                                                    //تفريغ الذاكرة
    if (!m_docBooK[boocNumIndex].setContent(&file))                                      //اذا كانت البيانات صحيحة
    {

        return false;
    }
    m_currentBookName[boocNumIndex]=bookName;
    m_currentBookPath[boocNumIndex]=bkpath;
    QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
    QVariant i = racine.childNodes().count();
    rowsCount[boocNumIndex]=i.toInt()-1;
    file.close();

    moveToPosition(position);
    isTefsir=getIfTefsir();
    return true;
}
void databook::claerBook(int num)
{
    m_docBooK[num].clear();
    bookNass[num].clear();
    listId.clear();
}
bool databook::getIfTefsir()
{

    for(int i=0;i<100;i++){
        QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
        QDomNode noeud = racine.childNodes().item(i);

        QVariant aya=   noeud.firstChildElement("aya").firstChild().toText().data();
        QVariant sora=  noeud.firstChildElement("sora").firstChild().toText().data();
        if(aya.toString().isEmpty()){
            return false;
        }

        if(aya.toInt()>0 && sora.toInt()>0){
            return true;
        }
    }
    return false;

}

void databook::moveToPosition(int position)
{
    int pos=position;
    //prevPosition[boocNumIndex]=currentPosition[boocNumIndex];

    //  if (position==currentPosition[boocNumIndex]){return;}
    QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
    QDomNode noeud = racine.childNodes().item(pos);


    if (removeTechkile==false){
        bookNass[boocNumIndex]=   noeud.firstChildElement("nass").firstChild().toText().data();
    }else{
        bookNass[boocNumIndex]=noTechkile(noeud.firstChildElement("nass").firstChild().toText().data()) ;
    }
    bookCurentId[boocNumIndex]=  noeud.firstChildElement("id").firstChild().toText().data();
    bookPage[boocNumIndex]=  noeud.firstChildElement("page").firstChild().toText().data();
    bookPart[boocNumIndex]=  noeud.firstChildElement("part").firstChild().toText().data();
    //convert_text(text);
    currentPosition[boocNumIndex]=pos;
    //m_label_page->setText(trUtf8("الصفحة :")+page +"  "+trUtf8("الجزء :")+jozaa);
    // m_label_progress_info->setText(trUtf8(" كتاب : ")+m_Book_title +trUtf8(".  المؤلف : ")+m_Book_Autors+". " +m_label_page->text());
}
QString databook::noTechkile(QString text) const
{
    QString str=text;
    str.replace(trUtf8("\331\213"),QString::null);
    str.replace(trUtf8("\331\214"),QString::null);
    str.replace(trUtf8("\331\215"),QString::null);
    str.replace(trUtf8("\331\216"),QString::null);
    str.replace(trUtf8("\331\217"),QString::null);
    str.replace(trUtf8("\331\220"),QString::null);
    str.replace(trUtf8("\331\221"),QString::null);
    str.replace(trUtf8("\331\222"),QString::null);
    return str;
}

bool databook::findAya(QString soura,QString aya)
{
    QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
    QString textsora;
    QString textaya;
    //    QString id;
    int r;
    for(r=rowsCount[boocNumIndex];r>0;r--){
        QDomNode noeud = racine.childNodes().item(r);
        textsora=   noeud.firstChildElement("sora").firstChild().toText().data();

        if(textsora==soura){
            textaya=  noeud.firstChildElement("aya").firstChild().toText().data();
            if( textaya.toInt()<=aya.toInt()){
                //    id=  noeud.firstChildElement("id").firstChild().toText().data();
                moveToPosition(r);
                return true;
            }
        }
    }
    return false;
}
void databook::updatPage(QString oldText,QString newText,bool fullText)
{
    if (fullText==true){
        bookNass[boocNumIndex]=newText;
    }else{
        bookNass[boocNumIndex].replace(oldText,newText.trimmed());
    }

    QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
    QDomNode noeud = racine.childNodes().item(currentPosition[boocNumIndex]);
    noeud.firstChildElement("nass").firstChild().setNodeValue(bookNass[boocNumIndex]);
}
bool databook::saveBook()
{
    // QString path=QCoreApplication::applicationDirPath ();
    //مسار الكتاب
    QFile file( m_currentBookPath[boocNumIndex]);
    if ( !file.open(QIODevice::WriteOnly)){
        return false;
    }//فتح الملف للكتابة
    QTextStream out(&file);                       //الكتابة
    m_docBooK[boocNumIndex].save(out,1);                      //حفظ الملف
    file.close();
    m_docBooK[boocNumIndex].clear(); QString str,id;
    return true;
}


void databook::treeOrganizFahrass(QTreeWidget *view,QString Bname)
{

    if( chargeList()==true){

        QDomDocument m_doc;
        QString titlepath= Bname +"/title.xml";
        QFile file(titlepath);
        file.open(QIODevice::ReadOnly);
        if (!m_doc.setContent(&file)){return;}
        view->clear();
        int d=0;
        QTreeWidgetItem *item[d];
        QString tit;
        QString id;
        QString lvl;
        QDomElement racine = m_doc.documentElement(); //renvoie la balise racine
        QDomNode noeud = racine.firstChild();

        QProgressDialog progress(trUtf8("الرجاء الانتظار...."), trUtf8("الغاء"), 0, racine.childNodes().count(), 0);
        progress.setWindowModality(Qt::WindowModal);
        progress.setEnabled(false);
        progress.show();
        qApp->processEvents();

        item[1]= new QTreeWidgetItem(view);
        item[1]->setText(0,trUtf8("بسم الله الرحمن الرحيم") );
        item[1]->setData(1,1,1);
        int r=0;
        while(!noeud.isNull())
        {
            ++r;

            progress.setValue(r);
            qApp->processEvents();
            QDomNodeList tab;
            QDomElement mesures;
            mesures=noeud.toElement();
            if (mesures.tagName()== "title")
            {
                tab = mesures.childNodes();
                for(int i=0;i<(int)tab.length();++i)
                {
                    QDomNode noeud2=tab.item(i);
                    QDomElement mesures2=noeud2.toElement();
                    QString text=noeud2.firstChild().toText().data();
                    if (mesures2.tagName()=="lvl"){lvl=text.trimmed() ;}
                    if (mesures2.tagName()=="tit"){tit=text ;}
                    if (mesures2.tagName()=="id"){

                        int dd=getPositionID(text) ;
                        QVariant ddv=dd;
                        id= ddv.toString();
                        // QMessageBox::information(this,trUtf8("معلومات"),id);
                    }

                }
                bool ok;
                d=lvl.toInt(&ok,0);
                if (ok==true){
                    if (d==1){
                        item[1]= new QTreeWidgetItem(view);
                        item[1]->setText(0,tit );
                        item[1]->setText(1,id );
                        item[1]->setData(1,1,id);
                    }else{
                        if (item[d-1]){
                            item[d]= new QTreeWidgetItem( item[d-1]);
                            item[d]->setText(0,tit );
                            item[d]->setText(1,id );
                            item[d]->setData(1,1,id);
                        }

                    }
                }else{
                    item[1]= new QTreeWidgetItem(view);
                    item[1]->setText(0,tit );
                    item[1]->setText(1,id );
                    item[1]->setData(1,1,id);
                }
            }
            noeud = noeud.nextSibling();
        }
        m_doc.clear();
        file.close();
    }
}

bool  databook::chargeList()
{
    listId.clear();
    QDomElement racine = m_docBooK[boocNumIndex].documentElement(); //renvoie la balise racine
    int count = racine.childNodes().count();

    QProgressDialog progress(trUtf8("الرجاء الانتظار...."), trUtf8("الغاء"), 0, count, 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setEnabled(false);
    progress.show();
    qApp->processEvents();

    for (int i=1;i<count;++i){
        progress.setValue(i);
        qApp->processEvents();
        QDomNode noeud = racine.childNodes().item(i);
        listId.append( noeud.firstChildElement("id").firstChild().toText().data());
    }
    return true;
}

int  databook::getPositionID(QString id)
{

    int newid=listId.indexOf(id);
    if (newid==-1){newid=1;}
    if (newid==0){newid=1;}
    return newid+2;
}

