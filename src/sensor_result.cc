
#include "sensor_result.h"


namespace sensor {
    
    // result class
    result::result(resultType _t, resultValue _r) {
        t = _t;
        r  = _r;
        date = std::chrono::system_clock::now(); // Get current time
    }
    
    result::~result() {}
    
    resultValue result::getValue() {
        return r;
    }
    
    resultType result::getType() {
        return t;
    }
    
    resultTimestamp result::getTime() {
        return date;
    }
    
    long long result::getTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(date.time_since_epoch()).count();
    }

    // resultsOrError class
    resultsOrError::resultsOrError() {}
    
    resultsOrError::resultsOrError(sensorException _error) : error(_error) {}

    resultsOrError::resultsOrError(std::list<result> _results) : results(_results) {}

    resultsOrError::~resultsOrError() {}

    sensorException resultsOrError::getError() {
        return error;
    }

    std::list<result> resultsOrError::getResults() {
        return results;
    }

    bool resultsOrError::hasError() {
        return error.code() != sensorErrorCode::NONE;
    }

    bool resultsOrError::hasResults() {
        return !hasError();
    }

}
