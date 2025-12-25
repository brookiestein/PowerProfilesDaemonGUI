#include <gtkmm/application.h>

#include "main_window.hpp"

int main(int argc, char *argv[])
{
	 auto app = Gtk::Application::create("com.github.brookiestein.power-profiles-daemon-gui");
	 return app->make_window_and_run<MainWindow>(argc, argv);
}
