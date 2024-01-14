
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

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1987 Gary W. Ng
Model Author: 1995 Colin McAndrew Motorola
Spice3 Implementation: 2003 Dietmar Warning DAnalyse GmbH
**********/

#include <stdio.h>
#include "vbicdefs.h"
#include "noisdefs.h"


// define the names of the noise sources
static const char *VBICnNames[VBICNSRCS] = {
    // Note that we have to keep the order consistent with the
    // strchr definitions in VBICdefs.h

    "_rc",          // noise due to rc
    "_rci",         // noise due to rci
    "_rb",          // noise due to rb
    "_rbi",         // noise due to rbi
    "_re",          // noise due to re
    "_rbp",         // noise due to rbp
    "_ic",          // noise due to ic
    "_ib",          // noise due to ib
    "_ibep",        // noise due to ibep
    "_1overfbe",    // flicker (1/f) noise ibe
    "_1overfbep",   // flicker (1/f) noise ibep
    "_rs",          // noise due to rs
    "_iccp",        // noise due to iccp
    ""              // total transistor noise
};

int
VBICdev::noise (int mode, int operation, sGENmodel *genmod, sCKT *ckt,
    sNdata *data, double *OnDens)
{
    char vbicname[N_MXVLNTH];
    sVBICmodel *model = static_cast<sVBICmodel*>(genmod);




    if (operation == N_OPEN) {
        // see if we have to to produce a summary report
        // if so, name all the noise generators

        if (((sNOISEAN*)ckt->CKTcurJob)->NStpsSm == 0)
            return (OK);

        if (mode == N_DENS) {
            for ( ; model; model = model->next()) {
                sVBICinstance *inst;
                for (inst = model->inst(); inst; inst = inst->next()) {

                    for (int i = 0; i < VBICNSRCS; i++) {
                        (void)snprintf(vbicname, sizeof(vbicname),
                            "onoise_%s%s",
                            (char*)inst->GENname,VBICnNames[i]);
                        Realloc(&data->namelist, data->numPlots+1,
                            data->numPlots);
                        ckt->newUid(&data->namelist[data->numPlots++],
                            0, vbicname, UID_OTHER);
                        // we've added one more plot
                    }
                }
            }
            return (OK);
        }

        if (mode == INT_NOIZ) {
            for ( ; model; model = model->next()) {
                sVBICinstance *inst;
                for (inst = model->inst(); inst; inst = inst->next()) {

                    for (int i = 0; i < VBICNSRCS; i++) {
                        (void)snprintf(vbicname, sizeof(vbicname),
                            "onoise_total_%s%s",
                            (char*)inst->GENname, VBICnNames[i]);
                        Realloc(&data->namelist, data->numPlots+2,
                            data->numPlots);
                        ckt->newUid(&data->namelist[data->numPlots++],
                            0, vbicname, UID_OTHER);
                        // we've added one more plot

                        (void)snprintf(vbicname, sizeof(vbicname),
                            "inoise_total_%s%s",
                            (char*)inst->GENname,VBICnNames[i]);
                        ckt->newUid(&data->namelist[data->numPlots++],
                            0, vbicname, UID_OTHER);
                        // we've added one more plot
                    }
                }
            }
        }
        return (OK);
    }

    if (operation == N_CALC) {
        if (mode == N_DENS) {
            for ( ; model; model = model->next()) {
                sVBICinstance *inst;
                for (inst = model->inst(); inst; inst = inst->next()) {

                    double noizDens[VBICNSRCS];
                    double lnNdens[VBICNSRCS];
                    NevalSrc(&noizDens[VBICRCNOIZ], &lnNdens[VBICRCNOIZ],
                        ckt, THERMNOISE, inst->VBICcollCXNode,
                        inst->VBICcollNode,
                        model->VBICcollectorConduct * inst->VBICarea *
                        inst->VBICm);

                    NevalSrc(&noizDens[VBICRCINOIZ], &lnNdens[VBICRCINOIZ],
                        ckt, THERMNOISE, inst->VBICcollCXNode,
                        inst->VBICcollCINode,
                        *(ckt->CKTstate0 + inst->VBICirci_Vrci));

                    NevalSrc(&noizDens[VBICRBNOIZ], &lnNdens[VBICRBNOIZ],
                        ckt, THERMNOISE, inst->VBICbaseBXNode,
                        inst->VBICbaseNode,
                        model->VBICbaseConduct * inst->VBICarea * inst->VBICm);

                    NevalSrc(&noizDens[VBICRBINOIZ], &lnNdens[VBICRBINOIZ],
                        ckt, THERMNOISE, inst->VBICbaseBXNode,
                        inst->VBICbaseBINode,
                        *(ckt->CKTstate0 + inst->VBICirbi_Vrbi));

                    NevalSrc(&noizDens[VBICRENOIZ], &lnNdens[VBICRENOIZ],
                        ckt, THERMNOISE, inst->VBICemitEINode,
                        inst->VBICemitNode,
                        model->VBICemitterConduct * inst->VBICarea *
                        inst->VBICm);

                    NevalSrc(&noizDens[VBICRBPNOIZ], &lnNdens[VBICRBPNOIZ],
                        ckt, THERMNOISE, inst->VBICemitEINode,
                        inst->VBICemitNode,
                        *(ckt->CKTstate0 + inst->VBICirbp_Vrbp));

                    NevalSrc(&noizDens[VBICRSNOIZ],&lnNdens[VBICRSNOIZ],
                        ckt, THERMNOISE, inst->VBICsubsSINode,
                        inst->VBICsubsNode,
                        model->VBICsubstrateConduct * inst->VBICarea *
                        inst->VBICm);

                    NevalSrc(&noizDens[VBICICNOIZ], &lnNdens[VBICICNOIZ],
                        ckt, SHOTNOISE, inst->VBICcollCINode,
                        inst->VBICemitEINode,
                        *(ckt->CKTstate0 + inst->VBICitzf));

                    NevalSrc(&noizDens[VBICIBNOIZ], &lnNdens[VBICIBNOIZ],
                        ckt, SHOTNOISE, inst->VBICbaseBINode,
                        inst->VBICemitEINode,
                        *(ckt->CKTstate0 + inst->VBICibe));

                    NevalSrc(&noizDens[VBICIBEPNOIZ], &lnNdens[VBICIBEPNOIZ],
                        ckt, SHOTNOISE, inst->VBICbaseBXNode,
                        inst->VBICbaseBPNode,
                        *(ckt->CKTstate0 + inst->VBICibep));

                    NevalSrc(&noizDens[VBICICCPNOIZ], &lnNdens[VBICICCPNOIZ],
                        ckt, SHOTNOISE, inst->VBICbaseBXNode,
                        inst->VBICsubsSINode,
                        *(ckt->CKTstate0 + inst->VBICiccp));

                    NevalSrc(&noizDens[VBICFLBENOIZ], (double*)NULL, ckt,
                        N_GAIN, inst->VBICbaseBINode, inst->VBICemitEINode,
                        0.0);
                    noizDens[VBICFLBENOIZ] *= inst->VBICm * model->VBICfNcoef * 
                        exp(model->VBICfNexpA *
                        log(SPMAX(fabs(*(ckt->CKTstate0 +
                        inst->VBICibe)/inst->VBICm),N_MINLOG))) /
                        pow(data->freq, model->VBICfNexpB);
                    lnNdens[VBICFLBENOIZ] = 
                        log(SPMAX(noizDens[VBICFLBENOIZ],N_MINLOG));

                    NevalSrc(&noizDens[VBICFLBEPNOIZ], (double*)NULL, ckt,
                        N_GAIN, inst->VBICbaseBXNode, inst->VBICbaseBPNode,
                        0.0);
                    noizDens[VBICFLBEPNOIZ] *= inst->VBICm *
                        model->VBICfNcoef * exp(model->VBICfNexpA *
                        log(SPMAX(fabs(*(ckt->CKTstate0 +
                        inst->VBICibep)/inst->VBICm),N_MINLOG))) /
                        pow(data->freq, model->VBICfNexpB);
                    lnNdens[VBICFLBEPNOIZ] = 
                        log(SPMAX(noizDens[VBICFLBEPNOIZ],N_MINLOG));

                    noizDens[VBICTOTNOIZ] = noizDens[VBICRCNOIZ] +
                        noizDens[VBICRCINOIZ] + noizDens[VBICRBNOIZ] +
                        noizDens[VBICRBINOIZ] + noizDens[VBICRENOIZ] +
                        noizDens[VBICRBPNOIZ] + noizDens[VBICICNOIZ] +
                        noizDens[VBICIBNOIZ] + noizDens[VBICIBEPNOIZ] +
                        noizDens[VBICFLBENOIZ] + noizDens[VBICFLBEPNOIZ];

                    lnNdens[VBICTOTNOIZ] = log(noizDens[VBICTOTNOIZ]);

                    *OnDens += noizDens[VBICTOTNOIZ];

                    if (data->delFreq == 0.0) { 

                        // if we haven't done any previous integration,
                        // we need to initialize our "history" variables

                        for (int i = 0; i < VBICNSRCS; i++)
                            inst->VBICnVar[LNLSTDENS][i] = lnNdens[i];

                        // clear out our integration variables if it's
                        // the first pass

                        if (data->freq ==
                                ((sNOISEAN*)ckt->CKTcurJob)->JOBac.fstart()) {

                            for (int i = 0; i < VBICNSRCS; i++) {
                                inst->VBICnVar[OUTNOIZ][i] = 0.0;
                                inst->VBICnVar[INNOIZ][i] = 0.0;
                            }
                        }
                    }
                    else {
                        // data->delFreq != 0.0 (we have to integrate)

                        // In order to get the best curve fit, we have
                        // to integrate each component separately

                        for (int i = 0; i < VBICNSRCS; i++) {
                            if (i != VBICTOTNOIZ) {
                                double tempOnoise;
                                double tempInoise;
                                tempOnoise = data->integrate(noizDens[i],
                                    lnNdens[i], inst->VBICnVar[LNLSTDENS][i]);
                                tempInoise = data->integrate(noizDens[i]*
                                    data->GainSqInv,
                                    lnNdens[i] + data->lnGainInv,
                                    inst->VBICnVar[LNLSTDENS][i] +
                                    data->lnGainInv);

                                inst->VBICnVar[LNLSTDENS][i] = lnNdens[i];
                                data->outNoiz += tempOnoise;
                                data->inNoise += tempInoise;

                                if (((sNOISEAN*)ckt->CKTcurJob)->NStpsSm != 0) {
                                    inst->VBICnVar[OUTNOIZ][i] += tempOnoise;
                                    inst->VBICnVar[OUTNOIZ][VBICTOTNOIZ] +=
                                        tempOnoise;
                                    inst->VBICnVar[INNOIZ][i] += tempInoise;
                                    inst->VBICnVar[INNOIZ][VBICTOTNOIZ] +=
                                        tempInoise;
                                }
                            }
                        }
                    }
                    if (data->prtSummary) {
                        for (int i = 0; i < VBICNSRCS; i++) {
                            // print a summary report
                            data->outpVector[data->outNumber++] = noizDens[i];
                        }
                    }
                }
            }
            return (OK);
        }

        if (mode == INT_NOIZ) {
            // already calculated, just output
            if (static_cast<sNOISEAN*>(ckt->CKTcurJob)->NStpsSm == 0)
                return (OK);
            for ( ; model; model = model->next()) {
                sVBICinstance *inst;
                for (inst = model->inst(); inst; inst = inst->next()) {

                    for (int i = 0; i < VBICNSRCS; i++) {
                        data->outpVector[data->outNumber++] =
                            inst->VBICnVar[OUTNOIZ][i];
                        data->outpVector[data->outNumber++] =
                            inst->VBICnVar[INNOIZ][i];
                    }
                }
            }
        }
    }
    return(OK);
}

