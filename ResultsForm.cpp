#include "ResultsForm.h"

namespace Zenko {

    ResultsForm::ResultsForm(int correct, int total, int bestStreak, bool darkTheme) {
        this->isDark = darkTheme;
        this->dragging = false;
        InitializeComponent(correct, total, bestStreak);
    }

    System::Void ResultsForm::btnHome_Click(System::Object^ sender, System::EventArgs^ e) {
        this->Close();
    }

    System::Void ResultsForm::titleBar_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (e->Button == System::Windows::Forms::MouseButtons::Left) {
            dragging = true;
            dragStart = e->Location;
        }
    }
    System::Void ResultsForm::titleBar_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        if (dragging) {
            System::Drawing::Point cur = System::Windows::Forms::Control::MousePosition;
            this->Location = System::Drawing::Point(cur.X - dragStart.X, cur.Y - dragStart.Y);
        }
    }
    System::Void ResultsForm::titleBar_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        dragging = false;
    }
} 