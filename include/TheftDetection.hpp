#ifndef THEFT_DETECTION_HPP
#define THEFT_DETECTION_HPP

class TheftDetection {
private:
    float expectedUnits = 0.0f;
    float actualUnits   = 0.0f;
    float threshold     = 100.0f; // units deviation considered suspicious

public:
    TheftDetection() = default;
    TheftDetection(float exp, float act, float thr = 100.0f)
        : expectedUnits(exp), actualUnits(act), threshold(thr) {}

    bool isTheft() const;
    void generateAlert() const;
};

#endif // THEFT_DETECTION_HPP
