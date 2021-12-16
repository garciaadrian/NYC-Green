#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_nyc_green.h"

class NYC_Green : public QMainWindow
{
    Q_OBJECT

public:
    NYC_Green(QWidget *parent = Q_NULLPTR);

public slots:
    void onSubmit();
    void onWattSlider();
    void onSpaceSlider();

private:
    Ui::NYC_GreenClass ui;
};
