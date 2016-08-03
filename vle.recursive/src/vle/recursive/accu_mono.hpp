/*
 * Copyright (C) 2015-2015 INRA
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VLE_RECURSIVE_ACCU_MONO_HPP_
#define VLE_RECURSIVE_ACCU_MONO_HPP_

#include <algorithm>
#include <vector>
#include <math.h>
#include <functional>
#include <numeric>
#include <limits>

#include <vle/utils/Exception.hpp>

namespace vle { namespace recursive {

namespace vu = vle::utils;
/**
 * @brief Different types of accumulators.
 */
enum AccuType
{
    STANDARD, //computes mean, std, var, min, max
    MEAN,     //compute mean only
    QUANTILE,  //compute quantiles
    ORDERED   //keep order: can acces to specific value
};

class AccuMono
{

public:
    AccuMono() :
        accu(STANDARD),  msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(false), mvalues(0)
    {
    }

    AccuMono(AccuType type) :
        accu(type), msum(0), mcount(0), msquareSum(0),
        mmin(std::numeric_limits<double>::max()),
        mmax(std::numeric_limits<double>::min()),
        msorted(false), mvalues(0)
    {
        switch (accu) {
        case STANDARD:
        case MEAN: {
            break;
        } case QUANTILE: {
            mvalues = new std::vector<double>();
            msorted = true;
            break;
        } case ORDERED: {
            mvalues = new std::vector<double>();
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (1)");
            break;
        }
    }

    AccuMono(const AccuMono& acc):
        accu(acc.accu), msum(acc.msum), mcount(acc.mcount),
        msquareSum(acc.msquareSum), mmin(acc.mmin),  mmax(acc.mmax),
        msorted(acc.msorted), mvalues(0)
    {
        if (acc.mvalues) {
            mvalues = new std::vector<double>(acc.mvalues->begin(),
                    acc.mvalues->end());
        }
    }

    ~AccuMono()
    {
        delete mvalues;
    }
    /**
     * @brief Inserts a real into the accumulator
     * @param v the real value
     */
    inline void insert(double v)
    {
        switch (accu) {
        case STANDARD: {
            msum += v;
            mcount++;
            msquareSum += pow(v,2);
            if (v < mmin) {
                mmin = v;
            }
            break;
        } case MEAN: {
            msum += v;
            mcount++;
            break;
        } case QUANTILE: {
            mvalues->push_back(v);
            msorted = false;
            break;
        } case ORDERED: {
            mvalues->push_back(v);
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (1)");
            break;
        }
    }

    /**
     * @brief Mean statistic extractor
     * @return the mean value
     */
    inline double mean() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN: {
            return msum / mcount;
            break;
        } case QUANTILE:
          case ORDERED: {
            double sum = std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            return sum / mvalues->size();
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Moment of order 2 extractor :
     * (sum of squares) divided by count
     * @return the moment of order 2 value
     */

    inline double moment2() const
    {
        switch (accu) {
        case STANDARD: {
            return msquareSum / mcount;
            break;
        } case MEAN: {
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        } default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Deviation Square sum extractor (requires rewriting) :
     * let N = count ; X = mean ; x_i the ieme value inserted.
     * sum_i [(x_i - X) ^ 2] = N * X^2 + sum_i (x_i ^ 2) - 2X sum_i(x_i)
     * @return the deviation square sum
     */
    inline double deviationSquareSum() const
    {
        switch (accu) {
        case STANDARD: {
            double mean = msum / mcount;
            return mcount * pow(mean,2) + msquareSum - 2 * mean * msum;
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Corrected standard deviation,
     * @return the stdDeviation
     */
    inline double stdDeviation() const
    {
        switch (accu) {
        case STANDARD: {
            if (mcount < 2) {
                return 0;
            } else {
                return sqrt(deviationSquareSum()/(mcount-1));
            }
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Corrected variance,
     * @return the variance
     */
    inline double variance() const
    {
        switch (accu) {
        case STANDARD: {
            if (mcount < 2) {
                return 0;
            } else {
                return deviationSquareSum()/(mcount-1);
            }
            break;
        } case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (2)");
            return 0;
            break;
        }
    }
    /**
     * @brief Square sum extractor
     * @return square sum value
     */
    inline double squareSum() const
    {
        switch (accu) {
        case STANDARD:
            return msquareSum;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE:
        case ORDERED: {
            double squareSum = 0;
            std::vector<double>::const_iterator itb = mvalues->begin();
            std::vector<double>::const_iterator ite = mvalues->end();
            for (; itb != ite; itb++) {
                squareSum += pow(*itb,2);
            }
            return squareSum;
        }}
        return 0;
    }
    /**
     * @brief Count extractor
     * @return the count value
     */
    inline unsigned int count() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return mcount;
            break;
        case QUANTILE:
        case ORDERED:
            return mvalues->size();
            break;
        }
        return 0;

    }
    /**
     * @brief Sum extractor
     * @return the sum value
     */
    inline double sum() const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            return msum;
            break;
        case QUANTILE:
        case ORDERED:
            return std::accumulate(mvalues->begin(),mvalues->end(),0.0);
            break;
        }
        return 0;
    }
    /**
     * @brief Minimal value extractor
     * @return the minimal value
     */
    inline double min() const
    {
        switch (accu) {
        case STANDARD:
            return mmin;
            break;
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        default:
            throw vle::utils::ArgError(" [accu_mono] not yet implemented (5)");
            return 0;
            break;
        }
    }

    /**
     * @brief acces to a specific value
     * @param i, the index of value
     * @return the minimal value
     */
    inline double at(unsigned int i) const
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
        case QUANTILE:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case ORDERED:
            return mvalues->at(i);
            break;
        }
        return 0;
    }

    /**
     * @brief Quantile value extractor (weigthed average method)
     * @param quantileOrder the quantile order
     * @returns the quantile value of order quantileOrder
     */
    inline double quantile(double quantileOrder)
    {
        switch (accu) {
        case STANDARD:
        case MEAN:
            throw vle::utils::ArgError(" [accu_mono] not available");
            break;
        case QUANTILE: {
            if (!msorted) {
                std::sort(mvalues->begin(),mvalues->end());
                msorted = true;
            }
            return quantileOnSortedVect(*mvalues, quantileOrder);
            break;
        } case ORDERED: {
            std::vector<double> vals;
            vals.insert(vals.begin(), mvalues->begin(),mvalues->end());
            return quantileOnSortedVect(vals, quantileOrder);
            break;
        }}
        return 0;
    }


    void clear()
    {
        msum = 0;
        mcount = 0;
        msquareSum = 0;
        mmin = std::numeric_limits<double>::max();
        mmax = std::numeric_limits<double>::min();
        msorted = false;
        delete mvalues;
        mvalues = 0;
    }

protected:

    double quantileOnSortedVect(const std::vector<double>& vals, double qOrder)
    {
        double quantile = 0;
        if (qOrder == 1.0) {
            quantile = vals[vals.size() - 1];
        } else {
            double rang_quantile = (vals.size() - 1) * qOrder;
            int ent_rang = floor(rang_quantile);
            double frac_rang = rang_quantile - ent_rang;
            quantile = vals[ent_rang] + frac_rang *
                    (vals[ent_rang + 1] - vals[ent_rang]);
        }
        return quantile;
    }

    AccuType accu;
    double msum;
    unsigned int mcount;
    double msquareSum;
    double mmin;
    double mmax;
    bool msorted;
    std::vector<double>* mvalues;

};

}} //namespaces

#endif
