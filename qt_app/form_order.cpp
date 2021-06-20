#include "form_order.h"
#include "ui_form_order.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QStyle>
#include <QString>
#include <vector>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QtSql>
#include <QSqlError>
#include <QThread>
#include <QSqlQuery>

Form_Order::Form_Order(QWidget *parent) : QDialog(parent), ui(new Ui::Form_Order){
    ui->setupUi(this);
    this->setWindowTitle("Order");
    myDB = database();
    window()->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,window()->size(),qApp->desktop()->availableGeometry()));
    setStyleSheet("background-color: rgb(224,243,176)");

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(ui->label,0,0);
    layout->addWidget(ui->lineEdit,0,1);
    layout->addWidget(ui->label_2,1,0);
    layout->addWidget(ui->lineEdit_2,1,1);
    layout->addWidget(ui->label_3,2,0);
    layout->addWidget(ui->lineEdit_3,2,1);
    layout->addWidget(ui->label_4,3,0);
    layout->addWidget(ui->lineEdit_4,3,1);
    layout->addWidget(ui->label_5,4,0);
    layout->addWidget(ui->lineEdit_5,4,1);
    layout->addWidget(ui->label_6,5,0);
    layout->addWidget(ui->lineEdit_6,5,1);
    layout->addWidget(ui->label_7,6,0);
    layout->addWidget(ui->comboBox,6,1);
    layout->addWidget(ui->process_order_record,7,0,1,0);
    this->setLayout(layout);
}

void Form_Order::on_show(){   
    ui->process_order_record->setText(get_mode(mode));

    QSqlQuery qr = myDB.executeQuery("SELECT * FROM customers");
    vector<int> cols{1};
    int row = 1;
    ui->comboBox->clear();
    int customerCount = qr.size();
    for(int i=1; i<=qr.size(); i++){
        QString cust = myDB.getCells(qr, row, cols);
        ui->comboBox->addItem(cust);
    }

    if(mode == ADD){
        qr = myDB.executeQuery("SELECT MAX(CONVERT(orderNumber,UNSIGNED INTEGER)) FROM orders");
        cols.clear();
        cols.push_back(0);
        row = 1;
        int n = myDB.getCells(qr, row, cols).toInt() + 1;
        ui->lineEdit->setText(QString::number(n));
    }
    else if(mode == UPDATE || mode == DELETE){
        qr = myDB.executeQuery("select * from orders");
        qr.next();
        ui->lineEdit->setText(qr.value(0).toString());
        ui->lineEdit_2->setText(qr.value(1).toString());
        ui->lineEdit_3->setText(qr.value(2).toString());
        ui->lineEdit_4->setText(qr.value(3).toString());
        ui->lineEdit_5->setText(qr.value(4).toString());
        ui->lineEdit_6->setText(qr.value(5).toString());

        QString qs = "select customerName from customers where customerNumber = ";
        qs.append(qr.value(6).toString());
        qr = myDB.executeQuery(qs);
        qr.next();
        qDebug() << customerCount;
        QString cName = qr.value(0).toString() + " ";

        //todo: refactor below solution
        for(int i=0; i<customerCount; i++){
            ui->comboBox->setCurrentIndex(i);
            QString aa = ui->comboBox->currentText();
            if(aa == cName){
                break;
            }
        }

        //fill_form_with_query_result(customerCount);
    }
}

void Form_Order::keyPressEvent(QKeyEvent *event){

}

QString Form_Order::get_mode(int m){
    if(m == ADD)
        return "Add";
    else if(m == UPDATE)
        return "Update";
    else if(m == DELETE)
        return "Delete";
    return "";
}

int Form_Order::get_next_order_code(){
    qr = myDB.executeQuery("SELECT MAX(CONVERT(orderNumber,UNSIGNED INTEGER)) FROM orders");
    vector<int> cols{0};
    int row = 1;
    return myDB.getCells(qr, row, cols).toInt() + 1;
}

void Form_Order::clear_form(){
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
    ui->lineEdit_5->setText("");
    ui->lineEdit_6->setText("");
}

void Form_Order::fill_form_with_query_result(){

}

void Form_Order::refresh_query(){

}

Form_Order::~Form_Order(){
    delete ui;
}

void Form_Order::on_process_order_record_clicked(){
    QString queryString;
    if( mode == ADD ){
        queryString = "SELECT customerNumber FROM customers WHERE customerName = ";
        queryString.append("'" + ui->comboBox->currentText() + "'");
        qr = myDB.executeQuery(queryString);
        vector<int> cols{0};
        int row = 1;
        QString customerNumber = myDB.getCells(qr, row, cols);

        queryString = "insert into `orders`(`orderNumber`,`orderDate`,`requiredDate`,`shippedDate`,`status`,`comments`,`customerNumber`) values (";
        queryString.append("'" + ui->lineEdit->text()   + "',");
        queryString.append("'" + ui->lineEdit_2->text() + "',");
        queryString.append("'" + ui->lineEdit_3->text() + "',");
        queryString.append("'" + ui->lineEdit_4->text() + "',");
        queryString.append("'" + ui->lineEdit_5->text() + "',");
        queryString.append("'" + ui->lineEdit_6->text() + "',");
        queryString.append(customerNumber + ")");
        myDB.executeQuery(queryString);
        qDebug() << queryString;

        QString nextOrderNumber = QString::number(ui->lineEdit->text().toInt() + 1);
        clear_form();
        ui->lineEdit->setText(nextOrderNumber);
    }
    else if( mode == UPDATE ){

    }
    else if( mode == DELETE ){

    }
}
