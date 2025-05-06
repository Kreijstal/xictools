
/*========================================================================*
 *                                                                        *
 *  Distributed by Whiteley Research Inc., Sunnyvale, California, USA     *
 *                       http://wrcad.com                                 *
 *  Copyright (C) 2017 Whiteley Research Inc., all rights reserved.       *
 *  Author: Stephen R. Whiteley, except as indicated.                     *
 *                                                                        *
 *  As fully as possible recognizing licensing terms and conditions       *
 *  imposed by earlier work from which this work was derived, if any,     *
 *  this work is released under the Apache License, Version 2.0 (the      *
 *  "License").  You may not use this file except in compliance with      *
 *  the License, and compliance with inherited licenses which are         *
 *  specified in a sub-header below this one if applicable.  A copy       *
 *  of the License is provided with this distribution, or you may         *
 *  obtain a copy of the License at                                       *
 *                                                                        *
 *        http://www.apache.org/licenses/LICENSE-2.0                      *
 *                                                                        *
 *  See the License for the specific language governing permissions       *
 *  and limitations under the License.                                    *
 *                                                                        *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      *
 *   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-        *
 *   INFRINGEMENT.  IN NO EVENT SHALL WHITELEY RESEARCH INCORPORATED      *
 *   OR STEPHEN R. WHITELEY BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER     *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,      *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE       *
 *   USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                        *
 *========================================================================*
 *               XicTools Integrated Circuit Design System                *
 *                                                                        *
 * WRspice Circuit Simulation and Analysis Tool:  Device Library          *
 *                                                                        *
 *========================================================================*
 $Id:$
 *========================================================================*/

/***********************************************************************
 HiSIM v1.1.0
 File: hsm1par.c of HiSIM v1.1.0

 Copyright (C) 2002 STARC

 June 30, 2002: developed by Hiroshima University and STARC
 June 30, 2002: posted by Keiichi MORIKAWA, STARC Physical Design Group
***********************************************************************/

#include "hsm1defs.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


int
HSM1dev::setInst(int param, IFdata *data, sGENinstance *geninst)
{
    sHSM1instance *here = static_cast<sHSM1instance*>(geninst);
    IFvalue *value = &data->v;

  switch (param) {
  case HSM1_W:
    here->HSM1_w = value->rValue;
    here->HSM1_w_Given = TRUE;
    break;
  case HSM1_L:
    here->HSM1_l = value->rValue;
    here->HSM1_l_Given = TRUE;
    break;
  case HSM1_AS:
    here->HSM1_as = value->rValue;
    here->HSM1_as_Given = TRUE;
    break;
  case HSM1_AD:
    here->HSM1_ad = value->rValue;
    here->HSM1_ad_Given = TRUE;
    break;
  case HSM1_PS:
    here->HSM1_ps = value->rValue;
    here->HSM1_ps_Given = TRUE;
    break;
  case HSM1_PD:
    here->HSM1_pd = value->rValue;
    here->HSM1_pd_Given = TRUE;
    break;
  case HSM1_NRS:
    here->HSM1_nrs = value->rValue;
    here->HSM1_nrs_Given = TRUE;
    break;
  case HSM1_NRD:
    here->HSM1_nrd = value->rValue;
    here->HSM1_nrd_Given = TRUE;
    break;
  case HSM1_TEMP:
    here->HSM1_temp = value->rValue;
    here->HSM1_temp_Given = TRUE;
    break;
  case HSM1_DTEMP:
    here->HSM1_dtemp = value->rValue;
    here->HSM1_dtemp_Given = TRUE;
    break;
  case HSM1_OFF:
    here->HSM1_off = value->iValue;
    break;
  case HSM1_IC_VBS:
    here->HSM1_icVBS = value->rValue;
    here->HSM1_icVBS_Given = TRUE;
    break;
  case HSM1_IC_VDS:
    here->HSM1_icVDS = value->rValue;
    here->HSM1_icVDS_Given = TRUE;
    break;
  case HSM1_IC_VGS:
    here->HSM1_icVGS = value->rValue;
    here->HSM1_icVGS_Given = TRUE;
    break;
  case HSM1_IC:
    switch (value->v.numValue) {
    case 3:
      here->HSM1_icVBS = *(value->v.vec.rVec + 2);
      here->HSM1_icVBS_Given = TRUE;
      // fallthrough
    case 2:
      here->HSM1_icVGS = *(value->v.vec.rVec + 1);
      here->HSM1_icVGS_Given = TRUE;
      // fallthrough
    case 1:
      here->HSM1_icVDS = *(value->v.vec.rVec);
      here->HSM1_icVDS_Given = TRUE;
            data->cleanup();
      break;
    default:
            data->cleanup();
      return(E_BADPARM);
    }
    break;
  default:
    return(E_BADPARM);
  }
  return(OK);
}

