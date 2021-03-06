/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2009 Roland Lichters
 Copyright (C) 2009 Ferdinando Ametrano
 Copyright (C) 2017 Joseph Jeisman
 Copyright (C) 2017 Fabrice Lecuyer

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

/*! \file overnightindexedswap.hpp
    \brief Overnight index swap paying compounded overnight vs. fixed
*/

#ifndef quantlib_overnight_indexed_swap_hpp
#define quantlib_overnight_indexed_swap_hpp

#include <ql/instruments/swap.hpp>
#include <ql/time/daycounter.hpp>
#include <ql/time/businessdayconvention.hpp>
#include <ql/time/calendar.hpp>

namespace QuantLib {

    class Schedule;
    class OvernightIndex;

    //! Overnight indexed swap: fix vs compounded overnight rate
    class OvernightIndexedSwap : public Swap {
    public:
        enum Type { Receiver = -1, Payer = 1 };
        OvernightIndexedSwap(
                        Type type,
                        Real nominal,
                        const Schedule& schedule,
                        Rate fixedRate,
                        const DayCounter& fixedDC,
                        const boost::shared_ptr<OvernightIndex>& overnightIndex,
                        Spread spread = 0.0,
                        Natural paymentLag = 0,
                        BusinessDayConvention paymentAdjustment = Following,
                        Calendar paymentCalendar = Calendar(),
                        bool telescopicValueDates = false);

        OvernightIndexedSwap(
                        Type type,
                        std::vector<Real> nominals,
                        const Schedule& schedule,
                        Rate fixedRate,
                        const DayCounter& fixedDC,
                        const boost::shared_ptr<OvernightIndex>& overnightIndex,
                        Spread spread = 0.0,
                        Natural paymentLag = 0,
                        BusinessDayConvention paymentAdjustment = Following,
                        Calendar paymentCalendar = Calendar(),
                        bool telescopicValueDates = false);

        //! \name Inspectors
        //@{
        Type type() const { return type_; }
        Real nominal() const;
        std::vector<Real> nominals() const { return nominals_; }

        //const Schedule& schedule() { return schedule_; }
        Frequency paymentFrequency() { return paymentFrequency_; }

        Rate fixedRate() const { return fixedRate_; }
        const DayCounter& fixedDayCount() { return fixedDC_; }

        const boost::shared_ptr<OvernightIndex>& overnightIndex();
        Spread spread() { return spread_; }

        const Leg& fixedLeg() const { return legs_[0]; }
        const Leg& overnightLeg() const { return legs_[1]; }
        //@}

        //! \name Results
        //@{
        Real fixedLegBPS() const;
        Real fixedLegNPV() const;
        Real fairRate() const;

        Real overnightLegBPS() const;
        Real overnightLegNPV() const;
        Spread fairSpread() const;
        //@}
      private:
        void initialize(const Schedule& schedule);
        Type type_;
        std::vector<Real> nominals_;

        Frequency paymentFrequency_;
        Calendar paymentCalendar_;
        BusinessDayConvention paymentAdjustment_;
        Natural paymentLag_;

        //Schedule schedule_;

        Rate fixedRate_;
        DayCounter fixedDC_;

        boost::shared_ptr<OvernightIndex> overnightIndex_;
        Spread spread_;
        bool telescopicValueDates_;
    };


    // inline

    inline Real OvernightIndexedSwap::nominal() const {
        QL_REQUIRE(nominals_.size()==1, "varying nominals");
        return nominals_[0];
    }

}

/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2009 Roland Lichters
 Copyright (C) 2009 Ferdinando Ametrano
 Copyright (C) 2017 Joseph Jeisman
 Copyright (C) 2017 Fabrice Lecuyer

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

#include <ql/cashflows/overnightindexedcoupon.hpp>
#include <ql/cashflows/fixedratecoupon.hpp>

namespace QuantLib {

  inline OvernightIndexedSwap::OvernightIndexedSwap(
                    Type type,
                    Real nominal,
                    const Schedule& schedule,
                    Rate fixedRate,
                    const DayCounter& fixedDC,
                    const boost::shared_ptr<OvernightIndex>& overnightIndex,
                    Spread spread,
                    Natural paymentLag,
                    BusinessDayConvention paymentAdjustment,
                    Calendar paymentCalendar,
                    bool telescopicValueDates)
    : Swap(2), type_(type),
      nominals_(std::vector<Real>(1, nominal)),
      paymentFrequency_(schedule.tenor().frequency()),
      paymentCalendar_(paymentCalendar.empty() ? schedule.calendar() : paymentCalendar),
      paymentAdjustment_(paymentAdjustment), paymentLag_(paymentLag),
      fixedRate_(fixedRate), fixedDC_(fixedDC),
      overnightIndex_(overnightIndex), spread_(spread),
      telescopicValueDates_(telescopicValueDates) {

          initialize(schedule);

    }

  inline OvernightIndexedSwap::OvernightIndexedSwap(
                    Type type,
                    std::vector<Real> nominals,
                    const Schedule& schedule,
                    Rate fixedRate,
                    const DayCounter& fixedDC,
                    const boost::shared_ptr<OvernightIndex>& overnightIndex,
                    Spread spread,
                    Natural paymentLag,
                    BusinessDayConvention paymentAdjustment,
                    Calendar paymentCalendar,
                    bool telescopicValueDates)
    : Swap(2), type_(type), nominals_(nominals),
      paymentFrequency_(schedule.tenor().frequency()),
      paymentCalendar_(paymentCalendar.empty() ? schedule.calendar() : paymentCalendar),
      paymentAdjustment_(paymentAdjustment), paymentLag_(paymentLag),
      fixedRate_(fixedRate), fixedDC_(fixedDC),
      overnightIndex_(overnightIndex), spread_(spread),
      telescopicValueDates_(telescopicValueDates) {

          initialize(schedule);

    }

  inline void OvernightIndexedSwap::initialize(const Schedule& schedule) {
        if (fixedDC_==DayCounter())
            fixedDC_ = overnightIndex_->dayCounter();
        legs_[0] = FixedRateLeg(schedule)
            .withNotionals(nominals_)
            .withCouponRates(fixedRate_, fixedDC_)
            .withPaymentLag(paymentLag_)
            .withPaymentAdjustment(paymentAdjustment_)
            .withPaymentCalendar(paymentCalendar_);

		legs_[1] = OvernightLeg(schedule, overnightIndex_)
            .withNotionals(nominals_)
            .withSpreads(spread_)
            .withTelescopicValueDates(telescopicValueDates_)
            .withPaymentLag(paymentLag_)
            .withPaymentAdjustment(paymentAdjustment_)
            .withPaymentCalendar(paymentCalendar_);

        for (Size j=0; j<2; ++j) {
            for (Leg::iterator i = legs_[j].begin(); i!= legs_[j].end(); ++i)
                registerWith(*i);
        }

        switch (type_) {
          case Payer:
            payer_[0] = -1.0;
            payer_[1] = +1.0;
            break;
          case Receiver:
            payer_[0] = +1.0;
            payer_[1] = -1.0;
            break;
          default:
            QL_FAIL("Unknown overnight-swap type");
        }
    }

  inline Real OvernightIndexedSwap::fairRate() const {
        static Spread basisPoint = 1.0e-4;
        calculate();
        return fixedRate_ - NPV_/(fixedLegBPS()/basisPoint);
    }

  inline Spread OvernightIndexedSwap::fairSpread() const {
        static Spread basisPoint = 1.0e-4;
        calculate();
        return spread_ - NPV_/(overnightLegBPS()/basisPoint);
    }

  inline Real OvernightIndexedSwap::fixedLegBPS() const {
        calculate();
        QL_REQUIRE(legBPS_[0] != Null<Real>(), "result not available");
        return legBPS_[0];
    }

  inline Real OvernightIndexedSwap::overnightLegBPS() const {
        calculate();
        QL_REQUIRE(legBPS_[1] != Null<Real>(), "result not available");
        return legBPS_[1];
    }

  inline Real OvernightIndexedSwap::fixedLegNPV() const {
        calculate();
        QL_REQUIRE(legNPV_[0] != Null<Real>(), "result not available");
        return legNPV_[0];
    }

  inline Real OvernightIndexedSwap::overnightLegNPV() const {
        calculate();
        QL_REQUIRE(legNPV_[1] != Null<Real>(), "result not available");
        return legNPV_[1];
    }

}


#endif
