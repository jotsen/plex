#pragma once

/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "DynamicDll.h"

#ifndef _LINUX
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int8  uint8_t;
typedef __int32          int32_t;
typedef __int16          int16_t;
#endif

#include "libdca/dts.h"

#ifdef LIBDCA_DOUBLE
typedef float convert_t;
#else
typedef sample_t convert_t;
#endif


class DllLibDtsInterface
{
public:
  virtual ~DllLibDtsInterface() {} 
  virtual dts_state_t * dts_init (uint32_t mm_accel)=0;
  virtual int dts_syncinfo (dts_state_t *state, uint8_t * buf, int * flags, int * sample_rate, int * bit_rate, int *frame_length)=0;
  virtual int dts_frame (dts_state_t * state, uint8_t * buf, int * flags, level_t * level, sample_t bias)=0;
  virtual void dts_dynrng (dts_state_t * state, level_t (* call) (level_t, void *), void * data)=0;
  virtual int dts_blocks_num (dts_state_t * state)=0;
  virtual int dts_block (dts_state_t * state)=0;
  virtual sample_t * dts_samples (dts_state_t * state)=0;
  virtual void dts_free (dts_state_t * state)=0;
};

class DllLibDts : public DllDynamic, DllLibDtsInterface
{
#ifdef __APPLE__
  DECLARE_DLL_WRAPPER(DllLibDts, Q:\\system\\players\\dvdplayer\\libdts-osx.so)
#elif !defined(_LINUX)
  DECLARE_DLL_WRAPPER(DllLibDts, Q:\\system\\players\\dvdplayer\\libdts.dll)
#else
  DECLARE_DLL_WRAPPER(DllLibDts, Q:\\system\\players\\dvdplayer\\libdts-i486-linux.so)
#endif
  DEFINE_METHOD1(dts_state_t *, dts_init, (uint32_t p1))
  DEFINE_METHOD6(int, dts_syncinfo, (dts_state_t *p1, uint8_t * p2, int * p3, int * p4, int * p5, int *p6))
  DEFINE_METHOD5(int, dts_frame, (dts_state_t * p1, uint8_t * p2, int * p3, level_t * p4, sample_t p5))
  DEFINE_METHOD3(void, dts_dynrng, (dts_state_t * p1, level_t (* p2) (level_t, void *), void * p3))
  DEFINE_METHOD1(int, dts_blocks_num ,(dts_state_t * p1))
  DEFINE_METHOD1(int, dts_block, (dts_state_t * p1))
  DEFINE_METHOD1(sample_t *, dts_samples, (dts_state_t * p1))
  DEFINE_METHOD1(void, dts_free, (dts_state_t * p1))
  BEGIN_METHOD_RESOLVE()
    RESOLVE_METHOD(dca_init)
    RESOLVE_METHOD(dca_syncinfo)
    RESOLVE_METHOD(dca_frame )
    RESOLVE_METHOD(dca_dynrng)
    RESOLVE_METHOD(dca_blocks_num)
    RESOLVE_METHOD(dca_block)
    RESOLVE_METHOD(dca_samples)
    RESOLVE_METHOD(dca_free)
  END_METHOD_RESOLVE()
};
