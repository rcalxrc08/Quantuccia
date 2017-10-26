/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2010 Hachemi Benyahia
 Copyright (C) 2010 DeriveXperts SAS

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

/*! \file alimikhailhaqcopula.hpp
    \brief Ali-Mikhail-Haq copula
*/

#ifndef quantlib_math_ali_mikhail_haq_copula_hpp
#define quantlib_math_ali_mikhail_haq_copula_hpp

#include <ql/types.hpp>
#include <functional>

namespace QuantLib {

    //! Ali-Mikhail-Haq copula
    class AliMikhailHaqCopula : public std::binary_function<Real,Real,Real> {
      public:
        AliMikhailHaqCopula(Real theta);
        Real operator()(Real x, Real y) const;
      private:
        Real theta_;
    };

}


/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2010 Hachemi Benyahia
 Copyright (C) 2010 DeriveXperts SAS

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

#include <ql/errors.hpp>

namespace QuantLib {

    inline AliMikhailHaqCopula::AliMikhailHaqCopula(Real theta) : theta_(theta)
    {
        QL_REQUIRE(theta >= -1.0 && theta <= 1.0 ,
                   "theta (" << theta << ") must be in [-1,1]");
    }

    inline Real AliMikhailHaqCopula::operator()(Real x, Real y) const
    {
        QL_REQUIRE(x >= 0.0 && x <=1.0 ,
                   "1st argument (" << x << ") must be in [0,1]");
        QL_REQUIRE(y >= 0.0 && y <=1.0 ,
                   "2nd argument (" << y << ") must be in [0,1]");
        return (x*y)/(1.0-theta_*(1.0-x)*(1.0-y));
    }

}

#endif
