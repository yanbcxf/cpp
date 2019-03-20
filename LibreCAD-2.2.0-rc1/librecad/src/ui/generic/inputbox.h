#ifndef INPUTBOX_H
#define INPUTBOX_H

#pragma once

#include <QDialog>
#include "ui_inputbox.h"

class InputBox : public QDialog, public Ui::InputBox
{
	Q_OBJECT

public:
	InputBox(QWidget *parent = Q_NULLPTR);
	~InputBox();
};

#endif