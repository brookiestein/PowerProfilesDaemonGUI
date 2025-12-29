#include <getopt.h>
#include <giomm/notification.h>
#include <gtkmm/application.h>
#include <print>

#include "config.hpp"
#include "dbus_manager.hpp"
#include "main_window.hpp"

void usage();

int main(int argc, char *argv[])
{
	 struct option long_options[] = {
		  { "active-profile",    no_argument,    0,    'a' },
		  { "help",              no_argument,    0,    'h' },
		  { "version",           no_argument,    0,    'v' },
		  { nullptr,             0,              0,     0 }
	 };

	 opterr = 0;
	 bool active_profile {};
	 int option = -1;
	 while ((option = getopt_long(argc, argv, "ahv", long_options, 0)) >= 0) {
		  switch (option)
		  {
		  case 'a':
			   active_profile = true;
			   break;
		  case 'h':
			   usage();
			   return 0;
		  case 'v':
			   std::println("{} v{}", PROJECT_NAME, PROJECT_VERSION);
			   return 0;
		  default:
			   std::println("Unknown option: {}.", optopt);
			   return -1;
		  }
	 }

	 auto app = Gtk::Application::create(APP_ID);

	 if (active_profile) {
		  if (!app->register_application()) {
			   std::println(
					stderr,
					"Could not acquire application identifier. Is another instance running?"
			   );

			   return 1;
		  }

		  DBusManager manager;
		  DBusManager::POWER_PROFILE profile(manager.fetch_current_power_profile());

		  std::string message;

		  if (profile == DBusManager::POWER_PROFILE::INVALID)
			   message = "Failed to fetch active profile. Is power-profiles-daemon running?";
		  else
			   message = std::format("Active profile: {}", manager.power_profile_to_string(profile));

		  Glib::RefPtr<Gio::Notification> notification = Gio::Notification::create(PRETTY_NAME);
		  notification->set_priority(Gio::Notification::Priority::NORMAL);
		  notification->set_body(message);
		  app->send_notification(APP_ID, notification);

		  return !!(profile == DBusManager::POWER_PROFILE::INVALID);
	 }

	 return app->make_window_and_run<MainWindow>(argc, argv);
}

void usage()
{
	 std::println("{} v{} usage:", PROJECT_NAME, PROJECT_VERSION);
	 std::println();
	 std::println("--active-profile | -a    Print active profile and exit.");
	 std::println("--help           | -h    Print this help and exit.");
	 std::println("--version        | -v    Print this program version and exit.");
	 std::println();
}
