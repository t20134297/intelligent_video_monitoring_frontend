#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMouseEvent>
#include<QFileDialog>
#include<opencv2/opencv.hpp>
#include<QPainter>
#include<vector>

using namespace std;
using namespace cv;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    string name;
    Ui::Widget *ui;
    string videoname;
    int waittime = 30000;
    int speed=1;
    Mat play_frame;
    VideoCapture cap;
    bool playorpause = false;
    int frame_position;
    int frame_length;
    QPixmap grapPix;

    vector<string>top_choosed_videos;
    int num_choosed_videos;

private slots :
    void select_video();
    void play_or_pause();
    void speedup_play();
    void slowdown_play();
    void change_play_process();
    void press_slider();
    void capture_image();
    void search_ped();
    void play_choosed1();
    void play_choosed2();
    void play_choosed3();
    void play_choosed4();
    void play_choosed5();
    void play_choosed6();
    void play_choosed7();
    void play_choosed8();
    void play_choosed9();
    void play_choosed10();

};



#endif // WIDGET_H
