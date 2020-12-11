// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// HillClimbing.cpp
//
// Defines classes for the HillClimbing concurrency-optimization algorithm.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"
#include <math.h>

#pragma warning(disable:4389)

namespace Concurrency
{
namespace details
{
    //
    // Initial hill climbing configuration settings. These are the starting points for any hill climbing instance.
    //
    static const unsigned int AlwaysIncrease = 0;            // Test setting for always allocating more resources
    static const unsigned int ControlGain = 1;               // Used to determine the magnitude of moves, in units of (coefficient of variation)/(thread count)
    static const unsigned int MaxControlSettingChange = 0;   // Maximum number of resources that can be changed in one transition (i.e. a capper)
    static const unsigned int MinHistorySize = 3;            // Minimum history size to consider relevant for climbing (used for significance test)
    static const unsigned int MaxHistorySize = 5;            // Maximum history size, after which a climbing move must be recommended
    static const unsigned int WarmupSampleCount = 1;         // How many samples are needed to warm up hill climbing
    static const unsigned int MinCompletionsPerSample = 1;   // Minimum number of completions needed to try hill climbing
    static const unsigned int MaxInvalidCount = 3;           // Maximum number of consecutive invalid samples; minimum recommended after this point
    static const unsigned int MaxHistoryAge = 50;            // Maximum amount of ticks to keep a history from a previous setting

    /// <summary>
    ///     Creates a new instance of hill climbing.
    /// </summary>
    /// <param name="id">
    ///     Scheduler id.
    /// </param>
    /// <param name="numberOfCores">
    ///     Number that represents the maximum resources available on the machine.
    /// </param>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy that controls this hill climbing instance.
    /// </param>
    HillClimbing::HillClimbing(unsigned int id, unsigned int numberOfCores, SchedulerProxy * pSchedulerProxy)
        : m_pSchedulerProxy(pSchedulerProxy)
        , m_currentControlSetting(0)
        , m_lastControlSetting(0)
        , m_id(id)
        , m_sampleCount(0)
        , m_totalSampleCount(0)
        , m_invalidCount(0)
        , m_saveCompleted(0)
        , m_saveIncoming(0)
        , m_nextRandomMoveIsUp(true)
    {
        //
        // Assign default configuration
        //
        m_controlGain = ControlGain * numberOfCores;
        m_maxControlSettingChange = (MaxControlSettingChange != 0) ? MaxControlSettingChange : numberOfCores;
    }

    /// <summary>
    ///     External call passing statistical information to hill climbing. Based on these
    ///     statistics, hill climbing will give a recommendation on the number of resources to be used.
    /// </summary>
    /// <param name="currentControlSetting">
    ///     The control setting used in this period of time.
    /// </param>
    /// <param name="completionRate">
    ///     The number of completed units or work in that period of time.
    /// </param>
    /// <param name="arrivalRate">
    ///     The number of incoming units or work in that period of time.
    /// </param>
    /// <param name="queueLength">
    ///     The total length of the work queue.
    /// </param>
    /// <returns>
    ///     The recommended number of resources to be used.
    /// </returns>
    unsigned int HillClimbing::Update(unsigned int currentControlSetting, unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength)
    {
        HillClimbingStateTransition transition = Undefined;
        int recommendedSetting = 0;

        // If there are no resources devoted to this scheduler proxy then there is
        // no statistical analysis needed.
        if (currentControlSetting == 0)
        {
            return 0;
        }

        //
        // Hill climbing made a recommendation for a number of resources to be used the next time around. However, that
        // does not mean that this recommendation was accepted by the consumer of that information. Thus, first establish
        // the control setting passed in by the consumer so that we can accurately track history information. Also, it is
        // necessary to flush old, stale history information before trying to hill climb.
        //
        m_totalSampleCount++;
        EstablishControlSetting(currentControlSetting);

        //
        // If we had some invalid samples, then carefully modify the actual parameters to this function
        //
        if (m_invalidCount > 0)
        {
            completionRate += m_saveCompleted;
            arrivalRate += m_saveIncoming;
        }

        //
        // If we have long running tasks that are not yet completed, report completions and arrivals for those
        // tasks, effectively chunking them up into sample sized tasks. A long running task scenario is defined as:
        //
        //  a) Number or completed tasks is smaller than number of resources used in the time interval, AND
        //  b) Number of completed tasks is smaller than a length of the queue (resources cannot be invalid)
        //
        if (completionRate < currentControlSetting && completionRate < queueLength)
        {
            arrivalRate += (currentControlSetting - completionRate);
            completionRate = currentControlSetting;
        }

        //
        // Check if reported statistics are within the bounds of a valid sample. A sample is invalid iff:
        // it is not a warmup run AND it is EITHER too short of a measurement OR there were not enough completions.
        //
        if (m_sampleCount >= WarmupSampleCount && MinCompletionsPerSample > completionRate && MinCompletionsPerSample > arrivalRate && queueLength == 0)
        {
            //
            // If this is an invalid sample, save the data
            //
            m_invalidCount++;
            m_saveCompleted = completionRate;
            m_saveIncoming = arrivalRate;

            unsigned int minimumSetting = m_pSchedulerProxy->MinHWThreads();
            unsigned int maximumSetting = m_pSchedulerProxy->DesiredHWThreads();
            (maximumSetting);

            recommendedSetting = (m_invalidCount < MaxInvalidCount) ? m_currentControlSetting : minimumSetting;

            TRACE(CONCRT_TRACE_HILLCLIMBING,
                L"********** Invalid sample!\n Process: %u\n Scheduler: %d\n Invalid count: %d\n Completions: %d\n Arrivals: %d\n Queue length: %d\n Minimum: %d\n Maximum: %d\n Current setting: %d\n Last setting: %d\n -----\n Recommended setting: %d\n**********\n",
                GetCurrentProcessId(),
                m_id,
                m_invalidCount,
                completionRate,
                arrivalRate,
                queueLength,
                minimumSetting,
                maximumSetting,
                m_currentControlSetting,
                m_lastControlSetting,
                recommendedSetting);

            return recommendedSetting;
        }

        unsigned int numberOfSamples = m_invalidCount + 1;

        //
        // Reset the statistics kept for invalid samples and initiate a valid sample
        //
        m_sampleCount++;
        m_saveCompleted = 0;
        m_saveIncoming = 0;
        m_invalidCount = 0;

        // Unless there is a good reason to climb, the current setting (set by EstablishControlSetting) will remain the same.
        recommendedSetting = m_currentControlSetting;

        // Calculate the throughput for this given instance
        double throughput = CalculateThroughput(numberOfSamples, completionRate, arrivalRate, queueLength);

        if (m_sampleCount <= WarmupSampleCount)
        {
            //
            // We're in the "warmup" phase, where we simply bide our time (and initialize our current control setting).
            //
            _CONCRT_ASSERT(m_currentControlSetting != 0);
            m_lastControlSetting = m_currentControlSetting;
            transition = Warmup;
        }
        else
        {
            MeasuredHistory * currentHistory = GetHistory(m_currentControlSetting);
            MeasuredHistory * lastHistory = GetHistory(m_lastControlSetting);

            currentHistory->Add(throughput, m_totalSampleCount);

            if (AlwaysIncrease > 0)
            {
                //
                // We're in the "always increase" diagnostic mode.  Just increase the control setting
                // along the desired slope.
                //
                unsigned int newSetting = (unsigned int) ((AlwaysIncrease / 1000.0) * m_sampleCount);

                if (newSetting > m_currentControlSetting)
                {
                    recommendedSetting = RecommendControlSetting(newSetting);
                    transition = DoClimbing;
                }
                else
                {
                    transition = ContinueLookingForClimb;
                }
            }
            else if (lastHistory->Count() == 0 || currentHistory == lastHistory)
            {
                //
                // If we have no previous history, then we need to initialize.  We wait until
                // the current history is stable, then make our first move.
                //
                if (IsStableHistory(currentHistory))
                {
                    //
                    // This is our first move; we have no history to use to predict the correct move.
                    // We'll just make a random move, and see what happens.
                    //
                    recommendedSetting = RecommendControlSetting(m_currentControlSetting + GetRandomMove());
                    transition = CompletedInitialization;
                }
                else
                {
                    transition = ContinueInitializing;
                }
            }
            else if (!IsStableHistory(currentHistory))
            {
                transition = ContinueLookingForClimb;
            }
            else
            {
                //
                // We have two separate stable histories.  We can compare them, and make a real climbing move.
                //
                double slope = CalculateThroughputSlope(m_lastControlSetting, m_currentControlSetting);
                double controlSettingAdjustment = slope * m_controlGain;
                unsigned int newControlSetting = (unsigned int) (m_currentControlSetting + controlSettingAdjustment);

                if (newControlSetting == m_currentControlSetting)
                {
                    newControlSetting = (unsigned int) (m_currentControlSetting + sign(controlSettingAdjustment));
                }

                recommendedSetting = RecommendControlSetting(newControlSetting);
                transition = DoClimbing;
            }
        }

        _CONCRT_ASSERT(transition != Undefined); // Unhandled case for HillClimbing controller

#if defined(CONCRT_TRACING)
        LogData(recommendedSetting, transition, numberOfSamples, completionRate, arrivalRate, queueLength, throughput);
#endif

        return recommendedSetting;
    }

    /// <summary>
    ///     Calculates the throughput based on the input parameters.
    /// </summary>
    /// <param name="numberOfSamples">
    ///     The number of sample points in this measurement, including invalid ones.
    /// </param>
    /// <param name="completionRate">
    ///     The number of completed units or work in that period of time.
    /// </param>
    /// <param name="arrivalRate">
    ///     The number of incoming units or work in that period of time.
    /// </param>
    /// <param name="queueLength">
    ///     The total length of the work queue.
    /// </param>
    /// <returns>
    ///     The calculated throughput.
    /// </returns>
    double HillClimbing::CalculateThroughput(unsigned int numberOfSamples, unsigned int completionRate, unsigned int, unsigned int)
    {
        const double samplesPerSecond = 10.0;    // A double constant representing number of valid samples per second

        // Compute the rate at which the queue is growing or shrinking. If it is growing, report a higher
        // number which will cause more resources to be allocated for this instance; it the length of the queue
        // is shrinking, try to take away resources while still shrinking the queue.
        //
        //           /_\ length     incoming - completed
        // growth = ------------ = ----------------------
        //            /_\ time            t2 - t1
        //
        // For now, instead of looking at the change in the queue length, completion rate will be used. This is due
        // to the fact that typical loads in ConcRT are self-balancing, i.e. completion rate ~ incoming rate.
        //
        return (completionRate * samplesPerSecond) / (double) (numberOfSamples);
    }

    /// <summary>
    ///     Recommends NewControlSetting to be used.
    /// </summary>
    /// <param name="newControlSetting">
    ///     The control setting to be established.
    /// </param>
    /// <returns>
    ///     New control setting to be used.
    /// </returns>
    unsigned int HillClimbing::RecommendControlSetting(unsigned int newControlSetting)
    {
        //
        // Make sure that the new setting is within the biggest individual move bounds.
        //
        unsigned int minimumSetting = m_pSchedulerProxy->MinHWThreads();
        unsigned int maximumSetting = m_pSchedulerProxy->DesiredHWThreads();

        newControlSetting = min(m_currentControlSetting + m_maxControlSettingChange, newControlSetting);

        if (m_currentControlSetting > m_maxControlSettingChange)
        {
            newControlSetting = max(m_currentControlSetting - m_maxControlSettingChange, newControlSetting);
        }

        if (newControlSetting == m_currentControlSetting)  // Can't draw a line with a single point
        {
            if (newControlSetting > minimumSetting)
            {
                newControlSetting--;
            }
            else
            {
                newControlSetting++;
            }
        }

        //
        // Make sure that the new setting is within the min and max bounds of the scheduler proxy.
        //
        newControlSetting = max(minimumSetting, newControlSetting);
        newControlSetting = min(maximumSetting, newControlSetting);

        if (AlwaysIncrease == 0 && newControlSetting != m_currentControlSetting)
        {
            // If this move would cause us to move through a setting that we know was recently worse than this
            // one, then back off to one before that setting.
            int direction = sign(newControlSetting - m_currentControlSetting);

            if (direction == -1)
            {
                for (int setting = m_currentControlSetting + direction;
                     setting == newControlSetting || sign(newControlSetting - setting) == direction;
                     setting += direction)
                {
                    if (GetHistory(setting)->Count() > 0)
                    {
                        double slope = CalculateThroughputSlope(m_currentControlSetting, setting) * direction;

                        if (slope <= 0)
                        {
                            newControlSetting = setting - direction;
                            break;
                        }
                    }
                }
            }
        }

        return newControlSetting;
    }

    /// <summary>
    ///     Establishes control setting as current. This is the only method that updates the control settings.
    /// </summary>
    /// <param name="newControlSetting">
    ///     The control setting to be established.
    /// </param>
    void HillClimbing::EstablishControlSetting(unsigned int newControlSetting)
    {
        if (newControlSetting != m_currentControlSetting)
        {
            m_lastControlSetting = m_currentControlSetting;
            m_currentControlSetting = newControlSetting;
            GetHistory(m_currentControlSetting)->Clear(0);
            FlushHistories();
        }
    }

    /// <summary>
    ///     Calculates the throughput slope given two history measurements.
    /// </summary>
    /// <param name="fromSetting">
    ///     The control setting to move from.
    /// </param>
    /// <param name="toSetting">
    ///     The control setting to move to.
    /// </param>
    /// <returns>
    ///     A value representing a slope between two measurements.
    /// </returns>
    double HillClimbing::CalculateThroughputSlope(int fromSetting, int toSetting)
    {
        //
        // Configurable constants to control reactiveness of hill climbing
        //
        const double minJustifiesChange = 0.15;         // A minimum fractional change in measurement that justifies a change (cost for making a change)
        const double changeAdjustmentMultiplier = 1.0;  // Controls change factor by reducing uncertainty (bigger number pessimizes change frequency)

        double fractionalChangeInControlSetting = (double) (toSetting - fromSetting) / (double) fromSetting;

        MeasuredHistory * lastHistory = GetHistory(fromSetting);
        MeasuredHistory * currentHistory = GetHistory(toSetting);

        double lastHistoryMean = lastHistory->Mean();
        double currentHistoryMean = currentHistory->Mean();
        double meanChangeInMeasuredValue = currentHistoryMean - lastHistoryMean;
        double fractionalChangeInMeasuredValue = meanChangeInMeasuredValue / lastHistoryMean;

        double slope = (fractionalChangeInMeasuredValue/fractionalChangeInControlSetting) - minJustifiesChange;

        double varianceOfcurrentHistory = currentHistory->VarianceMean();
        double varianceOflastHistory = lastHistory->VarianceMean();
        double standardDeviationOfDifferenceInMeans = sqrt(varianceOfcurrentHistory + varianceOflastHistory);
        double coefficientOfVariationOfChangeInMeasuredValue =
            (abs(meanChangeInMeasuredValue) > 0) ? abs(standardDeviationOfDifferenceInMeans / meanChangeInMeasuredValue) : 0;

        double adjustedSlope = slope * exp(-changeAdjustmentMultiplier * coefficientOfVariationOfChangeInMeasuredValue);

        return adjustedSlope;
    }

    /// <summary>
    ///     Determines whether a given history measurement is stable enough to make a hill climbing move.
    /// </summary>
    /// <returns>
    ///     True if history measurement is stable.
    /// </returns>
    bool HillClimbing::IsStableHistory(MeasuredHistory * pMeasuredHistory)
    {
        const double maxCoefficientOfVariation = 0.004; // Controls history relevance between min and max by bounding the error

        if (pMeasuredHistory->Count() > MaxHistorySize)
        {
            return true;
        }

        if (pMeasuredHistory->Count() < MinHistorySize)
        {
            return false;
        }

        if (abs(pMeasuredHistory->CoefficientOfVariationMean()) > maxCoefficientOfVariation)
        {
            return false;
        }

        return true;
    }

    /// <summary>
    ///     Flushes all measurement histories that are no longer relevant.
    /// </summary>
    void HillClimbing::FlushHistories()
    {
        for (int i = 0; i < MaxHistoryCount; i++)
        {
            if (m_histories[i].ControlSetting() != m_currentControlSetting &&
                m_histories[i].ControlSetting() != m_lastControlSetting &&
                m_totalSampleCount - m_histories[i].LastDataPointCount() > MaxHistoryAge)
            {
                m_histories[i].Clear(0);
            }
        }
    }

    /// <summary>
    ///     Clears all measurement histories.
    /// </summary>
    void HillClimbing::ClearHistories()
    {
        for (int i = 0; i < MaxHistoryCount; i++)
        {
            m_histories[i].Clear(0);
        }
    }

    /// <summary>
    ///     Makes a pseudo-random hill climbing move by alternating between up and down.
    /// </summary>
    /// <returns>
    ///     The random move.
    /// </returns>
    int HillClimbing::GetRandomMove()
    {
        int result = m_nextRandomMoveIsUp ? 1 : 0;
        m_nextRandomMoveIsUp = !m_nextRandomMoveIsUp;
        return result;
    }

    /// <summary>
    ///     Gets the history measurement for a given control setting.
    /// </summary>
    /// <returns>
    ///     The history measurement.
    /// </returns>
    HillClimbing::MeasuredHistory * HillClimbing::GetHistory(unsigned int controlSetting)
    {
        int i = controlSetting % MaxHistoryCount;

        if (m_histories[i].ControlSetting() != controlSetting)
        {
            m_histories[i].Clear(controlSetting);
        }

        return &m_histories[i];
    }

    /// <summary>
    ///     Creates a new measurement history.
    /// </summary>
    HillClimbing::MeasuredHistory::MeasuredHistory()
    {
        Clear(0);
    }

    /// <summary>
    ///     Clears the history values for this control setting.
    /// </summary>
    /// <param name="controlSetting">
    ///     The control setting to reset.
    /// </param>
    void HillClimbing::MeasuredHistory::Clear(unsigned int controlSetting)
    {
        m_count = 0;
        m_sum = 0;
        m_sumOfSquares = 0;
        m_controlSetting = controlSetting;
        m_lastDataPointCount = 0;
    }

    /// <summary>
    ///     Adds a new history data point.
    /// </summary>
    /// <param name="dataValue">
    ///     The value representing throughput in this invocation.
    /// </param>
    /// <param name="totalSampleCount">
    ///     The value representing the total number of samples for this history, including invalid samples and samples for previous settings.
    /// </param>
    void HillClimbing::MeasuredHistory::Add(const double dataValue, unsigned int totalSampleCount)
    {
        m_sum += dataValue;
        m_sumOfSquares += dataValue * dataValue;
        m_count++;
        m_lastDataPointCount = totalSampleCount;
    }

    /// <summary>
    ///     Gets the count for this history measurement.
    /// </summary>
    /// <returns>
    ///     The count.
    /// </returns>
    int HillClimbing::MeasuredHistory::Count()
    {
        return m_count;
    }

    /// <summary>
    ///     Gets the count at the last data point for this history measurement.
    /// </summary>
    /// <returns>
    ///     The last data point count.
    /// </returns>
    unsigned int HillClimbing::MeasuredHistory::LastDataPointCount()
    {
        return m_lastDataPointCount;
    }

    /// <summary>
    ///     Gets the control setting for this history measurement.
    /// </summary>
    /// <returns>
    ///     The control setting.
    /// </returns>
    int HillClimbing::MeasuredHistory::ControlSetting() {
        return m_controlSetting;
    }

    /// <summary>
    ///     Computes the mean for a given history.
    /// </summary>
    /// <returns>
    ///     The mean.
    /// </returns>
    double HillClimbing::MeasuredHistory::Mean()
    {
        return (m_count == 0) ? 0.0 : (m_sum / m_count);
    }

    /// <summary>
    ///     Computes the coefficient of variation for a given history.
    /// </summary>
    /// <returns>
    ///     The coefficient of variation.
    /// </returns>
    double HillClimbing::MeasuredHistory::CoefficientOfVariation()
    {
        double mean = Mean();
        return (mean <= 0.0) ? 0.0 : (StandardDeviation() / mean);
    }

    /// <summary>
    ///     Computes the mean of coefficients of variation for a given history.
    /// </summary>
    /// <returns>
    ///     The mean of coefficients of variation.
    /// </returns>
    double HillClimbing::MeasuredHistory::CoefficientOfVariationMean()
    {
        return (StandardDeviation() / sqrt(1.0 * m_count)) / Mean();
    }

    /// <summary>
    ///     Computes the variance for a given history.
    /// </summary>
    /// <returns>
    ///     The variance.
    /// </returns>
    double HillClimbing::MeasuredHistory::Variance()
    {
        const double smallValue = 0.0001;
        double variance = 0.0;

        if (m_count >= 2)
        {
            variance = (m_sumOfSquares - (m_sum * m_sum)/ m_count)/ (m_count - 1);
        }

        return abs(variance) > smallValue ? variance : 0;
    }

    /// <summary>
    ///     Computes the mean of variances for a given history.
    /// </summary>
    /// <returns>
    ///     The mean of variances.
    /// </returns>
    double HillClimbing::MeasuredHistory::VarianceMean()
    {
        return Variance() / Count();
    }

    /// <summary>
    ///     Computes the standard deviation for a given history.
    /// </summary>
    /// <returns>
    ///     The standard deviation.
    /// </returns>
    double HillClimbing::MeasuredHistory::StandardDeviation()
    {
        return sqrt(Variance());
    }

    /// <summary>
    ///     Computes the mean of standard deviations for a given history.
    /// </summary>
    /// <returns>
    ///     The mean of standard deviations.
    /// </returns>
    double HillClimbing::MeasuredHistory::StandardDeviationMean()
    {
        return (m_count == 0) ? 0.0 : (StandardDeviation() / sqrt(m_count * 1.0));
    }

    /// <summary>
    ///     Tests if the difference between two measurement histories is statistically significant to
    ///     make a hill climbing decision.
    /// </summary>
    /// <remarks>
    ///     A two sided test is used.
    /// </remarks>
    /// <param name="value">
    ///     The value representing the second history.
    /// </param>
    /// <param name="significanceLevel">
    ///     The significance level in percent. Accepts 1 through 10.
    /// </param>
    /// <param name="totalSampleCount">
    ///     The value representing the total number of samples for this history, including invalid samples and samples for previous settings.
    /// </param>
    /// <returns>
    ///     -1 - second history is larger than this history
    ///      0 - statistically identical
    ///      1 - this history is larger than second history
    /// </returns>
    int HillClimbing::MeasuredHistory::SignificanceTest(double value, const int significanceLevel, unsigned int totalSampleCount)
    {
        MeasuredHistory singleValue;
        singleValue.Add(value, totalSampleCount);

        return MeasuredHistory::SignificanceTest(&singleValue, significanceLevel);
    }

    /// <summary>
    ///     Tests if the difference between two measurement histories is statistically significant to
    ///     make a hill climbing decision.
    /// </summary>
    /// <remarks>
    ///     A two sided test is used.
    /// </remarks>
    /// <param name="pMeasuredHistory">
    ///     The pointer to second measurement history.
    /// </param>
    /// <param name="significanceLevel">
    ///     The significance level in percent. Accepts 1 through 10.
    /// </param>
    /// <returns>
    ///     -1 - second history is larger than this history
    ///      0 - statistically identical
    ///      1 - this history is larger than second history
    /// </returns>
    int HillClimbing::MeasuredHistory::SignificanceTest(MeasuredHistory * pMeasuredHistory, const int significanceLevel)
    {
        const int critSize = 10;
        double critArray[critSize] = { 2.576, 2.3263, 2.17, 2.05, 1.96, 1.88, 1.81, 1.75, 1.70, 1.64 };

        double thisVariance = this->VarianceMean();
        double thisMean = Mean();
        double secondVariance = pMeasuredHistory->VarianceMean();
        double secondMean = pMeasuredHistory->Mean();

        _CONCRT_ASSERT(significanceLevel > 0 && significanceLevel <= 10); // Invalid significance level

        int result = (int) sign(thisMean - secondMean);

        if (thisVariance > 0 && secondVariance > 0)
        {
            double pooledVar = thisVariance / Count() + secondVariance / pMeasuredHistory->Count();

            double testStatistic = (thisMean - secondMean) / sqrt(pooledVar);
            double critVal = critArray[significanceLevel-1];
            double absVal = abs(testStatistic);

            if (absVal < critVal)
            {
                result = 0;
            }
        }

        return result;
    }

#if defined(CONCRT_TRACING)

    // Logging mechanism

    struct HillClimbingLogEntry
    {
        long sampleCount;
        unsigned int currentTotalSampleCount;
        double throughput;
        double currentHistoryMean;
        double currentHistoryStd;
        double lastHistoryMean;
        double lastHistoryStd;
        unsigned int currentControlSetting;
        unsigned int lastControlSetting;
        unsigned int currentHistoryCount;
        unsigned int lastHistoryCount;
        HillClimbingStateTransition transition;
    };

    static const int HillClimbingLogCapacity = 100;
    static HillClimbingLogEntry HillClimbingLog[HillClimbingLogCapacity];
    static int HillClimbingLogFirstIndex = 0;
    static int HillClimbingLogSize = 0;

    static const wchar_t * const HillClimbingTransitionNames[] =
    {
        L"Warmup",
        L"ContinueInitializing",
        L"CompletedInitialization",
        L"DoClimbing",
        L"ChangePoint",
        L"ContinueLookingForClimb",
        L"Undefined"
    };

    /// <summary>
    ///     Logs the hill climbing decision.
    /// </summary>
    /// <param name="recommendedSetting">
    ///     The control setting to be established.
    /// </param>
    /// <param name="transition">
    ///     The transition that is recommended by hill climbing.
    /// </param>
    /// <param name="numberOfSamples">
    ///     The number of sample points in this measurement, including invalid ones.
    /// </param>
    /// <param name="completionRate">
    ///     The number of completed units or work in that period of time.
    /// </param>
    /// <param name="arrivalRate">
    ///     The number of incoming units or work in that period of time.
    /// </param>
    /// <param name="queueLength">
    ///     The total length of the work queue.
    /// </param>
    /// <param name="throughput">
    ///     The throughput of the given instance.
    /// </param>
    void HillClimbing::LogData(unsigned int recommendedSetting, HillClimbingStateTransition transition, unsigned int numberOfSamples,
        unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength, double throughput)
    {
        //
        // First, log to memory so we can see it in the debugger
        //
        int index = (HillClimbingLogFirstIndex + HillClimbingLogSize) % HillClimbingLogCapacity;
        if (HillClimbingLogSize == HillClimbingLogCapacity)
        {
            HillClimbingLogFirstIndex = (HillClimbingLogFirstIndex + 1) % HillClimbingLogCapacity;
            HillClimbingLogSize--; //hide this slot while we update it
        }

        HillClimbingLogEntry * entry = &HillClimbingLog[index];
        unsigned int minimumSetting = m_pSchedulerProxy->MinHWThreads();
        unsigned int maximumSetting = m_pSchedulerProxy->DesiredHWThreads();

        entry->sampleCount = m_sampleCount;
        entry->currentTotalSampleCount = numberOfSamples;
        entry->throughput = throughput;
        entry->transition = transition;
        entry->currentControlSetting = m_currentControlSetting;
        entry->lastControlSetting = m_lastControlSetting;

        MeasuredHistory * currentHistory = GetHistory(m_currentControlSetting);
        entry->currentHistoryCount = currentHistory->Count();
        entry->currentHistoryMean = currentHistory->Mean();
        entry->currentHistoryStd = currentHistory->StandardDeviation();

        MeasuredHistory * lastHistory = GetHistory(m_lastControlSetting);
        entry->lastHistoryCount = lastHistory->Count();
        entry->lastHistoryMean = lastHistory->Mean();
        entry->lastHistoryStd = lastHistory->StandardDeviation();

        HillClimbingLogSize++;

        const int bufferSize = 180;
        const wchar_t * delim = L"*******************************************************";

        wchar_t dateBuffer[bufferSize];
        SYSTEMTIME time;
        GetLocalTime(&time);
        int dateLen = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &time, NULL, dateBuffer, bufferSize);
        dateBuffer[dateLen-1] = L' ';
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER, &time, NULL, dateBuffer + dateLen, bufferSize - dateLen);

        TRACE(CONCRT_TRACE_HILLCLIMBING, L"%ls\n Process: %u\n Scheduler: %d\n Date: %ls\n Number of samples: %d\n Number of samples in this measurement (including invalid): %d\n Completions: %d\n Arrivals: %d\n Queue length: %d\n Throughput: %.4f\n Transition: %ls\n Next random move: %ls\n Minimum: %d\n Maximum: %d\n Current setting: %d\n  * count: %d mean: %g dev: %g varm: %g\n Last setting: %d\n  * count: %d mean: %g dev: %g varm: %g\n -----\n Recommended setting: %d\n%ls\n",
            delim,
            GetCurrentProcessId(),
            m_id,
            dateBuffer,
            m_sampleCount,
            numberOfSamples,
            completionRate,
            arrivalRate,
            queueLength,
            throughput,
            HillClimbingTransitionNames[transition],
            m_nextRandomMoveIsUp ? L"Up" : L"Down",
            minimumSetting,
            maximumSetting,
            m_currentControlSetting,
            currentHistory->Count(),
            currentHistory->Mean(),
            currentHistory->StandardDeviation(),
            currentHistory->CoefficientOfVariationMean(),
            m_lastControlSetting,
            lastHistory->Count(),
            lastHistory->Mean(),
            lastHistory->StandardDeviation(),
            lastHistory->CoefficientOfVariationMean(),
            recommendedSetting,
            delim);
    }
#endif
} // namespace details
} // namespace Concurrency
