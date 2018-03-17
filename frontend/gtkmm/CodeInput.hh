
#pragma once

#include "DataCell.hh"
#include <gtkmm/box.h>
#include <gtkmm/textview.h>
#include <gtkmm/separator.h>

namespace cadabra {

	/// \ingroup gtkmm
	///
	/// A text cell editor widget with support for editing Python and LaTeX.
   /// CodeInput is essentially a TextView with some additional i/o logic.

	class CodeInput : public Gtk::VBox {
		public:
			/// Initialise with existing TextBuffer and a pointer to the Datacell
			/// corresponding to this CodeInput widget. CodeInput is not allowed
			/// to modify this DataCell directly, but can read properties from
			/// it (e.g. in order to know when to display a 'busy' indicator).
			/// The scale parameter refers to hdpi scaling.

			CodeInput(DTree::iterator, Glib::RefPtr<Gtk::TextBuffer>, double scale, int font_step, bool highlight);

			/// Initialise with a new TextBuffer (to be created by
			/// CodeInput), filling it with the content of the given
			/// string.

			CodeInput(DTree::iterator, const std::string&, double scale, int font_step, bool highlight);
			
			/// The actual text widget used by CodeInput. 

			class exp_input_tv : public Gtk::TextView {
				public:
					exp_input_tv(DTree::iterator, Glib::RefPtr<Gtk::TextBuffer>, double scale);
					virtual bool on_key_press_event(GdkEventKey*) override;
					virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>&) override;
					virtual bool on_focus_in_event(GdkEventFocus *) override;
					virtual void on_show() override;

					void         shift_enter_pressed();
					
					sigc::signal1<bool, DTree::iterator>                   content_execute;
					sigc::signal2<bool, std::string, DTree::iterator>      content_changed;
					sigc::signal3<bool, std::string, int, DTree::iterator> content_insert;
					sigc::signal3<bool, int, int, DTree::iterator>         content_erase;
					sigc::signal1<bool, DTree::iterator>                   cell_got_focus;

					friend CodeInput;

				private:
					double scale_;
					DTree::iterator datacell;
			};

			/// Set the font size, 0 being default, negative smaller, positive larger.
			
			void set_font_size(int num);

			void enable_python_highlighting();
			void enable_latex_highlighting();
			void disable_highlighting();

			/// Handle mouse buttons.

			bool handle_button_press(GdkEventButton *);

			/// Handle an insert event, which can consist of one or more
			/// inserted characters. This function will just massage that
			/// data and then feed it through to the notebook window class
			/// by emitting a signal on content_insert (done like this to
			/// separate DTree modification from the widget).

			void handle_insert(const Gtk::TextIter& pos, const Glib::ustring& text, int bytes);

			/// Handle an erase event. This function will just massage that
			/// data and then feed it through to the notebook window class
			/// by emitting a signal on content_erase (done like this to
			/// separate DTree modification from the widget).

			void handle_erase(const Gtk::TextIter& start, const Gtk::TextIter& end);

			/// Ensure that the visual representation matches the DTree
			/// cell.

			void update_buffer(); 

			/// Return two strings corresponding to the text before and
			/// after the current cursor position.

			void slice_cell(std::string& before, std::string& after);

			/// We cannot edit the content of the DataCell directly,
			/// because Gtk needs a Gtk::TextBuffer. However, the
			/// CodeInput widgets corresponding to a single DataCell all
			/// share their TextBuffer.

			Glib::RefPtr<Gtk::TextBuffer> buffer;

			exp_input_tv                  edit;

		private:
			void init(int font_step, bool highlight);

			void tag_by_regex(const std::string& regex_str, const std::string& tag);
			void highlight_python();
			void highlight_latex();

			sigc::connection hl_conn; // Connection holding the syntax highlighting signal
	};

}
