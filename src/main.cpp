#include <getopt.h>
#include <giomm/notification.h>
#include <gtkmm/application.h>
#include <print>
#include <string>

#include "config.hpp"
#include "dbus_manager.hpp"
#include "main_window.hpp"

void usage();
void show_notification(const std::string &message,
					   Glib::RefPtr<Gtk::Application> app);

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
		  manager
			   .signal_error()
			   .connect(sigc::bind(sigc::ptr_fun(show_notification), app));

		  DBusManager::POWER_PROFILE profile(manager.fetch_active_power_profile());

		  std::string message;

		  if (profile != DBusManager::POWER_PROFILE::INVALID) {
			   message = std::format("Active profile: {}", manager.power_profile_to_string(profile));
			   show_notification(message, app);
		  }

		  return !!(profile == DBusManager::POWER_PROFILE::INVALID);
	 }

	 return app->make_window_and_run<MainWindow>(argc, argv, app);
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

void show_notification(const std::string &message, Glib::RefPtr<Gtk::Application> app)
{
	 Glib::RefPtr<Gio::Notification> notification = Gio::Notification::create(PRETTY_NAME);
	 notification->set_priority(Gio::Notification::Priority::NORMAL);
	 notification->set_body(message);
	 app->send_notification(APP_ID, notification);
}
