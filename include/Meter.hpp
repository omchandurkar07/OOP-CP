#ifndef METER_HPP
#define METER_HPP

class Meter {
private:
    float previousReading = 0.0f;
    float currentReading  = 0.0f;

public:
    Meter() = default;
    Meter(float prev, float cur) : previousReading(prev), currentReading(cur) {}

    void enterReading();
    float calculateUnits() const;
    void setPrevious(float r) { previousReading = r; }
    void setCurrent (float r) { currentReading  = r; }
    float getPrev() const { return previousReading; }
    float getCurr() const { return currentReading; }
};

#endif // METER_HPP
