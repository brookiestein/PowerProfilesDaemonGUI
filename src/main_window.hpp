#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

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
	 MainWindow();
	 enum class ALERT_TYPE { INFO = 0, WARNING, ERROR };

private:
	 void activate_current_profile_on_radio_button();
	 void show_alert(ALERT_TYPE type, const std::string &message);
	 void on_success(const std::string &message);
	 void on_error(const std::string &message);

	 std::unique_ptr<DBusManager> m_dbus_manager;
	 DBusManager::POWER_PROFILE m_current_profile;

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
