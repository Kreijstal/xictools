
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

/******************************************************************************
 *  BSIM4 4.8.2 released by Chetan Kumar Dabhi 01/01/2020                     *
 *  BSIM4 Model Equations                                                     *
 ******************************************************************************

 ******************************************************************************
 *  Copyright (c) 2020 University of California                               *
 *                                                                            *
 *  Project Director: Prof. Chenming Hu.                                      *
 *  Current developers: Chetan Kumar Dabhi   (Ph.D. student, IIT Kanpur)      *
 *                      Prof. Yogesh Chauhan (IIT Kanpur)                     *
 *                      Dr. Pragya Kushwaha  (Postdoc, UC Berkeley)           *
 *                      Dr. Avirup Dasgupta  (Postdoc, UC Berkeley)           *
 *                      Ming-Yen Kao         (Ph.D. student, UC Berkeley)     *
 *  Authors: Gary W. Ng, Weidong Liu, Xuemei Xi, Mohan Dunga, Wenwei Yang     *
 *           Ali Niknejad, Chetan Kumar Dabhi, Yogesh Singh Chauhan,          *
 *           Sayeef Salahuddin, Chenming Hu                                   * 
 ******************************************************************************/

/*
Licensed under Educational Community License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may
obtain a copy of the license at
    http://opensource.org/licenses/ECL-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT 
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations
under the License.

BSIM-CMG model is supported by the members of Silicon Integration
Initiative's Compact Model Coalition. A link to the most recent version of
this standard can be found at: http://www.si2.org/cmc 
*/

#include "b4defs.h"


#define BSIM4nextModel      next()
#define BSIM4nextInstance   next()
#define BSIM4instances      inst()

int
BSIM4dev::getic(sGENmodel *genmod, sCKT *ckt)
{
    sBSIM4model *model = static_cast<sBSIM4model*>(genmod);
    sBSIM4instance *here;

    for (; model ; model = model->BSIM4nextModel)
    {
        for (here = model->BSIM4instances; here; here = here->BSIM4nextInstance)
        {
            if (!here->BSIM4icVDSGiven)
            {
                here->BSIM4icVDS = *(ckt->CKTrhs + here->BSIM4dNode)
                                   - *(ckt->CKTrhs + here->BSIM4sNode);
            }
            if (!here->BSIM4icVGSGiven)
            {
                here->BSIM4icVGS = *(ckt->CKTrhs + here->BSIM4gNodeExt)
                                   - *(ckt->CKTrhs + here->BSIM4sNode);
            }
            if(!here->BSIM4icVBSGiven)
            {
                here->BSIM4icVBS = *(ckt->CKTrhs + here->BSIM4bNode)
                                   - *(ckt->CKTrhs + here->BSIM4sNode);
            }
        }
    }
    return(OK);
}

