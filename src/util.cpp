#include "util.h"
#include <cmath>

namespace DiscreteControlSystem {
    double T = 0.01; 
    
    double Kp = 0.0;
    double Ki = 0.0;
    double Kd = 0.0;

    double y[3] = {0.0, 0.0, 0.0};
    double u[2] = {0.0, 0.0};
    double integral = 0.0;
    double last_error = 0.0;

    double PIDController(double error) {
        double P = Kp * error;
        integral += error * T;
        double I = Ki * integral;
        double derivative = (error - last_error) / T;
        double D = Kd * derivative;
        
        last_error = error;
        return P + I + D;
    }

    double SystemModel(double input) {
        u[1] = input;
        double a1 = std::exp(-2.0 * T) + std::exp(-8.0 * T);
        double a2 = -std::exp(-10.0 * T);
        double b1 = 0.25 * (1.0 - a1 - a2); 

        y[2] = (a1 * y[1]) + (a2 * y[0]) + (b1 * u[0]);

        y[0] = y[1];
        y[1] = y[2];
        u[0] = u[1];

        return y[2];
    }
}