#include "mock_hardware/mock_hardware.hpp"
#include <hardware_interface/types/hardware_interface_return_values.hpp>
#include <rclcpp/logging.hpp>

namespace mock_hardware
{

hardware_interface::CallbackReturn MockHardware::on_init(const hardware_interface::HardwareInfo &info)
{
    if (hardware_interface::SystemInterface::on_init(info) != hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    hw_commands_.resize(info.joints.size(), 0.0);
    hw_states_.resize(info.joints.size() * 2, 0.0); // For position and velocity

    RCLCPP_INFO(rclcpp::get_logger("MockHardware"), "Initialized mock hardware successfully.");
    return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> MockHardware::export_state_interfaces()
{
    std::vector<hardware_interface::StateInterface> state_interfaces;
    for (size_t i = 0; i < hw_states_.size() / 2; ++i)
    {
        state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, "position", &hw_states_[i * 2]));
        state_interfaces.emplace_back(hardware_interface::StateInterface(
            info_.joints[i].name, "velocity", &hw_states_[i * 2 + 1]));
    }
    return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> MockHardware::export_command_interfaces()
{
    std::vector<hardware_interface::CommandInterface> command_interfaces;
    for (size_t i = 0; i < hw_commands_.size(); ++i)
    {
        command_interfaces.emplace_back(hardware_interface::CommandInterface(
            info_.joints[i].name, "velocity", &hw_commands_[i]));
    }
    return command_interfaces;
}

hardware_interface::CallbackReturn MockHardware::on_activate(const rclcpp_lifecycle::State & /*previous_state*/)
{
    RCLCPP_INFO(rclcpp::get_logger("MockHardware"), "Activating mock hardware...");
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MockHardware::on_deactivate(const rclcpp_lifecycle::State & /*previous_state*/)
{
    RCLCPP_INFO(rclcpp::get_logger("MockHardware"), "Deactivating mock hardware...");
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type MockHardware::read(const rclcpp::Time & /*time*/, const rclcpp::Duration &period)
{
    // Simulate encoder feedback
    double delta_seconds = period.seconds();
    for (size_t i = 0; i < hw_states_.size() / 2; ++i)
    {
        hw_states_[i * 2] += hw_commands_[i] * delta_seconds; // Update position
        hw_states_[i * 2 + 1] = hw_commands_[i];             // Update velocity
    }
    return hardware_interface::return_type::OK;
}

hardware_interface::return_type MockHardware::write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
    // Simulate sending commands to actuators
    for (size_t i = 0; i < hw_commands_.size(); ++i)
    {
        RCLCPP_INFO(rclcpp::get_logger("MockHardware"), "Command for joint %ld: %f", i, hw_commands_[i]);
    }
    return hardware_interface::return_type::OK;
}

} // namespace mock_hardware

#include "pluginlib/class_list_macros.hpp"
// Register the MockHardware plugin with the hardware_interface::SystemInterface base class
PLUGINLIB_EXPORT_CLASS(mock_hardware::MockHardware, hardware_interface::SystemInterface)
