/**
* @file  hwDigitalFilterGen_IIR.h
* @date April 2009
* Copyright (C) 2009-2018 Altair Engineering, Inc.  
* This file is part of the OpenMatrix Language (�OpenMatrix�) software.
* Open Source License Information:
* OpenMatrix is free software. You can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* OpenMatrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.
* You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Commercial License Information: 
* For a copy of the commercial license terms and conditions, contact the Altair Legal Department at Legal@altair.com and in the subject line, use the following wording: Request for Commercial License Terms for OpenMatrix.
* Altair�s dual-license business model allows companies, individuals, and organizations to create proprietary derivative works of OpenMatrix and distribute them - whether embedded or bundled with other software - under a commercial license agreement.
* Use of Altair�s trademarks and logos is subject to Altair's trademark licensing policies.  To request a copy, email Legal@altair.com and in the subject line, enter: Request copy of trademark and logo usage policy.
*/
#ifndef _Signals_hwDigitalFilter_IIR_h
#define _Signals_hwDigitalFilter_IIR_h

#include "hwMathStatus.h"

// forward declarations
class hwDigitalFilter;
class hwFilterSpecs;

//------------------------------------------------------------------------------
//!
//! \class hwDigitalFilterGen_IIR
//! \brief Digital IIR filter generator base class
//!
//------------------------------------------------------------------------------
class hwDigitalFilterGen_IIR
{
public:
    //!
    //! Constructor
    //! \param filterSpecs
    //! \param digitalFilter 
    //!
    hwDigitalFilterGen_IIR(const hwFilterSpecs& filterSpecs, 
                           hwDigitalFilter&     digitalFilter);
    //!
    //! Destructor
    //!
    virtual ~hwDigitalFilterGen_IIR();

    //!
    //! Gets status
    //!
    const hwMathStatus& Status() const { return m_status; }
    //!
    //! Creates low pass filter
    //!
    virtual void CreateLowPassFilter() = 0;
    //!
    //! Creates high pass filter
    //!
    virtual void CreateHighPassFilter() = 0;
    //!
    //! Creates band pass filter
    //!
    virtual void CreateBandPassFilter() = 0;
    //!
    //! Creates band stop filter
    //!
    virtual void CreateBandStopFilter() = 0;

    //!
    //! Scales transfer function coeffients, sets gain factor and normalizes
    //!
    void ScaleCoefs();

protected:
    hwDigitalFilter*     m_pDigitalFilter;
    const hwFilterSpecs* m_pFilterSpecs;
    hwMathStatus         m_status;          //!< Status

};

#endif // _Signals_hwDigitalFilter_IIR_h