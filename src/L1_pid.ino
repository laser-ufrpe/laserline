//===========================================================
//                   PID CONTROLLER
//===========================================================
class PID {
    public:
        // Construtor: recebe os ganhos p, i, d
    PID(float p, float i, float d) {
        conf(p, i, d);
        limit(0.0f, 1.0f);
        reset();
    }

    // Zera histórico e erro anterior
    void reset() {
        prev_error = 0.0f;
        integral = 0.0f;
    }

    // Define ganhos
    void conf(float p, float i, float d) {
        kp = p;
        ki = i;
        kd = d;
    }

    // Define limites de saída
    void limit(float minVal, float maxVal) {
        this->min_out = minVal;
        this->max_out = maxVal;
    }

    // Calcula saída do PID para um erro
    float calc(float error) {
        integral += error;  // termo integral
        float derivative = error - prev_error; // termo derivativo
        prev_error = error;
        float output = (kp * error) + (ki * integral) + (kd * derivative);

        // Limita valor entre min_out e max_out
        if (output > max_out) output = max_out;
        else if (output < min_out) output = min_out;
        return output;
    }
    
    // Ganhos
    float kp, ki, kd;
    // Último erro (para derivada) e soma de erros (para integral)
    float prev_error, integral;
    // Limites
    float min_out, max_out;
};
