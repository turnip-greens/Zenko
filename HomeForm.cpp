#include "HomeForm.h"
#include "ClassicModeForm.h"

namespace Zenko {

    // ── Load ──────────────────────────────────────────────
    // Runs once when the app starts. Creates the engine,
    // loads the card data and saved SRS progress, then
    // updates the stats display.
    System::Void HomeForm::HomeForm_Load(System::Object^ sender, System::EventArgs^ e) {
        isDark = true;
        dragging = false;

        engine = gcnew CardEngine();
        engine->LoadCSV(CSVPath());
        engine->LoadProgress(ProgressPath());

        ApplyTheme();
        RefreshStats();
    }

    // ── RefreshStats ──────────────────────────────────────
    // Updates the due count and total card labels.
    // Call this after any session finishes so the numbers
    // stay accurate without restarting the app.
    void HomeForm::RefreshStats() {
        int due = engine->DueCount();
        int total = engine->GetAllCards()->Count;

        labelDueCount->Text = due > 0
            ? due + " cards due"
            : "All caught up!";

        labelTotalCards->Text = "Total cards: " + total;

        // Green = nothing to do, red = cards waiting
        labelDueCount->ForeColor = due > 0
            ? (isDark ? Color::FromArgb(220, 80, 80) : Color::FromArgb(180, 30, 30))
            : (isDark ? Color::FromArgb(80, 220, 120) : Color::FromArgb(0, 160, 80));

        btnStudyDue->Enabled = due > 0;
        btnStudyDue->Text = due > 0
            ? "Study Due (" + due + ")"
            : "Nothing Due";
    }

    // ── LaunchSession ─────────────────────────────────────
    // The single entry point for starting any study session.
    // Pass in any List<Card^>^ — due cards, all cards, a
    // future AI-generated set — and this handles the rest.
    // Adding a new mode = add a new button that calls this.
    void HomeForm::LaunchSession(List<Card^>^ cards) {
        if (cards->Count == 0) {
            MessageBox::Show("No cards to study!", "Zenko",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
            return;
        }
         
        auto session = gcnew SessionManager(cards, engine);

        auto study = gcnew ClassicModeForm(session, isDark);
        study->ShowDialog(this);

        // Always save after a session — never lose SRS progress
        engine->SaveProgress(ProgressPath());
        RefreshStats();
    }

    // ── Button handlers ───────────────────────────────────

    System::Void HomeForm::btnStudyDue_Click(System::Object^ sender, System::EventArgs^ e) {
        LaunchSession(engine->GetDueCards());
    }

    System::Void HomeForm::btnStudyAll_Click(System::Object^ sender, System::EventArgs^ e) {
        LaunchSession(engine->GetAllCards());
    }

    System::Void HomeForm::btnClose_Click(System::Object^ sender, System::EventArgs^ e) {
        engine->SaveProgress(ProgressPath());
        Application::Exit();
    }

    System::Void HomeForm::btnTheme_Click(System::Object^ sender, System::EventArgs^ e) {
        isDark = !isDark;
        ApplyTheme();
        RefreshStats();
    }

    // ── Drag (borderless window) ──────────────────────────
    System::Void HomeForm::titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (e->Button == System::Windows::Forms::MouseButtons::Left) {
            dragging = true;
            dragStart = e->Location;
        }
    }
    System::Void HomeForm::titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (dragging) {
            System::Drawing::Point cur = System::Windows::Forms::Control::MousePosition;
            this->Location = System::Drawing::Point(cur.X - dragStart.X, cur.Y - dragStart.Y);
        }
    }
    System::Void HomeForm::titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        dragging = false;
    }

    // ── ApplyTheme ────────────────────────────────────────
    // Applies current dark/light colours to every control.
    // Called on load and whenever the theme button is clicked.
    void HomeForm::ApplyTheme() {
        this->BackColor = BgColor();
        labelTitle->BackColor = BarColor();
        labelTitle->ForeColor = AccentColor();
        btnClose->BackColor = BarColor();
        btnClose->ForeColor = AccentColor();
        btnTheme->BackColor = BarColor();
        btnTheme->ForeColor = SubTextColor();
        btnTheme->Text = isDark ? L"Light" : L"Dark";
        labelTotalCards->ForeColor = SubTextColor();
        labelAccuracy->ForeColor = SubTextColor();
        btnStudyDue->BackColor = AccentColor();
        btnStudyDue->ForeColor = Color::White;
        btnStudyAll->BackColor = CardColor();
        btnStudyAll->ForeColor = TextColor();
        btnStudyAll->FlatAppearance->BorderColor = SubTextColor();
    }

    System::Void HomeForm::btnStudyMatch_Click(System::Object^ sender, System::EventArgs^ e) {
        // Toggle slider visibility
        bool showing = sliderPairs->Visible;
        sliderPairs->Visible = !showing;
        labelPairsCount->Visible = !showing;
        btnStudyMatch->Text = showing ? L"Match Mode" : L"Start Match";

        if (!showing) return; // just opened the slider, wait for Start

        // If already showing, this click means Start
        int pairs = sliderPairs->Value;
        auto cards = engine->GetDueCards();
        if (cards->Count < pairs) cards = engine->GetAllCards();

        auto match = gcnew MatchModeForm(cards, engine, pairs, isDark);
        match->ShowDialog(this);
        engine->SaveProgress(ProgressPath());
        RefreshStats();

        // Reset UI
        sliderPairs->Visible = false;
        labelPairsCount->Visible = false;
        btnStudyMatch->Text = L"Match Mode";
    }

    System::Void HomeForm::sliderPairs_Scroll(System::Object^ sender, System::EventArgs^ e) {
        labelPairsCount->Text = sliderPairs->Value + " pairs";
    }
}