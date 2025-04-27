
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

// SRW --
// Added the HAS_SENSE2 ifdefs to get rid of the old sensitivity analysis
// references

// this is from spice3
#define MAX_EXP_ARG   709.0

#define GENinstance sGENinstance
#define EKVnextModel      next()
#define EKVnextInstance   next()
#define EKVinstances      inst()
#define CKTreltol CKTcurTask->TSKreltol
#define CKTabstol CKTcurTask->TSKabstol
#define CKTvoltTol CKTcurTask->TSKvoltTol
#define CKTgmin CKTcurTask->TSKgmin
#define CKTbypass CKTcurTask->TSKbypass
#define CKTfixLimit CKTcurTask->TSKfixLimit
#define MAX SPMAX
#define MIN SPMIN
#define NIintegrate(ck, g, c, a, b) ((ck)->integrate(g, c, a, b),OK)

extern void EKVcap(sEKVmodel*, sEKVinstance*, double, double, double*, double*,
    double*);
extern void EKVevaluate(double, double, double, sEKVinstance*, sEKVmodel*,
    double*, double*, sCKT*);

#undef PREDICTOR
#undef NOBYPASS
#undef CAPBYPASS
#undef CAPZEROBYPASS
#undef NODELIMITING
#define NOSQRT

int
// SRW EKVdev::load(sGENmodel *genmod, sCKT *ckt)
EKVdev::load(sGENinstance *in_inst, sCKT *ckt)
{
// SRW    sEKVmodel *model = static_cast<sEKVmodel*>(genmod);
// SRW    sEKVinstance *here;
    sEKVinstance *here = (sEKVinstance*)in_inst;
    sEKVmodel *model = (sEKVmodel*)here->GENmodPtr;

//    double Beta;
    double DrainSatCur;
    double EffectiveLength;
    double EffectiveWidth;
    double GateBulkOverlapCap;
    double GateDrainOverlapCap;
    double GateSourceOverlapCap;
//    double OxideCap;
    double SourceSatCur;
    double arg;
    double cbhat;
    double cdhat;
    double cdrain;
    double cdreq;
    double ceq;
    double ceqbd;
    double ceqbs;
    double ceqgb;
    double ceqgd;
    double ceqgs;
    double delvbd;
    double delvbs;
    double delvds;
    double delvgd;
    double delvgs;
    double evbd;
    double evbs;
    double gcgb;
    double gcgd;
    double gcgs;
    double geq;
    double sarg;
    double sargsw;
    double vbd;
    double vbs;
    double vds;
    double vdsat;
    double vgb1;
    double vgb;
    double vgd1;
    double vgd;
    double vgdo;
    double vgs1;
    double vgs;
    double von;
    double vt;
    double xfact=0.0;
    int xnrm;
    int xrev;
    double capgs=0.0;   /* total gate-source capacitance */
    double capgd=0.0;   /* total gate-drain capacitance */
    double capgb=0.0;   /* total gate-bulk capacitance */
    int Check;
#ifndef NOBYPASS
    double tempv;
#endif /*NOBYPASS*/
    int error;
#ifdef CAPBYPASS
    int senflag;
#endif /* CAPBYPASS */ 
#ifdef HAS_SENSE2
    int SenCond;
#endif

#ifdef CAPBYPASS
    senflag = 0;
    if(ckt->CKTsenInfo && ckt->CKTsenInfo->SENstatus == PERTURBATION &&
        (ckt->CKTsenInfo->SENmode & (ACSEN | TRANSEN))) {
        senflag = 1;
    }
#endif /* CAPBYPASS */ 

/* SRW
    for( ; model != NULL; model = model->EKVnextModel ) {

        for (here = model->EKVinstances; here != NULL ;
            here=here->EKVnextInstance) {
*/

            vt = CONSTKoverQ * here->EKVtemp;
            Check=1;
#ifdef HAS_SENSE2
            if(ckt->CKTsenInfo){
#ifdef SENSDEBUG
                printf("EKVload \n");
#endif /* SENSDEBUG */

                if((ckt->CKTsenInfo->SENstatus == PERTURBATION)&&
                        (here->EKVsenPertFlag == OFF))
// SRW                    continue;
                    return (OK);

            }
            SenCond = ckt->CKTsenInfo && here->EKVsenPertFlag;
#endif

            /* first, we compute a few useful values - these could be
             * pre-computed, but for historical reasons are still done
             * here.  They may be moved at the expense of instance size
             */

            EffectiveLength=here->EKVl+model->EKVdl;
            EffectiveWidth =here->EKVw+model->EKVdw;

            if( (here->EKVtSatCurDens == 0) || 
                (here->EKVdrainArea == 0) ||
                (here->EKVsourceArea == 0)) {
                DrainSatCur = here->EKVtSatCur;
                SourceSatCur = here->EKVtSatCur;
            } else {
                DrainSatCur = here->EKVtSatCurDens * 
                    here->EKVdrainArea;
                SourceSatCur = here->EKVtSatCurDens * 
                    here->EKVsourceArea;
            }
            GateSourceOverlapCap = model->EKVgateSourceOverlapCapFactor * 
                EffectiveWidth;
            GateDrainOverlapCap = model->EKVgateDrainOverlapCapFactor * 
                EffectiveWidth;
            GateBulkOverlapCap = model->EKVgateBulkOverlapCapFactor * 
                EffectiveLength;
//            Beta = here->EKVtkp * EffectiveWidth/EffectiveLength;
//            OxideCap = model->EKVcox*EffectiveLength*EffectiveWidth;
            /* 
             * ok - now to do the start-up operations
             *
             * we must get values for vbs, vds, and vgs from somewhere
             * so we either predict them or recover them from last iteration
             * These are the two most common cases - either a prediction
             * step or the general iteration step and they
             * share some code, so we put them first - others later on
             */

#ifdef HAS_SENSE2
            if(SenCond){
#ifdef SENSDEBUG
                printf("EKVsenPertFlag = ON \n");
#endif /* SENSDEBUG */
                if((ckt->CKTsenInfo->SENmode == TRANSEN) &&
                    (ckt->CKTmode & MODEINITTRAN)) {
                    vgs = *(ckt->CKTstate1 + here->EKVvgs);
                    vds = *(ckt->CKTstate1 + here->EKVvds);
                    vbs = *(ckt->CKTstate1 + here->EKVvbs);
                    vbd = *(ckt->CKTstate1 + here->EKVvbd);
                    vgb = vgs - vbs;
                    vgd = vgs - vds;
                }
                else if (ckt->CKTsenInfo->SENmode == ACSEN){
                    vgb = model->EKVtype * ( 
                        *(ckt->CKTrhsOp+here->EKVgNode) -
                        *(ckt->CKTrhsOp+here->EKVbNode));
                    vbs = *(ckt->CKTstate0 + here->EKVvbs);
                    vbd = *(ckt->CKTstate0 + here->EKVvbd);
                    vgd = vgb + vbd ;
                    vgs = vgb + vbs ;
                    vds = vbs - vbd ;
                }
                else{
                    vgs = *(ckt->CKTstate0 + here->EKVvgs);
                    vds = *(ckt->CKTstate0 + here->EKVvds);
                    vbs = *(ckt->CKTstate0 + here->EKVvbs);
                    vbd = *(ckt->CKTstate0 + here->EKVvbd);
                    vgb = vgs - vbs;
                    vgd = vgs - vds;
                }
#ifdef SENSDEBUG
                printf(" vbs = %.7e ,vbd = %.7e,vgb = %.7e\n",vbs,vbd,vgb);
                printf(" vgs = %.7e ,vds = %.7e,vgd = %.7e\n",vgs,vds,vgd);
#endif /* SENSDEBUG */
                goto next1;
            }
#endif


            if((ckt->CKTmode & (MODEINITFLOAT | MODEINITPRED | MODEINITSMSIG
                | MODEINITTRAN)) ||
                ( (ckt->CKTmode & MODEINITFIX) && (!here->EKVoff) )  ) {
#ifndef PREDICTOR
                if(ckt->CKTmode & (MODEINITPRED | MODEINITTRAN) ) {

                    /* predictor step */

                    xfact=ckt->CKTdelta/ckt->CKTdeltaOld[1];
                    *(ckt->CKTstate0 + here->EKVvbs) = 
                        *(ckt->CKTstate1 + here->EKVvbs);
                    vbs = (1+xfact)* (*(ckt->CKTstate1 + here->EKVvbs))
                        -(xfact * (*(ckt->CKTstate2 + here->EKVvbs)));
                    *(ckt->CKTstate0 + here->EKVvgs) = 
                        *(ckt->CKTstate1 + here->EKVvgs);
                    vgs = (1+xfact)* (*(ckt->CKTstate1 + here->EKVvgs))
                        -(xfact * (*(ckt->CKTstate2 + here->EKVvgs)));
                    *(ckt->CKTstate0 + here->EKVvds) = 
                        *(ckt->CKTstate1 + here->EKVvds);
                    vds = (1+xfact)* (*(ckt->CKTstate1 + here->EKVvds))
                        -(xfact * (*(ckt->CKTstate2 + here->EKVvds)));
                    *(ckt->CKTstate0 + here->EKVvbd) = 
                        *(ckt->CKTstate0 + here->EKVvbs)-
                        *(ckt->CKTstate0 + here->EKVvds);
                } else {
#endif /* PREDICTOR */

                    /* general iteration */

                    vbs = model->EKVtype * ( 
                        *(ckt->CKTrhsOld+here->EKVbNode) -
                        *(ckt->CKTrhsOld+here->EKVsNodePrime));
                    vgs = model->EKVtype * ( 
                        *(ckt->CKTrhsOld+here->EKVgNode) -
                        *(ckt->CKTrhsOld+here->EKVsNodePrime));
                    vds = model->EKVtype * ( 
                        *(ckt->CKTrhsOld+here->EKVdNodePrime) -
                        *(ckt->CKTrhsOld+here->EKVsNodePrime));
#ifndef PREDICTOR
                }
#endif /* PREDICTOR */

                /* now some common crunching for some more useful quantities */

                vbd=vbs-vds;
                vgd=vgs-vds;
                vgdo = *(ckt->CKTstate0 + here->EKVvgs) - 
                    *(ckt->CKTstate0 + here->EKVvds);
                delvbs = vbs - *(ckt->CKTstate0 + here->EKVvbs);
                delvbd = vbd - *(ckt->CKTstate0 + here->EKVvbd);
                delvgs = vgs - *(ckt->CKTstate0 + here->EKVvgs);
                delvds = vds - *(ckt->CKTstate0 + here->EKVvds);
                delvgd = vgd-vgdo;

                /* these are needed for convergence testing */

                if (here->EKVmode >= 0) {
                    cdhat=
                        here->EKVcd-
                        here->EKVgbd * delvbd +
                        here->EKVgmbs * delvbs +
                        here->EKVgm * delvgs + 
                        here->EKVgds * delvds ;
                } else {
                    cdhat=
                        here->EKVcd -
                        ( here->EKVgbd -
                        here->EKVgmbs) * delvbd -
                        here->EKVgm * delvgd + 
                        here->EKVgds * delvds ;
                }
                cbhat=
                    here->EKVcbs +
                    here->EKVcbd +
                    here->EKVgbd * delvbd +
                    here->EKVgbs * delvbs ;
#ifndef NOBYPASS
                /* now lets see if we can bypass (ugh) */
                /* the following mess should be one if statement, but
                 * many compilers can't handle it all at once, so it
                 * is split into several successive if statements
                 */
                tempv = MAX(FABS(cbhat),FABS(here->EKVcbs
                    + here->EKVcbd))+ckt->CKTabstol;
                if((!(ckt->CKTmode & (MODEINITPRED|MODEINITTRAN|MODEINITSMSIG)
                    )) && (ckt->CKTbypass) )
                    if ( (FABS(cbhat-(here->EKVcbs + 
                        here->EKVcbd)) < ckt->CKTreltol * 
                        tempv))
                        if( (FABS(delvbs) < (ckt->CKTreltol * MAX(FABS(vbs),
                            FABS(*(ckt->CKTstate0+here->EKVvbs)))+
                            ckt->CKTvoltTol)))
                            if ( (FABS(delvbd) < (ckt->CKTreltol * MAX(FABS(vbd),
                                FABS(*(ckt->CKTstate0+here->EKVvbd)))+
                                ckt->CKTvoltTol)) )
                                if( (FABS(delvgs) < (ckt->CKTreltol * MAX(FABS(vgs),
                                    FABS(*(ckt->CKTstate0+here->EKVvgs)))+
                                    ckt->CKTvoltTol)))
/* code folded from here */
    if ( (FABS(delvds) < (ckt->CKTreltol * MAX(FABS(vds),
        FABS(*(ckt->CKTstate0+here->EKVvds)))+
        ckt->CKTvoltTol)) )
        if( (FABS(cdhat- here->EKVcd) <
            ckt->CKTreltol * MAX(FABS(cdhat),FABS(
            here->EKVcd)) + ckt->CKTabstol) ) {
/* unfolding */
            /* bypass code
                     * nothing interesting has changed since last
                     * iteration on this device, so we just
                     * copy all the values computed last iteration out
                     * and keep going
                     */
/* code folded from here */
            vbs = *(ckt->CKTstate0 + here->EKVvbs);
            vbd = *(ckt->CKTstate0 + here->EKVvbd);
            vgs = *(ckt->CKTstate0 + here->EKVvgs);
            vds = *(ckt->CKTstate0 + here->EKVvds);
            vgd = vgs - vds;
            vgb = vgs - vbs;

            cdrain = here->EKVmode *
                (here->EKVcd + here->EKVcbd);

            if(ckt->CKTmode & (MODETRAN | MODETRANOP)) {
                capgs = ( *(ckt->CKTstate0+here->EKVcapgs)+ 
                    *(ckt->CKTstate1+here->EKVcapgs) +
                    GateSourceOverlapCap );
                capgd = ( *(ckt->CKTstate0+here->EKVcapgd)+ 
                    *(ckt->CKTstate1+here->EKVcapgd) +
                    GateDrainOverlapCap );
                capgb = ( *(ckt->CKTstate0+here->EKVcapgb)+ 
                    *(ckt->CKTstate1+here->EKVcapgb) +
                    GateBulkOverlapCap );

#ifdef HAS_SENSE2
                if(ckt->CKTsenInfo){
                    here->EKVcgs = capgs;
                    here->EKVcgd = capgd;
                    here->EKVcgb = capgb;
                }
#endif
            }
            goto bypass;
        }
#endif /*NOBYPASS*/
/* unfolding */
                /* ok - bypass is out, do it the hard way */

                von = model->EKVtype * here->EKVvon;

#ifndef NODELIMITING
                /* 
                 * limiting
                 *  we want to keep device voltages from changing
                 * so fast that the exponentials churn out overflows
                 * and similar rudeness
                 */

                if(*(ckt->CKTstate0 + here->EKVvds) >=0) {
                    vgs = DEV.fetlim(vgs,*(ckt->CKTstate0 + here->EKVvgs)
                        ,von);
                    vds = vgs - vgd;
                    vds = DEV.limvds(vds,*(ckt->CKTstate0 + here->EKVvds));
                    vgd = vgs - vds;
                } else {
                    vgd = DEV.fetlim(vgd,vgdo,von);
                    vds = vgs - vgd;
                    if(!(ckt->CKTfixLimit)) {
                        vds = -DEV.limvds(-vds,-(*(ckt->CKTstate0 + 
                            here->EKVvds)));
                    }
                    vgs = vgd + vds;
                }
                if(vds >= 0) {
                    vbs = DEV.pnjlim(vbs,*(ckt->CKTstate0 + here->EKVvbs),
                        vt,here->EKVsourceVcrit,&Check);
                    vbd = vbs-vds;
                } else {
                    vbd = DEV.pnjlim(vbd,*(ckt->CKTstate0 + here->EKVvbd),
                        vt,here->EKVdrainVcrit,&Check);
                    vbs = vbd + vds;
                }
#endif /*NODELIMITING*/
            } else {

                /* ok - not one of the simple cases, so we have to
                 * look at all of the possibilities for why we were
                 * called.  We still just initialize the three voltages
                 */

                if((ckt->CKTmode & MODEINITJCT) && !here->EKVoff) {
                    vds= model->EKVtype * here->EKVicVDS;
                    vgs= model->EKVtype * here->EKVicVGS;
                    vbs= model->EKVtype * here->EKVicVBS;
                    if((vds==0) && (vgs==0) && (vbs==0) && 
                        ((ckt->CKTmode & 
                        (MODETRAN|MODEDCOP|MODEDCTRANCURVE)) ||
                        (!(ckt->CKTmode & MODEUIC)))) {
                        vbs = -1;
                        vgs = model->EKVtype * here->EKVtVto;
                        vds = 0;
                    }
                } else {
                    vbs=vgs=vds=0;
                }
            }
            /*
             * now all the preliminaries are over - we can start doing the
             * real work
             */
            vbd = vbs - vds;
            vgd = vgs - vds;
            vgb = vgs - vbs;


            /*
             * bulk-source and bulk-drain diodes
             *   here we just evaluate the ideal diode current and the
             *   corresponding derivative (conductance).
             */
#ifdef HAS_SENSE2
next1:      
#endif
            if(vbs <= 0) {
                here->EKVgbs = SourceSatCur/vt;
                here->EKVcbs = here->EKVgbs*vbs;
                here->EKVgbs += ckt->CKTgmin;
            } else {
                evbs = exp(MIN(MAX_EXP_ARG,vbs/vt));
                here->EKVgbs = SourceSatCur*evbs/vt + ckt->CKTgmin;
                here->EKVcbs = SourceSatCur * (evbs-1);
            }
            if(vbd <= 0) {
                here->EKVgbd = DrainSatCur/vt;
                here->EKVcbd = here->EKVgbd *vbd;
                here->EKVgbd += ckt->CKTgmin;
            } else {
                evbd = exp(MIN(MAX_EXP_ARG,vbd/vt));
                here->EKVgbd = DrainSatCur*evbd/vt +ckt->CKTgmin;
                here->EKVcbd = DrainSatCur *(evbd-1);
            }

            /* now to determine whether the user was able to correctly
             * identify the source and drain of his device
             */
            if(vds >= 0)
                /* normal mode */
                here->EKVmode = 1;
            else
                /* inverse mode */
                here->EKVmode = -1;

            /*
             *     this block of code evaluates the drain current and its 
             *     derivatives using EKV V2.3 model 
             *
             */

            if (vds>=0.0)
                EKVevaluate(vds,vbs,vgs,here,model,&von,&vdsat,ckt);
            else
                EKVevaluate(-vds,vbd,vgd,here,model,&von,&vdsat,ckt);

            cdrain = here->EKVcd;

            /* now deal with n vs p polarity */

            here->EKVvon   = model->EKVtype * von;
            here->EKVvdsat = model->EKVtype * vdsat;

            /* line 490 */
            /*
             *  COMPUTE EQUIVALENT DRAIN CURRENT SOURCE
             */
            here->EKVcd=here->EKVmode * cdrain - here->EKVcbd;

            if (ckt->CKTmode & (MODETRAN | MODETRANOP | MODEINITSMSIG)) {
                /* 
                 * now we do the hard part of the bulk-drain and bulk-source
                 * diode - we evaluate the non-linear capacitance and
                 * charge
                 *
                 * the basic equations are not hard, but the implementation
                 * is somewhat long in an attempt to avoid log/exponential
                 * evaluations
                 */
                /*
                 *  charge storage elements
                 *
                 *.. bulk-drain and bulk-source depletion capacitances
                 */
#ifdef CAPBYPASS
                if(((ckt->CKTmode & (MODEINITPRED | MODEINITTRAN) ) ||
                    FABS(delvbs) >= ckt->CKTreltol * MAX(FABS(vbs),
                    FABS(*(ckt->CKTstate0+here->EKVvbs)))+
                    ckt->CKTvoltTol)|| senflag)
#endif /*CAPBYPASS*/
                    {
                        /* can't bypass the diode capacitance calculations */
#ifdef CAPZEROBYPASS
                        if(here->EKVCbs != 0 || here->EKVCbssw != 0 ) {
#endif /*CAPZEROBYPASS*/
                            if (vbs < here->EKVtDepCap){
                                arg=1-vbs/here->EKVtBulkPot;
                                /*
                         * the following block looks somewhat long and messy,
                         * but since most users use the default grading
                         * coefficients of .5, and sqrt is MUCH faster than an
                         * exp(log()) we use this special case code to buy time.
                         * (as much as 10% of total job time!)
                         */
#ifndef NOSQRT
                                if(model->EKVbulkJctBotGradingCoeff ==
                                    model->EKVbulkJctSideGradingCoeff) {
/* code folded from here */
    if(model->EKVbulkJctBotGradingCoeff == .5) {
        sarg = sargsw = 1/sqrt(arg);
    } else {
        sarg = sargsw =
            exp(-model->EKVbulkJctBotGradingCoeff*
            log(arg));
    }
/* unfolding */
                                } else {
/* code folded from here */
    if(model->EKVbulkJctBotGradingCoeff == .5) {
        sarg = 1/sqrt(arg);
    } else {
#endif /*NOSQRT*/
        sarg = exp(-model->EKVbulkJctBotGradingCoeff*
            log(arg));
#ifndef NOSQRT
    }
    if(model->EKVbulkJctSideGradingCoeff == .5) {
        sargsw = 1/sqrt(arg);
    } else {
#endif /*NOSQRT*/
        sargsw =exp(-model->EKVbulkJctSideGradingCoeff*
            log(arg));
#ifndef NOSQRT
    }
/* unfolding */
                                }
#endif /*NOSQRT*/
                                *(ckt->CKTstate0 + here->EKVqbs) =
                                    here->EKVtBulkPot*(here->EKVCbs*
                                    (1-arg*sarg)/(1-model->EKVbulkJctBotGradingCoeff)
                                    +here->EKVCbssw*
                                    (1-arg*sargsw)/
                                    (1-model->EKVbulkJctSideGradingCoeff));
                                here->EKVcapbs=here->EKVCbs*sarg+
                                    here->EKVCbssw*sargsw;
                            } else {
                                *(ckt->CKTstate0 + here->EKVqbs) = here->EKVf4s +
                                    vbs*(here->EKVf2s+vbs*(here->EKVf3s/2));
                                here->EKVcapbs=here->EKVf2s+here->EKVf3s*vbs;
                            }
#ifdef CAPZEROBYPASS
                        } else {
                            *(ckt->CKTstate0 + here->EKVqbs) = 0;
                            here->EKVcapbs=0;
                        }
#endif /*CAPZEROBYPASS*/
                    }
#ifdef CAPBYPASS
                if(((ckt->CKTmode & (MODEINITPRED | MODEINITTRAN) ) ||
                    FABS(delvbd) >= ckt->CKTreltol * MAX(FABS(vbd),
                    FABS(*(ckt->CKTstate0+here->EKVvbd)))+
                    ckt->CKTvoltTol)|| senflag)
#endif /*CAPBYPASS*/
                    /* can't bypass the diode capacitance calculations */
                    {
#ifdef CAPZEROBYPASS
                        if(here->EKVCbd != 0 || here->EKVCbdsw != 0 ) {
#endif /*CAPZEROBYPASS*/
                            if (vbd < here->EKVtDepCap) {
                                arg=1-vbd/here->EKVtBulkPot;
                                /*
                         * the following block looks somewhat long and messy,
                         * but since most users use the default grading
                         * coefficients of .5, and sqrt is MUCH faster than an
                         * exp(log()) we use this special case code to buy time.
                         * (as much as 10% of total job time!)
                         */
#ifndef NOSQRT
                                if(model->EKVbulkJctBotGradingCoeff == .5 &&
                                    model->EKVbulkJctSideGradingCoeff == .5) {
/* code folded from here */
    sarg = sargsw = 1/sqrt(arg);
/* unfolding */
                                } else {
/* code folded from here */
    if(model->EKVbulkJctBotGradingCoeff == .5) {
        sarg = 1/sqrt(arg);
    } else {
#endif /*NOSQRT*/
        sarg = exp(-model->EKVbulkJctBotGradingCoeff*
            log(arg));
#ifndef NOSQRT
    }
    if(model->EKVbulkJctSideGradingCoeff == .5) {
        sargsw = 1/sqrt(arg);
    } else {
#endif /*NOSQRT*/
        sargsw =exp(-model->EKVbulkJctSideGradingCoeff*
            log(arg));
#ifndef NOSQRT
    }
/* unfolding */
                                }
#endif /*NOSQRT*/
                                *(ckt->CKTstate0 + here->EKVqbd) =
                                    here->EKVtBulkPot*(here->EKVCbd*
                                    (1-arg*sarg)
                                    /(1-model->EKVbulkJctBotGradingCoeff)
                                    +here->EKVCbdsw*
                                    (1-arg*sargsw)
                                    /(1-model->EKVbulkJctSideGradingCoeff));
                                here->EKVcapbd=here->EKVCbd*sarg+
                                    here->EKVCbdsw*sargsw;
                            } else {
                                *(ckt->CKTstate0 + here->EKVqbd) = here->EKVf4d +
                                    vbd * (here->EKVf2d + vbd * here->EKVf3d/2);
                                here->EKVcapbd=here->EKVf2d + vbd * here->EKVf3d;
                            }
#ifdef CAPZEROBYPASS
                        } else {
                            *(ckt->CKTstate0 + here->EKVqbd) = 0;
                            here->EKVcapbd = 0;
                        }
#endif /*CAPZEROBYPASS*/
                    }
#ifdef HAS_SENSE2
                if(SenCond && (ckt->CKTsenInfo->SENmode==TRANSEN)) goto next2;
#endif

                if ( (ckt->CKTmode & MODETRAN) || ( (ckt->CKTmode&MODEINITTRAN)
                    && !(ckt->CKTmode&MODEUIC)) ) {
                    /* (above only excludes tranop, since we're only at this
                     * point if tran or tranop )
                     */

                    /*
                     *    calculate equivalent conductances and currents for
                     *    depletion capacitors
                     */

                    /* integrate the capacitors and save results */

                    error = NIintegrate(ckt,&geq,&ceq,here->EKVcapbd,
                        here->EKVqbd);
                    if(error) return(error);
                    here->EKVgbd += geq;
                    here->EKVcbd += *(ckt->CKTstate0 + here->EKVcqbd);
                    here->EKVcd -= *(ckt->CKTstate0 + here->EKVcqbd);
                    error = NIintegrate(ckt,&geq,&ceq,here->EKVcapbs,
                        here->EKVqbs);
                    if(error) return(error);
                    here->EKVgbs += geq;
                    here->EKVcbs += *(ckt->CKTstate0 + here->EKVcqbs);
                }
            }
#ifdef HAS_SENSE2
            if(SenCond) goto next2;
#endif

            /*
             *  check convergence
             */
            if ( (here->EKVoff == 0)  || 
                (!(ckt->CKTmode & (MODEINITFIX|MODEINITSMSIG))) ){
                if (Check == 1) {
                    ckt->incNoncon();  // SRW
                    ckt->CKTtroubleElt = (GENinstance *) here;
#ifndef NEWCONV
                } else {
                    tol=ckt->CKTreltol*MAX(FABS(cdhat),
                        FABS(here->EKVcd))+ckt->CKTabstol;
                    if (FABS(cdhat-here->EKVcd) >= tol) {
                        ckt->incNoncon();  // SRW
                        ckt->CKTtroubleElt = (GENinstance *) here;
                    } else {
                        tol=ckt->CKTreltol*MAX(FABS(cbhat),
                            FABS(here->EKVcbs+here->EKVcbd))+
                            ckt->CKTabstol;
                        if (FABS(cbhat-(here->EKVcbs+here->EKVcbd)) > tol) {
                            ckt->incNoncon();  // SRW
                            ckt->CKTtroubleElt = (GENinstance *) here;
                        }
                    }
#endif /*NEWCONV*/
                }
            }
            /* save things away for next time */

#ifdef HAS_SENSE2
next2:      
#endif
            *(ckt->CKTstate0 + here->EKVvbs) = vbs;
            *(ckt->CKTstate0 + here->EKVvbd) = vbd;
            *(ckt->CKTstate0 + here->EKVvgs) = vgs;
            *(ckt->CKTstate0 + here->EKVvds) = vds;

            /*
             *     meyer's capacitor model
             */
            if ( ckt->CKTmode & (MODETRAN | MODETRANOP | MODEINITSMSIG) ) {
                /*
                 *     calculate meyer's capacitors
                 */
                /* 
                 * new cmeyer - this just evaluates at the current time,
                 * expects you to remember values from previous time
                 * returns 1/2 of non-constant portion of capacitance
                 * you must add in the other half from previous time
                 * and the constant part
                 */
                if (here->EKVmode > 0)
                    EKVcap(model,here,vds,vbs,
                        (ckt->CKTstate0 + here->EKVcapgs),
                        (ckt->CKTstate0 + here->EKVcapgd),
                        (ckt->CKTstate0 + here->EKVcapgb));
                else
                    EKVcap(model,here,-vds,vbd,
                        (ckt->CKTstate0 + here->EKVcapgd),
                        (ckt->CKTstate0 + here->EKVcapgs),
                        (ckt->CKTstate0 + here->EKVcapgb));

                vgs1 = *(ckt->CKTstate1 + here->EKVvgs);
                vgd1 = vgs1 - *(ckt->CKTstate1 + here->EKVvds);
                vgb1 = vgs1 - *(ckt->CKTstate1 + here->EKVvbs);
                if(ckt->CKTmode & (MODETRANOP|MODEINITSMSIG)) {
                    capgs =  2 * *(ckt->CKTstate0+here->EKVcapgs)+ 
                        GateSourceOverlapCap ;
                    capgd =  2 * *(ckt->CKTstate0+here->EKVcapgd)+ 
                        GateDrainOverlapCap ;
                    capgb =  2 * *(ckt->CKTstate0+here->EKVcapgb)+ 
                        GateBulkOverlapCap ;
                } else {
                    capgs = ( *(ckt->CKTstate0+here->EKVcapgs)+ 
                        *(ckt->CKTstate1+here->EKVcapgs) +
                        GateSourceOverlapCap );
                    capgd = ( *(ckt->CKTstate0+here->EKVcapgd)+ 
                        *(ckt->CKTstate1+here->EKVcapgd) +
                        GateDrainOverlapCap );
                    capgb = ( *(ckt->CKTstate0+here->EKVcapgb)+ 
                        *(ckt->CKTstate1+here->EKVcapgb) +
                        GateBulkOverlapCap );
                }
#ifdef HAS_SENSE2
                if(ckt->CKTsenInfo){
                    here->EKVcgs = capgs;
                    here->EKVcgd = capgd;
                    here->EKVcgb = capgb;
                }
                /*
                 *     store small-signal parameters (for meyer's model)
                 *  all parameters already stored, so done...
                 */
                if(SenCond){
                    if((ckt->CKTsenInfo->SENmode == DCSEN)||
                        (ckt->CKTsenInfo->SENmode == ACSEN)){
// SRW                        continue;
                        return (OK);
                    }
                }
#endif

#ifndef PREDICTOR
                if (ckt->CKTmode & (MODEINITPRED | MODEINITTRAN) ) {
                    *(ckt->CKTstate0 + here->EKVqgs) =
                        (1+xfact) * *(ckt->CKTstate1 + here->EKVqgs)
                        - xfact * *(ckt->CKTstate2 + here->EKVqgs);
                    *(ckt->CKTstate0 + here->EKVqgd) =
                        (1+xfact) * *(ckt->CKTstate1 + here->EKVqgd)
                        - xfact * *(ckt->CKTstate2 + here->EKVqgd);
                    *(ckt->CKTstate0 + here->EKVqgb) =
                        (1+xfact) * *(ckt->CKTstate1 + here->EKVqgb)
                        - xfact * *(ckt->CKTstate2 + here->EKVqgb);
                } else {
#endif /*PREDICTOR*/
                    if(ckt->CKTmode & MODETRAN) {
                        *(ckt->CKTstate0 + here->EKVqgs) = (vgs-vgs1)*capgs +
                            *(ckt->CKTstate1 + here->EKVqgs) ;
                        *(ckt->CKTstate0 + here->EKVqgd) = (vgd-vgd1)*capgd +
                            *(ckt->CKTstate1 + here->EKVqgd) ;
                        *(ckt->CKTstate0 + here->EKVqgb) = (vgb-vgb1)*capgb +
                            *(ckt->CKTstate1 + here->EKVqgb) ;
                    } else {
                        /* TRANOP only */
                        *(ckt->CKTstate0 + here->EKVqgs) = vgs*capgs;
                        *(ckt->CKTstate0 + here->EKVqgd) = vgd*capgd;
                        *(ckt->CKTstate0 + here->EKVqgb) = vgb*capgb;
                    }
#ifndef PREDICTOR
                }
#endif /*PREDICTOR*/
            }
bypass:
#ifdef HAS_SENSE2
// SRW            if(SenCond) continue;
            if(SenCond) return (OK);
#endif

            if ( (ckt->CKTmode & (MODEINITTRAN)) || 
                (! (ckt->CKTmode & (MODETRAN)) )  ) {
                /*
                 *  initialize to zero charge conductances 
                 *  and current
                 */
                gcgs=0;
                ceqgs=0;
                gcgd=0;
                ceqgd=0;
                gcgb=0;
                ceqgb=0;
            } else {
                if(capgs == 0) *(ckt->CKTstate0 + here->EKVcqgs) =0;
                if(capgd == 0) *(ckt->CKTstate0 + here->EKVcqgd) =0;
                if(capgb == 0) *(ckt->CKTstate0 + here->EKVcqgb) =0;
                /*
                 *    calculate equivalent conductances and currents for
                 *    meyer"s capacitors
                 */
                error = NIintegrate(ckt,&gcgs,&ceqgs,capgs,here->EKVqgs);
                if(error) return(error);
                error = NIintegrate(ckt,&gcgd,&ceqgd,capgd,here->EKVqgd);
                if(error) return(error);
                error = NIintegrate(ckt,&gcgb,&ceqgb,capgb,here->EKVqgb);
                if(error) return(error);
                ceqgs=ceqgs-gcgs*vgs+ckt->CKTag[0]* 
                    *(ckt->CKTstate0 + here->EKVqgs);
                ceqgd=ceqgd-gcgd*vgd+ckt->CKTag[0]*
                    *(ckt->CKTstate0 + here->EKVqgd);
                ceqgb=ceqgb-gcgb*vgb+ckt->CKTag[0]*
                    *(ckt->CKTstate0 + here->EKVqgb);
            }
            /*
             *     store charge storage info for meyer's cap in lx table
             */

// SRW -- save for interpolation
*(ckt->CKTstate0 + here->EKVa_cd) = here->EKVcd;  
*(ckt->CKTstate0 + here->EKVa_cbd) = here->EKVcbd;  
*(ckt->CKTstate0 + here->EKVa_cbs) = here->EKVcbs;  

            /*
             *  load current vector
             */
            ceqbs = model->EKVtype * 
                (here->EKVcbs-(here->EKVgbs-ckt->CKTgmin)*vbs);
            ceqbd = model->EKVtype * 
                (here->EKVcbd-(here->EKVgbd-ckt->CKTgmin)*vbd);
            if (here->EKVmode >= 0) {
                xnrm=1;
                xrev=0;
                cdreq=model->EKVtype*(cdrain-here->EKVgds*vds-
                    here->EKVgm*vgs-here->EKVgmbs*vbs);
                ceqbs -= model->EKVtype*here->EKVisub;
            } else {
                xnrm=0;
                xrev=1;
                cdreq = -(model->EKVtype)*(cdrain-here->EKVgds*(-vds)-
                    here->EKVgm*vgd-here->EKVgmbs*vbd);
                ceqbd -= model->EKVtype*here->EKVisub;
            }
            ckt->rhsadd(here->EKVgNode, -( 
                (model->EKVtype * (ceqgs + ceqgb + ceqgd))));
            ckt->rhsadd(here->EKVbNode, -(
                (ceqbs + ceqbd - model->EKVtype * ceqgb)));
            ckt->rhsadd(here->EKVdNodePrime,
                (ceqbd - cdreq + model->EKVtype * ceqgd));
            ckt->rhsadd(here->EKVsNodePrime,
                cdreq + ceqbs + model->EKVtype * ceqgs);
            /*
             *  load y matrix
             */

            ckt->ldadd(here->EKVDdPtr, here->EKVdrainConductance);
            ckt->ldadd(here->EKVGgPtr, gcgd+gcgs+gcgb);
            ckt->ldadd(here->EKVSsPtr, here->EKVsourceConductance);
            ckt->ldadd(here->EKVBbPtr, here->EKVgbd+here->EKVgbs+gcgb);
            ckt->ldadd(here->EKVDPdpPtr,
                here->EKVdrainConductance+here->EKVgds+
                here->EKVgbd+xrev*(here->EKVgm+here->EKVgmbs)+gcgd);
            ckt->ldadd(here->EKVSPspPtr,
                here->EKVsourceConductance+here->EKVgds+
                here->EKVgbs+xnrm*(here->EKVgm+here->EKVgmbs)+gcgs);
            ckt->ldadd(here->EKVDdpPtr, -here->EKVdrainConductance);
            ckt->ldadd(here->EKVGbPtr, -gcgb);
            ckt->ldadd(here->EKVGdpPtr, -gcgd);
            ckt->ldadd(here->EKVGspPtr, -gcgs);
            ckt->ldadd(here->EKVSspPtr, -here->EKVsourceConductance);
            ckt->ldadd(here->EKVBgPtr, -gcgb);
            ckt->ldadd(here->EKVBdpPtr, -here->EKVgbd);
            ckt->ldadd(here->EKVBspPtr, -here->EKVgbs);
            ckt->ldadd(here->EKVDPdPtr, -here->EKVdrainConductance);
            ckt->ldadd(here->EKVDPgPtr, (xnrm-xrev)*here->EKVgm-gcgd);
            ckt->ldadd(here->EKVDPbPtr, -here->EKVgbd+(xnrm-xrev)*here->EKVgmbs);
            ckt->ldadd(here->EKVDPspPtr, -here->EKVgds-xnrm*
                (here->EKVgm+here->EKVgmbs));
            ckt->ldadd(here->EKVSPgPtr, -(xnrm-xrev)*here->EKVgm-gcgs);
            ckt->ldadd(here->EKVSPsPtr, -here->EKVsourceConductance);
            ckt->ldadd(here->EKVSPbPtr, -here->EKVgbs-(xnrm-xrev)*here->EKVgmbs);
            ckt->ldadd(here->EKVSPdpPtr, -here->EKVgds-xrev*
                (here->EKVgm+here->EKVgmbs));
// SRW        }
// SRW    }
    return(OK);
}

