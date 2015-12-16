//****************************************************************
//
// Copyright 2015 Young Eun Cho, Hwan Suk Choi (Seoul National University) All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "SynchronizedLocalizationSensor.h"
#include <cmath.h>
#define VSOUND      340.29
#define NSTATIONARY 3
double sls_Px[], sls_Py[];
double sls_d[];


void set_stationary_coordinates(double[] xcoor, double[] ycoor){
    for(int i = 0; i < NSTATIONARY; i++){
        sls_Px[i] = xcoor[i];
        sls_Py[i] = ycoor[i];
    }
    return;
}

void init_sls(double[] xcoor, double[] ycoor){
    sls_Px[] = new double [NSTATIONARY];
    sls_Py[] = new double [NSTATIONARY];
    sls_d[] = new double [NSTATIONARY];
    set_stationary_coordinates(xcoor, ycoor);
}

void to_distance(int index, unsigned long t1, unsigned long t2){
    sls_d[index] = ((double)t2 - (double)t1) * VSOUND / 1000000;
    return;
}

double[] trilateration(){
    double a, b;
    a = 2 * sls_d[0] * sls_d[0] - sls_d[1] * sls_d[1] / (2 * sls_Px[1]);
    b = sls_Py[1] / sls_Px[1];

    double y, yd, y1, y2;
    y = a * b / (b * b - 1);
    yd = sqrt(a * a * b * b - (b * b - 1) * (a * a - sls_d[0]) * (a * a - sls_d[0]));
    y1 = y + yd;
    y2 = y - yd;

    double x1, y1;
    x1 = a - b * y1;
    x2 = a - b * y2;

    double d1, d2;
    d1 = sqrt((x1 - sls_Px[2]) * (x1 - sls_Px[2]) + (y1 - sls_Py[2]) * (y1 - sls_Py[2]));
    d2 = sqrt((x2 - sls_Px[2]) * (x12- sls_Px[2]) + (y2 - sls_Py[2]) * (y2 - sls_Py[2]));

    if(d1 < d2){
        return [x1, y1];
    } else {
        return [x2, y2];
    }

}


SynchLocalizationSensorResource::SynchLocalizationSensorResource()
{
    m_pSynchLocalizationSensor = new SynchLocalizationSensor();
}

SynchLocalizationSensorResource::~SynchLocalizationSensorResource()
{
    delete m_pSynchLocalizationSensor;
}