#include "main_window.hpp"

#include <cassert>
#include <format>
#include <pangomm/attrlist.h>
#include <print>

MainWindow::MainWindow()
	 : m_title_label("Please choose one profile")
	 , m_explanation_label("Profile will automatically be applied upon selection")
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
					std::ref(m_power_saver)
			   )
		   );

	 m_balanced
		  .signal_toggled()
		  .connect(
			   sigc::bind(
					sigc::mem_fun(*this, &MainWindow::on_profile_changed),
					std::ref(m_balanced)
			   )
		   );

	 m_performance
		  .signal_toggled()
		  .connect(
			   sigc::bind(
					sigc::mem_fun(*this, &MainWindow::on_profile_changed),
					std::ref(m_performance)
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
	 set_title("Power Profiles Daemon GUI");
}

void MainWindow::on_profile_changed(const Gtk::CheckButton &sender)
{
	 // Don't reapply the currently used power profile.
	 if (m_starting) {
		  m_starting = false;
		  return;
	 }

	 // The toggled signal is emitted twice:
	 // one for the "untoggled" state and another one for the "toggled" state.
	 // We're just interested in the "toggled" one.
	 if (!sender.get_active())
		  return;

	 // FIXME: Is there a better way to know which check button emitted this signal?
	 if (&sender == &m_power_saver) {
		  std::println("Power Saver");
	 } else if (&sender == &m_balanced) {
		  std::println("Balanced");
	 } else if (&sender == &m_performance) {
		  std::println("Performance");
	 } else {
		  assert(0 && "Slot called with an unrecognized check button.");
	 }
}

void MainWindow::on_quit()
{
	 close();
}
