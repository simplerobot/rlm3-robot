# Connect phone to robot.
As a user, I would like to connect my phone to the robot using my phone, so I can control the robot.

Typical Sequence:
* Robot: Creates a local wifi network with known settings.
* User: Launches phone application.
* User: Presses connect.
* Phone: Connects to local wifi network of robot.
* User: Presses ping.
* Phone: Sends simple data to robot.
* Robot: Echos data to console.

# TODO: Control robot from phone.
As a user, I would like to control the robot's movement using my phone, so I can move the robot around my yard.

Typical Sequence:
* Robot: Creates a local wifi network with known settings.
* User: Launches phone application.
* User: Presses connect.
* Phone: Connects to local wifi network of robot.
* Phone: Shows control application.
* User: Presses forward button.
* Robot: Moves forward.

Safety concerns:
* Robot stops if user removes finger from control.
* Robot stops if network lags.



