#pragma once
#include "SessionManager.h"

namespace Zenko {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class ClassicModeForm : public System::Windows::Forms::Form
    {
    public:
        // Takes the session and the current theme from HomeForm
        ClassicModeForm(SessionManager^ session, bool darkTheme);

    protected:
        ~ClassicModeForm() { if (components) delete components; }

    private:
        // ── Session ───────────────────────────────────────
        SessionManager^ session;
        Question^ currentQuestion;

        // ── Timer ─────────────────────────────────────────
        // Tracks how long the player takes to answer.
        // Used to auto-grade: fast correct = Easy, slow = Correct.
        System::Windows::Forms::Timer^ questionTimer;
        int elapsedSeconds;
        static const int EasyCutoff = 8; // seconds

        // ── Controls ──────────────────────────────────────
        System::Windows::Forms::Label^ labelTitle;
        System::Windows::Forms::Label^ labelStreak;
        System::Windows::Forms::Label^ labelProgress;
        System::Windows::Forms::Label^ labelPrompt;
        System::Windows::Forms::Label^ labelQuestion;
        System::Windows::Forms::Label^ labelFeedback;
        System::Windows::Forms::Label^ labelTimer;
        System::Windows::Forms::Button^ btnNext;
        System::Windows::Forms::Button^ btnClose;
        array<System::Windows::Forms::Button^>^ choiceButtons;
        System::ComponentModel::Container^ components;

        // ── State ─────────────────────────────────────────
        bool                   answered;
        bool                   isDark;
        bool                   dragging;
        System::Drawing::Point dragStart;

        // ── Theme ─────────────────────────────────────────
        System::Drawing::Color BgColor() { return isDark ? System::Drawing::Color::FromArgb(15, 10, 10) : System::Drawing::Color::FromArgb(245, 250, 252); }
        System::Drawing::Color BarColor() { return isDark ? System::Drawing::Color::FromArgb(25, 10, 10) : System::Drawing::Color::FromArgb(220, 245, 250); }
        System::Drawing::Color AccentColor() { return isDark ? System::Drawing::Color::FromArgb(200, 50, 50) : System::Drawing::Color::FromArgb(0, 180, 200); }
        System::Drawing::Color AccentSoft() { return isDark ? System::Drawing::Color::FromArgb(120, 30, 30) : System::Drawing::Color::FromArgb(0, 120, 140); }
        System::Drawing::Color TextColor() { return isDark ? System::Drawing::Color::White : System::Drawing::Color::FromArgb(20, 20, 30); }
        System::Drawing::Color SubTextColor() { return isDark ? System::Drawing::Color::FromArgb(180, 130, 130) : System::Drawing::Color::FromArgb(80, 140, 160); }
        System::Drawing::Color CardColor() { return isDark ? System::Drawing::Color::FromArgb(35, 18, 18) : System::Drawing::Color::FromArgb(230, 248, 252); }
        System::Drawing::Color CorrectBg() { return isDark ? System::Drawing::Color::FromArgb(15, 60, 30) : System::Drawing::Color::FromArgb(200, 245, 220); }
        System::Drawing::Color WrongBg() { return isDark ? System::Drawing::Color::FromArgb(60, 15, 15) : System::Drawing::Color::FromArgb(255, 220, 220); }
        System::Drawing::Color CorrectBtn() { return isDark ? System::Drawing::Color::FromArgb(30, 120, 60) : System::Drawing::Color::FromArgb(100, 210, 150); }
        System::Drawing::Color WrongBtn() { return isDark ? System::Drawing::Color::FromArgb(140, 30, 30) : System::Drawing::Color::FromArgb(220, 100, 100); }

        void ApplyTheme();

        // ── Core logic ────────────────────────────────────
        void ShowNextQuestion();
        void HandleAnswer(String^ given);
        void UpdateHUD();

        // ── Events ────────────────────────────────────────
        System::Void ClassicModeForm_Load(System::Object^ sender, System::EventArgs^ e);
        System::Void choiceButton_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnNext_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnClose_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void questionTimer_Tick(System::Object^ sender, System::EventArgs^ e);
        System::Void titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);

#pragma region Windows Form Designer generated code
        void InitializeComponent(void) {
            this->labelTitle = gcnew System::Windows::Forms::Label();
            this->labelStreak = gcnew System::Windows::Forms::Label();
            this->labelProgress = gcnew System::Windows::Forms::Label();
            this->labelPrompt = gcnew System::Windows::Forms::Label();
            this->labelQuestion = gcnew System::Windows::Forms::Label();
            this->labelFeedback = gcnew System::Windows::Forms::Label();
            this->labelTimer = gcnew System::Windows::Forms::Label();
            this->btnNext = gcnew System::Windows::Forms::Button();
            this->btnClose = gcnew System::Windows::Forms::Button();

            choiceButtons = gcnew array<System::Windows::Forms::Button^>(4);
            for (int i = 0; i < 4; i++)
                choiceButtons[i] = gcnew System::Windows::Forms::Button();

            this->SuspendLayout();

            // ── Form ──────────────────────────────────────
            this->ClientSize = System::Drawing::Size(580, 610);
            this->Text = L"Zenko";
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;

            // ── Title bar ─────────────────────────────────
            this->labelTitle->Location = System::Drawing::Point(0, 0);
            this->labelTitle->Size = System::Drawing::Size(540, 38);
            this->labelTitle->Font = gcnew System::Drawing::Font("Yu Gothic UI", 13, System::Drawing::FontStyle::Bold);
            this->labelTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelTitle->Text = L"Zenko";
            this->labelTitle->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &ClassicModeForm::titleBar_MouseDown);
            this->labelTitle->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &ClassicModeForm::titleBar_MouseMove);
            this->labelTitle->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &ClassicModeForm::titleBar_MouseUp);

            // ── Close ─────────────────────────────────────
            this->btnClose->Location = System::Drawing::Point(540, 0);
            this->btnClose->Size = System::Drawing::Size(40, 38);
            this->btnClose->Text = L"x";
            this->btnClose->Font = gcnew System::Drawing::Font("Yu Gothic UI", 12, System::Drawing::FontStyle::Bold);
            this->btnClose->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnClose->FlatAppearance->BorderSize = 0;
            this->btnClose->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnClose->Click += gcnew System::EventHandler(this, &ClassicModeForm::btnClose_Click);

            // ── HUD ───────────────────────────────────────
            this->labelStreak->Location = System::Drawing::Point(15, 50);
            this->labelStreak->Size = System::Drawing::Size(200, 24);
            this->labelStreak->Font = gcnew System::Drawing::Font("Yu Gothic UI", 10, System::Drawing::FontStyle::Bold);
            this->labelStreak->Text = L"Streak: 0";

            this->labelProgress->Location = System::Drawing::Point(300, 50);
            this->labelProgress->Size = System::Drawing::Size(265, 24);
            this->labelProgress->Font = gcnew System::Drawing::Font("Yu Gothic UI", 10);
            this->labelProgress->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
            this->labelProgress->Text = L"0/0";

            // ── Timer label ───────────────────────────────
            // Shows elapsed seconds so the player knows
            // if they're on track for an Easy grade
            this->labelTimer->Location = System::Drawing::Point(215, 50);
            this->labelTimer->Size = System::Drawing::Size(80, 24);
            this->labelTimer->Font = gcnew System::Drawing::Font("Yu Gothic UI", 10);
            this->labelTimer->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelTimer->Text = L"";

            // ── Prompt ────────────────────────────────────
            this->labelPrompt->Location = System::Drawing::Point(0, 86);
            this->labelPrompt->Size = System::Drawing::Size(580, 30);
            this->labelPrompt->Font = gcnew System::Drawing::Font("Yu Gothic UI", 11, System::Drawing::FontStyle::Italic);
            this->labelPrompt->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelPrompt->Text = L"";

            // ── Question ──────────────────────────────────
            this->labelQuestion->Location = System::Drawing::Point(40, 122);
            this->labelQuestion->Size = System::Drawing::Size(500, 150);
            this->labelQuestion->Font = gcnew System::Drawing::Font("Yu Gothic UI", 52, System::Drawing::FontStyle::Bold);
            this->labelQuestion->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelQuestion->Text = L"";

            // ── Feedback ──────────────────────────────────
            this->labelFeedback->Location = System::Drawing::Point(40, 278);
            this->labelFeedback->Size = System::Drawing::Size(400, 35);
            this->labelFeedback->Font = gcnew System::Drawing::Font("Yu Gothic UI", 12, System::Drawing::FontStyle::Bold);
            this->labelFeedback->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            this->labelFeedback->Text = L"";
            this->labelFeedback->Visible = false;

            // ── Next button ───────────────────────────────
            this->btnNext->Location = System::Drawing::Point(430, 278);
            this->btnNext->Size = System::Drawing::Size(110, 35);
            this->btnNext->Text = L"Next →";
            this->btnNext->Font = gcnew System::Drawing::Font("Yu Gothic UI", 11, System::Drawing::FontStyle::Bold);
            this->btnNext->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnNext->FlatAppearance->BorderSize = 0;
            this->btnNext->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnNext->Visible = false;
            this->btnNext->Click += gcnew System::EventHandler(this, &ClassicModeForm::btnNext_Click);

            // ── Choice buttons 2x2 ────────────────────────
            array<System::Drawing::Point>^ pos = gcnew array<System::Drawing::Point>(4) {
                System::Drawing::Point(40, 325),
                    System::Drawing::Point(300, 325),
                    System::Drawing::Point(40, 415),
                    System::Drawing::Point(300, 415)
            };
            for (int i = 0; i < 4; i++) {
                choiceButtons[i]->Location = pos[i];
                choiceButtons[i]->Size = System::Drawing::Size(240, 74);
                choiceButtons[i]->Font = gcnew System::Drawing::Font("Yu Gothic UI", 12);
                choiceButtons[i]->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
                choiceButtons[i]->FlatAppearance->BorderSize = 2;
                choiceButtons[i]->Cursor = System::Windows::Forms::Cursors::Hand;
                choiceButtons[i]->Click += gcnew System::EventHandler(this, &ClassicModeForm::choiceButton_Click);
                this->Controls->Add(choiceButtons[i]);
            }

            // ── Add controls ──────────────────────────────
            this->Controls->Add(this->labelTitle);
            this->Controls->Add(this->btnClose);
            this->Controls->Add(this->labelStreak);
            this->Controls->Add(this->labelProgress);
            this->Controls->Add(this->labelTimer);
            this->Controls->Add(this->labelPrompt);
            this->Controls->Add(this->labelQuestion);
            this->Controls->Add(this->labelFeedback);
            this->Controls->Add(this->btnNext);

            this->Load += gcnew System::EventHandler(this, &ClassicModeForm::ClassicModeForm_Load);
            this->ResumeLayout(false);
        }
#pragma endregion 
    };
}