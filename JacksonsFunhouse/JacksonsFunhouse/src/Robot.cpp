#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <I2C.h>
#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>
#include <Timer.h>
#include <WPIlib.h>


class Robot: public frc::IterativeRobot {

	// all necessary private instance variables
	frc::LiveWindow* lw = LiveWindow::GetInstance();
	frc::SendableChooser<std::string> chooser;
	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";
	std::string autoSelected;

	// Motors for the robot dive
	frc::VictorSP frontLeft;
	frc::VictorSP frontRight;
	frc::VictorSP rearLeft;
	frc::VictorSP rearRight;
	frc::RobotDrive drive;

	// Climber mechanism motors
	frc::VictorSP climberLeft;
	frc::VictorSP climberRight;

	// Servos for Gear mechanism
	frc::Servo leftServo;
	frc::Servo rightServo;

	// Joystick controls
	frc::Joystick leftStick;
	frc::Joystick rightStick;

	// Timer used in autonomous mode for "dead reckoning"
	frc::Timer timer;

	// Pointer to the Lidar object
	I2C* i2c_Lidar;

	public:

	/**
	 * Constructor for the Robot class
	 *
	 * Post Condition: establish where all the motors are and initialize variables
	 *
	 */
	Robot() : frontLeft(1),
			  frontRight(3),
			  rearLeft(0),
			  rearRight(2),
			  drive(frontLeft, rearLeft, frontRight, rearRight),
			  climberLeft(4),
			  climberRight(5),
			  leftServo(6),
			  rightServo(7),
			  leftStick(0),
			  rightStick(1)
	{

		// new I2C object in order to manipulate Lidar
		i2c_Lidar = new I2C(I2C::Port::kOnboard, 0x62);

		// new timer object to monitor time
		timer = new Timer();

	}

	void RobotInit() {

	// helps configure the smart dashboard
	chooser.AddDefault(autoNameDefault, autoNameDefault);
	chooser.AddObject(autoNameCustom, autoNameCustom);
	frc::SmartDashboard::PutData("Auto Modes", &chooser);

	}

	// Helper function which opens servos to release a Gear
	void OpenServos()
	{
		leftServo.Set(0.5);
		rightServo.Set(0.75);
	}

	// Helper function which closes servos to hold onto a Gear
	void CloseServos()
	{
		leftServo.Set(1.0);
		rightServo.Set(0.0);
	}

	// Helper function to return all actuators to their off/reset state
	void Reset()
	{
		// Reset everything
		drive.ArcadeDrive(0.0, 0.0);
		CloseServos();
		climberLeft.Set(0.0);
		climberRight.Set(0.0);

	}

	// return the received distance from the Lidar
	int getDistance() {

		i2c_Lidar->Write(0x00, 0x00);
		Wait(.025);
		i2c_Lidar->Write(0x00, 0x04);
		Wait(.005);

		int highByte, lowByte;

		uint8_t writeData [2];
		uint8_t readData [2];

		writeData[0] = 0x8F;  // Bulk Write Based Read_Only for a two register Read of 0x8F (distance)
		i2c_Lidar->WriteBulk(writeData,1);
		i2c_Lidar->ReadOnly(2, readData);

		highByte = (int)readData[0];
		lowByte = (int)readData[1];

		int distance = 256*highByte + lowByte;

		return distance;

	}

	/*
	 * This autonomous (along with the chooser code above) shows how to select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * GetString line to get the auto name from the text box below the Gyro.
	 *
	 * You can add additional auto modes by adding additional comparisons to the
	 * if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit() override {

	autoSelected = chooser.GetSelected();
	std::string autoSelected = SmartDashboard::GetString("Auto Selector", autoNameDefault);
	std::cout << "Auto selected: " << autoSelected << std::endl;

	if (autoSelected == autoNameCustom) {

	} else {

	}

	}

	void driveForward() {

		int distance = getDistance();
		while (distance < 320) {

			drive.ArcadeDrive(-0.5, 0.0);

		}

	}

	void driveForward( std::String side ) {

		if (side == "L") {

			timer.Start();

			double time = timer.Get();

			if (time < 10) {

				drive.ArcadeDrive(-0.5, 0.0);

			}


		}

	}

	void AutonomousPeriodic() {
	//	if (autoSelected == autoNameCustom) {
			// Custom Auto goes here
//		} else {
			// Default Auto goes here
	//	}


		timer.Start();

		double time = timer.Get();

		if (time < 5) {

			drive.ArcadeDrive(-0.5, 0.0);

		}
		else {

			drive.ArcadeDrive(0.5, 1);

		}



















	}

	void TeleopInit() {

		Reset();

	}

	void TeleopPeriodic() {

		// Accept UI input, command robot
		double forwardEffort = -leftStick.GetY();
		double direction = -rightStick.GetX();
		drive.ArcadeDrive(forwardEffort, direction);

		bool openServos = rightStick.GetRawButton(1);
		if (openServos)
		{

			OpenServos();

		} else {



		}

		bool climb = rightStick.GetRawButton(6);

		if (climb)
		{

			climberLeft.Set(-0.5);
			climberRight.Set(-0.5);

		} else {

			climberLeft.Set(0.0);
			climberRight.Set(0.0);

		}


		int distance = getDistance();
		if (distance < 25 && distance > 1) {

		OpenServos();

		} else {
			CloseServos();
		}

		std::cout << getDistance() << std::endl;

	}

	void TestPeriodic() {

	//	lw->Run();

	}

};

START_ROBOT_CLASS(Robot)
