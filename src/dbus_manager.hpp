#ifndef DBUS_MANAGER_HPP
#define DBUS_MANAGER_HPP

#include <giomm/dbusconnection.h>
#include <string>
#include <sigc++/sigc++.h>

class DBusManager
{
public:
	 DBusManager();
	 sigc::signal<void(const std::string &)> &signal_success();
	 sigc::signal<void(const std::string &)> &signal_error();
	 enum class POWER_PROFILE { INVALID = -1, POWER_SAVER = 0, BALANCED, PERFORMANCE };
	 POWER_PROFILE fetch_active_power_profile();
	 std::string power_profile_to_string(POWER_PROFILE profile);
	 bool set_profile(POWER_PROFILE profile);

private:
	 POWER_PROFILE string_to_power_profile(const std::string &profile);

	 sigc::signal<void(const std::string &)> m_signal_success;
	 sigc::signal<void(const std::string &)> m_signal_error;
	 Glib::RefPtr<Gio::DBus::Connection> m_dbus_connection;
	 const Glib::ustring DEST;
	 const Glib::ustring POWER_PROFILES_PATH;
	 const Glib::ustring IFACE; // DBus interface
	 const Glib::ustring TARGET_IFACE; // PowerProfilesDaemon interface.
	 const Glib::ustring PROPERTY;
};

#endif // DBUS_MANAGER_HPP
