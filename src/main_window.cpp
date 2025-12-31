#include "main_window.hpp"
#include "config.hpp"

#include <cassert>
#include <format>
#include <gdkmm/display.h>
#include <gtkmm/icontheme.h>
#include <giomm/notification.h>
#include <gtkmm/application.h>
#include <pangomm/attrlist.h>
#include <print>

MainWindow::MainWindow(Glib::RefPtr<Gtk::Application> app)
	 : m_app(app)
	 , m_title_label("Please choose a profile")
	 , m_explanation_label("Profile will automatically be applied upon selection.")
	 , m_power_saver("Power Saver")
	 , m_balanced("Balanced")
	 , m_performance("Performance")
	 , m_master_vbox(Gtk::Orientation::VERTICAL, 0)
	 , m_labels_vbox(Gtk::Orientation::VERTICAL, 0)
	 , m_check_buttons_hbox(Gtk::Orientation::HORIZONTAL, 0)
	 , m_check_buttons_vbox(Gtk::Orientation::VERTICAL, 0)
	 , m_buttons_hbox(Gtk::Orientation::HORIZONTAL, 0)
	 , m_quit_button("Quit")
{
	 // Constructor can throw an exception if system bus can't be obtained.
	 m_dbus_manager = std::make_unique<DBusManager>();

	 m_dbus_manager
		  ->signal_success()
		  .connect(sigc::mem_fun(*this, &MainWindow::on_success));

	 m_dbus_manager
		  ->signal_error()
		  .connect(sigc::mem_fun(*this, &MainWindow::on_error));

	 m_active_profile = m_dbus_manager->fetch_active_power_profile();
	 if (m_active_profile == DBusManager::POWER_PROFILE::INVALID) {
		  m_power_saver.set_sensitive(false);
		  m_balanced.set_sensitive(false);
		  m_performance.set_sensitive(false);
	 } else {
		  activate_current_profile_on_radio_button();
	 }

	 m_balanced.set_group(m_power_saver);
	 m_performance.set_group(m_power_saver);

	 Pango::AttrList titleAttributes;
	 auto font_weight = Pango::Attribute::create_attr_weight(Pango::Weight::BOLD);
	 auto font_size = Pango::Attribute::create_attr_size(14 * 1024);

	 titleAttributes.insert(font_weight);
	 titleAttributes.insert(font_size);
	 m_title_label.set_attributes(titleAttributes);

	 Pango::AttrList explanationAttributes;
	 auto font_style = Pango::Attribute::create_attr_style(Pango::Style::ITALIC);

	 explanationAttributes.insert(font_style);
	 m_explanation_label.set_attributes(explanationAttributes);

	 m_labels_vbox.append(m_title_label);
	 m_labels_vbox.append(m_explanation_label);
	 m_labels_vbox.set_margin(10);

	 m_power_saver
		  .signal_toggled()
		  .connect(
			   sigc::bind(
					sigc::mem_fun(*this, &MainWindow::on_profile_changed),
					&m_power_saver
			   )
		   );

	 m_balanced
		  .signal_toggled()
		  .connect(
			   sigc::bind(
					sigc::mem_fun(*this, &MainWindow::on_profile_changed),
					&m_balanced
			   )
		   );

	 m_performance
		  .signal_toggled()
		  .connect(
			   sigc::bind(
					sigc::mem_fun(*this, &MainWindow::on_profile_changed),
					&m_performance
			   )
		   );

	 m_check_buttons_vbox.append(m_power_saver);
	 m_check_buttons_vbox.append(m_balanced);
	 m_check_buttons_vbox.append(m_performance);

	 m_check_buttons_hbox.set_margin(10);
	 m_check_buttons_hbox.append(m_check_buttons_vbox);
	 m_check_buttons_vbox.set_hexpand(true);
	 m_check_buttons_vbox.set_halign(Gtk::Align::CENTER);

	 m_buttons_hbox.append(m_quit_button);
	 m_quit_button.set_hexpand(true);
	 m_quit_button.set_halign(Gtk::Align::END);
	 m_quit_button.set_margin(10);
	 m_quit_button.set_tooltip_text("Immediately closes the application.");
	 m_quit_button
		  .signal_clicked()
		  .connect(sigc::mem_fun(*this, &MainWindow::on_quit));

	 m_master_vbox.append(m_labels_vbox);
	 m_master_vbox.append(m_check_buttons_hbox);
	 m_master_vbox.append(m_separator);
	 m_master_vbox.append(m_buttons_hbox);
	 m_master_vbox.set_vexpand();
	 m_master_vbox.set_valign(Gtk::Align::CENTER);

	 set_child(m_master_vbox);
	 set_title(PRETTY_NAME);

	 Glib::RefPtr<Gdk::Display> display = get_display();
	 Glib::RefPtr<Gtk::IconTheme> icon_theme = Gtk::IconTheme::get_for_display(display);

	 icon_theme->add_resource_path("com/github/brookiestein/powerprofilesdaemongui/icons");
	 set_icon_name("icon");
}

void MainWindow::activate_current_profile_on_radio_button()
{
	 switch (m_active_profile)
	 {
	 case DBusManager::POWER_PROFILE::INVALID:
		  break;
	 case DBusManager::POWER_PROFILE::POWER_SAVER:
		  m_power_saver.set_active();
		  m_last_active = &m_power_saver;
		  break;
	 case DBusManager::POWER_PROFILE::BALANCED:
		  m_balanced.set_active();
		  m_last_active = &m_balanced;
		  break;
	 case DBusManager::POWER_PROFILE::PERFORMANCE:
		  m_performance.set_active();
		  m_last_active = &m_performance;
		  break;
	 }
}

void MainWindow::show_alert(MainWindow::ALERT_TYPE type, const std::string &message)
{
	 // Fallback to the get_application() method in case main() didn't send the app pointer.
	 // I guess we'll get a segfault here if MainWindow isn't fully created at the time this
	 // method gets called which just happens if DBusManager::fetch_active_power_profile()
	 // fails to fetch the profile because power-profiles-daemon isn't running in the
	 // user's machine, for example.
	 //
	 // The condition should never be true, though, because I made sure main() sends the ptr.
	 // Take that into account if trying to modify main()'s behavior, though.
	 if (!m_app)
		  m_app = get_application();

	 Glib::RefPtr<Gio::Notification> notification = Gio::Notification::create(PRETTY_NAME);

	 Gio::Notification::Priority priority;

	 switch (type)
	 {
	 case ALERT_TYPE::INFO:
		  priority = Gio::Notification::Priority::NORMAL;
		  break;
	 case ALERT_TYPE::WARNING:
		  priority = Gio::Notification::Priority::HIGH;
		  break;
	 case ALERT_TYPE::ERROR:
		  priority = Gio::Notification::Priority::URGENT;
		  break;
	 }

	 notification->set_priority(priority);
	 notification->set_body(message);
	 m_app->send_notification(APP_ID, notification);
}

void MainWindow::on_success(const std::string &message)
{
	 show_alert(ALERT_TYPE::INFO, message);
}

void MainWindow::on_error(const std::string &message)
{
	 show_alert(ALERT_TYPE::ERROR, message);
}

void MainWindow::on_profile_changed(Gtk::CheckButton *sender)
{
	 // Don't reapply the currently used power profile.
	 if (m_starting) {
		  m_starting = false;
		  return;
	 }

	 assert(sender && "Need to know which profile the user wants to activate.");

	 // The toggled signal is emitted twice:
	 // one for the "untoggled" state and another one for the "toggled" state.
	 // We're just interested in the "toggled" one.
	 if (!sender->get_active())
		  return;

	 DBusManager::POWER_PROFILE profile = DBusManager::POWER_PROFILE::INVALID;
	 if (sender == &m_power_saver) {
		  profile = DBusManager::POWER_PROFILE::POWER_SAVER;
	 } else if (sender == &m_balanced) {
		  profile = DBusManager::POWER_PROFILE::BALANCED;
	 } else if (sender == &m_performance) {
		  profile = DBusManager::POWER_PROFILE::PERFORMANCE;
	 } else {
		  assert(0 && "Slot called with an unrecognized check button.");
	 }

	 if (!m_dbus_manager->set_profile(profile)) {
		  m_last_active->set_active();
		  show_alert(
			   ALERT_TYPE::ERROR,
			   std::format("Failed to set {} as the current power profile.", std::string(sender->get_label()))
		  );
	 } else {
		  m_last_active = sender;
	 }
}

void MainWindow::on_quit()
{
	 close();
}
