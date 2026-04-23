#ifndef UTIL_H
#define UTIL_H

namespace DiscreteControlSystem {
    extern double T; 
    extern double y[3];
    
    extern double Kp;
    extern double Ki;
    extern double Kd;
    
    double PIDController(double error); 
    double SystemModel(double input);
}

#endif