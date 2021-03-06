/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2017-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SVGCONFIG_H
#define SVGCONFIG_H

#include <QTableWidget>
#include <QWidget>
#include "svgview.h"
#include "vlePlan.h"

class svgConfig : public QWidget
{
    Q_OBJECT
public:
    explicit svgConfig(QWidget *parent = 0);
    void clear(void);
    void setDefaultColor(QString name);
    void setPlan(vlePlan *plan);
    void setView(SvgView *view);
private:
    void setupUi(void);
private slots:
    void colorSelectionChange();
    void colorSelectionEdit(int row, int col);
private:
    vlePlan      *mPlan;
    QString       mDefaultColor;
    QTableWidget *mUiColorTable;
    SvgView      *mViewWidget;
};

#endif // SVGCONFIG_H
