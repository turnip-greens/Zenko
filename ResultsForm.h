#pragma once

namespace Zenko {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class ResultsForm : public System::Windows::Forms::Form
    {
    public:
        ResultsForm(int correct, int total, int bestStreak, bool darkTheme);

    protected:
        ~ResultsForm() { if (components) delete components; }

    private:
        // ── Controls ──────────────────────────────────────
        System::Windows::Forms::Label^ labelTitle;
        System::Windows::Forms::Label^ labelScore;
        System::Windows::Forms::Label^ labelAccuracy;
        System::Windows::Forms::Label^ labelStreak;
        System::Windows::Forms::Label^ labelGrade;
        System::Windows::Forms::Button^ btnHome;
        System::ComponentModel::Container^ components;

        bool isDark;
        bool dragging;
        System::Drawing::Point dragStart;

        // ── Theme ─────────────────────────────────────────
        System::Drawing::Color BgColor() { return isDark ? System::Drawing::Color::FromArgb(15, 10, 10) : System::Drawing::Color::FromArgb(245, 250, 252); }
        System::Drawing::Color BarColor() { return isDark ? System::Drawing::Color::FromArgb(25, 10, 10) : System::Drawing::Color::FromArgb(220, 245, 250); }
        System::Drawing::Color AccentColor() { return isDark ? System::Drawing::Color::FromArgb(200, 50, 50) : System::Drawing::Color::FromArgb(0, 180, 200); }
        System::Drawing::Color TextColor() { return isDark ? System::Drawing::Color::White : System::Drawing::Color::FromArgb(20, 20, 30); }
        System::Drawing::Color SubTextColor() { return isDark ? System::Drawing::Color::FromArgb(180, 130, 130) : System::Drawing::Color::FromArgb(80, 140, 160); }

        // Returns a letter grade and flavour text based on accuracy
        String^ GetGradeText(double accuracy) {
            if (accuracy >= 95) return L"S  Perfect!";
            if (accuracy >= 85) return L"A  Great work!";
            if (accuracy >= 70) return L"B  Good effort!";
            if (accuracy >= 55) return L"C  Keep studying!";
            return L"D  More practice needed!";
        }

        System::Drawing::Color GetGradeColor(double accuracy) {
            if (accuracy >= 95) return System::Drawing::Color::FromArgb(255, 215, 0);   // gold
            if (accuracy >= 85) return System::Drawing::Color::FromArgb(80, 220, 120);  // green
            if (accuracy >= 70) return System::Drawing::Color::FromArgb(100, 180, 255); // blue
            if (accuracy >= 55) return System::Drawing::Color::FromArgb(255, 180, 50);  // orange
            return System::Drawing::Color::FromArgb(220, 80, 80);                       // red
        }

        // ── Events ────────────────────────────────────────
        System::Void btnHome_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);

#pragma region Windows Form Designer generated code
        void InitializeComponent(int correct, int total, int bestStreak) {
            this->labelTitle = gcnew System::Windows::Forms::Label();
            this->labelScore = gcnew System::Windows::Forms::Label();
            this->labelAccuracy = gcnew System::Windows::Forms::Label();
            this->labelStreak = gcnew System::Windows::Forms::Label();
            this->labelGrade = gcnew System::Windows::Forms::Label();
            this->btnHome = gcnew System::Windows::Forms::Button();

            this->SuspendLayout();

            // ── Form ──────────────────────────────────────
            this->ClientSize = System::Drawing::Size(420, 380);
            this->Text = L"Zenko — Results";
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->BackColor = BgColor();

            // ── Title ─────────────────────────────────────
            this->labelTitle->Location = System::Drawing::Point(0, 0);
            this->labelTitle->Size = System::Drawing::Size(420, 40);
            this->labelTitle->Font = gcnew System::Drawing::Font("Yu Gothic UI", 13, System::Drawing::FontStyle::Bold);
            this->labelTitle->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelTitle->Text = L"Session Complete";
            this->labelTitle->BackColor = BarColor();
            this->labelTitle->ForeColor = AccentColor();
            this->labelTitle->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &ResultsForm::titleBar_MouseDown);
            this->labelTitle->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &ResultsForm::titleBar_MouseMove);
            this->labelTitle->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &ResultsForm::titleBar_MouseUp);

            // ── Grade ─────────────────────────────────────
            double accuracy = total > 0 ? (double)correct / total * 100.0 : 0;
            this->labelGrade->Location = System::Drawing::Point(0, 60);
            this->labelGrade->Size = System::Drawing::Size(420, 80);
            this->labelGrade->Font = gcnew System::Drawing::Font("Yu Gothic UI", 36, System::Drawing::FontStyle::Bold);
            this->labelGrade->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelGrade->Text = GetGradeText(accuracy);
            this->labelGrade->ForeColor = GetGradeColor(accuracy);

            // ── Score ─────────────────────────────────────
            this->labelScore->Location = System::Drawing::Point(0, 155);
            this->labelScore->Size = System::Drawing::Size(420, 40);
            this->labelScore->Font = gcnew System::Drawing::Font("Yu Gothic UI", 18, System::Drawing::FontStyle::Bold);
            this->labelScore->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelScore->Text = correct + " / " + total + " correct";
            this->labelScore->ForeColor = TextColor();

            // ── Accuracy ──────────────────────────────────
            this->labelAccuracy->Location = System::Drawing::Point(0, 200);
            this->labelAccuracy->Size = System::Drawing::Size(420, 35);
            this->labelAccuracy->Font = gcnew System::Drawing::Font("Yu Gothic UI", 13);
            this->labelAccuracy->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelAccuracy->Text = Math::Round(accuracy, 1) + "% accuracy";
            this->labelAccuracy->ForeColor = SubTextColor();

            // ── Best streak ───────────────────────────────
            this->labelStreak->Location = System::Drawing::Point(0, 240);
            this->labelStreak->Size = System::Drawing::Size(420, 35);
            this->labelStreak->Font = gcnew System::Drawing::Font("Yu Gothic UI", 13);
            this->labelStreak->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->labelStreak->Text = "Best streak: " + bestStreak;
            this->labelStreak->ForeColor = SubTextColor();

            // ── Home button ───────────────────────────────
            this->btnHome->Location = System::Drawing::Point(110, 310);
            this->btnHome->Size = System::Drawing::Size(200, 45);
            this->btnHome->Text = L"Back to Home";
            this->btnHome->Font = gcnew System::Drawing::Font("Yu Gothic UI", 13, System::Drawing::FontStyle::Bold);
            this->btnHome->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnHome->FlatAppearance->BorderSize = 0;
            this->btnHome->BackColor = AccentColor();
            this->btnHome->ForeColor = System::Drawing::Color::White;
            this->btnHome->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnHome->Click += gcnew System::EventHandler(this, &ResultsForm::btnHome_Click);

            // ── Add controls ──────────────────────────────
            this->Controls->Add(this->labelTitle);
            this->Controls->Add(this->labelGrade);
            this->Controls->Add(this->labelScore);
            this->Controls->Add(this->labelAccuracy);
            this->Controls->Add(this->labelStreak);
            this->Controls->Add(this->btnHome);

            this->ResumeLayout(false);
        }
#pragma endregion
    };
} 