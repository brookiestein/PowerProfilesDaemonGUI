#include "dbus_manager.hpp"

#include <algorithm>
#include <cctype>
#include <cassert>
#include <format>
#include <giomm/dbusmessage.h>
#include <giomm/error.h>
#include <giomm/init.h>
#include <print>
#include <string>
#include <tuple>

DBusManager::DBusManager()
	 : DEST("org.freedesktop.UPower.PowerProfiles")
	 , POWER_PROFILES_PATH("/org/freedesktop/UPower/PowerProfiles")
	 , IFACE("org.freedesktop.DBus.Properties")
	 , TARGET_IFACE("org.freedesktop.UPower.PowerProfiles")
	 , PROPERTY("ActiveProfile")
{
	 m_dbus_connection = Gio::DBus::Connection::get_sync(Gio::DBus::BusType::SYSTEM, nullptr);

	 if (!m_dbus_connection)
		  throw std::runtime_error("Could not get system bus.");
}

sigc::signal<void(const std::string &)> &DBusManager::signal_success()
{
	 return m_signal_success;
}

sigc::signal<void(const std::string &)> &DBusManager::signal_error()
{
	 return m_signal_error;
}

DBusManager::POWER_PROFILE DBusManager::fetch_current_power_profile()
{
	 auto arguments = Glib::Variant<std::tuple<Glib::ustring, Glib::ustring>>::create(
		  std::make_tuple(TARGET_IFACE, PROPERTY)
	 );

	 Glib::VariantContainerBase reply = m_dbus_connection->call_sync(POWER_PROFILES_PATH,
																	 IFACE,
																	 Glib::ustring("Get"),
																	 arguments,
																	 nullptr,
																	 DEST,
																	 -1,
																	 Gio::DBus::CallFlags::NONE,
																	 Glib::VariantType("(v)"));

	 Glib::Variant<std::tuple<Glib::ustring>> rply;
	 reply.get_child(rply);

	 std::string active_profile(std::get<0>(rply.get()));
	 return string_to_power_profile(active_profile);
}

DBusManager::POWER_PROFILE DBusManager::string_to_power_profile(const std::string &profile)
{
	 auto ichar_equals = [] (char a, char b) -> bool {
		  return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
	 };

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
		  m_signal_error.emit("Cannot set invalid an power profile.");
		  return false;
	 }

	 Glib::ustring new_profile;

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

	 assert(!new_profile.empty());

	 auto arguments = Glib::Variant<std::tuple<Glib::ustring, Glib::ustring, Glib::Variant<Glib::ustring>>>::create(
		  std::make_tuple(TARGET_IFACE, PROPERTY, Glib::Variant<Glib::ustring>::create(new_profile))
	 );

	 // The set method returns nothing.
	 (void) m_dbus_connection->call_sync(POWER_PROFILES_PATH,
										 IFACE,
										 Glib::ustring("Set"),
										 arguments,
										 nullptr,
										 DEST,
										 -1,
										 Gio::DBus::CallFlags::NONE);

	 m_signal_success.emit(
		  std::format("Asked power-profiles-daemon to change active profile to: {}",
					  std::string(new_profile))
	 );

	 return true;
}
