
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
 * Ginterf Graphical Interface Library                                    *
 *                                                                        *
 *========================================================================*
 $Id:$
 *========================================================================*/

#ifndef GR_PKG_SETUP_H
#define GR_PKG_SETUP_H

// This must included in the application once, it provides the package
// initialization function.  The variable GR_CONFIG must be defined to
// an ORed list of the tokens listed at the top of graphics.h, before
// this file is included:  For example,
//
//     #include "graphics.h"
//      . . .
//     #define GR_CONFIG  (GR_ALL_PKGS | GR_ALL_DRIVERS)
//     #include "gr_pkg_setup.h"
//

#include "nulldev.h"
#if (GR_CONFIG & _devPS_)
#include "hcpslindrw.h"
#endif
#if (GR_CONFIG & _devPSBM_)
#include "hcpsbm.h"
#endif
#if (GR_CONFIG & _devPSBC_)
#include "hcpsbc.h"
#endif
#if (GR_CONFIG & _devHP_)
#include "hchpgl.h"
#endif
#if (GR_CONFIG & _devPCL_)
#include "hcpcl.h"
#endif
#if (GR_CONFIG & _devXF_)
#include "hcxfig.h"
#endif
#if (GR_CONFIG & _devIM_)
#include "hcimlib.h"
#endif

// Initialize the package list and the screen package.  Return false
// on success, true if error.  The first argument represents the
// initial driver set, which should be a subset of thise given in
// GR_CONFIG.
//
bool
GRpkg::InitPkg(unsigned int cfg, int *argc, char **argv)
{
#if ((GR_CONFIG & GR_ALL_PKGS) == _devNULL_)
    RegisterDevice(new NULLdev);
#else
    if ((cfg & GR_ALL_PKGS) != _devNULL_)
        DevDepInit(GR_CONFIG);
    else
        RegisterDevice(new NULLdev);
#endif
#if (GR_CONFIG & _devPS_)
    if (cfg & _devPS_) {
        RegisterDevice(new PSdev);
        RegisterHcopyDesc(&PSdesc_m);
        RegisterHcopyDesc(&PSdesc_c);
    }
#endif
#if (GR_CONFIG & _devPSBM_)
    if (cfg & _devPSBM_) {
        RegisterDevice(new PSBMdev);
        RegisterHcopyDesc(&PSBMdesc);
        RegisterHcopyDesc(&PSBMdesc_e);
    }
#endif
#if (GR_CONFIG & _devPSBC_)
    if (cfg & _devPSBC_) {
        RegisterDevice(new PSBCdev);
        RegisterHcopyDesc(&PSBCdesc);
        RegisterHcopyDesc(&PSBCdesc_e);
    }
#endif
#if (GR_CONFIG & _devHP_)
    if (cfg & _devHP_) {
        RegisterDevice(new HPdev);
        RegisterHcopyDesc(&HPdesc);
    }
#endif
#if (GR_CONFIG & _devPCL_)
    if (cfg & _devPCL_) {
        RegisterDevice(new PCLdev);
        RegisterHcopyDesc(&PCLdesc);
    }
#endif
#if (GR_CONFIG & _devXF_)
    if (cfg & _devXF_) {
        RegisterDevice(new XFdev);
        RegisterHcopyDesc(&XFdesc);
    }
#endif
#if (GR_CONFIG & _devIM_)
    if (cfg & _devIM_) {
        RegisterDevice(new IMdev);
        RegisterHcopyDesc(&IMdesc);
    }
#endif

    // pkg_device[0] is always the screen device
    if (pkg_devices[0]->Init(argc, argv))
        return (true);
    pkg_cur_dev = pkg_devices[0];
    pkg_main_dev = dynamic_cast<GRscreenDev*>(pkg_cur_dev);
    return (false);
}

#endif

