#pragma once
#include "CardEngine.h"
#include "SessionManager.h"

namespace Zenko {

    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;
    using namespace System::ComponentModel;

    public ref class HomeForm : public System::Windows::Forms::Form
    {
    public:
        HomeForm(void) { InitializeComponent(); }

    protected:
        ~HomeForm() { if (components) delete components; }

    private:
        // ── Engine ────────────────────────────────────────
        CardEngine^ engine;

        // ── Controls ──────────────────────────────────────
        // Declared here so the designer can see and manage them
        System::Windows::Forms::Label^ labelTitle;
        System::Windows::Forms::Label^ labelDueCount;
        System::Windows::Forms::Label^ labelTotalCards;
        System::Windows::Forms::Label^ labelAccuracy;
        System::Windows::Forms::Button^ btnStudyMatch;
        System::Windows::Forms::TrackBar^ sliderPairs;
        System::Windows::Forms::Label^ labelPairsCount;
        Button^ btnStudyDue;
        Button^ btnStudyAll;
        Button^ btnClose;
        Button^ btnTheme;
        System::ComponentModel::Container^ components;

        // ── State ─────────────────────────────────────────
        bool                   dragging;
        System::Drawing::Point dragStart;
        bool                   isDark;

        // ── Paths ─────────────────────────────────────────
        // Static so any other form can call HomeForm::CSVPath() etc.
        static String^ DataPath() { return "C:\\Zenko\\"; }
        static String^ CSVPath() { return DataPath() + "cards.csv"; }
        static String^ ProgressPath() { return DataPath() + "progress.bin"; }
        static String^ StrokePath() { return DataPath() + "strokes\\"; }

        // ── Theme helpers ─────────────────────────────────
        Color BgColor() { return isDark ? Color::FromArgb(15, 10, 10) : Color::FromArgb(245, 250, 252); }
        Color BarColor() { return isDark ? Color::FromArgb(25, 10, 10) : Color::FromArgb(220, 245, 250); }
        Color AccentColor() { return isDark ? Color::FromArgb(200, 50, 50) : Color::FromArgb(0, 180, 200); }
        Color SubTextColor() { return isDark ? Color::FromArgb(180, 130, 130) : Color::FromArgb(80, 140, 160); }
        Color TextColor() { return isDark ? Color::White : Color::FromArgb(20, 20, 30); }
        Color CardColor() { return isDark ? Color::FromArgb(35, 18, 18) : Color::FromArgb(230, 248, 252); }

        // ── Methods (implemented in HomeForm.cpp) ─────────
        void ApplyTheme();
        void RefreshStats();
        void LaunchSession(List<Card^>^ cards);

        // ── Events (implemented in HomeForm.cpp) ──────────
        System::Void HomeForm_Load(System::Object^ sender, System::EventArgs^ e);
        System::Void btnStudyDue_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnStudyAll_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnClose_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnTheme_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
        System::Void btnStudyMatch_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void sliderPairs_Scroll(System::Object^ sender, System::EventArgs^ e);
         
        // ── Designer region ───────────────────────────────
        // The designer reads and writes ONLY this function.
        // Don't move it to the .cpp or the designer will break.
#pragma region Windows Form Designer generated code
        void InitializeComponent(void) {
            this->labelTitle = gcnew System::Windows::Forms::Label();
            this->labelDueCount = gcnew System::Windows::Forms::Label();
            this->labelTotalCards = gcnew System::Windows::Forms::Label();
            this->labelAccuracy = gcnew System::Windows::Forms::Label();
            this->btnStudyDue = gcnew Button();
            this->btnStudyAll = gcnew Button();
            this->btnClose = gcnew Button();
            this->btnTheme = gcnew Button();

            this->SuspendLayout();

            // ── Form ──────────────────────────────────────
            this->ClientSize = System::Drawing::Size(480, 440);
            this->Text = L"Zenko";
            this->BackColor = Color::FromArgb(15, 10, 10);
            this->ForeColor = Color::White;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;

            // ── Title bar ─────────────────────────────────
            this->labelTitle->Location = System::Drawing::Point(0, 0);
            this->labelTitle->Size = System::Drawing::Size(380, 40);
            this->labelTitle->Font = gcnew System::Drawing::Font("Yu Gothic UI", 14, FontStyle::Bold);
            this->labelTitle->TextAlign = ContentAlignment::MiddleCenter;
            this->labelTitle->Text = L"善好  Zenko";
            this->labelTitle->BackColor = Color::FromArgb(25, 10, 10);
            this->labelTitle->ForeColor = Color::FromArgb(200, 50, 50);
            this->labelTitle->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &HomeForm::titleBar_MouseDown);
            this->labelTitle->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &HomeForm::titleBar_MouseMove);
            this->labelTitle->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &HomeForm::titleBar_MouseUp);

            // ── Theme button ──────────────────────────────
            this->btnTheme->Location = System::Drawing::Point(380, 0);
            this->btnTheme->Size = System::Drawing::Size(60, 40);
            this->btnTheme->Text = L"Light";
            this->btnTheme->Font = gcnew System::Drawing::Font("Yu Gothic UI", 8);
            this->btnTheme->FlatStyle = FlatStyle::Flat;
            this->btnTheme->FlatAppearance->BorderSize = 0;
            this->btnTheme->BackColor = Color::FromArgb(25, 10, 10);
            this->btnTheme->ForeColor = Color::FromArgb(180, 130, 130);
            this->btnTheme->Cursor = Cursors::Hand;
            this->btnTheme->Click += gcnew System::EventHandler(this, &HomeForm::btnTheme_Click);

            // ── Close button ──────────────────────────────
            this->btnClose->Location = System::Drawing::Point(440, 0);
            this->btnClose->Size = System::Drawing::Size(40, 40);
            this->btnClose->Text = L"x";
            this->btnClose->Font = gcnew System::Drawing::Font("Yu Gothic UI", 12, FontStyle::Bold);
            this->btnClose->FlatStyle = FlatStyle::Flat;
            this->btnClose->FlatAppearance->BorderSize = 0;
            this->btnClose->BackColor = Color::FromArgb(25, 10, 10);
            this->btnClose->ForeColor = Color::FromArgb(200, 50, 50);
            this->btnClose->Cursor = Cursors::Hand;
            this->btnClose->Click += gcnew System::EventHandler(this, &HomeForm::btnClose_Click);

            // ── Due count ─────────────────────────────────
            this->labelDueCount->Location = System::Drawing::Point(0, 80);
            this->labelDueCount->Size = System::Drawing::Size(480, 60);
            this->labelDueCount->Font = gcnew System::Drawing::Font("Yu Gothic UI", 28, FontStyle::Bold);
            this->labelDueCount->TextAlign = ContentAlignment::MiddleCenter;
            this->labelDueCount->Text = L"...";

            // ── Total cards ───────────────────────────────
            this->labelTotalCards->Location = System::Drawing::Point(0, 150);
            this->labelTotalCards->Size = System::Drawing::Size(480, 30);
            this->labelTotalCards->Font = gcnew System::Drawing::Font("Yu Gothic UI", 11);
            this->labelTotalCards->TextAlign = ContentAlignment::MiddleCenter;
            this->labelTotalCards->Text = L"";
            this->labelTotalCards->ForeColor = Color::FromArgb(180, 130, 130);

            // ── Accuracy (placeholder for future stats) ───
            this->labelAccuracy->Location = System::Drawing::Point(0, 185);
            this->labelAccuracy->Size = System::Drawing::Size(480, 25);
            this->labelAccuracy->Font = gcnew System::Drawing::Font("Yu Gothic UI", 10);
            this->labelAccuracy->TextAlign = ContentAlignment::MiddleCenter;
            this->labelAccuracy->Text = L"";
            this->labelAccuracy->ForeColor = Color::FromArgb(180, 130, 130);

            // ── Study Due button ──────────────────────────
            this->btnStudyDue->Location = System::Drawing::Point(90, 250);
            this->btnStudyDue->Size = System::Drawing::Size(300, 55);
            this->btnStudyDue->Text = L"Study Due";
            this->btnStudyDue->Font = gcnew System::Drawing::Font("Yu Gothic UI", 14, FontStyle::Bold);
            this->btnStudyDue->FlatStyle = FlatStyle::Flat;
            this->btnStudyDue->FlatAppearance->BorderSize = 0;
            this->btnStudyDue->BackColor = Color::FromArgb(200, 50, 50);
            this->btnStudyDue->ForeColor = Color::White;
            this->btnStudyDue->Cursor = Cursors::Hand;
            this->btnStudyDue->Click += gcnew System::EventHandler(this, &HomeForm::btnStudyDue_Click);

            // ── Study All button ──────────────────────────
            this->btnStudyAll->Location = System::Drawing::Point(90, 318);
            this->btnStudyAll->Size = System::Drawing::Size(300, 40);
            this->btnStudyAll->Text = L"Study All Cards";
            this->btnStudyAll->Font = gcnew System::Drawing::Font("Yu Gothic UI", 11);
            this->btnStudyAll->FlatStyle = FlatStyle::Flat;
            this->btnStudyAll->FlatAppearance->BorderSize = 2;
            this->btnStudyAll->FlatAppearance->BorderColor = Color::FromArgb(120, 30, 30);
            this->btnStudyAll->BackColor = Color::FromArgb(35, 18, 18);
            this->btnStudyAll->ForeColor = Color::White;
            this->btnStudyAll->Cursor = Cursors::Hand;
            this->btnStudyAll->Click += gcnew System::EventHandler(this, &HomeForm::btnStudyAll_Click);

            // ── Match mode slider ─────────────────────────────
            this->sliderPairs->Location = System::Drawing::Point(90, 370);
            this->sliderPairs->Size = System::Drawing::Size(220, 40);
            this->sliderPairs->Minimum = 3;
            this->sliderPairs->Maximum = 7;
            this->sliderPairs->Value = 4;
            this->sliderPairs->TickFrequency = 1;
            this->sliderPairs->Visible = false;
            this->sliderPairs->Scroll += gcnew System::EventHandler(this, &HomeForm::sliderPairs_Scroll);

            this->labelPairsCount->Location = System::Drawing::Point(315, 370);
            this->labelPairsCount->Size = System::Drawing::Size(80, 40);
            this->labelPairsCount->Font = gcnew System::Drawing::Font("Yu Gothic UI", 12, System::Drawing::FontStyle::Bold);
            this->labelPairsCount->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            this->labelPairsCount->Text = L"4 pairs";
            this->labelPairsCount->Visible = false;

            this->btnStudyMatch->Location = System::Drawing::Point(90, 365);
            this->btnStudyMatch->Size = System::Drawing::Size(300, 40);
            this->btnStudyMatch->Text = L"Match Mode";
            this->btnStudyMatch->Font = gcnew System::Drawing::Font("Yu Gothic UI", 11);
            this->btnStudyMatch->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->btnStudyMatch->FlatAppearance->BorderSize = 2;
            this->btnStudyMatch->FlatAppearance->BorderColor = Color::FromArgb(120, 30, 30);
            this->btnStudyMatch->BackColor = System::Drawing::Color::FromArgb(35, 18, 18);
            this->btnStudyMatch->ForeColor = System::Drawing::Color::White;
            this->btnStudyMatch->Cursor = System::Windows::Forms::Cursors::Hand;
            this->btnStudyMatch->Click += gcnew System::EventHandler(this, &HomeForm::btnStudyMatch_Click);

            // ── Add all controls ──────────────────────────
            this->Controls->Add(this->labelTitle);
            this->Controls->Add(this->btnTheme);
            this->Controls->Add(this->btnClose);
            this->Controls->Add(this->labelDueCount);
            this->Controls->Add(this->labelTotalCards);
            this->Controls->Add(this->labelAccuracy);
            this->Controls->Add(this->btnStudyDue);
            this->Controls->Add(this->btnStudyAll);

            this->Load += gcnew System::EventHandler(this, &HomeForm::HomeForm_Load);
            this->ResumeLayout(false);
        }
#pragma endregion
    };
}