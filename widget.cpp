#include "widget.h"
#include "ui_widget.h"
#include<QMouseEvent>
#include<opencv2/opencv.hpp>
#include<QFileDialog>
#include "QDebug"
#include<unistd.h>
#include<ctime>
#include <QMessageBox>
#include<QPainter>
#include<mysql/mysql.h>

#define waittime_result 15000

using namespace cv;
using namespace std;



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(select_video()) );
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(play_or_pause()) );
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(speedup_play()) );
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slowdown_play()) );
    connect(ui->horizontalSlider,SIGNAL(sliderReleased()),this,SLOT(change_play_process())  );
    connect(ui->horizontalSlider,SIGNAL(sliderPressed()),this,SLOT(press_slider())  );
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(capture_image()) );
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(search_ped()) );
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(play_choosed1()) );
    connect(ui->pushButton_8,SIGNAL(clicked()),this,SLOT(play_choosed2()) );
    connect(ui->pushButton_9,SIGNAL(clicked()),this,SLOT(play_choosed3()) );
    connect(ui->pushButton_10,SIGNAL(clicked()),this,SLOT(play_choosed4()) );
    connect(ui->pushButton_11,SIGNAL(clicked()),this,SLOT(play_choosed5()) );
    connect(ui->pushButton_12,SIGNAL(clicked()),this,SLOT(play_choosed6()) );
    connect(ui->pushButton_13,SIGNAL(clicked()),this,SLOT(play_choosed7()) );
    connect(ui->pushButton_14,SIGNAL(clicked()),this,SLOT(play_choosed8()) );
    connect(ui->pushButton_15,SIGNAL(clicked()),this,SLOT(play_choosed9()) );
    connect(ui->pushButton_16,SIGNAL(clicked()),this,SLOT(play_choosed10()) );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::select_video()
{
    int height = ui->label->height();
    int width = ui->label->width();
    Mat backgroundimg;
    QImage img;
    QPixmap qpix;
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("open video");
    fileDialog->setDirectory("/home/ansheng/Desktop/hikvision");
    fileDialog->setNameFilter(tr("(*.mp4 *.mkv)"));
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileName;
    if(fileDialog->exec())
      {
         fileName = fileDialog->selectedFiles();
      }
    videoname = fileName.at(0).toStdString();
    name = videoname;
    cap = VideoCapture(videoname);
    cap>>play_frame;
    backgroundimg = play_frame;
    cvtColor(backgroundimg,backgroundimg,CV_BGR2RGB);
    img = QImage((const unsigned char*)(backgroundimg.data),backgroundimg.cols,backgroundimg.rows,backgroundimg.cols*backgroundimg.channels(),QImage::Format_RGB888);
    qpix = QPixmap::fromImage(img);
    qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
    ui->label->setPixmap(qpix);
    frame_length = cap.get(CV_CAP_PROP_FRAME_COUNT);
    frame_position = 1;
    ui->horizontalSlider->setMaximum(frame_length-3);
    ui->horizontalSlider->setMinimum(1);
    ui->horizontalSlider->setValue(frame_position);
}



void Widget::play_or_pause()
{
    cout<<waittime_result+1<<endl;
    playorpause = !playorpause;
    QImage img;
    QPixmap qpix;
    int height = ui->label->height();
    int width = ui->label->width();
//    ui->label->resize(1280,720);

    while(play_frame.data && playorpause)
    {
        frame_position += 1;
        ui->label_2->setText(QString::number(frame_position, 10));
        ui->horizontalSlider->setValue(frame_position);
        cap>>play_frame;
        cvtColor(play_frame,play_frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(play_frame.data),play_frame.cols,play_frame.rows,play_frame.cols*play_frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label->setPixmap(qpix);
        usleep(waittime);
        waitKey(5);
    }
}

void Widget::speedup_play()
{
    if(speed<10)
    {
        speed = speed+1;
        waittime = waittime/speed;
    }
}

void Widget::slowdown_play()
{
     waittime = waittime + 3000;
}

void Widget::press_slider()
{
    playorpause = !playorpause;
}

void Widget::change_play_process()
{
    frame_position = ui->horizontalSlider->value();
    cap.set(CV_CAP_PROP_POS_FRAMES,frame_position);
    ui->pushButton_2->click();
}

void Widget::capture_image()
{
    Mat captured;
    captured = play_frame;
    int height = ui->label_3->height();
    int width = ui->label_3->width();
    QImage img;
    QPixmap qpix;
//    cvtColor( play_frame, play_frame,CV_BGR2RGB);
    img = QImage((const unsigned char*)( play_frame.data), play_frame.cols, play_frame.rows, play_frame.cols* play_frame.channels(),QImage::Format_RGB888);
    qpix = QPixmap::fromImage(img);
    qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
    ui->label_3->setPixmap(qpix);

    QMessageBox::information(this, QString::fromLocal8Bit("information"),QString::fromLocal8Bit("imageSaved"));

}

void Widget::search_ped()
{
    string cameraid = videoname.substr(videoname.find_last_of("/")+1,videoname.find(".")-videoname.find_last_of("/")-1);
    vector<string> temp_result;
    vector<string> final_result;
    string sql = "select * from ReId where start<";
    int position = frame_position;
    sql = sql+to_string(position) +" and end > " +to_string(position)+" and cameraId = "+"'"+cameraid+"'"+" ;";
//    cout<<sql<<endl;
    MYSQL mysql;
    mysql_init(&mysql);
    mysql_real_connect(&mysql,"localhost","root","neuadmin","first",3306,NULL,0);

    mysql_query(&mysql,sql.c_str());
    MYSQL_RES *result = NULL;
    result = mysql_store_result(&mysql);

    MYSQL_ROW row = NULL;
    row = mysql_fetch_row(result);
    while(NULL != row)
    {
        for(int i=1;i<6;i=i+1)
        {
            temp_result.push_back(row[i]);
        }
        row = mysql_fetch_row(result);
    }

    mysql_free_result(result);
    mysql_close(&mysql);
    final_result.push_back(temp_result[0]);
    bool readd=false;
    for(int i=1;i<temp_result.size();i=i+1)
    {
        readd=false;
        for(int j=0;j<final_result.size();j=j+1)
        {
            if(temp_result[i] == final_result[j])
            {
                readd = true;
            }
        }
        if (!readd)
        {
            final_result.push_back(temp_result[i]);
        }
    }
    top_choosed_videos = final_result;
//    for(int i=0;i<top_choosed_videos.size();i=i+1)
//    {
//        cout<<top_choosed_videos[i]<<endl;
//    }
    num_choosed_videos = final_result.size();
    string path_with_slash = videoname.substr(0,videoname.find_last_of("/")+1); //for exmple /home/ansheng/desktop/      with the last slash
//    cout<<path_with_slash<<endl;
    VideoCapture top_share;
    Mat frame_share;
    string list_sample;
    string camera_share;
    string position_start_end;
    string position_start,position_end;
    string video_share;
    string label_name;
    int ps,pe;
    QImage img;
    QPixmap qpix;
    int height = ui->label_3->height();
    int width = ui->label_3->width();
    for(int i = 0 ;i<num_choosed_videos;i=i+1)
    {
        list_sample = top_choosed_videos[i];
        camera_share = list_sample.substr(0,list_sample.find("_"));
        position_start_end = list_sample.substr(list_sample.find("_ps")+3,list_sample.length());
        position_start = position_start_end.substr(0,position_start_end.find("_"));
        position_end = position_start_end.substr(position_start_end.find("_pe")+3,position_start_end.find(".")-position_start_end.find("_pe")-3);
        video_share = path_with_slash + camera_share + ".mp4";
        ps = stoi(position_start);
        pe = stoi(position_end);
        label_name = "label_" + to_string(i+4);
        top_share = VideoCapture(video_share);
        top_share.set(CV_CAP_PROP_POS_FRAMES,ps);
        top_share>>frame_share;
        cvtColor(frame_share,frame_share,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame_share.data),frame_share.cols,frame_share.rows,frame_share.cols*frame_share.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        if(0 == i)
        {
            ui->label_4->setPixmap(qpix);
        }
        if(1 == i)
        {
            ui->label_5->setPixmap(qpix);
        }
        if(2 == i)
        {
            ui->label_6->setPixmap(qpix);
        }
        if(3 == i)
        {
            ui->label_7->setPixmap(qpix);
        }
        if(4 == i)
        {
            ui->label_8->setPixmap(qpix);
        }
        if(5 == i)
        {
            ui->label_9->setPixmap(qpix);
        }
        if(6 == i)
        {
            ui->label_10->setPixmap(qpix);
        }
        if(7 == i)
        {
            ui->label_11->setPixmap(qpix);
        }
        if(8 == i)
        {
            ui->label_12->setPixmap(qpix);
        }
        if(9 == i)
        {
            ui->label_13->setPixmap(qpix);
        }
    }
}

void Widget::play_choosed1()
{
    string img_choosed = top_choosed_videos[0];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_4->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed2()
{
    string img_choosed = top_choosed_videos[1];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_5->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed3()
{
    string img_choosed = top_choosed_videos[2];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_6->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed4()
{
    string img_choosed = top_choosed_videos[3];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_7->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed5()
{
    string img_choosed = top_choosed_videos[4];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_8->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed6()
{
    string img_choosed = top_choosed_videos[5];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_9->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed7()
{
    string img_choosed = top_choosed_videos[6];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_10->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed8()
{
    string img_choosed = top_choosed_videos[7];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_11->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed9()
{
    string img_choosed = top_choosed_videos[8];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_12->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}

void Widget::play_choosed10()
{
    string img_choosed = top_choosed_videos[9];
    cout<<img_choosed<<endl;
    VideoCapture capture;
    string cameraID ;
    string position_s_e;
    string pos_s,pos_e;
    Mat frame;
    string absolute_path;
    string video_path_name;
    int pos_s_int;
    int pos_e_int;
    int played_num = 0;

    absolute_path = videoname.substr(0,videoname.find_last_of("/")+1);
    cameraID = img_choosed.substr(0,img_choosed.find("_"));
    position_s_e = img_choosed.substr(img_choosed.find("_ps")+3,img_choosed.length());
    pos_s = position_s_e.substr(0,position_s_e.find("_"));
    pos_e = position_s_e.substr(position_s_e.find("_pe")+3,position_s_e.find(".")-position_s_e.find("_pe")-3);
    video_path_name = absolute_path + cameraID + ".mp4";
    pos_s_int = stoi(pos_s);
    pos_e_int = stoi(pos_e);

    int height = ui->label_4->height();
    int width = ui->label_4->width();
    QImage img;
    QPixmap qpix;

    capture = VideoCapture(video_path_name);
    capture.set(CV_CAP_PROP_POS_FRAMES,pos_s_int);
    capture >> frame;
    int all_num = pos_e_int - pos_s_int;

    while(played_num<all_num)
    {
        cvtColor(frame,frame,CV_BGR2RGB);
        img = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_RGB888);
        qpix = QPixmap::fromImage(img);
        qpix = qpix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  //按比例填充
        ui->label_13->setPixmap(qpix);
        usleep(waittime_result);
        waitKey(5);
        capture>>frame;
        played_num = played_num + 1;
    }
}









//you are beautiful
