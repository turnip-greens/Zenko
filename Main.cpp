#include "HomeForm.h"

using namespace Zenko;

[STAThread]
int main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew HomeForm());
    return 0;
} 