
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
 * Xic Integrated Circuit Layout and Schematic Editor                     *
 *                                                                        *
 *========================================================================*
 $Id:$
 *========================================================================*/

#ifndef DSP_TKIF_H
#define DSP_TKIF_H

//
// Toolkit interface.  These classes will be implemented using
// toolkit-specific code.
//

#include "ginterf/graphics.h"

struct hostent;


// Type of font description string, for SetFont.
enum FNT_FMT
{
    FNT_FMT_ANY = 0,    // unknown or unspecified
    FNT_FMT_P   = 1,    // Pango description (GTK2)
    FNT_FMT_Q   = 2,    // QT font description
    FNT_FMT_W   = 3,    // Microsoft Windows
    FNT_FMT_X   = 4     // X font description
};

// The graphics package class.
//
class DSPpkg : public GRpkg
{
public:
    DSPpkg()
    {
        app_busy = 0;
        app_override_busy = false;
        dispatch_events = true;
    }

    virtual ~DSPpkg() { }

    static DSPpkg *self()   { return (dynamic_cast<DSPpkg*>(GRpkg::self())); }

    bool IsBusy()   { return (app_busy && !app_override_busy); }

    bool CheckForInterruptDispatch(bool set)
    {
        bool tmp = dispatch_events;
        dispatch_events = set;
        return (tmp);
    }

    // Misc globals
    virtual GRwbag *NewGX() = 0;
    virtual int Initialize(GRwbag*) = 0;
    virtual void ReinitNoGraphics() = 0;
    virtual void Halt() = 0;
    virtual void AppLoop() = 0;
    virtual bool CheckForInterrupt() = 0;
    virtual int Iconify(int) = 0;
    virtual bool SubwinInit(int) = 0;
    virtual void SubwinDestroy(int) = 0;

    virtual bool SetWorking(bool) = 0;
    virtual void SetOverrideBusy(bool) = 0;
    virtual bool GetMainWinIdentifier(char*) = 0;

    virtual bool IsDualPlane() = 0;
    virtual bool IsTrueColor() = 0;
    virtual bool UsingX11() = 0;
    virtual void CloseGraphicsConnection() = 0;
    virtual const char *GetDisplayString() = 0;
    virtual bool CheckScreenAccess(hostent*, const char*, const char*) = 0;
    virtual int RegisterIdleProc(int(*)(void*), void*) = 0;
    virtual bool RemoveIdleProc(int) = 0;
    virtual int RegisterTimeoutProc(int, int(*)(void*), void*) = 0;
    virtual bool RemoveTimeoutProc(int) = 0;
    virtual int StartTimer(int, bool*) = 0;
    virtual void SetFont(const char*, int, FNT_FMT = FNT_FMT_ANY) = 0;
    virtual const char *GetFont(int) = 0;
    virtual FNT_FMT GetFontFmt() = 0;

protected:
    int app_busy; // Busy level, set while performing long operations.
    bool app_override_busy;  // Override busy state
    bool dispatch_events;    // CheckForInterrupt will dispatch events
};

// Per-window toolkit interface.
//
class cAppWinFuncs
{
public:
    virtual ~cAppWinFuncs() { }

    // pixmap manipulations
    virtual void SwitchToPixmap()                                   = 0;
    virtual void SwitchFromPixmap(const BBox*)                      = 0;
    virtual GRobject DrawableReset()                                = 0;
    virtual void CopyPixmap(const BBox*)                            = 0;
    virtual void DestroyPixmap()                                    = 0;
    virtual bool DumpWindow(const char*, const BBox*)               = 0;
    virtual bool PixmapOk()                                         = 0;

    // key handling/display interface
    virtual void GetTextBuf(char*)                                  = 0;
    virtual void SetTextBuf(const char*)                            = 0;
    virtual void ShowKeys()                                         = 0;
    virtual void SetKeys(const char*)                               = 0;
    virtual void BspKeys()                                          = 0;
    virtual void CheckExec(bool)                                    = 0;
    virtual char *KeyBuf()                                          = 0;
    virtual int KeyPos()                                            = 0;

    // label
    virtual void SetLabelText(const char*)                          = 0;

    // misc. pop-ups
    virtual void PopUpGrid(GRobject, ShowMode)                      = 0;
    virtual void PopUpExpand(GRobject, ShowMode,
        bool(*)(const char*, void*), void*, const char*, bool)      = 0;
    virtual void PopUpZoom(GRobject, ShowMode)                      = 0;
};

// Base class for the "widget bag" associated with each drawing window.
//
struct DSPwbag : virtual public GRwbag, virtual public cAppWinFuncs
{
};

#endif

