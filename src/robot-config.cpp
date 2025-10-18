#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor LF = motor(PORT1, ratio6_1, true);
motor LM = motor(PORT2, ratio6_1, true);
motor LB = motor(PORT3, ratio6_1, true);
motor RB = motor(PORT13, ratio6_1, false);
motor RF = motor(PORT11, ratio6_1, false);
motor RM = motor(PORT12, ratio6_1, false);

//motor Roller = motor(PORT21, ratio6_1, false);

motor IntakeU = motor(PORT10, ratio6_1, false);
motor IntakeB = motor(PORT21, ratio6_1, false);

digital_out Pistake = digital_out(Brain.ThreeWirePort.D);
digital_out Tilt = digital_out(Brain.ThreeWirePort.B);
digital_out Clamp = digital_out(Brain.ThreeWirePort.A);

digital_out IntakeLift = digital_out(Brain.ThreeWirePort.D);
digital_out Scraper = digital_out(Brain.ThreeWirePort.B);
inertial Gyro = inertial(PORT2);
//Naming convention: 
// Important variables
const double wheelDiam = 3.25;
const double wheelToMotorRatio = 48.0/36;

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}