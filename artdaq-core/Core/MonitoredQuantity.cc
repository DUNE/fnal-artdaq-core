#include "artdaq-core/Core/MonitoredQuantity.hh"

#include <algorithm>
#include <math.h>

using namespace artdaq;

MonitoredQuantity::MonitoredQuantity
(
  DURATION_T expectedCalculationInterval,
  DURATION_T timeWindowForRecentResults
):
  _enabled(true),
  _expectedCalculationInterval(expectedCalculationInterval)
{
  setNewTimeWindowForRecentResults(timeWindowForRecentResults);
}

void MonitoredQuantity::addSample(const double value)
{
  if (! _enabled) {return;}
  boost::mutex::scoped_lock sl(_accumulationMutex);
  if (_lastCalculationTime <= 0.0) {
    _lastCalculationTime = getCurrentTime();
  }
  ++_workingSampleCount;
  _workingValueSum += value;
  _workingValueSumOfSquares += (value * value);
  if (value < _workingValueMin) { _workingValueMin = value; }
  if (value > _workingValueMax) { _workingValueMax = value; }
  _workingLastSampleValue = value;
}

void  MonitoredQuantity::addSample(const int value)
{
  addSample(static_cast<double>(value));
}

void  MonitoredQuantity::addSample(const uint32_t value)
{
  addSample(static_cast<double>(value));
}

void  MonitoredQuantity::addSample(const uint64_t value)
{
  addSample(static_cast<double>(value));
}

bool MonitoredQuantity::calculateStatistics(TIME_POINT_T currentTime)
{
  if (! _enabled) {return false;}
  if (_lastCalculationTime <= 0.0) {return false;}
  if (currentTime - _lastCalculationTime < _expectedCalculationInterval) {
    return false;
  }
  // create local copies of the working values to minimize the
  // time that we could block a thread trying to add a sample.
  // Also, reset the working values.
  long long latestSampleCount;
  double latestValueSum;
  double latestValueSumOfSquares;
  double latestValueMin;
  double latestValueMax;
  DURATION_T latestDuration;
  double latestLastLatchedSampleValue;
  {
    boost::mutex::scoped_lock sl(_accumulationMutex);
    latestSampleCount = _workingSampleCount;
    latestValueSum = _workingValueSum;
    latestValueSumOfSquares = _workingValueSumOfSquares;
    latestValueMin = _workingValueMin;
    latestValueMax = _workingValueMax;
    latestDuration = currentTime - _lastCalculationTime;
    latestLastLatchedSampleValue = _workingLastSampleValue;
    _lastCalculationTime = currentTime;
    _workingSampleCount = 0;
    _workingValueSum = 0.0;
    _workingValueSumOfSquares = 0.0;
    _workingValueMin =  INFINITY;
    _workingValueMax = -INFINITY;
  }
  // lock out any interaction with the results while we update them
  {
    boost::mutex::scoped_lock sl(_resultsMutex);
    _lastLatchedSampleValue = latestLastLatchedSampleValue;
    _lastLatchedCalculationTime = _lastCalculationTime;
    // we simply add the latest results to the full set
    _fullSampleCount += latestSampleCount;
    _fullValueSum += latestValueSum;
    _fullValueSumOfSquares += latestValueSumOfSquares;
    if (latestValueMin < _fullValueMin) {_fullValueMin = latestValueMin;}
    if (latestValueMax > _fullValueMax) {_fullValueMax = latestValueMax;}
    _fullDuration += latestDuration;
    // for the recent results, we need to replace the contents of
    // the working bin and re-calculate the recent values
    _binSampleCount[_workingBinId] = latestSampleCount;
    _binValueSum[_workingBinId] = latestValueSum;
    _binValueSumOfSquares[_workingBinId] = latestValueSumOfSquares;
    _binValueMin[_workingBinId] = latestValueMin;
    _binValueMax[_workingBinId] = latestValueMax;
    _binDuration[_workingBinId] = latestDuration;
    _binEndTime[_workingBinId] = _lastCalculationTime;
    if (latestDuration > 0.0) {
      _lastLatchedValueRate = latestValueSum / latestDuration;
    }
    else {
      _lastLatchedValueRate = 0.0;
    }
    _recentSampleCount = 0;
    _recentValueSum = 0.0;
    _recentValueSumOfSquares = 0.0;
    _recentValueMin =  INFINITY;
    _recentValueMax = -INFINITY;
    _recentDuration = 0.0;
    for (unsigned int idx = 0; idx < _binCount; ++idx) {
      _recentSampleCount += _binSampleCount[idx];
      _recentValueSum += _binValueSum[idx];
      _recentValueSumOfSquares += _binValueSumOfSquares[idx];
      if (_binValueMin[idx] < _recentValueMin) {
        _recentValueMin = _binValueMin[idx];
      }
      if (_binValueMax[idx] > _recentValueMax) {
        _recentValueMax = _binValueMax[idx];
      }
      _recentDuration += _binDuration[idx];
    }
    // update the working bin ID here so that we are ready for
    // the next calculation request
    ++_workingBinId;
    if (_workingBinId >= _binCount) {_workingBinId = 0;}
    // calculate the derived full values
    if (_fullDuration > 0.0) {
      _fullSampleRate = static_cast<double>(_fullSampleCount) / _fullDuration;
      _fullValueRate = static_cast<double>(_fullValueSum) / _fullDuration;
    }
    else {
      _fullSampleRate = 0.0;
      _fullValueRate = 0.0;
    }
    if (_fullSampleCount > 0) {
      _fullValueAverage = _fullValueSum / static_cast<double>(_fullSampleCount);
      double squareAvg = _fullValueSumOfSquares / static_cast<double>(_fullSampleCount);
      double avg = _fullValueSum / static_cast<double>(_fullSampleCount);
      double sigSquared = squareAvg - avg * avg;
      if (sigSquared > 0.0) {
        _fullValueRMS = sqrt(sigSquared);
      }
      else {
        _fullValueRMS = 0.0;
      }
    }
    else {
      _fullValueAverage = 0.0;
      _fullValueRMS = 0.0;
    }
    // calculate the derived recent values
    if (_recentDuration > 0.0) {
      _recentSampleRate = static_cast<double>(_recentSampleCount) / _recentDuration;
      _recentValueRate = static_cast<double>(_recentValueSum) / _recentDuration;
    }
    else {
      _recentSampleRate = 0.0;
      _recentValueRate = 0.0;
    }
    if (_recentSampleCount > 0) {
      _recentValueAverage = _recentValueSum / static_cast<double>(_recentSampleCount);
      double squareAvg = _recentValueSumOfSquares /
                         static_cast<double>(_recentSampleCount);
      double avg = _recentValueSum / static_cast<double>(_recentSampleCount);
      double sigSquared = squareAvg - avg * avg;
      if (sigSquared > 0.0) {
        _recentValueRMS = sqrt(sigSquared);
      }
      else {
        _recentValueRMS = 0.0;
      }
    }
    else {
      _recentValueAverage = 0.0;
      _recentValueRMS = 0.0;
    }
  }
  return true;
}

void MonitoredQuantity::_reset_accumulators()
{
  _lastCalculationTime = 0;
  _workingSampleCount = 0;
  _workingValueSum = 0.0;
  _workingValueSumOfSquares = 0.0;
  _workingValueMin =  INFINITY;
  _workingValueMax = -INFINITY;
  _workingLastSampleValue = 0;
}

void MonitoredQuantity::_reset_results()
{
  _workingBinId = 0;
  for (unsigned int idx = 0; idx < _binCount; ++idx) {
    _binSampleCount[idx] = 0;
    _binValueSum[idx] = 0.0;
    _binValueSumOfSquares[idx] = 0.0;
    _binValueMin[idx] =  INFINITY;
    _binValueMax[idx] = -INFINITY;
    _binDuration[idx] = 0.0;
    _binEndTime[idx] = 0.0;
  }
  _fullSampleCount = 0;
  _fullSampleRate = 0.0;
  _fullValueSum = 0.0;
  _fullValueSumOfSquares = 0.0;
  _fullValueAverage = 0.0;
  _fullValueRMS = 0.0;
  _fullValueMin =  INFINITY;
  _fullValueMax = -INFINITY;
  _fullValueRate = 0.0;
  _fullDuration = 0.0;
  _recentSampleCount = 0;
  _recentSampleRate = 0.0;
  _recentValueSum = 0.0;
  _recentValueSumOfSquares = 0.0;
  _recentValueAverage = 0.0;
  _recentValueRMS = 0.0;
  _recentValueMin =  INFINITY;
  _recentValueMax = -INFINITY;
  _recentValueRate = 0.0;
  _recentDuration = 0.0;
  _lastLatchedSampleValue = 0.0;
  _lastLatchedValueRate = 0.0;
}

void MonitoredQuantity::reset()
{
  {
    boost::mutex::scoped_lock sl(_accumulationMutex);
    _reset_accumulators();
  }
  {
    boost::mutex::scoped_lock sl(_resultsMutex);
    _reset_results();
  }
}

void MonitoredQuantity::enable()
{
  if (! _enabled) {
    reset();
    _enabled = true;
  }
}

void MonitoredQuantity::disable()
{
  // It is faster to just set _enabled to false than to test and set
  // it conditionally.
  _enabled = false;
}

void MonitoredQuantity::setNewTimeWindowForRecentResults(DURATION_T interval)
{
  // lock the results objects since we're dramatically changing the
  // bins used for the recent results
  {
    boost::mutex::scoped_lock sl(_resultsMutex);
    _intervalForRecentStats = interval;
    // determine how many bins we should use in our sliding window
    // by dividing the input time window by the expected calculation
    // interval and rounding to the nearest integer.
    // In case that the calculation interval is larger then the
    // interval for recent stats, keep the last one.
    _binCount = std::max(1U,
                         static_cast<unsigned int>(
                           (_intervalForRecentStats / _expectedCalculationInterval) + 0.5
                         )
                        );
    // create the vectors for the binned quantities
    _binSampleCount.reserve(_binCount);
    _binValueSum.reserve(_binCount);
    _binValueSumOfSquares.reserve(_binCount);
    _binValueMin.reserve(_binCount);
    _binValueMax.reserve(_binCount);
    _binDuration.reserve(_binCount);
    _binEndTime.reserve(_binCount);
    _reset_results();
  }
  {
    boost::mutex::scoped_lock sl(_accumulationMutex);
    _reset_accumulators();
  }
  // call the reset method to populate the correct initial values
  // for the internal sample data
  //reset();
}

bool MonitoredQuantity::
waitUntilAccumulatorsHaveBeenFlushed(DURATION_T timeout) const
{
  {
    boost::mutex::scoped_lock sl(_accumulationMutex);
    if (_workingSampleCount == 0) {return true;}
  }
  long sleepTime = static_cast<long>(timeout * 100000.0);
  for (int idx = 0; idx < 10; ++idx) {
    usleep(sleepTime);
    {
      boost::mutex::scoped_lock sl(_accumulationMutex);
      if (_workingSampleCount == 0) {return true;}
    }
  }
  return false;
}

void
MonitoredQuantity::getStats(Stats & s) const
{
  boost::mutex::scoped_lock results(_resultsMutex);
  s.fullSampleCount = _fullSampleCount;
  s.fullSampleRate = _fullSampleRate;
  s.fullValueSum = _fullValueSum;
  s.fullValueSumOfSquares = _fullValueSumOfSquares;
  s.fullValueAverage = _fullValueAverage;
  s.fullValueRMS = _fullValueRMS;
  s.fullValueMin = _fullValueMin;
  s.fullValueMax = _fullValueMax;
  s.fullValueRate = _fullValueRate;
  s.fullDuration = _fullDuration;
  s.recentSampleCount = _recentSampleCount;
  s.recentSampleRate = _recentSampleRate;
  s.recentValueSum = _recentValueSum;
  s.recentValueSumOfSquares = _recentValueSumOfSquares;
  s.recentValueAverage = _recentValueAverage;
  s.recentValueRMS = _recentValueRMS;
  s.recentValueMin = _recentValueMin;
  s.recentValueMax = _recentValueMax;
  s.recentValueRate = _recentValueRate;
  s.recentDuration = _recentDuration;
  s.recentBinnedSampleCounts.resize(_binCount);
  s.recentBinnedValueSums.resize(_binCount);
  s.recentBinnedDurations.resize(_binCount);
  s.recentBinnedEndTimes.resize(_binCount);
  unsigned int sourceBinId = _workingBinId;
  for (unsigned int idx = 0; idx < _binCount; ++idx) {
    if (sourceBinId >= _binCount) {sourceBinId = 0;}
    s.recentBinnedSampleCounts[idx] = _binSampleCount[sourceBinId];
    s.recentBinnedValueSums[idx] = _binValueSum[sourceBinId];
    s.recentBinnedDurations[idx] = _binDuration[sourceBinId];
    s.recentBinnedEndTimes[idx] = _binEndTime[sourceBinId];
    ++sourceBinId;
  }
  s.lastSampleValue = _lastLatchedSampleValue;
  s.lastValueRate = _lastLatchedValueRate;
  s.lastCalculationTime = _lastLatchedCalculationTime;
  s.enabled = _enabled;
}

MonitoredQuantity::TIME_POINT_T MonitoredQuantity::getCurrentTime()
{
  TIME_POINT_T result = -1.0;
  timeval now;
  if (gettimeofday(&now, 0) == 0) {
    result = static_cast<TIME_POINT_T>(now.tv_sec);
    result += static_cast<TIME_POINT_T>(now.tv_usec) / (1000 * 1000);
  }
  return result;
}
