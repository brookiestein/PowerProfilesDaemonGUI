#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>
#include <memory>
#include <string>

#include "dbus_manager.hpp"

class MainWindow : public Gtk::Window
{
	 void on_profile_changed(Gtk::CheckButton *sender);
	 void on_quit();

public:
	 MainWindow(Glib::RefPtr<Gtk::Application> app = nullptr);
	 enum class ALERT_TYPE { INFO = 0, WARNING, ERROR };

private:
	 void activate_current_profile_on_radio_button();
	 void show_alert(ALERT_TYPE type, const std::string &message);
	 void on_success(const std::string &message);
	 void on_error(const std::string &message);

	 // We could've just gotten the pointer with get_application(), and we used to do it actually,
	 // but we try to fetch the active profile from DBusManager::fetch_active_power_profile()
	 // in the constructor which could emit an error signal, and at that time MainWindow
	 // isn't fully created yet, so we can't just get that pointer for sending the error message
	 // to the user through a desktop notification.
	 //
	 // I think it's easier for main() to append the app pointer that's created there in the
	 // make_window_and_run<>() method's parameters, instead of, for example, making a separate
	 // method here for fetching the active power profile.
	 Glib::RefPtr<Gtk::Application> m_app;
	 std::unique_ptr<DBusManager> m_dbus_manager;
	 DBusManager::POWER_PROFILE m_active_profile;

	 Gtk::Label m_title_label;
	 Gtk::Label m_explanation_label;

	 Gtk::CheckButton m_power_saver;
	 Gtk::CheckButton m_balanced;
	 Gtk::CheckButton m_performance;
	 Gtk::CheckButton *m_last_active;
	 Gtk::Box m_master_vbox;
	 Gtk::Box m_labels_vbox;
	 Gtk::Box m_check_buttons_hbox;
	 Gtk::Box m_check_buttons_vbox;
	 Gtk::Separator m_separator;
	 Gtk::Box m_buttons_hbox;
	 Gtk::Button m_quit_button;

	 // When the program starts, we set the currently active power profile
	 // in its corresponding check button. This flag tells on_profile_changed()
	 // not to reapply the same profile.
	 bool m_starting;
};

#endif // MAIN_WINDOW_HPP
