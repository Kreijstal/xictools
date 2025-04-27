
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

/*
 * Author: 2000 Wladek Grabinski; EKV v2.6 Model Upgrade
 * Author: 1997 Eckhard Brass;    EKV v2.5 Model Implementation
 *     (C) 1990 Regents of the University of California. Spice3 Format
 */

#include "ekvdefs.h"

#define EKVnextModel      next()
#define EKVnextInstance   next()
#define EKVinstances      inst()
#define CKTnode sCKTnode
#define EKVname GENname
#define CKTmkVolt(a, b, c, d) (a)->mkVolt(b, c, d)
#define CKTdltNNum(c, n)
#define CKTnomTemp CKTcurTask->TSKnomTemp


namespace {
    int get_node_ptr(sCKT *ckt, sEKVinstance *inst)
    {
        TSTALLOC(EKVDdPtr,EKVdNode,EKVdNode)
        TSTALLOC(EKVGgPtr,EKVgNode,EKVgNode)
        TSTALLOC(EKVSsPtr,EKVsNode,EKVsNode)
        TSTALLOC(EKVBbPtr,EKVbNode,EKVbNode)
        TSTALLOC(EKVDPdpPtr,EKVdNodePrime,EKVdNodePrime)
        TSTALLOC(EKVSPspPtr,EKVsNodePrime,EKVsNodePrime)
        TSTALLOC(EKVDdpPtr,EKVdNode,EKVdNodePrime)
        TSTALLOC(EKVGbPtr,EKVgNode,EKVbNode)
        TSTALLOC(EKVGdpPtr,EKVgNode,EKVdNodePrime)
        TSTALLOC(EKVGspPtr,EKVgNode,EKVsNodePrime)
        TSTALLOC(EKVSspPtr,EKVsNode,EKVsNodePrime)
        TSTALLOC(EKVBdpPtr,EKVbNode,EKVdNodePrime)
        TSTALLOC(EKVBspPtr,EKVbNode,EKVsNodePrime)
        TSTALLOC(EKVDPspPtr,EKVdNodePrime,EKVsNodePrime)
        TSTALLOC(EKVDPdPtr,EKVdNodePrime,EKVdNode)
        TSTALLOC(EKVBgPtr,EKVbNode,EKVgNode)
        TSTALLOC(EKVDPgPtr,EKVdNodePrime,EKVgNode)
        TSTALLOC(EKVSPgPtr,EKVsNodePrime,EKVgNode)
        TSTALLOC(EKVSPsPtr,EKVsNodePrime,EKVsNode)
        TSTALLOC(EKVDPbPtr,EKVdNodePrime,EKVbNode)
        TSTALLOC(EKVSPbPtr,EKVsNodePrime,EKVbNode)
        TSTALLOC(EKVSPdpPtr,EKVsNodePrime,EKVdNodePrime)
        return (OK);
    }
}


int
EKVdev::setup(sGENmodel *genmod, sCKT *ckt, int *states)
{
    sEKVmodel *model = static_cast<sEKVmodel*>(genmod);
    sEKVinstance *here;

    int error;
    CKTnode *tmp;

    /*  loop through all the EKV device models */
    for( ; model != NULL; model = model->EKVnextModel ) {
        if(!model->EKVtypeGiven) {
            model->EKVtype = NMOS;
        }
        if(!model->EKVtnomGiven) {
            model->EKVtnom = ckt->CKTnomTemp;
        }
        if((!model->EKVvt0Given)&&(model->EKVtype==NMOS)) {
            model->EKVvt0 = 0.5;
        }
        if((!model->EKVvt0Given)&&(model->EKVtype==PMOS)) {
            model->EKVvt0 = -0.5;
        }
        if(!model->EKVgammaGiven) {
            model->EKVgamma = 1.0;
        }
        if(!model->EKVekvintGiven) {
            model->EKVekvint = 0.0;
        }
        if(!model->EKVphiGiven) {
            model->EKVphi = 0.7;
        }
        if(!model->EKVkpGiven) {
            model->EKVkp = 5.0e-5;
        }
        if(!model->EKVcoxGiven) {
            model->EKVcox = 7.0e-4;
        }
        if(!model->EKVxjGiven) {
            model->EKVxj = 1.0e-7;
        }
        if(!model->EKVthetaGiven) {
            model->EKVtheta = 0.0;
        }
        if(!model->EKVe0Given) {
            model->EKVe0 = 1.e12;
        }
        if(!model->EKVucritGiven) {
            model->EKVucrit = 100.0e6;
        }
        if(!model->EKVdwGiven) {
            model->EKVdw = 0.0;
        }
        if(!model->EKVdlGiven) {
            model->EKVdl = 0.0;
        }
        if(!model->EKVlambdaGiven) {
            model->EKVlambda = 0.5;
        }
        if(!model->EKVwetaGiven) {
            model->EKVweta = 0.25;
        }
        if(!model->EKVletaGiven) {
            model->EKVleta = 0.1;
        }
        if(!model->EKVibaGiven) {
            model->EKViba = 0.0;
        }
        if(!model->EKVibbGiven) {
            model->EKVibb = 3.0e8;
        }
        if(!model->EKVibnGiven) {
            model->EKVibn = 1.0;
        }
        if(!model->EKVq0Given) {
            model->EKVq0 = 0.0;
        }
        if(!model->EKVlkGiven) {
            model->EKVlk = 2.9e-7;
        }
        if(!model->EKVtcvGiven) {
            model->EKVtcv = 1.0e-3;
        }
        if(!model->EKVbexGiven) {
            model->EKVbex = -1.5;
        }
        if(!model->EKVucexGiven) {
            model->EKVucex = 0.8;
        }
        if(!model->EKVibbtGiven) {
            model->EKVibbt = 9.0e-4;
        }
        if(!model->EKVnqsGiven) {
            model->EKVnqs = 0.0;
        }
        if(!model->EKVsatlimGiven) {
            model->EKVsatlim = exp(4.0);
        }
        if(!model->EKVfNcoefGiven) {
            model->EKVfNcoef = 0;
        }
        if(!model->EKVfNexpGiven) {
            model->EKVfNexp = 1;
        }
        if(!model->EKVjctSatCurGiven) {
            model->EKVjctSatCur = 1e-14;
        }
        if(!model->EKVjctSatCurDensityGiven) {
            model->EKVjctSatCurDensity = 0.0;
        }
        if(!model->EKVjswGiven) {
            model->EKVjsw = 0.0;
        }
        if(!model->EKVnGiven) {
            model->EKVn = 1.0;
        }
        if(!model->EKVcapBDGiven) {
            model->EKVcapBD = 0.0;
        }
        if(!model->EKVcapBSGiven) {
            model->EKVcapBS = 0.0;
        }
        if(!model->EKVbulkCapFactorGiven) {
            model->EKVbulkCapFactor = 0.0;
        }
        if(!model->EKVsideWallCapFactorGiven) {
            model->EKVsideWallCapFactor = 0.0;
        }
        if(!model->EKVbulkJctBotGradingCoeffGiven) {
            model->EKVbulkJctBotGradingCoeff = 0.5;
        }
        if(!model->EKVbulkJctSideGradingCoeffGiven) {
            model->EKVbulkJctSideGradingCoeff = 0.33;
        }
        if(!model->EKVfwdCapDepCoeffGiven) {
            model->EKVfwdCapDepCoeff = 0.5;
        }
        if(!model->EKVbulkJctPotentialGiven) {
            model->EKVbulkJctPotential = 0.8;
        }
        if(!model->EKVpbswGiven) {
            model->EKVpbsw = 0.8;
        }
        if(!model->EKVttGiven) {
            model->EKVtt = 0.0;
        }
        if(!model->EKVgateSourceOverlapCapFactorGiven) {
            model->EKVgateSourceOverlapCapFactor = 0.0;
        }
        if(!model->EKVgateDrainOverlapCapFactorGiven) {
            model->EKVgateDrainOverlapCapFactor = 0.0;
        }
        if(!model->EKVgateBulkOverlapCapFactorGiven) {
            model->EKVgateBulkOverlapCapFactor = 0.0;
        }
        if(!model->EKVdrainResistanceGiven) {
            model->EKVdrainResistance = 0.0;
        }
        if(!model->EKVsourceResistanceGiven) {
            model->EKVsourceResistance = 0.0;
        }
        if(!model->EKVsheetResistanceGiven) {
            model->EKVsheetResistance = 0.0;
        }
        if(!model->EKVrscGiven) {
            model->EKVrsc = 0.0;
        }
        if(!model->EKVrdcGiven) {
            model->EKVrdc = 0.0;
        }
        if(!model->EKVxtiGiven) {
            model->EKVxti = 3.0;
        }
        if(!model->EKVtr1Given) {
            model->EKVtr1 = 0.0;
        }
        if(!model->EKVtr2Given) {
            model->EKVtr2 = 0.0;
        }
        if(!model->EKVnlevelGiven) {
            model->EKVnlevel = 1.0;
        }

        /* loop through all the instances of the model */
        for (here = model->EKVinstances; here != NULL ;
            here=here->EKVnextInstance) {

            if(!here->EKVdrainPerimiterGiven) {
                here->EKVdrainPerimiter = 0;
            }
            if(!here->EKVicVBSGiven) {
                here->EKVicVBS = 0;
            }
            if(!here->EKVicVDSGiven) {
                here->EKVicVDS = 0;
            }
            if(!here->EKVicVGSGiven) {
                here->EKVicVGS = 0;
            }
            if(!here->EKVsourcePerimiterGiven) {
                here->EKVsourcePerimiter = 0;
            }
            if(!here->EKVvdsatGiven) {
                here->EKVvdsat = 0;
            }
            if(!here->EKVvonGiven) {
                here->EKVvon = 0;
            }
            if(!here->EKVdrainSquaresGiven) {
                here->EKVdrainSquares=1;
            }
            if(!here->EKVsourceSquaresGiven) {
                here->EKVsourceSquares=1;
            }

            /* allocate a chunk of the state vector */
            here->EKVstates = *states;
            *states += EKVnumStates;
#ifdef HAS_SENSE2
            if(ckt->CKTsenInfo && (ckt->CKTsenInfo->SENmode & TRANSEN) ){
                *states += 10 * (ckt->CKTsenInfo->SENparms);
            }
#endif

            if ((here->EKVtrd != 0
                || (here->EKVtrsh != 0
                && here->EKVdrainSquares != 0) )
                && here->EKVdNodePrime == 0) {
                error = CKTmkVolt(ckt,&tmp,here->EKVname,"drain");
                if(error) return(error);
                here->EKVdNodePrime = tmp->number();
            } else {
                here->EKVdNodePrime = here->EKVdNode;
            }

            if((here->EKVtrs != 0 ||
                (here->EKVtrsh != 0 &&
                here->EKVsourceSquares != 0) ) &&
                here->EKVsNodePrime==0) {
                error = CKTmkVolt(ckt,&tmp,here->EKVname,"source");
                if(error) return(error);
                here->EKVsNodePrime = tmp->number();
            } else {
                here->EKVsNodePrime = here->EKVsNode;
            }
            /* macro to make elements with built in test for out of memory */
/*
#define TSTALLOC(ptr,first,second) \
if((here->ptr = SMPmakeElt(matrix,here->first,here->second))==(double *)NULL){\
    return(E_NOMEM);\
}
*/
            error = get_node_ptr(ckt, here);
            if (error != OK)
                return (error);
        }
    }
    return(OK);
}


// SRW
#undef inst
#define EKVmodel sEKVmodel
#define EKVinstance sEKVinstance

int
EKVdev::unsetup(sGENmodel *inModel, sCKT*)
{
#ifndef HAS_BATCHSIM
    EKVmodel *model;
    EKVinstance *here;

    for (model = (EKVmodel *)inModel; model != NULL;
            model = model->EKVnextModel)
    {
        for (here = model->EKVinstances; here != NULL;
                here=here->EKVnextInstance)
        {
            if (here->EKVdNodePrime
                    && here->EKVdNodePrime != here->EKVdNode)
            {
                CKTdltNNum(ckt, here->EKVdNodePrime);
                here->EKVdNodePrime = 0;
            }
            if (here->EKVsNodePrime
                    && here->EKVsNodePrime != here->EKVsNode)
            {
                CKTdltNNum(ckt, here->EKVsNodePrime);
                here->EKVsNodePrime = 0;
            }
        }
    }
#endif
    return OK;
}


// SRW - reset the matrix element pointers.
//
int
EKVdev::resetup(sGENmodel *inModel, sCKT *ckt)
{
    for (sEKVmodel *model = (sEKVmodel*)inModel; model;
            model = model->next()) {
        for (sEKVinstance *here = model->inst(); here;
                here = here->next()) {
            int error = get_node_ptr(ckt, here);
            if (error != OK)
                return (error);
        }
    }
    return (OK);
}

