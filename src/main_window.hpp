#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window
{
	 void on_profile_changed(const Gtk::CheckButton &sender);
	 void on_quit();

public:
	 MainWindow();

private:
	 Gtk::Label m_title_label;
	 Gtk::Label m_explanation_label;

	 Gtk::CheckButton m_power_saver;
	 Gtk::CheckButton m_balanced;
	 Gtk::CheckButton m_performance;

	 Gtk::Box m_master_vbox; // Box having other boxes because a Window can only contain one widget.
	                        // Will have vertical orientation.
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
