/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Mark Joshi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file latticersg.hpp
    \brief lattice rule code for low discrepancy numbers
*/

#ifndef quantlib_lattice_rsg_hpp
#define quantlib_lattice_rsg_hpp

#include <ql/methods/montecarlo/sample.hpp>
#include <vector>

namespace QuantLib {

   
    class LatticeRsg 
    {
      public:
        typedef Sample<std::vector<Real> > sample_type;
         LatticeRsg(Size dimensionality,
             const std::vector<Real>& z,
             Size N);
        /*! skip to the n-th sample in the low-discrepancy sequence */
        void skipTo(unsigned long n);
        const LatticeRsg::sample_type& nextSequence();     
        Size dimension() const { return dimensionality_; }
        const sample_type& lastSequence() const { return sequence_; }

      private:
        Size dimensionality_;
        Size N_;
        Size i_;
        std::vector<Real> z_;
        
        sample_type sequence_;
    };

}

/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
Copyright (C) 2007 Mark Joshi

This file is part of QuantLib, a free-software/open-source library
for financial quantitative analysts and developers - http://quantlib.org/

QuantLib is free software: you can redistribute it and/or modify it
under the terms of the QuantLib license.  You should have received a
copy of the license along with this program; if not, please email
<quantlib-dev@lists.sf.net>. The license is also available online at
<http://quantlib.org/license.shtml>.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file latticersg.cpp
\brief lattice rule code for low discrepancy numbers
*/

namespace QuantLib 
{

    inline LatticeRsg::LatticeRsg(Size dimensionality,
        const std::vector<Real>& z,
        Size N)             
        :
    dimensionality_(dimensionality),
        N_(N),
        i_(0),
        z_(z),
        sequence_(std::vector<Real> (dimensionality), 1.0)
    {
    }
    /*! skip to the n-th sample in the low-discrepancy sequence */
    inline void LatticeRsg::skipTo(unsigned long n)
    {
        i_+=n;
    }

    inline const LatticeRsg::sample_type& LatticeRsg::nextSequence()
    {
        for (Size j=0; j < dimensionality_; ++j)
        {
            Real theta = i_*z_[j]/N_;
            sequence_.value[j]= std::fmod(theta,1.0);
        }
        ++i_;

        return sequence_;
    
    }

}


#endif