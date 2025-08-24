class SPEED {
public:
    f32_t min;
    f32_t max;
    f32_t base;
    PID pid;

    SPEED(f32_t min_, f32_t max_, f32_t base_, PID pid_)
        : min(min_), max(max_), base(base_), pid(pid_) {}

    int adjustPID(int error) {
        float correction = pid.calc(error);
        int output = static_cast<int>(base + correction);

        if (output < min) output = static_cast<int>(min);
        if (output > max) output = static_cast<int>(max);
        return output;
    }
};
