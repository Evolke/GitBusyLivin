#include "toolbarcombo.h"

ToolbarCombo::ToolbarCombo(QWidget *parent) : QComboBox(parent)
{
    setMaximumHeight(22);
    setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //setFrame(false);
}
