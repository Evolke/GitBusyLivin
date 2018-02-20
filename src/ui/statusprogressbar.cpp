#include "statusprogressbar.h"

StatusProgressBar::StatusProgressBar(QWidget *parent) : QProgressBar(parent)
{
    setMaximumSize(150,15);
    setMinimum(0);
    setMaximum(0);
}
