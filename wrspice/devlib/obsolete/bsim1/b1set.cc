
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

/***************************************************************************
JSPICE3 adaptation of Spice3f2 - Copyright (c) Stephen R. Whiteley 1992
Copyright 1990 Regents of the University of California.  All rights reserved.
Authors: 1985 Hong J. Park, Thomas L. Quarles 
         1993 Stephen R. Whiteley
****************************************************************************/

#include "b1defs.h"

namespace {
    int get_node_ptr(sCKT *ckt, sB1instance *inst)
    {
        TSTALLOC(B1DdPtr, B1dNode, B1dNode)
        TSTALLOC(B1GgPtr, B1gNode, B1gNode)
        TSTALLOC(B1SsPtr, B1sNode, B1sNode)
        TSTALLOC(B1BbPtr, B1bNode, B1bNode)
        TSTALLOC(B1DPdpPtr, B1dNodePrime, B1dNodePrime)
        TSTALLOC(B1SPspPtr, B1sNodePrime, B1sNodePrime)
        TSTALLOC(B1DdpPtr, B1dNode, B1dNodePrime)
        TSTALLOC(B1GbPtr, B1gNode, B1bNode)
        TSTALLOC(B1GdpPtr, B1gNode, B1dNodePrime)
        TSTALLOC(B1GspPtr, B1gNode, B1sNodePrime)
        TSTALLOC(B1SspPtr, B1sNode, B1sNodePrime)
        TSTALLOC(B1BdpPtr, B1bNode, B1dNodePrime)
        TSTALLOC(B1BspPtr, B1bNode, B1sNodePrime)
        TSTALLOC(B1DPspPtr, B1dNodePrime, B1sNodePrime)
        TSTALLOC(B1DPdPtr, B1dNodePrime, B1dNode)
        TSTALLOC(B1BgPtr, B1bNode, B1gNode)
        TSTALLOC(B1DPgPtr, B1dNodePrime, B1gNode)
        TSTALLOC(B1SPgPtr, B1sNodePrime, B1gNode)
        TSTALLOC(B1SPsPtr, B1sNodePrime, B1sNode)
        TSTALLOC(B1DPbPtr, B1dNodePrime, B1bNode)
        TSTALLOC(B1SPbPtr, B1sNodePrime, B1bNode)
        TSTALLOC(B1SPdpPtr, B1sNodePrime, B1dNodePrime)
        return (OK);
    }
}


int
B1dev::setup(sGENmodel *genmod, sCKT *ckt, int *states)
{
    int error;
    sCKTnode *tmp;

    sB1model *model = static_cast<sB1model*>(genmod);
    for ( ; model; model = model->next()) {
    
        // Default value Processing for B1 MOSFET Models
        if (!model->B1typeGiven)
            model->B1type = NMOS;  // NMOS
        if (!model->B1vfb0Given)
            model->B1vfb0 = 0.0;
        if (!model->B1vfbLGiven)
            model->B1vfbL = 0.0;
        if (!model->B1vfbWGiven)
            model->B1vfbW = 0.0;
        if (!model->B1phi0Given)
            model->B1phi0 = 0.0;
        if (!model->B1phiLGiven)
            model->B1phiL = 0.0;
        if (!model->B1phiWGiven)
            model->B1phiW = 0.0;
        if (!model->B1K10Given)
            model->B1K10 = 0.0;
        if (!model->B1K1LGiven)
            model->B1K1L = 0.0;
        if (!model->B1K1WGiven)
            model->B1K1W = 0.0;
        if (!model->B1K20Given)
            model->B1K20 = 0.0;
        if (!model->B1K2LGiven)
            model->B1K2L = 0.0;
        if (!model->B1K2WGiven)
            model->B1K2W = 0.0;
        if (!model->B1eta0Given)
            model->B1eta0 = 0.0;
        if (!model->B1etaLGiven)
            model->B1etaL = 0.0;
        if (!model->B1etaWGiven)
            model->B1etaW = 0.0;
        if (!model->B1mobZeroGiven)
            model->B1mobZero = 0.0;
        if (!model->B1deltaLGiven)
            model->B1deltaL = 0.0;
        if (!model->B1deltaWGiven)
            model->B1deltaW = 0.0;
        if (!model->B1ugs0Given)
            model->B1ugs0 = 0.0;
        if (!model->B1ugsLGiven)
            model->B1ugsL = 0.0;
        if (!model->B1ugsWGiven)
            model->B1ugsW = 0.0;
        if (!model->B1uds0Given)
            model->B1uds0 = 0.0;
        if (!model->B1udsLGiven)
            model->B1udsL = 0.0;
        if (!model->B1udsWGiven)
            model->B1udsW = 0.0;
        if (!model->B1mobZeroB0Given)
            model->B1mobZeroB0 = 0.0;
        if (!model->B1mobZeroBlGiven)
            model->B1mobZeroBl = 0.0;
        if (!model->B1mobZeroBwGiven)
            model->B1mobZeroBw = 0.0;
        if (!model->B1etaB0Given)
            model->B1etaB0 = 0.0;
        if (!model->B1etaBlGiven)
            model->B1etaBl = 0.0;
        if (!model->B1etaBwGiven)
            model->B1etaBw = 0.0;
        if (!model->B1etaD0Given)
            model->B1etaD0 = 0.0;
        if (!model->B1etaDlGiven)
            model->B1etaDl = 0.0;
        if (!model->B1etaDwGiven)
            model->B1etaDw = 0.0;
        if (!model->B1ugsB0Given)
            model->B1ugsB0 = 0.0;
        if (!model->B1ugsBLGiven)
            model->B1ugsBL = 0.0;
        if (!model->B1ugsBWGiven)
            model->B1ugsBW = 0.0;
        if (!model->B1udsB0Given)
            model->B1udsB0 = 0.0;
        if (!model->B1udsBLGiven)
            model->B1udsBL = 0.0;
        if (!model->B1udsBWGiven)
            model->B1udsBW = 0.0;
        if (!model->B1mobVdd0Given)
            model->B1mobVdd0 = 0.0;
        if (!model->B1mobVddlGiven)
            model->B1mobVddl = 0.0;
        if (!model->B1mobVddwGiven)
            model->B1mobVddw = 0.0;
        if (!model->B1mobVddB0Given)
            model->B1mobVddB0 = 0.0;
        if (!model->B1mobVddBlGiven)
            model->B1mobVddBl = 0.0;
        if (!model->B1mobVddBwGiven)
            model->B1mobVddBw = 0.0;
        if (!model->B1mobVddD0Given)
            model->B1mobVddD0 = 0.0;
        if (!model->B1mobVddDlGiven)
            model->B1mobVddDl = 0.0;
        if (!model->B1mobVddDwGiven)
            model->B1mobVddDw = 0.0;
        if (!model->B1udsD0Given)
            model->B1udsD0 = 0.0;
        if (!model->B1udsDLGiven)
            model->B1udsDL = 0.0;
        if (!model->B1udsDWGiven)
            model->B1udsDW = 0.0;
        if (!model->B1oxideThicknessGiven)
            model->B1oxideThickness = 0.0;  // um
        if (!model->B1tempGiven)
            model->B1temp = 0.0;
        if (!model->B1vddGiven)
            model->B1vdd = 0.0;
        if (!model->B1gateDrainOverlapCapGiven)
            model->B1gateDrainOverlapCap = 0.0;
        if (!model->B1gateSourceOverlapCapGiven)
            model->B1gateSourceOverlapCap = 0.0;
        if (!model->B1gateBulkOverlapCapGiven)
            model->B1gateBulkOverlapCap = 0.0;
        if (!model->B1channelChargePartitionFlagGiven)
            model->B1channelChargePartitionFlag = 0;
        if (!model->B1subthSlope0Given)
            model->B1subthSlope0 = 0.0;
        if (!model->B1subthSlopeLGiven)
            model->B1subthSlopeL = 0.0;
        if (!model->B1subthSlopeWGiven)
            model->B1subthSlopeW = 0.0;
        if (!model->B1subthSlopeB0Given)
            model->B1subthSlopeB0 = 0.0;
        if (!model->B1subthSlopeBLGiven)
            model->B1subthSlopeBL = 0.0;
        if (!model->B1subthSlopeBWGiven)
            model->B1subthSlopeBW = 0.0;
        if (!model->B1subthSlopeD0Given)
            model->B1subthSlopeD0 = 0.0;
        if (!model->B1subthSlopeDLGiven)
            model->B1subthSlopeDL = 0.0;
        if (!model->B1subthSlopeDWGiven)
            model->B1subthSlopeDW = 0.0;
        if (!model->B1sheetResistanceGiven)
            model->B1sheetResistance = 0.0;
        if (!model->B1unitAreaJctCapGiven)
            model->B1unitAreaJctCap = 0.0;
        if (!model->B1unitLengthSidewallJctCapGiven)
            model->B1unitLengthSidewallJctCap = 0.0;
        if (!model->B1jctSatCurDensityGiven)
            model->B1jctSatCurDensity = 0.0;
        if (!model->B1bulkJctPotentialGiven)
            model->B1bulkJctPotential = 0.0;
        if (!model->B1sidewallJctPotentialGiven)
            model->B1sidewallJctPotential = 0.0;
        if (!model->B1bulkJctBotGradingCoeffGiven)
            model->B1bulkJctBotGradingCoeff = 0.0;
        if (!model->B1bulkJctSideGradingCoeffGiven)
            model->B1bulkJctSideGradingCoeff = 0.0;
        if (!model->B1defaultWidthGiven)
            model->B1defaultWidth = 0.0;
        if (!model->B1deltaLengthGiven)
            model->B1deltaLength = 0.0;

        sB1instance *inst;
        for (inst = model->inst(); inst; inst = inst->next()) {

            // allocate a chunk of the state vector
            inst->GENstate = *states;
            *states += B1numStates;

            // perform the parameter defaulting

            if (!inst->B1mGiven)
                inst->B1m = 1.0;
            if (!inst->B1drainAreaGiven)
                inst->B1drainArea = ckt->mos_default_ad();
            if (!inst->B1drainPerimeterGiven)
                inst->B1drainPerimeter = 0;
            if (!inst->B1drainSquaresGiven)
                inst->B1drainSquares = 1;
            if (!inst->B1icVBSGiven)
                inst->B1icVBS = 0;
            if (!inst->B1icVDSGiven)
                inst->B1icVDS = 0;
            if (!inst->B1icVGSGiven)
                inst->B1icVGS = 0;
            if (!inst->B1lGiven)
                inst->B1l = ckt->mos_default_l();
            if (!inst->B1sourceAreaGiven)
                inst->B1sourceArea = ckt->mos_default_as();
            if (!inst->B1sourcePerimeterGiven)
                inst->B1sourcePerimeter = 0;
            if (!inst->B1sourceSquaresGiven)
                inst->B1sourceSquares = 1;
            if (!inst->B1vdsatGiven)
                inst->B1vdsat = 0;
            if (!inst->B1vonGiven)
                inst->B1von = 0;
            if (!inst->B1wGiven)
                inst->B1w = ckt->mos_default_w();

            // process drain series resistance
            if ((model->B1sheetResistance != 0) && 
                    (inst->B1drainSquares != 0.0 ) &&
                    (inst->B1dNodePrime == 0)) {
                error = ckt->mkVolt(&tmp, inst->GENname, "drain");
                if (error)
                    return(error);
                inst->B1dNodePrime = tmp->number();
            }
            else
                inst->B1dNodePrime = inst->B1dNode;
                   
            // process source series resistance
            if ((model->B1sheetResistance != 0) && 
                    (inst->B1sourceSquares != 0.0 ) &&
                    (inst->B1sNodePrime == 0)) {
                if (inst->B1sNodePrime == 0) {
                    error = ckt->mkVolt(&tmp, inst->GENname, "source");
                    if (error)
                        return(error);
                    inst->B1sNodePrime = tmp->number();
                }
            }
            else
                inst->B1sNodePrime = inst->B1sNode;

            // set Sparse Matrix Pointers
            error = get_node_ptr(ckt, inst);
            if (error != OK)
                return (error);
        }
    }
    return (OK);
}


int
B1dev::unsetup(sGENmodel *genmod, sCKT*)
{
    sB1model *model = static_cast<sB1model*>(genmod);
    for ( ; model; model = model->next()) {
        sB1instance *inst;
        for (inst = model->inst(); inst; inst = inst->next()) {
            if (inst->B1dNodePrime != inst->B1dNode)
                inst->B1dNodePrime = 0;
            if (inst->B1sNodePrime != inst->B1sNode)
                inst->B1sNodePrime = 0;
        }
    }
    return (OK);
}


// SRW - reset the matrix element pointers.
//
int
B1dev::resetup(sGENmodel *inModel, sCKT *ckt)
{
    for (sB1model *model = (sB1model*)inModel; model;
            model = model->next()) {
        for (sB1instance *here = model->inst(); here;
                here = here->next()) {
            int error = get_node_ptr(ckt, here);
            if (error != OK)
                return (error);
        }
    }
    return (OK);
}

