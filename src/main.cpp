#include "main.h"


// CONSTANTS

#define LEFT_MOTOR_ONE 1
#define LEFT_MOTOR_TWO 2
#define LEFT_MOTOR_THREE 3

// reverse this one?
#define RIGHT_MOTOR_ONE 4
#define RIGHT_MOTOR_TWO 5
#define RIGHT_MOTOR_THREE 6

#define FLYWHEEL_MOTOR_PORT 7
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

#define PNEUMATICS_PORT_LEFT 'A'
#define PNEUMATICS_PORT_RIGHT 'B'

void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);

	pros::ADIDigitalOut left_piston (PNEUMATICS_PORT_LEFT);
	pros::ADIDigitalOut right_piston (PNEUMATICS_PORT_RIGHT);

	pros::Motor flywheel_initializer (FLYWHEEL_MOTOR_PORT, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
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

	std::shared_ptr<ChassisController> drive =
		ChassisControllerBuilder()
			.withMotors({LEFT_MOTOR_ONE, LEFT_MOTOR_TWO, LEFT_MOTOR_THREE}, {RIGHT_MOTOR_ONE, RIGHT_MOTOR_TWO, RIGHT_MOTOR_THREE}) // TODO: REVERSE ONE OF THESE???
			.withDimensions(AbstractMotor::gearset::green, {{4_in, 14_in}, imev5GreenTPR}) // TODO: CHANGE THESE!!!!
			.build();

	while (true) {

		// optionally: replace with ->arcade
		drive->getModel()->tank(controller.getAnalog(ControllerAnalog::leftY),controller.getAnalog(ControllerAnalog::rightY));

		if (controller.getDigital(ControllerDigital::A)) {
			left_piston.set_value(true);
			right_piston.set_value(true);
		} else {
			left_piston.set_value(false);
			right_piston.set_value(false);
		}

		if (controller.getDigital(ControllerDigital::B)) {
			flywheel_motor.move(127);
		} else {
			flywheel_motor.brake();
		}

		pros::delay(10);
	
	}
}
