#include <getopt.h>
#include <gtkmm/application.h>
#include <print>

#include "config.hpp"
#include "main_window.hpp"

int main(int argc, char *argv[])
{
	 struct option long_options[] = {
		  { "version",    no_argument,    0,    'v' },
		  { nullptr,      0,              0,     0 }
	 };

	 opterr = 0;
	 int option = -1;
	 while ((option = getopt_long(argc, argv, "v", long_options, 0)) >= 0) {
		  switch (option)
		  {
		  case 'v':
			   std::println("{} v{}", PROJECT_NAME, PROJECT_VERSION);
			   return 0;
		  default:
			   std::println("Unknown option: {}.", optopt);
			   return -1;
		  }
	 }

	 auto app = Gtk::Application::create(APP_ID);
	 return app->make_window_and_run<MainWindow>(argc, argv);
}
