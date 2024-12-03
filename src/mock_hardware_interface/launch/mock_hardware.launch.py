from launch import LaunchDescription
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.substitutions import PathJoinSubstitution, Command, FindExecutable

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # Define paths for your package
    package_name = "mock_hardware_interface"
    urdf_file = "robot.urdf"
    rviz_file = "robot.rviz"
    controller_config_file = "controllers.yaml"

    # Get URDF file content
    robot_description_content = PathJoinSubstitution(
        [FindPackageShare(package_name), "description", urdf_file]
    )

    robot_description = {"robot_description": Command(["cat ", robot_description_content])}

    # Path to the controller configuration
    robot_controllers = PathJoinSubstitution(
        [FindPackageShare(package_name), "config", controller_config_file]
    )

    # RViz configuration file
    rviz_config_file = PathJoinSubstitution(
        [FindPackageShare(package_name), "rviz", rviz_file]
    )

    # Main control node (ros2_control)
    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, robot_controllers],
        arguments=['--ros-args','--log-level','debug'],
        output="both",
    )

    # Robot state publisher node
    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[robot_description],
    )

    # RViz node
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config_file],
    )

    # Joint state broadcaster spawner
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    # Robot controller spawner
    robot_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_drive_controller", "--controller-manager", "/controller_manager"],
        output="screen",
    )

    # Delay RViz start after `joint_state_broadcaster`
    delay_rviz_after_joint_state_broadcaster = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[rviz_node],
        )
    )

    # Delay robot controller start after `joint_state_broadcaster`
    delay_robot_controller_after_joint_state_broadcaster = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[robot_controller_spawner],
        )
    )

    # Nodes to launch
    nodes = [
        control_node,
        robot_state_pub_node,
        joint_state_broadcaster_spawner,
        delay_rviz_after_joint_state_broadcaster,
        delay_robot_controller_after_joint_state_broadcaster,
    ]

    return LaunchDescription(nodes)
