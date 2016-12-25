/*
Chroma Shift filter for AviUtl

Copyright (c) 2016 OKA Motofumi(chikuzen.mo at gmail dot com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#include <windows.h>
#include "filter.h"


static void
copy_to_temp(const short* orig, short* temp, const int width, const int height,
    const int stride) noexcept
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            temp[x] = orig[3 * x];
        }
        orig += stride;
        temp += stride;
    }
}


static void
copy_to_orig(const short* temp, short* orig, const int width, const int height,
    const int stride) noexcept
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            orig[3 * x] = temp[x];
        }
        temp += stride;
        orig += stride;
    }
}


static void
set_params(const int shift, int& offset_s, int& offset_d, int& c0, int& c1, int& first)
{
    if (shift > 0) {
        offset_s = 0;
        offset_d = 1 + shift / 4;
        first = -offset_d;
        c1 = 4 - (shift & 3);
        c0 = 4 - c1;
    } else {
        offset_s = -shift / 4 + 1;
        offset_d = 0;
        first = offset_s - 1;
        c0 = 4 - (-shift & 3);
        c1 = 4 - c0;
    }
}


static void
shift_h(const int shift, const short* srcp, short* dstp, const int width,
        const int height, const int stride) noexcept
{
    int offset_s, offset_d, c0, c1, left;
    set_params(shift, offset_s, offset_d, c0, c1, left);
    const int right = left + 1;
    const int last = width - offset_s;

    for (int y = 0; y < height; ++y) {
        int x = 0;
        for (; x < offset_d; ++x) {
            dstp[x] = srcp[0];
        }
        for (; x < last; ++x) {
            dstp[x] = (srcp[(x + left) * 3] * c0 + srcp[(x + right) * 3] * c1) / 4;
        }
        for (; x < width; ++x) {
            dstp[x] = srcp[(width - 1) * 3];
        }
        srcp += stride;
        dstp += stride;
    }
}


static void
shift_v(const int shift, const short* srcp, short* dstp, const int width,
        const int height, const int stride) noexcept
{
    int offset_s, offset_d, c0, c1, first;
    set_params(shift, offset_s, offset_d, c0, c1, first);
    const short* above = srcp + first * stride;

    int y = 0;
    for (; y < offset_d; ++y) {
        for (int x = 0; x < width; ++x) {
            dstp[3 * x] = srcp[x];
        }
        dstp += stride;
        above += stride;
    }

    const short* bellow = above + stride;
    for (const int last = height - offset_s; y < last; ++y) {
        for (int x = 0; x < width; ++x) {
            dstp[3 * x] = (above[x] * c0 + bellow[x] * c1) / 4;
        }
        dstp += stride;
        above += stride;
        bellow += stride;
    }

    srcp += (height - 1) * stride;
    for (; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            dstp[3 * x] = srcp[x];
        }
        dstp += stride;
    }
}


static void
proc_chroma(const int h, const int v, short* orig, short* temp,
            const int width, const int height, const int stride) noexcept
{
    if (h == 0) {
        if (v == 0) {
            return;
        } else {
            copy_to_temp(orig, temp, width, height, stride);
            shift_v(v, temp, orig, width, height, stride);
        }
    } else {
        shift_h(h, orig, temp, width, height, stride);
        if (v == 0) {
            copy_to_orig(temp, orig, width, height, stride);
        } else {
            shift_v(v, temp, orig, width, height, stride);
        }
    }
}


static BOOL proc_filter(FILTER* filter, FILTER_PROC_INFO* info) noexcept
{
    const int width = info->w;
    const int height = info->h;
    const int stride = info->max_w * 3;
    const int* shift = filter->track;

    short* orig_cb = reinterpret_cast<short*>(info->ycp_edit) + 1;
    short* orig_cr = orig_cb + 1;
    short* temp_cb = reinterpret_cast<short*>(info->ycp_temp);
    short* temp_cr = temp_cb + ((width + 7) & ~8);

    proc_chroma(shift[0], shift[2], orig_cb, temp_cb, width, height, stride);
    proc_chroma(shift[1], shift[3], orig_cr, temp_cr, width, height, stride);

    return TRUE;
}


static BOOL update_settings(FILTER* filter, int)
{
    static int backup_h = 0;
    static int backup_v = 0;

    if(GetForegroundWindow() != filter->hwnd) {
        return FALSE;
    }

    int* shift = filter->track;

    if (filter->check[0] == 0) {
        backup_h = shift[1];
        backup_v = shift[3];
        return FALSE;
    }

    if (backup_h != shift[0]) {
        shift[1] = shift[0];
    } else if (backup_h != shift[1]) {
        shift[0] = shift[1];
    }
    backup_h = shift[0];

    if(backup_v != shift[2]){
        shift[3] = shift[2];
    }else if(backup_v != shift[3]){
        shift[2] = shift[3];
    }
    backup_v = shift[2];

    filter->exfunc->filter_window_update(filter);

    return TRUE;
}



constexpr int NUM_TRACKS = 4;
constexpr int SHIFT_MAX = 128;
static TCHAR* track_name[] = { "Cb-H", "Cr-H", "Cb-V", "Cr-V" };
static int track_default[] = { 0, 0, 0, 0 };
static int track_min[] = { -SHIFT_MAX, -SHIFT_MAX, -SHIFT_MAX, -SHIFT_MAX };
static int track_max[] = { SHIFT_MAX, SHIFT_MAX, SHIFT_MAX, SHIFT_MAX };

constexpr int NUM_CHECK = 1;
static TCHAR* check_name[] = { "sync" };
static int check_default[] = { 0 };

FILTER_DLL filter = {
    FILTER_FLAG_EX_INFORMATION,
    0, 0,
    "ChromaShift",
    NUM_TRACKS,
    track_name,
    track_default,
    track_min,
    track_max,
    NUM_CHECK,
    check_name,
    check_default,
    proc_filter,
    nullptr,
    nullptr,
    update_settings,
    nullptr,
    nullptr, nullptr,
    nullptr,
    0,
    "Chroma Shift version 0.0.0 by Chikuzen",
    nullptr,
};


extern "C" __declspec(dllexport) FILTER_DLL* __stdcall GetFilterTable(void)
{
    return &filter;
}


