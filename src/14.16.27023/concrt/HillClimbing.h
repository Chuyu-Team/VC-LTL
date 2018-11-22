// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// HillClimbing.h
//
// Defines classes for the HillClimbing concurrency-optimization algorithm.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     An enum representing all possible hill climbing transition moves.
    /// </summary>
    enum HillClimbingStateTransition
    {
        Warmup,
        ContinueInitializing,
        CompletedInitialization,
        DoClimbing,
        ChangePoint,
        ContinueLookingForClimb,
        Undefined,
    };

    /// <summary>
    ///     A class responsible for hill climbing.
    /// </summary>
    class HillClimbing
    {
    public:

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
        HillClimbing(unsigned int id, unsigned int numberOfCores, SchedulerProxy * pSchedulerProxy);

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
        unsigned int Update(unsigned int currentControlSetting, unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength);

    private:

        /// <summary>
        ///     A class responsible for keeping hill climbing history measurements.
        /// </summary>
        class MeasuredHistory
        {
        public:

            /// <summary>
            ///     Creates a new measurement history.
            /// </summary>
            MeasuredHistory();

            /// <summary>
            ///     Adds a new history data point.
            /// </summary>
            /// <param name="dataValue">
            ///     The value representing throughput in this invocation.
            /// </param>
            /// <param name="totalSampleCount">
            ///     The value representing the total number of samples for this history, including invalid samples and samples for previous settings.
            /// </param>
            void Add(const double data, unsigned int totalSampleCount);

            /// <summary>
            ///     Clears the history values for this control setting.
            /// </summary>
            /// <param name="controlSetting">
            ///     The control setting to reset.
            /// </param>
            void Clear(unsigned int controlSetting);

            /// <summary>
            ///     Gets the count for this history measurement.
            /// </summary>
            /// <returns>
            ///     The count.
            /// </returns>
            int Count();

            /// <summary>
            ///     Gets the count at the last data point for this history measurement.
            /// </summary>
            /// <returns>
            ///     The last data point count.
            /// </returns>
            unsigned int LastDataPointCount();

            /// <summary>
            ///     Gets the control setting for this history measurement.
            /// </summary>
            /// <returns>
            ///     The control setting.
            /// </returns>
            int ControlSetting();

            /// <summary>
            ///     Computes the mean for a given history.
            /// </summary>
            /// <returns>
            ///     The mean.
            /// </returns>
            double Mean();

            /// <summary>
            ///     Computes the coefficient of variation for a given history.
            /// </summary>
            /// <returns>
            ///     The coefficient of variation.
            /// </returns>
            double CoefficientOfVariation();

            /// <summary>
            ///     Computes the mean of coefficients of variation for a given history.
            /// </summary>
            /// <returns>
            ///     The mean of coefficients of variation.
            /// </returns>
            double CoefficientOfVariationMean();

            /// <summary>
            ///     Computes the variance for a given history.
            /// </summary>
            /// <returns>
            ///     The variance.
            /// </returns>
            double Variance();

            /// <summary>
            ///     Computes the mean of variances for a given history.
            /// </summary>
            /// <returns>
            ///     The mean of variances.
            /// </returns>
            double VarianceMean();

            /// <summary>
            ///     Computes the standard deviation for a given history.
            /// </summary>
            /// <returns>
            ///     The standard deviation.
            /// </returns>
            double StandardDeviation();

            /// <summary>
            ///     Computes the mean of standard deviations for a given history.
            /// </summary>
            /// <returns>
            ///     The mean of standard deviations.
            /// </returns>
            double StandardDeviationMean();

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
            int SignificanceTest(double value, const int significanceLevel, unsigned int totalSampleCount);

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
            int SignificanceTest(MeasuredHistory * pMeasuredHistory, const int significanceLevel);

        private:

            // Running sum of throughputs
            double m_sum;

            // Sum of throughput squares
            double m_sumOfSquares;

            // Count of measurements in this history
            int m_count;

            // An integer representing the control setting for this history measurement
            int m_controlSetting;

            // Last count value when a measurement was taken (used for relevance test)
            unsigned int m_lastDataPointCount;
        };

        /// <summary>
        ///     Makes a pseudo-random hill climbing move by alternating between up and down.
        /// </summary>
        /// <returns>
        ///     The random move.
        /// </returns>
        int GetRandomMove();

        /// <summary>
        ///     Recommends NewControlSetting to be used.
        /// </summary>
        /// <param name="newControlSetting">
        ///     The control setting to be established.
        /// </param>
        /// <returns>
        ///     New control setting to be used.
        /// </returns>
        unsigned  RecommendControlSetting(unsigned int newControlSetting);

        /// <summary>
        ///     Establishes control setting as current. This is the only method that updates the control settings.
        /// </summary>
        /// <param name="newControlSetting">
        ///     The control setting to be established.
        /// </param>
        void EstablishControlSetting(unsigned int newControlSetting);

        /// <summary>
        ///     Determines whether a given history measurement is stable enough to make a hill climbing move.
        /// </summary>
        /// <returns>
        ///     True if history measurement is stable.
        /// </returns>
        bool IsStableHistory(MeasuredHistory * pMeasuredHistory);

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
        double CalculateThroughput(unsigned int numberOfSamples, unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength);

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
        double CalculateThroughputSlope(int fromSetting, int toSetting);

        /// <summary>
        ///     Flushes all measurement histories that are no longer relevant.
        /// </summary>
        void FlushHistories();

        /// <summary>
        ///     Clears all measurement histories.
        /// </summary>
        void ClearHistories();

        /// <summary>
        ///     Gets the history measurement for a given control setting.
        /// </summary>
        /// <returns>
        ///     The history measurement.
        /// </returns>
        MeasuredHistory * GetHistory(unsigned int controlSetting);

        // The maximum number of histories to keep
        static const unsigned int MaxHistoryCount = 64;

        // The array where history data is kept
        MeasuredHistory m_histories[MaxHistoryCount];

        // Scheduler proxy to which this hill climbing instance belongs
        SchedulerProxy * m_pSchedulerProxy;

        // Used to determine the magnitude of moves, in units of (coefficient of variation)/(thread count)
        double m_controlGain;

        // Maximum number of resources that can be changed in one transition
        unsigned int m_maxControlSettingChange;

        // The current amount of resources allocated in this hill climbing instance
        unsigned int m_currentControlSetting;

        // The amount of resources allocated in this hill climbing instance before the last move
        unsigned int m_lastControlSetting;

        // Scheduler id
        unsigned int m_id;

        // Number of samples collected
        unsigned long m_sampleCount;

        // Number of samples collected including invalid samples, across settings
        unsigned long m_totalSampleCount;

        // Number of consecutive invalid samples
        unsigned long m_invalidCount;

        // Save sum of completions for consecutive invalid samples
        unsigned int m_saveCompleted;

        // Save sum of arrivals for consecutive invalid samples
        unsigned int m_saveIncoming;

        // Determines where the next random move is going
        bool m_nextRandomMoveIsUp;

#if defined(CONCRT_TRACING)
        /// <summary>
        ///     Logs the hill climbing decision by constructing a CSV dump of data.
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
        void LogData(unsigned int recommendedSetting, HillClimbingStateTransition transition, unsigned int numberOfSamples,
            unsigned int completionRate, unsigned int arrivalRate, unsigned int queueLength, double throughput);
#endif
    };
} // namespace details
} // namespace Concurrency
