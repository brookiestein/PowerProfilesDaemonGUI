#include "main_window.hpp"

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

	 auto context = m_title_label.get_pango_context();
	 auto font = context->get_font_description();
	 font.set_weight(Pango::Weight::ULTRABOLD);
	 context->set_font_description(font);

	 m_labels_vbox.append(m_title_label);
	 m_labels_vbox.append(m_explanation_label);
	 m_labels_vbox.set_margin(10);

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

void MainWindow::on_profile_changed()
{

}

void MainWindow::on_quit()
{
	 close();
}
