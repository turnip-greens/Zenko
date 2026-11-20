#include "ClassicModeForm.h"
#include "ResultsForm.h"

namespace Zenko {

    // ── Constructor ───────────────────────────────────────
    // Receives the session from HomeForm and the current theme.
    // HomeForm owns the engine; this form just uses the session.
    ClassicModeForm::ClassicModeForm(SessionManager^ session, bool darkTheme) {
        this->session = session;
        this->isDark = darkTheme;
        this->answered = false;
        this->dragging = false;
        this->elapsedSeconds = 0;
        InitializeComponent();
    }

    // ── Load ──────────────────────────────────────────────
    System::Void ClassicModeForm::ClassicModeForm_Load(System::Object^ sender, System::EventArgs^ e) {
        // One-second tick timer for grading speed
        questionTimer = gcnew System::Windows::Forms::Timer();
        questionTimer->Interval = 1000;
        questionTimer->Tick += gcnew System::EventHandler(this, &ClassicModeForm::questionTimer_Tick);

        ApplyTheme();
        ShowNextQuestion();
    }

    // ── ShowNextQuestion ──────────────────────────────────
    // Asks the session for the next question and populates
    // all the controls. Also resets the speed timer so
    // grading is always based on this question only.
    void ClassicModeForm::ShowNextQuestion() {
        if (session->IsComplete()) {
            questionTimer->Stop();

            // Hand off to results screen
            auto results = gcnew ResultsForm(
                session->TotalCorrect,
                session->TotalAnswered,
                session->Streak,
                isDark);
            results->ShowDialog(this);
            this->Close();
            return; 
        }

        currentQuestion = session->NextQuestion();
        answered = false;
        elapsedSeconds = 0;

        // Reset background and hide feedback
        this->BackColor = BgColor();
        labelFeedback->Visible = false;
        btnNext->Visible = false;
        labelPrompt->Text = currentQuestion->prompt;
        labelQuestion->Text = currentQuestion->questionText;
        labelTimer->Text = L"";

        // Reset choice buttons
        for (int i = 0; i < 4; i++) {
            if (i < currentQuestion->choices->Count) {
                choiceButtons[i]->Text = currentQuestion->choices[i];
                choiceButtons[i]->BackColor = CardColor();
                choiceButtons[i]->ForeColor = TextColor();
                choiceButtons[i]->FlatAppearance->BorderColor = AccentSoft();
                choiceButtons[i]->Enabled = true;
                choiceButtons[i]->Visible = true;
            }
            else {
                choiceButtons[i]->Visible = false;
            }
        }

        UpdateHUD();
        questionTimer->Start();
    }

    // ── HandleAnswer ──────────────────────────────────────
    // Called when any choice button is clicked.
    // Stops the timer, grades automatically based on
    // correctness and elapsed time, colours the buttons,
    // shows feedback, and reveals the Next button.
    void ClassicModeForm::HandleAnswer(String^ given) {
        if (answered) return;
        answered = true;
        questionTimer->Stop();

        // Auto-grade based on correctness and speed
        // Wrong = 0, Correct slow (>8s) = 3, Correct fast = 5
        Grade grade;
        bool correct = false;

        // Let the session validate and get the grade back
        // We override grade to Easy if they were fast enough
        correct = session->Answer(given, grade);

        if (correct && elapsedSeconds <= EasyCutoff)
            grade = Grade::Easy;

        // Colour correct answer green, wrong answer red
        for (int i = 0; i < 4; i++) {
            if (choiceButtons[i]->Text == currentQuestion->correctAnswer) {
                choiceButtons[i]->BackColor = CorrectBtn();
                choiceButtons[i]->ForeColor = System::Drawing::Color::White;
            }
            else if (choiceButtons[i]->Text == given && !correct) {
                choiceButtons[i]->BackColor = WrongBtn();
                choiceButtons[i]->ForeColor = System::Drawing::Color::White;
            }
            choiceButtons[i]->Enabled = false;
        }

        // Background flash
        this->BackColor = correct ? CorrectBg() : WrongBg();

        // Feedback label
        if (correct) {
            labelFeedback->ForeColor = isDark
                ? System::Drawing::Color::FromArgb(80, 220, 120)
                : System::Drawing::Color::FromArgb(0, 160, 80);
            labelFeedback->Text = elapsedSeconds <= EasyCutoff
                ? "Correct! >>Fast>>"
                : "Correct!";
        }
        else {
            labelFeedback->ForeColor = isDark
                ? System::Drawing::Color::FromArgb(220, 80, 80)
                : System::Drawing::Color::FromArgb(180, 30, 30);
            labelFeedback->Text = "Wrong!  Answer: " + currentQuestion->correctAnswer;
        }

        labelFeedback->Visible = true;
        btnNext->Visible = true;
        UpdateHUD();
    }

    // ── UpdateHUD ─────────────────────────────────────────
    void ClassicModeForm::UpdateHUD() {
        int streak = session->Streak;
        labelStreak->Text = streak >= 5
            ? "Streak: " + streak + " !!!"
            : "Streak: " + streak;

        int correct = session->TotalCorrect;
        int answered = session->TotalAnswered;
        labelProgress->Text = answered > 0
            ? correct + "/" + answered + " (" +
            Math::Round((double)correct / answered * 100, 1) + "%)"
            : "0/0";
    }

    // ── Timer tick ────────────────────────────────────────
    // Fires every second. Updates the elapsed time label so
    // the player can see if they're still in the Easy window.
    System::Void ClassicModeForm::questionTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
        elapsedSeconds++;
        if (elapsedSeconds <= EasyCutoff)
            labelTimer->Text = elapsedSeconds + "s >>fast>>";
        else
            labelTimer->Text = elapsedSeconds + "s";
    }

    // ── Button handlers ───────────────────────────────────

    System::Void ClassicModeForm::choiceButton_Click(System::Object^ sender, System::EventArgs^ e) {
        HandleAnswer(safe_cast<System::Windows::Forms::Button^>(sender)->Text);
    }

    System::Void ClassicModeForm::btnNext_Click(System::Object^ sender, System::EventArgs^ e) {
        ShowNextQuestion();
    }

    System::Void ClassicModeForm::btnClose_Click(System::Object^ sender, System::EventArgs^ e) {
        questionTimer->Stop();
        this->Close();
    }

    // ── Drag ──────────────────────────────────────────────
    System::Void ClassicModeForm::titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (e->Button == System::Windows::Forms::MouseButtons::Left) {
            dragging = true;
            dragStart = e->Location;
        }
    }
    System::Void ClassicModeForm::titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (dragging) {
            System::Drawing::Point cur = System::Windows::Forms::Control::MousePosition;
            this->Location = System::Drawing::Point(cur.X - dragStart.X, cur.Y - dragStart.Y);
        }
    }
    System::Void ClassicModeForm::titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        dragging = false;
    }

    // ── ApplyTheme ────────────────────────────────────────
    void ClassicModeForm::ApplyTheme() {
        this->BackColor = BgColor();
        labelTitle->BackColor = BarColor();
        labelTitle->ForeColor = AccentColor();
        btnClose->BackColor = BarColor();
        btnClose->ForeColor = AccentColor();
        labelStreak->ForeColor = AccentColor();
        labelProgress->ForeColor = isDark
            ? System::Drawing::Color::FromArgb(150, 200, 150)
            : System::Drawing::Color::FromArgb(0, 150, 120);
        labelTimer->ForeColor = isDark
            ? System::Drawing::Color::FromArgb(180, 180, 80)
            : System::Drawing::Color::FromArgb(160, 120, 0);
        labelPrompt->ForeColor = SubTextColor();
        labelQuestion->ForeColor = TextColor();
        btnNext->BackColor = AccentSoft();
        btnNext->ForeColor = System::Drawing::Color::White;

        for (int i = 0; i < 4; i++) {
            choiceButtons[i]->BackColor = CardColor();
            choiceButtons[i]->ForeColor = TextColor();
            choiceButtons[i]->FlatAppearance->BorderColor = AccentSoft();
        }
    }
}