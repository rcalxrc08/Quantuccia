/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Katiuscia Manzoni
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005 StatPro Italia srl

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

/*! \file capfloortermvolcurve.hpp
    \brief Cap/floor at-the-money term-volatility curve
*/

#ifndef quantlib_cap_volatility_vector_hpp
#define quantlib_cap_volatility_vector_hpp

#include <ql/termstructures/volatility/capfloor/capfloortermvolatilitystructure.hpp>
#include <ql/math/interpolation.hpp>
#include <ql/quote.hpp>
#include <ql/patterns/lazyobject.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <boost/noncopyable.hpp>
#include <vector>

namespace QuantLib {

    //! Cap/floor at-the-money term-volatility vector
    /*! This class provides the at-the-money volatility for a given cap/floor
        interpolating a volatility vector whose elements are the market
        volatilities of a set of caps/floors with given length.
    */
    class CapFloorTermVolCurve : public LazyObject,
                                 public CapFloorTermVolatilityStructure,
                                 private boost::noncopyable {
      public:
        //! floating reference date, floating market data
        CapFloorTermVolCurve(Natural settlementDays,
                             const Calendar& calendar,
                             BusinessDayConvention bdc,
                             const std::vector<Period>& optionTenors,
                             const std::vector<Handle<Quote> >& vols,
                             const DayCounter& dc = Actual365Fixed());
        //! fixed reference date, floating market data
        CapFloorTermVolCurve(const Date& settlementDate,
                             const Calendar& calendar,
                             BusinessDayConvention bdc,
                             const std::vector<Period>& optionTenors,
                             const std::vector<Handle<Quote> >& vols,
                             const DayCounter& dc = Actual365Fixed());
        //! fixed reference date, fixed market data
        CapFloorTermVolCurve(const Date& settlementDate,
                             const Calendar& calendar,
                             BusinessDayConvention bdc,
                             const std::vector<Period>& optionTenors,
                             const std::vector<Volatility>& vols,
                             const DayCounter& dc = Actual365Fixed());
        //! floating reference date, fixed market data
        CapFloorTermVolCurve(Natural settlementDays,
                             const Calendar& calendar,
                             BusinessDayConvention bdc,
                             const std::vector<Period>& optionTenors,
                             const std::vector<Volatility>& vols,
                             const DayCounter& dc = Actual365Fixed());
        //! \name TermStructure interface
        //@{
        Date maxDate() const;
        //@}
        //! \name VolatilityTermStructure interface
        //@{
        Real minStrike() const;
        Real maxStrike() const;
        //@}
        //! \name LazyObject interface
        //@{
        void update();
        void performCalculations() const;
        //@}
        //! \name some inspectors
        //@{
        const std::vector<Period>& optionTenors() const;
        const std::vector<Date>& optionDates() const;
        const std::vector<Time>& optionTimes() const;
        //@}
      protected:
        Volatility volatilityImpl(Time length,
                                  Rate) const;
      private:
        void checkInputs() const;
        void initializeOptionDatesAndTimes() const;
        void registerWithMarketData();
        void interpolate();

        Size nOptionTenors_;
        std::vector<Period> optionTenors_;
        mutable std::vector<Date> optionDates_;
        mutable std::vector<Time> optionTimes_;
        Date evaluationDate_;

        std::vector<Handle<Quote> > volHandles_;
        mutable std::vector<Volatility> vols_;

        // make it not mutable if possible
        mutable Interpolation interpolation_;
    };

    // inline definitions

    inline Date CapFloorTermVolCurve::maxDate() const {
        calculate();
        return optionDateFromTenor(optionTenors_.back());
    }

    inline Real CapFloorTermVolCurve::minStrike() const {
        return QL_MIN_REAL;
    }

    inline Real CapFloorTermVolCurve::maxStrike() const {
        return QL_MAX_REAL;
    }

    inline
    Volatility CapFloorTermVolCurve::volatilityImpl(Time t,
                                                    Rate) const {
        calculate();
        return interpolation_(t, true);
    }

    inline
    const std::vector<Period>& CapFloorTermVolCurve::optionTenors() const {
        return optionTenors_;
    }

    inline
    const std::vector<Date>& CapFloorTermVolCurve::optionDates() const {
        // what if quotes are not available?
        calculate();
        return optionDates_;
    }

    inline
    const std::vector<Time>& CapFloorTermVolCurve::optionTimes() const {
        // what if quotes are not available?
        calculate();
        return optionTimes_;
    }

}


/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Ferdinando Ametrano
 Copyright (C) 2007 Katiuscia Manzoni
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005 StatPro Italia srl

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

#include <ql/math/interpolations/cubicinterpolation.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/utilities/dataformatters.hpp>

namespace QuantLib {

    // floating reference date, floating market data
    inline CapFloorTermVolCurve::CapFloorTermVolCurve(
                        Natural settlementDays,
                        const Calendar& calendar,
                        BusinessDayConvention bdc,
                        const std::vector<Period>& optionTenors,
                        const std::vector<Handle<Quote> >& vols,
                        const DayCounter& dc)
    : CapFloorTermVolatilityStructure(settlementDays, calendar, bdc, dc),
      nOptionTenors_(optionTenors.size()),
      optionTenors_(optionTenors),
      optionDates_(nOptionTenors_),
      optionTimes_(nOptionTenors_),
      volHandles_(vols),
      vols_(vols.size()) // do not initialize with nOptionTenors_
    {
        checkInputs();
        initializeOptionDatesAndTimes();
        registerWithMarketData();
        interpolate();
    }

    // fixed reference date, floating market data
    inline CapFloorTermVolCurve::CapFloorTermVolCurve(
                            const Date& settlementDate,
                            const Calendar& calendar,
                            BusinessDayConvention bdc,
                            const std::vector<Period>& optionTenors,
                            const std::vector<Handle<Quote> >& vols,
                            const DayCounter& dayCounter)
    : CapFloorTermVolatilityStructure(settlementDate, calendar, bdc, dayCounter),
      nOptionTenors_(optionTenors.size()),
      optionTenors_(optionTenors),
      optionDates_(nOptionTenors_),
      optionTimes_(nOptionTenors_),
      volHandles_(vols),
      vols_(vols.size()) // do not initialize with nOptionTenors_
    {
        checkInputs();
        initializeOptionDatesAndTimes();
        registerWithMarketData();
        interpolate();
    }

    // fixed reference date, fixed market data
    inline CapFloorTermVolCurve::CapFloorTermVolCurve(
                                const Date& settlementDate,
                                const Calendar& calendar,
                                BusinessDayConvention bdc,
                                const std::vector<Period>& optionTenors,
                                const std::vector<Volatility>& vols,
                                const DayCounter& dayCounter)
    : CapFloorTermVolatilityStructure(settlementDate, calendar, bdc, dayCounter),
      nOptionTenors_(optionTenors.size()),
      optionTenors_(optionTenors),
      optionDates_(nOptionTenors_),
      optionTimes_(nOptionTenors_),
      volHandles_(vols.size()), // do not initialize with nOptionTenors_
      vols_(vols)
    {
        checkInputs();
        initializeOptionDatesAndTimes();
        // fill dummy handles to allow generic handle-based computations later
        for (Size i=0; i<nOptionTenors_; ++i)
            volHandles_[i] = Handle<Quote>(boost::shared_ptr<Quote>(new
                SimpleQuote(vols_[i])));
        interpolate();
    }

    // floating reference date, fixed market data
    inline CapFloorTermVolCurve::CapFloorTermVolCurve(
                                Natural settlementDays,
                                const Calendar& calendar,
                                BusinessDayConvention bdc,
                                const std::vector<Period>& optionTenors,
                                const std::vector<Volatility>& vols,
                                const DayCounter& dayCounter)
    : CapFloorTermVolatilityStructure(settlementDays, calendar, bdc, dayCounter),
      nOptionTenors_(optionTenors.size()),
      optionTenors_(optionTenors),
      optionDates_(nOptionTenors_),
      optionTimes_(nOptionTenors_),
      volHandles_(vols.size()), // do not initialize with nOptionTenors_
      vols_(vols)
    {
        checkInputs();
        initializeOptionDatesAndTimes();
        // fill dummy handles to allow generic handle-based computations later
        for (Size i=0; i<nOptionTenors_; ++i)
            volHandles_[i] = Handle<Quote>(boost::shared_ptr<Quote>(new
                SimpleQuote(vols_[i])));
        interpolate();
    }

    inline void CapFloorTermVolCurve::checkInputs() const
    {
        QL_REQUIRE(!optionTenors_.empty(), "empty option tenor vector");
        QL_REQUIRE(nOptionTenors_==vols_.size(),
                   "mismatch between number of option tenors (" <<
                   nOptionTenors_ << ") and number of volatilities (" <<
                   vols_.size() << ")");
        QL_REQUIRE(optionTenors_[0]>0*Days,
                   "negative first option tenor: " << optionTenors_[0]);
        for (Size i=1; i<nOptionTenors_; ++i)
            QL_REQUIRE(optionTenors_[i]>optionTenors_[i-1],
                       "non increasing option tenor: " << io::ordinal(i) <<
                       " is " << optionTenors_[i-1] << ", " <<
                       io::ordinal(i+1) << " is " << optionTenors_[i]);
    }

    inline void CapFloorTermVolCurve::registerWithMarketData()
    {
        for (Size i=0; i<volHandles_.size(); ++i)
            registerWith(volHandles_[i]);
    }

    inline void CapFloorTermVolCurve::interpolate()
    {
        interpolation_ = CubicInterpolation(
                                    optionTimes_.begin(), optionTimes_.end(),
                                    vols_.begin(),
                                    CubicInterpolation::Spline, false,
                                    CubicInterpolation::SecondDerivative, 0.0,
                                    CubicInterpolation::SecondDerivative, 0.0);
    }

    inline void CapFloorTermVolCurve::update()
    {
        // recalculate dates if necessary...
        if (moving_) {
            Date d = Settings::instance().evaluationDate();
            if (evaluationDate_ != d) {
                evaluationDate_ = d;
                initializeOptionDatesAndTimes();
            }
        }
        CapFloorTermVolatilityStructure::update();
        LazyObject::update();
    }

    inline void CapFloorTermVolCurve::initializeOptionDatesAndTimes() const
    {
        for (Size i=0; i<nOptionTenors_; ++i) {
            optionDates_[i] = optionDateFromTenor(optionTenors_[i]);
            optionTimes_[i] = timeFromReference(optionDates_[i]);
        }
    }

    inline void CapFloorTermVolCurve::performCalculations() const
    {
        // check if date recalculation must be called here

        for (Size i=0; i<vols_.size(); ++i)
            vols_[i] = volHandles_[i]->value();

        interpolation_.update();
    }

}


#endif
