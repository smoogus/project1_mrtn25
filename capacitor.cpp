// Samuel Martin
// 10-14-2026
// ECE-231L Fall 2026 Section Group 1BW
// Project 1
// program simulates charging a capacitor for 5 microseconds using finite-difference eq-s.
// does this twice: once with a constant current source and once with a constant voltage source through a resistor
// time, voltage, and current values are stored in dynamically allocated arrays and printed every 200 timesteps

#include <iostream>
#include <iomanip>

struct _capacitor
{
    double *time;     // time array
    double *voltage;  // voltage array
    double *current;  // current array
    double C;         // capacitance value
};
typedef struct _capacitor Capacitor;

// Simulation constants
const double DT = 1e-10;          // time step (s)
const double T_FINAL = 5e-6;      // final time (s)
const int    NUM_STEPS = 50000;   // T_FINAL / DT
const double R = 1000.0;          // resistance (ohms)
const double C_VAL = 100e-12;     // capacitance (F)
const double I_CONST = 1e-2;      // constant current source (A)
const double V0 = 10.0;           // constant voltage source (V)
const int    PRINT_EVERY = 200;   // print interval (timesteps)

// Allocate arrays (NUM_STEPS + 1 entries so index 0 .. NUM_STEPS fits)
void allocateCapacitor(Capacitor *cap, int numSteps, double C)
{
    cap->time    = new double[numSteps + 1];
    cap->voltage = new double[numSteps + 1];
    cap->current = new double[numSteps + 1];
    cap->C = C;
}

// Free arrays
void freeCapacitor(Capacitor *cap)
{
    delete[] cap->time;
    delete[] cap->voltage;
    delete[] cap->current;
    cap->time = nullptr;
    cap->voltage = nullptr;
    cap->current = nullptr;
}

// Print every PRINT_EVERY timesteps
void printResults(const Capacitor *cap, int numSteps, int interval)
{
    std::cout << std::scientific << std::setprecision(6);
    std::cout << std::setw(8)  << "step"
              << std::setw(16) << "time (s)"
              << std::setw(16) << "voltage (V)"
              << std::setw(16) << "current (A)" << "\n";
    for (int i = 0; i <= numSteps; i += interval)
    {
        std::cout << std::setw(8)  << i
                  << std::setw(16) << cap->time[i]
                  << std::setw(16) << cap->voltage[i]
                  << std::setw(16) << cap->current[i] << "\n";
    }
}

// Constant current source: I(t) = I_CONST, V(t+1) = V(t) + I(t)*dt/C
void solveConstantCurrent(Capacitor *cap, int numSteps, double dt, double I0)
{
    // Initial conditions (timestep 0)
    cap->time[0]    = 0.0;
    cap->voltage[0] = 0.0;
    cap->current[0] = I0;

    for (int i = 1; i <= numSteps; i++)
    {
        cap->time[i]    = i * dt;
        cap->current[i] = I0;  // source holds current constant
        cap->voltage[i] = cap->voltage[i - 1]
                        + cap->current[i - 1] * dt / cap->C;
    }
}

// Constant voltage source with series resistor:
// I(t+1) = I(t) - I(t)/(RC) * dt,  V(t+1) = V(t) + I(t)*dt/C
void solveConstantVoltage(Capacitor *cap, int numSteps, double dt,
                          double Vsource, double Rval)
{
    // Initial conditions (timestep 0)
    cap->time[0]    = 0.0;
    cap->voltage[0] = 0.0;
    cap->current[0] = Vsource / Rval;

    for (int i = 1; i <= numSteps; i++)
    {
        cap->time[i]    = i * dt;
        cap->current[i] = cap->current[i - 1]
                        - (cap->current[i - 1] / (Rval * cap->C)) * dt;
        cap->voltage[i] = cap->voltage[i - 1]
                        + cap->current[i - 1] * dt / cap->C;
    }
}

int main()
{
    Capacitor cap;

    // Const current case
    allocateCapacitor(&cap, NUM_STEPS, C_VAL);
    solveConstantCurrent(&cap, NUM_STEPS, DT, I_CONST);
    std::cout << "=== Constant current source (I = " << I_CONST << " A) ===\n";
    printResults(&cap, NUM_STEPS, PRINT_EVERY);
    freeCapacitor(&cap);

    std::cout << "\n";

    // Const voltage case
    allocateCapacitor(&cap, NUM_STEPS, C_VAL);
    solveConstantVoltage(&cap, NUM_STEPS, DT, V0, R);
    std::cout << "=== Constant voltage source (V0 = " << V0
              << " V, R = " << R << " ohms) ===\n";
    printResults(&cap, NUM_STEPS, PRINT_EVERY);
    freeCapacitor(&cap);

    return 0;
}

