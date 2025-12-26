#ifndef DBUS_MANAGER_HPP
#define DBUS_MANAGER_HPP

#include <string>
#include <sigc++/sigc++.h>
#include <dbus/dbus.h>

class DBusManager
{
public:
	 DBusManager(sigc::slot<void (const std::string &)> on_error);
	 enum class POWER_PROFILE { INVALID = -1, POWER_SAVER = 0, BALANCED, PERFORMANCE };
	 POWER_PROFILE fetch_current_power_profile();
	 std::string power_profile_to_string(POWER_PROFILE profile);
	 bool set_profile(POWER_PROFILE profile);

private:
	 POWER_PROFILE string_to_power_profile(const std::string &profile);

	 sigc::signal<void(const std::string &)> m_error_signal;
	 const char *DEST;
	 const char *PATH;
	 const char *IFACE; // DBus interface
	 const char *TARGET_IFACE; // PowerProfilesDaemon interface.
	 const char *PROPERTY;
	 DBusConnection *m_dbus_connection;
};

#endif // DBUS_MANAGER_HPP
