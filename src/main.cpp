#include "main.h"


// EDIT THESE CONSTANTS

// The numbers are port numbers and the negative means that the motor should be reversed

// Do not change negatives unless they break
// These are for the left motors
#define LEFT_MOTOR_ONE -1
#define LEFT_MOTOR_TWO -2
#define LEFT_MOTOR_THREE -3

// These are for the right motors 
#define RIGHT_MOTOR_ONE 4
#define RIGHT_MOTOR_TWO 5
#define RIGHT_MOTOR_THREE 6

// this is for the flywheel motor
#define FLYWHEEL_MOTOR_PORT 10
// this is for the arm motor
#define ARM_MOTOR_PORT 8

// note about pneumatics: plug the red and black wire into the left side of the solenoid
// and the white wire onto the right side
// put port names in 'single quotes' (ex: 'A')
// this is for the left pneumatic piston
#define PNEUMATICS_PORT_LEFT 'A'
// this is for the right pneumatic piston
#define PNEUMATICS_PORT_RIGHT 'B'

// how to change these: just edit the thing after the 2 colons
// ex, changing pneumatics button from A to B would be 
// old => ... ControllerDigital::A;
// new => ... ControllerDigital::X;
const ControllerDigital PNEUMATICS_BUTTON = ControllerDigital::A;
const ControllerDigital REVERSE_BUTTON = ControllerDigital::X;

const ControllerDigital CONTINUOUS_FLYWHEEL_BUTTON = ControllerDigital::up;
const ControllerDigital FLYWHEEL_PULL_BUTTON = ControllerDigital::R2;
const ControllerDigital FLYWHEEL_PUSH_BUTTON = ControllerDigital::L2;

const ControllerDigital LEVER_UP_BUTTON = ControllerDigital::R1;
const ControllerDigital LEVER_DOWN_BUTTON = ControllerDigital::L1;



// OTHER CONSTANTS

std::shared_ptr<ChassisController> drive;

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}


}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */


void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);

	pros::ADIDigitalOut left_piston (PNEUMATICS_PORT_LEFT);
	pros::ADIDigitalOut right_piston (PNEUMATICS_PORT_RIGHT);

	pros::Motor flywheel_initializer (FLYWHEEL_MOTOR_PORT, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
	pros::Motor arm_initializer (ARM_MOTOR_PORT, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */



void opcontrol() {
	Controller controller;
	pros::ADIDigitalOut left_piston (PNEUMATICS_PORT_LEFT);
	pros::ADIDigitalOut right_piston (PNEUMATICS_PORT_RIGHT);

	pros::Motor flywheel_motor (FLYWHEEL_MOTOR_PORT);
	pros::Motor arm_motor (ARM_MOTOR_PORT);

	ControllerButton pneumatics (PNEUMATICS_BUTTON);
	ControllerButton reverse (REVERSE_BUTTON);
	ControllerButton continious_flywheel (CONTINUOUS_FLYWHEEL_BUTTON);
	ControllerButton flywheel_pull (FLYWHEEL_PULL_BUTTON);
	ControllerButton flywheel_push (FLYWHEEL_PUSH_BUTTON);
	ControllerButton lever_up (LEVER_UP_BUTTON);
	ControllerButton lever_down (LEVER_DOWN_BUTTON);


	ControllerButton abutton (ControllerDigital::A);

	std::shared_ptr<ChassisController> drive =
		ChassisControllerBuilder()
			.withMotors({LEFT_MOTOR_ONE, LEFT_MOTOR_TWO, LEFT_MOTOR_THREE}, {RIGHT_MOTOR_ONE, RIGHT_MOTOR_TWO, RIGHT_MOTOR_THREE}) // TODO: REVERSE ONE OF THESE???
			.withDimensions(AbstractMotor::gearset::green, {{4_in, 14_in}, imev5GreenTPR}) // TODO: CHANGE THESE!!!!
			.build();

	bool pistonState = false;
	bool flywheelContinous = false;
	bool reverseSt = false;

	while (true) {

		if (reverse.changedToPressed()) reverseSt = !reverseSt;
		if (continious_flywheel.changedToPressed()) flywheelContinous = !flywheelContinous;

		if (flywheelContinous) flywheel_motor.move(127);
		else flywheel_motor.brake();

		if (!flywheelContinous && flywheel_pull.isPressed()) flywheel_motor.move(127);
		if (!flywheelContinous && flywheel_push.isPressed()) flywheel_motor.move(-127);

		if (lever_up.isPressed()) arm_motor.move(127);
		else if (lever_down.isPressed()) arm_motor.move(-127);
		else arm_motor.brake();

		if (!reverseSt) {
			drive->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY), -1.0 * controller.getAnalog(ControllerAnalog::rightX));
		} else {
			drive->getModel()->arcade(controller.getAnalog(ControllerAnalog::leftY) * -1.0, controller.getAnalog(ControllerAnalog::rightX));
		}


		if (pneumatics.changedToPressed()) {
			pistonState = !pistonState;
		} 

		left_piston.set_value(pistonState);
		right_piston.set_value(pistonState);

		pros::delay(10);
	
	}
}
