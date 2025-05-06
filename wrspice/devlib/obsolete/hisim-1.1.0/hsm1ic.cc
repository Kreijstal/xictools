
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
 File: hsm1getic.c of HiSIM v1.1.0

 Copyright (C) 2002 STARC

 June 30, 2002: developed by Hiroshima University and STARC
 June 30, 2002: posted by Keiichi MORIKAWA, STARC Physical Design Group
***********************************************************************/

#include "hsm1defs.h"

#define HSM1nextModel      next()
#define HSM1nextInstance   next()
#define HSM1instances      inst()


int
HSM1dev::getic(sGENmodel *genmod, sCKT *ckt)
{
    sHSM1model *model = static_cast<sHSM1model*>(genmod);
    sHSM1instance *here;

  /*
   * grab initial conditions out of rhs array.   User specified, so use
   * external nodes to get values
   */

  for ( ;model ;model = model->HSM1nextModel ) {
    for ( here = model->HSM1instances; here ;here = here->HSM1nextInstance ) {
      if (!here->HSM1_icVBS_Given) {
        here->HSM1_icVBS = 
          *(ckt->CKTrhs + here->HSM1bNode) - 
          *(ckt->CKTrhs + here->HSM1sNode);
      }
      if (!here->HSM1_icVDS_Given) {
        here->HSM1_icVDS = 
          *(ckt->CKTrhs + here->HSM1dNode) - 
          *(ckt->CKTrhs + here->HSM1sNode);
      }
      if (!here->HSM1_icVGS_Given) {
        here->HSM1_icVGS = 
          *(ckt->CKTrhs + here->HSM1gNode) - 
          *(ckt->CKTrhs + here->HSM1sNode);
      }
    }
  }
  return(OK);
}

