/*
* Copyright (c) 2014-2020, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file     mhw_render_g9_X.h
//! \brief    Defines functions for constructing  render engine commands on Gen9-based platforms
//!

#ifndef __MHW_RENDER_G9_X_H__
#define __MHW_RENDER_G9_X_H__

#include "mhw_render_generic.h"
#include "mhw_render_hwcmd_g9_X.h"
#include "mhw_state_heap_g9.h"

class MhwRenderInterfaceG9 : public MhwRenderInterfaceGeneric<mhw_render_g9_X>
{
public:
    MhwRenderInterfaceG9(
        MhwMiInterface          *miInterface,
        PMOS_INTERFACE          osInterface,
        MEDIA_SYSTEM_INFO       *gtSystemInfo,
        uint8_t                 newStateHeapManagerRequested) :
        MhwRenderInterfaceGeneric(miInterface, osInterface, gtSystemInfo, newStateHeapManagerRequested)
    {
        MHW_FUNCTION_ENTER;

        if (gtSystemInfo == nullptr)
        {
            MHW_ASSERTMESSAGE("Invalid input pointer provided");
            return;
        }

#if (_DEBUG || _RELEASE_INTERNAL)
        if (gtSystemInfo->EUCount > 24)
        {
            if (m_osInterface->bSimIsActive)
            {
                m_hwCaps.dwMaxThreads = (48 * 7);
            }
        }
#endif

        // SLM     URB     DC      RO     Rest
        // 0      256      0       0      512 (KB chunks based on GT2)
        m_l3CacheCntlRegisterValueDefault = 0x80000040;

        InitMmioRegisters();
    }

    virtual ~MhwRenderInterfaceG9() { MHW_FUNCTION_ENTER; }

    MOS_STATUS AddMediaVfeCmd(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        PMHW_VFE_PARAMS                 params);

    MOS_STATUS AddPipelineSelectCmd(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        bool                            gpGpuPipe);

    MOS_STATUS AddMediaObject(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        PMHW_BATCH_BUFFER               batchBuffer,
        PMHW_MEDIA_OBJECT_PARAMS        params);

    MOS_STATUS AddMediaObjectWalkerCmd(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        PMHW_WALKER_PARAMS              params);

    MOS_STATUS AddPaletteLoadCmd(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        PMHW_PALETTE_PARAMS             params);

    MOS_STATUS AddGpgpuCsrBaseAddrCmd(
        PMOS_COMMAND_BUFFER             cmdBuffer,
        PMOS_RESOURCE                   csrResource);

    MOS_STATUS EnableL3Caching(
        PMHW_RENDER_ENGINE_L3_CACHE_SETTINGS    cacheSettings);

    MOS_STATUS SetL3Cache(
        PMOS_COMMAND_BUFFER             cmdBuffer );

    MHW_RENDER_ENGINE_L3_CACHE_CONFIG* GetL3CacheConfig() { return &m_l3CacheConfig; }

    virtual PMHW_MI_MMIOREGISTERS GetMmioRegisters()
    {
        return &m_mmioRegisters;
    }

    //!
    //! \brief    Get AVS sampler state Inc unit
    //! \details  Get AVS sampler state Inc unit
    //! \return   [out] uint32_t
    //!           AVS sampler unit.
    virtual uint32_t GetSamplerStateAVSIncUnit() { return MHW_SAMPLER_STATE_AVS_INC_G9; }

    //!
    //! \brief    Get Conv sampler state Inc unit
    //! \details  Get Conv sampler state Inc unit
    //! \return   [out] uint32_t
    //!           Conv sampler unit.
    virtual uint32_t GetSamplerStateConvIncUnit() { return MHW_SAMPLER_STATE_CONV_INC_G9; }

    //!
    //! \brief    Get the sampler height and width align unit
    //! \details  NV12 format needs the width and height to be a multiple of some unit
    //! \param    [in] bool
    //!           true if AVS sampler, false otherwise
    //! \param    [in, out] uint32_t
    //!           weight align unit
    //! \param    [in, out] uint32_t
    //!           height align unit
    virtual void GetSamplerResolutionAlignUnit(bool isAVSSampler, uint32_t &widthAlignUnit, uint32_t &heightAlignUnit)
    {
        // NV12 format width need to be a multiple of 2, while height need be
        // a multiple of 4. Since already post PV, just keep the old logic to
        // enable 2 plane NV12 when the width or Height is not a multiple of 4.
        widthAlignUnit  = MHW_SAMPLER_WIDTH_ALIGN_UNIT_G9;
        heightAlignUnit = MHW_SAMPLER_HEIGHT_ALIGN_UNIT_G9;
    }

private:
    //! \brief Mmio registers address
    MHW_MI_MMIOREGISTERS    m_mmioRegisters = {};
    void InitMmioRegisters();
};

#endif
