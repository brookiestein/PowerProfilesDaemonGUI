#include "dbus_manager.hpp"

#include <algorithm>
#include <cctype>
#include <cassert>
#include <format>
#include <print>
#include <string>

DBusManager::DBusManager(sigc::slot<void (const std::string &)> on_error)
	 : DEST("org.freedesktop.UPower.PowerProfiles")
	 , PATH("/org/freedesktop/UPower/PowerProfiles")
	 , IFACE("org.freedesktop.DBus.Properties")
	 , TARGET_IFACE("org.freedesktop.UPower.PowerProfiles")
	 , PROPERTY("ActiveProfile")
{
	 m_error_signal.connect(on_error);

	 DBusError error;
	 dbus_error_init(&error);

	 m_dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

	 if (!m_dbus_connection) {
		  std::string message = std::format("Could not get system bus.\nError message: {}", error.message);
		  dbus_error_free(&error);
		  throw std::runtime_error(message);
	 }

	 dbus_error_free(&error);
}

DBusManager::POWER_PROFILE DBusManager::fetch_current_power_profile()
{
	 const char *method = "Get";
	 DBusMessage *message = dbus_message_new_method_call(DEST,
														 PATH,
														 IFACE,
														 method);

	 if (!message) {
		  m_error_signal.emit("System ran out of memory when allocating the DBus message.");
		  return POWER_PROFILE::INVALID;
	 }

	 assert(
		  dbus_message_append_args(message, DBUS_TYPE_STRING, &TARGET_IFACE, DBUS_TYPE_STRING, &PROPERTY, DBUS_TYPE_INVALID) &&
		  "Failed to append arguments."
	 );

	 DBusError error;
	 dbus_error_init(&error);

	 DBusMessage *reply = dbus_connection_send_with_reply_and_block(
		  m_dbus_connection,
		  message,
		  DBUS_TIMEOUT_USE_DEFAULT,
		  &error
	 );

	 if (!reply) {
		  m_error_signal.emit(
			   std::format("Failed to fetch current profile.\nError message: {}", error.message)
		  );

		  dbus_error_free(&error);
		  return POWER_PROFILE::INVALID;
	 }

	 dbus_error_free(&error);

	 DBusMessageIter args;
	 if (!dbus_message_iter_init(reply, &args)) {
		  dbus_message_unref(reply);
		  m_error_signal.emit("Got no response from Power Profiles Daemon.");
		  return POWER_PROFILE::INVALID;
	 }

	 if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT) {
		  dbus_message_unref(reply);
		  m_error_signal.emit("Got ill-formed response.");
		  return POWER_PROFILE::INVALID;
	 }

	 DBusMessageIter current_arg;
	 dbus_message_iter_recurse(&args, &current_arg);

	 if (dbus_message_iter_get_arg_type(&current_arg) != DBUS_TYPE_STRING) {
		  dbus_message_unref(reply);
		  m_error_signal.emit("Didn't found current profile in response.");
		  return POWER_PROFILE::INVALID;
	 }

	 char *str;
	 dbus_message_iter_get_basic(&current_arg, &str);

	 std::string current_profile(str);

	 dbus_message_unref(reply);

	 return string_to_power_profile(current_profile);
}

DBusManager::POWER_PROFILE DBusManager::string_to_power_profile(const std::string &profile)
{
	 auto ichar_equals = [] (char a, char b) -> bool {
		  return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
	 };

	 std::string prof = power_profile_to_string(POWER_PROFILE::POWER_SAVER);

	 if (std::equal(profile.begin(), profile.end(),
					power_profile_to_string(POWER_PROFILE::POWER_SAVER).begin(),
					ichar_equals)) {
		  return POWER_PROFILE::POWER_SAVER;
	 } else if (std::equal(profile.begin(), profile.end(),
						   power_profile_to_string(POWER_PROFILE::BALANCED).begin(), ichar_equals)) {
		  return POWER_PROFILE::BALANCED;
	 } else if (std::equal(profile.begin(), profile.end(),
						   power_profile_to_string(POWER_PROFILE::PERFORMANCE).begin(), ichar_equals)) {
		  return POWER_PROFILE::PERFORMANCE;
	 } else {
		  return POWER_PROFILE::INVALID;
	 }
}

std::string DBusManager::power_profile_to_string(POWER_PROFILE profile)
{
	 switch (profile)
	 {
	 case POWER_PROFILE::INVALID:
		  return "invalid";
	 case POWER_PROFILE::POWER_SAVER:
		  return "power-saver";
	 case POWER_PROFILE::BALANCED:
		  return "balanced";
	 case POWER_PROFILE::PERFORMANCE:
		  return "performance";
	 }
}

bool DBusManager::set_profile(POWER_PROFILE profile)
{
	 if (profile == POWER_PROFILE::INVALID) {
		  m_error_signal.emit("Cannot set invalid power profile.");
		  return false;
	 }

	 const char *method = "Set";
	 DBusMessage *message = dbus_message_new_method_call(DEST, PATH, IFACE, method);

	 if (!message) {
		  m_error_signal.emit("System ran out of memory while allocating the DBus message to be sent.");
		  return false;
	 }

	 const char *new_profile = nullptr;

	 switch (profile)
	 {
	 case POWER_PROFILE::INVALID:
		  break;
	 case POWER_PROFILE::POWER_SAVER:
		  new_profile = "power-saver";
		  break;
	 case POWER_PROFILE::BALANCED:
		  new_profile = "balanced";
		  break;
	 case POWER_PROFILE::PERFORMANCE:
		  new_profile = "performance";
		  break;
	 }

	 assert(new_profile);

	 DBusMessageIter args;
	 dbus_message_iter_init_append(message, &args);

	 assert(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &TARGET_IFACE));
	 assert(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &PROPERTY));

	 DBusMessageIter value;
	 const char *signature = "s";

	 assert(dbus_message_iter_open_container(&args, DBUS_TYPE_VARIANT, signature, &value));
	 assert(dbus_message_iter_append_basic(&value, DBUS_TYPE_STRING, &new_profile));
	 assert(dbus_message_iter_close_container(&args, &value));

	 DBusError error;
	 dbus_error_init(&error);

	 DBusMessage *reply = dbus_connection_send_with_reply_and_block(
		  m_dbus_connection,
		  message,
		  DBUS_TIMEOUT_USE_DEFAULT,
		  &error
	 );

	 if (!reply) {
		  m_error_signal.emit(
			   std::format("Failed to set current profile.\nError message: {}", error.message)
		  );

		  dbus_error_free(&error);
		  return false;
	 }

	 dbus_error_free(&error);
	 return true;
}
